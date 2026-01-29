## C2P

Chunky to Planair
Operation: bit re-order source → d
Syntax: C2P  <vea>,d
Condition Codes: not affected
Short: Chunky to planar conversion, bit-wise transpose.
Description: Chunky-to-planar conversion and vice versa.
15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
1 1 1 1 1 1 1 A 0 D Mode Register
0 0 0 0 d 0 0 1 0 1 0 0 0
 
From a 8 byte source all bits from place n are put in destination byte n, in the 
order of source. So all msb are placed in the top byte of the destination and the lsb
are all placed in the lowest byte.

---
