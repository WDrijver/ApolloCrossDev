# ApolloCrossDev GCC-6.50 - Install Script v0.9 - Stable from WD Repo (= Bebbo December 2023, before starting 68080 changes)

EDITION=GCC-6.50
VERSION=0.9
CPU=-j16

WORKSPACE="`pwd`"
COMPILER=GCC-6.50

ARCHIVES=$WORKSPACE/_archives
BUILDS=$WORKSPACE/$COMPILER/_builds
SOURCES=$WORKSPACE/$COMPILER/_sources
PATCHES=$WORKSPACE/$COMPILER/_install/patches
LOGFILES=$WORKSPACE/$COMPILER/_logs
PREFIX=$WORKSPACE/$COMPILER
TARGET=m68k-amigaos
export PATH=$PREFIX/bin:$PATH

MUI5_ARCHIVE=MUI-5.0-20210831-os3.lha
NDK32_DOWNLOAD=http://aminet.net/dev/misc/NDK3.2.lha

# INIT Terminal
clear
echo -e "\e[1m\e[37m########## \e[31mApollo\e[1;30mCrossDev \e[36m$EDITION\e[30m v$VERSION \e[37m ###########\e[0m\e[36m"
echo -e "\e[1m\e[37m#"
echo -e "\e[1m\e[37m# \e[0mBuilding with CPU=$CPU | If Build fails set CPU=-j1\e[0m\e[36m"
echo " "
echo -e "\e[1m\e[37m0. Sudo Password\e[0m"

# PART 1: Clean the House
sudo echo -e "\e[1m\e[37m1. Clean the House\e[0m\e[36m"
rm -f -r $PREFIX
mkdir $PREFIX
rm -f -r $LOGFILES
mkdir -p $LOGFILES
rm -f -r $BUILDS
mkdir -p $BUILDS
rm -f -r $SOURCES
mkdir $SOURCES
cd $SOURCES

# PART 2: Update Linux Packages 
echo -e "\e[1m\e[37m2. Update Linux Packages\e[0m\e[36m"
sudo apt -y update >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log
sudo apt -y install build-essential gawk flex bison expect dejagnu texinfo lhasa git subversion \
     make wget libgmp-dev libmpfr-dev libmpc-dev gettext texinfo ncurses-dev autoconf rsync libreadline-dev >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log
 
# PART 3: Clone Amiga-GCC
echo -e "\e[1m\e[37m3. Clone Amiga-GCC (Stefan -Bebbo- Franke)\e[0m\e[36m"
git clone --progress https://github.com/WDrijver/amiga-gcc 2>>$LOGFILES/part3_err.log

# Part 4: Compile Amiga-GCC
echo -e "\e[1m\e[37m4. Compile Amiga-GCC\e[0m\e[36m"
cd $SOURCES/amiga-gcc
echo -e "\e[0m\e[36m   * Clean Amiga-GCC\e[0m"
make clean >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
echo -e "\e[0m\e[36m   * Clean ApolloCrossDev\e[0m"
make drop-prefix PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
echo -e "\e[0m\e[36m   * Build Amiga-GCC (be patient)\e[0m"
make all $CPU PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# Part 5: SDL
echo -e "\e[1m\e[37m5. Adding Open-GL, SDL, TTF and FreeType\e[0m\e[36m"
cd $PREFIX
cp -r -f $PREFIX/include/GL $PREFIX/$TARGET/include/GL >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
cp -r -f $PREFIX/include/gdb $PREFIX/$TARGET/include/GDB >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
cp -r -f $PREFIX/include/SDL $PREFIX/$TARGET/include/SDL >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
cp -r -f $PREFIX/include/SDL_*.* $PREFIX/$TARGET/include/SDL  >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
cp -r -f $PREFIX/lib/libSDL* $PREFIX/$TARGET/lib >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
rm -r -f $PREFIX/lib/libSDL* >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
rm -r -f $PREFIX/include >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
cd $ARCHIVES
cp -r -f SDL/* $PREFIX/$TARGET >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log

# Part 6: MUI
echo -e "\e[1m\e[37m6. Adding MUI5\e[0m\e[36m"
cd $ARCHIVES/MUI5
lha -xw=$SOURCES/MUI5 $MUI5_ARCHIVE >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
mkdir $PREFIX/$TARGET/include/MUI >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -rf $SOURCES/MUI5/SDK/MUI/C/include/* $PREFIX/$TARGET/include/MUI >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -rf $SOURCES/MUI5/SDK/MUI/C/lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

# PART 7: NDK
echo -e "\e[1m\e[37m8. Development Kits\e[0m\e[36m"
mv $PREFIX/$TARGET/ndk-include $PREFIX/$TARGET/ndk39-include
cd $PREFIX/$TARGET
git clone --progress https://github.com/WDrijver/DevPac >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
cd $SOURCES
wget -nc $NDK32_DOWNLOAD -a  $LOGFILES/part7.log
mkdir $PREFIX/$TARGET/ndk32-include
lha -xw=$PREFIX/$TARGET/ndk32-include NDK3.2.lha >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log

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
