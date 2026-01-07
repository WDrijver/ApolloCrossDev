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

    File    :   MPEGAbase.h

    Author  :   Stéphane TAVENARD

    (C) Copyright 1997-1998 Stéphane TAVENARD
        All Rights Reserved

    #Rev|   Date   |                      Comment
    ----|----------|--------------------------------------------------------
    0   |25/10/1997| Initial revision                                     ST

    ------------------------------------------------------------------------

    Base of MPEGA library (private)

------------------------------------------------------------------------------*/

#ifndef MPEGABASE_H
#define MPEGABASE_H

#include <exec/libraries.h>

typedef struct {
   struct Library         lib_node;
   APTR                   seg_list;
   struct ExecBase       *sys_base;
} MPEGA_BASE;

#endif /* MPEGABASE_H */
