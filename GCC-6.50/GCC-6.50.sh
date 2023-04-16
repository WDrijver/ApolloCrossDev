# ApolloCrossDev - GCC-6.50 - Build Script v0.2

EDITION=GCC-6.50
VERSION=0.2
CPU=-j16

WORKSPACE="`pwd`"
SOURCES=$WORKSPACE/_sources
LOGFILES=$WORKSPACE/_logs
PREFIX=$WORKSPACE/ApolloCrossDev
TARGET=m68k-amigaos
export PATH=$PREFIX/bin:$PATH

# INIT Terminal
clear
echo "########## ApolloCrossDev $EDITION v$VERSION ##########"
echo " "
echo "0. Sudo Password"

# PART 1: Clean the House
sudo echo "1. Clean the House"
rm -f -r $PREFIX
mkdir $PREFIX
rm -f -r $LOGFILES
mkdir -p $LOGFILES
rm -f -r $SOURCES
mkdir $SOURCES
cd $SOURCES
 
# PART 2: Update Linux Packages 
echo "2. Update Linux Packages"
apt -y update >>$LOGFILES/part2.log 2>/dev/null
apt -y install make wget git gcc g++ lhasa libgmp-dev libmpfr-dev libmpc-dev flex bison gettext texinfo ncurses-dev autoconf rsync libreadline-dev >>$LOGFILES/part2.log 2>/dev/null

# PART 3: Clone Amiga-GCC
echo "3. Clone Amiga-GCC (Stefan -Bebbo- Franke)"
git clone --progress https://github.com/bebbo/amiga-gcc 2>>$LOGFILES/part3.log

# Part 4: Compile Amiga-GCC
echo "4. Compile Amiga-GCC"
cd amiga-gcc
make clean
make $CPU all PREFIX=$PREFIX
cd ..

# Part 4: Compile Amiga-GCC
echo "4. Compile Amiga-GCC"
cd amiga-gcc
make $CPU all PREFIX=$PREFIX
make $CPU all PREFIX=$PREFIX
cd ..

# FINISH
echo " "
echo "FINISHED"
echo " "
exit
