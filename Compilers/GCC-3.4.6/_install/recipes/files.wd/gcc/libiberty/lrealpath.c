/* Libiberty realpath.  Like realpath, but more consistent behavior.
   Based on gdb_realpath from GDB.

   Copyright 2003 Free Software Foundation, Inc.

   This file is part of the libiberty library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/*

@deftypefn Replacement {const char*} lrealpath (const char *@var{name})

Given a pointer to a string containing a pathname, returns a canonical
version of the filename.  Symlinks will be resolved, and ``.'' and ``..''
components will be simplified.  The returned value will be allocated using
@code{malloc}, or @code{NULL} will be returned on a memory allocation error.

@end deftypefn

*/

#include "config.h"
#include "ansidecl.h"
#include "libiberty.h"


#include <limits.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

/* On GNU libc systems the declaration is only visible with _GNU_SOURCE.  */
#if defined(HAVE_CANONICALIZE_FILE_NAME) \
    && defined(NEED_DECLARATION_CANONICALIZE_FILE_NAME)
extern char *canonicalize_file_name (const char *);
#endif

#if defined(HAVE_REALPATH)
# if defined (PATH_MAX)
#  define REALPATH_LIMIT PATH_MAX
# else
#  if defined (MAXPATHLEN)
#   define REALPATH_LIMIT MAXPATHLEN
#  endif
# endif
#endif

char *
lrealpath (filename)
     const char *filename;
{
  /* Method 1: The system has a compile time upper bound on a filename
     path.  Use that and realpath() to canonicalize the name.  This is
     the most common case.  Note that, if there isn't a compile time
     upper bound, you want to avoid realpath() at all costs.  */
#if defined(REALPATH_LIMIT)
  {
    char buf[REALPATH_LIMIT];
    const char *rp = realpath (filename, buf);
    if (rp == NULL)
      rp = filename;
    return strdup (rp);
  }
#endif /* REALPATH_LIMIT */

  /* Method 2: The host system (i.e., GNU) has the function
     canonicalize_file_name() which malloc's a chunk of memory and
     returns that, use that.  */
#if defined(HAVE_CANONICALIZE_FILE_NAME)
  {
    char *rp = canonicalize_file_name (filename);
    if (rp == NULL)
      return strdup (filename);
    else
      return rp;
  }
#endif

  /* Method 3: Now we're getting desperate!  The system doesn't have a
     compile time buffer size and no alternative function.  Query the
     OS, using pathconf(), for the buffer limit.  Care is needed
     though, some systems do not limit PATH_MAX (return -1 for
     pathconf()) making it impossible to pass a correctly sized buffer
     to realpath() (it could always overflow).  On those systems, we
     skip this.  */
#if defined (HAVE_REALPATH) && defined (HAVE_UNISTD_H)
  {
    /* Find out the max path size.  */
    long path_max = pathconf ("/", _PC_PATH_MAX);
    if (path_max > 0)
      {
	/* PATH_MAX is bounded.  */
	char *buf, *rp, *ret;
	buf = malloc (path_max);
	if (buf == NULL)
	  return NULL;
	rp = realpath (filename, buf);
	ret = strdup (rp ? rp : filename);
	free (buf);
	return ret;
      }
  }
#endif

  /* This system is a lost cause, just duplicate the filename.  */
  return strdup (filename);
}
