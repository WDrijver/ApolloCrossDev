## PAND

> Context (from PAND page)
>
> _Bron: AC68080PRM PDF, pagina 43_
>
> PAND                                                                  PAND
> Vector and
> Operation: 64x a & b → d
> Syntax: PAND <vea>,b,d
> Condition Codes: not affected
> Short: Bitwise logical operantion.
> 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
> 1 1 1 1 1 1 1 A B D Mode Register
> b d 0 0 0 0 1 0 0 0
>
> ### Extra notes (AMMX reference)
> Source: ammx.txt

### PAND
mnemonic: pand  (VEA),b,d
          por   (VEA),b,d
          peor  (VEA),b,d
          pandn (VEA),b,d

 short: 64 Bit logic operations

 graphic:
     input1:  (bytes l-s with their bits 7...0)
      -------------------------------------------------------------------------
      |llllllll|mmmmmmmm|nnnnnnnn|oooooooo|pppppppp|qqqqqqqq|rrrrrrrr|ssssssss|
      |76543210|76543210|76543210|76543210|76543210|76543210|76543210|76543210|
      -------------------------------------------------------------------------
       |      |                                                       |      |
       | ...  |                   ...                                 | ...  |
       |      |                                                       |      |
       |      |                                                       |      |
     input2: (contents of register b with bytes b,c,t-y)              |      |
      -------------------------------------------------------------------------
      |bbbbbbbb|cccccccc|tttttttt|uuuuuuuu|vvvvvvvv|wwwwwwww|xxxxxxxx|yyyyyyyy|
      |76543210|76543210|76543210|76543210|76543210|76543210|76543210|76543210|
      -------------------------------------------------------------------------
       |       \                                                      |      |
       | ...    \____    ...                               ...       /       |
       |             \                                         _____/        |
       |              \                                       /              |
      ---------    ---------                             ---------    ---------
      | b7&l7 | .. | b0&l0 | (partial PAND ILLUSTRATION) | s7&y7 | .. | s0&y0 |
      ---------    ---------                             ---------    ---------
       |  ...   ______/          ...                         \_______     \_
       |       /                                                     \      \
      -------------------------------------------------------------------------
      |dddddddd|eeeeeeee|ffffffff|gggggggg|hhhhhhhh|iiiiiiii|jjjjjjjj|kkkkkkkk|
      |76543210|76543210|76543210|76543210|76543210|76543210|76543210|76543210|
      -------------------------------------------------------------------------

 equivalent C Code:
  {
   unsigned long long a;
   unsigned long long b;
   unsigned long lond d;

   d =  a & b; /* PAND  */
   d =  a | b; /* POR   */
   d =  a ^ b; /* PEOR  */
   d = !a & b; /* PANDN */
  }

 typical application cases:

  EOR,AND,OR AND I think NOT that I have to write more. :-)
