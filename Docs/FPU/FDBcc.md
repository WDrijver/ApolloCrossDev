## FDBcc

Floating-point Test Decrement & Branch Conditional
### Operation
If not cc then ( PC – 1 → Dn ; if Dn <> – 1 then PC + dn → PC )

### Syntax
FDBcc.L Dn,<label>

### Short
Decrement Dn & conditiononal jump to label.

### Description
Controls a loop of instructions.If condition is true the loop ends and

the program continues with the next instruction.
Else count register Dn is decremented by 1.
If Dn = – 1 the loop also ends and the program continues with the next instruction.
If Dn <> – 1 the loop continues and the program execution continues at location
(PC) + displacement.
The displacement is always even. When it appears as odd, then the counter is a
long, not a word.
### Condition Codes

N Z I nan
– – – –
15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
1 1 1 1 0 0 1 0 0 1 0 0 1 Count Register
0 0 0 0 0 0 0 0 0 0 Conditional predicate
16 bit Displacement 1
### Note

Variant on fdbcc. Here Dn is a long counter, not a word counter.


### Extra notes (FPU reference)
Source: fpu.txt

Floating-Point Test Condition, Decrement, and Branch
Operation:

If Condition True Then
  No Operation 
Else Dn -1 => Dn 
  If Dn != -1 Then
    PC + dalte => PC
  Else
    Execute Next Instruction 
Assembler Syntax:
FDBcc Dn,label

Attributes: unsized

Description: This instruction is a looping primitive of three parameters: a floating-point condition, a counter (data register), and a 16-bit displacement. The instruction first tests the condition to determine if the termination condition for the loop has been met, and if so, execution continues with the next instruction in the instruction stream. If the termination condition is not true, the low-order 16 bits of the counter register are decremented by one. If the result is -1, the count is exhausted, and execution continues with the next instruction. If the result is not equal to -1, execution continues at the location specified by the current value of the program counter plus the sign- extended 16-bit displacement. The value of the program counter used in the branch address calculation is the address of the displacement word.
