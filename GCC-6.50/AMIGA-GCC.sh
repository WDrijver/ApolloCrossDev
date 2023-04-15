# ApolloCrossDev - Amiga-GCC - Build Script v0.1

EDITION=Amiga-GCC
VERSION=0.1
CPU=-j1 # Maximum = 8 Cores

SOURCES=_sources
LOGS=_logs
APOLLOCROSSDEV=ApolloCrossDev

export PREFIX="`pwd`/$APOLLOCROSSDEV"
export LOGFILES="`pwd`/$LOGS"

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

# PART 3: Download Amiga-GCC
echo "3. Download Amiga-GCC"
git clone --progress https://github.com/bebbo/amiga-gcc 2>>$LOGFILES/part3.log

# Part 4: Compile Amiga-GCC
echo "4. Compile Amiga-GCC"
cd amiga-gcc
make clean
make $CPU all PREFIX=/home/willem/ApolloCrossDev/Amiga-GCC/ApolloCrossDev
cd ..

exit

# PART 8: Download Amiga OS NDK's
echo "8. Download AmigaOS NDK's"
echo "   * Installing LHA" 
apt -y install lhasa >>$LOGFILES/part8.log 2>/dev/null
mkdir NDK3.2
cd NDK3.2
echo "   * Download NDK3.2.lha from AmiNet" 
wget -nc http://aminet.net/dev/misc/NDK3.2.lha -a $LOGFILES/part8.log
echo "   * Extracting Archive" 
lha -xw=$APOLLOCROSSDEV/include/NDK3.2 NDK3.2.lha >>$LOGFILES/part8.log
echo "   * Adding default GCC Includes"
cp $APOLLOCROSSDEV/lib/gcc/m68k-amiga-elf/12.2.0/include/*.* $APOLLOCROSSDEV/include/NDK3.2 >>$LOGFILES/part8.log
rm $APOLLOCROSSDEV/include/NDK3.2/stdint.h
mv $APOLLOCROSSDEV/include/NDK3.2/stdint-gcc.h $APOLLOCROSSDEV/include/NDK3.2/stdint.h
cd ..
mkdir NDK3.9
cd NDK3.9
echo "   * Download NDK3.9 from os.amigaworld.de" 
wget -nc https://os.amigaworld.de/download.php?id=3 -a $LOGFILES/part8.log
echo "   * Extracting Archive" 
lha -xw=$APOLLOCROSSDEV/include/NDK3.9 NDK39.lha >>$LOGFILES/part8.log
echo "   * Adding default GCC Includes"
cp $APOLLOCROSSDEV/lib/gcc/m68k-amiga-elf/12.2.0/include/*.* $APOLLOCROSSDEV/include/NDK3.9 >>$LOGFILES/part8.log
rm $APOLLOCROSSDEV/include/NDK3.9/stdint.h
mv $APOLLOCROSSDEV/include/NDK3.9/stdint-gcc.h $APOLLOCROSSDEV/include/NDK3.9/stdint.h
cd ..

# FINISH
echo " "
echo "FINISHED"
echo " "
exit
