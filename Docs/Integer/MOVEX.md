## MOVEX — Move convert source to destination
### Operation
Re-ordered source → destination

### Syntax
```asm
MOVEX <ea+>,b
MOVEX b,<ea+>
```

### Short
Endian convert and move

### Description
Changes byte order from the source and places it in the destination.

### Condition Codes
| X | N | Z | V | C |
|---|---|---|---|---|
| – | * | * | 0 | 0 |

### Encoding
**Word 0 (16-bit):**
|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|0|0|0|0|1|1|1|0|Size|Mode|Register|

**Word 1 (16-bit):**
- `Size`: 1=word, 2=long  
- direction `d=0`: `<ea+>,b` else `b,<ea+>`  
- `<ea+>` = `<ea>` where An is replaced for Bn if A=1  

`Bbbbb:`
- `00nnn` D0..7 data register  
- `01nnn` A0..7 address register  
- `11nnn` B0..7 address register  

---
