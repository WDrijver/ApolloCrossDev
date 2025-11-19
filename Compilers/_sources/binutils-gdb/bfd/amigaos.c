/* BFD back-end for Commodore-Amiga AmigaOS binaries.
   Copyright (C) 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998
   Free Software Foundation, Inc.
   Contributed by Leonard Norrgard.  Partially based on the bout
   and ieee BFD backends and Markus Wild's tool hunk2gcc.
   Revised and updated by Stephan Thesing.

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*
SECTION
	amiga back end

This section describes the overall structure of the Amiga BFD back end.
The linker stuff can be found in @xref{amigalink}.
@menu
@* implementation::
@* amigalink::
@end menu

INODE
implementation, amigalink, amiga, amiga

SECTION
	implementation

The need for a port of the bfd library for Amiga style object (hunk) files
arose by the desire to port the GNU debugger gdb to the Amiga.
Also, the linker ld should be updated to the current version (2.5.2).
@@*
This port bases on the work done by Leonard Norrgard, who started porting
gdb. Raphael Luebbert, who supports the ixemul.library, has also worked on
implementing the needed @code{ptrace()} system call and gas2.5.

@menu
@* not supported::
@* Does it work?::
@* TODO::
@end menu

INODE
not supported, Does it work?, implementation, implementation

SUBSECTION
	not supported

Currently, the implementation does not support Amiga link library files, like
e.g. amiga.lib. This may be added in a later version, if anyone starts work
on it, or I find some time for it.

The handling of the symbols in hunk files is a little bit broken:
	  o The symbols in a load file are totally ignored at the moment, so gdb and gprof
	    do not work.
	  o The symbols of a object module (Hunk file, starting with HUNK_UNIT) are read in
	    correctly, but HUNK_SYMBOL hunks are also ignored.

The reason for this is the following:
Amiga symbol hunks do not allow for much information. Only a name and a value are allowed.
On the other hand, a.out format carries along much more information (see, e.g. the
entry on set symbols in the ld manual). The old linker copied this information into
a HUNK_DEBUG hunk. Now there is the choice:
	o ignoring the debug hunk, read in only HUNK_SYMBOL definitions => extra info is lost.
	o read in the debug hunk and use the information therein => How can clashs between the
	  information in the debug hunk and HUNK_SYMBOL or HUNK_EXT hunks be avoided?
I haven't decided yet, what to do about this.

Although bfd allows to link together object modules of different flavours,
producing a.out style executables does not work on Amiga :-)
It should, however, be possible to create a.out files with the -r option of ld
(incremental link).

INODE
Does it work?, TODO, not supported, implementation

SUBSECTION
	Does it work?

Currently, the following utilities work:
	o objdump
	o objcopy
	o strip
	o nm
	o ar
	o gas

INODE
TODO, , Does it work?, implementation

SUBSECTION
	TODO

	o fix FIXME:s

@*
BFD:
	o add flag to say if the format allows multiple sections with the
	  same name. Fix bfd_get_section_by_name() and bfd_make_section()
	  accordingly.

	o dumpobj.c: the disassembler: use relocation record data to find symbolic
	  names of addresses, when available.  Needs new routine where one can
	  specify the source section of the symbol to be printed as well as some
	  rewrite of the disassemble functions.
*/

#include "sysdep.h"
#include "bfd.h"
#include "bfdlink.h"
#include "libbfd.h"
#include "libamiga.h"

#define BYTES_IN_WORD 4
#include "aout/aout64.h" /* struct external_nlist */

#ifndef PARAMS
#define PARAMS(x) x
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef alloca
extern void * alloca PARAMS ((size_t));
#endif


extern void amiga_set_link_info(struct bfd_link_info * link_info);
static struct bfd_link_info * plink_info;
void amiga_set_link_info(struct bfd_link_info * link_info)
{
  plink_info = link_info;
}

#define bfd_is_special_section(sec) \
  (bfd_is_abs_section(sec)||bfd_is_com_section(sec)||bfd_is_und_section(sec)||bfd_is_ind_section(sec))

typedef struct aout_symbol {
  asymbol symbol;
  short desc;
  char other;
  unsigned char type;
} aout_symbol_type;

struct arch_syms {
  unsigned long offset;		/* disk offset in the archive */
  unsigned long size;		/* size of the block of symbols */
  unsigned long unit_offset;	/* start of unit on disk */
  char * name; /* pre read name from .stabstr */
  struct arch_syms *next;	/* linked list */
};

typedef struct amiga_ardata_struct {
  /* generic stuff */
  struct artdata generic;
  /* amiga-specific stuff */
  unsigned long filesize;
  struct arch_syms *defsyms;
  unsigned long defsym_count;
  unsigned long outnum;
} amiga_ardata_type;

#define amiga_ardata(bfd) ((bfd)->tdata.amiga_ar_data)

#define bfd_msg (*_bfd_error_handler)

#define GL(x) bfd_get_32 (abfd, (bfd_byte *) (x))
#define GW(x) bfd_get_16 (abfd, (bfd_byte *) (x))
#define LONGSIZE(l) (((l)+3) >> 2)

/* AmigaOS doesn't like HUNK_SYMBOL with symbol names longer than 124 characters */
#define MAX_NAME_SIZE 124

aout_symbol_type  **
amiga_load_stab_symbols (bfd *abfd);
bool
aout_32_find_nearest_line (bfd *abfd, struct aout_symbol **symbols, sec_ptr section,
     bfd_vma offset, const char **filename_ptr, const char **functionname_ptr,
     unsigned int *line_ptr, unsigned int * discriminator_ptr);
bool
aout_32_translate_symbol_table (bfd *abfd, aout_symbol_type *in, struct external_nlist *ext,
     bfd_size_type count, char *str, bfd_size_type strsize, bool dynamic);

static bool amiga_reloc_long_p PARAMS ((unsigned long, bool));
static reloc_howto_type *howto_for_raw_reloc PARAMS ((unsigned long, bool));
static reloc_howto_type *howto_for_reloc PARAMS ((unsigned long));
static bool get_long PARAMS ((bfd *, unsigned long *));
static bool get_word PARAMS ((bfd *, unsigned long *));
static bfd_cleanup amiga_object_p PARAMS ((bfd *));
static sec_ptr amiga_get_section_by_hunk_number PARAMS ((bfd *, long));
static bool amiga_add_reloc PARAMS ((bfd *, sec_ptr, bfd_size_type,
	amiga_symbol_type *, reloc_howto_type *, long));
sec_ptr amiga_make_unique_section PARAMS ((bfd *, const char *));
static bool parse_archive_units PARAMS ((bfd *, int *, file_ptr,
 	bool, struct arch_syms **, symindex *));
static bool amiga_digest_file PARAMS ((bfd *));
static bool amiga_read_unit PARAMS ((bfd *, file_ptr));
static bool amiga_read_load PARAMS ((bfd *));
static bool amiga_handle_cdb_hunk PARAMS ((bfd *, unsigned long,
	unsigned long, unsigned long, unsigned long));
static bool amiga_handle_rest PARAMS ((bfd *, sec_ptr, bool));
static bool amiga_mkobject PARAMS ((bfd *));
static bool amiga_mkarchive PARAMS ((bfd *));
static bool write_longs PARAMS ((const unsigned long *, unsigned long,
	bfd *));
static bool write_words PARAMS ((const unsigned long *, unsigned long,
	bfd *));
static long determine_datadata_relocs PARAMS ((bfd *, sec_ptr));
static void remove_section_index PARAMS ((sec_ptr, int *));
static bool amiga_write_object_contents PARAMS ((bfd *));
static bool write_name PARAMS ((bfd *, const char *, unsigned long));
static bool amiga_write_archive_contents PARAMS ((bfd *));
static bool amiga_write_armap PARAMS ((bfd *, unsigned int,
	struct orl *, unsigned int, int));
static int determine_type PARAMS ((arelent *));
static bool amiga_write_section_contents PARAMS ((bfd *, sec_ptr,
	sec_ptr, unsigned long, int *, int));
static bool amiga_write_symbols PARAMS ((bfd *, sec_ptr));
static bool amiga_get_section_contents PARAMS ((bfd *, sec_ptr, void *,
	file_ptr, bfd_size_type));
static bool amiga_new_section_hook PARAMS ((bfd *, sec_ptr));
static bool amiga_slurp_symbol_table PARAMS ((bfd *));
static long amiga_get_symtab_upper_bound PARAMS ((bfd *));
static long amiga_canonicalize_symtab PARAMS ((bfd *, asymbol **));
static asymbol *amiga_make_empty_symbol PARAMS ((bfd *));
static void amiga_get_symbol_info PARAMS ((bfd *, asymbol *, symbol_info *));
static void amiga_print_symbol PARAMS ((bfd *, void *,   asymbol *,
	bfd_print_symbol_type));
static long amiga_get_reloc_upper_bound PARAMS ((bfd *, sec_ptr));
static bool read_raw_relocs PARAMS ((bfd *, sec_ptr, unsigned long,
	unsigned long));
static bool amiga_slurp_relocs PARAMS ((bfd *, sec_ptr, asymbol **));
static long amiga_canonicalize_reloc PARAMS ((bfd *, sec_ptr, arelent **,
	asymbol **));
static bool amiga_set_section_contents PARAMS ((bfd *, sec_ptr, const void *,
	file_ptr, bfd_size_type));
static bool amiga_set_arch_mach PARAMS ((bfd *, enum bfd_architecture,
	unsigned long));
static int amiga_sizeof_headers PARAMS ((bfd *, struct bfd_link_info *));
static bool amiga_find_nearest_line PARAMS ((bfd *, asymbol **, sec_ptr,
	 bfd_vma, const char **, const char **, unsigned int *, unsigned int *));
static reloc_howto_type *amiga_bfd_reloc_type_lookup PARAMS ((bfd *,
	bfd_reloc_code_real_type));
static bool amiga_bfd_copy_private_bfd_data PARAMS ((bfd *, bfd *));
static bool amiga_bfd_copy_private_section_data PARAMS ((bfd *,
	sec_ptr, bfd *, sec_ptr));
static bool amiga_slurp_armap PARAMS ((bfd *));
static void amiga_truncate_arname PARAMS ((bfd *, const char *, char *));
static bfd_cleanup amiga_archive_p PARAMS ((bfd *));
static bfd *amiga_openr_next_archived_file PARAMS ((bfd *, bfd *));
static void * amiga_read_ar_hdr PARAMS ((bfd *));
static int amiga_generic_stat_arch_elt PARAMS ((bfd *, struct stat *));
static bool amiga_gc_sections (bfd *abfd, struct bfd_link_info *info);


/* #define DEBUG_AMIGA 1 */
#if DEBUG_AMIGA
#include <stdarg.h>
static void
error_print (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  vfprintf (stderr, fmt, args);
  va_end (args);
}
#define DPRINT(L,x) if (L>=DEBUG_AMIGA) error_print x
#else
#define DPRINT(L,x)
#endif

enum {R_ABS32=0,R_ABS16,R_ABS8,R_PC32,R_PC16,R_PC8,R_SD32,R_SD16,R_SD8,R_PC26,R__MAX};
reloc_howto_type howto_table[R__MAX] =
{
  {H_ABS32,   /* type */
      4,          /* size */
      32,         /* bitsize */
      0,          /* rightshift */
      0,          /* bitpos */
      complain_overflow_bitfield,/* complain_on_overflow */
      false,	  /* negate */
      false,      /* pc_relative */
      false,      /* partial_inplace */
      false, 	/* pcrel_offset */
      0xffffffff, /* src_mask */
      0xffffffff, /* dst_mask */
      0,          /* special_function */
      "RELOC32"  /* textual name */
  },
  {H_ABS16,  2, 16, 0, 0, complain_overflow_bitfield, false, false, false, false, 0x0000ffff, 0x0000ffff, 0, "RELOC16"   },
  {H_ABS8,   1,  8, 0, 0, complain_overflow_bitfield, false, false, false, false, 0x000000ff, 0x000000ff, 0, "RELOC8"    },
  {H_PC32,   4, 32, 0, 0, complain_overflow_signed,   false, true,  false, true , 0xffffffff, 0xffffffff, 0, "RELRELOC32"},
  {H_PC16,   2, 16, 0, 0, complain_overflow_signed,   false, true,  false, true , 0x0000ffff, 0x0000ffff, 0, "RELRELOC16"},
  {H_PC8,    1,  8, 0, 0, complain_overflow_signed,   false, true,  false, true , 0x000000ff, 0x000000ff, 0, "RELRELOC8" },
  {H_SD32,   4, 32, 0, 0, complain_overflow_bitfield, false, false, false, false, 0xffffffff, 0xffffffff, 0, "DREL32"    },
  {H_SD16,   2, 16, 0, 0, complain_overflow_bitfield, false, false, false, false, 0x0000ffff, 0x0000ffff, 0, "DREL16"    },
  {H_SD8,    1,  8, 0, 0, complain_overflow_bitfield, false, false, false, false, 0x000000ff, 0x000000ff, 0, "DREL8"     },
  {H_PC26,   4, 26, 0, 0, complain_overflow_signed,   false, true,  false, true , 0x03fffffc, 0x03fffffc, 0, "RELRELOC26"},
};

/* Determine the on-disk relocation size.
   AmigaOS upto 3.9 goofed parsing HUNK_RELRELOC32 within an executable
   and reads the hunk data as 16bit words.  */
static bool
amiga_reloc_long_p (unsigned long type, bool isload)
{
  if (type == HUNK_RELOC32SHORT
      || (isload && (type == HUNK_DREL32 || type == HUNK_RELRELOC32)))
    return false;
  return true;
}

static reloc_howto_type *
howto_for_raw_reloc (unsigned long type, bool isload)
{
  switch (type)
  {
  case HUNK_ABSRELOC32:
  case HUNK_RELOC32SHORT:
    return &howto_table[R_ABS32];
  case HUNK_RELRELOC16:
    return &howto_table[R_PC16];
  case HUNK_RELRELOC8:
    return &howto_table[R_PC8];
  case HUNK_DREL32:
    return &howto_table[isload ? R_ABS32 : R_SD32];
  case HUNK_DREL16:
    return &howto_table[R_SD16];
  case HUNK_DREL8:
    return &howto_table[R_SD8];
  case HUNK_RELRELOC32:
    return &howto_table[R_PC32];
  case HUNK_ABSRELOC16:
    return &howto_table[R_ABS16];
  case HUNK_RELRELOC26:
    return &howto_table[R_PC26];
  default:
    return NULL;
  }
}

static reloc_howto_type *
howto_for_reloc (unsigned long type)
{
  switch (type)
  {
  case EXT_ABSREF32:
  case EXT_ABSCOMMON:
    return &howto_table[R_ABS32];
  case EXT_ABSREF16:
    return &howto_table[R_ABS16];
  case EXT_ABSREF8:
    return &howto_table[R_ABS8];
  case EXT_RELREF32:
  case EXT_RELCOMMON:
    return &howto_table[R_PC32];
  case EXT_RELREF16:
    return &howto_table[R_PC16];
  case EXT_RELREF8:
    return &howto_table[R_PC8];
  case EXT_DEXT32:
  case EXT_DEXT32COMMON:
    return &howto_table[R_SD32];
  case EXT_DEXT16:
  case EXT_DEXT16COMMON:
    return &howto_table[R_SD16];
  case EXT_DEXT8:
  case EXT_DEXT8COMMON:
    return &howto_table[R_SD8];
  case EXT_RELREF26:
    return &howto_table[R_PC26];
  default:
    return NULL;
  }
}

/* The following are gross hacks that need to be fixed.  The problem is
   that the linker unconditionally references these values without
   going through any of bfd's standard interface.  Thus they need to
   be defined in a bfd module that is included in *all* configurations,
   and are currently in bfd.c, otherwise linking the linker will fail
   on non-Amiga target configurations. */

/* This one is used by the linker and tells us, if a debug hunk should
   be written out. */
extern BFDDECL int write_debug_hunk;

/* This is also used by the linker to set the attribute of sections. */
extern BFDDECL int amiga_attribute;

/* used with base-relative linking */
extern BFDDECL int amiga_base_relative;

/* used with -resident linking */
extern BFDDECL int amiga_resident;

static bool
get_long (bfd * abfd, unsigned long *n)
{
  if (bfd_bread ((void *)n, 4, abfd) != 4)
    return false;
  *n = GL (n);
  return true;
}

static bool
get_word (bfd * abfd, unsigned long *n)
{
  if (bfd_bread ((void *)n, 2, abfd) != 2)
    return false;
  *n = GW (n);
  return true;
}

static bfd_cleanup
amiga_object_p (bfd * abfd)
{
  unsigned long x;
  char buf[8];

  /* An Amiga object file must be at least 8 bytes long.  */
  if (bfd_bread (buf, sizeof(buf), abfd) != sizeof(buf))
    {
      bfd_set_error (bfd_error_wrong_format);
      return NULL;
    }

  bfd_seek (abfd, 0, SEEK_SET);

  /* Does it look like an Amiga object file?  */
  x = GL (&buf[0]);
  if ((x != HUNK_UNIT) && (x != HUNK_HEADER))
    {
      /* Not an Amiga file.  */
      bfd_set_error (bfd_error_wrong_format);
      return NULL;
    }

  /* Can't fail and return (but must be declared bool to suit
     other bfd requirements).  */
  (void) amiga_mkobject (abfd);

  AMIGA_DATA(abfd)->IsLoadFile = (x == HUNK_HEADER);

  if (!amiga_digest_file (abfd))
    {
      /* Something went wrong.  */
      DPRINT(20,("bfd parser stopped at offset 0x%lx\n",bfd_tell(abfd)));
      return NULL;
    }

  /* Set default architecture to m68k:68000.  */
  /* So we can link on 68000 AMIGAs... */
  abfd->arch_info = bfd_scan_arch ("m68k:68000");

  return _bfd_no_cleanup;
}

static sec_ptr amiga_get_section_by_hunk_number (bfd *abfd, long hunk_number)
{
  /* A cache, so we don't have to search the entire list every time.  */
  static sec_ptr last_reference;
  static bfd *last_bfd;
  sec_ptr p;

  if (last_reference)
    if (last_bfd == abfd && last_reference->target_index == hunk_number)
       return last_reference;
  for (p = abfd->sections; p != NULL; p = p->next)
    if (p->target_index == hunk_number)
      {
	last_reference = p;
	last_bfd = abfd;
	return p;
      }
  BFD_FAIL ();
  return NULL;
}

static bool
amiga_add_reloc (
     bfd *abfd,
     sec_ptr section,
     bfd_size_type offset,
     amiga_symbol_type *symbol,
     reloc_howto_type *howto,
     long target_hunk)
{
  amiga_reloc_type *reloc;
  sec_ptr target_sec;

  reloc = (amiga_reloc_type *) bfd_alloc (abfd, sizeof (amiga_reloc_type));
  if (reloc == NULL)
    return false;

  abfd->flags |= HAS_RELOC;
  section->flags |= SEC_RELOC;

  if (amiga_per_section(section)->reloc_tail)
    amiga_per_section(section)->reloc_tail->next = reloc;
  else
    section->relocation = &reloc->relent;
  amiga_per_section(section)->reloc_tail = reloc;

  reloc->relent.sym_ptr_ptr = &reloc->symbol;
  reloc->relent.address = offset;
  reloc->relent.addend = 0;
  reloc->relent.howto = howto;

  reloc->next = NULL;
  if (symbol == NULL)
    { /* relative to section */
    target_sec = amiga_get_section_by_hunk_number (abfd, target_hunk);
    if (target_sec)
	  reloc->symbol = target_sec->symbol;
    else
      return false;
  }
  else
      reloc->symbol = &symbol->symbol;

  return true;
}

/* BFD doesn't currently allow multiple sections with the same
   name, so we try a little harder to get a unique name.  */
sec_ptr
amiga_make_unique_section (
     bfd *abfd,
     const char *name)
{
  sec_ptr section;

  bfd_set_error (bfd_error_no_error);
  section = bfd_make_section (abfd, name);
  if ((section == NULL) && (bfd_get_error() == bfd_error_no_error))
    {
#if 0
      char *new_name = bfd_alloc (abfd, strlen(name) + 4);
      int i = 1;

      /* We try to come up with an original name (since BFD currently
	 requires all sections to have different names).  */
      while (!section && (i<=99))
	{
	  sprintf (new_name, "%s_%u", name, i++);
	  section = bfd_make_section (abfd, new_name);
	}
#else
      section = bfd_make_section_anyway (abfd, name);
#endif
    }
  return section;
}

#if DEBUG_AMIGA
#define DPRINTHUNK(x) fprintf(stderr,"Processing %s hunk (0x%x)...",\
	(x) == HUNK_UNIT ? "HUNK_UNIT" :\
	(x) == HUNK_NAME ? "HUNK_NAME" :\
	(x) == HUNK_CODE ? "HUNK_CODE" :\
	(x) == HUNK_DATA ? "HUNK_DATA" :\
	(x) == HUNK_BSS ? "HUNK_BSS" :\
	(x) == HUNK_ABSRELOC32 ? "HUNK_RELOC32" :\
	(x) == HUNK_RELRELOC16 ? "HUNK_RELRELOC16" :\
	(x) == HUNK_RELRELOC8 ? "HUNK_RELRELOC8" :\
	(x) == HUNK_EXT ? "HUNK_EXT" :\
	(x) == HUNK_SYMBOL ? "HUNK_SYMBOL" :\
	(x) == HUNK_DEBUG ? "HUNK_DEBUG" :\
	(x) == HUNK_END ? "HUNK_END" :\
	(x) == HUNK_HEADER ? "HUNK_HEADER" :\
	(x) == HUNK_OVERLAY ? "HUNK_OVERLAY" :\
	(x) == HUNK_BREAK ? "HUNK_BREAK" :\
	(x) == HUNK_DREL32 ? "HUNK_DREL32" :\
	(x) == HUNK_DREL16 ? "HUNK_DREL16" :\
	(x) == HUNK_DREL8 ? "HUNK_DREL8" :\
	(x) == HUNK_LIB ? "HUNK_LIB" :\
	(x) == HUNK_INDEX ? "HUNK_INDEX" :\
	(x) == HUNK_RELOC32SHORT ? "HUNK_RELOC32SHORT" :\
	(x) == HUNK_RELRELOC32 ? "HUNK_RELRELOC32" :\
	(x) == HUNK_PPC_CODE ? "HUNK_PPC_CODE" :\
	(x) == HUNK_RELRELOC26 ? "HUNK_RELRELOC26" :\
	"*unknown*",(x))
#define DPRINTHUNKEND fprintf(stderr,"done\n")
#else
#define DPRINTHUNK(x)
#define DPRINTHUNKEND
#endif

static bool
parse_archive_units (
     bfd *abfd,
     int *n_units,
     file_ptr filesize,
     bool one,			/* parse only the first unit? */
     struct arch_syms **syms,
     symindex *symcount)
{
  struct arch_syms *nsyms,*syms_tail=NULL;
  unsigned long unit_offset = (unsigned long)-1,defsym_pos=0;
  unsigned long hunk_type,type,len,no,n;
  symindex defsymcount=0;
  char * last_name = 0;
  bfd_vma stab_pos = 0;
  bfd_vma stabstr_pos = 0;
  unsigned stab_size = 0, stabstr_size = 0;

  *n_units = 0;
  while (get_long (abfd, &hunk_type))
    {
      switch (HUNK_VALUE(hunk_type))
	{
    case HUNK_END:
      break;
    case HUNK_UNIT:
      unit_offset = bfd_tell (abfd) - 4;
      (*n_units)++;
	  if (one && *n_units > 1)
	    {
	bfd_seek (abfd, -4, SEEK_CUR);
	return true;
      }
	  stab_pos = stabstr_pos = 0;
	  /* fall through */
    case HUNK_NAME:
	  if (!get_long (abfd, &len))
	    return false;

	  /* check for .stab and .stabstr. */
	  if (len <= 16)
	    {
	      static char name[16*4 + 1];
	      if (bfd_bread (name, len<<2, abfd) != len<<2)
		return false;
	      name[len<<2] = 0;
	      last_name = name;
#ifdef DEBUG_AMIGA
	      if (HUNK_VALUE(hunk_type) == HUNK_UNIT)
		DPRINT(10, ("unit: %s\n", last_name));
#endif
	    }
	  else if (bfd_seek (abfd, HUNK_VALUE (len) << 2, SEEK_CUR))
	    return false;
	  break;
    case HUNK_CODE:
    case HUNK_DATA:
    case HUNK_PPC_CODE:
	  last_name = 0;
	  if (!get_long (abfd, &len) || bfd_seek (abfd, HUNK_VALUE (len) << 2, SEEK_CUR))
	    return false;
	  break;
	case HUNK_DEBUG:
	  if (!get_long (abfd, &len))
	    return false;
	  if (last_name)
	    {
	      if (0 == strcmp (".stab", last_name))
		{
		  stab_size = HUNK_VALUE (len) << 2;
		  stab_pos = bfd_tell (abfd);
		}
	      else if (0 == strcmp (".stabstr", last_name))
		{
		  stabstr_size = HUNK_VALUE (len) << 2;
		  stabstr_pos = bfd_tell (abfd);
		}
	      last_name = 0;

	      /*
	       * Create arch_syms for the stab entries.
	       */
	      if (stab_pos && stabstr_pos)
		{
		  bfd_vma last_pos = bfd_tell (abfd);
		  unsigned char * stabstrdata = (unsigned char *) bfd_alloc (abfd, stabstr_size);
		  if (!stabstrdata)
		    return false;
		  if (bfd_seek (abfd, stabstr_pos, SEEK_SET))
		    return false;
		  if (bfd_bread (stabstrdata, stabstr_size, abfd) != stabstr_size)
		    return false;

		  unsigned char * stabdata = (unsigned char *) bfd_alloc (abfd, stab_size);
		  if (!stabdata)
		    return false;
		  if (bfd_seek (abfd, stab_pos, SEEK_SET))
		    return false;
		  if (bfd_bread (stabdata, stab_size, abfd) != stab_size)
		    return false;

		  {unsigned i; for (i = 0; i < stab_size; i += 12)
		    {
		      unsigned str_offset = bfd_getb32 (stabdata + i);
		      char ctype = stabdata[i + 4];

		      switch (ctype)
			{
			default:
			  continue;

			case N_WARNING:
			case N_INDR | N_EXT:
			  i += 12; // skip next
			  break;
			}

		      if (str_offset > stabstr_size)
	return false;

		      nsyms = (struct arch_syms *) bfd_alloc (abfd, sizeof(struct arch_syms));
		      nsyms->next = NULL;
		      if (syms_tail)
			syms_tail->next = nsyms;
		      else
			*syms = nsyms;
		      syms_tail = nsyms;

		      nsyms->offset = stab_pos + 1;
		      nsyms->size = 12;
		      nsyms->unit_offset = unit_offset;

		      nsyms->name = (char *) stabstrdata + str_offset;
		      DPRINT(20,("sym: %s\n", nsyms->name));
		      ++defsymcount;
		    }}
		  bfd_seek (abfd, last_pos, SEEK_SET);
		  stab_pos = stabstr_pos = 0;
		}
	    }

	  if (bfd_seek (abfd, HUNK_VALUE (len) << 2, SEEK_CUR))
	    return false;
      break;
    case HUNK_BSS:
      if (!get_long (abfd, &len))
	return false;
      break;
    case HUNK_ABSRELOC32:
    case HUNK_RELRELOC16:
    case HUNK_RELRELOC8:
    case HUNK_SYMBOL:
    case HUNK_DREL32:
    case HUNK_DREL16:
    case HUNK_DREL8:
    case HUNK_RELRELOC32: /* 32bit data in an object file! */
    case HUNK_ABSRELOC16:
    case HUNK_RELRELOC26:
	  for (;;)
	    {
	/* read offsets count */
	if (!get_long (abfd, &no))
	  return false;
	if (!no)
	  break;
	/* skip hunk+offsets */
	if (bfd_seek (abfd, (no+1)<<2, SEEK_CUR))
	  return false;
      }
      break;
    case HUNK_RELOC32SHORT:
	  for (;;)
	    {
	/* read offsets count */
	if (!get_word (abfd, &no))
	  return false;
	if (!no)
	  break;
	/* skip hunk+offsets */
	if (bfd_seek (abfd, (no+1)<<1, SEEK_CUR))
	  return false;
      }
      if ((bfd_tell (abfd) & 2) && bfd_seek (abfd, 2, SEEK_CUR))
        return false;
      break;
    case HUNK_EXT:
      if (!get_long (abfd, &n))
	return false;
	  while (n)
	    {
	len = n & 0xffffff;
	type = (n>>24) & 0xff;
	if (type < 200) type &= ~0x60;
	      switch (type)
		{
	case EXT_SYMB:
	case EXT_DEF:
	case EXT_ABS:
	  /* retain the positions of defined symbols for each object
	     in the archive. They'll be used later to build a
	     pseudo-armap, which _bfd_generic_link_add_archive_symbols
	     needs */
	    defsym_pos = bfd_tell (abfd) - 4;
	  /* skip name & value */
	  if (bfd_seek (abfd, (len+1)<<2, SEEK_CUR))
	    return false;
	  break;

	case EXT_ABSREF32:
	case EXT_RELREF16:
	case EXT_RELREF8:
	case EXT_DEXT32:
	case EXT_DEXT16:
	case EXT_DEXT8:
	case EXT_RELREF32:
	case EXT_ABSREF16:
	case EXT_ABSREF8:
	case EXT_RELREF26:
	  /* skip name */
	  if (bfd_seek (abfd, len<<2, SEEK_CUR))
	    return false;
	  /* skip references */
	  if (!get_long (abfd, &no))
	    return false;
	  if (no && bfd_seek (abfd, no<<2, SEEK_CUR))
	    return false;
	  break;

	case EXT_ABSCOMMON:
		  defsym_pos = bfd_tell (abfd) - 4;
		  // fall through
	case EXT_RELCOMMON:
	case EXT_DEXT32COMMON:
	case EXT_DEXT16COMMON:
	case EXT_DEXT8COMMON:
	  /* skip name & value */
	  if (bfd_seek (abfd, (len+1)<<2, SEEK_CUR))
	    return false;
	  /* skip references */
	  if (!get_long (abfd, &no))
	    return false;
	  if (no && bfd_seek (abfd, no<<2, SEEK_CUR))
	    return false;
	  break;

	default: /* error */
		  bfd_msg ("unexpected type %ld(0x%lx) in hunk_ext1 at offset 0x%lx", type, type, (long unsigned int)bfd_tell (abfd));
	  return false;
	}

		if (defsym_pos != 0 && syms)
		  {
	/* there are some defined symbols, keep enough information on
	   them to simulate an armap later on */
	nsyms = (struct arch_syms *) bfd_alloc (abfd, sizeof (struct arch_syms));
	nsyms->next = NULL;
	if (syms_tail)
	  syms_tail->next = nsyms;
	else
	  *syms = nsyms;
	syms_tail = nsyms;
	nsyms->offset = defsym_pos;
	nsyms->size = bfd_tell (abfd) - defsym_pos;
	nsyms->unit_offset = unit_offset;
		    nsyms->name = 0;

		    ++defsymcount;
		    defsym_pos = 0;
      }

	      if (!get_long (abfd, &n))
		return false;
	    }
      break; /* of HUNK_EXT */

    default:
#if 0
      bfd_msg ("unexpected hunk 0x%lx at offset 0x%lx",
	       hunk_type, bfd_tell (abfd));
#endif
      return false;
    }
  }
  if (syms && symcount)
    *symcount = defsymcount;
  return (bfd_tell (abfd) == filesize);
}

static bool amiga_digest_file (
  bfd *abfd)
{
  ufile_ptr file_size;
  unsigned long tmp;

  if (!get_long (abfd, &tmp))
    {
      bfd_set_error (bfd_error_wrong_format);
      return false;
    }

  switch (HUNK_VALUE (tmp))
    {
    case HUNK_UNIT:
      /* Read the unit(s) */
      file_size = bfd_get_file_size(abfd);
/*
      while ((pos=bfd_tell (abfd)) < stat_buffer.st_size)
	{*/
      if (!amiga_read_unit (abfd, file_size))
	return false;
      if (abfd->arelt_data)
	arelt_size (abfd) = bfd_tell (abfd);
/*	}*/
      break;

    case HUNK_HEADER:
      /* This is a load file */
      if (!amiga_read_load (abfd))
	return false;
      break;
    }

  return true;
}/* of amiga_digest_file */


/* Read in Unit file */
/* file pointer is located after the HUNK_UNIT LW */
static bool
amiga_read_unit (
     bfd *abfd,
     file_ptr size)
{
  unsigned long hunk_number=0,hunk_type,tmp;

  /* read LW length of unit's name */
  if (!get_long (abfd, &tmp))
    return false;

  /* and skip it (FIXME maybe) */
  if (bfd_seek (abfd, tmp<<2, SEEK_CUR))
    return false;

  while (bfd_tell (abfd) < size)
    {
      if (!get_long (abfd, &hunk_type))
	return false;

      /* Now there may be CODE, DATA, BSS, SYMBOL, DEBUG, RELOC Hunks */
      switch (HUNK_VALUE (hunk_type))
	{
	case HUNK_UNIT:
	  /* next unit, seek back and return */
	  return (bfd_seek (abfd, -4, SEEK_CUR) == 0);

	case HUNK_DEBUG:
	  /* we don't parse hunk_debug at the moment */
	  if (!get_long (abfd, &tmp) || bfd_seek (abfd, tmp<<2, SEEK_CUR))
	    return false;
	  break;

	case HUNK_NAME:
	case HUNK_CODE:
	case HUNK_DATA:
	case HUNK_BSS:
	case HUNK_PPC_CODE:
	  /* Handle this hunk, including relocs, etc.
	     The finishing HUNK_END is consumed by the routine */
	  if (!amiga_handle_cdb_hunk (abfd, hunk_type, hunk_number++, 0, -1))
	    return false;
	  break;

	case HUNK_END:
		break;

	default:
	  /* Something very nasty happened: invalid hunk occured... */
	  bfd_set_error (bfd_error_wrong_format);
	  return false;
	  break;
	}/* Of switch hunk_type */

      /* Next hunk */
    }
  return true;
}

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wstack-usage="
#endif
/* Read a load file */
static bool
amiga_read_load (
  bfd *abfd)
{
  unsigned long *hunk_attributes,*hunk_sizes;
  unsigned long max_hunk_number,hunk_type,i;
  char buf[16];

  /* Read hunk lengths (and memory attributes...) */
  /* Read in each hunk */

  if (bfd_bread (buf, sizeof(buf), abfd) != sizeof(buf))
    return false;

  /* If there are resident libs: abort (obsolete feature) */
  if (GL (&buf[0]) != 0)
    return false;

  max_hunk_number = GL (&buf[4]);

  /* Sanity */
  if (max_hunk_number<1)
    {
      bfd_set_error (bfd_error_wrong_format);
      return false;
    }

  AMIGA_DATA(abfd)->nb_hunks = max_hunk_number;

  /* Num of root hunk must be 0 */
  if (GL (&buf[8]) != 0)
    {
      bfd_set_error (bfd_error_wrong_format);
      return false;
    }

  /* Num of last hunk must be mhn-1 */
  if (GL (&buf[12]) != max_hunk_number-1)
    {
      bfd_msg ("Overlay loadfiles are not supported");
      bfd_set_error (bfd_error_wrong_format);
      return false;
    }

  hunk_sizes = alloca (max_hunk_number * sizeof (unsigned long));
  hunk_attributes = alloca (max_hunk_number * sizeof (unsigned long));
  if (hunk_sizes == NULL || hunk_attributes == NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }

  /* Now, read in sizes and memory attributes */
  for (i=0; i<max_hunk_number; i++)
    {
      if (!get_long (abfd, &hunk_sizes[i]))
	return false;
      switch (HUNK_ATTRIBUTE (hunk_sizes[i]))
	{
	case HUNK_ATTR_CHIP:
	  hunk_attributes[i] = MEMF_CHIP;
	  break;
	case HUNK_ATTR_FAST:
	  hunk_attributes[i] = MEMF_FAST;
	  break;
	case HUNK_ATTR_FOLLOWS:
	  if (!get_long (abfd, &hunk_attributes[i]))
	    return false;
	  break;
	default:
	  hunk_attributes[i] = 0;
	  break;
	}
      hunk_sizes[i] = HUNK_VALUE (hunk_sizes[i]) << 2;
    }

  for (i=0; i<max_hunk_number; i++)
    {
      if (!get_long (abfd, &hunk_type))
	return false;

      /* This may be HUNK_NAME, CODE, DATA, BSS, DEBUG */
      switch (HUNK_VALUE (hunk_type))
	{
	case HUNK_NAME:
	case HUNK_CODE:
	case HUNK_DATA:
	case HUNK_BSS:
	case HUNK_PPC_CODE:
	  if (!amiga_handle_cdb_hunk (abfd, hunk_type, i,
				      hunk_attributes[i], hunk_sizes[i]))
	    {
	      bfd_set_error (bfd_error_wrong_format);
	      return false;
	    }
	  break;

	case HUNK_DEBUG:
	  if (--i,!amiga_handle_cdb_hunk (abfd, hunk_type, -1, 0, 0))
	    {
	      bfd_set_error (bfd_error_wrong_format);
	      return false;
	    }
	  break;

	default:
	  /* invalid hunk */
	  bfd_set_error (bfd_error_wrong_format);
	  return false;
	  break;
	}/* Of switch */
    }

  /* read stab  */
  if (get_long (abfd, &hunk_type) && hunk_type == HUNK_DEBUG)
    {
      unsigned long sz;
      if (get_long (abfd, &sz))
	if (bfd_seek(abfd, -4, SEEK_CUR) >= 0)
	  amiga_handle_cdb_hunk (abfd, hunk_type, -1, 0, sz);
    }

  return true;
}/* Of amiga_read_load */


/* Handle NAME, CODE, DATA, BSS, DEBUG Hunks */
static bool
amiga_handle_cdb_hunk (
     bfd *abfd,
     unsigned long hunk_type,
     unsigned long hunk_number,
     unsigned long hunk_attribute,
     unsigned long hunk_size)
/* If hunk_size==-1, then we are digesting a HUNK_UNIT */
{
  sec_ptr current_section;
  char *sec_name = "",*current_name=NULL;
  unsigned long len,tmp;
  int secflags,is_load=(hunk_size!=(unsigned long)-1);

  if (HUNK_NAME == HUNK_VALUE (hunk_type)) /* get name */
    {
      if (!get_long (abfd, &tmp))
	return false;

      len = HUNK_VALUE (tmp) << 2;
      if (len != 0)
	{
	  current_name = bfd_alloc (abfd, len+1);
	  if (!current_name)
	    return false;

	  if (bfd_bread (current_name, len, abfd) != len)
	    return false;

	  current_name[len] = '\0';
	  if (current_name[0] == '\0')
	    {
	       bfd_release (abfd, current_name);
	       current_name = NULL;
	    }
	}

      if (!get_long (abfd, &hunk_type))
	return false;
    }

  /* file_pointer is now after hunk_type */
  secflags = 0;
  switch (HUNK_VALUE (hunk_type))
    {
    case HUNK_CODE:
    case HUNK_PPC_CODE:
      secflags = SEC_ALLOC | SEC_LOAD | SEC_CODE | SEC_HAS_CONTENTS;
      sec_name = ".text";
      goto do_section;

    case HUNK_DATA:
      secflags = SEC_ALLOC | SEC_LOAD | SEC_DATA | SEC_HAS_CONTENTS;
      sec_name = ".data";
      goto do_section;

    case HUNK_BSS:
      secflags = SEC_ALLOC;
      sec_name = ".bss";

    do_section:
      if (!current_name)
	current_name = sec_name;
      if (!get_long (abfd, &tmp))
	return false;
      len = HUNK_VALUE (tmp) << 2; /* Length of section */
      if (!is_load)
	{
	  hunk_attribute=HUNK_ATTRIBUTE (len);
	  hunk_attribute=(hunk_attribute==HUNK_ATTR_CHIP)?MEMF_CHIP:
			 (hunk_attribute==HUNK_ATTR_FAST)?MEMF_FAST:0;
	}

      /* Make new section */
      current_section = amiga_make_unique_section (abfd, current_name);
      if (!current_section)
	return false;

      current_section->filepos = bfd_tell (abfd);
      /* For a loadfile, the section size in memory comes from the
	 hunk header. The size on disk may be smaller. */
      current_section->size = current_section->rawsize =
	((hunk_size==(unsigned long)-1) ? len : hunk_size);
      current_section->target_index = hunk_number;
      bfd_set_section_flags (current_section, secflags);

      amiga_per_section(current_section)->disk_size = len; /* size on disk */
      amiga_per_section(current_section)->attribute = hunk_attribute;

      /* SBF: reserve symbol space for parsed stabs. */
      if (HUNK_VALUE (hunk_type) == HUNK_DEBUG && current_name && (0 == strcmp (current_name, ".stab")))
	{
	  /* read the content and scan for stabs to create BSF_CONSTRUCTORS. */
	  char buf[12];
	  while (len > 0)
	    {
	      len -= sizeof(buf);
	      if (bfd_bread (buf, sizeof(buf), abfd) != sizeof(buf))
		return false;

	      switch (buf[4])
		{
		case N_INDR | N_EXT:
		case N_WARNING:
//		  /* always take the next symbol. */
//		  len -= sizeof(buf);
//		  if (len > amiga_per_section(current_section)->disk_size || bfd_bread (buf, sizeof(buf), abfd) != sizeof(buf))
//		    return false;
//		  abfd->symcount++;
//
//		  /* fallthrough */
		case N_UNDF | N_EXT:
		case N_ABS | N_EXT:
		case N_TEXT | N_EXT:
		case N_DATA | N_EXT:
		case N_SETV | N_EXT:
		case N_BSS | N_EXT:
		case N_COMM | N_EXT:
		case N_SETA:
		case N_SETA | N_EXT:
		case N_SETT:
		case N_SETT | N_EXT:
		case N_SETD:
		case N_SETD | N_EXT:
		case N_SETB:
		case N_SETB | N_EXT:
		case N_WEAKU:
		case N_WEAKA:
		case N_WEAKT:
		case N_WEAKD:
		case N_WEAKB:
		  abfd->symcount++;
		  break;
		}
	    }

	  /* We have symbols */
	  if (abfd->symcount)
	    abfd->flags |= HAS_SYMS;
	}
      else
      /* skip the contents */
      if ((secflags & SEC_HAS_CONTENTS) && bfd_seek (abfd, len, SEEK_CUR))
	return false;

      if (!amiga_handle_rest (abfd, current_section, is_load))
	return false;
      break;

      /* Currently, there is one debug hunk per executable, instead of one
	 per unit as it would with a "standard" AmigaOS implementation. So
	 the debug hunk is at the same level as code/data/bss.
	 This will change in the future */
    case HUNK_DEBUG:
      /* handle .stab and .stabs as real sections. */
      if (current_name && (0 == strcmp (current_name, ".stab") || 0 == strcmp (current_name, ".stabstr")
	  || 0 == strncmp (current_name, ".debug_", 7)))
	{
	  secflags = SEC_HAS_CONTENTS;
	  goto do_section;
	}

      /* format of gnu debug hunk is:
	  HUNK_DEBUG
	      N
	    ZMAGIC
	  symtabsize
	  strtabsize
	  symtabdata  [length=symtabsize]
	  strtabdata  [length=strtabsize]
	  [pad bytes]
	  */

      /* read LW length */
      if (!get_long (abfd, &tmp))
	return false;
      len = tmp << 2;
      if (len > 12)
	{
	  char buf[12];
	  if (bfd_bread (buf, sizeof(buf), abfd) != sizeof(buf))
	    return false;
	  if (GL (&buf[0]) == ZMAGIC) /* GNU DEBUG HUNK */
	    {
	      amiga_data_type *amiga_data=AMIGA_DATA(abfd);
	      /* FIXME: we should add the symbols in the debug hunk to symtab... */
	      /* SBF: no, we should not. */
	      amiga_data->symtab_size = GL (&buf[4]);
	      amiga_data->stringtab_size = GL (&buf[8]);
	      adata(abfd).sym_filepos = bfd_tell (abfd);
	      adata(abfd).str_filepos = adata(abfd).sym_filepos + amiga_data->symtab_size;

	      /* SBF: the next sections are needed by gdb. */
	      /* Also make .stab section */
	      current_section = amiga_make_unique_section (abfd, ".stab");
	      if (!current_section)
	    	  return false;

	      current_section->filepos = adata(abfd).sym_filepos;
	      current_section->size = amiga_data->symtab_size;
	      current_section->target_index = hunk_number;
	      bfd_set_section_flags (current_section, SEC_ALLOC | SEC_DEBUGGING | SEC_HAS_CONTENTS);

	      amiga_per_section(current_section)->disk_size = amiga_data->symtab_size; /* size on disk */
	      amiga_per_section(current_section)->attribute = 0;

	      /* Also make .stabstr section */
	      current_section = amiga_make_unique_section (abfd, ".stabstr");
	      if (!current_section)
	    	  return false;

	      current_section->filepos = adata(abfd).str_filepos;
	      current_section->size = amiga_data->stringtab_size;
	      current_section->target_index = hunk_number + 1;
	      bfd_set_section_flags (current_section, SEC_ALLOC | SEC_DEBUGGING | SEC_HAS_CONTENTS);

	      amiga_per_section(current_section)->disk_size = amiga_data->stringtab_size; /* size on disk */
	      amiga_per_section(current_section)->attribute = 0;


	    }
	  len -= sizeof(buf);
	}
      if (bfd_seek (abfd, len, SEEK_CUR))
	return false;
      break;

    default:
      bfd_set_error (bfd_error_wrong_format);
      return false;
      break;
    }/* switch (hunk_type) */

  return true;
}/* Of amiga_handle_cdb_hunk */


/* Handle rest of a hunk
   I.e.: Relocs, EXT, SYMBOLS... */
static bool
amiga_handle_rest (
     bfd *abfd,
     sec_ptr current_section,
     bool isload)
{
  amiga_per_section_type *asect=amiga_per_section(current_section);
  unsigned long hunk_type,hunk_value,relno,type,len,no;
  raw_reloc_type *relp;

  for (relno=0;;)
    {
      if (!get_long (abfd, &hunk_type))
	return false;
      hunk_value = HUNK_VALUE (hunk_type);
      switch (hunk_value)
	{
	case HUNK_END:
	  if (relno)
	    {
	      abfd->flags |= HAS_RELOC;
	      current_section->flags |= SEC_RELOC;
	      current_section->reloc_count = relno;
	    }
	  return true;
	  break;

	case HUNK_ABSRELOC32:
	case HUNK_RELRELOC16:
	case HUNK_RELRELOC8:
	case HUNK_DREL32:
	case HUNK_DREL16:
	case HUNK_DREL8:
	case HUNK_RELOC32SHORT:
	case HUNK_RELRELOC32:
	case HUNK_ABSRELOC16:
	case HUNK_RELRELOC26:
	  /* count and skip relocs */
	  relp = (raw_reloc_type *) bfd_alloc (abfd, sizeof (*relp));
	  relp->next = asect->relocs;
	  asect->relocs = relp;
	  relp->pos = bfd_tell (abfd) - 4;
	  relp->num = 0;
	  if (amiga_reloc_long_p (hunk_value, isload))
	    {
	      for (;;)
		{
	      if (!get_long (abfd, &no))
		return false;
	      if (!no)
		break;
	      relp->num += no;
	      if (!get_long (abfd, &relp->hunk))
		return false;
	      if (bfd_seek (abfd, (no)<<2, SEEK_CUR))
		return false;
	    }
	  }
	  else
	    {
	      for (;;)
		{
	      if (!get_word (abfd, &no))
		return false;
	      if (!no)
		break;
	      relp->num += no;
	      if (!get_word(abfd, &relp->hunk))
		return false;
	      if (bfd_seek (abfd, (no)<<1, SEEK_CUR))
		return false;
	    }
	    if ((bfd_tell (abfd) & 2) && bfd_seek (abfd, 2, SEEK_CUR))
	      return false;
	  }
	  relno += relp->num;
	  break;

	case HUNK_SYMBOL:
	  /* In a unit, we ignore these, since all symbol information
	     comes with HUNK_EXT, in a load file, these are added */
	  if (!isload)
	    {
	      asect->hunk_symbol_pos = bfd_tell (abfd);
	      for (;;)
		{
		/* size of symbol */
		if (!get_long (abfd, &no))
		  return false;
		if (!no)
		  break;
		/* skip the name */
		if (bfd_seek (abfd, (no+1)<<2, SEEK_CUR))
		  return false;
	      }
	      break;
	    }
	  /* We add these, by falling through... */
	  /* fall through */
	case HUNK_EXT:
	  /* We leave these alone, until they are requested by the user */
	  asect->hunk_ext_pos = bfd_tell (abfd);
	  for (;;)
	    {
	      aname_list_type * nlt;
	      if (!get_long (abfd, &no))
		return false;
	      if (!no)
		break;

	      /* symbol type and length */
	      type = (no>>24) & 0xff;
	      len = no & 0xffffff;

	      /* read symbol name */
	      nlt = (aname_list_type *)bfd_zalloc(abfd, sizeof(aname_list_type) + (len << 2) + 1);
	      if (bfd_bread (nlt + 1, len<<2, abfd) != len << 2)
		return false;

	      nlt->name = (char const *)(nlt + 1);
	      nlt->next = asect->sym_names;
	      asect->sym_names = nlt;

	      /* We have symbols */
	      abfd->flags |= HAS_SYMS;
	      abfd->symcount++;

		if (type < 200) type &= ~0x60;
	      switch (type)
		{
		case EXT_SYMB: /* Symbol hunks are relative to hunk start... */
		case EXT_DEF: /* def relative to hunk */
		case EXT_ABS: /* def absolute */
		  /* skip the value */
		  if (!get_long (abfd, &no))
		    return false;
		  break;

		case EXT_ABSCOMMON: /* Common ref/def */
		case EXT_RELCOMMON:
		case EXT_DEXT32COMMON:
		case EXT_DEXT16COMMON:
		case EXT_DEXT8COMMON:
		  /* FIXME: skip the size of common block */
		  if (!get_long (abfd, &no))
		    return false;

		  /* Fall through */

		case EXT_ABSREF32: /* 32 bit ref */
		case EXT_RELREF16: /* 16 bit ref */
		case EXT_RELREF8: /* 8 bit ref */
		case EXT_DEXT32: /* 32 bit baserel */
		case EXT_DEXT16: /* 16 bit baserel */
		case EXT_DEXT8: /* 8 bit baserel */
		case EXT_RELREF32:
		case EXT_ABSREF16:
		case EXT_ABSREF8:
		case EXT_RELREF26:
		  if (!get_long (abfd, &no))
		    return false;
		  if (no)
		    {
		      relno += no;
		      /* skip references */
		      if (bfd_seek (abfd, no<<2, SEEK_CUR))
			return false;
		    }
		  break;

		default: /* error */
		  bfd_msg ("unexpected type %ld(0x%lx) in hunk_ext2 at offset 0x%lx", type, type, (long unsigned int)bfd_tell (abfd));
		  bfd_set_error (bfd_error_wrong_format);
		  return false;
		  break;
		}/* of switch type */
	    }
	  break;

	case HUNK_DEBUG:
	  /* If a debug hunk is found at this position, the file has
	     been generated by a third party tool and the debug info
	     here are useless to us. Just skip the hunk, then. */
	  if (!get_long (abfd, &no) || bfd_seek (abfd, no<<2, SEEK_CUR))
	    return false;
	  break;

	default: /* error */
	  bfd_seek (abfd, -4, SEEK_CUR);
	  bfd_msg ("HUNK_END missing: unexpected hunktype %ld(0x%lx) at offset 0x%lx", hunk_type, hunk_type,
		   (long unsigned int)bfd_tell (abfd));
	  switch (hunk_value)
	    {
	    default:
	      bfd_set_error (bfd_error_wrong_format);
	      return false;
	    case HUNK_CODE:
	    case HUNK_DATA:
	    case HUNK_BSS:
	      return true;
	  }
	  break;
	}/* Of switch */
    }/* Of for */
  return true;
}/* of amiga_handle_rest */

static bool
amiga_mkobject (
  bfd *abfd)
{
  amiga_data_type *rawptr;
  rawptr = (amiga_data_type *) bfd_zalloc (abfd, sizeof (*rawptr));
  abfd->tdata.amiga_data = rawptr;
  return (rawptr!=NULL);
}

static bool
amiga_mkarchive (
  bfd *abfd)
{
  amiga_ardata_type *ar;
  ar = (amiga_ardata_type *) bfd_zalloc (abfd, sizeof (*ar));
  amiga_ardata (abfd) = ar;
  return (ar!=NULL);
}

/* write nb long words (possibly swapped out) to the output file */
static bool
write_longs (
     const unsigned long *in,
     unsigned long nb,
     bfd *abfd)
{
  unsigned char out[10*4];
  unsigned long i;

  while (nb)
    {
      for (i=0; i<nb && i<10; in++,i++)
        bfd_putb32 (in[0], &out[i*4]);
      if (bfd_bwrite ((void *)out, 4*i, abfd) != 4*i)
	return false;
      nb -= i;
    }
  return true;
}

static bool
write_words (
     const unsigned long *in,
     unsigned long nb,
     bfd *abfd)
{
  unsigned char out[10*2];
  unsigned long i;

  while (nb)
    {
      for (i=0; i<nb && i<10; in++,i++)
        bfd_putb16 (in[0], &out[i*2]);
      if (bfd_bwrite ((void *)out, 2*i, abfd) != 2*i)
	return false;
      nb -= i;
    }
  return true;
}

static long
determine_datadata_relocs (
     bfd *abfd ATTRIBUTE_UNUSED,
     sec_ptr section)
{
  sec_ptr insection;
  asymbol *sym_p;
  unsigned int i;
  long relocs=1;

  for (i=0;i<section->reloc_count;i++)
    {
      arelent *r=section->orelocation[i];
      if (r == NULL)
	continue;
      sym_p=*(r->sym_ptr_ptr); /* The symbol for this relocation */
      insection=sym_p->section;

      /* Is reloc relative to a special section? */
      if (bfd_is_special_section(insection))
	continue; /* Nothing to do, since this translates to HUNK_EXT */
      if (insection->output_section == section)
	relocs++;
    }
  return relocs;
}

/* Adjust the indices map when we decide not to output the section <sec> */
static void
remove_section_index (
     sec_ptr sec,
     int *index_map)
{
  int i=sec->index;
  for (sec=sec->next,index_map[i++]=-1; sec; sec=sec->next)
    (index_map[i++])--;
}

/* Write out the contents of a bfd */
static bool
amiga_write_object_contents (
  bfd *abfd)
{
  long datadata_relocs=0,bss_size=0,idx;
  int *index_map,max_hunk=-1;
  sec_ptr data_sec = 0, p, q, stab = 0, stabstr = 0;
  unsigned long n[5];

  /* Distinguish UNITS, LOAD Files
    Write out hunks+relocs+HUNK_EXT+HUNK_DEBUG (GNU format) */
  DPRINT(5,("Entering write_object_conts\n"));

  abfd->output_has_begun=true; /* Output has begun */

  if (AMIGA_DATA(abfd)->IsLoadFile)
    {
      // remove .stab and .stabstr
      for (q = abfd->sections, p = q ? q->next : 0; p; p = p->next)
	{
	  if (0 == strcmp (p->name, ".stab"))
	    {
	      stab = p;
	      q->next = p->next;
	      if (!p->next)
		break;
	      continue;
	    }
	  if (0 == strcmp (p->name, ".stabstr"))
	    {
	      stabstr = p;
		  q->next = p->next;
	      if (!p->next)
	      break;
	      continue;
	    }
	  q = p;
	}
      // q points to last section - patch stab section and append
      if (write_debug_hunk && stab && stabstr)
	{
	  unsigned total = 12 + stab->rawsize + stabstr->rawsize;
	  unsigned char * data = (unsigned char *)bfd_alloc(abfd, total);
	  bfd_putb32(ZMAGIC, data);
	  bfd_putb32(stab->rawsize, data + 4);
	  bfd_putb32(stabstr->rawsize, data + 8);
	  memcpy(data + 12, stab->contents, stab->rawsize);
	  memcpy(data + 12 + stab->rawsize, stabstr->contents, stabstr->rawsize);
	  stab->rawsize = stab->size = total;
	  stab->contents = data;
	  q->next = stab;
	  stab->next = 0;
	}
      int nn = 0;
      for (p = abfd->sections; p != NULL; p = p->next)
	p->index = nn++;
    }

  index_map = bfd_alloc (abfd, abfd->section_count * sizeof (int));
  if (!index_map)
    return false;

  for (idx=0, p=abfd->sections; p!=NULL; p=p->next)
    index_map[idx++] = p->index;

  /* Distinguish Load files and Unit files */
  if (AMIGA_DATA(abfd)->IsLoadFile)
    {
      DPRINT(5,("Writing load file\n"));

      /* no longer true - a base relative executable may have a chip,far and stab/stabstr sections */
//      if (amiga_base_relative)
//	BFD_ASSERT (abfd->section_count==3);
      /* Write out load file header */
      n[0] = HUNK_HEADER;
      n[1] = n[2] = 0;
      for (p = abfd->sections; p != NULL; p = p->next)
	{
	/* For baserel linking, don't remove empty sections, since they
	   may get some contents later on */
	  if ((amiga_base_relative || p->rawsize != 0 || p->size != 0)
	      && !(amiga_base_relative && !strcmp (p->name, ".bss")))
	    {
	      /* don't count debug sections. */
	      if (strcmp (p->name, ".stab") && strncmp(p->name, ".debug_", 7))
		n[2]++;
	    }
	else
	  remove_section_index (p, index_map);
      }
      n[3] = 0;
      n[4] = n[2] > 0 ? n[2]-1 : 0;
      if (!write_longs (n, 5, abfd))
	return false;

      /* Write out sizes and memory specifiers... */
      /* We have to traverse the section list again, bad but no other way... */
      if (amiga_base_relative)
	{
	for (p=abfd->sections; p!=NULL; p=p->next)
	  {
	    if (amiga_resident && !strcmp(p->name,".data"))
	      {
		datadata_relocs = determine_datadata_relocs (abfd, p);
		data_sec = p;
	      }
	    else  // really!!!
	    if (!strcmp(p->name,".bss"))
	      {
		/* Get size for header */
		  bss_size = p->rawsize;
	      }
	  }
      }

      for (p=abfd->sections; p!=NULL; p=p->next)
	{
	  long extra = 0, ii;

	  if (index_map[p->index] < 0)
	    continue;

	  /* don't add debug sections. */
	  if (!strcmp (p->name, ".stab") || !strcmp (p->name, ".stabstr") || !strncmp(p->name, ".debug_", 7))
	    continue;

	  if (datadata_relocs && !strcmp(p->name,".text"))
	    extra = datadata_relocs * 4;
	  else if (bss_size && !strcmp (p->name, ".data"))
	    extra = bss_size;
	  /* convert to a size in long words */
	  n[0] = LONGSIZE(p->rawsize + extra);

	  ii = amiga_per_section(p)->attribute;
	  switch (ii)
	    {
	    case MEMF_CHIP:
	      n[0]|=HUNKF_CHIP;
	      ii = 1;
	      break;
	    case MEMF_FAST:
	      n[0]|=HUNKF_FAST;
	      ii = 1;
	      break;
	    case 0: /* nothing */
	      ii = 1;
	      break;
	    default: /* special one */
	      n[0]|=0xc0000000;
	      n[1] = ii;
	      ii = 2;
	      break;
	    }/* Of switch */

	  if (!write_longs (n, ii, abfd))
	    return false;
	}/* Of for */
    }
  else
    { /* Unit, no base-relative linking here.. */
      DPRINT(5,("Writing unit\n"));

      /* Write out unit header */
      n[0]=HUNK_UNIT;
      if (!write_longs (n, 1, abfd) || !write_name (abfd, abfd->filename, 0))
	return false;

      unsigned i;
      for (i = 0; i < bfd_get_symcount(abfd); i++)
	{
	  asymbol *sym_p = abfd->outsymbols[i];
	  sec_ptr osection = sym_p->section;
	  if (!osection || !bfd_is_com_section(osection->output_section))
	    continue;

	  for (p = abfd->sections; p != NULL; p = p->next)
	    {
	      if (!strcmp (p->name, ".bss"))
		{
		  if (!p->rawsize && !p->size)
		    p->size = sym_p->value;
		  break;
		}
	    }
	  break;
        }

      for (p = abfd->sections; p != NULL; p = p->next)
	{
	  if (p->rawsize == 0 && p->size == 0 && strcmp (".text", p->name))
	    {
	      // remove only if there are no symbols
	      bool remove = true;
	      for (i = 0; i < bfd_get_symcount(abfd); i++)
		{
		  asymbol *sym_p = abfd->outsymbols[i];
		  if (sym_p->section == p)
		    {
		      remove = false;
		      break;
		    }
		}
	      if (remove)
		remove_section_index (p, index_map);
	    }
        }
    }

  /* Compute the maximum hunk number of the ouput file */
  for (p=abfd->sections; p!=NULL; p=p->next)
    max_hunk++;

  /* Write out every section */
  for (p=abfd->sections; p!=NULL; p=p->next)
    {
      if (index_map[p->index] < 0)
	continue;

#define ddrels (datadata_relocs&&!strcmp(p->name,".text")?datadata_relocs:0)
      if (!amiga_write_section_contents (abfd, p, data_sec, ddrels, index_map, max_hunk))
	return false;

      if (!amiga_write_symbols (abfd,p)) /* Write out symbols + HUNK_END */
	return false;
    }/* of for sections */

  /* Write out debug hunk, if requested */
  if (AMIGA_DATA(abfd)->IsLoadFile && write_debug_hunk)
    {
      extern bool
	translate_to_native_sym_flags (bfd*, asymbol*, struct external_nlist*);

      unsigned int offset = 4, symbols = 0, ii;
      unsigned long str_size = 4; /* the first 4 bytes will be replaced with the length */
      asymbol *sym;
      sec_ptr s;

      /* We have to convert all the symbols in abfd to a.out style... */
      if (bfd_get_symcount (abfd))
	{
#define CAN_WRITE_OUTSYM(sym) (sym!=NULL && sym->section && \
				((sym->section->owner && \
				 bfd_get_flavour (sym->section->owner) == \
				 bfd_target_aout_flavour) || \
				 bfd_asymbol_flavour (sym) == \
				 bfd_target_aout_flavour))

	  for (ii = 0; ii < bfd_get_symcount(abfd); ii++)
	    {
	      sym = abfd->outsymbols[ii];
	      /* NULL entries have been written already... */
	      if (CAN_WRITE_OUTSYM (sym))
		{
		  str_size += strlen(sym->name) + 1;
		  symbols++;
		}
	    }

	  if (!symbols)
	    return true;

	  /* Now, set the .text, .data and .bss fields in the tdata struct
	     because translate_to_native_sym_flags needs them... */
	  for (ii = 0, s = abfd->sections; s != NULL; s = s->next)
	    if (!strcmp(s->name,".text"))
	      {
		ii |= 1;
		adata(abfd).textsec=s;
	      }
	    else if (!strcmp(s->name,".data"))
	      {
		ii |= 2;
		adata(abfd).datasec=s;
	      }
	    else if (!strcmp(s->name,".bss"))
	      {
		ii |= 4;
		adata(abfd).bsssec=s;
	      }

	  if (ii != 7) /* section(s) missing... */
	    {
	      bfd_msg ("Missing section, debughunk not written");
	      return true;
	    }

	  /* Write out HUNK_DEBUG, size, ZMAGIC, ... */
	  n[0] = HUNK_DEBUG;
	  n[1] = 3 + ((symbols * sizeof(struct external_nlist) + str_size + 3) >> 2);
	  n[2] = ZMAGIC; /* Magic number */
	  n[3] = symbols * sizeof(struct external_nlist);
	  n[4] = str_size;
	  if (!write_longs (n, 5, abfd))
	    return false;

	  /* Write out symbols */
	  for (ii = 0; ii < bfd_get_symcount(abfd); ii++) /* Translate every symbol */
	    {
	      sym = abfd->outsymbols[ii];
	      if (CAN_WRITE_OUTSYM (sym))
		{
		  amiga_symbol_type *t = amiga_symbol(sym);
		  struct external_nlist data;

		  bfd_h_put_16(abfd, t->desc, data.e_desc);
		  bfd_h_put_8(abfd, t->other, data.e_other);
		  bfd_h_put_8(abfd, t->type, data.e_type);
		  if (!translate_to_native_sym_flags(abfd,sym,&data))
		    {
		      bfd_msg ("Cannot translate flags for %s", sym->name);
		    }
		  bfd_h_put_32(abfd, offset, &data.e_strx[0]); /* Store index */
		  offset += strlen(sym->name) + 1;
		  if (bfd_bwrite ((void *) &data, sizeof(data), abfd) != sizeof(data))
		    return false;
		}
	    }

	  /* Write out strings */
	  if (!write_longs (&str_size, 1, abfd))
	    return false;

	  for (ii = 0; ii < bfd_get_symcount(abfd); ii++)
	    {
	      sym = abfd->outsymbols[ii];
	      if (CAN_WRITE_OUTSYM (sym))
		{
		  size_t len = strlen(sym->name) + 1;

		  /* Write string tab */
		  if (bfd_bwrite (sym->name, len, abfd) != len)
		    return false;
		}
	    }

	  /* Write padding */
	  n[0] = 0;
	  ii = (4 - (str_size & 3)) & 3;
	  if (ii && bfd_bwrite ((void *) n, ii, abfd) != ii)
	    return false;

	  /* write a HUNK_END here to finish the loadfile, or AmigaOS
	     will refuse to load it */
	  n[0] = HUNK_END;
	  if (!write_longs (n, 1, abfd))
	    return false;
	}/* Of if bfd_get_symcount (abfd) */
    }/* Of write out debug hunk */

  bfd_release (abfd, index_map);
  return true;
}

/* Write a string padded to 4 bytes and preceded by it's length in
   long words ORed with <value> */
static bool
write_name (
     bfd *abfd,
     const char *name,
     unsigned long value)
{
  unsigned long n[1];
  size_t l;

  l = strlen (name);
  if (AMIGA_DATA(abfd)->IsLoadFile && l > MAX_NAME_SIZE)
    l = MAX_NAME_SIZE;
  n[0] = (LONGSIZE (l) | value);
  if (!write_longs (n, 1, abfd))
    return false;
  if (bfd_bwrite (name, l, abfd) != l)
    return false;
  n[0] = 0;
  l = (4 - (l & 3)) & 3;
  return (l && bfd_bwrite ((void *)n, l, abfd) != l ? false : true);
}

static bool
amiga_write_archive_contents (
  bfd *arch)
{
  struct stat status;
  bfd *object;

  for (object = arch->archive_head; object; object = object->archive_next)
    {
      unsigned long remaining;

      if (bfd_write_p (object))
	{
	  bfd_set_error (bfd_error_invalid_operation);
	  return false;
	}

      if (object->arelt_data != NULL)
	{
	  remaining = arelt_size (object);
	}
      else
	{
	  if (stat (object->filename, &status) != 0)
	    {
	      bfd_set_error (bfd_error_system_call);
	      return false;
	    }
	  remaining = status.st_size;
	}

      if (bfd_seek (object, 0, SEEK_SET))
	return false;

      while (remaining)
	{
	  char buf[DEFAULT_BUFFERSIZE];
	  unsigned long amt = sizeof(buf);
	  if (amt > remaining)
	    amt = remaining;
	  errno = 0;
	  if (bfd_bread (buf, amt, object) != amt)
	    {
	      if (bfd_get_error () != bfd_error_system_call)
		bfd_set_error (bfd_error_malformed_archive);
	      return false;
	    }
	  if (bfd_bwrite (buf, amt, arch) != amt)
	    return false;
	  remaining -= amt;
	}
    }
  return true;
}

static bool
amiga_write_armap (
     bfd *arch ATTRIBUTE_UNUSED,
     unsigned int elength ATTRIBUTE_UNUSED,
     struct orl *map ATTRIBUTE_UNUSED,
     unsigned int orl_count ATTRIBUTE_UNUSED,
     int stridx ATTRIBUTE_UNUSED)
{
  return true;
}

static int
determine_type (
  arelent *r)
{
  switch (r->howto->type)
    {
      case H_ABS32: /* 32 bit absolute */
	return 0;

      case H_ABS16: /* 16 bit absolute */
      return 0; // not working properly?

      case H_PC32:  /* 32 bit pcrel */
	return 2;

      case H_PC16:  /* 16 bit pcrel */
	return 3;

      case H_ABS8: /* 8 bit absolute */
      case H_PC8:  /* 8 bit pcrel */
	return 4;

      case H_SD32: /* 32 bit baserel */
	return 5;

      case H_SD16: /* 16 bit baserel */
	return 6;

      case H_SD8: /* 8 bit baserel */
	return 7;

      default: /* Error, can't represent this */
	bfd_set_error (bfd_error_nonrepresentable_section);
	return -1;
    }/* Of switch */
}

#define NB_RELOC_TYPES 8
static const unsigned long reloc_types[NB_RELOC_TYPES] = {
  HUNK_ABSRELOC32, HUNK_ABSRELOC16,
  HUNK_RELRELOC32, HUNK_RELRELOC16, HUNK_RELRELOC8,
  HUNK_DREL32,     HUNK_DREL16,     HUNK_DREL8
};

/* Write out section contents, including relocs */
static bool
amiga_write_section_contents (
     bfd *abfd,
     sec_ptr section,
     sec_ptr data_sec,
     unsigned long datadata_relocs,
     int *index_map,
     int max_hunk)
{
  sec_ptr insection;
  asymbol *sym_p;
  arelent *r;
  unsigned long zero=0,disksize,pad,n[2],k,l,s;
  long *reloc_counts,reloc_count=0;
  unsigned char *values;
  int i,j,x,type;

  DPRINT(5,("Entering write_section_contents\n"));

  /* If we are base-relative linking and the section is .bss and abfd
     is a load file, then return */
  if (AMIGA_DATA(abfd)->IsLoadFile)
    {
      if (amiga_base_relative && !strcmp(section->name, ".bss"))
	return true; /* Nothing to do */
    }
  else
    {
      /* WRITE out HUNK_NAME + section name */
      n[0] = HUNK_NAME;
      if (!write_longs (n, 1, abfd) || !write_name (abfd, section->name, 0))
	return false;
    }
  if (0 == strncmp(section->name, ".debug_", 7))
    section->flags = (section->flags & ~(SEC_CODE | SEC_DATA | SEC_ALLOC | SEC_LOAD)) | SEC_DEBUGGING;

  /* Depending on the type of the section, write out HUNK_{CODE|DATA|BSS} */
  if (section->flags & SEC_CODE) /* Code section */
    n[0] = HUNK_CODE;
  else if (section->flags & (SEC_DATA | SEC_LOAD)) /* data section */
    n[0] = HUNK_DATA;
  else if (section->flags & SEC_ALLOC) /* BSS */
    n[0] = HUNK_BSS;
  else if (section->flags & SEC_DEBUGGING) /* debug section */
    n[0] = HUNK_DEBUG;
  else /* Error */
    {
#if 0
      bfd_set_error (bfd_error_nonrepresentable_section);
      return false;
#else
      /* FIXME: Just dump everything we don't currently recognize into
	 a DEBUG hunk. */
      n[0] = HUNK_DEBUG;
#endif
    }

  DPRINT(10,("Section type is %lx\n",n[0]));

  /* Get real size in n[1], this may be shorter than the size in the header */
  if (amiga_per_section(section)->disk_size == 0)
    amiga_per_section(section)->disk_size = section->rawsize;
  disksize = LONGSIZE (amiga_per_section(section)->disk_size) + datadata_relocs;
  n[1] = disksize;

  /* in a load file, we put section attributes only in the header */
  if (!AMIGA_DATA(abfd)->IsLoadFile)
    {
      /* Get attribute for section */
      switch (amiga_per_section(section)->attribute)
	{
	case MEMF_CHIP:
	  n[1] |= HUNKF_CHIP;
	  break;
	case MEMF_FAST:
	  n[1] |= HUNKF_FAST;
	  break;
	case 0:
	  break;
	default: /* error, can't represent this */
	  bfd_set_error (bfd_error_nonrepresentable_section);
	  return false;
	  break;
	}
    }/* Of switch */

  if (!write_longs (n, 2, abfd))
      return false;

  DPRINT(5,("Wrote code and size=%lx\n",n[1]));

  /* If a BSS hunk, we're done, else write out section contents */
  if (HUNK_VALUE (n[0]) == HUNK_BSS)
    return true;

  DPRINT(5,("Non bss hunk...\n"));

  /* Traverse through the relocs, sample them in reloc_data, adjust section
     data to get 0 addend
     Then compactify reloc_data
     Set the entry in the section for the reloc to NULL */

  if (disksize != 0)
    BFD_ASSERT ((section->flags & SEC_IN_MEMORY) != 0);

  reloc_counts = (long *) bfd_zalloc (abfd, NB_RELOC_TYPES * (max_hunk + 1) * sizeof(long));
  if (!reloc_counts)
    return false;

  DPRINT(5,("Section has %d relocs\n",section->reloc_count));

  for (l = 0; l < section->reloc_count; l++)
    {
      r = section->orelocation[l];
      if (r == NULL)
	continue;

      sym_p = *(r->sym_ptr_ptr); /* The symbol for this relocation */
      insection = sym_p->section;
      DPRINT(5,("Sec for reloc is %lx(%s)\n",insection,insection->name));
      DPRINT(5,("Symbol for this reloc is %lx(%s)\n",sym_p,sym_p->name));
      /* Is reloc relative to a special section? */
      if (bfd_is_special_section(insection))
	continue; /* Nothing to do, since this translates to HUNK_EXT */

      r->addend += sym_p->value; /* Add offset of symbol from section start */

      /* Address of reloc has been unchanged since original reloc, or has
	 been adjusted by get_relocated_section_contents. */
      /* For relocs, the vma of the target section is in the data, the
	 addend is -vma of that section =>No need to add vma */
      /* Add in offset */
      r->addend += insection->output_offset;

      /* Determine which hunk to write, and index of target */
      x = index_map[insection->output_section->index];
      if (x < 0 || x > max_hunk)
	{
	  bfd_msg ("erroneous relocation to hunk %d/%s from %s", x, insection->output_section->name, insection->name);
	bfd_set_error (bfd_error_nonrepresentable_section);
	return false;
      }

      type = determine_type(r);
      if (type == -1)
	return false;
      if (type >= NB_RELOC_TYPES)
	{
	bfd_set_error (bfd_error_nonrepresentable_section);
	return false;
      }
      reloc_counts[type+(x*NB_RELOC_TYPES)]++;
      reloc_count++;

      /* There is no error checking with these... */
      DPRINT(5,("reloc address=%lx,addend=%lx\n",r->address,r->addend));
      values = &section->contents[r->address];

      switch (r->howto->size)
	{
	case 1: /* adjust byte */
	  x = ((char *)values)[0] + r->addend;
	  values[0] = x & 0xff;
	  break;
	case 2: /* adjust word */
	  k = values[1] | (values[0] << 8);
	  x = (int)k + r->addend;
	  values[0] = (x & 0xff00) >> 8;
	  values[1] = x & 0xff;
	  break;
	case 4: /* adjust long */
	  k = values[3] | (values[2] << 8) | (values[1] << 16) | (values[0] << 24);
	  x = (int)k + r->addend;
	  values[3] = x & 0xff;
	  values[2] = (x & 0xff00) >> 8;
	  values[1] = (x & 0xff0000) >> 16;
	  values[0] = ((unsigned int)x & 0xff000000) >> 24;
	  break;
	}/* of switch */

      r->addend = 0;
      DPRINT(5,("Did adjusting\n"));
    }/* of for l */

  DPRINT(5,("Did all relocs\n"));

  /* We applied all the relocs, as far as possible to obtain 0 addend fields */
  /* Write the section contents */
  if (amiga_per_section(section)->disk_size != 0)
    {
      if (bfd_bwrite ((void *)section->contents,
      amiga_per_section(section)->disk_size,
		      abfd) !=
	  amiga_per_section(section)->disk_size)
	return false;

      /* pad the section on disk if necessary (to a long boundary) */
      pad = (4 - (amiga_per_section(section)->disk_size & 3)) & 3;
      if (pad && (bfd_bwrite ((void *)&zero, pad, abfd) != pad))
	return false;
    }

#if 0
  /* write bss data in the data hunk if needed */
  for (; bss_size--;)
    if (!write_longs (&zero, 1, abfd))
      return false;
#endif

  if (datadata_relocs)
    {
      datadata_relocs--;
      if (!write_longs (&datadata_relocs, 1, abfd))
	return false;
      for (s = 0; s < data_sec->reloc_count; s++)
	{
	  r = data_sec->orelocation[s];
	  if (r == NULL)
	    continue;

	  sym_p = *(r->sym_ptr_ptr); /* The symbol for this relocation */
	  insection = sym_p->section;
	  /* Is reloc relative to a special section? */
	  if (bfd_is_special_section(insection))
	    continue; /* Nothing to do, since this translates to HUNK_EXT */

	  if (insection->output_section == data_sec)
	    {
	      if (determine_type(r) == 0)
		if (!write_longs (&r->address, 1, abfd))
		  return false;
	    }
	}
    }

  DPRINT(10,("Wrote contents, writing relocs now\n"));

  if (reloc_count > 0)
    {

      /* do not use H_ABS32 and H_ABS16 simultaneous. */
      if (reloc_counts[0] && reloc_counts[1])
	{

	}

    /* Sample every reloc type */
      for (i = 0; i < NB_RELOC_TYPES; i++)
	{
      bool rel32 = amiga_reloc_long_p (reloc_types[i], AMIGA_DATA(abfd)->IsLoadFile);
      int written = false;
	  for (j = 0; j <= max_hunk; j++)
	    {
	long relocs;
	      while ((relocs = reloc_counts[i + (j * NB_RELOC_TYPES)]) > 0)
		{

		  if (!written)
		    {
	    if (!write_longs (&reloc_types[i], 1, abfd))
	      return false;
	    written = true;
	  }

	  if (relocs > 0xffff)
	    relocs = 0xffff;

	  n[0] = relocs;
	  n[1] = j;
		  if (rel32)
		    {
	    if (!write_longs (n, 2, abfd))
	      return false;
	  }
		  else
		    {
	    if (!write_words (n, 2, abfd))
	      return false;
	  }

	  reloc_counts[i+(j*NB_RELOC_TYPES)] -= relocs;
	  reloc_count -= relocs;

		  for (k = 0; k < section->reloc_count; k++)
		    {
	    int jj;

	    r = section->orelocation[k];
	    if (r == NULL) /* already written */
	      continue;

	    sym_p = *(r->sym_ptr_ptr); /* The symbol for this relocation */
	    insection = sym_p->section;
	    /* Is reloc relative to a special section? */
	    if (bfd_is_special_section(insection))
	      continue; /* Nothing to do, since this translates to HUNK_EXT */
#if 0
	    /* Determine which hunk to write, and index of target */
		      for (jj = 0, sec = abfd->sections; sec; sec = sec->next, jj++)
			{
	      if (sec == insection->output_section)
		break;
	    }
	    BFD_ASSERT (jj==index_map[insection->output_section->index]);
#else
	    jj=index_map[insection->output_section->index];
#endif
		      if (jj == j && i == determine_type (r))
			{
	      section->orelocation[k] = NULL;
			  if (rel32)
			    {
		if (!write_longs (&r->address, 1, abfd))
		  return false;
	      }
			  else
			    {
		if (!write_words (&r->address, 1, abfd))
		  return false;
	      }
	      if (--relocs == 0)
		break;
	    }
	  }
	}
      }
      /* write a zero to finish the relocs */
	  if (written)
	    {
	      if (rel32 || (bfd_tell (abfd) & 2) == 0)
		{
	  if (!write_longs (&zero, 1, abfd))
	    return false;
	}
	      else
		{
	  if (!write_words (&zero, 1, abfd))
	    return false;
	}
      }
    }
  }

  bfd_release (abfd, reloc_counts);
  DPRINT(5,("Leaving write_section...\n"));
  if (reloc_count > 0)
    {
    bfd_set_error (bfd_error_nonrepresentable_section);
    return false;
  }
  return true;
}


/* Write out symbol information, including HUNK_EXT, DEFS, ABS.
   In the case, we were linking base relative, the symbols of the .bss
   hunk have been converted already to belong to the .data hunk */

static bool amiga_write_symbols (
  bfd *abfd, sec_ptr section)
{
  sec_ptr osection;
  asymbol *sym_p;
  arelent *r;
  unsigned long n[3],symbol_header,type;
  unsigned int i,j,idx,ncnt,symbol_count;

  /* If base rel linking and section is .bss ==> exit */
  if (amiga_base_relative && !strcmp(section->name,".bss"))
    return true;

  if (section->reloc_count==0 && bfd_get_symcount (abfd)==0)
    {/* Write HUNK_END */
    alldone:
      DPRINT(5,("Leaving write_symbols\n"));
      n[0]=HUNK_END;
      return write_longs (n, 1, abfd);
    }

  /* If this is Loadfile, then do not write HUNK_EXT, but rather HUNK_SYMBOL */
  symbol_header = AMIGA_DATA(abfd)->IsLoadFile ? HUNK_SYMBOL : HUNK_EXT;

  /* Write out all the symbol definitions, then HUNK_END

     Now, first traverse the relocs, all entries that are non NULL
     have to be taken into account */
  symbol_count = 0;

  DPRINT(10,("Traversing relocation table\n"));
  for (i=0;i<section->reloc_count;i++)
    {
      r=section->orelocation[i];
      if (r==NULL)
	continue;

      sym_p=*(r->sym_ptr_ptr); /* The symbol for this relocation */
      osection=sym_p->section; /* The section the symbol belongs to */
      /* this section MUST be a special section */

      DPRINT(5,("Symbol is %s, section is %lx(%s)\n",sym_p->name,osection,osection->name));

      /* group together relocations referring to the same symbol and howto */
      for(idx=i,j=i+1;j<section->reloc_count;j++)
	{
	  arelent *rj=section->orelocation[j];
	  if (rj==NULL || sym_p!=*(rj->sym_ptr_ptr) || r->howto!=rj->howto)
	    continue; /* no match */
	  if (++i == j)
	    continue; /* adjacent */
	  section->orelocation[j] = section->orelocation[i];
	  section->orelocation[i] = rj;
	}

      if ((symbol_count++)==0) /* First write out the HUNK_EXT */
	{
	  if (!write_longs (&symbol_header, 1, abfd))
	    return false;
	}

      if (!bfd_is_com_section(osection)) /* Not common symbol */
	{
	  DPRINT(5,("Non common ref\n"));
	  /* Determine type of ref */
	  switch (r->howto->type)
	    {
	    case H_ABS8:
	      type=EXT_ABSREF8;
	      break;

	    case H_ABS16:
	      type=EXT_ABSREF16;
	      break;

	    case H_ABS32:
	      type=EXT_ABSREF32;
	      break;

	    case H_PC8:
	      type=EXT_RELREF8;
	      break;

	    case H_PC16:
	      type=EXT_RELREF16;
	      break;

	    case H_PC32:
	      type=EXT_RELREF32;
	      break;

	    case H_SD8:
	      type=EXT_DEXT8;
	      break;

	    case H_SD16:
	      type=EXT_DEXT16;
	      break;

	    case H_SD32:
	      type=EXT_DEXT32;
	      break;

	    case H_PC26:
	      type=EXT_RELREF26;
	      break;

	    default: /* Error, can't represent this */
	      bfd_msg ("unexpected reloc %d(%s) at offset 0x%lx", r->howto->type, r->howto->name, (long unsigned int)bfd_tell (abfd));
	      bfd_set_error (bfd_error_nonrepresentable_section);
	      return false;
	      break;
	    }/* Of switch */
	  ncnt=0;
	}/* Of is ref to undefined or abs symbol */
      else /* ref to common symbol */
	{
	  DPRINT(5,("Common ref\n"));
	  switch (r->howto->type)
	    {
	    default:
	      bfd_msg ("Warning: bad reloc %s for common symbol %s", r->howto->name, sym_p->name);
	      // fall through
	    case H_ABS32:
	      type=EXT_ABSCOMMON;
	      break;

	    case H_PC32:
	      type=EXT_RELCOMMON;
	      break;

	    case H_SD8:
	      type=EXT_DEXT8COMMON;
	      break;

	    case H_SD16:
	      type=EXT_DEXT16COMMON;
	      break;

	    case H_SD32:
	      type=EXT_DEXT32COMMON;
	      break;
	    }/* Of switch */
	  n[0]=sym_p->value; /* Size of common block */
	  ncnt=1;
	}/* Of is common section */

	DPRINT(5,("Type is %lx\n",type));
	if (!write_name (abfd, sym_p->name, type << 24))
	  return false;
	n[ncnt]=i-idx+1; /* refs for symbol... */
	if (!write_longs (n, ncnt+1, abfd))
	  return false;
	for(;idx<=i;++idx)
	  {
	    n[0]=section->orelocation[idx]->address;
	    if (!write_longs (n, 1, abfd))
	      return false;
	  }
    }/* Of traverse relocs */

  /* Now traverse the symbol table and write out all definitions, that are relative
     to this hunk.
     Absolute defs are always only written out with the first hunk.
     Don't write out:
	local symbols
	undefined symbols
	indirect symbols
	warning symbols
	debugging symbols
	warning symbols
	constructor symbols
     since they are unrepresentable in HUNK format.. */

  DPRINT(10,("Traversing symbol table\n"));
  for (i=0;i<bfd_get_symcount (abfd);i++)
    {
      sym_p=abfd->outsymbols[i];
      osection=sym_p->section;

      DPRINT(5,("%d: symbol(%s), osec=%lx(%s)\n",
	i,sym_p->name,osection,osection?osection->name:"null"));

      if (osection==NULL) /* FIXME: Happens with constructor functions. */
	continue;

      if (bfd_is_und_section(osection)
	/*||bfd_is_com_section(osection)*/
	  ||bfd_is_ind_section(osection))
	continue; /* Don't write these */

      /* Only write abs defs, if not writing a Loadfile */
      if (bfd_is_abs_section(osection) && (section->index == 0) && !AMIGA_DATA(abfd)->IsLoadFile)
	{
	  DPRINT(5,("Abs symbol\n"));
	  /* don't write debug symbols, they will be written in a
	     HUNK_DEBUG later on */
	  if (sym_p->flags & BSF_DEBUGGING)
	    continue;

	  if ((symbol_count++)==0) /* First write out the HUNK_EXT */
	    {
	      if (!write_longs (&symbol_header, 1, abfd))
		return false;
	    }

	  if (!write_name (abfd, sym_p->name, (EXT_ABS
	      | ((sym_p->flags & BSF_WEAK) ? 0x40 : 0)
	      | ((sym_p->flags & BSF_LOCAL) ? 0x20 : 0)
	      )<< 24))
	    return false;
	  n[0]=sym_p->value;
	  if (!write_longs (n, 1, abfd))
	    return false;
	  continue;
	}/* Of abs def */
      if (bfd_is_abs_section(osection))
	continue; /* Not first hunk, already written */

      /* If it is a warning symbol, or a constructor symbol or a
	 debugging, don't write it */
      if (sym_p->flags & (BSF_WARNING|BSF_CONSTRUCTOR|BSF_DEBUGGING))
	continue;

      if (0 == (sym_p->flags & (BSF_WEAK | BSF_GLOBAL)))
	{
	  if (sym_p->name[0] == '.')
	    continue;

	  sym_p->flags |= BSF_LOCAL;
	}

      if ((sym_p->flags & BSF_LOCAL) && symbol_header == HUNK_SYMBOL)
	continue;

      /* Now, if osection==section, write it out */
      if (osection->output_section==section)
	{
	  DPRINT(5,("Writing it out\n"));

	  if ((symbol_count++)==0) /* First write out the header */
	    {
	      if (!write_longs (&symbol_header, 1, abfd))
		return false;
	    }

	  type = symbol_header == HUNK_EXT ? (EXT_DEF
	      | ((sym_p->flags & BSF_WEAK) ? 0x40 : 0)
	      | ((sym_p->flags & BSF_LOCAL) ? 0x20 : 0)
	      )<< 24 : 0;
	  if (!write_name (abfd, sym_p->name, type))
	    return false;
	  n[0] = sym_p->value + sym_p->section->output_offset;
	  if (!write_longs (n, 1, abfd))
	    return false;
	}
      else
	{
	  /* write common definitions as bss common references */
	  if (bfd_is_com_section(osection->output_section) && section->index == 2)
	    {
	      if ((symbol_count++)==0) /* First write out the header */
		{
		  if (!write_longs (&symbol_header, 1, abfd))
		    return false;
		}

	      if (!write_name (abfd, sym_p->name, (EXT_ABSCOMMON
		  | ((sym_p->flags & BSF_WEAK) ? 0x40 : 0)
		  | ((sym_p->flags & BSF_LOCAL) ? 0x20 : 0)
		  ) << 24))
		return false;
	      n[0]=sym_p->value;
	      n[1]=0;
	      if (!write_longs (n, 2, abfd))
		return false;
	    }
	}
    }/* Of for */

  DPRINT(10,("Did traversing\n"));
  if (symbol_count) /* terminate HUNK_EXT, HUNK_SYMBOL */
    {
      n[0]=0;
      if (!write_longs (n, 1, abfd))
	return false;
    }
  DPRINT(5,("Leaving\n"));
  goto alldone;
  /* Write HUNK_END, return */
}

static bool
amiga_get_section_contents (bfd *abfd, sec_ptr section,
			    void * location,
			    file_ptr offset,
			    bfd_size_type count)
{
  unsigned long disk_size=amiga_per_section(section)->disk_size;

  if (bfd_seek (abfd, section->filepos + offset, SEEK_SET))
    return false;

  if (offset+count > disk_size)
    {
      /* the section's size on disk may be smaller than in memory
       in this case, pad the contents */
      if (bfd_bread (location, disk_size-offset, abfd) != disk_size-offset)
      return false;
      memset ((char *) location + disk_size - offset, 0, count-(disk_size-offset));
    }
  else
    {
      if (bfd_bread (location, count, abfd) != count)
      return false;
    }
  return true;
}

static bool
amiga_new_section_hook (
     bfd *abfd,
     sec_ptr newsect)
{
  newsect->used_by_bfd = (void *) bfd_zalloc (abfd, sizeof(amiga_per_section_type));
  newsect->alignment_power = 2;
  if (!strcmp (newsect->name, ".datachip") || !strcmp (newsect->name, ".bsschip"))
    amiga_per_section(newsect)->attribute |= MEMF_CHIP;
  if (!strcmp (newsect->name, ".datafast") || !strcmp (newsect->name, ".bssfast"))
    amiga_per_section(newsect)->attribute |= MEMF_FAST;
  return true;
}

static bool
amiga_slurp_symbol_table (
  bfd *abfd)
{
  amiga_data_type *amiga_data=AMIGA_DATA(abfd);
  amiga_symbol_type *asp;
  unsigned long l,len,type;
  sec_ptr section;
  sec_ptr stab = 0;
  sec_ptr stabstr = 0;
  sec_ptr sbss = 0;
  sec_ptr sdata = 0;
  sec_ptr stext = 0;

  if (amiga_data->symbols)
    return true; /* already read */

  unsigned totalsymcount = bfd_get_symcount(abfd);
  for (section=abfd->sections; section!=NULL; section=section->next)
    {
	  if (0 == strcmp (section->name, ".stab"))
	    {
	      amiga_per_section_type *astab = amiga_per_section(section);
	      totalsymcount += astab->disk_size / 12;
	      break;
	    }
    }
  if (!totalsymcount)
	return true;


  asp = (amiga_symbol_type *) bfd_zalloc (abfd, sizeof(amiga_symbol_type) * totalsymcount);
  if ((amiga_data->symbols = asp) == NULL)
    return false;

  /* Symbols are associated with every section */
  for (section=abfd->sections; section!=NULL; section=section->next)
    {
      amiga_per_section_type *asect=amiga_per_section(section);

      /* remember the stabstr section. */
      if (0 == strcmp (section->name, ".stabstr"))
	stabstr = section;
      else if (0 == strcmp (section->name, ".stab"))
	stab = section;
      else if (0 == strcmp (section->name, ".data"))
	sdata = section;
      else if (0 == strcmp (section->name, ".text"))
	stext = section;
      else if (0 == strcmp (section->name, ".bss"))
	sbss = section;

      if (asect->hunk_ext_pos == 0)
	continue;

      if (bfd_seek (abfd, asect->hunk_ext_pos, SEEK_SET))
	return false;

      for (asect->amiga_symbols=asp; get_long (abfd, &l) && l; asp++)
	{
	  ++asect->amiga_symbol_count;
	  type = l>>24;	/* type of entry */
	  len = (l & 0xffffff) << 2; /* namelength */

	  /* read the name */
	  if ((asp->symbol.name = bfd_alloc (abfd, len+1))==NULL)
	    return false;
	  if (bfd_bread ((void *)asp->symbol.name, len, abfd) != len)
	    return false;
	  ((char *)asp->symbol.name)[len] = '\0';

	  asp->symbol.the_bfd = abfd;
	  asp->symbol.flags = BSF_GLOBAL;
	  /*asp->desc = 0;
	  asp->other = 0;*/
	  asp->type = type;
	  asp->index = asp - amiga_data->symbols;

	  if (type < 200 && (type & 0x40))
	    {
	      asp->symbol.flags = BSF_WEAK | BSF_GLOBAL;
	      type &= ~0x40;
	    }
	  else if (type < 200 && (type & 0x20))
	    {
	      asp->symbol.flags = BSF_LOCAL;
	      type &= ~0x20;
	    }

	  switch (type)
	    {
	  case EXT_ABSCOMMON: /* Common reference/definition */
	  case EXT_RELCOMMON:
	  case EXT_DEXT32COMMON:
	  case EXT_DEXT16COMMON:
	  case EXT_DEXT8COMMON:
	    asp->symbol.section = bfd_com_section_ptr;
	    /* size of common block -> symbol's value */
	    if (!get_long (abfd, &l))
	      return false;
	    asp->symbol.value = l;
	    /* skip refs */
	    if (!get_long (abfd, &l) || bfd_seek (abfd, l<<2, SEEK_CUR))
	      return false;
	    asp->refnum = l;
	    break;
	  case EXT_ABS: /* Absolute */
	    asp->symbol.section = bfd_abs_section_ptr;
	    goto rval;
	    break;
	  case EXT_DEF: /* Relative Definition */
	  case EXT_SYMB: /* Same as EXT_DEF for load files */
	    asp->symbol.section = section;
	  rval:
	    /* read the value */
	    if (!get_long (abfd, &l))
	      return false;
	    asp->symbol.value = l;
	    break;
	  default: /* References to an undefined symbol */
	    asp->symbol.section = bfd_und_section_ptr;
	    asp->symbol.flags = 0;
	    /* skip refs */
	    if (!get_long (abfd, &l) || bfd_seek (abfd, l<<2, SEEK_CUR))
	      return false;
	    asp->refnum = l;
	    break;
	  }
	}
      {
	unsigned i = 0;
	bool all_weak = true;
	for (i = 0; i < asect->amiga_symbol_count; ++i)
	  {
	    if (asect->amiga_symbols[i].symbol.value == 0)
	    {
	      section->symbol->name = asect->amiga_symbols[i].symbol.name;
	      section->symbol->flags |= BSF_GLOBAL;
	    }
	    if (0 == (asect->amiga_symbols[i].symbol.flags & BSF_WEAK))
	      all_weak = false;
	  }
	if (i == asect->amiga_symbol_count)
	  section->symbol->name = section->name;
	if (i && all_weak)
	  {
	    /* SBF: mark the section optional */
	    section->flags |= SEC_LINK_ONCE | SEC_LINK_DUPLICATES_DISCARD | SEC_LINK_DUPLICATES_SAME_CONTENTS;
	    /* SBF: but clear the flags for the first found section .*/
	    if (!bfd_section_already_linked (abfd, section, plink_info))
	      section->flags &= ~(SEC_LINK_ONCE | SEC_LINK_DUPLICATES_DISCARD | SEC_LINK_DUPLICATES_SAME_CONTENTS);
	  }
      }
    }

  /* add the constructor symbols defined by stab. */
  if (stab && stabstr)
    {
      amiga_per_section_type *astab = amiga_per_section(stab);
      amiga_per_section_type *astabstr = amiga_per_section(stabstr);

      astab->amiga_symbols = asp;
      astabstr->amiga_symbols = asp;

      if (sbss == 0)
	sbss = amiga_make_unique_section (abfd, ".bss");

      if (sdata == 0)
	sdata = amiga_make_unique_section (abfd, ".data");

      unsigned char * stabstrdata = (unsigned char *) bfd_alloc (abfd, astabstr->disk_size);
      if (!stabstrdata)
	return false;
      if (bfd_seek (abfd, stabstr->filepos, SEEK_SET))
	return false;
      if (bfd_bread (stabstrdata, astabstr->disk_size, abfd) != astabstr->disk_size)
	return false;

      stabstr->contents = stabstrdata;

      unsigned char * stabdata = (unsigned char *) bfd_alloc (abfd, astab->disk_size);
      if (!stabdata)
	return false;
      if (bfd_seek (abfd, stab->filepos, SEEK_SET))
	return false;
      if (bfd_bread (stabdata, astab->disk_size, abfd) != astab->disk_size)
	return false;

      stab->contents = stabdata;

      {unsigned i; for (i = 0; i < astab->disk_size; i += 12)
	{
	  unsigned str_offset = bfd_getb32 (stabdata + i);
	  char ctype = stabdata[i + 4];
	  unsigned value = bfd_getb32 (stabdata + i + 8);
	  unsigned flags = BSF_GLOBAL;

	  switch (ctype)
	    {
	    default:
	      continue;

	    case N_WARNING:
	      section = bfd_und_section_ptr;
	      flags |= BSF_WARNING;
	      break;
	    case N_INDR | N_EXT:
	      asp->symbol.section = bfd_ind_section_ptr;
	      flags |= BSF_INDIRECT;
	      break;
	    case N_UNDF | N_EXT:
	      if (value == 0)
		{
		  asp->symbol.section = bfd_und_section_ptr;
		  flags = 0;
		}
	      else
		asp->symbol.section = bfd_com_section_ptr;
	      break;
	    case N_ABS | N_EXT:
	      asp->symbol.section = bfd_abs_section_ptr;
	      break;
	    case N_TEXT | N_EXT:
	      asp->symbol.section = stext;
	      break;
	    case N_DATA | N_EXT:
	    case N_SETV | N_EXT:
	      asp->symbol.section = sdata;
	      break;
	    case N_BSS | N_EXT:
	      asp->symbol.section = sbss;
	      break;
	    case N_COMM | N_EXT:
	      asp->symbol.section = bfd_com_section_ptr;
	      break;
	    case N_SETA:
	    case N_SETA | N_EXT:
	      flags |= BSF_CONSTRUCTOR;
	      asp->symbol.section = bfd_abs_section_ptr;
	      break;
	    case N_SETT:
	    case N_SETT | N_EXT:
	      flags |= BSF_CONSTRUCTOR;
	      asp->symbol.section = stext;
	      break;
	    case N_SETD:
	    case N_SETD | N_EXT:
	      flags |= BSF_CONSTRUCTOR;
	      asp->symbol.section = sdata;
	      break;
	    case N_SETB:
	    case N_SETB | N_EXT:
	      flags |= BSF_CONSTRUCTOR;
	      asp->symbol.section = sbss;
	      break;
	    case N_WEAKU:
	      asp->symbol.section = bfd_und_section_ptr;
	      flags |= BSF_WEAK;
	      break;
	    case N_WEAKA:
	      asp->symbol.section = bfd_abs_section_ptr;
	      flags |= BSF_WEAK;
	      break;
	    case N_WEAKT:
	      asp->symbol.section = stext;
	      flags |= BSF_WEAK;
	      break;
	    case N_WEAKD:
	      asp->symbol.section = sdata;
	      flags |= BSF_WEAK;
	      break;
	    case N_WEAKB:
	      asp->symbol.section = sbss;
	      flags |= BSF_WEAK;
	      break;
            }

	  ++astab->amiga_symbol_count;
	  ++astabstr->amiga_symbol_count;
	  asp->symbol.flags = flags;
	  asp->symbol.the_bfd = abfd;
	  asp->type = 0x81; //??
	  asp->index = asp - amiga_data->symbols;

	  if (str_offset > astabstr->disk_size)
	    return false;

	  asp->symbol.name = (char *) stabstrdata + str_offset;

	  asp->symbol.value = value;

	  ++asp;
	}}
    }
  return true;
}

/* Get size of symtab */
static long amiga_get_symtab_upper_bound (
  bfd *abfd)
{
  if (!amiga_slurp_symbol_table (abfd))
    return -1;
  return (bfd_get_symcount (abfd)+1) * (sizeof (amiga_symbol_type *));
}

static long amiga_canonicalize_symtab (
  bfd *abfd,asymbol **location)
{
  if(!amiga_slurp_symbol_table (abfd))
    return -1;
  if (bfd_get_symcount (abfd))
    {
      amiga_symbol_type *symp=AMIGA_DATA(abfd)->symbols;
      unsigned int i;
      for (i = 0; i < bfd_get_symcount (abfd); i++, symp++)
	*location++ = &symp->symbol;
      *location = 0;
    }
  return bfd_get_symcount (abfd);
}


static asymbol *
amiga_make_empty_symbol (
  bfd *abfd)
{
  amiga_symbol_type *new = (amiga_symbol_type *) bfd_zalloc (abfd, sizeof(amiga_symbol_type));
  new->symbol.the_bfd = abfd;
  return &new->symbol;
}


static void
amiga_get_symbol_info (
     bfd *ignore_abfd ATTRIBUTE_UNUSED,
     asymbol *symbol,
     symbol_info *ret)
{
  bfd_symbol_info (symbol, ret);
  if (symbol->name[0] == ' ')
    ret->name = "* empty table entry ";
  if (bfd_is_abs_section(symbol->section))
    ret->type = (symbol->flags & BSF_LOCAL) ? 'a' : 'A';
}


static void
amiga_print_symbol (
  bfd *abfd,
  void * afile,
asymbol *symbol,
bfd_print_symbol_type how)
{
  FILE *file = (FILE *)afile;

    switch (how)
      {
  case bfd_print_symbol_name:
    fprintf (file, "%s", symbol->name);
    break;
  case bfd_print_symbol_more:
    fprintf (file, "%4lx %2x",
	     amiga_symbol(symbol)->refnum,
	     (unsigned int)amiga_symbol(symbol)->type);
    break;
  case bfd_print_symbol_all:
    if (symbol->name[0] == ' ')
      {
	fprintf (file, "* empty table entry ");
      }
    else
      {
	bfd_print_symbol_vandf (abfd, (void *)file, symbol);
	fprintf (file, " %-10s %04lx %02x %s",
		 symbol->section->name,
		 amiga_symbol(symbol)->refnum,
		 (unsigned int)amiga_symbol(symbol)->type,
		 symbol->name);
      }
    break;
  }
}


static long
amiga_get_reloc_upper_bound (
     bfd *abfd ATTRIBUTE_UNUSED,
     sec_ptr asect)
{
  return (asect->reloc_count + 1) * sizeof (arelent *);
}


static bool
read_raw_relocs (
     bfd *abfd,
     sec_ptr section,
     unsigned long d_offset,	/* offset in the bfd */
  unsigned long count) /* number of relocs */
{
  unsigned long hunk_number,offset,type,no,j;
  reloc_howto_type *howto;

  if (bfd_seek (abfd, d_offset, SEEK_SET))
    return false;
  while ((long)count > 0)
    {
      /* first determine type of reloc */
      if (!get_long (abfd, &type))
	return false;

      howto = howto_for_raw_reloc (type, AMIGA_DATA(abfd)->IsLoadFile);
      if (howto == NULL)
	{
	  bfd_set_error (bfd_error_wrong_format);
	  return false;
	}

	  /* read reloc count, hunk number and offsets */
      if (amiga_reloc_long_p (type, AMIGA_DATA(abfd)->IsLoadFile))
	{
	  for (;;)
	    {
	    /* read offsets and hunk number */
	    if (!get_long (abfd, &no))
	      return false;
	    if (!no)
	      break;
	    count -= no;
	    if (!get_long (abfd, &hunk_number))
	      return false;
	    /* add relocs */
	      for (j = 0; j < no; j++)
		{
		  if (!get_long (abfd, &offset) || !amiga_add_reloc (abfd, section, offset, NULL, howto, hunk_number))
		return false;
	    }
	  }
	}
      else
	{
	  for (;;)
	    {
	    /* read offsets and hunk number */
	    if (!get_word (abfd, &no))
	      return false;
	    if (!no)
	      break;
	    count -= no;
	    if (!get_word (abfd, &hunk_number))
	      return false;
	    /* add relocs */
	      for (j = 0; j < no; j++)
		{
		  if (!get_word (abfd, &offset) || !amiga_add_reloc (abfd, section, offset, NULL, howto, hunk_number))
		return false;
	    }
	  }
	}
    }

  return true;
}


/* slurp in relocs, amiga_digest_file left various pointers for us */
static bool
amiga_slurp_relocs (
     bfd *abfd,
     sec_ptr section,
     asymbol **symbols ATTRIBUTE_UNUSED)
{
  amiga_per_section_type *asect=amiga_per_section(section);
  reloc_howto_type *howto;
  amiga_symbol_type *asp;
  raw_reloc_type *relp;
  unsigned long offset,type,n,i;

  if (section->relocation)
    return true;

  for (relp=asect->relocs; relp!=NULL; relp=relp->next)
    if (relp->num && !read_raw_relocs (abfd, section, relp->pos, relp->num))
      return false;

  /* Now step through the raw_symbols and add all relocs in them */
  if (!AMIGA_DATA(abfd)->symbols && !amiga_slurp_symbol_table (abfd))
    return false;

  if (asect->hunk_ext_pos == 0)
    return true;

  if (bfd_seek (abfd, asect->hunk_ext_pos, SEEK_SET))
    return false;

  for (asp=asect->amiga_symbols; get_long (abfd, &n) && n; asp++)
    {
      type = (n>>24) & 0xff;
      n &= 0xffffff;

      /* skip the name */
      if (bfd_seek (abfd, n<<2, SEEK_CUR))
	return false;

      if (type < 200)
	type &= ~0x60;

      switch (type)
	{
	case EXT_SYMB:
	case EXT_DEF:
	case EXT_ABS: /* no relocs here */
	  if (bfd_seek (abfd, 4, SEEK_CUR))
	    return false;
	  break;

	  /* same as below, but advance lp by one to skip common size */
	case EXT_DEXT32COMMON:
	case EXT_DEXT16COMMON:
	case EXT_DEXT8COMMON:
	case EXT_RELCOMMON:
	case EXT_ABSCOMMON:
	  if (bfd_seek (abfd, 4, SEEK_CUR))
	    return false;
	  /* Fall through */
	default: /* reference to something */
	  /* points to num of refs to hunk */
	  if (!get_long (abfd, &n))
	    return false;
	  /* determine howto */
	  howto = howto_for_reloc (type);
	  if (howto == NULL)
	    return false;
	  /* Add relocs to this section, relative to asp */
	  for (i=0;i<n;i++) /* refs follow */
	    {
	      if (!get_long (abfd, &offset))
		return false;
	      if (!amiga_add_reloc (abfd, section, offset,
				    abfd->outsymbols ? amiga_symbol(abfd->outsymbols[asp->index]) : asp, howto, -4))
		return false;
	    }
	  break;
	}/* of switch */
    }
  return true;
}/* Of slurp_relocs */


static long
amiga_canonicalize_reloc (
     bfd *abfd,
     sec_ptr section,
     arelent **relptr,
     asymbol **symbols)
{
  amiga_reloc_type *src;

  if (!section->relocation && !amiga_slurp_relocs (abfd, section, symbols))
    return -1;

  for (src = (amiga_reloc_type *)section->relocation; src; src = src->next)
    *relptr++ = &src->relent;
  *relptr = NULL;

  return section->reloc_count;
}


/* Set section contents */
/* We do it the following way:
   If this is a bss section ==> error
   Otherwise, we try to allocate space for this section,
   if this has not already been done
   Then we set the memory area to the contents */
static bool
amiga_set_section_contents (
     bfd *abfd,
     sec_ptr section,
  const void * location,
file_ptr offset,
bfd_size_type count)
{
  if ((section->flags&SEC_HAS_CONTENTS)==0) /* BSS */
    {
      bfd_set_error (bfd_error_no_contents);
      return false;
    }

  if ((section->flags&SEC_IN_MEMORY)==0) /* Not in memory, so alloc space */
    {
      section->contents = (bfd_byte *) bfd_zalloc (abfd, section->rawsize);
      if (section->contents == NULL)
	return false;
      section->flags |= SEC_IN_MEMORY;
	DPRINT(5,("Allocated %lx bytes at %lx\n",section->rawsize,section->contents));
    }

  /* Copy mem */
  memmove(&section->contents[offset],location,count);

  return true;
}/* Of set_section_contents */


/* FIXME: Is this everything? */
static bool
amiga_set_arch_mach (
     bfd *abfd,
     enum bfd_architecture arch,
     unsigned long machine)
{
  bfd_default_set_arch_mach(abfd, arch, machine);
  if (arch == bfd_arch_m68k)
    {
      switch (machine)
	{
	case bfd_mach_m68000:
	case bfd_mach_m68008:
	case bfd_mach_m68010:
	case bfd_mach_m68020:
	case bfd_mach_m68030:
	case bfd_mach_m68040:
	case bfd_mach_m68060:
	case bfd_mach_m68080:
	case 0:
	  return true;
	default:
	  break;
	}
    }
  return false;
}

static int
amiga_sizeof_headers (
     bfd *ignore_abfd ATTRIBUTE_UNUSED,
     struct bfd_link_info * ignore ATTRIBUTE_UNUSED)
{
  /* The amiga hunk format doesn't have headers. */
  return 0;
}

/*
 * Load the stab symbols if both sections .stab and .stabstr exist.
 * For Loadables use the stabs DEBUG section, if present
 */
aout_symbol_type  **
amiga_load_stab_symbols (bfd *abfd)
{
  amiga_data_type *amiga_data = AMIGA_DATA(abfd);
  if (amiga_data->stab_symbols)
    return amiga_data->stab_symbols;

  /* search the stab sections. */
  unsigned char * stab = 0;
  unsigned char * stabstr = 0;
  sec_ptr s;
  for (s = abfd->sections; s; s = s->next)
    {
      if (0 == strcmp(".text", s->name))
	amiga_data->a.textsec = s;
      else
      if (0 == strcmp(".data", s->name))
	amiga_data->a.datasec = s;
      else
      if (0 == strcmp(".bss", s->name))
	amiga_data->a.bsssec = s;
      else
      if (0 == strcmp(".stab", s->name))
	{
	  amiga_data->symtab_size = s->size;
	  stab = s->contents;
	}
      else
      if (0 == strcmp(".stabstr", s->name))
	{
	  amiga_data->stringtab_size = s->size;
	  stabstr = s->contents;
	}
    }

  if (!stab || !stabstr)
    {
      if (!adata(abfd).sym_filepos || !adata(abfd).str_filepos)
	return 0;

      // executable - load the data now.
      stab = (unsigned char *)bfd_alloc(abfd, amiga_data->symtab_size);
      bfd_seek(abfd, adata(abfd).sym_filepos, SEEK_SET);
      if (bfd_bread (stab, amiga_data->symtab_size, abfd) != amiga_data->symtab_size)
	return 0;

      stabstr = (unsigned char *)bfd_alloc(abfd, amiga_data->stringtab_size);
      bfd_seek(abfd, adata(abfd).str_filepos, SEEK_SET);
      if (bfd_bread (stabstr, amiga_data->stringtab_size, abfd) != amiga_data->stringtab_size)
	return 0;
    }

  unsigned num = amiga_data->symtab_size / sizeof(struct external_nlist);
  struct aout_symbol * asyms = (struct aout_symbol *)bfd_zalloc(abfd, num * sizeof(struct aout_symbol));
  if (!aout_32_translate_symbol_table(abfd, asyms, (struct external_nlist *) stab, num, (char *)stabstr, amiga_data->stringtab_size, 0))
    return 0;

  struct aout_symbol ** ss = (struct aout_symbol **)bfd_alloc(abfd, (1 + num) * sizeof(struct aout_symbol  *));
  unsigned i;
  for (i = 0; i < num; ++i)
    ss[i] = &asyms[i];
  ss[num] = 0;

  amiga_data->stab_symbols = ss;
  return ss;
}

/* Provided a BFD, a section and an offset into the section, calculate
   and return the name of the source file and the line nearest to the
   wanted location.  */
bool
amiga_find_nearest_line (
     bfd *abfd ATTRIBUTE_UNUSED,
     asymbol **symbols ATTRIBUTE_UNUSED,
     sec_ptr section ATTRIBUTE_UNUSED,
     bfd_vma offset ATTRIBUTE_UNUSED,
     const char **filename_ptr ATTRIBUTE_UNUSED,
     const char **functionname_ptr ATTRIBUTE_UNUSED,
     unsigned int *line_ptr ATTRIBUTE_UNUSED,
     unsigned int *discriminator_ptr ATTRIBUTE_UNUSED)
{
  aout_symbol_type  **stab_symbols = amiga_load_stab_symbols(abfd);
  if (stab_symbols)
    return aout_32_find_nearest_line(abfd, stab_symbols, section, offset, filename_ptr, functionname_ptr, line_ptr, discriminator_ptr);

  *filename_ptr = abfd->filename;
  *functionname_ptr = NULL;
  *line_ptr = 0;
  if (discriminator_ptr)
    *discriminator_ptr = 0;

  asymbol * best_sym = NULL;
  bfd_vma best_vma = 0;
  unsigned int index;
  for (index = 0; index < abfd->symcount; ++index)
    {
      asymbol * asym = symbols[index];

      // search only first section
      if (asym->section != abfd->sections)
	continue;

      if (asym->value > offset)
	continue;

      if (asym->value < best_vma)
	continue;

      best_sym = asym;
      best_vma = asym->value;
    }

  if (best_sym != NULL)
    {
      *functionname_ptr = best_sym->name;
    }

  return best_sym != NULL;
}

static reloc_howto_type *
amiga_bfd_reloc_type_lookup (
     bfd *abfd ATTRIBUTE_UNUSED,
     bfd_reloc_code_real_type code)
{
  DPRINT(5,("reloc: %s (%d)\n",bfd_get_reloc_code_name(code),code));
  switch (code)
    {
    case BFD_RELOC_8_PCREL:    return &howto_table[R_PC8];
    case BFD_RELOC_16_PCREL:   return &howto_table[R_PC16];
    case BFD_RELOC_32_PCREL:   return &howto_table[R_PC32];
    case BFD_RELOC_8:          return &howto_table[R_ABS8];
    case BFD_RELOC_16:         return &howto_table[R_ABS16];
    case BFD_RELOC_32:         return &howto_table[R_ABS32];
    case BFD_RELOC_8_BASEREL:  return &howto_table[R_SD8];
    case BFD_RELOC_16_BASEREL: return &howto_table[R_SD16];
    case BFD_RELOC_32_BASEREL: return &howto_table[R_SD32];
    case BFD_RELOC_CTOR:       return &howto_table[R_ABS32];
      /* FIXME: everything handled? */
    default:                   return NULL;
    }
}

static bool
amiga_bfd_copy_private_bfd_data (
     bfd *ibfd,
     bfd *obfd)
{
  if (bfd_get_flavour (ibfd) == bfd_target_amiga_flavour
      && bfd_get_flavour (obfd) == bfd_target_amiga_flavour) {
    AMIGA_DATA(obfd)->IsLoadFile = AMIGA_DATA(ibfd)->IsLoadFile;
  }
  return true;
}

static bool
amiga_bfd_copy_private_section_data (
     bfd *ibfd ATTRIBUTE_UNUSED,
     sec_ptr isec,
     bfd *obfd ATTRIBUTE_UNUSED,
     sec_ptr osec)
{
  if (bfd_get_flavour (osec->owner) == bfd_target_amiga_flavour
      && bfd_get_flavour (isec->owner) == bfd_target_amiga_flavour)
    {
    amiga_per_section(osec)->disk_size = amiga_per_section(isec)->disk_size;
    amiga_per_section(osec)->attribute = amiga_per_section(isec)->attribute;
  }
  return true;
}

/* There is no armap in the amiga libraries, so we fill carsym entries
   one by one after having parsed the whole archive. */
static bool
amiga_slurp_armap (
  bfd *abfd)
{
  struct arch_syms *syms;
  carsym *defsyms,*csym;
  unsigned long symcount;

  /* allocate the carsyms */
  syms = amiga_ardata(abfd)->defsyms;
  symcount = amiga_ardata(abfd)->defsym_count;

  defsyms = (carsym *) bfd_alloc (abfd, sizeof (carsym) * symcount);
  if (!defsyms)
    return false;

  bfd_ardata(abfd)->symdefs = defsyms;
  bfd_ardata(abfd)->symdef_count = symcount;

  for (csym = defsyms; syms; syms = syms->next)
    {
    unsigned long len, n;
    char *symblock;

	if(csym >= defsyms + symcount)
	{
	  fprintf(stderr, "slurp_armap: read to many symbols\n");
	  exit(1);
	}

      if (syms->name)
	{
	  csym->file_offset = syms->unit_offset;
	  csym->name = syms->name;
	  ++csym;
	  continue;
	}

      if (bfd_seek (abfd, syms->offset, SEEK_SET))
	return false;
      symblock = (char *) bfd_alloc (abfd, syms->size);
      if (!symblock)
	return false;
      if (bfd_bread (symblock, syms->size, abfd) != syms->size)
	return false;

      n = GL(symblock);

      symblock += 4;
      len = n & 0xffffff;
      len <<= 2;
      csym->name = symblock;
      symblock[len] = '\0';
      csym->file_offset = syms->unit_offset;
      ++csym;
    }

  if(csym != defsyms + symcount)
    {
      fprintf(stderr, "slurp_armap: read not enough symbols\n");
      exit(1);
    }
  abfd->has_armap = true;
  return true;
}

static void
amiga_truncate_arname (
     bfd *abfd ATTRIBUTE_UNUSED,
     const char *pathname ATTRIBUTE_UNUSED,
     char *arhdr ATTRIBUTE_UNUSED)
{
}

static bfd_cleanup
amiga_archive_p (
  bfd *abfd)
{
  struct arch_syms *symbols=NULL;
  struct stat stat_buffer;
  symindex symcount=0;
  int units;

  if (bfd_stat (abfd, &stat_buffer) < 0)
    {
      bfd_set_error (bfd_error_wrong_format);
      return NULL;
    }

  if (stat_buffer.st_size != 0)
    {
      /* scan the units */
      if (!parse_archive_units (abfd, &units, stat_buffer.st_size, false,
				&symbols, &symcount))
	{
	  bfd_set_error (bfd_error_wrong_format);
	  return NULL;
	}

      /* if there is only one unit, file suffix is not .a and .lib, we
	 consider it an object, not an archive. Obviously it's not
	 always true but taking objects for archives makes ld fail,
	 so we don't have much of a choice */
      if (units == 1)
	{
	  char *p = strrchr (abfd->filename, '.');
	  if (p == NULL || (strcmp (p, ".a") && strcmp (p, ".lib")))
	    {
	      bfd_set_error (bfd_error_wrong_format);
	      return NULL;
	    }
	}
    }

  if (abfd->arelt_data)
    arelt_size (abfd) = bfd_tell (abfd);

  bfd_seek (abfd, 0, SEEK_SET);
  abfd->arch_info = bfd_scan_arch ("m68k:68000");

  if (amiga_mkarchive (abfd))
    {
      bfd_ardata(abfd)->first_file_filepos = 0;
      amiga_ardata(abfd)->filesize = stat_buffer.st_size;
      amiga_ardata(abfd)->defsyms = symbols;
      amiga_ardata(abfd)->defsym_count = symcount;
      if (amiga_slurp_armap (abfd))
	{
	  bfd_set_error (bfd_error_no_more_archived_files);
	return _bfd_no_cleanup;
    }
    }

  return NULL;
}

static bfd *
amiga_openr_next_archived_file (
     bfd *archive,
     bfd *last_file)
{
  file_ptr filestart;

  if (!last_file)
    filestart = bfd_ardata (archive)->first_file_filepos;
  else
    {
      unsigned int size = arelt_size (last_file);
      /* Pad to an even boundary... */
      filestart = last_file->origin + size;
      filestart += filestart % 2;
    }

  return _bfd_get_elt_at_filepos (archive, filestart, NULL);
}

static void *
amiga_read_ar_hdr (
bfd *abfd)
{
  struct areltdata *ared;
  unsigned long start_pos,len;
  char buf[8],*base,*name;

  start_pos = bfd_tell (abfd);
    if (start_pos >= amiga_ardata(abfd)->filesize)
      {
    bfd_set_error (bfd_error_no_more_archived_files);
    return NULL;
  }

  /* get unit type and name length in long words */
  if (bfd_bread (buf, sizeof(buf), abfd) != sizeof(buf))
    return NULL;

  if (GL (&buf[0]) != HUNK_UNIT)
    {
      bfd_set_error (bfd_error_malformed_archive);
      return NULL;
    }

  ared = bfd_zalloc (abfd, sizeof (struct areltdata));
  if (ared == NULL)
    return NULL;

  len = GL (&buf[4]) << 2;

  ared->filename = bfd_alloc (abfd, len+1 > 16 ? len+1+16 : 32);
  if (ared->filename == NULL)
    return NULL;

  switch (len)
    {
    default:
      if (bfd_bread (ared->filename, len, abfd) != len)
	return NULL;
      ared->filename[len] = '\0';
      /* strip path part */
      base = strchr (name = ared->filename, ':');
      if (base != NULL)
	name = base + 1;
      for (base = name; *name; ++name)
	{
	  if (*name == '/')
	    base = name + 1;
	}
    if (*base != '\0')
      {
	char *const p = strrchr (ared->filename = base, '.');
	if (!p || (strcmp (p, ".o") && strcmp (p, ".obj")))
	  sprintf (name, "-%08lu.o", ++amiga_ardata(abfd)->outnum);
	break;
      }
      /* Fall through */
    case 0: /* fake a name */
      sprintf (ared->filename, "obj-%08lu.o", ++amiga_ardata(abfd)->outnum);
      break;
  }

  if (bfd_seek (abfd, start_pos+4, SEEK_SET))
    return NULL;

  if (!amiga_read_unit (abfd, amiga_ardata(abfd)->filesize))
    return NULL;

  ared->parsed_size = bfd_tell (abfd) - start_pos;
  if (bfd_seek (abfd, start_pos, SEEK_SET))
    return NULL;

  return (void *) ared;
}

static int
amiga_generic_stat_arch_elt (
     bfd *abfd,
     struct stat *buf)
{
  if (abfd->arelt_data == NULL)
    {
      bfd_set_error (bfd_error_invalid_operation);
      return -1;
    }

  /* No header in amiga archives. Let's set reasonable default values */
  buf->st_mode = 0644;
  buf->st_uid = 0;
  buf->st_gid = 0;
  buf->st_mtime = 2922*24*60*60;
  buf->st_size = arelt_size (abfd);

  return 0;
}

struct bfd_hash_entry *
ref_section_hash_newfunc (struct bfd_hash_entry *entry,
			struct bfd_hash_table *table, const char *string)
{
  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (entry == NULL)
    entry = (struct bfd_hash_entry *)
	bfd_hash_allocate (table, sizeof (struct ref_section_entry));
  if (entry == NULL)
    return NULL;

  /* Call the allocation method of the superclass.  */
  entry = bfd_hash_newfunc (entry, table, string);

  if (entry != NULL)
    {
      /* Initialize the local fields.  */
      struct ref_section_entry *ret = (struct ref_section_entry *) entry;
      ret->section = NULL;
    }

  return entry;
}


/**
 * Keep this section and recurse into all referenced sections.
 * Record referenced symbols.
 */
static bool
amiga_keep_section (struct bfd_hash_table *ht, struct bfd_section * sec)
{
  asymbol * sym;
  amiga_reloc_type *src;

  if (sec->flags & SEC_KEEP)
    return true;

//  fprintf(stderr, "keeping section %s of %s\n", sec->name, sec->owner->filename);
  sec->flags |= SEC_KEEP;

  amiga_slurp_relocs(sec->owner, sec, 0);
  // keep all referenced sections
  for (src = (amiga_reloc_type *)sec->relocation; src; src = src->next)
    {
      struct bfd_section * symsec;
      sym = *src->relent.sym_ptr_ptr;
      symsec = sym->section;
      if (!bfd_is_com_section(symsec) && !bfd_is_und_section(symsec) && !bfd_is_abs_section(symsec))
	amiga_keep_section(ht, symsec);
      else
	{
	  symsec->flags |= SEC_KEEP;
	  bfd_hash_lookup(ht, sym->name, true, true);
//	  fprintf(stderr, "object %s wants %s\n", sec->owner->filename, sym->name);
	}
    }

  // add all lists
  for (sec = sec->owner->sections; sec; sec = sec->next)
      if (0 == strncmp(".list__", sec->name, 7) || 0 == strncmp(".dlist__", sec->name, 8))
	amiga_keep_section(ht, sec);

  return true;
}

/**
 * keep this section if it resolves an unresolved symbol.
 */
static bool
amiga_collect (struct bfd_hash_table *ht, asection * sec)
{
  amiga_per_section_type *asect=amiga_per_section(sec);
  unsigned j;
  if (asect)
    for (j = 0; j < asect->amiga_symbol_count; ++j)
    {
      struct ref_section_entry *he;
      asymbol * sym = &asect->amiga_symbols[j].symbol;
      if (0 == (sym->flags & (BSF_GLOBAL | BSF_WEAK)))
	continue;

      if (bfd_is_und_section(sym->section) || bfd_is_abs_section(sym->section) || bfd_is_com_section(sym->section))
	continue;

      he = (struct ref_section_entry*)bfd_hash_lookup(ht, sym->name, false, false);
      if (he && !he->section)
	{
	  he->section = sec;
	  if (amiga_keep_section(ht, sec))
	    {
//	      fprintf(stderr, "keeping section %s for symbol %s\n", sec->name, sym->name);
	    }
	}
    }
  else
    // handle a.out
    for (j = 0; j < sec->owner->symcount; ++j)
      {
	asymbol * sym = sec->owner->outsymbols[j];
	struct ref_section_entry *he = (struct ref_section_entry*)bfd_hash_lookup(ht, sym->name, false, false);
	if (he && !he->section)
	  {
	    he->section = sec;
	    sec->flags |= SEC_KEEP;
	  }
      }
  return true;
}

/**
 * Mark all not kept as SEC_EXCLUDE
 */
static bool
amiga_purge (struct bfd_hash_table *ht, asection * sec, bool print)
{
  if (!sec->owner)
    sec->flags |= SEC_KEEP;

  if (sec->flags & SEC_KEEP)
    return true;

  if (0 == strcmp(sec->name, "COMMON") 
	  || 0 == strncmp(sec->name, ".stab", 5))
    return true;

  sec->flags |= SEC_EXCLUDE;
  if (print && sec->rawsize)
    /* xgettext:c-format */
    _bfd_error_handler (_("removing unused section '%pA' in file '%pB'"),
			sec, sec->owner);
  sec->output_section = NULL;
  return true;
}

/* Keep all symbols undefined on the command-line. */
static void
amiga_gc_keep (struct bfd_hash_table *ht, struct bfd_link_info *info)
{
  struct bfd_sym_chain *sym;
  for (sym = info->gc_sym_list; sym != NULL; sym = sym->next)
      bfd_hash_lookup(ht, sym->name, true, true);
}


static bool
amiga_gc_sections (bfd *abfd ATTRIBUTE_UNUSED, struct bfd_link_info *info)
{
  bfd *ibfd;
  asection *sec;
  struct bfd_hash_table referenced;
  unsigned i;

  bfd_hash_table_init(&referenced, ref_section_hash_newfunc, 101);

  // mark all provided symbols as wanted.
  amiga_gc_keep (&referenced, info);

  // keep all from first/startup module
  for (sec = info->input_bfds->sections; sec != NULL; sec = sec->next)
    amiga_keep_section(&referenced, sec);

  // keep all init sections starting with .list__ or .dlist__
  for (ibfd = info->input_bfds; ibfd != NULL; ibfd = ibfd->link.next)
    for (sec = ibfd->sections; sec != NULL; sec = sec->next)
      if (0 == strncmp(".list__", sec->name, 7) || 0 == strncmp(".dlist__", sec->name, 8))
    	amiga_keep_section(&referenced, sec);
  // loop until nothing new was added.
  for(i = 0;i != referenced.count;)
    {
      i = referenced.count;
      for (ibfd = info->input_bfds; ibfd != NULL; ibfd = ibfd->link.next)
	for (sec = ibfd->sections; sec != NULL; sec = sec->next)
	  amiga_collect(&referenced, sec);
    }

  // discard all not visited stuff
  for (ibfd = info->input_bfds; ibfd != NULL; ibfd = ibfd->link.next)
    for (sec = ibfd->sections; sec != NULL; sec = sec->next)
      amiga_purge(&referenced, sec, info->print_gc_sections);

  // free all relocs - these are read again, with correct output sections.
  for (ibfd = info->input_bfds; ibfd != NULL; ibfd = ibfd->link.next)
    for (sec = ibfd->sections; sec != NULL; sec = sec->next)
      {
	amiga_per_section_type *asect = amiga_per_section(sec);
	if (!asect)
	  continue;

	asect->reloc_tail = 0;
	sec->relocation = 0;
      }
  return true;
}

/* Entry points through BFD_JUMP_TABLE_GENERIC */
#define amiga_close_and_cleanup		_bfd_generic_close_and_cleanup
#define amiga_bfd_free_cached_info	_bfd_generic_bfd_free_cached_info
/* amiga_new_section_hook defined above */
/* amiga_get_section_contents defined above */
#define amiga_get_section_contents_in_window _bfd_generic_get_section_contents_in_window

/* Entry points through BFD_JUMP_TABLE_COPY */
#define amiga_bfd_merge_private_bfd_data _bfd_generic_bfd_merge_private_bfd_data
/*#define amiga_bfd_copy_private_section_data _bfd_generic_bfd_copy_private_section_data*/
#define amiga_bfd_copy_private_symbol_data _bfd_generic_bfd_copy_private_symbol_data
#define amiga_bfd_set_private_flags _bfd_generic_bfd_set_private_flags
#define amiga_bfd_print_private_bfd_data _bfd_generic_bfd_print_private_bfd_data

/* Entry points through BFD_JUMP_TABLE_ARCHIVE */
/*#define amiga_slurp_armap		bfd_slurp_armap*/
#define amiga_slurp_extended_name_table	_bfd_slurp_extended_name_table
#define amiga_construct_extended_name_table _bfd_archive_bsd_construct_extended_name_table
/*#define amiga_truncate_arname		bfd_gnu_truncate_arname*/
/*#define amiga_write_armap		bsd_write_armap*/
/*#define amiga_read_ar_hdr		_bfd_generic_read_ar_hdr*/
/*#define amiga_openr_next_archived_file	bfd_generic_openr_next_archived_file*/
#define amiga_get_elt_at_index		_bfd_generic_get_elt_at_index
/*#define amiga_generic_stat_arch_elt	bfd_generic_stat_arch_elt*/
#define amiga_update_armap_timestamp	_bfd_archive_bsd_update_armap_timestamp

/* Entry points through BFD_JUMP_TABLE_SYMBOLS */
/* amiga_get_symtab_upper_bound defined above */
/* amiga_get_symtab defined above */
/* amiga_make_empty_symbol defined above */
/* amiga_print_symbol defined above */
/* amiga_get_symbol_info defined above */
#define amiga_bfd_is_local_label_name	bfd_generic_is_local_label_name
#define amiga_get_lineno		_bfd_nosymbols_get_lineno
/* amiga_find_nearest_line defined above */
#define amiga_bfd_make_debug_symbol	_bfd_nosymbols_bfd_make_debug_symbol
#define amiga_read_minisymbols		_bfd_generic_read_minisymbols
#define amiga_minisymbol_to_symbol	_bfd_generic_minisymbol_to_symbol

/* Entry points through BFD_JUMP_TABLE_LINK
   NOTE: We use a special get_relocated_section_contents both in amiga AND in a.out files.
   In addition, we use an own final_link routine, which is nearly identical to _bfd_generic_final_link */
bfd_byte *
get_relocated_section_contents PARAMS ((bfd *, struct bfd_link_info *,
	struct bfd_link_order *, bfd_byte *, bool, asymbol **));
#define amiga_bfd_get_relocated_section_contents get_relocated_section_contents
#define amiga_bfd_relax_section		bfd_generic_relax_section
#define amiga_bfd_link_hash_table_create _bfd_generic_link_hash_table_create
#define amiga_bfd_link_hash_table_free	_bfd_generic_link_hash_table_free
#define amiga_bfd_link_add_symbols	_bfd_generic_link_add_symbols
#define amiga_bfd_link_just_syms	_bfd_generic_link_just_syms
bool amiga_final_link PARAMS ((bfd *, struct bfd_link_info *));
#define amiga_bfd_final_link		amiga_final_link
#define amiga_bfd_link_split_section	_bfd_generic_link_split_section
#define amiga_bfd_gc_sections		amiga_gc_sections
#define amiga_bfd_merge_sections	bfd_generic_merge_sections
#define amiga_bfd_discard_group		bfd_generic_discard_group

#if defined (amiga)
#undef amiga /* So that the JUMP_TABLE() macros below can work.  */
#endif

#define amiga_bfd_copy_private_header_data	_bfd_generic_bfd_copy_private_header_data
#define amiga_write_ar_hdr		_bfd_generic_write_ar_hdr
#define amiga_get_symbol_version_string	_bfd_nosymbols_get_symbol_version_string

#define amiga_find_line         _bfd_nosymbols_find_line
#define amiga_find_inliner_info _bfd_nosymbols_find_inliner_info

#define amiga_bfd_is_target_special_symbol         _bfd_bool_bfd_asymbol_false

#define amiga_set_reloc				   _bfd_generic_set_reloc
#define amiga_bfd_reloc_name_lookup          _bfd_norelocs_bfd_reloc_name_lookup

#define amiga_bfd_copy_link_hash_symbol_type \
  _bfd_generic_copy_link_hash_symbol_type
#define amiga_bfd_link_split_section               _bfd_generic_link_split_section
#define amiga_bfd_link_check_relocs                _bfd_generic_link_check_relocs

#define amiga_bfd_lookup_section_flags             bfd_generic_lookup_section_flags
#define amiga_bfd_merge_sections                   bfd_generic_merge_sections
#define amiga_bfd_is_group_section                 bfd_generic_is_group_section
#define amiga_bfd_group_name			  bfd_generic_group_name
#define amiga_bfd_discard_group                    bfd_generic_discard_group
#define amiga_section_already_linked               _bfd_generic_section_already_linked
#define amiga_bfd_define_common_symbol             bfd_generic_define_common_symbol
#define amiga_bfd_link_hide_symbol _bfd_generic_link_hide_symbol
#define amiga_bfd_define_start_stop                bfd_generic_define_start_stop


const bfd_target amiga_vec =
{
  "amiga",		/* name */
  bfd_target_amiga_flavour,
  BFD_ENDIAN_BIG,	/* data byte order */
  BFD_ENDIAN_BIG,	/* header byte order */
  HAS_RELOC | EXEC_P | HAS_LINENO | HAS_DEBUG | HAS_SYMS | HAS_LOCALS | WP_TEXT, /* object flags */
  SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_CODE | SEC_DATA, /* section flags */
  '_',			/* symbol leading char */
  ' ',			/* ar_pad_char */
  15,			/* ar_max_namelen (15 for UNIX compatibility) */
  0,				/* match priority.  */

  false, // do not keep unused symbols

  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16, /* data */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16, /* hdrs */
  {
    /* bfd_check_format */
    _bfd_dummy_target,
    amiga_object_p,
    amiga_archive_p,
    _bfd_dummy_target
  },
  {
    /* bfd_set_format */
    _bfd_bool_bfd_false_error,
    amiga_mkobject,
    amiga_mkarchive,
    _bfd_bool_bfd_false_error
  },
  {
    /* bfd_write_contents */
    _bfd_bool_bfd_false_error,
    amiga_write_object_contents,
    amiga_write_archive_contents,
    _bfd_bool_bfd_false_error
  },
  BFD_JUMP_TABLE_GENERIC (amiga),
  BFD_JUMP_TABLE_COPY (amiga),
  BFD_JUMP_TABLE_CORE (_bfd_nocore),
  BFD_JUMP_TABLE_ARCHIVE (amiga),
  BFD_JUMP_TABLE_SYMBOLS (amiga),
  BFD_JUMP_TABLE_RELOCS (amiga),
  BFD_JUMP_TABLE_WRITE (amiga),
  BFD_JUMP_TABLE_LINK (amiga),
  BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),
  NULL,
  NULL
};
