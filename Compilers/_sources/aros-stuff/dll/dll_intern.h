/*
 * Copyright (C) 2002 Hyperion Entertainment
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

#ifndef __DLL_INTERN_H
#define __DLL_INTERN_H

/************************************************************
 * Internal structures
 ************************************************************/

/*
** The stack type the application using the DLL prefers.
** If there is no support for this type in your DLL, return
** DLLERR_StackNotSupported.
**
** Implementation note: In the startup code, return different
** function pointers depending on the stack frame. This is
** the preferred method. Some of the stack frames might be
** identical, for example, since there is not UBYTE passing
** on the stack for the different DLL interface functions,
** using the same stack from for all 68k stack types is safe.
*/

typedef enum
{
	DLLSTACK_STORM         = 0x01,    // 68k, StormC
	DLLSTACK_EGCS          = 0x02,    // 68k, GCC or egcs
	DLLSTACK_SAS           = 0x04,    // 68k, SAS/C
	DLLSTACK_VBCC          = 0x08,    // 68k, vbcc
	DLLSTACK_POWEROPEN     = 0x10,    // PPC, StormC or vbcc
	DLLSTACK_SYSV          = 0x20     // PPC, egcs
	//..
} dll_tStackType;

#ifdef __STORM__
#ifdef __PPC__
#define DLLSTACK_DEFAULT DLLSTACK_POWEROPEN
#else
#define DLLSTACK_DEFAULT DLLSTACK_STORM
#endif
#else //not Storm
#ifdef __VBCC__
#ifdef __PPC__
#define DLLSTACK_DEFAULT DLLSTACK_POWEROPEN
#else
#define DLLSTACK_DEFAULT DLLSTACK_VBCC
#endif
#else //not VBCC
#ifdef __GNUC__
#ifdef __PPC
#define DLLSTACK_DEFAULT DLLSTACK_SYSV
#else
#define DLLSTACK_DEFAULT DLLSTACK_EGCS
#endif
#else //not GCC
#ifdef __SASC__
#define DLLSTACK_DEFAULT DLLSTACK_SAS
#endif
#endif //GCC
#endif //VBCC
#endif //STORMC

typedef enum
{
	DLLERR_NoError              = 0,    // No error occured
	DLLERR_StackNotSupported    = 1,    // Illegal stack frame
	DLLERR_OutOfMemory          = 2     // Init failed due to memory shortage
} dll_tErrorCode;

typedef enum
{
	DLLMTYPE_Open           = 0,
	DLLMTYPE_Close          = 1,
	DLLMTYPE_SymbolQuery    = 2,
	DLLMTYPE_Kill           = 3
} dll_tMessageType;

typedef struct dll_sSymbolQuery
{
	// Preferred stack type of the main program
	dll_tStackType StackType;
		
	// Name of the Symbol
	char *SymbolName;

	// Where to put the Symbol Address
	void **SymbolPointer;
} dll_tSymbolQuery;

typedef struct dll_sMessage
{
	// Message for sending
	struct Message Message;

	dll_tMessageType dllMessageType;

	union 
	{
		struct
		{
			// Preferred stack type of the main program
			dll_tStackType StackType;

			// Initialization error code
			dll_tErrorCode ErrorCode;
		} dllOpen;

		struct
		{
			//Empty for now
		} dllClose;

		dll_tSymbolQuery dllSymbolQuery;

	} dllMessageData;
} dll_tMessage;

/*
** This structure is returned by the LoadLibrary() call. It is strictly
** Off-Limits for both the caller and the DLL but rather used internally
** to for tracking resources and other stuff for the DLL. This structure
** may change at any time.
*/

typedef struct dll_sInstance
{
	struct Node node;
	struct MsgPort *dllPort;
	char name[1];
} dll_tInstance;

void *dllInternalLoadLibrary(const char *filename, const char *portname, int raiseusecount);
int dllInternalFreeLibrary(dll_tInstance *n);

#endif
