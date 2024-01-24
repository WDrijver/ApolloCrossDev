# ApolloCrossDev GCC-6.50 - Install Script v0.7 - Apollo Dev Patches

EDITION=GCC-6.50
VERSION=0.7
CPU=-j16

WORKSPACE="`pwd`"
ARCHIVES=$WORKSPACE/_archives
BUILDS=$WORKSPACE/_builds
SOURCES=$WORKSPACE/_sources
PATCHES=$WORKSPACE/_patches
LOGFILES=$WORKSPACE/_logs
PREFIX=$WORKSPACE/ApolloCrossDev
TARGET=m68k-amigaos
export PATH=$PREFIX/bin:$PATH

# INIT Terminal
clear
echo -e "\e[1m\e[37m########## \e[31mApollo\e[1;30mCrossDev \e[36m$EDITION\e[30m v$VERSION \e[37m ##########\e[0m\e[36m"
echo " "
echo -e "\e[1m\e[37m0. Sudo Password\e[0m"

# PART 5: Patch Amiga-GCC with additional Apollo Development Patches
echo -e "\e[1m\e[37m5. Patch Amiga-GCC with additional Apollo Opcodes not yet adopted by (Stefan -Bebbo- Franke)\e[0m\e[36m"

if [ -f $PATCHES/m68k-opc.c ]; then
    echo     m68k-opc.c found: creating m68k-opc.c.org backup and copy new m68k-opc.cs
    cp -f $SOURCES/amiga-gcc/projects/binutils/opcodes/m68k-opc.c $SOURCES/amiga-gcc/projects/binutils/opcodes/m68k-opc.c.org >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
    cp $PATCHES/m68k-opc.c $SOURCES/amiga-gcc/projects/binutils/opcodes   >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
fi
if [ -f $PATCHES/tc-m68k.c ]; then
    echo  tc-m68k.c found: creating tc-m68k.c.org backup and copy new tc-m68k.c
    cp -f $SOURCES/amiga-gcc/projects/binutils/gas/config/tc-m68k.c $SOURCES/amiga-gcc/projects/binutils/gas/config/tc-m68k.c.org >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
    cp $PATCHES/tc-m68k.c $SOURCES/amiga-gcc/projects/binutils/gas/config >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
fi
if [ -f $PATCHES/m68k.c ]; then
    echo  m68k.c found: creating m68k.c.org backup and copy new m68k.c
    cp -f $SOURCES/amiga-gcc/projects/gcc/gcc/config/m68k/m68k.c $SOURCES/amiga-gcc/projects/gcc/gcc/config/m68k/m68k.c.org >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
    cp $PATCHES/m68k.c $SOURCES/amiga-gcc/projects/gcc/gcc/config/m68k    >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
fi
if [ -f $PATCHES/m68k.h ]; then
    echo  m68k.h found: creating m68k.h.org backup and copy new m68k.h
    cp -f $SOURCES/amiga-gcc/projects/gcc/gcc/config/m68k/m68k.h $SOURCES/amiga-gcc/projects/gcc/gcc/config/m68k/m68k.h.org >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
    cp $PATCHES/m68k.h $SOURCES/amiga-gcc/projects/gcc/gcc/config/m68k    >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
fi
if [ -f $PATCHES/m68k.md ]; then
    echo      m68k.md found: creating m68k.md.org backup and copy new m68k.md
    cp -f $SOURCES/amiga-gcc/projects/gcc/gcc/config/m68k/m68k.md $SOURCES/amiga-gcc/projects/gcc/gcc/config/m68k/m68k.md.org >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
    cp $PATCHES/m68k.md $SOURCES/amiga-gcc/projects/gcc/gcc/config/m68k   >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
fi

cd $SOURCES

# Part 6: Compile Amiga-GCC - Rebuild
echo -e "\e[1m\e[37m6. Compile Amiga-GCC - Rebuild with Patches\e[0m\e[36m"
cd $SOURCES/amiga-gcc
echo -e "\e[0m\e[36m   * Build Amiga-GCC\e[0m"
make all -j16 PREFIX=$PREFIX >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

# PART 8: Cleanup
echo -e "\e[1m\e[37m8. Cleanup\e[0m\e[36m"
cd $PREFIX
rm -rf info
rm -rf man

# FINISH
echo " "
echo -e "\e[1m\e[32mFINISHED\e[0m"
echo " "
exit
