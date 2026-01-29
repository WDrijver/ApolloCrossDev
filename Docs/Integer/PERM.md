## PERM — Permute
### Operation
Pick bytes from a → d

### Syntax
`PERM #n,Ra,Rb` where `#n` contains the picking order from a

### Short
Change order and place in destination.

### Condition Codes
Not affected.

### Encoding
|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|0|1|0|0|1|1|0|0|1|1|0|0|a|
|b|pos0|pos1|pos2|pos3|

- `a` & `b` = 0..7 data register, 8..15 address register

### Example
```asm
perm #@3320,d0,d1
; src d0: 0 0 1 1 2 2 3 3
; dst d1: 3 3 3 3 2 2 0 0
```

---
