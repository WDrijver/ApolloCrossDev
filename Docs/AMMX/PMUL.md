## PMUL
_Bron: AC68080PRM PDF, pagina 53_

PMUL                                                                PMUL
Vector multiply
Operation: a x b → d
Syntax: PMULH <vea>,b,d
PMULL <vea>,b,d
PMUL88 <vea>,b,d
Condition Codes: not affected
Short: Vector multiply short 
Description: Pmul is 4 word vector multiply. Pmulh keeps upper 16 bits. Pmull 
keeps lower 16 bits. Pmul88 keeps the middle part, bit 23..8.
15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
1 1 1 1 1 1 1 A B D Mode Register
b d 0 0 0 1 1 0 T
T type
0=pmul88
1=pmula (next page)
2=pmulh
3=pmull

### Extra notes (AMMX reference)
Source: ammx.txt

#### PMUL88

mnemonic: pmul88  (VEA),b,d

 short: vector multiply short and shift down 8

 graphic:
     input a
      -----------------------------------------
      |   a0    |   a1    |   a2    |   a3    |
      -----------------------------------------
        |           |         |         |
        |           |         |         |
     input b 
      -----------------------------------------
      |   b0    |   b1    |   b2    |   b3    |
      -----------------------------------------
          |         |         |         |
          |         |         |         \_____________
          \         \____     \_________              \
           |             \              \              \
    -------------- -------------- -------------- -------------- 
    | (b0*a0)>>8 | | (b1*a1)>>8 | | (b3*a3)>>8 | | (b3*a3)>>8 | 
    -------------- -------------- -------------- -------------- 
          |           ___/      ________/ _____________/
	  |          /         /         /
      -----------------------------------------
      |   d0    |   d1    |   d2    |   d3    |
      -----------------------------------------


 equivalent C Code:
   int i;
   short a[4]; /* signed inputs */
   short b[4];
   short d[4];

   for( i = 0 ; i<4 ; i++ )
   {
	d[i] = ( a[i] * b[i] ) >> 8;
   }


 typical application cases:

  PMUL88 is effectively a multiply of a 16.0 integer with an 8.8 fixed point.
  As such, it can be used for short-range shifts as well. 
  
  Should accuracy and dynamic range not fit the particular needs, please
  have a look at PMULL and PMULH.

 examples:
  pmul88.w #64,E0,E1   ; E1 = (E0*64)>>8 = E0>>2
  pmul88.w #1024,E0,E2 ; E2 = (E0*1024)>>8 = E0<<2
  
PMULA

#### PMULL

mnemonic: pmull  ,b,d

 short: vector multiply short and keep lower 16 Bit

 graphic:
     input a
      -----------------------------------------
      |   a0    |   a1    |   a2    |   a3    |
      -----------------------------------------
        |           |         |         |
        |           |         |         |
     input b 
      -----------------------------------------
      |   b0    |   b1    |   b2    |   b3    |
      -----------------------------------------
          |         |         |         |
          |         |         |         \_____________
          \         \____     \_________              \
           |             \              \              \
    -------------- -------------- -------------- -------------- 
    |(b0*a0)&ffff| |(b1*a1)&ffff| |(b3*a3)&ffff| |(b3*a3)&ffff| 
    -------------- -------------- -------------- -------------- 
          |           ___/      ________/ _____________/
	  |          /         /         /
      -----------------------------------------
      |   d0    |   d1    |   d2    |   d3    |
      -----------------------------------------


 equivalent C Code:
   int i;
   short a[4]; /* signed inputs */
   short b[4];
   short d[4];

   for( i = 0 ; i<4 ; i++ )
   {
	d[i] = ( a[i] * b[i] ) & 0xffff; /* the AND is here for   */
	                                 /* clarification only... */
   }


 typical application cases:

  PMULL mostly applies to cases where the result is known to fit in 16 Bit. 
  Usually, it is expected that PMULL is used in tandem with PMULH to
  obtain a 16x16 = 32 Bit result.
  
  Another application for PMULL is to serve as a left shift operator. Being
  a multiplication instruction, pass 2^(shift value) instead of the number
  of bits.

 examples:
  pmull.w  #54,E0,E2   ; E2 = E0*54
  pmull.w  #1024,E0,E3 ; E3 = E0<<10
  ;16*16 = 32 Bit multiplication example
  pmull.w  E4,E5,E6    ;low part  l0 l1 l2 l3
  pmulh.w  E4,E5,E7    ;high part h0 h1 h2 h3
  vperm    #$018923ab,E7,E6,E8 ; h0 l0 h1 l1
  vperm    #$45cd67ef,E7,E6,E9 ; h2 l2 h3 l3

#### PMULH

mnemonic: pmulh  (VEA),b,d

 short: vector multiply short and keep upper 16 Bit

 graphic:
     input a
      -----------------------------------------
      |   a0    |   a1    |   a2    |   a3    |
      -----------------------------------------
        |           |         |         |
        |           |         |         |
     input b 
      -----------------------------------------
      |   b0    |   b1    |   b2    |   b3    |
      -----------------------------------------
          |         |         |         |
          |         |         |         \_____________
          \         \____     \_________              \
           |             \              \              \
    -------------- -------------- -------------- -------------- 
    | (b0*a0)>>16| | (b1*a1)>>16| | (b3*a3)>>16| | (b3*a3)>>16| 
    -------------- -------------- -------------- -------------- 
          |           ___/      ________/ _____________/
	  |          /         /         /
      -----------------------------------------
      |   d0    |   d1    |   d2    |   d3    |
      -----------------------------------------


 equivalent C Code:
   int i;
   short a[4]; /* signed inputs */
   short b[4];
   short d[4];

   for( i = 0 ; i<4 ; i++ )
   {
	d[i] = ( a[i] * b[i] ) >> 16;
   }


 typical application cases:

  PMULH is useful on it's own in cases where one of the operands is a 
  fixed-point fractional variable with an absolute value <1. In said cases,
  the usual renormalizing shift is not necessary. Application cases like
  trigonometric transforms and 3D matrix transforms come to mind.

  PMULH teams up with PMULL when 16x16 = 32 Bit results are of interest.

 examples:
  pmulh.w  #1024,E0,E2 ; E2 = (E0*1024)>>16 = E0>>6

---
