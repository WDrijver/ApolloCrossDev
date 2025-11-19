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

/*
** This file handles the implicit (loadtime) imports.
** For a DLL its called automatically but a normal executable must call it manually
** if it wants to import symbols from a DLL
*/

#include <exec/exec.h>
#include "dll.h"
#include "dll_intern.h"

int dllImportSymbols()
{
	dll_tImportSymbol *symtable = DLL_ImportSymbols;  //reference caller's import symbol table

	while (symtable->SymbolPointer) //End of table ??
	{
		void *sym;
		void *h;

		if (!(h = dllInternalLoadLibrary(symtable->DLLFileName, symtable->DLLPortName, 0)))
			return 0;

		if (!(sym = dllGetProcAddress(h, symtable->SymbolName)))
			return 0;

		*symtable->SymbolPointer = sym;
		symtable++;
	}
	
	return 1; //Success
}
