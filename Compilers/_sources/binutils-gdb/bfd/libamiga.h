/* BFD back-end for Commodore-Amiga AmigaOS binaries. Data structures.
   Copyright (C) 1992, 1993, 1994, 1995, 1996, 1997, 1998
   Free Software Foundation, Inc.
   Contributed by Leonard Norrgard.
   Extended by Stephan Thesing 11/1994.

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

/* Hunk ID numbers.  */

#define HUNK_UNIT		999
#define HUNK_NAME		1000
#define HUNK_CODE		1001
#define HUNK_DATA		1002
#define HUNK_BSS		1003
#define HUNK_RELOC32		1004
#define HUNK_ABSRELOC32		HUNK_RELOC32
#define HUNK_RELOC16		1005
#define HUNK_RELRELOC16		HUNK_RELOC16
#define HUNK_RELOC8		1006
#define HUNK_RELRELOC8		HUNK_RELOC8
#define HUNK_EXT		1007
#define HUNK_SYMBOL		1008
#define HUNK_DEBUG		1009
#define HUNK_END		1010
#define HUNK_HEADER		1011
#define HUNK_OVERLAY		1013
#define HUNK_BREAK		1014
#define HUNK_DREL32		1015
#define HUNK_DREL16		1016
#define HUNK_DREL8		1017
#define HUNK_LIB		1018
#define HUNK_INDEX		1019
#define HUNK_RELOC32SHORT	1020
#define HUNK_RELRELOC32		1021
#define HUNK_ABSRELOC16		1022
/* EHF extensions */
#define HUNK_PPC_CODE		1257
#define HUNK_RELRELOC26		1260

/* The hunk ID part.  */

#define HUNK_VALUE(hunk_id)	((hunk_id) & 0x3fffffff)

/* Attributes of a hunk.  */

#define HUNK_ATTRIBUTE(hunk_id)	((hunk_id) >> 30)
#define HUNK_ATTR_CHIP 		0x01	/* hunk content must go into chip ram */
#define HUNK_ATTR_FAST		0x02	/* fast */
#define HUNK_ATTR_FOLLOWS	0x03	/* mem id follows */

/* HUNK_EXT subtypes.  */

#define EXT_SYMB		0
#define EXT_DEF			1
#define EXT_ABS			2
#define EXT_RES			3
#define EXT_REF32		129
#define EXT_ABSREF32		EXT_REF32
#define EXT_COMMON		130
#define EXT_ABSCOMMON		EXT_COMMON
#define EXT_REF16		131
#define EXT_RELREF16		EXT_REF16
#define EXT_REF8		132
#define EXT_RELREF8		EXT_REF8
#define EXT_DEXT32		133
#define EXT_DEXT16		134
#define EXT_DEXT8		135
#define EXT_RELREF32		136
#define EXT_RELCOMMON		137
#define EXT_ABSREF16		138
#define EXT_ABSREF8		139
/* VBCC extensions */
#define EXT_DEXT32COMMON	208
#define EXT_DEXT16COMMON	209
#define EXT_DEXT8COMMON		210
/* EHF extensions */
#define EXT_RELREF26		229

/* HOWTO types almost matching aoutx.h/howto_table_std.  */

enum {
  H_ABS8=0,H_ABS16,H_ABS32,H_ABS32SHORT,H_PC8,H_PC16,H_PC32,H_PC32SHORT,H_SD8,H_SD16,H_SD32,H_PC26
};

/* Various structures.  */

typedef struct amiga_reloc {
  arelent relent;
  struct amiga_reloc *next;
  asymbol *symbol;
} amiga_reloc_type;

/* Structure layout *must* match libaout.h/struct aout_symbol.  */

typedef struct amiga_symbol {
  asymbol symbol;
  short desc;
  char other;
  unsigned char type;
  /* amiga data */
  unsigned long index,refnum;
} amiga_symbol_type;

typedef struct aname_list {
  struct aname_list * next;
  char const * name;
} aname_list_type;

/* We take the address of the first element of an asymbol to ensure that the
   macro is only ever applied to an asymbol.  */
#define amiga_symbol(asymbol) ((amiga_symbol_type *)(&(asymbol)->the_bfd))

typedef struct raw_reloc {
  unsigned long num,pos,hunk;
  struct raw_reloc *next;
} raw_reloc_type;

typedef struct amiga_per_section {
  amiga_reloc_type *reloc_tail; /* last reloc, first is in section->relocation */
  int attribute; /* Memory type required by this section */
  unsigned long disk_size; /* Section size on disk, _raw_size may be larger than this */
  amiga_symbol_type *amiga_symbols; /* the symbols for this section */
  unsigned int amiga_symbol_count;
  unsigned long hunk_ext_pos; /* offset of hunk_ext in the bfd file */
  unsigned long hunk_symbol_pos; /* offset of hunk_symbol in the bfd file */
  raw_reloc_type *relocs;
  aname_list_type * sym_names;
  unsigned int gc_count;
} amiga_per_section_type;

#define amiga_per_section(x) ((amiga_per_section_type *)((x)->used_by_bfd))

/* Structure layout *must* match libaout.h/struct aoutdata.  */

struct amiga_data {
  char *dummy[2];
  sec_ptr textsec;
  sec_ptr datasec;
  sec_ptr bsssec;
  file_ptr sym_filepos;
  file_ptr str_filepos;
  /* rest for compatibility only */
  /* Size of a relocation entry in external form.  */
  unsigned reloc_entry_size;

  /* Size of a symbol table entry in external form.  */
  unsigned symbol_entry_size;

  /* Page size - needed for alignment of demand paged files.  */
  unsigned long page_size;

  /* Segment size - needed for alignment of demand paged files.  */
  unsigned long segment_size;

  /* Zmagic disk block size - need to align the start of the text
     section in ZMAGIC binaries.  Normally the same as page_size.  */
  unsigned long zmagic_disk_block_size;

  unsigned exec_bytes_size;
  unsigned vma_adjusted : 1;

  /* Used when a bfd supports several highly similar formats.  */
  enum
    {
      default_format = 0,
      /* Used on HP 9000/300 running HP/UX.  See hp300hpux.c.  */
      gnu_encap_format,
      /* Used on Linux, 386BSD, etc.  See include/aout/aout64.h.  */
      q_magic_format
    } subformat;

  enum
    {
      undecided_magic = 0,
      z_magic,
      o_magic,
      n_magic
    } magic;

  /* A buffer for find_nearest_line.  */
  char *line_buf;

  /* The external symbol information.  */
  struct external_nlist *external_syms;
  bfd_size_type external_sym_count;
  bfd_window sym_window;
  char *external_strings;
  bfd_size_type external_string_size;
  bfd_window string_window;
  struct aout_link_hash_entry **sym_hashes;

  /* A pointer for shared library information.  */
  void * dynamic_info;

  /* A mapping from local symbols to offsets into the global offset
     table, used when linking on SunOS.  This is indexed by the symbol
     index.  */
  bfd_vma *local_got_offsets;


  carsym *symdefs;		/* the symdef entries */
  symindex symdef_count;	/* how many there are */
};

struct aout_symbol;
typedef struct amiga_data_struct {
  struct amiga_data a;
  unsigned long symtab_size;
  unsigned long stringtab_size;
  amiga_symbol_type *symbols;
  bool IsLoadFile; /* If true, this is a load file (for output bfd only) */
  unsigned int nb_hunks;
  /* The next two fields are set at final_link time (for the output bfd only) */
  bool baserel;/* true if there is ___init_a4 in the global hash table */
  bfd_vma a4init;     /* cache the value for efficiency */
  struct aout_symbol ** stab_symbols;
  struct bfd_hash_table * section_lookup;

  bool vma_reloc; /* SBF: relocate using vma addresses. */
} amiga_data_type;

#define adata(bfd)	((bfd)->tdata.amiga_data->a)
#define AMIGA_DATA(bfd)	((bfd)->tdata.amiga_data)

#define HUNKB_ADVISORY	29
#define HUNKB_CHIP	30
#define HUNKB_FAST	31
#define HUNKF_ADVISORY	(1L << HUNKB_ADVISORY)
#define HUNKF_CHIP	(1L << HUNKB_CHIP)
#define HUNKF_FAST	(1L << HUNKB_FAST)

#ifndef MEMF_ANY
#define MEMF_ANY	(0L)
#define MEMF_PUBLIC	(1L << 0)
#define MEMF_CHIP	(1L << 1)
#define MEMF_FAST	(1L << 2)
#define MEMF_LOCAL	(1L << 8)
#define MEMF_24BITDMA	(1L << 9)
#define MEMF_KICK	(1L << 10)
#define MEMF_CLEAR	(1L << 16)
#define MEMF_LARGEST	(1L << 17)
#define MEMF_REVERSE	(1L << 18)
#define MEMF_TOTAL	(1L << 19)
#define MEMF_NO_EXPUNGE	(1L << 31)
#endif /* MEMF_ANY */

#ifndef NULL
#define NULL 0
#endif

#ifndef PARAMS
#define PARAMS(a) a
#endif


struct ref_section_entry
{
  struct bfd_hash_entry root;
  asection * section;
};

extern
struct bfd_hash_entry *
ref_section_hash_newfunc (struct bfd_hash_entry *entry,
			struct bfd_hash_table *table, const char *string);
