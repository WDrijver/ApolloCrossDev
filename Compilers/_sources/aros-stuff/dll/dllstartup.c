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

/* DLL Startup function
 * This file gets linked in when the user does not define a main function
 * that is, if he wants to compile a dll
 */

#include <string.h>
#include <proto/exec.h>
#include <clib/alib_protos.h>
#include "debug.h"
#include "dll.h"
#include "dll_intern.h"

/*
* Only DLL's can export symbols so this function is defined here.
* Note that on the other hand normal executables *can* have a symbolimport table,
* so dllImportSymbols is defined elsewhere.
*/

void dllExportSymbol(dll_tSymbolQuery *sym)
{
	dll_tExportSymbol *symtable = DLL_ExportSymbols;  //reference DLL's export symbol table

	if (!sym->SymbolPointer)
		return;     //Paranoia

	while (symtable->SymbolAddress) //End of table ??
	{
		if (strcmp(symtable->SymbolName, sym->SymbolName) == 0)
		{
			//FIXME: Stackframe handling
			*sym->SymbolPointer = symtable->SymbolAddress;
			return;
		}
		symtable++;
	}

	*sym->SymbolPointer = NULL;  //Symbol not found
}

/* 
** The actual main function of a DLL
*/

int main(int argc, char **argv)
{
	struct MsgPort *myport;
	char *PortName;
	dll_tMessage *msg;
	int expunge = FALSE;
	int opencount = 0;

	/*
	* If an argument was passed, use it as the port name,
	* otherwise use the program name
	*/
	if (argc > 1)
	{
		PortName = argv[1];
	}
	else
	{
		PortName = argv[0];
	}

	/*
	* Process symbol import table
	*/
	if (!dllImportSymbols())
	{
		D(bug("[DLL] %s: dllImportSymbols failed\n", PortName));
		//exit(0);
		return 0;
	}

	/*
	* Call DLL specific constructor
	*/
	if (!DLL_Init())
	{
		D(bug("[DLL] %s: DLL_Init failed\n", PortName));
		//exit(0);
		return 0;
	}

	/*
	* Create a (public) message port
	*/
	if (!(myport = CreatePort(PortName, 0)))
	{
		D(bug("[DLL] %s: can't create message port\n", PortName));
		//exit(0);
		return 0;
	}

	/*
	** Loop until DLL expunges (that is if a CloseMessage leads to opencount==0)
	** and no pending Messages are left
	*/
	while ((msg = (dll_tMessage *)GetMsg(myport)) || (!expunge))
	{
		if (msg)
		{
			switch (msg->dllMessageType)
			{
				case DLLMTYPE_Open:
					D(bug("[DLL] %s: received DLLMTYPE_Open\n", PortName));
					if (msg->dllMessageData.dllOpen.StackType != DLLSTACK_DEFAULT)
					{
						msg->dllMessageData.dllOpen.ErrorCode = DLLERR_StackNotSupported;
						if (opencount <= 0)
						{
							D(bug("[DLL] %s: unsupported stack type %d, expunging\n",
								PortName, msg->dllMessageData.dllOpen.StackType));
							expunge = TRUE;
						}
						break;
					}
					opencount++;
					D(bug("[DLL] %s: increased open count to %d\n", PortName, opencount));
					if (opencount > 0)
					{
						expunge = FALSE;
					}
					msg->dllMessageData.dllOpen.ErrorCode = DLLERR_NoError;
					break;

				case DLLMTYPE_Close:
					D(bug("[DLL] %s: received DLLMTYPE_Close\n", PortName));
					opencount--;
					D(bug("[DLL] %s: descreased open count to %d\n", PortName, opencount));
					if (opencount <= 0)
					{
						D(bug("[DLL] %s: open count is 0, expunging\n", PortName));
						expunge = TRUE;
					}
					break;

				case DLLMTYPE_SymbolQuery:
					D(bug("[DLL] %s: received DLLMTYPE_SymbolQuery\n", PortName));
					dllExportSymbol(&msg->dllMessageData.dllSymbolQuery);
					D(bug("[DLL] %s: result of symbol query for '%s': %p\n",
						PortName, msg->dllMessageData.dllSymbolQuery.SymbolName,
						*msg->dllMessageData.dllSymbolQuery.SymbolPointer));
					break;

				case DLLMTYPE_Kill:
					D(bug("[DLL] %s: received DLLMTYPE_Kill\n", PortName));
					expunge = TRUE;
					break;
			}

			/*
			* Send the message back
			*/
			D(bug("[DLL] %s: replying message\n", PortName));
			ReplyMsg((struct Message *)msg);
		}

		/*
		* Wait for messages to pop up
		* Note that if the DLL is expunged it doesn't wait anymore,
		* but it still processes all pending messages (including open messages
		* which can disable the expunge flag).
		* FIXME: Is this multithread safe ??
		*/
		if (!expunge)
		{
			D(bug("[DLL] %s: Waiting for message...\n", PortName));
			WaitPort(myport);
		}
	}

	/*
	* Delete public port
	*/
	DeletePort(myport);

	/*
	* Call DLL specific destructor
	*/
	DLL_DeInit();

	return 0;
}
