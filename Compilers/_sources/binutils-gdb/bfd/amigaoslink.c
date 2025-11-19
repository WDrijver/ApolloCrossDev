/* BFD back-end for Commodore-Amiga AmigaOS binaries. Linker routines.
   Copyright (C) 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998
   Free Software Foundation, Inc.
   Contributed by Stephan Thesing.

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
INODE
amigalink, , implementation, amiga
SECTION
	amigalink

This is the description of the linker routines for the amiga.
In fact, this includes a description of the changes made to the
a.out code, in order to build a working linker for the Amiga.
@menu
@* alterations::
@end menu

INODE
alterations, , , amigalink
SUBSECTION
	alterations

The file @file{aout-amiga.c} defines the amiga a.out backend. It differs from
the sun3 backend only in these details:
	o The @code{final_link} routine is @code{amiga_final_link}.
	o The routine to get the relocated section contents is
	  @code{get_relocated_section_contents}.

This ensures that the link is performed properly, but has the side effect of
loosing performance.

The amiga bfd code uses the same functions since they check for the used flavour.
@@*

The usage of a special linker code has one reason:
The bfd library assumes that a program is always loaded at a known memory
address. This is not a case on an Amiga. So the Amiga format has to take over
some relocs to an executable output file.
This is not the case with a.out formats, so there relocations can be applied at link time,
not at run time, like on the Amiga.
The special routines compensate this: instead of applying the relocations, they are
copied to the output file, if neccessary.
As as consequence, @code{final_link} and @code{get_relocated_section_contents} are nearly identical to
the original routines from @file{linker.c} and @file{reloc.c}.
*/

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "bfdlink.h"
#include "genlink.h"
#include "libamiga.h"

#ifndef alloca
extern void * alloca PARAMS ((size_t));
#endif

#define bfd_msg (*_bfd_error_handler)

/*#define DEBUG_AMIGA 1*/
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

/* This one is used by the linker and tells us, if a debug hunk should be
   written out */
BFDDECL int write_debug_hunk = 1;

/* This is also used by the linker to set the attribute of sections */
BFDDECL int amiga_attribute = 0;

/* This one is used to indicate base-relative linking */
BFDDECL int amiga_base_relative = 0;

/* This one is used to indicate -resident linking */
BFDDECL int amiga_resident = 0;

bool
default_indirect_link_order PARAMS ((bfd *, struct bfd_link_info *,
	 asection *, struct bfd_link_order *, bool));
bfd_byte *
get_relocated_section_contents PARAMS ((bfd *, struct bfd_link_info *,
	struct bfd_link_order *, bfd_byte *, bool, asymbol **));
bool
amiga_final_link PARAMS ((bfd *, struct bfd_link_info *));
bool
aout_amiga_final_link PARAMS ((bfd *, struct bfd_link_info *));

bool amiga_slurp_relocs PARAMS ((bfd *, sec_ptr, asymbol **));

sec_ptr amiga_make_unique_section PARAMS ((bfd *, const char *));

static bfd_reloc_status_type
my_add_to PARAMS ((arelent *, void *, int, int));
static void amiga_update_target_section PARAMS ((sec_ptr));
static bfd_reloc_status_type
amiga_perform_reloc PARAMS ((bfd *, arelent *, void *, sec_ptr, bfd *, char **));
static bfd_reloc_status_type
aout_perform_reloc PARAMS ((bfd *, arelent *, void *, sec_ptr, bfd *, char **));
static bool
amiga_reloc_link_order PARAMS ((bfd *, struct bfd_link_info *, asection *,
	struct bfd_link_order *));

enum { ADDEND_UNSIGNED=0x01, RELOC_SIGNED=0x02 };

int relocation;

extern reloc_howto_type  howto_table[10];

struct rel_chain {
  asymbol * symbol;
  signed offset;
};

static struct rel_chain * rel_jumps;
static unsigned rel_jumps_count;
static unsigned rel_jumps_max;

/* handle datadata_reloc*/
static amiga_reloc_type ** r_datadata;
static unsigned r_datadata_count;
static unsigned r_datadata_max;
static int datadata_addend;

static void
insert_long_jumps (bfd *abfd, bfd *input_bfd, asection *input_section, struct bfd_link_order *link_order,
		   bfd_byte **datap)
{
  bfd_byte *data = *datap;
  /**
   * Check here for to large pcrel relocs which are to large.
   * hack the current input_section:
   * 1. append a jmp <symbol>   4ef9 0000 0000
   * 2. perform relocation to the new jmp
   * 3. patch the relent: address and howto
   * 4. patch output_offsets of all input_sections starting behind current input_section
   * 5. patch bfd_section_reloc_link_order entries since the linker already put an offset into these.
   *
   * Since the output offsets may change, a dry run is needed to precompute the new section sizes and offsets.
   *
   * All section with an output_offset into the output .text section are handled
   * s->compressed_size : track the original size
   * s->raw_size        : how large the section should be
   * s->size            : current size
   *
   * same for input_section
   *
   */
  static asection *target;
  // on first invocation there is no content yet in the output_section
  if (0 == strcmp (input_section->output_section->name, ".text") && !input_section->output_section->contents)
    {
      target = input_section->output_section;
      // record the current sizes;
      struct bfd_link_order *lol = link_order;
      for (; lol; lol = lol->next)
	{
	  if (lol->type != bfd_indirect_link_order)
	    continue;
	  asection *s = lol->u.indirect.section;
	  if (s->owner->xvec->flavour != bfd_target_amiga_flavour)
	    continue;

//	  printf("%s:%p:%d:%d=%d -> %s:%d:%d\n", s->name, s, (int)s->output_offset, (int)s->rawsize, (int)(s->output_offset + s->rawsize),
//		 s->output_section->name, (int)s->output_section->output_offset, (int)s->output_section->rawsize);

	  if (s->output_section != target)
	    continue;

	  s->compressed_size = s->rawsize;
	}

      /**
       * determine new section sizes, loop until output_section no longer changes.
       */
      for (;;)
	{
	  unsigned totalsize = input_section->output_section->rawsize;
	  rel_jumps_count = 0;

	  lol = link_order;
	  for (; lol; lol = lol->next)
	    {
	      if (lol->type != bfd_indirect_link_order)
		continue;
	      asection *s = lol->u.indirect.section;
	      if (s->owner->xvec->flavour != bfd_target_amiga_flavour)
		continue;

	      if (s->output_section != target)
		continue;

	      if (s->reloc_count <= 0)
		continue;

	      DPRINT(10, ("%s: %d relocs\n", s->owner->filename, s->reloc_count));

	      // reset rawsize to initial size
	      unsigned cursize = s->rawsize;
	      s->rawsize = s->compressed_size;

	      amiga_reloc_type *src;
	      for (src = (amiga_reloc_type*) s->relocation; src; src = src->next)
		{
		  signed from;
		  signed to;
		  signed dist;

		  // track all accesses to ___datadata_relocs
		  if (src->relent.howto->type == H_ABS32 && 0 == strcmp ("___datadata_relocs", src->symbol->name))
		    {
		      unsigned ri = 0;
		      for (; ri < r_datadata_count; ++ri)
			if (r_datadata[ri] == src)
			  break;
		      if (ri == r_datadata_count)
			{
			  if (r_datadata_count == r_datadata_max)
			    {
			      r_datadata_max += r_datadata_max + 2;
			      r_datadata = (amiga_reloc_type**) realloc (r_datadata,
									 r_datadata_max * sizeof(amiga_reloc_type*));
			    }
			  r_datadata[r_datadata_count++] = src;
			}
		      continue;
		    }

		  if (src->relent.howto->type != H_PC16 || src->symbol->section->output_section != target)
		    continue;

		  // check if relative jump is in 16 bit range
		  to = src->symbol->section->output_offset + src->symbol->value;
		  from = s->output_offset + src->relent.address;
		  dist = to - from;

//		  fprintf(stderr, "0 sym %s: %d, tsec %p %s: %d, sec %p %s %d, rel %d\n",
//			 src->symbol->name, src->symbol->value, src->symbol->section, src->symbol->section->name, src->symbol->section->output_offset,
//			 s, s->name, s->output_offset, src->relent.address);

		  if (-32766 <= dist && dist <= 32766)
		    continue;

		  DPRINT(10, ("%s %d, ", src->symbol->name, dist));

		  // check last generated jumps
		  if (rel_jumps)
		    {
		      unsigned i = 0;
		      for (; i < rel_jumps_count; ++i)
			{
			  if (rel_jumps[i].symbol == src->symbol)
			    {
			      to = rel_jumps[i].offset;
			      dist = to - from;
			      break;
			    }
			}
		      // use an existing jump entry
		      if (-32766 <= dist && dist <= 32766)
			{
			  DPRINT(10, ("reuse %d, ", dist));
			  continue;
			}
		    }

		  // 2. calculate size for updated rel_jumps
		  unsigned slot = 0;
		  for (; slot < rel_jumps_count; ++slot)
		    {
		      if (rel_jumps[slot].offset - from < -32766)
			break;
		    }
		  if (slot == rel_jumps_max)
		    {
		      rel_jumps_max += 16;
		      rel_jumps = (struct rel_chain*) realloc (rel_jumps, sizeof(struct rel_chain) * rel_jumps_max);
		    }
		  rel_jumps[slot].symbol = src->symbol;
		  rel_jumps[slot].offset = s->rawsize + s->output_offset;
		  if (slot == rel_jumps_count)
		    ++rel_jumps_count;

		  s->rawsize += 6;
		}

	      DPRINT(10, ("\n"));

	      // update sizes and offsets
	      unsigned delta = s->rawsize - cursize;
	      if (delta == 0)
		continue;

	      // move it by that delta
	      datadata_addend += delta;

	      struct bfd_link_order *lo;

	      // 5. also patch data's link statements with bfd_section_reloc_link_order, needed for CTs
	      // can be done multiple times since only the delta to the last state is applied
	      asection *xs = target;
	      for (; xs; xs = xs->next)
		{
		  for (lo = xs->map_head.link_order; lo; lo = lo->next)
		    {
		      if (lo->type == bfd_section_reloc_link_order && lo->u.reloc.p->u.section == target
			  && lo->u.reloc.p->addend >= s->size + s->output_offset)
			lo->u.reloc.p->addend += delta;
		    }
		}

	      s->size = s->rawsize;
	      target->rawsize += delta; // update output section's size
	      lol->size += delta;

	      // 4. update output_offsets
	      for (lo = lol->next; lo; lo = lo->next)
		{
		  if (lo->type == bfd_indirect_link_order)
		    {
		      asection *ss = lo->u.indirect.section;
		      if (ss->output_section == target)
			{
			  ss->output_offset += delta;
			  lo->offset += delta;
			}
		    }
		}
	    }

	  // stop if there was no size increase
	  if (target->rawsize == totalsize)
	    break;
	}

      input_section->output_section->size = target->rawsize;

      // reset count
      rel_jumps_count = 0;

      /* increase memory for the section. */
      if (input_section->compressed_size < input_section->rawsize)
	{
	  void * odata = data;
	  data = bfd_alloc (abfd, input_section->rawsize);
	  memcpy (data, odata, input_section->compressed_size);
	  *datap = data;
	}

      /**
       * Now all sections have its final offset and size plus the old size in compressed_size.
       */
      if (datadata_addend)
	{
	  // patch all datadata_reloc refs
	  unsigned ri = 0;
	  for (; ri < r_datadata_count; ++ri)
	    {
	      amiga_reloc_type *rsrc = r_datadata[ri];
	      asymbol *sym = rsrc->symbol;
	      sym->value += (datadata_addend + 3) & ~3;
	      struct bfd_link_hash_entry *blh = (struct bfd_link_hash_entry*) sym->udata.p;
	      blh->u.def.value += (datadata_addend + 3) & ~3;
	    }

	  // patch __etext
	  unsigned oi = 0;
	  for (; oi < abfd->symcount; ++oi)
	    {
	      asymbol *sym = abfd->outsymbols[oi];
	      if (0 == strcmp ("__etext", sym->name) || 0 == strcmp ("___datadata_relocs", sym->name))
		sym->value += datadata_addend;
	    }
	}
    }

  /**
   * Create the jump tables.
   */
  if (input_section->owner->xvec->flavour == bfd_target_amiga_flavour && input_section->reloc_count > 0)
    {
      amiga_reloc_type *src;
      for (src = (amiga_reloc_type*) input_section->relocation; src; src = src->next)
	{
	  signed from;
	  signed to;
	  signed dist;
	  if (src->relent.howto->type != H_PC16 || src->symbol->section->output_section != target)
	    continue;

	  // check if relative jump is in 16 bit range
	  to = src->symbol->section->output_offset + src->symbol->value;
	  from = input_section->output_offset + src->relent.address;
	  dist = to - from;

	  if (-32766 <= dist && dist <= 32766)
	    continue;

	  // check last generated jumps
	  if (rel_jumps)
	    {
	      unsigned i = 0;
	      for (; i < rel_jumps_count; ++i)
		{
		  if (rel_jumps[i].symbol == src->symbol)
		    {
		      to = rel_jumps[i].offset;
		      dist = to - from;
		      break;
		    }
		}
	      // use an existing jump entry
	      if (-32766 <= dist && dist <= 32766)
		{
		  // 3.
		  signed relpos = src->relent.address;
		  signed noffset = rel_jumps[i].offset - input_section->output_offset - relpos;
		  data[relpos] = noffset >> 8;
		  data[relpos + 1] = noffset;

		  src->relent.address = 0x80000000;

		  DPRINT(10, ("reuse %s %d\n", src->symbol->name, dist));

		  continue;
		}
	    }

	  printf ("INFO: using long jump from %s to %s:%s\n", input_section->owner->filename,
		  src->symbol->section->owner->filename, src->symbol->name);
	  fflush (stdout);

	  // 1. append a long jump
	  signed endpos = input_section->compressed_size;
	  input_section->compressed_size += 6;
	  data[endpos] = 0x4e;
	  data[endpos + 1] = 0xf9;
	  data[endpos + 2] = 0;
	  data[endpos + 3] = 0;
	  data[endpos + 4] = 0;
	  data[endpos + 5] = 0;

	  // update rel_jumps
	  unsigned slot = 0;
	  for (; slot < rel_jumps_count; ++slot)
	    {
	      if (rel_jumps[slot].offset - from < -32766)
		break;
	    }
	  rel_jumps[slot].symbol = src->symbol;
	  rel_jumps[slot].offset = endpos + input_section->output_offset;
	  if (slot == rel_jumps_count)
	    ++rel_jumps_count;

	  // 2. apply relocation
	  signed relpos = src->relent.address;
	  signed offset = endpos - relpos;
	  data[relpos] = offset >> 8;
	  data[relpos + 1] = offset;

	  // 3. convert to ABS32 reloc
	  src->relent.howto = &howto_table[0];
	  src->relent.addend = 0;
	  src->relent.address = endpos + 2;
	}
    }

  /*
   * update the associated .stab if .stabstr
   */
  if (input_section->output_offset && 0 == strcmp(input_section->name, ".stabstr"))
    {
      sec_ptr s = input_bfd->sections;
      while (s && s->next != input_section)
	  s = s->next;

      if (s && s->output_section->contents) {
	  unsigned char * start = s->output_section->contents + s->output_offset;
	  unsigned char * end = start + s->rawsize;
	  while (start < end)
	    {
	      // update the name offset
	      unsigned noffset = bfd_getb32(start);
	      if (noffset)
		bfd_putb32(noffset + input_section->output_offset, start);

	      start += 12;
	    }
	}
    }
}


/* This one is nearly identical to bfd_generic_get_relocated_section_contents
   in reloc.c */
bfd_byte *
get_relocated_section_contents (
     bfd *abfd,
     struct bfd_link_info *link_info,
     struct bfd_link_order *link_order,
     bfd_byte *data,
     bool relocateable,
     asymbol **symbols)
{
  /* Get enough memory to hold the stuff.  */
  bfd *input_bfd = link_order->u.indirect.section->owner;
  asection *input_section = link_order->u.indirect.section;

  long reloc_size = bfd_get_reloc_upper_bound (input_bfd, input_section);
  arelent **reloc_vector = NULL;
  long reloc_count;
  bfd_reloc_status_type (*reloc_func)(bfd *, arelent *, void *, sec_ptr,
				      bfd *, char **);

  DPRINT(5,("Entering get_rel_sec_cont\n"));

  if (reloc_size < 0)
    goto error_return;

  if (bfd_get_flavour (input_bfd) == bfd_target_amiga_flavour)
    reloc_func = amiga_perform_reloc;
  else if (bfd_get_flavour (input_bfd) == bfd_target_aout_flavour)
    {
      reloc_func = aout_perform_reloc;
      if (!input_section->rawsize)
	input_section->rawsize = input_section->size;
    }
  else
    {
      bfd_set_error (bfd_error_bad_value);
      goto error_return;
    }

  reloc_vector = (arelent **) bfd_malloc ((bfd_size_type) reloc_size);
  if (reloc_vector == NULL && reloc_size != 0)
    goto error_return;

  DPRINT(5,("GRSC: GetSecCont()\n"));
  /* Read in the section.  */
  if (!bfd_get_section_contents (input_bfd,
				 input_section,
				 (void *) data,
				 (bfd_vma) 0,
				 input_section->rawsize))
    goto error_return;

  insert_long_jumps(abfd, input_bfd, input_section, link_order, &data);

  /* We're not relaxing the section, so just copy the size info.  */
  input_section->size = input_section->rawsize;
//  input_section->reloc_done = true;

  DPRINT(5,("GRSC: CanReloc\n"));
  reloc_count = bfd_canonicalize_reloc (input_bfd,
					input_section,
					reloc_vector,
					symbols);
  if (reloc_count < 0)
    goto error_return;

  if (reloc_count > 0)
    {
      arelent **parent;

      DPRINT(5,("reloc_count=%ld\n",reloc_count));

      for (parent = reloc_vector; *parent != (arelent *) NULL;
	   parent++)
	{
	  char *error_message = (char *) NULL;
	  bfd_reloc_status_type r;

	  DPRINT(5,("Applying a reloc\nparent=%lx, reloc_vector=%lx, "
		    "*parent=%lx\n",parent,reloc_vector,*parent));
	  r=(*reloc_func) (input_bfd,
			   *parent,
			   (void *) data,
			   input_section,
			   relocateable ? abfd : (bfd *) NULL,
			   &error_message);
	  if (r != bfd_reloc_ok)
	    {
	      if (relocateable)
		{
		  asection *os = input_section->output_section;

		  DPRINT(5,("Keeping reloc\n"));
		  /* A partial link, so keep the relocs.  */
		  os->orelocation[os->reloc_count] = *parent;
		  os->reloc_count++;
		}
	      else switch (r)
		{
		case bfd_reloc_undefined:
		  ((*link_info->callbacks->undefined_symbol)
			(link_info, bfd_asymbol_name (*(*parent)->sym_ptr_ptr),
			 input_bfd, input_section, (*parent)->address,
			 true));
//		    goto error_return;
		  break;
		case bfd_reloc_dangerous:
		  BFD_ASSERT (error_message != (char *) NULL);
		  ((*link_info->callbacks->reloc_dangerous)
			(link_info, error_message, input_bfd, input_section,
			 (*parent)->address));
//		    goto error_return;
		  break;
		case bfd_reloc_overflow:
		  ((*link_info->callbacks->reloc_overflow)
			(link_info, 0, bfd_asymbol_name (*(*parent)->sym_ptr_ptr),
			 (*parent)->howto->name, (*parent)->addend,
			 input_bfd, input_section, (*parent)->address));
//		    goto error_return;
		  break;
		case bfd_reloc_outofrange:
		default:
		  DPRINT(10,("get_rel_sec_cont fails, perform reloc "
			     "returned $%x\n",r));
		  fprintf(stderr, "%s: %s reloc for %s is out of range: %08x\n", abfd->filename, (*(*parent)->sym_ptr_ptr)->section->name, bfd_asymbol_name (*(*parent)->sym_ptr_ptr), relocation);
//		  abort ();
		  break;
		}
	    }
	}
    }
  if (reloc_vector != NULL)
    free (reloc_vector);
  DPRINT(5,("GRSC: Returning ok\n"));
  return data;

error_return:
  DPRINT(5,("GRSC: Error_return\n"));
  if (reloc_vector != NULL)
    free (reloc_vector);
  return NULL;
}


/* Add a value to a location */
static bfd_reloc_status_type
my_add_to (
     arelent *r,
     void * data,
     int add, int flags)
{
  bfd_reloc_status_type ret=bfd_reloc_ok;
  bfd_byte *p=((bfd_byte *)data)+r->address;
  int val;

  DPRINT(5,("Entering add_value\n"));

  switch (r->howto->size)
    {
    case 1: /* byte size */
      if ((flags & ADDEND_UNSIGNED) == 0)
	val = ((*p & 0xff) ^ 0x80) - 0x80 + add;
      else
	val = (*p & 0xff) + add;
      /* check for overflow */
      if ((flags & RELOC_SIGNED) != 0) {
	if (val<-0x80 || val>0x7f)
	  ret = bfd_reloc_overflow;
      }
      else {
	if ((val&0xffffff00)!=0 && (val&0xffffff00)!=0xffffff00)
	  ret=bfd_reloc_overflow;
      }
      /* set the value */
      *p = val & 0xff;
      break;

    case 2: /* word size */
      if ((flags & ADDEND_UNSIGNED) == 0)
	val = bfd_getb_signed_16 (p);
      else
	val = bfd_getb16 (p);

//      printf("%p: %04x+%04x=%05x %s %s\n", p, val, add, val + add, (*r->sym_ptr_ptr)->name, (*r->sym_ptr_ptr)->the_bfd->filename);
      val += add;

      /* check for overflow */
      if ((flags & RELOC_SIGNED) != 0) {
	if (val<-0x8000 || val>0x7fff)
	  ret = bfd_reloc_overflow;
      }
      else {
	if ((val&0xffff0000)!=0 && (val&0xffff0000)!=0xffff0000)
	  ret=bfd_reloc_overflow;
      }
      /* set the value */
      bfd_putb16 (val, p);
      break;

    case 4: /* long word */
      val = bfd_getb_signed_32 (p) + add;
      /* If we are linking a resident program, then we limit the reloc size
	 to about +/- 1 GB.

	 When linking a shared library all variables defined in other
	 libraries are placed in memory >0x80000000, so if the library
	 tries to use one of those variables an error is output.

	 Without this it would be much more difficult to check for
	 incorrect references. */
      if (amiga_resident &&
	  (val & 0xc0000000)!=0 && (val&0xc0000000)!=0xc0000000) /* Overflow */
	{
	  ret=bfd_reloc_overflow;
	}
      bfd_putb32 (val, p);
      break;

    default: /* Error */
      ret=bfd_reloc_notsupported;
      break;
    }/* Of switch */

  DPRINT(5,("Leaving add_value\n"));
  return ret;
}


/* For base-relative linking place .bss symbols in the .data section.  */
static void
amiga_update_target_section (
     sec_ptr target_section)
{
  /* If target->out is .bss, add the value of the .data section to
     sym->value and set new output_section */
  /* If we access a symbol in the .bss section, we have to convert
     this to an access to .data section */
  /* This is done through a change to the output section of
     the symbol.. */
  if (!strcmp(target_section->output_section->name,".bss"))
    {
      /* get value for .data section */
      bfd *ibfd;
      sec_ptr s;

      ibfd=target_section->output_section->owner;
      for (s=ibfd->sections;s!=NULL;s=s->next)
	if (!strcmp(s->name,".data"))
	  {
	    target_section->output_offset+=s->rawsize;
	    target_section->output_section=s;
	  }
    }
}


/* Perform an Amiga relocation */
static bfd_reloc_status_type
amiga_perform_reloc (
     bfd *abfd,
     arelent *r,
     void * data,
     sec_ptr sec,
     bfd *obfd,
     char **error_message ATTRIBUTE_UNUSED)
{
  asymbol *sym; /* Reloc is relative to sym */
  sec_ptr target_section; /* reloc is relative to this section */
  bfd_reloc_status_type ret;
  bool copy;
  int flags;
  bool hard_reloc = AMIGA_DATA(sec->output_section->owner)->vma_reloc;

  DPRINT(5,("Entering APR\nflavour is %d (amiga_flavour=%d, aout_flavour=%d)\n",
	    bfd_get_flavour (sec->owner), bfd_target_amiga_flavour,
	    bfd_target_aout_flavour));

  /* If obfd==NULL: Apply the reloc, if possible. */
  /* Else: Modify it and return */

  if (obfd!=NULL) /* Only modify the reloc */
    {
      r->address+=sec->output_offset;
      sec->output_section->flags|=SEC_RELOC;
      DPRINT(5,("Leaving amiga_perf_reloc, modified\n"));
      return bfd_reloc_ok;
    }

  /* Try to apply the reloc */

  sym=*(r->sym_ptr_ptr);

  target_section = sym->section;
  if (target_section->kept_section)
      sym->section = target_section = target_section->kept_section;

  if (bfd_is_und_section(target_section)) /* Error */
    {
      DPRINT(10,("amiga_perf_reloc: target_sec==UND\n"));
      return bfd_reloc_undefined;
    }

  relocation=0; flags=RELOC_SIGNED; copy=false; ret=bfd_reloc_ok;

  DPRINT(5,("%s: size=%u\n",r->howto->name,bfd_get_reloc_size(r->howto)));
  switch (r->howto->type)
    {
    case H_ABS16:
    case H_ABS32:
      if (hard_reloc)
	relocation= sym->value + target_section->output_offset + target_section->output_section->vma;
      else if (bfd_is_abs_section(target_section)) /* Ref to absolute hunk */
	relocation=sym->value;
      else if (bfd_is_com_section(target_section)) /* ref to common */
	{
	  relocation=0;
	  copy=true;
	}
      else
	{
	  if (0 == strcmp(sec->name, ".stab") || 0 == strcmp(sec->name, ".stabstr"))
	    {
	      relocation=sym->value + target_section->output_offset;
//		  printf("stab: %s %s %d+%d=%d\n", sym->section->name, sym->name, sym->value, target_section->output_offset, relocation);
	    }
	  else
	    {
	    if (amiga_base_relative)
	      amiga_update_target_section (target_section);
	    relocation=0;
	    copy=true;
  	  }
	}
      break;

    case H_PC16:
      if (r->address == 0x80000000)
	return bfd_reloc_ok;
      /* fall through */
    case H_PC8: /* pcrel */
    case H_PC32:
      if (bfd_is_abs_section(target_section)) /* Ref to absolute hunk */
	relocation=sym->value;
      else if (bfd_is_com_section(target_section)) /* Error.. */
	{
	  ret=bfd_reloc_undefined;
	}
      else if (sec->output_section!=target_section->output_section) /* Error */
	{
	  DPRINT(5,("pc relative, but out-of-range\n"));
	  ret=bfd_reloc_outofrange;
	}
      else /* Same section */
	{
	  DPRINT(5,("PC relative\n"));


	      relocation = sym->value + target_section->output_offset
	    - sec->output_offset - r->address;
	}
      break;

    case H_SD8: /* baserel */
    case H_SD16:
    case H_SD32:
      /* Relocs are always relative to the symbol ___a4_init */
      /* Relocs to .bss section are converted to a reloc to .data section,
	 since .bss section contains only COMMON sections...... and should
	 be following .data section.. */
      if (bfd_is_abs_section(target_section))
	relocation=sym->value;
      else if (!AMIGA_DATA(target_section->output_section->owner)->baserel)
	{
	  bfd_msg ("Base symbol for base relative reloc not defined: "
		   "section %s, reloc to symbol %s",sec->name,sym->name);
	  ret=bfd_reloc_notsupported;
	}
      else if ((target_section->flags&SEC_CODE)!=0)
        {
	  bfd_msg ("%s: baserelative text relocation to \"%s\"",
		    abfd->filename, sym->name);
	  ret=bfd_reloc_notsupported;
        }
      else
	{
	  amiga_update_target_section (target_section);
	  relocation = sym->value + target_section->output_offset + r->addend;

	  DPRINT(20,("symbol=%s (0x%lx)\nsection %s (0x%lx; %s; output=0x%lx)"
		     "\nrelocation @0x%lx\n", sym->name, sym->value,
		     target_section->name, target_section,
		     target_section->owner->filename, target_section->output_offset,
		     r->address));

	  relocation -= (AMIGA_DATA(target_section->output_section->owner))->a4init;
	}
      break;

    default:
      bfd_msg ("Error: unsupported reloc: %s(%d)",r->howto->name,r->howto->size);
      ret=bfd_reloc_notsupported;
      break;
    }/* Of switch */

  /* Add in relocation */
  if (relocation!=0)
    ret = my_add_to (r, data, relocation, flags);

  if (copy) /* Copy reloc to output section */
    {
      DPRINT(5,("Copying reloc\n"));
      target_section=sec->output_section;
      r->address+=sec->output_offset;
      target_section->orelocation[target_section->reloc_count++]=r;
      target_section->flags|=SEC_RELOC;
    }
  DPRINT(5,("Leaving amiga_perf_reloc with %d (OK=%d)\n",ret,bfd_reloc_ok));
  return ret;
}

/* Perform an a.out relocation */
static bfd_reloc_status_type
aout_perform_reloc (
     bfd *abfd,
     arelent *r,
     void * data,
     sec_ptr sec,
     bfd *obfd,
     char **error_message ATTRIBUTE_UNUSED)
{
  asymbol *sym; /* Reloc is relative to sym */
  sec_ptr target_section; /* reloc is relative to this section */
  bfd_reloc_status_type ret;
  bool copy;
  int flags;

  DPRINT(5,("Entering aout_perf_reloc\n"));

  /* If obfd==NULL: Apply the reloc, if possible. */
  /* Else: Modify it and return */

  if (obfd!=NULL) /* Only modify the reloc */
    {
      r->address+=sec->output_offset;
      DPRINT(5,("Leaving aout_perf_reloc, modified\n"));
      return bfd_reloc_ok;
    }

  /* Try to apply the reloc */

  sym=*(r->sym_ptr_ptr);
  target_section=sym->section;

  if (bfd_is_und_section(target_section)) /* Error */
    {
      if ((sym->flags & BSF_WEAK) == 0)
        {
	  DPRINT(10,("aout_perf_reloc: target_sec==UND\n"));
	  return bfd_reloc_undefined;
	}
      target_section=bfd_abs_section_ptr;
    }

  relocation=0; flags=RELOC_SIGNED; copy=false; ret=bfd_reloc_ok;

  DPRINT(10,("RELOC: %s: size=%u\n",r->howto->name,bfd_get_reloc_size(r->howto)));
  switch (r->howto->type)
    {
    case H_ABS8: /* 8/16 bit reloc, pc relative or absolute */
    case H_ABS16:
      if (bfd_is_abs_section(target_section)) /* Ref to absolute hunk */
	relocation=sym->value;
      else if (bfd_is_com_section(target_section)) /* Error.. */
	{
	  bfd_msg ("pc relative relocation to common symbol \"%s\" in "
		   "section %s",sym->name,sec->name);
	  DPRINT(10,("Ref to common symbol...aout_perf_reloc\n"));
	  ret=bfd_reloc_undefined;
	}
      else if (sec->output_section!=target_section->output_section)
	{
	  // SBF: the if statement has been fixed, but the linker does no longer work.
	  // if ((target_section->output_section->flags&SEC_DATA)!=0)
	  // I replaced it with the old (obvious bogus) statement
	  // TODO: a better fix?
	  //if (target_section->output_section->flags & (SEC_DATA != 0))
	  if (amiga_base_relative)
	    goto baserel; /* Dirty, but no code duplication.. */
	  bfd_msg ("pc relative relocation out-of-range in section %s. "
		   "Relocation was to symbol %s",sec->name,sym->name);
	  DPRINT(10,("Section %s, target %s: Reloc out-of-range...not same "
		     "section, aout_perf\nsec->out=%s, target->out=%s, "
		     "offset=%lx\n",sec->name,target_section->name,
		     sec->output_section->name,
		     target_section->output_section->name,r->address));
	  ret=bfd_reloc_outofrange;
	}
      else
	{
	  /* Same section, this is a pc relative hunk... */
	  DPRINT(5,("Reloc to same section...\n"));
	  relocation=-(long)(r->address+sec->output_offset);
	}
      break;

    case H_ABS32: /* 32 bit reloc, pc relative or absolute */
      if (bfd_is_abs_section(target_section)) /* Ref to absolute hunk */
	relocation=sym->value;
      else if (bfd_is_com_section(target_section)) /* ref to common */
	{
	  relocation=0;
	  copy=true;
	}
      else
	{
	  if (amiga_base_relative)
	    amiga_update_target_section (target_section);
	  relocation=0;
	  copy=true;
	}
      DPRINT(10,("target->out=%s(%lx), sec->out=%s(%lx), symbol=%s\n",
		 target_section->output_section->name,
		 target_section->output_section,
		 sec->output_section->name,
		 sec->output_section,
		 sym->name));
      break;

     case H_PC16:
      if (r->address == 0x80000000)
	return bfd_reloc_ok;
      // fall through
    case H_PC8: /* pcrel */
    case H_PC32:
      if (bfd_is_abs_section(target_section)) /* Ref to absolute hunk */
	relocation=sym->value;
      else if (sec->output_section!=target_section->output_section) /* Error */
	{
	  DPRINT(5,("pc relative, but out-of-range\n"));
	  ret=bfd_reloc_outofrange;
	}
      else /* Same section */
	{
	  relocation = sym->value + target_section->output_offset
	    - sec->output_offset;
	}
      break;

    case H_SD16: /* baserel */
    case H_SD32:
    baserel:
      /* We use the symbol ___a4_init as base */
      if (bfd_is_abs_section(target_section))
	relocation=sym->value;
      else if (bfd_is_com_section(target_section)) /* Error.. */
	{
	  bfd_msg ("baserelative relocation to common \"%s\"",sym->name);
	  DPRINT(10,("Ref to common symbol...aout_perf_reloc\n"));
	  ret=bfd_reloc_undefined;
	}
      else if (!AMIGA_DATA(target_section->output_section->owner)->baserel)
	{
	  bfd_msg ("Base symbol for base relative reloc not defined: "
		   "section %s, reloc to symbol %s",sec->name,sym->name);
	  ret=bfd_reloc_notsupported;
	}
      else if ((target_section->flags&SEC_CODE)!=0)
        {
	  bfd_msg ("%s: baserelative text relocation to \"%s\"",
		    abfd->filename, sym->name);
	  ret=bfd_reloc_notsupported;
        }
      else /* Target section and sec need not be the same.. */
	{
	  amiga_update_target_section (target_section);
	  relocation = sym->value + target_section->output_offset;
	  /* if the symbol is in .bss, subtract the offset that gas has put
	     into the opcode */
	  if (target_section->index == 2 && !(sym->flags & BSF_GLOBAL))
	    relocation -= adata(abfd).datasec->rawsize;
	  DPRINT(20,("symbol=%s (0x%lx)\nsection %s (0x%lx; %s; output=0x%lx)"
		     "\nrelocation @0x%lx\n", sym->name, sym->value,
		     target_section->name, target_section,
		     target_section->owner->filename, target_section->output_offset,
		     r->address));

	  relocation -= (AMIGA_DATA(target_section->output_section->owner))->a4init;
	}
      DPRINT(10,("target->out=%s(%lx), sec->out=%s(%lx), symbol=%s\n",
		 target_section->output_section->name,
		 target_section->output_section,
		 sec->output_section->name,
		 sec->output_section,
		 sym->name));
      break;

    default:
      bfd_msg ("Error: unsupported reloc: %s(%d)",r->howto->name,r->howto->size);
      ret=bfd_reloc_notsupported;
      break;
    }/* Of switch */

  /* Add in relocation */
  if (relocation!=0)
    ret = my_add_to (r, data, relocation, flags);

  if (copy) /* Copy reloc to output section */
    {
      DPRINT(5,("Copying reloc\n"));
      target_section=sec->output_section;
      r->address+=sec->output_offset;
      target_section->orelocation[target_section->reloc_count++]=r;
    }
  DPRINT(5,("Leaving aout_perf_reloc with %d (OK=%d)\n",ret,bfd_reloc_ok));
  return ret;
}


/* The final link routine, used both by Amiga and a.out backend */
/* This is nearly a copy of linker.c/_bfd_generic_final_link */
bool
amiga_final_link (
     bfd *abfd,
     struct bfd_link_info *info)
{
  bfd *sub;
  asection *o;
  struct bfd_link_order *p;
  size_t outsymalloc;
  struct generic_write_global_symbol_info wginfo;
  struct bfd_link_hash_entry *h = bfd_link_hash_lookup (info->hash, "___a4_init", false, false, true);
  struct bfd_link_hash_entry *h2 = bfd_link_hash_lookup (info->hash, "___a4_init2", false, false, true);

  /**
   * insert an empty data section if bss exists.
   */
  if (amiga_base_relative)
    {
      asection * ds = NULL;
      asection * bs = NULL;
      asection * s;
      for (s = abfd->sections; s != NULL; s = s->next)
	{
	  if (!strcmp(s->name, ".data"))
	    ds = s;
	  if (!strcmp(s->name, ".bss"))
	    bs = s;
	}
      if (bs && !ds)
	{
	  asection * section = bs->output_section;
	  section->name = ".data";
	  section->flags |= SEC_ALLOC | SEC_LOAD | SEC_DATA | SEC_HAS_CONTENTS | SEC_IN_MEMORY;
	  section->contents = (unsigned char *)bfd_malloc(8); // for empty data_data_relocs
	  amiga_per_section(section)->disk_size = 0;
	}
    }
  if (amiga_base_relative && h2 && h2->type == bfd_link_hash_defined)
    AMIGA_DATA(abfd)->a4init = h2->u.def.value;
  else
    AMIGA_DATA(abfd)->a4init = 0x7ffe;

  AMIGA_DATA(abfd)->baserel = amiga_base_relative;
//  printf("br=%d, off=%d %p %p\n", amiga_base_relative, AMIGA_DATA(abfd)->a4init, h ? h->u.def.value : 0, h2 ? h2->u.def.value : 0);

  DPRINT(5,("Entering final_link\n"));

  if (bfd_get_flavour (abfd) == bfd_target_aout_flavour)
    return aout_amiga_final_link (abfd, info);

  abfd->outsymbols = (asymbol **) NULL;
  abfd->symcount = 0;
  outsymalloc = 0;

  /* Mark all sections which will be included in the output file.  */
  for (o = abfd->sections; o != NULL; o = o->next)
    for (p = o->map_head.link_order; p != NULL; p = p->next)
      if (p->type == bfd_indirect_link_order)
	p->u.indirect.section->linker_mark = true;

  /* Build the output symbol table.  */
  for (sub = info->input_bfds; sub != (bfd *) NULL; sub = sub->link.next)
    if (! _bfd_generic_link_output_symbols (abfd, sub, info, &outsymalloc))
      return false;

  DPRINT(10,("Did build output symbol table\n"));

  /* Accumulate the global symbols.  */
  wginfo.info = info;
  wginfo.output_bfd = abfd;
  wginfo.psymalloc = &outsymalloc;
  _bfd_generic_link_hash_traverse (_bfd_generic_hash_table (info),
				   _bfd_generic_link_write_global_symbol,
				   (void *) &wginfo);

  DPRINT(10,("Accumulated global symbols\n"));

  DPRINT(10,("Output bfd is %s(%lx)\n",abfd->filename,abfd));

  if (1)
    {
      /* Allocate space for the output relocs for each section.  */
      /* We also handle base-relative linking special, by setting the .data
	 sections real length to it's length + .bss length */
      /* This is different to bfd_generic_final_link: We ALWAYS alloc space
	 for the relocs, because we may need it anyway */
      for (o = abfd->sections;
	   o != (asection *) NULL;
	   o = o->next)
	{
	  DPRINT(10,("Section in output bfd is %s (%lx)\n",o->name,o));

	  o->reloc_count = 0;
	  for (p = o->map_head.link_order;
	       p != (struct bfd_link_order *) NULL;
	       p = p->next)
	    {
	      if (p->type == bfd_section_reloc_link_order
		  || p->type == bfd_symbol_reloc_link_order)
		++o->reloc_count;
	      else if (p->type == bfd_indirect_link_order)
		{
		  asection *input_section;
		  bfd *input_bfd;
		  long relsize;
		  arelent **relocs;
		  asymbol **symbols;
		  long reloc_count;

		  input_section = p->u.indirect.section;
		  input_bfd = input_section->owner;

		  DPRINT(10,("\tIndirect section from bfd %s, section is %s(%lx) "
			     "(COM=%lx)\n",
			     input_bfd->filename,input_section->name,input_section,
			     bfd_com_section_ptr));

		  relsize = bfd_get_reloc_upper_bound (input_bfd,
						       input_section);
		  if (relsize < 0)
		    {
		      DPRINT(10,("Relsize<0.I..in bfd %s, sec %s\n",
				 input_bfd->filename, input_section->name));
		      return false;
		    }
		  relocs = (arelent **) bfd_malloc ((bfd_size_type) relsize);
		  if (!relocs && relsize != 0)
		    return false;
		  symbols = _bfd_generic_link_get_symbols (input_bfd);
		  reloc_count = bfd_canonicalize_reloc (input_bfd,
							input_section,
							relocs,
							symbols);
		  free (relocs);
		  if (reloc_count < 0)
		    {
		      DPRINT(10,("Relsize<0.II..in bfd %s, sec %s\n",
				 input_bfd->filename, input_section->name));
		      return false;
		    }
		  BFD_ASSERT ((unsigned long) reloc_count
			      == input_section->reloc_count);
		  o->reloc_count += reloc_count;
		}
	    }
	  if (o->reloc_count > 0)
	    {
	      bfd_size_type amt;

	      amt = o->reloc_count;
	      amt *= sizeof (arelent *);
	      o->orelocation = (arelent **) bfd_alloc (abfd, amt);
	      if (!o->orelocation)
		return false;
	      /* o->flags |= SEC_RELOC; There may be no relocs. This can
		 be determined later only */
	      /* Reset the count so that it can be used as an index
		 when putting in the output relocs.  */
	      o->reloc_count = 0;
	    }
	}
    }

  DPRINT(10,("Got all relocs\n"));

  /* SBF: check if some offset is given, then all relocs are resolved using output_section->vma. */
  for (o = abfd->sections->output_section; o != NULL; o = o->next)
    if (o->vma != 0 && (o->prev == NULL || o->prev->size + o->prev->vma != o->vma) && 0 == strncmp(".text", o->name, 5))
      {
	AMIGA_DATA(abfd)->vma_reloc = true;
	break;
      }

  /* Handle all the link order information for the sections.  */
  for (o = abfd->sections;
       o != (asection *) NULL;
       o = o->next)
    {
      for (p = o->map_head.link_order;
	   p != (struct bfd_link_order *) NULL;
	   p = p->next)
	{
	  switch (p->type)
	    {
	    case bfd_section_reloc_link_order:
	    case bfd_symbol_reloc_link_order:
	      if (! amiga_reloc_link_order (abfd, info, o, p)) /* We use an own routine */
		return false;
	      break;
	    case bfd_indirect_link_order:
	      if (! default_indirect_link_order (abfd, info, o, p, false))
		/* Calls our get_relocated_section_contents */
		return false;
	      break;
	    default:
	      if (! _bfd_default_link_order (abfd, info, o, p))
		return false;
	      break;
	    }
	}
    }

  if (bfd_get_flavour(abfd)==bfd_target_amiga_flavour
      && !bfd_link_relocatable (info)
      )
    AMIGA_DATA(abfd)->IsLoadFile = true;

  DPRINT(10,("Leaving final_link\n"));
  return true;
}


/* Handle reloc link order.
   This is nearly a copy of linker.c/_bfd_generic_reloc_link_order */
static bool
amiga_reloc_link_order (
     bfd *abfd,
     struct bfd_link_info *info,
     asection *sec,
     struct bfd_link_order *link_order)
{
  arelent *r;

  DPRINT(5,("Entering amiga_reloc_link_order\n"));

  if (sec->orelocation == (arelent **) NULL)
    {
      DPRINT(10,("aborting, since orelocation==NULL\n"));
      abort ();
    }

  /* We generate a new ***AMIGA*** style reloc */
  r = (arelent *) bfd_zalloc (abfd, (bfd_size_type) sizeof (amiga_reloc_type));
  if (r == (arelent *) NULL)
    {
      DPRINT(5,("Leaving amiga_reloc_link, no mem\n"));
      return false;
    }

  r->address = link_order->offset;
  r->howto = bfd_reloc_type_lookup (abfd, link_order->u.reloc.p->reloc);
  if (r->howto == 0)
    {
      bfd_set_error (bfd_error_bad_value);
      DPRINT(5,("Leaving amiga_reloc_link, bad value\n"));
      return false;
    }

  /* Get the symbol to use for the relocation.  */
  if (link_order->type == bfd_section_reloc_link_order)
    r->sym_ptr_ptr = link_order->u.reloc.p->u.section->symbol_ptr_ptr;
  else
    {
      struct generic_link_hash_entry *h;

      h = ((struct generic_link_hash_entry *)
	   bfd_wrapped_link_hash_lookup (abfd, info,
					 link_order->u.reloc.p->u.name,
					 false, false, true));
      if (h == (struct generic_link_hash_entry *) NULL
	  || ! h->written)
	{
	  ((*info->callbacks->unattached_reloc)
		 (info, link_order->u.reloc.p->u.name,
		  (bfd *) NULL, (asection *) NULL, (bfd_vma) 0));
//	    return false;
	  bfd_set_error (bfd_error_bad_value);
	  DPRINT(5,("Leaving amiga_reloc_link, bad value in hash lookup\n"));
	  return false;
	}
      r->sym_ptr_ptr = &h->sym;
    }
  DPRINT(5,("Got symbol for relocation\n"));
  /* Store the addend */
  r->addend = link_order->u.reloc.p->addend;

  /* If we are generating relocateable output, just add the reloc */
  /* Try to apply the reloc */
    {
      void * data=(void *)sec->contents;
      bfd_reloc_status_type ret;
      char *em=NULL;

      DPRINT(5,("Apply link_order_reloc\n"));

      /* FIXME: Maybe, we have to get the section contents, before we
	  use them, if they have not been set by now.. */
      BFD_ASSERT (data!=NULL);

      if (bfd_get_flavour(abfd)==bfd_target_amiga_flavour)
	ret=amiga_perform_reloc(abfd,r,data,sec,NULL,&em);
      else
	ret=aout_perform_reloc(abfd,r,data,sec,NULL,&em);

      if (ret!=bfd_reloc_ok)
	{
	  if (bfd_link_relocatable (info))
	    {
	      DPRINT(5,("Adding reloc\n"));
	      sec->orelocation[sec->reloc_count] = r;
	      ++sec->reloc_count;
	      sec->flags|=SEC_RELOC;
	    }
	  else
	    {
	      DPRINT(5,("Leaving amiga_reloc_link, value false\n"));
	      return false;
	    }
	}
    }
  DPRINT(5,("Leaving amiga_reloc_link\n"));
  return true;
}
