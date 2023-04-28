#include <glob.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "stdlib_headers.h"

pid_t wait(int *status)
{
	__set_errno(ECHILD);

	return -1;
}

pid_t vfork(void)
{
	__set_errno(EAGAIN);

	return -1;
}

int pipe(int pipefd[2])
{
	__set_errno(ENFILE);

	return -1;
}

int glob(const char *pattern, int flags,
	int (*errfunc) (const char *epath, int eerrno),
	glob_t *pglob)
{
	return GLOB_NOMATCH;
}

void globfree(glob_t *pglob)
{
}

int getrusage(int who, struct rusage *usage)
{
	usage->ru_utime.tv_secs = 0;
	usage->ru_utime.tv_micro = 0;
	usage->ru_stime.tv_secs = 0;
	usage->ru_stime.tv_micro = 0;

	return 0;
}

