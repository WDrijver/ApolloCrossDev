/*
** cpu.h SWEET16 cpu-description header-file
** (c) in 2026 by Frank Wille
*/

#define BIGENDIAN 0
#define LITTLEENDIAN 1
#define BITSPERBYTE 8
#define VASM_CPU_SWEET16 1

/* maximum number of operands for one mnemonic */
#define MAX_OPERANDS 2

/* maximum number of mnemonic-qualifiers per mnemonic */
#define MAX_QUALIFIERS 0

/* data type to represent a target-address */
typedef int16_t taddr;
typedef uint16_t utaddr;

/* minimum instruction alignment */
#define INST_ALIGN 1

/* default alignment for n-bit data */
#define DATA_ALIGN(n) 1

/* operand class for n-bit data definitions */
#define DATA_OPERAND(n) MDAT

/* returns true when instruction is valid for selected cpu */
#define MNEMONIC_VALID(i) 1

/* type to store each operand */
typedef struct {
  int mode;
  expr *val;
} operand;

/* addressing modes */
enum {
  MNONE=0,      /* no operand */
  MDAT,         /* absolute value */
  MREL,         /* relative branch destination */
  MREG,         /* register: n */
  MRIN,         /* register indirect: @n */
  MCOUNT        /* number of addressing modes */
};

/* additional mnemonic data */
typedef struct {
  uint8_t opcode;
} mnemonic_extension;
