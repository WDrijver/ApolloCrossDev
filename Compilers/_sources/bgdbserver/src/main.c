/*
main.c - entry point of bgdbserver
Copyright (C) 2018-2019 by Stefan "Bebbo" Franke <stefan@franke.ms> 

This file is part of bgdbserver.

bgdbserver is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

bgdbserver is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with bgdbserver.  If not, see <http://www.gnu.org/licenses/>.
*/  
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#include <clib/alib_protos.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <dos/dostags.h>
#include <exec/execbase.h>

#include <inline/bsdsocket.h>


#include "common.h"
#include "breakpoint.h"

void unload(void);
void load(char const * progname, char const * clargs);

struct Library * SocketBase;

// the message port of the debugger.
struct MsgPort * dport;
struct PcMessage msg;
LONG trap;

// the loaded segments, if any
static BPTR seglist;
static char * codeseg;
static char * dataseg;
static char * bssseg;

static struct Process * theProc;

#define E_EOS 1
#define E_BUFFER_TO_SMALL 2

#define BUFFERSIZE 2050
static char buffer[BUFFERSIZE + 2];

/**
 * Cleanup function.
 */
void cleanup(void) {
	Printf((UBYTE*)"*** CLEANUP ***\n");
	if (dport) {
		DeleteMsgPort(dport);
		dport = 0;
	}
	if (SocketBase)
		CloseLibrary(SocketBase);
}

/**
 * Allocate what's necessary.
 */
void setup(void) {
	atexit(cleanup);
	dport = CreatePort(0, 0);
	if (!dport) {
		puts("FATAL: no message port");
		exit(1);
	}
	SocketBase = OpenLibrary((UBYTE*)"bsdsocket.library", 0);
	if (!SocketBase) {
		puts("FATAL: could not open bsdsocket.library!");
		exit(1);
	}

	dprintf("port %lx, sig lib %lx\n", dport, SocketBase);

    struct TagItem list[] = {
      { SBTM_SETVAL(SBTC_BREAKMASK),             SIGBREAKF_CTRL_C },
      { TAG_END,TAG_END }
    };
    /* I will assume this always is successful */
    SocketBaseTagList(list);
}

/**
 * Read a line until 0xa.
 */
int readline(int fd) {
	for (int pos = 0; pos < BUFFERSIZE;) {
		int n = recv(fd, buffer + pos, BUFFERSIZE - pos, 0);
		if (n <= 0)
			return E_EOS;
		pos += n;

		buffer[pos] = 0;
		char * p = strchr(buffer, 10);
		if (p) {
			*p = 0;
			dprintf("readline: %s\n", buffer);
			return 0;
		}
	}
	return E_BUFFER_TO_SMALL;
}

/**
 * Read a command until #xy
 */
int readcmd(int fd) {
	for (int pos = 0; pos < BUFFERSIZE;) {
		int n = recv(fd, buffer + pos, BUFFERSIZE - pos, 0);
		if (n <= 0)
			return -1;
		pos += n;


		char *p;
		if (pos > 3)
			p = buffer + pos - 3;
		else
			p = 0;

		if (p && *p == '#') {
			p[3] = 0;
			dprintf("::<%s>\n", buffer);
			return pos;
		}

		buffer[pos] = 0;
		dprintf("%lx %ld:<%s>\n", (int)(buffer[0]), pos, buffer);

		// CTRL+C ends up here
		if (pos == 1 && buffer[0] == 3) {
			return pos;
		}
	}
	return -2;
}


/**
 * Convert the <len> hex digits into the value.
 */
unsigned hex2n(char * p, short len) {
	unsigned n = 0;
	while (--len >= 0) {
		unsigned char c = *p++;
		if (!c)
			break;

		n <<= 4;
		if (c > '9') {
			c |= 0x20;
			n += c - 'a' + 10;
		} else {
			n += c - '0';
		}
	}
	return n;
}

/**
 * Convert the the value n int <len> hex digits.
 */
void n2hex(int n, char * p, short len) {
	p += len;
	*p = 0;
	while (--len >= 0) {
		char c = n & 0xf;
		n >>= 4;
		if (c > 9)
			c += 'a' - 10;
		else
			c += '0';
		*--p = c;
	}
}

/**
 * True if string starts with part.
 */
short startswith(char const * string, char const * part) {
    while(*part) {
        if(*part++ != *string++)
            return 0;
    }

    return 1;
}

/**
 * Send a reply to gdb.
 *
 * Calculates the check sum and does the 'formatting'.
 */
short reply(int sockfd, char const * q) {
	static char buffer[1030];
	if (strlen(q) > 1024)
		return -1;
	dprintf("<- %s\n", q);
	buffer[0] = '$';
	char * p = buffer + 1;
	short sum = 0;
	while (*q) {
		sum += *p++ = *q++;
	}
	*p++ = '#';
	n2hex(sum, p, 2);
	p += 2;

	int n = send(sockfd, buffer, p - buffer, 0);

	*p = 0;
//	dprintf("<- %s\n", buffer);

	return n;
}

/**
 * Find the PC starting from the saved SP.
 */
UWORD * findPC(APTR in) {
	UWORD * sp = (UWORD *) in;
	UWORD attn = SysBase->AttnFlags;

//	printf("start at sp=%p  attn=%04x\n", sp, attn);
//
//	for (int i = 0; i < 16; ++i)
//		printf("%04x ", sp[i]);
//	printf("\n");

	if (attn > 1) {
		// 68020+
		unsigned chk = *(UBYTE*)sp;
//		printf("ch = %02x\n", chk);
		if (chk) {
			// FPU was active
			sp += 1 + 3 * 2 + 8 * 6 + 1; // ctrl word, FPCR/FPSR/FPIAR, FP0-FP7, ???
			if (chk == 0x90) {
				// mid insn
				sp += 3*2; // mid insn frame
			}

			UWORD v = * sp;
			if (v & 0xff00)
				sp += (v & 0xff) / 2;

		}
		sp += 2; // FRESTORE
	}

	// 68000 and 68010
	UWORD ** pcp = (UWORD **)sp;
//	printf("pc=%p at %p\n", *pcp, sp);
	return *pcp;

}

/**
 * Let the client continue.
 */
void messageSlave(enum action myaction) {
	newpc = reginfo.pc;
	action = myaction;
	ReplyMsg(&msg.msg);
}

void rungdb(int sockfd, char const * prg) {
	ULONG portMask = (1 << dport->mp_SigBit);

	dprintf("sock=%ld\n", sockfd);

	fd_set readfds;

	for (;;) {
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);

		ULONG signales = SIGBREAKF_CTRL_C | portMask;
		WaitSelect(sockfd + 1, &readfds, NULL, NULL, 0, &signales);

//		dprintf("signaled %lx\n", signaled);

		if (signales & SIGBREAKF_CTRL_C)
			break;

		// is there a message from the debugged process?
		if (signales & portMask) {
			if (GetMsg(dport)) {
				dprintf("GOT MSG pc=%lX\n", reginfo.pc);

				if (action == STEP_BEFORE_CONTINUE) {
					enableBreakpoints();
					messageSlave(CONTINUE);
				} else {
					disableBreakpoints();

					// still running
					if (reginfo.pc) {
						sprintf(buffer, "T051:%08x;", reginfo.pc);
						reply(sockfd, buffer);
					} else {
						// is in endProc -> unload seglist
						messageSlave(PARK);
						unload();
						reply(sockfd, "X09");
						break;
					}
				}
			}
		}

		if (!FD_ISSET(sockfd, &readfds))
			continue;

//		dprintf("reading\n");
		int n = readcmd(sockfd);
		if (n <= 0)
			break;

		dprintf("-->%s<--\n", buffer);

		// handle CTRL+C
		if (n == 1 && buffer[0] == 3) {
			dprintf("CTRL + C received %08lx sp=%08lx, %x\n", theProc, theProc->pr_Task.tc_SPReg, offsetof(struct Task, tc_SPReg));
			if (!theProc) {
				reply(sockfd, "E 01");
				continue;
			}

			Printf("CTRL + C received\n");
			do {
				Forbid();
				APTR cursp = theProc->pr_Task.tc_SPReg;
				int chk = *(UBYTE*)cursp;
				volatile UWORD * pc = findPC(cursp);

				// check that the pc is somewhere inside the loaded segments
				short ok = 0;
				for (ULONG * s = (ULONG *) BADDR(seglist); s; s = (ULONG *) BADDR(*s)) {
					if ((ULONG *)pc >= s && (ULONG *)pc < s + *(s - 1) - 8) {
						ok = 1;
						break;
					}
				}

				if (ok) {
					// try to insert a break point at pc
					disableBreakpoints();
					addBreakpoint(pc, 1, 0);
					enableBreakpoints();

					Permit();
					reply(sockfd, "OK");
					Printf("break at 0x%08lx\n", pc);
					continue;
				}
				Permit();
				Printf("can't break at 0x%08lx - %02lx\n", pc, chk);
				Delay(1);
			} while(SetSignal(0L,0L) == 0);
			continue;
		}

		short start = 0;
		while (start < n && buffer[start] != '$')
			++start;

		short sum = 0;
		for (short i = n - 4; i > start; --i) {
			sum += buffer[i];
		}

		sum &= 0xff;

		short check = hex2n(buffer + n - 2, 2);

// check packet
		if (n < 4 || buffer[start] != '$' || buffer[n - 3] != '#'
				|| check != sum) {
			send(sockfd, "-", 1, 0);
			continue;
		}

//		dprintf(":: checksum ok\n");
		send(sockfd, "+", 1, 0);

		char * cmd = buffer + start + 1;
		buffer[n - 3] = 0;

		dprintf("-> %s\n", cmd);

		if (startswith(cmd, "?")) {
			sprintf(buffer, "T051:%08x;", reginfo.pc);
			reply(sockfd, buffer);
			continue;
		}

		if (startswith(cmd, "qSupported")) {
			reply(sockfd, "qSupported:swbreak+;multiprocess-;vCont+");
			continue;
		}
		if (startswith(cmd, "vMustReplyEmpty")) {
			reply(sockfd, "");
			continue;
		}
		if (startswith(cmd, "H")) {
			reply(sockfd, "OK");
			continue;
		}

		if (startswith(cmd, "qTStatus")) {
			//reply(sockfd, "Trunning;tnotrun:0");
			reply(sockfd, "");
			continue;
		}

		if (startswith(cmd, "qfThreadInfo")) {
			reply(sockfd, "m1");
			continue;
		}
		if (startswith(cmd, "qsThreadInfo")) {
			reply(sockfd, "l");
			continue;
		}
		if (startswith(cmd, "qThreadExtraInfo")) {
			reply(sockfd, "6D61696E"); // main as hex
			continue;
		}

		if (startswith(cmd, "qAttached")) {
			reply(sockfd, "0");
			continue;
		}

		if (startswith(cmd, "qC")) {
//			reply(sockfd, "QC1");
			reply(sockfd, "");
			continue;
		}

		if (startswith(cmd, "qOffsets")) {
			if (bssseg)
				sprintf(buffer, "Text=%08x;Data=%08x;Bss=%08x", codeseg, dataseg,
						bssseg);
			else if (dataseg)
				sprintf(buffer, "Text=%08x;Data=%08x;Bss=%08x", codeseg, dataseg,
						dataseg);
			else
				sprintf(buffer, "Text=%08x;Data=%08x;Bss=%08x", codeseg, 0, 0);

			reply(sockfd, buffer);
			continue;
		}

		if (startswith(cmd, "qTfV")) {
			reply(sockfd, "");
			continue;
		}

		// read all registers 16?
		if (startswith(cmd, "g")) {
			sprintf(buffer,
					"%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x",
					reginfo.regs[0], reginfo.regs[1], reginfo.regs[2],
					reginfo.regs[3], reginfo.regs[4], reginfo.regs[5],
					reginfo.regs[6], reginfo.regs[7], reginfo.regs[8],
					reginfo.regs[9], reginfo.regs[10], reginfo.regs[11],
					reginfo.regs[12], reginfo.regs[13], reginfo.regs[14],
					reginfo.usp);

			reply(sockfd, buffer);
			continue;
		}

		// specific register, 11 == pc
		if (startswith(cmd, "p")) {
			sprintf(buffer, "%08x", reginfo.pc);
			reply(sockfd, buffer);
			continue;
		}

		// target wants symbols?
		if (startswith(cmd, "qSymbol")) {
			reply(sockfd, "OK");
			continue;
		}

		// read memory
		if (startswith(cmd, "m")) {
			unsigned char * address = 0;
			int len = 0;
			sscanf(cmd + 1, "%08x,%08x", &address, &len);
			char * p = buffer;
			for (int i = 0; i < len; ++i) {
				sprintf(p, "%02x", *address);
				++address;
				p += 2;
			}
			*p = 0;
			reply(sockfd, buffer);
			continue;
		}

		// supported vCont commands
		if (startswith(cmd, "vCont?")) {
			reply(sockfd, "vCont;cst");
			continue;
		}

		// set sw breakpoints
		if (startswith(cmd, "Z")) {
			// Z0,address,kind
			int no = 0;
			UWORD * addr = 0;
			int kind = 0; // unused atm
			sscanf(cmd + 1, "%d,%08x,%d", &no, &addr, &kind);

			addBreakpoint(addr, 0, 0);

			reply(sockfd, "OK");
			continue;
		}

		// clear sw breakpoints
		if (startswith(cmd, "z")) {
			// Z0,address,kind
			int no = 0;
			UWORD * addr = 0;
			int kind = 0; // unused atm
			sscanf(cmd + 1, "%d,%08x,%d", &no, &addr, &kind);

			delBreakpoint(addr);

			reply(sockfd, "OK");
			continue;
		}

		// continue
		if (startswith(cmd, "c")) {
			messageSlave(STEP_BEFORE_CONTINUE);
			continue;
		}

		if (startswith(cmd, "vKill")) {
			reginfo.pc = (UWORD *)endProc;
			messageSlave(CONTINUE);
			continue;
		}

		// step
		if (startswith(cmd, "s")) {
			messageSlave(STEP);
			continue;
		}

		// stop
		if (startswith(cmd, "T")) {
			// thread alive
			if (seglist)
				reply(sockfd, "OK");
			else
				reply(sockfd, "E 01");
			continue;
		}

		if (startswith(cmd, "X")) {
			char * p = 0;
			int len = 0;
			char * q = strchr(cmd, ':');
			if (*q) {
				*q = 0;
				sscanf(cmd + 1, "%x,%d", &p, &len);
			}
			if (p) {
				++q;
				while (len-- > 0) {
					char c = *q++;
					if (c == 0x7d)
						c = *q++ ^ 0x20;
					dprintf("%02x", (short)c);
					*p++ = c;
				}
				dprintf("\n");
				reply(sockfd, "OK");
			} else
				reply(sockfd, "E 01");
			continue;
		}

		Printf("unknown packet %s\n", cmd);
		reply(sockfd, "E 42");

		if (n < 0) {
			puts("ERROR writing to socket");
			break;
		}
	}
}

/**
 * Open a socket on the given port.
 */
int openSocket(int portno, struct sockaddr_in * serv_addr) {
	/* First call to socket() function */
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		Printf("ERROR opening socket\n");
		return -1;
	}

	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		Printf("setsockopt(SO_REUSEADDR) failed\n");
	}

	/* Initialize socket structure */
	bzero((char *) serv_addr, sizeof(struct sockaddr_in));

	dprintf("create server socket %ld for port: %ld\n", sockfd, portno);

	serv_addr->sin_family = AF_INET;
	serv_addr->sin_addr.s_addr = INADDR_ANY;
	serv_addr->sin_port = htons(portno);

	/* Now bind the host address using bind() call.*/
	if (bind(sockfd, (struct sockaddr *) serv_addr, sizeof(struct sockaddr_in))
			< 0) {
		Printf("ERROR on binding\n");
		shutdown(sockfd, 0);
		CloseSocket(sockfd);
		return -1;
	}
	dprintf("bound server socket %ld for port: %ld\n", sockfd, portno);
	listen(sockfd, 5);
	return sockfd;
}

int wait4Connect(int sockfd, struct sockaddr_in * cli_addr) {
	dprintf("waiting for connection on %ld\n", sockfd);
	socklen_t clilen = sizeof(struct sockaddr_in);
	bzero(cli_addr, clilen);
	int newsockfd = accept(sockfd, (struct sockaddr *) cli_addr, &clilen);
	dprintf("got socket %ld\n", newsockfd);
	return newsockfd;
}

/**
 * Run gdbserver on the given port.
 */
int runGdbServer(char * p, int fd) {
	Printf("Running GDB server on port %s\n", p + 1);
	while (*p && *p != ':')
		++p;

	if (!p)
		return 3;

	char * cmd = ++p;
	while (*cmd >= '0' && *cmd <= '9')
		++cmd;

	if (*cmd)
		*cmd++ = 0;

	int gdbport = atoi(p);

	while (*cmd && *cmd <= 32)
		++cmd;


	// find end
	p = cmd;
	while (*p)
		++p;

	// find last ;
	while (p > cmd && *p != ';')
		--p;

	if (p > cmd)
		*p = 0;

	p = cmd;
	while (*p && *p > 32)
		++p;

	if (*p) *p++ = 0;
	char const * clargs =  p;

	dprintf("port = %ld, cmd = %s\n", gdbport, cmd);

	struct sockaddr_in serv_addr;
	int gdbfd = openSocket(gdbport, &serv_addr);
	dprintf("socket =%ld\n", gdbfd);
	if (gdbfd < 0)
		return 4;

	if (fd > 0) {
		load(cmd, clargs);
		if (seglist) {
			sprintf(buffer, "Process %s created; pid = 1\n", cmd);
			send(fd, buffer, strlen(buffer), 0);
			sprintf(buffer, "Listening on port %d\n", gdbport);
			send(fd, buffer, strlen(buffer), 0);
		} else {
			sprintf(buffer, "Failed to load: %s\n", cmd);
			send(fd, buffer, strlen(buffer), 0);
		}
	}

	if (seglist) {
		struct sockaddr_in cli;
		int newsockfd = wait4Connect(gdbfd, &cli);
		if (newsockfd >= 0) {
			rungdb(newsockfd, cmd);
			dprintf("shutdown %ld\n", newsockfd);
			shutdown(newsockfd, 0);
			CloseSocket(newsockfd);
		}
	}
	dprintf("shutdown %ld\n", gdbfd);
	shutdown(gdbfd, 0);
	CloseSocket(gdbfd);

	return 0;
}

/**
 * Open a port on 514 and wait for the gdbserver command
 */
int runRshFake(char const * sport) {
	int err = 1;
	int port = atoi(sport + 1);

	Printf("Starting RSH server on port %s\n", sport + 1);

	// open rsh port
	struct sockaddr_in serv_addr;
	int rshfd = openSocket(port, &serv_addr);
	if (rshfd >= 0) {
		do {
			struct sockaddr_in cli;
			int newsockfd = wait4Connect(rshfd, &cli);
			if (newsockfd < 0)
				break;

			err = readline(newsockfd);
			if (!err && 0 == strncmp("gdbserver ", buffer, 10))
				err = runGdbServer(buffer + 10, newsockfd);

			shutdown(newsockfd, 0);
			CloseSocket(newsockfd);
		} while (err >= 0);
		shutdown(rshfd, 0);
		CloseSocket(rshfd);
	} else {
		puts("ERROR: could not open socket on port 514\n");
		err = 1;
	}

	// negative means clean exit
	if (err < 0)
		err = 0;
	return err;

}

/**
 * Show some usage info.
 */
int showUsage() {
	printf("USAGE:\n"
			"\tbgdbserver [:<port=514>]\n"
			"\t\topen rsh at <port> and wait for a gdbserver command line\n"
			"\t\tthen load the program into the bgdbserver\n"
			"\tbgdbserver [:<port=2345>] <program> [arguments...] \n"
			"\t\tload the given program into the bgdbserver and listen on port <port>\n");
	return 0;
}

/**
 * Unload everything.
 */
void unload(void) {
	clearBreakpoints();
	if (theProc)
		Signal((struct Task *)theProc, SIGBREAKB_CTRL_C);
	if (seglist) {
		UnLoadSeg(seglist);
		seglist = 0;
		puts("unloaded program");
	}

	codeseg = dataseg = bssseg = 0;
	theProc = 0;
}

struct MyHunk {
	LONG size;
	BPTR next;
	UWORD jmp;
	ULONG offset;
} dummyhunk;

/**
 * Load a program.
 */
void load(char const * progname, char const * clargs) {
	unload();
	seglist = (BPTR) LoadSeg((UBYTE * )progname);

	if (!seglist) {
		printf("failed to load program %s\n", progname);
		return;
	}

	printf("loaded program %s -> %p\n", progname, seglist);

	int n = 0;
	for (ULONG * s = (ULONG *) BADDR(seglist); s; s = (ULONG *) BADDR(*s)) {
		printf("hunk at: %p sz=%08x\n", s, *(s - 1) - 8);

		if (n == 0)
			codeseg = (char *) (s + 1);
		else if (n == 1)
			dataseg = (char *) (s + 1);
		else if (n == 2)
			bssseg = (char *) (s + 1);

		++n;
	}

	// init the message port
	msg.msg.mn_Node.ln_Type = NT_MESSAGE;
	msg.msg.mn_Length = sizeof(struct PcMessage);
	reginfo.pc = (UWORD *) codeseg;

	BPTR stdio = Open((UBYTE* )"*", MODE_READWRITE);
	dprintf("con %lx\n", stdio);
	theProc = CreateNewProcTags(NP_Entry, (ULONG )startProc,
			NP_Input, (ULONG)stdio,
			NP_Output, (ULONG )stdio,
			NP_Arguments, (ULONG)clargs,
			NP_FreeSeglist, 0,
			NP_StackSize, 100000,
			NP_Cli, 1,
			NP_Name, (ULONG )progname,
			TAG_END);
	dprintf("proc = %08lx\n", theProc);
}

/**
 * Bebbo's Debug Server.
 *
 * provide a dumy rsh server which waits for
 *  "gdbserver :port <program>"
 * then open the desired port and act like a gdbserver.
 */
int main(int argc, char *argv[]) {
	char * sport = 0;

	Printf("bgdbserver 1.3 (c) by Stefan 'Bebbo' Franke 2018-2019\n");

	setup();
	atexit(unload);

	int argx = 1;
	if (argx < argc && argv[argx][0] == ':') {
		sport = argv[argx];
		++argx;
	}
	if (argx < argc && argv[argx][0] == '-') {
		if (argv[argx][1] == 'r')
			puts("-r is deprecated");
		else {
			showUsage();
			return 0;
		}
		++argx;
	}

	if (argx == argc)
		return runRshFake(sport ? sport : ":514" );

	// load the given program and run the gdbserver on given port
	struct Process * p = (struct Process *)FindTask(0);
	char const * clargs = (char *)p->pr_Arguments;

	for (int i = 1; i <= argx; ++i) {
		clargs = strstr(clargs, argv[i]);
		clargs += strlen(argv[i]);
		while (*clargs > ' ')
			++clargs;
	}

	load(argv[argx], clargs);
	return runGdbServer(sport ? sport : ":2345" , 0);
}
