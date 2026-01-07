/*
* This file is part of MPEGALibrary.
* Copyright (C) 1999 Stephane Tavenard
* 
* MPEGALibrary is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* MPEGALibrary is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with MPEGALibrary.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/*
**      $VER: compiler.h 37.5 (24.1.97)
**
**      Compiler independent register and SAS/C extensions handling
**
**      (C) Copyright 1997 Andreas R. Kleinert
**      All Rights Reserved.
*/

/* Basically, Amiga C compilers must reach the goal to be
   as SAS/C compatible as possible. But on the other hand,
   when porting AmigaOS to other platforms, one perhaps
   can't expect GCC becoming SAS/C compatible.

   There are two ways to make your sources portable:

    - using non ANSI SAS/C statements and making these
      "available" to the other compilers (re- or undefining)
    - using replacements for SAS/C statements and smartly
      redefining these for any compiler

   The last mentioned is the most elegant, but may require to
   rewrite your source codes, so this compiler include file
   basically does offer both.

   For some compilers, this may have been done fromout project or
   makefiles for the first method (e.g. StormC) to ensure compileablity.

   Basically, you should include this header file BEFORE any other stuff.
*/

/* ********************************************************************* */
/* Method 1: redefining SAS/C keywords                                   */
/*                                                                       */
#ifdef __MAXON__  // ignore this switches of SAS/Storm
#define __aligned
#define __asm
#define __regargs
#define __saveds
#define __stdargs
#endif

 /* for SAS/C we don't need this, for StormC this is done in the
    makefile or projectfile */

/*                                                                       */
/* ********************************************************************* */


/* ********************************************************************* */
/* Method 2: defining our own keywords                                   */
/*                                                                       */
#ifdef __SASC

#  define REG(r)     register __ ## r
#  define SAVEDS     __saveds
#  define ASM        __asm
#  define REGARGS    __regargs
#  define STDARGS    __stdargs
#  define ALIGNED    __aligned

#else
# ifdef __MAXON__

#   define REG(r)    register __ ## r
#   define SAVEDS
#   define ASM
#   define REGARGS
#   define STDARGS
#   define ALIGNED

# else
#   ifdef __STORM__

#     define REG(r)  register __ ## r
#     define SAVEDS  __saveds
#     define ASM
#     define REGARGS
#     define STDARGS
#     define ALIGNED

#   else /* any other compiler, to be added here */

#     define REG(r)
#     define SAVEDS
#     define ASM
#     define REGARGS
#     define STDARGS
#     define ALIGNED

#   endif /* __STORM__ */
# endif /* __MAXON__ */
#endif /* __SASC */
/*                                                                       */
/* ********************************************************************* */
