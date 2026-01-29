## PSUBW

> Context (from PSUB page)
>
> _Bron: AC68080PRM PDF, pagina 56_
>
> PSUB                                                                   PSUB
> Vector substract
> Operation: b – a  → d
> Syntax: PSUBB <vea>,b,d
> PSUBW <vea>,b,d
> PSUBUSB <vea>,b,d
> PSUBUSW <vea>,b,d
> Condition Codes: not affected
> Short: Vector subtract word.
> Description: Psubb is 8byte vector, It calculates 8 substractions.  Psubw is 4 
> word vector. This is for 4 substractions. Unsigned Saturated has a lower limit and 
> an upper limit. When the result is below zero it is clipped to be zero. Same for 
> above maximum.
> 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
> 1 1 1 1 1 1 1 A B D Mode Register
> b d 0 0 0 1 0 U 1 S
> S=Size 0=byte else word.
> U=1 Unsigned Saturated else signed & no limiting.
>
> ### Extra notes (AMMX reference)
> Source: ammx.txt

### PSUBW
mnemonic: psubw (VEA),b,d

 short: vector subtract short

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
  |     b0-a0     | |     b1-a1     | |     b2-a2     | |     b3-a3     |
  ----------------- ----------------- ----------------- -----------------
          |            ____/     ____________/                 /
          |           /         /         ____________________/
          |          /         /         /
      -----------------------------------------
      |   d0    |   d1    |   d2    |   d3    |
      -----------------------------------------


 equivalent C Code:
   int i;
   short a[4];
   short b[4];
   short d[4];

   for( i = 0 ; i<4 ; i++ )
   {
	d[i] = a[i] - b[i];
   }


 typical application cases:

  PADDW is a plain and simple vectorized 16 Bit subtraction operation that 
  performs four independent sub.w operations in one shot.
