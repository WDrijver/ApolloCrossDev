## MOVE sr — Move from status register
### Operation
sr → d

### Syntax
`MOVE sr,<ea>`

### Description
Moves status register to the destination.

### Condition Codes
Not affected.

### Encoding
|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|0|1|0|0|0|0|0|0|1|1|Mode|Register|

Size is word.

### Note
Privileged instruction, except on the 68000 & 68080 where reading the status register may be done in user mode.

---
