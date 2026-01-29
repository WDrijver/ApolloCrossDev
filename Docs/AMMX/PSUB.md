## PSUB
_Bron: AC68080PRM PDF, pagina 56_

PSUB                                                                   PSUB
Vector substract
Operation: b – a  → d
Syntax: PSUBB <vea>,b,d
PSUBW <vea>,b,d
PSUBUSB <vea>,b,d
PSUBUSW <vea>,b,d
Condition Codes: not affected
Short: Vector subtract word.
Description: Psubb is 8byte vector, It calculates 8 substractions.  Psubw is 4 
word vector. This is for 4 substractions. Unsigned Saturated has a lower limit and 
an upper limit. When the result is below zero it is clipped to be zero. Same for 
above maximum.
15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
1 1 1 1 1 1 1 A B D Mode Register
b d 0 0 0 1 0 U 1 S
S=Size 0=byte else word.
U=1 Unsigned Saturated else signed & no limiting.

### Extra notes (AMMX reference)
Source: ammx.txt

#### PSUBB

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

#### PSUBW

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

#### PSUBUSB

mnemonic: psubusb (VEA),b,d

 short: vector subtract unsigned bytes with unsigned saturation

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
    ------------------ ------------------ ------------------
    | max(0,(b0-a0)) | | max(0,(b1-a1)) | | max(0,(b7-a7)) |
    ------------------ ------------------ ------------------
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
	d[i] = max( 0, ( b[i] - a[i] ));
   }


 typical application cases:

  Psubusb is useful when dealing with 8 Bit pixel data. All subtractions
  are guaranteed to be kept in the 8 Bit range. Subtraction results <0
  are saturated to 0.

#### PSUBUSW

mnemonic: psubusw (VEA),b,d

 short: vector subtract unsigned short with saturation

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
    |min($ffff,b0-a0| |min($ffff,b1-a1| |min($ffff,b2-a2| |min($ffff,b3-a3|
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
	d[i] = min(0xffff, b[i] - a[i] );
   }


 typical application cases:

  PSUBUSW might come in handy for pixel manipulation with accuracy demands
  beyond 8 Bit per gun. Subtraction results are implicitly saturated, leaving
  the complete 16 Bit range available plus the ability to subtract dithering 
  offsets, for example.

---
