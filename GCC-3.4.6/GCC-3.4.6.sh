# ApolloCrossDev Build Script v0.2

EDITION=GNU-3.4.6
VERSION=0.2
CPU=-j16

WORKSPACE="`pwd`"
SOURCES=$WORKSPACE/_sources
LOGFILES=$WORKSPACE/_logs
PREFIX=$WORKSPACE/ApolloCrossDev
TARGET=m68k-unknown-amigaos
export PATH=$PREFIX/bin:$PATH

GCC_AUTOCONF=autoconf2.64
GCC_AUTOHEADER=autoheader2.64
GCC_AUTORECONF=autoreconf2.64
GCC_AUTOM4TE=autom4te2.64

BINUTILS_VERSION=amigaos-binutils-2.14
BINUTILS_DOWNLOAD=https://github.com/adtools/$BINUTILS_VERSION
GCC_VERSION=gcc-3.4.6
GCC_DOWNLOAD=https://ftp.gnu.org/gnu/gcc/$GCC_VERSION/$GCC_VERSION.tar.gz
GMP_VERSION=gmp-4.3.2
GMP_DOWNLOAD=https://ftp.gnu.org/gnu/gmp/$GMP_VERSION.tar.gz
MPFR_VERSION=mpfr-2.4.2
MPFR_DOWNLOAD=http://www.mpfr.org/$MPFR_VERSION/$MPFR_VERSION.tar.gz
MPC_VERSION=mpc-0.8.2
MPC_DOWNLOAD=http://www.multiprecision.org/downloads/$MPC_VERSION.tar.gz
BISON_VERSION=bison-2.7.1
BISON_DOWNLOAD=https://ftp.gnu.org/gnu/bison/$BISON_VERSION.tar.gz
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
sudo apt -y update >>$LOGFILES/part2.log 2>/dev/null
sudo apt -y install build-essential gawk flex bison expect dejagnu texinfo lhasa >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log

# PART 3: Download GNU-Sources
echo "3. Download GNU-Sources"
echo "   * $BINUTILS_VERSION" 
git clone --progress $BINUTILS_DOWNLOAD >>$LOGFILES/part3.log 2>>$LOGFILES/part3_err.log
echo "   * $GCC_VERSION" 
wget -nc $GCC_DOWNLOAD -a $LOGFILES/part3.log
echo "   * $GMP_VERSION" 
wget -nc $GMP_DOWNLOAD -a $LOGFILES/part3.log
echo "   * $MPFR_VERSION" 
wget -nc $MPFR_DOWNLOAD -a $LOGFILES/part3.log
echo "   * $MPC_VERSION" 
wget -nc $MPC_DOWNLOAD -a $LOGFILES/part3.log
echo "   * $BISON_VERSION" 
wget -nc $BISON_DOWNLOAD -a $LOGFILES/part3.log

# PART 4: Unpack GNU-Sources
echo "4. Unpack GNU-Sources"
for f in *.tar*; do tar xfk $f >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log; done 

# Part 5: Compile Bison
echo "5. Compile Bison"
echo "   * Patch Bison files"
for p in `ls $WORKSPACE/_install/recipes/patches/bison/*.p`; do patch -d $WORKSPACE/_sources/$BISON_VERSION <$p -p0 >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log; done 
echo "   * Configure"
mkdir -p build-bison
cd build-bison
../$BISON_VERSION/configure \
    --prefix="$PREFIX" >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
echo "   * Build ($CPU)"
make $CPU >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
echo "   * Install ($CPU)"
make $CPU install >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
cd ..

# Part 6: Compile BinUtils
echo "6. Compile BinUtils"
mkdir -p build-binutils
echo "   * Configure"
cd build-binutils
CFLAGS="-m32" LDFLAGS="-m32" ../$BINUTILS_VERSION/configure \
    --prefix="$PREFIX" \
    --target=$TARGET \
    --disable-nls \
    --disable-werror \
    >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
echo "   * Build ($CPU)"
make $CPU >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
echo "   * Install ($CPU)"
make $CPU install-binutils >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make $CPU install-gas >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make $CPU install-ld >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cd ..

# Part 7 Prepare GCC Sources
echo "7. Prepare GCC Sources"
echo "   * Move $GMP_VERSION to $GCC_VERSION/gmp"
mv $GMP_VERSION $GCC_VERSION/gmp >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
echo "   * Move $MPFR_VERSION to $GCC_VERSION/mpfr"
mv $MPFR_VERSION $GCC_VERSION/mpfr >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
echo "   * Move $MPC_VERSION to $GCC_VERSION/mpc"
mv $MPC_VERSION $GCC_VERSION/mpc >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
echo "   * Patch GCC files"
for p in `ls $WORKSPACE/_install/recipes/patches/gcc/*.p`; do patch -d $WORKSPACE/_sources/$GCC_VERSION <$p -p0 >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log; done 
patch $WORKSPACE/_sources/$GCC_VERSION/gcc/collect2.c $WORKSPACE/_install/recipes/patches.wd/collect2.c.p >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
echo "   * Add m68k-amigaos files"
cp -r $WORKSPACE/_install/recipes/files/gcc/* $GCC_VERSION >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log

# Part 8: Compile GCC
echo "8. Compile GCC"
mkdir -p build-gcc
cd build-gcc
echo "   * Configure"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" ../$GCC_VERSION/configure \
	--disable-threads \
	--disable-nls --disable-c-mbchar \
	--enable-languages=c --enable-checking=no \
	--enable-c99 --with-cross-host \
	--disable-multilib --without-x \
	--enable-maintainer-mode --disable-shared \
	--without-headers \
    --prefix="$PREFIX" \
    --target=$TARGET \
    >>$LOGFILES/part8.log 2>$LOGFILES/part8_err.log 
echo "   * Build (single CPU only)"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" make -j1 all-gcc >>$LOGFILES/part8.log 2>$LOGFILES/part8_err.log
echo "   * Install (single CPU only)"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" make -j1 install-gcc >>$LOGFILES/part8.log 2>$LOGFILES/part8_err.log
cd ..
exit

# PART 7: Amiga Libs/Includes
echo "7. Amiga Libs"
echo "   * libnix"
cp -r $WORKSPACE/_install/libnix $PREFIX/$TARGET
echo "   * clib2"
cp -r $WORKSPACE/_install/clib2 $PREFIX/$TARGET

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
