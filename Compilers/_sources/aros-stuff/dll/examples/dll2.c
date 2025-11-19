// Minimal dll example

#include "dll.h"
#include <stdio.h>

void (*extfunc)();
int *extvar;

void function(void)
{
	*extvar = 100;
    extfunc();
    printf("dll2 function called\n");
}

dll_tExportSymbol DLL_ExportSymbols[] =
{

    {function, "function"},
    {NULL, NULL}
};

dll_tImportSymbol DLL_ImportSymbols[] =
{
    {(void **)&extfunc, "function", "dll1.dll", NULL},
    {(void **)&extvar, "variable", "dll1.dll", NULL},
    {NULL, NULL, NULL, NULL}
};

int DLL_Init(void)
{
    printf("dll2 DLL_Init called\n");
    return 1;
}

void DLL_DeInit(void)
{
    printf("dll2 DLL_DeInit called\n");
}
