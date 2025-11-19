#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <clib/dos_protos.h>
#include <dos/dos.h>
#include <proto/dos.h>
#include <inline/dos.h>
#include <stabs.h>

#pragma GCC optimize ("no-toplevel-reorder")

BPTR * __fh;
int __maxfh;

asm("_open: .globl _open");
int _open(const char *name, int flags, ...) {
	int mode;
	if (flags & O_CREAT) {
		if (flags & O_TRUNC)
			mode = MODE_NEWFILE;
		else
			mode = MODE_READWRITE;
	} else
		mode = MODE_OLDFILE;

	BPTR r = Open(name, mode);
	if (!r)
		return -1;

	int fh = 3;
	while(fh < __maxfh) {
		if (!__fh[fh])
			break;
		++fh;
	}

	if (fh == __maxfh) {
		int n = __maxfh + __maxfh + 8;
		__fh = (BPTR*)realloc(__fh, n * sizeof(BPTR));
		if (!__fh) {
			exit(ENOMEM);
		}
		while (__maxfh < n) {
			__fh[__maxfh++] = 0;
		}
	}
	__fh[fh] = r;
	return fh;
}

asm("_close: .globl _close");
int _close(int file) {
	int r = -1;
	if (file < __maxfh) {
		 r = Close(__fh[file]) == 0;
		 __fh[file] = 0;
	}
	return r;
}

static BPTR wbstdout;
static int check_fno(unsigned file) {

	if (file >= __maxfh)
		return 0;

	if (file < 3 && __fh[0] == 0) {
		__fh[0] = Input();

		if (__fh[0] == 0) {
			__fh[0] = __fh[1] = __fh[2] = wbstdout = Open("*", MODE_OLDFILE);
		} else {
			__fh[1] = __fh[2] = Output();
		}
	}

	return __fh[file];
}

asm("_write: .globl _write");
int _write(int file, char *ptr, int len) {
	if (check_fno(file))
		return Write(__fh[file], ptr, len);
	return -1;
}

asm("_read: .globl _read");
int _read(int file, char *ptr, int len) {
	if (check_fno(file))
		return Read(__fh[file], ptr, len);
	return -1;
}


void __init_fh() {
	__fh = (BPTR *)calloc(4, sizeof(BPTR));
	if (!__fh)
		exit(ENOMEM);

	__maxfh = 4;
}

void __exit_fh() {
	if (wbstdout)
		Close(wbstdout);
}

int _isatty(int file) {
	return (unsigned)file <= 2;
}

ADD2INIT(__init_fh, -50);
ADD2EXIT(__exit_fh, -50);

