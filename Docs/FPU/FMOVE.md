## FMOVE

Floating point Move
### Operation
FPn → Dn.D

### Syntax
FMOVE.D Dn,FPn

FMOVE.D FPn,Dn
### Description
Move in Double format from/to data-register.

Move in Single format was always possible, now double too.
### Condition Codes

N Z I nan
– – – –
15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
1 1 1 1 0 0 1 0 0 0 Mode Data Register
0 1 D Src fpn Opmode
Mode=000=data register
Source Specifier=101=double
Direction d=0: <ea>,fpn else fpn,<ea>


### Extra notes (FPU reference)
Source: fpu.txt

Move Floating-Point Data Register
Operation: Source => FPn
Operation: FPn => Destination

Assembler Syntax:
FMOVE.X (ea),Fpn
FMOVE.X Fpn,(ea)

Attributes: Size = (Byte, Word, Long, Single, Double, Extended)

Description: Moves the contents of the source operand to the destination operand. Although the primary function of this instruction is data movement, it is also considered an arithmetic instruction since conversions from the source operand format to the destination operand format are performed implicitly during the move operation. Also, the source operand is rounded according to the selected rounding precision and mode. Unlike the MOVE instruction, the FMOVE instruction does not support a memory-to- memory format. For such transfers, it is much faster to utilize the MOVE instruction to transfer the floating- point data than to use the FMOVE instruction. The FMOVE instruction only supports memory-to-register, register-to- register, and register-to- memory operations (in this context, memory may refer to an integer data register if the data format is byte, word, long, or single). The memory-to-register and register- to-reg- ister operation uses a command word encoding distinctly different from that used by the register-to-memory operation; these two operation classes are described separately.

Memory-to-Register and Register-to-Register Operation: Converts the source operand to an extended-precision floating-point number (if necessary) and stores it in the destination floating-point data register. MOVE will round the result to the precision selected in the floating-point control register. FSMOVE and FDMOVE will round the result to single or double precision, respectively, regardless of the rounding precision selected in the floating-point control register. Depending on the source data format and the rounding precision, some operations may produce an inexact result. In the following table, combinations that can produce an inexact result are marked with a dot ( â‹… ), but all other combinations produce an exact result.
