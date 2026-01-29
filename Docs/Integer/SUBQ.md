## SUBQ — Sub Quick (68080 B-address register variant)
### Operation
Bn – data → Bn

### Syntax
`SUBQ #<data>,Bn`

### Short
Subtracts data from destination.

### Description
Subtracts an immediate value of one to eight from the destination. Destination is a B-address register. Size is long.

### Condition Codes
| X | N | Z | V | C |
|---|---|---|---|---|
| * | * | * | * | * |

### Encoding
|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|0|1|0|1|Data|1|0|0|0|0|1|Register|

### Note
The unused `subq.b #data,An` is used.



### Extra notes (Integer reference)
Source: integer.txt

Operation:
Immediate Data + Destination -->; Destination

Assembler Syntax:
SUBQ #<data>, An;

Attributes: Size = (Word, Long)

Description:Subtracts the immediate data 1 to 8 from the ADDRESS register
Always the entire address register is updated.

Condition Codes: Not affected
Example:

1) A1= $00001004
SUBQ.L #8,A1
Result:
A1 = $00000FFC

SUBQ is usefull to decrement pointers


---
