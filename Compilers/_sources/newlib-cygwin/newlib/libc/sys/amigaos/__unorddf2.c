/* This is a stripped down version of floatlib.c.  It supplies only those
   functions which exist in libgcc, but for which there is not assembly
   language versions in m68k/lb1sf68.S.

   It also includes simplistic support for extended floats (by working in
   double precision).  You must compile this file again with -DEXTFLOAT
   to get this support.  */

/*
** gnulib support for software floating point.
** Copyright (C) 1991 by Pipeline Associates, Inc.  All rights reserved.
** Permission is granted to do *anything* you want with this file,
** commercial or otherwise, provided this message remains intact.  So there!
** I would appreciate receiving any updates/patches/changes that anyone
** makes, and am willing to be the repository for said changes (am I
** making a big mistake?).
**
** Pat Wood
** Pipeline Associates, Inc.
** pipeline!phw@motown.com or
** sun!pipeline!phw or
** uunet!motown!pipeline!phw
**
** 05/01/91 -- V1.0 -- first release to gcc mailing lists
** 05/04/91 -- V1.1 -- added float and double prototypes and return values
**                  -- fixed problems with adding and subtracting zero
**                  -- fixed rounding in truncdfsf2
**                  -- fixed SWAP define and tested on 386
*/

/*
** The following are routines that replace the gnulib soft floating point
** routines that are called automatically when -msoft-float is selected.
** The support single and double precision IEEE format, with provisions
** for byte-swapped machines (tested on 386).  Some of the double-precision
** routines work at full precision, but most of the hard ones simply punt
** and call the single precision routines, producing a loss of accuracy.
** long long support is not assumed or included.
** Overall accuracy is close to IEEE (actually 68882) for single-precision
** arithmetic.  I think there may still be a 1 in 1000 chance of a bit
** being rounded the wrong way during a multiply.  I'm not fussy enough to
** bother with it, but if anyone is, knock yourself out.
**
** Efficiency has only been addressed where it was obvious that something
** would make a big difference.  Anyone who wants to do this right for
** best speed should go in and rewrite in assembler.
**
** I have tested this only on a 68030 workstation and 386/ix integrated
** in with -msoft-float.
*/

/* Prototypes for the above in case we use them.  */
double __floatunsidf (unsigned long);
double __floatsidf (long);
float __floatsisf (long);
double __extendsfdf2 (float);
float __truncdfsf2 (double);
long __fixdfsi (double);
long __fixsfsi (float);

/* the following deal with IEEE single-precision numbers */
#define EXCESS		126L
#define SIGNBIT		0x80000000L
#define HIDDEN		(1L << 23L)
#define SIGN(fp)	((fp) & SIGNBIT)
#define EXP(fp)		(((fp) >> 23L) & 0xFF)
#define MANT(fp)	(((fp) & 0x7FFFFFL) | HIDDEN)
#define PACK(s,e,m)	((s) | ((e) << 23L) | (m))

/* the following deal with IEEE double-precision numbers */
#define EXCESSD		1022L
#define HIDDEND		(1L << 20L)
#define EXPDBITS	11
#define EXPDMASK	0x7FFL
#define EXPD(fp)	(((fp.l.upper) >> 20L) & 0x7FFL)
#define SIGND(fp)	((fp.l.upper) & SIGNBIT)
#define MANTD(fp)	(((((fp.l.upper) & 0xFFFFF) | HIDDEND) << 10) | \
				(fp.l.lower >> 22))
#define MANTDMASK	0xFFFFFL /* mask of upper part */

/* the following deal with IEEE extended-precision numbers */
#define EXCESSX		16382L
#define HIDDENX		(1L << 31L)
#define EXPXBITS	15
#define EXPXMASK	0x7FFF
#define EXPX(fp)	(((fp.l.upper) >> 16) & EXPXMASK)
#define SIGNX(fp)	((fp.l.upper) & SIGNBIT)
#define MANTXMASK	0x7FFFFFFFL /* mask of upper part */

union double_long
{
  double d;
  struct {
      long upper;
      unsigned long lower;
    } l;
};

union float_long {
  float f;
  long l;
};

union long_double_long
{
  long double ld;
  struct
    {
      long upper;
      unsigned long middle;
      unsigned long lower;
    } l;
};

int
__unorddf2(double a, double b)
{
  union double_long dl;

  dl.d = a;
  if (EXPD(dl) == EXPDMASK
      && ((dl.l.upper & MANTDMASK) != 0 || dl.l.lower != 0))
    return 1;
  dl.d = b;
  if (EXPD(dl) == EXPDMASK
      && ((dl.l.upper & MANTDMASK) != 0 || dl.l.lower != 0))
    return 1;
  return 0;
}

