/*
 * Copyright (C) 2014 Szilard Biro
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __DLL_DEBUG_H
#define __DLL_DEBUG_H

#ifdef __AROS__
	#include <aros/debug.h>
#else
	#ifdef DEBUG
		#define D(x) x
	#else
		#define D(x)
	#endif
	#ifdef __amigaos4__
		#include <proto/exec.h>
		#define bug DebugPrintF
	#else
		#include <clib/debug_protos.h>
		#define bug(fmt, ...) kprintf((CONST_STRPTR)fmt, __VA_ARGS__)
	#endif
#endif

#endif
