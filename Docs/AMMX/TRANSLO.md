## TRANSLO

> Context (from TRANS page)
>
> _Bron: AC68080PRM PDF, pagina 64_
>
> TRANS                                                             TRANS
> Transposition
> Operation: takes bytes from 4 sources and places them in 2 destinations.
> Syntax: TRANSHI a0-a3,d:d2
> TRANSLO a0-a3,d:d2
> Condition Codes: not affected
> Short: Matrix transposition, upper half or lower half
> Description: Translo puts longs from 4 following registers in 2 64bit registers.
> Constraints: This instruction does not support memory operands. The four 
> inputs and the destination  must be consecutive registers. The first source a is 
> constrained to a multiple of 4 (i.e. D0-D3,D4-D7,E0-E3,...,E20-E23). The 
> destination register index pair (d:d2) are restricted to a multiple of two (i.e. 
> D0:D1,D2:D3 etc.)
> 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
> 1 1 1 1 1 1 1 A 0 D 0 0 a a 0 0
> 0 0 0 0 d d d 0 0 0 0 0 0 0 1 L
> L=1 translo else transhi
> Example:
>     translo    d0-d3,d6:d7
> src
> 0 0 1 1 2 2 3 3
> 4 4 5 5 6 6 7 7
> 8 8 9 9 A A B B
> C C D D E E F F
> dst
> 0 0 1 1 2 2 3 3 4 4 5 5 6 6 7 7
> 8 8 9 9 A A B B C C D D E E F F
>
> ### Extra notes (AMMX reference)
> Source: ammx.txt

### TRANSLO
mnemonic: translo a-d , e:f

 short:    matrix transposition, lower half

 graphic:
    input: 4 consecutive registers
      -----------------------------------------
      | a0 | a1 | a2 | a3 | a4 | a5 | a6 | a7 | 
      -----------------------------------------
                                            
      -----------------------------------------
      | b0 | b1 | b2 | b3 | b4 | b5 | b6 | b7 |
      -----------------------------------------
                                            
      -----------------------------------------
      | c0 | c1 | c2 | c3 | c4 | c5 | c6 | c7 |
      -----------------------------------------
                                            
      -----------------------------------------
      | d0 | d1 | d2 | d3 | d4 | d5 | d6 | d7 |
      -----------------------------------------

    output: two consecutive registers

      -----------------------------------------
      | a4 | a5 | b4 | b5 | c4 | c5 | d4 | d5 | 
      -----------------------------------------
                                            
      -----------------------------------------
      | a6 | a7 | b6 | b7 | c6 | c7 | d6 | d7 |
      -----------------------------------------
 
 constraints:
  This instruction does not support memory operands. The four inputs (a-d) and the
  destination (e,f) must be consecutive registers. The first source a is constrained to
  a multiple of 4 (i.e. D0-D3,D4-D7,E0-E3,...,E20-E23). The destination register index
  pair (e,f) are restricted to a multiple of two (i.e. D0:D1,D2:D3 etc.).


 equivalent C Code:
   int i;
   unsigned short a[16]; /* 4 registers of 8 bytes each */
   unsigned short e[4];  /* 1 register of 8 bytes */
   unsigned short f[4];  /* 1 register of 8 bytes */

   for( i = 0 ; i<4 ; i++ )
   {
	e[i] = a[ 4*i + 2 ];
	f[i] = a[ 4*i + 3 ];
   }


 typical application cases:

   This instruction allows a 4x4 matrix transposition in just two CPU cycles (in conjunction
   with transhi). The extension towards larger matrices (8x8,16x16 etc.) is straightforward.

 example:
   load  (a0),E0
   load  8(a0),E1
   load  16(a0),E2
   load  24(a0),E3
   transhi E0-E3,E4:E5  ;transpose upper (left) half of matrix (to complete example)
   translo E0-E3,E6:E7	;transpose lower (right) half of matrix
