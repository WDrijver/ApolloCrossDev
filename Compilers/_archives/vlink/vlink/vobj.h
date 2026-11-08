/* $VER: vlink vobj.h V0.18a (14.10.25)
 *
 * This file is part of vlink, a portable linker for multiple
 * object formats.
 * Copyright (c) 1997-2025  Frank Wille
 */

/* maximum VOBJ version to support */
#define VOBJ_MAX_VERSION 3
#define VOBJV3 (2<<2)

/* symbol types */
#define LABSYM 1
#define IMPORT 2
#define EXPRESSION 3

/* symbol flags */
#define TYPE(sym) ((sym)->flags&7)
#define TYPE_UNKNOWN  0
#define TYPE_OBJECT   1
#define TYPE_FUNCTION 2
#define TYPE_SECTION  3
#define TYPE_FILE     4

#define EXPORT (1<<3)
#define INEVAL (1<<4)
#define COMMON (1<<5)
#define WEAK (1<<6)
#define SYMINDIR (1<<18)      /* V3+ */

/* section flags */
#define UNALLOCATED (1<<2)
#define ABSOLUTE    (1<<4)    /* V3+ writes absolute section address */


typedef lword taddr;

struct vobj_symbol {
  char *name;
  int type,flags,sec,size;
  taddr val;
};

#define VOBJ_REL_S 0x20       /* signed VOBJ reloc type */
#define VOBJ_REL_U 0x40       /* unsigned VOBJ reloc type */
#define FIRST_CPU_RELOC 0x80  /* first cpu-specific reloc type in VOBJ */
#define STD_REL_TYPE(t) ((t)&0x1f)
