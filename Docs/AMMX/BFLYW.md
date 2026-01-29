# BFLYW

## Butterfly

mnemonic: bflyw (VEA),b,d:d+1

 short: butterfly operation, single-cycle vector short addition _and_ subtraction

 graphic:
     inputs:
      -----------------------------------------
      |   a0w   |   a1w   |   a2w   |   a3w   | 
      -----------------------------------------
          |         |         |         |
      -----------------------------------------
      |   b0w   |   b1w   |   b2w   |   b3w   |
      -----------------------------------------
          | |       | |       | |       | |
          | |       | |       | |       | |
     outputs:       | |       | |       | |
      -----------------------------------------
      | b0w+a0w | b1w+a1w | b2w+a2w | b3w+a3w | 
      -----------------------------------------
          | |       | |       | |       | |
      -----------------------------------------
      | b0w-a0w | b1w-a1w | b2w-a2w | b3w-a3w |
      -----------------------------------------

 constraints:
  The destination register pair needs to be consecutive, starting with an even register
  index (e.g. bflyw (a0),E8,E0:E1 ). Immediate operands don't apply here.


 equivalent C Code:
   int i;
   short a[4];  /* */
   short b[4];  /* */
   short d[4];  /* */
   short e[4];  /* */

   for( i = 0 ; i<4 ; i++ )
   {
	d[i] = b[ i ] + a[ i ];
	f[i] = b[ i ] - a[ i ];
   }

 constraints:

  The destination can be either a register or memory location. Immediate
  operands don't apply here.


 typical application cases:

  Signal transforms like FFT, DCT, DWT etc. and lifting approaches in terms of
  filter banks in general.

