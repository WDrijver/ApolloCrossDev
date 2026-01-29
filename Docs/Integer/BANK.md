## BANK — Bank (Prefix for legacy instructions)
### Operation
Inform the next instruction Apollo-registers are used.

### Syntax
`- none -`

### Short
Prefix for legacy instructions.

### Description
BANK gives older instructions more bits to select more registers.  
- **AA** extends source from 8 to 32 possible sources.  
- **BB** extends destination from 8 to 32 possible sources.  
- **CCccc** is XORed to BBbbb to create a third operand.  
- **SS** is the length of the whole bundle = opcode length + bank length.

### Encoding (16-bit)
|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|0|1|1|1|c|c|c|1|S|S|C|C|A|A|B|B|

### Notes
- If `CCccc <> 0` then `BBbbb xor CCccc → DDddd` (third operand selection).
- Data and Float registers share the same extra registers when banked.

---
