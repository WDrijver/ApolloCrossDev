## LOAD
_Bron: AC68080PRM PDF, pagina 36_

LOAD                                                                 LOAD
Load source into register
Operation: <vea> → d
Syntax: LOAD <vea>,d
Condition Codes: not affected
Short: Load 64 bit into destination register.
Description: Load is the AMMX equivalent of move <ea>,dn
15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
1 1 1 1 1 1 1 A 0 D Mode Register
0 0 0 0 d 0 0 0 0 0 0 0 1

### Extra notes (AMMX reference)
Source: ammx.txt

#### LOAD

mnemonic: load  VEA,d

 short: load 64 bit into destination register 

 equivalent C Code:
  {
   unsigned long long src;
   unsigned long lond d;

   d = src;
  }

 typical application cases:

  Load is the AMMX equivalent to move ,dn. As most other AMMX
  instructions, it can load either from memory (any An,Bn addressing
  modes), from another register or constants. In case of memory sources,
  there are no restrictions towards alignment. You may load from any
  valid address in Chip- and FastRAM.

 examples:
  load (a0),E0
  load 1(a0),E1
  load #$c0ffee00feedface,E2
  load.w #$beef,E3 ; this one is with implicit splat
                   ; E3 = $beefbeefbeefbeef

---
