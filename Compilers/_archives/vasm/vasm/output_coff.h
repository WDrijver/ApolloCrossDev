/* coff.h header file for COFF objects */


/* COFF header - 32-bit COFF - 20 bytes length */
struct coff_hdr {
    uint16_t f_magic;   /* Number identifying type of target machine */
    uint16_t f_nscns;   /* Number of sections; indicates size of the Section Table, which immediately follows the headers */
    uint32_t f_timdat;  /* Time and date the file was created */
    uint32_t f_symptr;  /* File offset of the symbol table or 0 if none is present */
    uint32_t f_nsyms;   /* Number of entries in the symbol table. This data can be used in locating the string table, which immediately follows the symbol table */
    uint16_t f_opthdr;  /* sizeof(Optional Header), an object file should have a value of 0 here */
    uint16_t f_flags;   /* Flags indicating attributes of the file */
};

/* Bits for f_flags */
#define  COFF_F_RELFLG      0x0000001       /* F_RELFLG, relocation info stripped from file */
#define  COFF_F_EXEC        0x0000002       /* F_EXEC, file is executable  (i.e. no unresolved external references) */
#define  COFF_F_LNNO        0x0000004       /* F_LNNO, line numbers stripped from file */
#define  COFF_F_LSYMS       0x0000010       /* F_LSYMS, local symbols stripped from file */
#define  COFF_F_MINMAL      0x0000020       /* F_MINMAL, this is a minimal object file (".m") output of fextract */
#define  COFF_F_UPDATE      0x0000040       /* F_UPDATE, this is a fully bound update file, output of ogen */
#define  COFF_F_SWABD       0x0000100       /* F_SWABD, this file has had its bytes swabbed (in names) */
#define  COFF_F_AR16WR      0x0000200       /* F_AR16WR, this file has the byte ordering of an AR16WR (e.g. 11/70) machine */
#define  COFF_F_AR32WR      0x0000400       /* F_AR32WR, this file has the byte ordering of an AR32WR machine (e.g. vax and iNTEL 386) */
#define  COFF_F_AR32W       0x0001000       /* F_AR32W, this file has the byte ordering of an AR32W machine (e.g. 3b,maxi) */
#define  COFF_F_PATCH       0x0002000       /* F_PATCH, file contains "patch" list in optional header */
#define  COFF_F_NODF        0x0002000       /* F_NODF, (minimal file only) no decision functions for replaced functions */


/* COFF section header - 32-bit COFF - 40 bytes legnth */
struct coff_section_header {
    char s_name[8];     /* Section Name */
    uint32_t s_paddr;   /* Physical Address */
    uint32_t s_vaddr;   /* Virtual Address */
    uint32_t s_size;    /* Section Size in Bytes */
    uint32_t s_scnptr;  /* File offset to the Section data */
    uint32_t s_relptr;  /* File offset to the Relocation table */
    uint32_t s_lnnoptr; /* File offset to the Line Number table */
    uint16_t s_nreloc;  /* Number of Relocation table entries */
    uint16_t s_nlnno;   /* Number of Line Number table entries */
    uint32_t s_flags;   /* Flags for this section */
};

/* Bits for s_flags */
#define COFF_STYP_TEXT  0x0020      /* STYP_TEXT, The section contains executable code */
#define COFF_STYP_DATA  0x0040      /* STYP_DATA, The section contains initialised data */
#define COFF_STYP_BSS   0x0080      /* STYP_BSS, The COFF file contains no data for this section, but needs space to be allocated for it */


/* COFF symbol structure - 18 bytes legnth */
struct coff_symbol {
    union {
        char n_name[8];             /* Symbol name (if <= 8 chars) */
        struct {
            uint8_t n_zeroes[4];    /* 0 indicates name in string table */
            uint8_t n_offset[4];    /* Offset into string table */
        } n_n;
    } n;
    uint8_t n_value[4];             /* Symbol value */
    uint8_t n_scnum[2];             /* Section number */
    uint8_t n_type[2];              /* Type */
    uint8_t n_sclass;               /* Storage class */
    uint8_t n_numaux;               /* Number of auxiliary entries */
};

/* Value for specific section number */
#define COFF_N_UNDEF    0       /* N_UNDEF, An undefined (extern) symbol */
#define COFF_N_ABS      -1      /* N_ABS, An absolute symbol (e_value is a constant, not an address) */
#define COFF_N_DEBUG    -2      /* N_DEBUG, A debugging symbol */

/* Value for type
* Note that all DT_* must be shifted by N_BTSHIFT to get actual values
*/
#define COFF_T_NULL     0b0000                          /* T_NULL, No symbol */
#define COFF_T_VOID     0b0001                          /* T_VOID, void function argument (not used) */
#define COFF_T_CHAR     0b0010                          /* T_CHAR, character */
#define COFF_T_SHORT    0b0011                          /* T_SHORT, short */
#define COFF_T_INT      0b0100                          /* T_INT, integer */
#define COFF_T_LONG     0b0101                          /* T_LONG, long */
#define COFF_T_FLOAT    0b0110                          /* T_FLOAT, floating point */
#define COFF_T_DOUBLE   0b0111                          /* T_DOUBLE, double precision floating point */
#define COFF_T_STRUCT   0b1000                          /* T_STRUCT, structure */
#define COFF_T_UNION    0b1001                          /* T_UNION, union */
#define COFF_T_ENUM     0b1010                          /* T_ENUM, enumeration */
#define COFF_T_MOE      0b1011                          /* T_MOE, member of enumeration */
#define COFF_T_UCHAR    0b1100                          /* T_UCHAR, unsigned character */
#define COFF_T_USHORT   0b1101                          /* T_USHORT, unsigned short */
#define COFF_T_UINT     0b1110                          /* T_UINT, unsigned integer */
#define COFF_T_ULONG    0b1111      /* ---- 1111 */     /* T_ULONG, unsigned long */
#define COFF_T_LNGDBL   0b10000     /* ---1 0000 */     /* T_LNGDBL, long double (special case bit pattern) */
/* e_type = base + derived << N_BTSHIFT */
#define COFF_DT_NON     0b00        /* --00 ---- */     /* DT_NON, No derived type */
#define COFF_DT_PTR     0b01        /* --01 ---- */     /* DT_PTR, pointer to T */
#define COFF_DT_FCN     0b10        /* --10 ---- */     /* DT_FCN, function returning T */
#define COFF_DT_ARY     0b11        /* --11 ---- */     /* DT_ARY, array of T */

/* Value for class */
#define COFF_C_NULL     0       /* C_NULL, No entry */
#define COFF_C_AUTO     1       /* C_AUTO, Automatic variable */
#define COFF_C_EXT      2       /* C_EXT, External (public) symbol - this covers globals and externs */
#define COFF_C_STAT     3       /* C_STAT, static (private) symbol */
#define COFF_C_REG      4       /* C_REG, register variable */
#define COFF_C_EXTDEF   5       /* C_EXTDEF, External definition */
#define COFF_C_LABEL    6       /* C_LABEL, label */
#define COFF_C_ULABEL   7       /* C_ULABEL, undefined label */
#define COFF_C_MOS      8       /* C_MOS, member of structure */
#define COFF_C_ARG      9       /* C_ARG, function argument */
#define COFF_C_STRTAG   10      /* C_STRTAG, structure tag */
#define COFF_C_MOU      11      /* C_MOU, member of union */
#define COFF_C_UNTAG    12      /* C_UNTAG, union tag */
#define COFF_C_TPDEF    13      /* C_TPDEF, type definition */
#define COFF_C_USTATIC  14      /* C_USTATIC, undefined static */
#define COFF_C_ENTAG    15      /* C_ENTAG, enumaration tag */
#define COFF_C_MOE      16      /* C_MOE, member of enumeration */
#define COFF_C_REGPARM  17      /* C_REGPARM, register parameter */
#define COFF_C_FIELD    18      /* C_FIELD, bit field */
#define COFF_C_AUTOARG  19      /* C_AUTOARG, auto argument */
#define COFF_C_LASTENT  20      /* C_LASTENT, dummy entry (end of block) */
#define COFF_C_BLOCK    100     /* C_BLOCK, ".bb" or ".eb" - beginning or end of block */
#define COFF_C_FCN      101     /* C_FCN, ".bf" or ".ef" - beginning or end of function */
#define COFF_C_EOS      102     /* C_EOS, end of structure */
#define COFF_C_FILE     103     /* C_FILE, file name */
#define COFF_C_LINE     104     /* C_LINE, line number, reformatted as symbol */
#define COFF_C_ALIAS    105     /* C_ALIAS, duplicate tag */
#define COFF_C_HIDDEN   106     /* C_HIDDEN, ext symbol in dmert public library */
#define COFF_C_EFCN     255     /* C_EFCN, physical end of function */

/* COFF machine types - these are target architecture specific */
#define COFF_I386_MAGIC         0x014c      /* Default for x86 / Intel 386 */
#define COFF_M68K_MAGIC         0x0150      /* Standard for M68k, this value is the most recognized one for the coff reader */
#define COFF_PDP11_MAGIC        0x0180      /* PDP-11 Historical value */
#define COFF_ARMV4_MAGIC        0x01c0      /* ARM v4 */
#define COFF_ARMV4T_MAGIC       0x01c2      /* ARM v4t */
#define COFF_PPC32_MAGIC        0x01f0      /* PowerPC 32-bit */
#define COFF_PPC32FPU_MAGIC     0x01f1      /* PowerPC 32-bit with FPU */
#define COFF_PPC64_MAGIC        0x01f2      /* PowerPC 64-bit */
/* #define COFF_JAGRISC_MAGIC               Vasm value specific to the Atari Jaguar risc (TBD) */


/* COFF relocation structure  - 32-bit COFF - 10 bytes length */
struct coff_reloc {
    uint8_t r_vaddr[4];     /* Virtual address of reference */
    uint8_t r_symndx[4];    /* Index into symbol table */
    uint8_t r_type[2];      /* Relocation type */
};

/* COFF relocation types */
#define R_ABS       0       /* Reference is absolute; no relocation is necessary: The entry will be ignored */
#define R_RELBYTE   017     /* Direct 8 - bit reference to the symbol's virtual address */
#define R_RELWORD   020     /* Direct 16 - bit reference to the symbol's virtual address */
#define R_RELLONG   021     /* Direct 32 - bit reference to the symbol's virtual address */
#define R_PCRBYTE   022     /* "PC-relative" 8 - bit reference to the symbol's virtual address */
#define R_PCRWORD   023     /* "PC-relative" 16 - bit reference to the symbol's virtual address */
#define R_PCRLONG   024     /* "PC-relative" 32-bit reference to the symbol's virtual address */
