## MOVEA — Move Address (<ea> → Bn)
### Operation
<ea> → Bn

### Syntax
`MOVEA <ea>,Bn`

### Description
Move the source into a B-address register.

### Condition Codes
Not affected.

### Constraints
No `LEA (Bn),Bm` or `MOVEA.L Bn,Bm`.

### Encoding
|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|0|0|0|1|Bn|0|0|1|Mode|Register|

Size is long.



### Extra notes (Integer reference)
Source: integer.txt

Operation: Source -->; Destination

Assembler Syntax: MOVEA <ea>, AN

Attributes: Size = (Word, Long)

Description: Moves the data at the source to the destination
location, and sets the condition codes according to the data.
The size of the operation may be specified as byte,
word, or long.

Condition Codes:
Not affected.

Example:

1)
A0= $00008000
A1= $00044444

MOVE.L A0,A1
Result:
A1 = $00008000



2)
A0= $00008000
A1= $00044444

MOVE.W A0,A1 ; Mind .W gets sign extended to 32bit
Result:
A1 = $FFFF8000


---
