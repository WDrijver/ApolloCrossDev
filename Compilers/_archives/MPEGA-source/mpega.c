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
#include <exec/exec.h>
#include <proto/exec.h>
#include "mpegabase.h"
#include "MPEGAfuncs.h"
#include "defs.h"
#include "mpegdec.h"
#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/memory.h>
#include <utility/tagitem.h>
#include <dos/stdio.h>
#include <proto/dos.h>
#include <clib/utility_protos.h>

struct MPEGA_BASE *MPEGABase;

#pragma libbase MPEGA_BASE

extern struct ExecBase *SysBase;                /* ExecBase */

struct Library *PowerPCBase;

void INIT_0_PowerPCBase(void)
{
 PowerPCBase=OpenLibrary("powerpc.library",0);
}

void EXIT_0_PowerPCBase(void)
{
 if (PowerPCBase) CloseLibrary(PowerPCBase);
}

ULONG CallHookPktPPC(struct Hook *hook,APTR obj, APTR msg)
{
 return CallHookPkt(hook,obj,msg);
}



void EXIT_0_FreeLibs(register __a6 struct MPEGA_BASE *base)
{
                base=MPEGABase;
}


void INIT_0_InitTheLib(register __a6 struct MPEGA_BASE *base)
{
        MPEGABase=base;
}

MPEGA_STREAM* __saveds MPEGA_open( register __a0 char *filename,
                                   register __a1 MPEGA_CTRL *ctrl ) {

   return (MPEGA_STREAM *)MPEGDEC_open( filename, (MPEGDEC_CTRL *)ctrl );
}

void  __saveds MPEGA_close( register __a0 MPEGA_STREAM *mpds ) {

   MPEGDEC_close( (MPEGDEC_STREAM *)mpds );
}

LONG  __saveds MPEGA_decode_frame( register __a0 MPEGA_STREAM *mpds,
                                         register __a1 WORD *pcm[ MPEGA_MAX_CHANNELS ] ) {

   return (LONG)MPEGDEC_decode_frame( (MPEGDEC_STREAM *)mpds, pcm );
}

LONG  __saveds MPEGA_seek( register __a0 MPEGA_STREAM *mpds,
                                 register __d0 ULONG ms_time_position ) {

   return (LONG)MPEGDEC_seek( (MPEGDEC_STREAM *)mpds, (UINT32)ms_time_position );
}

LONG  __saveds MPEGA_time( register __a0 MPEGA_STREAM *mpds,
                                 register __a1 ULONG *ms_time_position ) {

   return (LONG)MPEGDEC_time( (MPEGDEC_STREAM *)mpds, (UINT32 *)ms_time_position );
}

LONG  __saveds MPEGA_find_sync( register __a0 BYTE *buffer,
                                      register __d0 LONG buffer_size ) {

   return (LONG)MPEGDEC_find_sync( (INT8 *)buffer, (INT32)buffer_size );

}

// #5 Begin
LONG  __saveds MPEGA_scale( register __a0 MPEGA_STREAM *mpds,
                                  register __d0 LONG scale_percent ) {

   return (LONG)MPEGDEC_scale( (MPEGDEC_STREAM *)mpds, (INT32)scale_percent );
}
// #5 End

