## PMULA

> Context (from PMULA page)
>
> _Bron: AC68080PRM PDF, pagina 54_
>
> PMULA                                                           PMULA
> Vector multiply
> Operation: alfa<100%  ?  a  +  alpha x b   →  d 
> alfa=100%  ?  b  →  d 
> Syntax: PMULA <vea>,b,d
> Condition Codes: not affected
> Short: 32bit color vector multiply and add.
> Description: Fade b-colors by alfa then add a-colors to it. Resulting colors are 
> unsigned saturated bytes.
> 0%=<alfa<100% ( alfa x b ) +  a → d
> alfa=100%=255 100% b → d
> 1 1 1 1 1 1 1 A B D Mode Register
> b d 0 0 0 1 1 0 0 1
> Long: 8bit alpha = 0..100%,  8bit red 0..255,  8bit green 0..255,  8bit blue 0..255
> a
> Alfa 8bit Red 8bit Green 8bit Blue 8 bit Src a
> b
> - Red 8bit Green 8bit Blue 8 bit Src b
> d
> 0 Red 8bit Green 8bit Blue 8 bit Dest d
> Note:
> When alfa is 100% (255) there is no addition done.
>
> ### Extra notes (AMMX reference)
> Source: ammx.txt

### PMULA
mnemonic: pmula  (VEA),b,d

 short: alpha multiply and add unsigned bytes with saturate.

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
     output d                                       
      -----------------------------------------
      | d0 | d1 | d2 | d3 | d4 | d5 | d6 | d7 |
      -----------------------------------------
        |    |                             |
        |    |                             |
        |    |                             \ 
        |    \_____________       ...       \ 
         \                 \                 \ 
          |                 \                 \ 
    ----------------- ----------------- ----------------- 
    |((B0*A0)>>8)+A0| |((B1*A0)>>8)+A1| |((B7*A4)>>8)+A7| 
    ----------------- ----------------- ----------------- 
          |    _____________/   ...          _/
          |   /                             /
      -----------------------------------------
      | d0 | d1 | d2 | d3 | d4 | d5 | d6 | d7 |
      -----------------------------------------


 equivalent C Code:
   int i,j;
   unsigned char a[8]; 
   unsigned char b[8];
   unsigned char d[8];
   d[0] = 0;
   d[1] = a[0] == 255 ? b[0] : min(a[0] * b[1] + a[1], 255);
   d[2] = a[0] == 255 ? b[0] : min(a[0] * b[2] + a[2], 255);
   d[3] = a[0] == 255 ? b[0] : min(a[0] * b[3] + a[3], 255);
   d[4] = 0;
   d[5] = a[4] == 255 ? b[4] : min(a[4] * b[5] + a[5], 255);
   d[6] = a[4] == 255 ? b[4] : min(a[4] * b[6] + a[6], 255);
   d[7] = a[4] == 255 ? b[4] : min(a[4] * b[7] + a[7], 255);


 typical application cases:

  Alpha Blending is the most prominent use for this instruction. 
  
  In case of premultiplied Alpha, a single PMULA instruction is sufficient for
  the task. Just load (a) with the premultiplied RGB/[255-alpha in A], and RGB in (b).
  Beware that the case of alpha==255, this output is (B).
  Beware that the case of alpha==0, this output is (A).

---
