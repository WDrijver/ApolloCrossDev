cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})

${RELOCATING+${LIB_SEARCH_DIRS}}
${STACKZERO+${RELOCATING+${STACKZERO}}}
${SHLIB_PATH+${RELOCATING+${SHLIB_PATH}}}

SECTIONS
{
  ${RELOCATING+PROVIDE(___machtype = 0x0);}
  ${RELOCATING+. = ${TEXT_START_ADDR};}
  .text :
  {
    ${RELOCATING+__stext = .;}
    *(.text)
    *(.text.main)
    *(.text*)
    *(_*)
    *(.rodata*)
    *(.data.rel.ro*)
    *(.gnu.linkonce.t.*)
    *(.gnu.linkonce.r.*)
    *(.gcc_except_table*)
    ${CONSTRUCTING+CONSTRUCTORS}    
    *(SORT_BY_NAME(.list___EH_FRAME*))
    *(SORT_BY_NAME(.list_*))
    *(.end_of_lists)
    ${RELOCATING+___datadata_relocs = .;}
    ${RELOCATING+__etext = .;}
    ${PAD_TEXT+${RELOCATING+. = ${DATA_ALIGNMENT};}}
  }
  ${RELOCATING+___text_size = SIZEOF(.text);}
  ${RELOCATING+. = ${DATA_ALIGNMENT};}
  .data :
  {
    ${RELOCATING+__sdata = .;}
    *(SORT_BY_NAME(.dlist___EH_FRAME_OBJECT*))
    *(SORT_BY_NAME(.dlist_*))
    *(.end_of_dlists)
    *(.data.__EH_FRAME_OBJECT__*)    
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    ${RELOCATING+___a4_init = 0x7ffe;}
    ${RELOCATING+__edata = .;}
  }
  .bss :
  {
    ${RELOCATING+__bss_start = .;}
    ${RELOCATING+___a4_init2 = 0x7ffe;}
    *(.bss)
    *(.bss.*)
    *(COMMON)
    ${RELOCATING+__end = .;}
  }
  ${RELOCATING+___data_size = SIZEOF(.data) + SIZEOF(.bss);}
  ${RELOCATING+___bss_size = 0x0;}
  ${RELOCATING+___bss_init_size = SIZEOF(.bss);}
  .datachip :
  {
    *(.datachip)
  }
  .bsschip :
  {
    *(.bsschip)
  }  
}
EOF
