## STOREILM

> Context (from STORE page)
>
> _Bron: AC68080PRM PDF, pagina 57_
>
> STORE                                                             STORE
> Store register into memory
> Operation: b →  <vea>
> Syntax: STORE b,<vea>
> Condition Codes: not affected
> Short: Store 64 bit from source register in memory 
> Description: Store is the AMMX equivalent of move dn,<ea>
> 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
> 1 1 1 1 1 1 1 A B 0 Mode Register
> b 0 0 0 0 0 0 0 0 0 1 0 0
> Not allowed <vea> :  #imm
>
> ### Extra notes (AMMX reference)
> Source: ammx.txt

### STOREILM
mnemonic: storeilm  a,m,(VEA)

 short: store bytes from a into destination, based on inverted long mask 

 graphic:
     input a
      -----------------------------------------
      | a0 | a1 | a2 | a3 | a4 | a5 | a6 | a7 |
      -----------------------------------------
        |    |            ...              |
        |    |                             |
     logical input: destination contents
      -----------------------------------------
      | d0 | d1 | d2 | d3 | d4 | d5 | d6 | d7 |
      -----------------------------------------
        |    |            ...              |
        |    |                             |
     input m: lower 8 bit of second argument
      -----------------------------------------
      | m0 | m1 | m2 | m3 | m4 | m5 | m6 | m7 |
      -----------------------------------------
        \___  \____________     ...         \___
            \              \                    \
      -------------- --------------     -------------- 
      | (m0&128) ? | | (m1&128) ? | ... | (m7&128) ? | 
      | a0 : d0    | | a1 : d1    |     |  a7 : d7   |
      -------------- --------------     -------------- 
         ___/  ____________/         ...     ___/
	|     /                             /
      -----------------------------------------
      | d0 | d1 | d2 | d3 | d4 | d5 | d6 | d7 |
      -----------------------------------------


 equivalent C Code:
   int i;
   unsigned char a[8];
   unsigned char m[8];
   unsigned char d[8];

   for( i = 0 ; i<8 ; i++ )
   {
	d[i] = ( m[i] & 128 ) ? d[i] : a[i];
   }


 typical application cases:

  It is usual in SIMD that you can write only the native amount of bytes at once.
  This instruction enables selective overwriting of memory, based on the contents
  of the mask register. Depending on your needs, you might consider either storem
  or storeilm for particular problems.

 examples:
  load.w #$ffff,d0       ;-1.q
  storeilm E0,d0,(a0)	 ;this example stores nothing :-)
  peor   d0,d0,d0        ;0.q 
  storeilm E0,d0,(a0)    ;is the same as store E0,(a0)

  ; another way of color key (see also storem)
  pcmpeqw.w #$f81f,E0,E2 ;magenta HiColor RGB565 pixel(s) in E0 ?
  storeilm  E0,E2,(a0)   ;store only words where the magenta check didn't match

STORE M3

 mnemonic: storem3  a,#mode,(VEA)

 short: store a selection of bytes,words,longwords into destination

 This in is designed to highly simplify coding of 2D cookie-cut operations.t
 It can operate in 4 modes.
 1) Byte mode
  In this mode each byte of the source is stored to the destination which
  has not the value of $00.
  This is perfect for copying sprites where $00 indicated transparent.

 2) WORD / 16bit mode
  In this mode each WORD of the source is stored to the destination which
  has not the value of $F81F. 
  This is perfect for copying sprites where PINK color indicated transparent.

 3) WORD / 15bit mode
  In this mode each WORD of the source is stored to the destination which
  has not Bit(15) set. 
  This is perfect for copying sprites in 32K color mode. 
 
 4) LONG 
  In this mode each LONG of the source is stored to the destination which
  has not Bit(31) set. 
  This is perfect for copying sprites in truecolor mode


 equivalent C Code:
   int i;
   unsigned char a[8];
   unsigned char m;
   unsigned char d[8];

   for( i = 0 ; i<8 ; i++ )
   {
        if( a[i] != 0x00 ){
	   d[i] = a[i];
        }
   }



 examples:

  load      (a0)+,D0 
  storem3   D0,#1,(a1)+   ;copy all non transparent bytes to screen

---
