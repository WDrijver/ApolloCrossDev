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
#include "defs.h" // #8
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitstr.h"

#ifdef AMIGA // #7
#include <dos/stdio.h>
#include <proto/dos.h>
#ifdef HOOKS
#endif
#endif

static ULONG __saveds def_baccess( register __a0 struct Hook       *hook,
                                     register __a2 APTR               handle,
                                     register __a1 BSTR_ACCESS_PARAM *access ) {
/*-----------------------------------------------------------------------
*/

   switch( access->func ) {

      case BSTR_FUNC_OPEN:
         return (ULONG)def_open( access->data.open.stream_name,
                                 access->data.open.buffer_size,
                                 &access->data.open.stream_size );
      case BSTR_FUNC_CLOSE:
         def_close( (long)handle );
         break;
      case BSTR_FUNC_READ:
         return (ULONG)def_read( (long)handle, access->data.read.buffer,
                                 access->data.read.num_bytes );
      case BSTR_FUNC_SEEK:
         return (ULONG)def_seek( (long)handle, access->data.seek.abs_byte_seek_pos );
   }
   return 0;
}

typedef ULONG (*BSTR_HOOK_FUNC)( register __a0 struct Hook       *hook,
                                     register __a2 APTR               handle,
                                     register __a1  BSTR_ACCESS_PARAM *access );
