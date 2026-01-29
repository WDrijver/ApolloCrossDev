## PSUBB

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

### PSUBB
mnemonic: psubb (VEA),b,d

 short: vector subtract bytes

 graphic:
     input a
      -----------------------------------------
      | a0 | a1 | a2 | a3 | a4 | a5 | a6 | a7 | 
      -----------------------------------------
        |    |          ...                |
	|    |                             |
     input b                                            
      -----------------------------------------
      | b0 | b1 | b2 | b3 | b4 | b5 | b6 | b7 |
      -----------------------------------------
        |    |          ...                |
	|    |                             |
        |    |                             |
	|    |                             |
        |    |                             \ 
        |    \_____________       ...       \ 
         \                 \                 \_____ 
          |                 \                      \    
     -----------        -----------            -----------
     |  b0-a0  |        |  b1-a1  |   ...      |  b7-a7  |
     -----------        -----------            -----------
          |    _____________/   ...          ______/
	  |   /                             /
      -----------------------------------------
      | d0 | d1 | d2 | d3 | d4 | d5 | d6 | d7 |
      -----------------------------------------


 equivalent C Code:
   int i;
   char a[8]; /* signed inputs */
   char b[8];
   char d[8];

   for( i = 0 ; i<8 ; i++ )
   {
	d[i] = b[i] - a[i];
   }


 typical application cases:

  Whenever a byte subtraction is needed and overflows either won't happen or
  are expected, this is the vector variant of sub.b.
