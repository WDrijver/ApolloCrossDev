# BSEL

## Bit Select

Operation: 64x  b=1 ?  then  a → d
Syntax: BSEL <vea>,mask,d
Condition Codes: not affected
Short: Bitwise selection from <vea> , taken if b=1
Description: Masked bits are taken from <vea> , unmasked stays d.
 This instruction allows a bit-by-bit selection of data from two sources into the 
destination. Typically, this is applied in conjunction with a prior pcmp instruction. 
15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
1 1 1 1 1 1 1 A B D Mode Register
b(mask) d 0 0 1 0 1 0 0 1
src <vea>
0 1 2 3 4 5 6 7 8 9 A B C D E F
selector b
0 0 0 F F F C 0 0 0 C F F F F 0
dst d before
5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5
dst d after
5 5 5 3 4 5 5 5 5 5 9 B C D E 5

mnemonic: bsel (VEA),b,d

 short: 

 graphic:
     input1:  (bytes l-s with their bits 7...0)
      -------------------------------------------------------------------------
      |llllllll|mmmmmmmm|nnnnnnnn|oooooooo|pppppppp|qqqqqqqq|rrrrrrrr|ssssssss|
      |76543210|76543210|76543210|76543210|76543210|76543210|76543210|76543210|
      -------------------------------------------------------------------------
       |      |                                                       |      |
       | ...  |                   ...                                 | ...  |
       |      |                                                       |      |
     input3: (contents of destination register d, with bytes d-k)     |      |
      -------------------------------------------------------------------------
      |dddddddd|eeeeeeee|ffffffff|gggggggg|hhhhhhhh|iiiiiiii|jjjjjjjj|kkkkkkkk|
      |76543210|76543210|76543210|76543210|76543210|76543210|76543210|76543210|
      -------------------------------------------------------------------------
       |      |                                                       |      |
       | ...  |                   ...                                 | ...  |
       |      |                                                       |      |
     input2: (contents of register b with bytes b,c,t-y)              |      |
      -------------------------------------------------------------------------
      |bbbbbbbb|cccccccc|tttttttt|uuuuuuuu|vvvvvvvv|wwwwwwww|xxxxxxxx|yyyyyyyy|
      |76543210|76543210|76543210|76543210|76543210|76543210|76543210|76543210|
      -------------------------------------------------------------------------
       |       \                                                      |      |
       | ...    \_________________    ...                      ______/  ...   \___
       |                          \                           /                   \
       |                           \                         /                     \
   --------------------    --------------------    --------------------    --------------------
   | (b7&l7)|(!b7&d7) | .. | (b0&l0)|(!b0&d0) | .. | (y7&s7)|(!y7&k7) | .. | (y0&s0)|(!y0&k0) |
   --------------------    --------------------    --------------------    --------------------
       |  ...   ____________________/          ...          \________   ...    ____/ 
       |       /                                                     \        /
      -------------------------------------------------------------------------
      |dddddddd|eeeeeeee|ffffffff|gggggggg|hhhhhhhh|iiiiiiii|jjjjjjjj|kkkkkkkk|
      |76543210|76543210|76543210|76543210|76543210|76543210|76543210|76543210|
      -------------------------------------------------------------------------

 equivalent C Code:
  {
   unsigned long long a;
   unsigned long long b;
   unsigned long lond d;

   d = ( d & (!b) ) | ( a & b ); /* BSEL a,b,d */
  }

 typical application cases:

  This instruction allows a bit-by-bit selection of data from two sources
  into the destination. Typically, this is applied in conjunction with a prior
  pcmp instruction. 
  
  Tasks like conditional replenishment and clipping easily come to mind.

