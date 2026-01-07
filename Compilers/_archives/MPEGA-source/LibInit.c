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

    File    :   LibInit.c

    Author  :   Stéphane TAVENARD

    $VER: LibInit.c 37.6 (27.3.97)
    (C) Copyright 1996-97 Andreas R. Kleinert
        All Rights Reserved.

    (C) Copyright 1997-1999 Stéphane TAVENARD
        All Rights Reserved

    #Rev|   Date   |                      Comment
    ----|----------|--------------------------------------------------------
    0   |25/10/1997| Initial revision                                     ST
    1   |14/06/1998| Adapted to Power Up PPC                              ST
    2   |20/06/1998| Set Task pri of PPC Msg handler                      ST
    3   |21/06/1998| Added MPEGA_scale -> bumped to V2.0                  ST
    4   |24/05/1999| Use new version of mpegdec.lib (bug fixed)           ST
    5   |18/09/1999| Corrected cpu version strings                        ST

    ------------------------------------------------------------------------

    Library initializers and functions to be called by StartUp.c

------------------------------------------------------------------------------*/

#define __USE_SYSBASE         // ignored by Maxon

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/resident.h>
#include <exec/initializers.h>
#include <dos/dos.h>
#include <dos/dosextens.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include "compiler.h"

#ifdef _PPC // #1
#include <powerup/ppclib/interface.h>
#include <powerup/ppclib/message.h>
#include <powerup/ppclib/tasks.h>
#include <powerup/proto/ppc.h>
#endif // #1 End

#include "defs.h"
#include "MPEGAbase.h"
#include "MPEGAfuncs.h"

#ifdef _PPC // #1
#include "mpegappc.h"
#endif // #1 End


ULONG __saveds __stdargs L_OpenLibs( void );
void  __saveds __stdargs L_CloseLibs( void );

struct ExecBase      *SysBase = NULL;
struct DosLibrary    *DOSBase = NULL;

#ifdef _PPC // #1 Begin
struct Library       *PPCLibBase = NULL;

#define PPC_TASK_NAME "MPEGA.library PPC"
#define PPC_PROG_NAME "mpegappc_library.elf"
void *ElfObject = NULL;
void *PPCTask = NULL;
struct SignalSemaphore PPCSemaphore;
#endif // #1 End

#define VERSION  2
#define REVISION 4

#ifndef CPU_VERSION // #5

#ifdef _PPC // #1
   #define CPU_VERSION "PPC"
#else
#ifdef _M68060
#ifdef _M68881
   #define CPU_VERSION "68040/60+FPU"
#else
   #define CPU_VERSION "68040/60"
#endif
#else
#ifdef _M68040
#ifdef _M68881
   #define CPU_VERSION "68040+FPU"
#else
   #define CPU_VERSION "68040"
#endif

#else
#ifdef _M68020
#ifdef _M68881
   #define CPU_VERSION "68020/30+FPU"
#else
   #define CPU_VERSION "68020/30"
#endif

#else
#ifdef _I486
   #define CPU_VERSION "486"
#else
#ifdef _I586
   #define CPU_VERSION "586"
#else
   #define CPU_VERSION "Unknown_CPU"
#endif
#endif

#endif
#endif
#endif
#endif

#endif // #5

char __aligned ExLibName [] = "mpega.library";
char __aligned ExLibID   [] = "mpega 2.4 "__AMIGADATE__" ["CPU_VERSION"] MPEG Audio decoding library (C)1997-1999 Stephane TAVENARD";
char __aligned Copyright [] = "(C)opyright 1997-1999 by Stéphane TAVENARD. All rights reserved.";

extern ULONG InitTab[];

extern APTR EndResident; /* below */

struct Resident __aligned ROMTag =     /* do not change */
{
 RTC_MATCHWORD,
 &ROMTag,
 &EndResident,
 RTF_AUTOINIT,
 VERSION,
 NT_LIBRARY,
 0,
 &ExLibName[0],
 &ExLibID[0],
 &InitTab[0]
};

APTR EndResident;

struct MyDataInit                      /* do not change */
{
 UWORD ln_Type_Init;      UWORD ln_Type_Offset;      UWORD ln_Type_Content;
 UBYTE ln_Name_Init;      UBYTE ln_Name_Offset;      ULONG ln_Name_Content;
 UWORD lib_Flags_Init;     UWORD lib_Flags_Offset;    UWORD lib_Flags_Content;
 UWORD lib_Version_Init;   UWORD lib_Version_Offset;  UWORD lib_Version_Content;
 UWORD lib_Revision_Init; UWORD lib_Revision_Offset; UWORD lib_Revision_Content;
 UBYTE lib_IdString_Init; UBYTE lib_IdString_Offset; ULONG lib_IdString_Content;
 ULONG ENDMARK;
} DataTab =
{
 INITBYTE(OFFSET(Node,         ln_Type),      NT_LIBRARY),
 0x80, (UBYTE) OFFSET(Node,    ln_Name),      (ULONG) &ExLibName[0],
 INITBYTE(OFFSET(Library,      lib_Flags),    LIBF_SUMUSED|LIBF_CHANGED),
 INITWORD(OFFSET(Library,      lib_Version),  VERSION),
 INITWORD(OFFSET(Library,      lib_Revision), REVISION),
 0x80, (UBYTE) OFFSET(Library, lib_IdString), (ULONG) &ExLibID[0],
 (ULONG) 0
};

 /* Libraries not shareable between Processes or libraries messing
    with RamLib (deadlock and crash) may not be opened here - open/close
    these later locally and or maybe close them fromout L_CloseLibs()
    when expunging !
 */

ULONG __saveds __stdargs L_OpenLibs( void ) {

#ifdef _PPC // #1
   struct TagItem MyTags[ 8 ];
#endif

   SysBase = (*((struct ExecBase **) 4));
   DOSBase = (struct DosLibrary *)OpenLibrary( DOSNAME, 0L );
   if( !DOSBase ) goto error;

#ifdef _PPC // #1
   PPCLibBase = OpenLibrary( "ppc.library", 44 );
   if( !PPCLibBase ) goto error;

   ElfObject = PPCLoadObject( PPC_PROG_NAME );
   if( !ElfObject ) {
      ElfObject = PPCLoadObject( "LIBS:"PPC_PROG_NAME );
      if( !ElfObject ) goto error;
   }

   MyTags[0].ti_Tag          = PPCTASKTAG_NAME;
   MyTags[0].ti_Data         = (ULONG)PPC_TASK_NAME;

   MyTags[1].ti_Tag          = TAG_DONE;

   PPCTask = PPCCreateTask( ElfObject, MyTags );
   if( !PPCTask ) goto error;
   InitSemaphore( &PPCSemaphore );
   // #2 Begin
   {
      struct Task *task;

      task = FindTask( "PPC:MsgHandler "PPC_TASK_NAME );
      if( task ) {
         SetTaskPri( task, 21 );
      }
   }
   // #2 End
#endif

   return TRUE;

error:
   L_CloseLibs();
   return FALSE;
}

void __saveds __stdargs L_CloseLibs( void ) {

#ifdef _PPC // #1 begin

   struct TagItem MyTags[ 8 ];

   if( PPCTask ) {
      void *PPCPort = NULL;
      void *M68kMsg, *ReplyPort;
      int loop;

      MyTags[0].ti_Tag  = PPCPORTTAG_NAME;
      MyTags[0].ti_Data = (ULONG)MPEGA_PPC_PORT;
      MyTags[1].ti_Tag  = TAG_DONE;
      loop = 50;

      while( loop-- ) {
         PPCPort = PPCObtainPort( MyTags );
         if( PPCPort ) break;
         Delay( 1 );
      }
      if( PPCPort ) {
         // Create Reply Port of M68k Message
         MyTags[0].ti_Tag = TAG_DONE;
         ReplyPort = PPCCreatePort( MyTags );
         if( ReplyPort ) {
            // Create M68k Message
            M68kMsg = PPCCreateMessage( ReplyPort, sizeof(struct M68kData) );
            if( M68kMsg ) {
               // Stop PPC Task
               PPCSendMessage( PPCPort, M68kMsg, NULL, 0, MSG_ID_STOP );
               PPCReleasePort( PPCPort );
               PPCWaitPort( ReplyPort );
               PPCDeleteMessage( M68kMsg );
            }
            else {
               PPCReleasePort( PPCPort );
            }
            while( PPCDeletePort( ReplyPort ) == FALSE ) Delay( 1 );
         }
         // Wait for PPC Task to shut down

         while( 1 ) {
            Delay( 1 );
            if( PPCFindTaskObject( PPCTask ) == NULL ) break;
         }
      }
      PPCTask = NULL;
   }
   if( ElfObject ) {
      PPCUnLoadObject( ElfObject );
      ElfObject = NULL;
   }
   if( PPCLibBase ) {
      CloseLibrary( PPCLibBase );
      PPCLibBase = NULL;
   }

#endif // #1 End

   if( DOSBase ) {
      CloseLibrary( (struct Library *)DOSBase );
      DOSBase = NULL;
   }
}

