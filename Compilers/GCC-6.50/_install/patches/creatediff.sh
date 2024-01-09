clear
echo "Creating Diff files to be used for Patching GCC-6.50"
echo " "
echo "1. diff m68k.c m68k.c.original > m68k.c.diff"
diff m68k.c m68k.c.original > m68k.c.diff
echo "2. diff m68k.h m68k.h.original > m68k.h.diff"
diff m68k.h m68k.h.original > m68k.h.diff
echo "3. diff m68k.md m68k.md.original > m68k.md.diff"
diff m68k.md m68k.md.original > m68k.md.diff
echo "4. diff m68k-opc.c m68k-opc.c.original > m68k-opc.c.diff"
diff m68k-opc.c m68k-opc.c.original > m68k-opc.c.diff
echo "5. diff tc-m68k.c tc-m68k.c.original > tc-m68k.c.diff"
diff tc-m68k.c tc-m68k.c.original > tc-m68k.c.diff
echo " "
