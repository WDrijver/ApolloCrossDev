/* Emulate getcwd using getwd.
   This function is in the public domain. */

/*
NAME
	getcwd -- get absolute pathname for current working directory

SYNOPSIS
	char *getcwd (char pathname[len], len)

DESCRIPTION
	Copy the absolute pathname for the current working directory into
	the supplied buffer and return a pointer to the buffer.  If the 
	current directory's path doesn't fit in LEN characters, the result
	is NULL and errno is set.

	If pathname is a null pointer, getcwd() will obtain size bytes of
	space using malloc.

BUGS
	Emulated via the getwd() call, which is reasonable for most
	systems that do not have getcwd().

*/

#include "config.h"

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#include <errno.h>

extern char *getwd ();
extern int errno;

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

char *
getcwd (buf, len)
  char *buf;
  size_t len;
{
  char ourbuf[MAXPATHLEN];
  char *result;

  result = getwd (ourbuf);
  if (result) {
    if (strlen (ourbuf) >= len) {
      errno = ERANGE;
      return 0;
    }
    if (!buf) {
       buf = (char*)malloc(len);
       if (!buf) {
           errno = ENOMEM;
	   return 0;
       }
    }
    strcpy (buf, ourbuf);
  }
  return buf;
}
