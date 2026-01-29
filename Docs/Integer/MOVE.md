## MOVE — Move (B-address register → <ea>)
### Operation
Bn → <ea>

### Syntax
`MOVE Bn,<ea>`

### Description
Move B-address register into destination.

### Condition Codes
| X | N | Z | V | C |
|---|---|---|---|---|
| – | * | * | 0 | 0 |

### Encoding
|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|0|0|0|1|Register|Mode|0|0|1|Bn|

### Notes
- Size is long.
- Unused `move.b an,<ea>` is used.

---
