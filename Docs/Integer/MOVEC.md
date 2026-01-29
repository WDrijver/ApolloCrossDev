## MOVEC — Move Control register
### Operation
Control → d

### Syntax
```asm
MOVEC Rc,Rn
MOVEC Rn,Rc   ; Super mode only
```

### Short
Control register request/set

### Description
Some useful event counters to look at.

### Condition Codes
Not affected.

### Encoding
|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|0|1|0|0|1|1|1|0|0|1|1|1|1|0|1|d|

- `a=1`: address register else data register  
- direction `d=0`: Rc to Rn else Rn to Rc

### Event counters
`$808 PCR` Processor Control Register  
`$809 CCC` CPU Clock Counter  
`$80A IEP1` Instructions Executed Pipe 1  
`$80B IEP2` Instructions Executed Pipe 2  
`$80C BPC` Branches Predicted Correct  
`$80D BPW` Branches Predicted Wrong  
`$80E DCH` Data Cache Hits  
`$80F DCM` Data Cache Miss  
`$00A STR` STalls Register  
`$00B STC` STalls Cache  
`$00C STH` STalls Hazard  
`$00D STB` STalls Buffer  
`$00E MWR` Memory Writes  

### Note
Privileged instruction. On 68080 reading a control register may be done in user mode.



### Extra notes (Integer reference)
Source: integer.txt

Move to/from Control Register
Operation: Source -->; Destination

Assembler Syntax: MOVEC CR, DN

Attributes: Size = (LONG)

Description: Move General Purpose Register to Control Register. Or move control register to general purpose register.

Condition Codes:
Not affected.


---
