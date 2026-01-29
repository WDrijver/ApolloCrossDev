## MOVE2 — Move two
### Operation
Source pair → destination pair

### Syntax
```asm
MOVE2 <ea>,b:b+1
MOVE2 b:b+1,<ea>
```

### Short
Move a pair.

### Description
Moves a source pair to destination pair. A destination register is extended unsigned to a long.
Conditions are taken from the first. Size can be byte, word or long.

### Condition Codes
Taken from first, not the second.
| X | N | Z | V | C |
|---|---|---|---|---|
| – | * | * | 0 | 0 |

### Encoding
|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|0|0|0|0|1|1|1|0|Size|Mode|Register|
|b|d|0|0|0|0|0|0|1|0|0|0|1|

- Size: `0=byte, 1=word, 2=long`
- `b`: `0..7` data register, `8..15` address register
- direction `d=0`: `<ea>,b` else `b,<ea>`

### Notes
- MOVE2 extends unsigned; MOVEM extends signed.
- MOVEX shares the same bitformat, but A & B do not function here → Bn cannot be used.

---
