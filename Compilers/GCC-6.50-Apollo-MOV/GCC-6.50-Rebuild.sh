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

cd $SOURCES

# Part 6: Compile Amiga-GCC - Rebuild
echo -e "\e[1m\e[37m6. Compile Amiga-GCC - Rebuild\e[0m\e[36m"
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
