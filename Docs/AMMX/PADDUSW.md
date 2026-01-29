## PADDUSW

> Context (from PADD page)
>
> _Bron: AC68080PRM PDF, pagina 42_
>
> PADD                                                                  PADD
> Vector add
> Operation: a + b → d
> Syntax: PADDB <vea>,b,d
> PADDW <vea>,b,d
> PADDUSB <vea>,b,d
> PADDUSW <vea>,b,d
> Condition Codes: not affected
> Short: Vector add word.
> Description: Paddb is 8byte vector, It calculates 8 additions.  Paddw is 4 word 
> vector. This is for 4 additions. Unsigned Saturated has a lower limit and an upper 
> limit. Result above maximum are clipped to maximum.
> 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
> 1 1 1 1 1 1 1 A B D Mode Register
> b d 0 0 0 1 0 U 1 S
> S=Size 0=byte else word.
> U=1 Unsigned Saturated else signed & no limiting.
>
> ### Extra notes (AMMX reference)
> Source: ammx.txt

### PADDUSW
mnemonic: paddusw (VEA),b,d

 short: vector add unsigned short with saturation

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
          |         |         |         \_____________________
          |         \______   \______________                 \
          |                \                 \                 \
    ----------------- ----------------- ----------------- -----------------
    |min($ffff,b0+a0| |min($ffff,b1+a1| |min($ffff,b2+a2| |min($ffff,b3+a3|
    ----------------- ----------------- ----------------- -----------------
          |            ____/     ____________/                 /
          |           /         /         ____________________/
          |          /         /         /
      -----------------------------------------
      |   d0    |   d1    |   d2    |   d3    |
      -----------------------------------------


 equivalent C Code:
   int i;
   unsigned short a[4];
   unsigned short b[4];
   unsigned short d[4];

   for( i = 0 ; i<4 ; i++ )
   {
	d[i] = min(0xffff, a[i] + b[i] );
   }


 typical application cases:

  PADDUSW might come in handy for pixel manipulation with accuracy demands
  beyond 8 Bit per gun. Addition results are implicitly saturated, leaving
  the complete 16 Bit range available plus the ability to add dithering 
  offsets, for example.

---
