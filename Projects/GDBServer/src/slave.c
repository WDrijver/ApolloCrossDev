/*
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


//m68k-amigaos-gcc -Os -fomit-frame-pointer slave.c

#include <string.h>
#include <clib/alib_protos.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <inline/dos.h>
#include <inline/exec.h>
#include "common.h"

// debugger's message port
extern struct MsgPort * dport;

// client's message port
struct MsgPort * replyport;

enum action action;
UWORD * newpc;


// the register values
struct RegInfo reginfo;
ULONG startSp;
ULONG upperSp;

UWORD insn[2] =  {
		0x4e40,
		0x4e75 // rts
};
void (*calltrap)(void) = (void (*)(void))(&insn);

// the used trap
WORD trap;
APTR oldTrapCode;

void park(void);
__interrupt __entrypoint void debugtrap(short * sp __asm("sp"));


static UBYTE prgName[256];

/**
 * pre startup code, use to boot the debugged program.
 */
__regargs void startProc(int cmdlen, void * cmdline) {
	// inital save registers
	__asm("movem.l d0-d7/a0-a7,_reginfo");

	// create our port and trap
	replyport = CreatePort(0, 0);
	trap = AllocTrap(-1);

	// exit of allocation failed.
	if (!replyport || trap < 0) {
		endProc();
		return;
	}

	insn[0] |= trap;
	trap += 0x20; // add 0x20 for an easy compare in the trap handler

	msg.msg.mn_Length = sizeof(msg);
	msg.msg.mn_ReplyPort = replyport;

	// install the trap handler
	struct Process * process = (struct Process *)FindTask(0);
	oldTrapCode = process->pr_Task.tc_TrapCode;
	process->pr_Task.tc_TrapCode = (APTR)debugtrap;

	// store stack range - if sp lands there process is finished
	startSp = reginfo.usp;
	upperSp = (ULONG)process->pr_Task.tc_SPUpper;

	// patch programname
	prgName[0] = strlen(process->pr_Task.tc_Node.ln_Name);
	strncpy(&prgName[1], process->pr_Task.tc_Node.ln_Name, prgName[0]);
	struct CommandLineInterface * cli = (struct CommandLineInterface *)BADDR(process->pr_CLI);
	cli->cli_CommandName = MKBADDR(prgName);

	reginfo.regs[0] = cmdlen;
	reginfo.regs[8] = (int)cmdline;
	*(void (**)(void))reginfo.usp = endProc;

	park();
}

/**
 * Some cleanup.
 */
void endProc(void) {
	// signal termination to debugger
	reginfo.pc = 0;
	FreeTrap(trap - 0x20);

	PutMsg(dport, &msg.msg);
	WaitPort(replyport);
	GetMsg(replyport);

	// cleanup and exit
	DeleteMsgPort(replyport);

	RemTask(0);
}

/**
 * Continue here after trap.
 * The debugged program will wait until the message is replied.
 *
 */
void park(void) {
	// transfer control to the debugger.
	PutMsg(dport, &msg.msg);

	// wait for command
	WaitPort(replyport);
	GetMsg(replyport);

	// the pc and the action is set by the main process either CONTINUE or STEP
	calltrap();
}

struct RegInfo savedregs;

/**
 * The handler is used to
 * - interrupt the debugged program -> call park() or endProc()
 * - continue the debugged program -> continue at given pc or last pc
 */
__attribute__((__noinline__)) __regargs void debughandler(unsigned trapcode, UWORD ** pc) {
	// always disable trace
	savedregs.sr &= 0x7fff;

	if (newpc) {
		// handle commands
		savedregs = reginfo; // restore regs
		// also enable trace
		if (action > 0) {
			savedregs.sr |= 0x8000; // enble trace
		}

		// set a new pc
		*pc = newpc;
		newpc = 0;
	} else {
		// interrupt program
		reginfo = savedregs;

		if (reginfo.usp > startSp && reginfo.usp < upperSp)
			*pc = (UWORD *)endProc;
		else
			*pc = (UWORD *)park;
	}
}



/**
 * The debug trap handler.
 *
 * save all registers
 * call the c debughandler
 *
 */
__interrupt __entrypoint void debugtrap(short * sp __asm("sp")) {
	__asm("movem.l d0-d7/a0-a6, _savedregs");
	__asm("move.l usp,a0");
	__asm("move.l a0, _savedregs + 15*4"); // usp
	__asm("move.w 4(sp), _savedregs + 16*4"); // status
	__asm("move.l 6(sp), _savedregs + 17*4"); // pc

	debughandler(*(unsigned *)sp, (UWORD **)(sp + 3));

	__asm("move.w _savedregs + 16*4, 4(sp)"); // status
	__asm("move.l _savedregs + 15*4, a0"); // usp
	__asm("move.l a0, usp");

	__asm("lea 4(sp),sp");
	__asm("movem.l _savedregs, d0-d7/a0-a6");
}
