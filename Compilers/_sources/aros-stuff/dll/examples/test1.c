#include "dll.h"
#include <stdio.h>

void (*extfunc)();
int *v;

dll_tImportSymbol DLL_ImportSymbols[] =
{
	{(void **)&extfunc, "function", "dll2.dll", NULL},
	{NULL, NULL, NULL, NULL}
};

int main(void)
{
	if (!dllImportSymbols())
	{
		return 1;
	}

	extfunc();
	
	// thhe atexit handler will close the dll, no need to murder anybody...
	//dllKillLibrary("dll2.dll");

	return 0;
}
