# ApolloCrossDev Build Script v0.2

EDITION=GNU-3.4.6
VERSION=0.2
CPU=-j16

WORKSPACE="`pwd`"
SOURCES=$WORKSPACE/_sources
LOGFILES=$WORKSPACE/_logs
PREFIX=$WORKSPACE/ApolloCrossDev
TARGET=m68k-amigaos
export PATH=$PREFIX/bin:$PATH

BINUTILS_VERSION=amigaos-binutils-2.14
BINUTILS_DOWNLOAD=https://github.com/adtools/$BINUTILS_VERSION
GCC_VERSION=gcc-3.4.6
GCC_DOWNLOAD=https://ftp.gnu.org/gnu/gcc/$GCC_VERSION/$GCC_VERSION.tar.gz
NDK32_DOWNLOAD=http://aminet.net/dev/misc/NDK3.2.lha
NDK39_DOWNLOAD=https://os.amigaworld.de/download.php?id=3

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
echo "2. Update Essential Linux Packages"
apt -y update >>$LOGFILES/part2.log 2>/dev/null
apt -y install build-essential gawk flex bison expect dejagnu texinfo lhasa >>$LOGFILES/part2.log 2>/dev/null

# PART 3: Download GNU-Sources
echo "3. Download GNU-Sources"
echo "   * $BINUTILS_VERSION" 
git clone --progress $BINUTILS_DOWNLOAD 2>>$LOGFILES/part3_err.log
echo "   * $GCC_VERSION" 
#wget -nc https://ftp.gnu.org/gnu/gcc/$GCC_VERSION/$GCC_VERSION.tar.gz -a $LOGFILES/part3.log
cp ../gcc-3.4.6.tar.* .

# PART 4: Unpack GNU-Sources
echo "4. Unpack GNU-Sources"
for f in *.tar*; do tar xfk $f >>$LOGFILES/part4.log; done 

# Part 4: Compile BinUtils
echo "4. Compile BinUtils"
mkdir -p build-binutils
echo "   * Configure"
cd build-binutils
../$BINUTILS_VERSION/configure \
    --disable-nls \
    --prefix="$PREFIX" \
    --with-gnu-as \
    --with-gnu-ld \
    --with-headers=$PREFIX/m68k-amigaos/include \
    --host=i686-linux-gnu \
    --build=i686-linux-gnu \
    --target=$TARGET >>$LOGFILES/part4.log 2>/dev/null
echo "   * Build ($CPU)"
make $CPU >>$LOGFILES/part4.log 2>/dev/null
echo "   * Install ($CPU)"
make $CPU install-binutils >>$LOGFILES/part4.log 2>/dev/null
make $CPU install-gas >>$LOGFILES/part4.log 2>/dev/null
make $CPU install-ld >>$LOGFILES/part4.log 2>/dev/null
cd ..
exit
# Part 6: Compile GCC
echo "6. Compile GCC"
mkdir -p build-gcc
cd build-gcc
echo "   * Configure"
../$GCC_VERSION/configure \
    --prefix="$PREFIX" \
    --enable-languages=c,c++ \
    --target=$TARGET  \
    >>$LOGFILES/part6.log 2>/dev/null  
echo "   * Build (single CPU only)"
make -j1 all-gcc >>$LOGFILES/part6.log 2>/dev/null
echo "   * Install (single CPU only)"
make -j1 install-gcc >>$LOGFILES/part6.log 2>/dev/null
cd ..

# PART 7: Amiga Libs/Includes
echo "7. Amiga Libs"
echo "   * libnix"
cp -r $WORKSPACE/../Sources/_amigalibs/libnix $PREFIX/$TARGET
echo "   * clib2"
cp -r $WORKSPACE/../Sources/_amigalibs/clib2 $PREFIX/$TARGET

# PART 8: Download Amiga OS NDK's
echo "8. Download AmigaOS NDK's"
mkdir NDK3.2
cd NDK3.2
echo "   * Download NDK3.2.lha from AmiNet" 
wget -nc $NDK32_DOWNLOAD -a $LOGFILES/part8.log
echo "   * Extracting Archive" 
lha -xw=$PREFIX/include/NDK3.2 NDK3.2.lha >>$LOGFILES/part8.log
cd ..
mkdir NDK3.9
cd NDK3.9
echo "   * Download NDK3.9 from os.amigaworld.de" 
wget -nc $NDK39_DOWNLOAD -a $LOGFILES/part8.log
mv download.php?id=3 NDK39.lha
echo "   * Extracting Archive" 
lha -xw=$PREFIX/include NDK39.lha >>$LOGFILES/part8.log
mv $PREFIX/include/NDK_3.9 $PREFIX/include/NDK3.9
rm -r $PREFIX/include/ndk_3.9
rm $PREFIX/include/NDK_3.9.info
cd ..

# PART 9: Cleanup
echo "9. Cleanup"
cd $PREFIX
rm -rf info
rm -rf man
rm -rf $TARGET/include

# FINISH
echo " "
echo "FINISHED"
echo " "
exit
