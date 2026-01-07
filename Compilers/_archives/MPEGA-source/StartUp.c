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
/*------------------------------------------------------------------------------

    File    :   Startup.c

    Author  :   Stéphane TAVENARD

    $VER: StartUp.c 37.6 (27.3.97)
    (C) Copyright 1996-97 Andreas R. Kleinert
        All Rights Reserved.

    (C) Copyright 1997-1998 Stéphane TAVENARD
        All Rights Reserved

    #Rev|   Date   |                      Comment
    ----|----------|--------------------------------------------------------
    0   |25/10/1997| Initial revision                                     ST
    1   |21/06/1998| Added MPEGA_scale -> bumped to V2.0                  ST

    ------------------------------------------------------------------------

    Library startup-code and function table definition

------------------------------------------------------------------------------*/

#define __USE_SYSBASE      // ignored by MAXON

#include "defs.h"

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/resident.h>
#include <exec/initializers.h>
#include <proto/exec.h>

//#include "compiler.h"
#include "MPEGAbase.h"

#include "MPEGAfuncs.h"

extern ULONG __saveds __stdargs L_OpenLibs(void);
extern void  __saveds __stdargs L_CloseLibs(void);

MPEGA_BASE * __saveds __asm InitLib( register __a6 struct ExecBase *sysbase,
                                     register __a0 struct SegList *seglist,
                                     register __d0 MPEGA_BASE *mpegabase );
MPEGA_BASE * __saveds __asm OpenLib( register __a6 MPEGA_BASE *mpegabase );
struct SegList * __saveds __asm CloseLib( register __a6 MPEGA_BASE *mpegabase );
struct SegList * __saveds __asm ExpungeLib( register __a6 MPEGA_BASE *mpegabase );
ULONG __saveds __asm ExtFuncLib( void );

LONG __saveds __asm LibStart( void )
{
   return -1;
}

extern APTR FuncTab [];
extern struct MyDataInit DataTab;

struct InitTable {
   ULONG              LibBaseSize;
   APTR              *FunctionTable;
   struct MyDataInit *DataTable;
   APTR               InitLibTable;
} InitTab = {
   sizeof( MPEGA_BASE ),
   &FuncTab[0],
   &DataTab,
   InitLib
};

APTR FuncTab [] = {
   OpenLib,
   CloseLib,
   ExpungeLib,
   ExtFuncLib,

   /* Library functions Begin */

   MPEGA_open,
   MPEGA_close,
   MPEGA_decode_frame,
   MPEGA_seek,
   MPEGA_time,
   MPEGA_find_sync,
   MPEGA_scale, // #1,

   /* Library functions End */

   (APTR) ((LONG)-1)
};


MPEGA_BASE * __saveds __asm InitLib( register __a6 struct ExecBase *sysbase,
                                     register __a0 struct SegList *seglist,
                                     register __d0 MPEGA_BASE *mpegabase )
{
   mpegabase->sys_base = sysbase;
   mpegabase->seg_list = seglist;

   if( L_OpenLibs() ) return mpegabase;

   L_CloseLibs();

   return NULL;
}

MPEGA_BASE * __saveds __asm OpenLib( register __a6 MPEGA_BASE *mpegabase ) {

   mpegabase->lib_node.lib_OpenCnt++;

   mpegabase->lib_node.lib_Flags &= ~LIBF_DELEXP;

   return mpegabase;
}

struct SegList * __saveds __asm CloseLib( register __a6 MPEGA_BASE *mpegabase ) {

   mpegabase->lib_node.lib_OpenCnt--;

   if( !mpegabase->lib_node.lib_OpenCnt ) {
      if( mpegabase->lib_node.lib_Flags & LIBF_DELEXP ) {
         return ExpungeLib( mpegabase );
      }
   }

   return NULL;
}

struct SegList * __saveds __asm ExpungeLib( register __a6 MPEGA_BASE *mpegabase ) {

   struct SegList *seglist;

   if( !mpegabase->lib_node.lib_OpenCnt ) {
      ULONG negsize, possize, fullsize;
      UBYTE *negptr = (UBYTE *)mpegabase;

      seglist = mpegabase->seg_list;

      Remove( (struct Node *)mpegabase );

      L_CloseLibs();

      negsize  = mpegabase->lib_node.lib_NegSize;
      possize  = mpegabase->lib_node.lib_PosSize;
      fullsize = negsize + possize;
      negptr  -= negsize;

      FreeMem( negptr, fullsize );

      return seglist;
   }

   mpegabase->lib_node.lib_Flags |= LIBF_DELEXP;

   return NULL;
}

ULONG __saveds __asm ExtFuncLib( void ) {

   return NULL;
}

void __stdargs _XCEXIT( long lcode ) {
}

#ifdef __SASC

#ifdef ARK_OLD_STDIO_FIX

ULONG XCEXIT       = NULL; /* these symbols may be referenced by    */
ULONG _XCEXIT      = NULL; /* some functions of sc.lib, but should  */
ULONG ONBREAK      = NULL; /* never be used inside a shared library */
ULONG _ONBREAK     = NULL;
ULONG base         = NULL;
ULONG _base        = NULL;
ULONG ProgramName  = NULL;
ULONG _ProgramName = NULL;
ULONG StackPtr     = NULL;
ULONG _StackPtr    = NULL;
ULONG oserr        = NULL;
ULONG _oserr       = NULL;
ULONG OSERR        = NULL;
ULONG _OSERR       = NULL;

#endif /* ARK_OLD_STDIO_FIX */

void __regargs __chkabort(void) { }  /* a shared library cannot be    */
void __regargs _CXBRK(void)     { }  /* CTRL-C aborted when doing I/O */

#endif /* __SASC */
