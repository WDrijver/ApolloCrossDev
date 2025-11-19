#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <clib/dos_protos.h>
#include <dos/dos.h>
#include <proto/dos.h>
#include <inline/dos.h>
#include <proto/exec.h>
#include <inline/exec.h>

int _kill(int pid, int sig) {

}

int _getpid() {
	return (int)FindTask(0);
}

extern BPTR * __fh;
extern int __maxfh;


int _lseek(int file, int ptr, int dir) {
	if (file >= __maxfh) {
		errno = EIO;
		return -1;
	}

	SetIoErr(0);
	Seek(__fh[file], ptr, dir - 1);
	int err = IoErr();
	if (err) {
		errno = EIO;
		return -1;
	}
	if (dir == SEEK_SET)
		return ptr;
	return Seek(__fh[file], 0, OFFSET_CURRENT);
}

int lseek(int file, int ptr, int dir) {
	return _lseek(file, ptr, dir);
}

int _fstat(int file, struct stat *st) {
	return 0;
}

int _unlink(char *name) {
	return !DeleteFile(name);
}

int __stdargs
ioctl (int fd, unsigned long cmd, ...)
{
  return -1;
}
