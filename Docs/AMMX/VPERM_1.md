## VPERM

> Context (from VPERM page)
>
> _Bron: AC68080PRM PDF, pagina 66_
>
> VPERM                                                          VPERM
> Vector Permute
> Operation: Pick bytes from a & b → d
> Syntax: VPERM #n,a,b,d
> where #n contains the picking order from a & b
> Condition Codes: not affected
> Short: Permute the contents of two registers into destination register.
> Constraints: The operands a, b & d must be data registers.
>
> |15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
> |---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
> |1|1|1|1|1|1|1|A|B|D|1|1|1|1|1|1|
>
> b d 0 0 0 0 a
> s0 pos0 s1 pos1 s2 pos2 s3 pos3
> s4 pos4 s5 pos5 s6 pos6 s7 pos7
> S=0 takes pos from a else from b.
> Example:
>     vperm    #$3210AB78,d0,e1,e6
> src d0
> 0 0 1 1 2 2 3 3 4 4 5 5 6 6 7 7
> src e1
> 8 8 9 9 A A B B C C D D E E F F
> dst e6
> 3 3 2 2 1 1 0 0 A A B B 7 7 8 8
>
> ### Extra notes (AMMX reference)
> Source: ammx.txt

### VPERM
mnemonic: vperm #N,a,b,d

 short: permute the contents of two registers into destination register

 graphic:
      ---------------------------------
      | N0 N1 | N2 N3 | N4 N5 | N6 N7 | (4 Bit each)
      ---------------------------------

      -----------------------------------------
      | a0 | a1 | a2 | a3 | a4 | a5 | a6 | a7 |
      -----------------------------------------
         \  /     |    |              |    |   -----------------------------------------
          \/     /     |          ___/    /    | b0 | b1 | b2 | b3 | b4 | b5 | b6 | b7 |
          /\    /      |         /       /     -----------------------------------------
         |  \  /       |        /   ____/________/                        |
         |   \/        |       /   /   /      (example permutation)       |          
         |   /\_       |      /   /   /       (  #N = #$1203687d  )      /          
         |   |  \      |     /   |    |     ____________________________/            
         |   |   \     |    /    |    |    /                                          
      -----------------------------------------
      | e0 | e1 | e2 | e3 | e4 | e5 | e6 | e7 |
      -----------------------------------------

  constraints:
  This instruction does not support memory operands. The two inputs (a,b) and the
  destination (d) must be registers. The permutation constant N must be given at assembly 
  time.


 equivalent C Code:
   int i,n;
   unsigned char a[8];
   unsigned char b[8];
   unsigned char d[8];
   unsigned char N[4];

   for( i = 0 ; i<8 ; i++ )
   {
   	n = N[i>>1] >> (i&1)*4; /* get next 4 bits from N */
        if( n < 8 )
	     d[i] = a[n];
	else
	     d[i] = b[n-8];
   }


 typical application cases:

   Flexible shuffling of bytes can be necessary virtually everywhere in SIMD-World.

 example:
   ; convert 8 Bit pixel data to 16 Bit
   load  (a0),E1    ;load 8 Bytes          
   moveq #0,d0      ;D0 = 0 (lower 32 Bit)
   vperm #$48494a4b,d0,E1,E2 ; first 4 Bytes of E1 into 4 Words (insert zeros from d0)
   vperm #$4c4d4e4f,d0,E1,E3 ; second 4 Bytes of E1 into 4 Words

---
