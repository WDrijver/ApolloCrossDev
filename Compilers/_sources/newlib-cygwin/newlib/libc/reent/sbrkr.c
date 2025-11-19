/* Reentrant version of sbrk system call. */

#include <reent.h>
#include <unistd.h>
#include <_syslist.h>

static int __nada;
