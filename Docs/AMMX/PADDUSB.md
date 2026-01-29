## PADDUSB

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

### PADDUSB
mnemonic: paddusb (VEA),b,d

 short: vector add unsigned bytes with unsigned saturation

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
    ------------------ ------------------     ------------------
    |min(255,(a0+b0))| |min(255,(a1+b1))| ... |min(255,(a7+b7))|
    ------------------ ------------------     ------------------
          |    _____________/   ...          ______/
	  |   /                             /
      -----------------------------------------
      | d0 | d1 | d2 | d3 | d4 | d5 | d6 | d7 |
      -----------------------------------------


 equivalent C Code:
   int i;
   unsigned char a[8]; 
   unsigned char b[8];
   unsigned char d[8];

   for( i = 0 ; i<8 ; i++ )
   {
	d[i] = min( 255, ( a[i] + b[i] ));
   }


 typical application cases:

  Paddusb is useful when dealing with 8 Bit pixel data. All additions
  are guaranteed to be kept in the 8 Bit range. Addition results >255
  are saturated to 255.
