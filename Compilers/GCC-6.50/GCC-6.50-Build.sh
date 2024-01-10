# ApolloCrossDev GCC-6.50 - Install Script v0.6
# 
# Installation:
# 1. Enter Compilers/GCC-6.5.0 directory
# 2. Type "./GCC-6.5.0.sh" and hit ENTER
#
# Instructions:
# 1. Create Projects/<mysource> directory
# 2. Copy Projects/make-gcc650 into <mysource> 
# 3. Read make-gcc650 for compile instructions

EDITION=GCC-6.50
VERSION=0.7
CPU=-j16

WORKSPACE="`pwd`"
ARCHIVES=$WORKSPACE/_archives
SOURCES=$WORKSPACE/_sources
PATCHES=$WORKSPACE/_install/patches
LOGFILES=$WORKSPACE/_logs
PREFIX=$WORKSPACE/ApolloCrossDev
TARGET=m68k-amigaos
export PATH=$PREFIX/bin:$PATH

# INIT Terminal
clear
echo -e "\e[1m\e[37m########## \e[31mApollo\e[1;30mCrossDev \e[36m$EDITION\e[30m v$VERSION \e[37m ##########\e[0m\e[36m"
echo " "
echo -e "\e[1m\e[37m0. Sudo Password\e[0m"

# Part 4: Compile Amiga-GCC - First Run
echo -e "\e[1m\e[37m4. Compile Amiga-GCC - First Run\e[0m\e[36m"
cd $SOURCES/amiga-gcc
echo -e "\e[0m\e[36m   * Clean Amiga-GCC\e[0m"
make clean >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
echo -e "\e[0m\e[36m   * Clean ApolloCrossDev\e[0m"
make drop-prefix PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
echo -e "\e[0m\e[36m   * Build Amiga-GCC (be patient)\e[0m"
make all -j3 PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# Part 7: SDL
echo -e "\e[1m\e[37m7. Adding SDL include and lib files\e[0m\e[36m"
cd $ARCHIVES
cp -r -f SDL/* $PREFIX/$TARGET >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
cp -r -f sys-include/* $PREFIX/$TARGET/sys-include >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
rm -r -f $PREFIX/include/SDL* >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log

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
