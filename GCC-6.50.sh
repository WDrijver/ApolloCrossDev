# ApolloCrossDev GCC-6.50 - Install Script v0.91

EDITION=GCC-6.50
VERSION=0.91
CPU=-j1

WORKSPACE="`pwd`"
COMPILERS=Compilers
PROJECTS=Projects
COMPILER=GCC-6.50
TARGET=m68k-amigaos
PREFIX=$WORKSPACE/$COMPILERS/$COMPILER

ARCHIVES=$WORKSPACE/$COMPILERS/_archives
LOGFILES=$PREFIX/_logs
BUILDS=$PREFIX/_builds
SOURCES=$PREFIX/_sources

export PATH=$PREFIX/bin:$PATH

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
mkdir -p $PREFIX
mkdir -p $LOGFILES
rm -f -r $PROJECTS/bgdbserver
rm -f -r $SOURCES
mkdir -p $SOURCES
cd $SOURCES

# PART 2: Update Linux Packages 
echo -e "\e[1m\e[37m2. Update Linux Packages\e[0m\e[36m"
sudo apt -y update >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log
sudo apt -y install make wget git gcc g++ lhasa libgmp-dev libmpfr-dev libmpc-dev flex bison gettext texinfo ncurses-dev autoconf rsync libreadline-dev >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log
sudo apt -y install build-essential devscripts debhelper qtbase5-dev qtbase5-dev-tools qt5-qmake libqt5x11extras5-dev qttools5-dev-tools >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log

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
echo -e "\e[0m\e[36m   * Cloning Amiga-GCC (be patient)\e[0m"
make update $CPU NDK=3.2 PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
echo -e "\e[0m\e[36m   * Build Amiga-GCC (be patient)\e[0m"
make all $CPU NDK=3.2 PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# Part 5: MUI
echo -e "\e[1m\e[37m5. Adding MUI5\e[0m\e[36m"
cd $SOURCES/amiga-gcc
make sdk=mui PREFIX=$PREFIX >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
cp -r -f $SOURCES/amiga-gcc/build/mui/SDK/MUI/C/include/mui/* $PREFIX/$TARGET/include/mui >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log

# Part 6: SDL
echo -e "\e[1m\e[37m6. Adding Open-GL, GDB, SDL, TTF and FreeType\e[0m\e[36m"
cd $PREFIX
cp -r -f $PREFIX/include/GL $PREFIX/$TARGET/include/GL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f $PREFIX/include/gdb $PREFIX/$TARGET/include/GDB >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f $PREFIX/include/SDL $PREFIX/$TARGET/include/SDL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f $PREFIX/lib/libSDL* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
rm -r -f $PREFIX/lib/libSDL* >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
rm -r -f $PREFIX/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cd $ARCHIVES
cp -r -f SDL/* $PREFIX/$TARGET >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

# PART 7: NDK
echo -e "\e[1m\e[37m7. Development Kits\e[0m\e[36m"
cd $PREFIX/$TARGET
git clone --progress https://github.com/WDrijver/DevPac >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
cd $ARCHIVES
cp -r -f P96/* $PREFIX/$TARGET/include >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log

# Part 8: ApolloExplorer
echo -e "\e[1m\e[37m8. ApolloExplorer\e[0m\e[36m"
cd $WORKSPACE/$PROJECTS
git clone --progress https://github.com/ronybeck/ApolloExplorer >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log
cd $WORKSPACE/$PROJECTS/ApolloExplorer
qmake >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log
make >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log

# Part 9: BGDBServer
echo -e "\e[1m\e[37m9. BGDG Server\e[0m\e[36m"
cd $WORKSPACE/$PROJECTS
git clone --progress https://franke.ms/git/bebbo/bgdbserver >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log
cd $WORKSPACE/$PROJECTS/bgdbserver
make >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log

# PART 10: Cleanup
echo -e "\e[1m\e[37m10. Cleanup\e[0m\e[36m"
cd $PREFIX
rm -rf info
rm -rf man

# FINISH
echo " "
echo -e "\e[1m\e[32mFINISHED\e[0m"
echo " "
exit
