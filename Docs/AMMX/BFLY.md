# BFLY
## Butterfly

Operation: b + a  → d  ,  b – a  → d2

Syntax: BFLYB <vea>,b,d:d2 , BFLYW <vea>,b,d:d2

Condition Codes: not affected

Short: Butterfly operation, vector short addition and subtraction.

Description: Bflyb is 8byte vector, It calculates 8additions and 8 substractions. 
Bflyw is 4 word vector. This is for 4additions and 4substractions.

Constraints: The destination register pair needs to be consecutive, starting with
an even register (e.g. bflyw (a0),E8,E0:E1 ).

[test](LOAD.md)


|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00|
| 1| 1| 1| 1| 1| 1| 1| A| B| D|  Mode  |Register|
|     b     |    d     0| 0| 0| 0| 1| 1| 1| 0| S|

S=Size 0=byte else word.

Note: There is no saturation.(limiting)

Example: bflyb (a0),e1,e6:e7

vea(a0) memory content where a0 points to
0 4 0 4 0 4 0 3 1 4 0 4 0 5 8 8

b e1
0 0 F F 7 F 3 3 7 4 5 5 6 6 7 7

d e6 & e7
0 4 0 3 3 8 3 6 8 8 5 9 6 B F F
F C F B 7 B 3 0 6 0 5 1 6 1 E F


