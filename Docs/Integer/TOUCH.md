## TOUCH — Touch data
### Operation
<ea> → void

### Syntax
`TOUCH <ea>`

### Short
Preload data cache.

### Description
Preload the data cache. Use it 12 to 15 cycles before needed, for occasional speedy access to data that is not detectable as sequential flow.

### Condition Codes
Not affected.

### Constraints
Only `<ea>` mode supported: **address index** (includes the full extension format).  
Mode = 6

### Encoding
|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|1|1|1|1|0|1|1|0|0|0|Mode|Register|

### Examples
```asm
touch 6000(A1,D1*4)           ; with index
touch (6000,A1,D1*4)          ; (same)
touch ([6000,A1],D1*4,7000)   ; post indexed
touch ([6000,A1,D1*4],7000)   ; pre indexed
```

### Note
TOUCH is seen as line-F coprocessor with id=3, like MOVE16.

---
