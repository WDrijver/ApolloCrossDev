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

#ifndef __DLL_H
#define __DLL_H

/************************************************************
 * External structures
 ************************************************************/

typedef struct dll_sExportSymbol
{
	void *SymbolAddress;
	char *SymbolName;
} dll_tExportSymbol;

typedef struct dll_sImportSymbol
{
	void **SymbolPointer;
	char *SymbolName;
	char *DLLFileName;
	char *DLLPortName;
} dll_tImportSymbol;

#ifdef __cplusplus
extern "C" {
#endif

void *dllLoadLibrary(const char *name, const char *portname);
int dllFreeLibrary(void *hinst);
void *dllGetProcAddress(void *hinst, const char *name);
char *dllGetLastError(void);
int dllImportSymbols(void);

int     DLL_Init(void);
void    DLL_DeInit(void);

#ifdef __cplusplus
}
#endif

/*
 * Prototypes for DLL implementations
 */

extern dll_tExportSymbol DLL_ExportSymbols[];
extern dll_tImportSymbol DLL_ImportSymbols[];

#endif
