## ADDIW — Add Immediate Word extended to Long
### Operation
data + <ea> → <ea>

### Syntax
`ADDIW.L #<data>,<ea>`

### Short
Add data to destination.

### Description
Sign-extend immediate data (16-bit) to long and add to the destination. Size is long.

### Condition Codes
| X | N | Z | V | C |
|---|---|---|---|---|
| * | * | * | * | * |

### Encoding
**Word 0 (16-bit):**
|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|0|0|0|0|0|1|1|0|1|1|Mode|Register|

**Word 1 (16-bit):** immediate data

### Note
Replaces CALLM.

---
