## ADDQ — Add Quick (68080 B-address register variant)
### Operation
data + Bn → Bn

### Syntax
`ADDQ #<data>,Bn`

### Short
Add data to destination.

### Description
Adds an immediate value of one to eight to the destination. Destination is a B-address register. Size is long.

### Condition Codes
| X | N | Z | V | C |
|---|---|---|---|---|
| * | * | * | * | * |

### Encoding
|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|0|1|0|1|Data|0|0|0|0|0|1|Register|

### Note
The unused `addq.b #data,An` is used.



### Extra notes (Integer reference)
Source: integer.txt

Operation:
Immediate Data + Destination -->; Destination

Assembler Syntax:
ADDQ #<data>, An;

Attributes: Size = (Word, Long)

Description:Adds the immediate data 1 to 8 to the ADDRESS register
Always the entire address register is updated.

Condition Codes: Not affected

Example:

1)
A1= $00001234
ADDQ.L #8,A1
Result:
A1 = $0000123C

ADDQ is usefull to increment pointers


---
