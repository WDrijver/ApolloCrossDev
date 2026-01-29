## STORE

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

### STORE
mnemonic: store a,(VEA)

 short: store 64 bit from source register in memory 

 equivalent C Code:
  {
   unsigned long long a;
   unsigned long lond dest;

   dest = a;
  }

 typical application cases:

  Store is the AMMX equivalent to move dn,. It unconditionally writes
  64 Bit into the destination. In typical cases, the destination is some
  location in memory. You may store to any valid address in Chip- and 
  FastRAM. There are no alignment restrictions.

 examples:
  store E0,(a0)
  store E1,9(a0)
  
STORE COUNT

 mnemonic: storec  a,count,(VEA)

 short: store at most "count" bytes from a into destination

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
     input count: number of bytes to write
      -----------------------------------------
      |                   |  count            |
      -----------------------------------------
        \___  \____________     ...         \___
            \              \                    \
      -------------- --------------     -------------- 
      | count>0 ?  | | count>1 ?  | ... | count>7 ?  | 
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
   int           count;
   unsigned char d[8];

   for( i = 0 ; i<8 ; i++ )
   {
   	if( (count - i) > 0 )
	 d[i] = a[i];
   }


 typical application cases:

  Memcopy. You can always read/write with 64 Bit instructions and won't have
  to worry about overwritten locations (as long as you count the written bytes
  within the loop, ofc).

 examples:
  move.l #1523,d0
 .loop
  load   (a0)+,E0
  storec E0,d0,(a1)+
  subq.l #8,d0
  bgt    .loop

STORE MASK

 mnemonic: storem  a,m,(VEA)

 short: store a selection of bytes from a into destination

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
      |    |    |    |    |    |    |    |  m |
      -----------------------------------------
        \___  \____________     ...         \___
            \              \                    \
      -------------- -------------     ---------------- 
      | (m&128) ?  | | (m&64) ?  | ... |  (m&1) ?     | 
      | a0 : d0    | | a1 : d1   |     |  a7 : d7     |
      -------------- -------------     ---------------- 
         ___/  ____________/         ...     ___/
	|     /                             /
      -----------------------------------------
      | d0 | d1 | d2 | d3 | d4 | d5 | d6 | d7 |
      -----------------------------------------


 equivalent C Code:
   int i;
   unsigned char a[8];
   unsigned char m;
   unsigned char d[8];

   for( i = 0 ; i<8 ; i++ )
   {
	d[i] = ( m & (1<<(7-i)) ) ? a[i] : d[i];
   }


 typical application cases:

  It is usual in SIMD that you can write only the native amount of bytes at once.
  This instruction enables selective overwriting of memory, based on the contents
  of the mask register (lower 8 Bit).

  This instruction has several uses. Perhaps the most prominent application is for
  cookie-cut: selective writing of pixels from sprites to the screen. 

 examples:
  move.b #$ff,d0
  storem E0,d0,(a0)	;this example is the same as store E0,(a0)
  move.b #$f0,d0
  storem E0,d0,8(a0)     ;overwrite 4 bytes from 8(a0) with upper 32 bit of E0

  ; one way of color key (see also storeilm)
  pcmpeqw.w #$f81f,E0,E2 ;magenta HiColor RGB565 pixel(s) in E0 ?
  c2p       e2,e2        ;re-order: get bits from word mask into one byte
  peor.w    #$ffff,e2,e2 ;negate mask (logical: !magenta)
  storem    E0,E2,(a0)   ;store only words where the magenta check didn't match
