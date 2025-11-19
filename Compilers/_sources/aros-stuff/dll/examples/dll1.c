// Minimal dll example

#include "dll.h"
#include <stdio.h>

int variable = 0;

void function(void)
{
    printf("dll1 function called variable: %d\n", variable);
}

dll_tExportSymbol DLL_ExportSymbols[] =
{
    {&variable, "variable"},
    {function, "function"},
    {NULL, NULL}
};

dll_tImportSymbol DLL_ImportSymbols[] =
{
    {NULL, NULL, NULL, NULL}
};

int DLL_Init(void)
{
    printf("dll1 DLL_Init called\n");
    return 1;
}

void DLL_DeInit(void)
{
    printf("dll1 DLL_DeInit called\n");
}
