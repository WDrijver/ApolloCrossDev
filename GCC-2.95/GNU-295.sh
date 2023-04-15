# ApolloCrossDev Build Script v0.1

EDITION=GNU-2.95.3
VERSION=0.1
CPU=-j16
SOURCES=_sources
LOGS=_logs
APOLLOCROSSDEV=ApolloCrossDev
TARGET=m68k-amigaos

BINUTILS_VERSION=amigaos-binutils-2.14
GCC_VERSION=amigaos-gcc-2.95.3
IXEMUL_ARCHIVE=ixemul-src.lha
IXEMUL_DIR=ixemul

BINUTILS_DOWNLOAD=https://github.com/adtools/$BINUTILS_VERSION
GCC_DOWNLOAD=https://github.com/adtools/$GCC_VERSION
IXEMUL_DOWNLOAD=http://downloads.sf.net/project/amiga/ixemul.library/48.2/ixemul-src.lha
# LIBNIX_DOWNLOAD=https://github.com/adtools/libnix

#   url = git://github.com/adtools/clib2

#	url = git://github.com/cahirwpz/fd2sfd
#	url = git://github.com/adtools/sfdc
#	url = git://github.com/cahirwpz/libdebug
#	url = git://github.com/FrodeSolheim/python-lhafile
#	url = https://github.com/adtools/fd2pragma.git

export PREFIX="`pwd`/$APOLLOCROSSDEV"
export PATH=$PREFIX/bin:$PATH
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
echo "2. Update Essential Linux Packages"
apt -y update >>$LOGFILES/part2.log 2>/dev/null
apt -y install build-essential gawk flex bison expect dejagnu texinfo lhasa >>$LOGFILES/part2.log 2>/dev/null

# PART 3: Download GNU-Sources
echo "3. Download GNU-Sources"
echo "   * $BINUTILS_VERSION" 
git clone --progress $BINUTILS_DOWNLOAD 2>>$LOGFILES/part3_err.log
echo "   * $GCC_VERSION" 
git clone --progress $GCC_DOWNLOAD 2>>$LOGFILES/part3_err.log

# Part 5: Compile BinUtils
echo "5. Compile BinUtils"
mkdir -p build-binutils
echo "   * Configure"
cd build-binutils
../$BINUTILS_VERSION/configure \
    --disable-nls \
    --infodir=$PREFIX/$TARGET/info \
    --mandir=$PREFIX/share/man \
    --prefix="$PREFIX" \
    --host=i686-linux-gnu \
    --target=$TARGET >>$LOGFILES/part5.log 2>/dev/null
echo "   * Build ($CPU)"
make $CPU >>$LOGFILES/part5.log 2>/dev/null
echo "   * Install ($CPU)"
make $CPU install-binutils >>$LOGFILES/part5.log 2>/dev/null
make $CPU install-gas >>$LOGFILES/part5.log 2>/dev/null
make $CPU install-ld >>$LOGFILES/part5.log 2>/dev/null
make $CPU install-info >>$LOGFILES/part5.log 2>/dev/null
cd ..

# Part 6: IXEMUL
echo "6. Download $IXEMUL_ARCHIVE"
wget -nc $IXEMUL_DOWNLOAD -a $LOGFILES/part6.log
echo "   * Extracting Archive" 
lha -xw=$PREFIX/include $IXEMUL_ARCHIVE >>$LOGFILES/part6.log

# Part 6: Compile GCC
echo "6. Compile GCC"
mkdir -p build-gcc
cd build-gcc
echo "   * Configure"
../$GCC_VERSION/configure \
    --infodir=$PREFIX/$TARGET/info \
    --mandir=$PREFIX/share/man \
    --prefix="$PREFIX" \
    --with-headers=$PREFIX/include/$IXEMUL_DIR/include \
    --enable-languages=c,c++ \
    --enable-version-specific-runtime-libs \
    --host=i686-linux-gnu \
    --build=i686-linux-gnu \
    --target=$TARGET  \
    >>$LOGFILES/part6.log 2>/dev/null  
echo "   * Build (single CPU only)"
make -j1 all-gcc >>$LOGFILES/part6.log 2>/dev/null
echo "   * Install (single CPU only)"
make -j1 install-gcc >>$LOGFILES/part6.log 2>/dev/null
cd ..



# PART 3: Download Libnix
#echo "3. Download Libnix"
#git clone --progress $LIBNIX_DOWNLOAD 2>>$LOGFILES/part3_err.log

# PART 8: Download Amiga OS NDK's
echo "8. Download AmigaOS NDK's"
mkdir NDK3.2
cd NDK3.2
echo "   * Download NDK3.2.lha from AmiNet" 
wget -nc http://aminet.net/dev/misc/NDK3.2.lha -a $LOGFILES/part8.log
echo "   * Extracting Archive" 
lha -xw=$PREFIX/include/NDK3.2 NDK3.2.lha >>$LOGFILES/part8.log
cd ..
mkdir NDK3.9
cd NDK3.9
echo "   * Download NDK3.9 from os.amigaworld.de" 
wget -nc https://os.amigaworld.de/download.php?id=3 -a $LOGFILES/part8.log
mv download.php?id=3 NDK39.lha
echo "   * Extracting Archive" 
lha -xw=$PREFIX/include NDK39.lha >>$LOGFILES/part8.log
mv $PREFIX/include/NDK_3.9 $PREFIX/include/NDK3.9
rm -r $PREFIX/include/ndk_3.9
rm $PREFIX/include/NDK_3.9.info
cd ..

# FINISH
echo " "
echo "FINISHED"
echo " "
exit
