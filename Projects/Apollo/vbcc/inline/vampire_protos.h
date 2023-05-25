#ifndef _VBCCINLINE_VAMPIRE_H
#define _VBCCINLINE_VAMPIRE_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

APTR __V_AllocExpansionPort(__reg("a6") void *, __reg("d0") ULONG unitNum, __reg("a1") UBYTE * name)="\tjsr\t-6(a6)";
#define V_AllocExpansionPort(unitNum, name) __V_AllocExpansionPort(VampireBase, (unitNum), (name))

VOID __V_FreeExpansionPort(__reg("a6") void *, __reg("d0") ULONG unitNum)="\tjsr\t-12(a6)";
#define V_FreeExpansionPort(unitNum) __V_FreeExpansionPort(VampireBase, (unitNum))

#endif /*  _VBCCINLINE_VAMPIRE_H  */
