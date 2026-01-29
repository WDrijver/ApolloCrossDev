## FMOVEM

Move muliple float registers from/to memory
### Operation
list → <ea>

### Syntax
FMOVE <list>,<ea>

FMOVE <ea>,<list>
### Description
Fmovem on the 68080 is the same as previous generations, execept

for the fact that the format in memory is a tiny bit different. For calculated results
it makes no differens when a push and later a pull is done.
To get all the bits of the float you need a fmove.d fpn,<ea>
Programs that use the expected extended layout in memory may be affected.
### Condition Codes

N Z I nan
– – – –
Motorola eXtende format:
31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
S E E E E E E E E E E E E E E E
M 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63
Apollo eXtende format:
31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
S E E E E E E E E E E E E E E E 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47
M 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
Double format:
31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
S E E E E E E E E E E E 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20
21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52


### Extra notes (FPU reference)
Source: fpu.txt

Move Multiple Floating-Point Data Registers

Register List => Destination 
Source =>  Register List 
Syntax: FMOVEM.X #list,(ea)
FMOVEM.X (ea),#list


Description: Moves one or more extended-precision numbers to or from a list of floating- point data registers. No conversion or rounding is performed during this operation, and the floating-point status register is not affected by the instruction. For the MC68881, this instruction does not cause pending exceptions (other than protocol violations) to be reported. Furthermore, a write to the floating- point control register exception enable byte or the floating-point status register exception status byte connot generate a new exception, despite the value written. Any combination of the eight floating-point data registers can be transferred, with the selected registers specified by a user- supplied mask. This mask is an 8-bit number, where each bit corresponds to one register; if a bit is set in the mask, that register is moved. The register select mask may be specified as a static value contained in the instruction or a dynamic value in the least significant eight bits of an integer data reg- ister (the remaining bits of the register are ignored). FMOVEM allows three types of addressing modes: the control modes, the predecre- ment mode, or the postincrement mode. If the effective address is one of the control addressing modes, the registers are transferred between the processor and memory starting at the specified address and up through higher addresses. The order of the transfer is from FP0 â€“ FP7.
