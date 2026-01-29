## MOVE16 — Move 16-byte block
### Operation
memory: source → destination

### Syntax
```asm
MOVE16 (Ax)+,(Ay)+
MOVE16 (An)+,abs.l
MOVE16 (An),abs.l
MOVE16 abs.l,(An)+
MOVE16 abs.l,(An)
```

### Description
Moves 16 bytes memory to the destination. The absolute is always a long extension word.

### Condition Codes
Not affected.

### Encoding
**First form:**
|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|1|1|1|1|0|1|1|0|0|0|1|0|0|Register|Ax|
|1|Register|Ay|0|0|0|0|0|0|0|0|0|0|0|0|0|

**Other forms:**
|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|1|1|1|1|0|1|1|0|0|0|0|m|d|Register|
- `m=0`: (An)+ else (An)  
- `d=0`: register → absolute else absolute → register

### Notes
- MOVE16 is seen as line-F coprocessor with id=3, like TOUCH.
- On the 68080, MOVE16 does **not** have to be aligned (unlike 68040).

---
