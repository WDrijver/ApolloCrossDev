/* aof.h header file for Acorn/ARM Object Format */
/* (c) in 2025 by Frank Wille */

#define CHUNK_FILE_ID 0xc3cbc6c5
#define CHUNKS_RESERVED 7       /* max chunks in file - we need 5 max */
#define AOF_RELOC_OBJ 0xc5e2d080
#define AOF_VERSION 310         /* according to the latest documentation I got */

#define AOFSTRHTABSIZE 0x1000

struct aof_chunk {
  struct aof_chunk *next;
  uint8_t id[8];
  uint32_t offs;
  uint32_t size;
};

struct aof_strtab {
  struct aof_strtab *next;
  const char *str;
  uint32_t offs;
};

struct aof_reloc {
  struct aof_reloc *next;
  uint32_t offset;
  uint8_t type,a,ft,ii;
  union {
    symbol *sym;
    section *sec;
  } ref;
};


struct area_hdr {
  struct area_hdr *next;
  section *sec;
  uint32_t name_offs;
  uint32_t attr_align;
  uint32_t aligned_size;
  uint32_t num_relocs;
  uint32_t base_addr;
  struct aof_reloc *relocs;
};

/* Area attributes (attr_align word) */
#define AA_ABSOLUTE     (1<<8)
#define AA_CODE         (1<<9)
#define AA_COMMDEF      (1<<10)
#define AA_COMMREF      (1<<11)
#define AA_NOINIT       (1<<12)
#define AA_READONLY     (1<<13)
#define AA_PIC          (1<<14)
#define AA_DEBUG        (1<<15)
#define AA_APCS32       (1<<16)
#define AA_REENTRANT    (1<<17)
#define AA_FPU          (1<<18)
#define AA_NOSWSTKCHK   (1<<19)
#define AA_THUMBRELOC   (1<<20)
#define AA_HALFRELOC    (1<<21)
#define AA_ARMTHUMB     (1<<22)


struct aof_symbol {
  struct aof_symbol *next;
  uint32_t symname;
  uint32_t attr;
  uint32_t value;
  uint32_t areaname;
};

/* symbol attr */
#define AOFSYM_LOCAL    1
#define AOFSYM_EXTERN   2
#define AOFSYM_GLOBAL   3
#define AOFSYM_ABS      (1<<2)
#define AOFSYM_NOCASE   (1<<3)
#define AOFSYM_WEAK     (1<<4)
#define AOFSYM_STRONG   (1<<5)
#define AOFSYM_COMMON   (1<<6)
