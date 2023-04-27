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
CLIB2_VERSION=V1_214
CLIB2_DOWNLOAD=https://github.com/adtools/clib2/archive/$CLIB2_VERSION.tar.gz
#CLIB2_DOWNLOAD=https://github.com/adtools/clib2
NDK32_DOWNLOAD=http://aminet.net/dev/misc/NDK3.2.lha
NDK39_DOWNLOAD=https://os.amigaworld.de/download.php?id=3
NDK_VERSION=3.9
NDK_ARCHIVE=ndk-$NDK_VERSION-includes.tar.bz2
NDK_DOWNLOAD=http://kas1e.mikendezign.com/zerohero_crosscompilers_backup/files/m68k-amigaos/$NDK_ARCHIVE
OPENURL_VERSION=7.16
OPENURL_ARCHIVE=OpenURL-$OPENURL_VERSION.lha
OPENURL_DOWNLOAD=https://github.com/jens-maus/libopenurl/releases/download/$OPENURL_VERSION/$OPENURL_ARCHIVE
AMISSL_VERSION=4.4
AMISSL_ARCHIVE=AmiSSL-$AMISSL_VERSION.lha 
AMISSL_DOWNLOAD=https://github.com/jens-maus/amissl/releases/download/$AMISSL_VERSION/$AMISSL_ARCHIVE
GUIGFX_ARCHIVE=guigfxlib.lha
GUIGFX_DOWNLOAD=http://neoscientists.org/~bifat/binarydistillery/$GUIGFX_ARCHIVE
RENDER_ARCHIVE=renderlib.lha
RENDER_DOWNLOAD=http://neoscientists.org/~bifat/binarydistillery/$RENDER_ARCHIVE
CODESETS_VERSION=6.20
CODESETS_ARCHIVE=codesets-$CODESETS_VERSION.lha
CODESETS_DOWNLOAD=https://github.com/jens-maus/libcodesets/releases/download/$CODESETS_VERSION/$CODESETS_ARCHIVE

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

# PART 3: Download Sources
echo "3. Download Sources"
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
echo "   * clib2-$CLIB2_VERSION"
wget -nc $CLIB2_DOWNLOAD -a $LOGFILES/part3.log 

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
cd $SOURCES

# Part 6: Compile BinUtils
echo "6. Compile BinUtils"
echo "   * Patch Binutils files"
for p in `ls $WORKSPACE/_install/recipes/patches/binutils/*.p`; do patch -d $WORKSPACE/_sources/$BINUTILS_VERSION <$p -p0 >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log; done 
echo "   * Configure"
mkdir -p build-binutils
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
make $CPU install >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cd $SOURCES

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

# Part 8: Compile GCC (Phase #1)
echo "8. Compile GCC (Phase #1)"
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
    >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log 
echo "   * Build (single CPU only)"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" make -j1 all-gcc >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log
echo "   * Install (single CPU only)"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" make -j1 install-gcc >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log

cd $SOURCES

# PART 9: Download Amiga OS NDK's
echo "9. Download AmigaOS NDK's"
mkdir NDK3.2
cd NDK3.2
echo "   * Download NDK3.2.lha from AmiNet" 
wget -nc $NDK32_DOWNLOAD -a $LOGFILES/part9.log
echo "   * Extracting Archive" 
lha -xw=$PREFIX/include/NDK3.2 NDK3.2.lha >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log; done
cd $SOURCES
mkdir NDK3.9
cd NDK3.9
echo "   * Download NDK3.9 from os.amigaworld.de" 
wget -nc $NDK39_DOWNLOAD -a $LOGFILES/part9.log
mv download.php?id=3 NDK39.lha
echo "   * Extracting Archive" 
lha -xw=$PREFIX/include NDK39.lha >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log; done
mv $PREFIX/include/NDK_3.9 $PREFIX/include/NDK3.9
rm -r $PREFIX/include/ndk_3.9
rm $PREFIX/include/NDK_3.9.info
cd $SOURCES
mkdir -p $PREFIX/$TARGET
echo "   * $NDK_ARCHIVE" 
wget -nc $NDK_DOWNLOAD -a $LOGFILES/part9.log 
tar -C $PREFIX/$TARGET --strip-components=2 -xjf $NDK_ARCHIVE
echo "   * Patch $NDK_ARCHIVE"
for p in `ls $WORKSPACE/_install/recipes/patches/ndk/*.p`; do patch -d $PREFIX/$TARGET <$p -p0 >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log; done 
echo "   * Add custom NDK files"
cp -r $WORKSPACE/_install/recipes/files/ndk/* $PREFIX/$TARGET >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log
echo "   * $OPENURL_ARCHIVE"
wget -nc $OPENURL_DOWNLOAD -a $LOGFILES/part9.log 
lha -xw=OpenURL $OPENURL_ARCHIVE >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log; done
cp -r OpenURL/libopenurl-$OPENURL_VERSION/include/* $PREFIX/$TARGET/sys-include/
echo "   * $AMISSL_ARCHIVE"
wget -nc $AMISSL_DOWNLOAD -a $LOGFILES/part9.log 
lha xw=AmiSSL $AMISSL_ARCHIVE >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log; done
cp -r AmiSSL/AmiSSL/Developer/include/* $PREFIX/$TARGET/sys-include/
cp -r AmiSSL/AmiSSL/Developer/lib/AmigaOS3/* $PREFIX/$TARGET/lib/
echo "   * $GUIGFX_ARCHIVE"
wget -nc $GUIGFX_DOWNLOAD -a $LOGFILES/part9.log 
lha xw=guigfxlib $GUIGFX_ARCHIVE >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log; done
cp -r guigfxlib/include/* $PREFIX/$TARGET/sys-include/
echo "   * $RENDER_ARCHIVE"
wget -nc $RENDER_DOWNLOAD -a $LOGFILES/part9.log 
lha xw=renderlib $RENDER_ARCHIVE >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log; done
cp -r renderlib/renderlib/include/* $PREFIX/$TARGET/sys-include/
echo "   * $CODESETS_ARCHIVE"
wget -nc $CODESETS_DOWNLOAD -a $LOGFILES/part9.log 
lha xw=codesets $CODESETS_ARCHIVE >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log; done
cp -r codesets/codesets/Developer/include/* $PREFIX/$TARGET/sys-include/
cd $SOURCES

# PART 10: Amiga Libs/Includes
echo "10. Amiga Libs"
echo "   * Configure"
cd build-gcc
mkdir -p clib2
cp -r $WORKSPACE/_sources/clib2-1_214/library/* $WORKSPACE/_sources/build-gcc/clib2
echo "   * Patch clib2 files"
for p in `ls $WORKSPACE/_install/recipes/patches/clib2/*.p`; do patch -d $WORKSPACE/_sources/build-gcc/clib2 <$p -p0 >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log; done 
echo "   * Add clib2 custom files"
cp -r $WORKSPACE/_install/recipes/files/clib2/* $WORKSPACE/_sources/build-gcc/clib2 >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
cd clib2
echo "   * Build ($CPU)"
PATH="$PREFIX/bin:$PATH" make -f GNUmakefile.68k >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
cp -r $WORKSPACE/_sources/build-gcc/clib2/include $PREFIX/$TARGET >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
cp -r $WORKSPACE/_sources/build-gcc/clib2/lib $PREFIX/$TARGET >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
ln -sf $PREFIX/$TARGET/lib/ncrt0.o $PREFIX/$TARGET/lib/crt0.o >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
cd $SOURCES

# Part 11: Compile GCC (Phase #2)
echo "11. Compile GCC (Phase #2)"
cd build-gcc
echo "   * Build (single CPU only)"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" make -j1 all-gcc >>$LOGFILES/part8.log 2>$LOGFILES/part8_err.log
echo "   * Install (single CPU only)"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" make -j1 install-gcc >>$LOGFILES/part8.log 2>$LOGFILES/part8_err.log
cd $SOURCES

# PART 12: Cleanup
echo "12. Cleanup"
cd $PREFIX
rm -rf info
rm -rf man
rm -rf $TARGET/include

# FINISH
echo " "
echo "FINISHED"
echo " "
exit


# SCRATCH

# PART 10: Amiga Libs/Includes
echo "10. Amiga Libs"
echo "   * Clone clib2"
git clone --progress $CLIB2_DOWNLOAD >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
echo "   * Configure"
cd build-gcc
mkdir -p clib2
cp -r $WORKSPACE/_sources/clib2/library/* $WORKSPACE/_sources/build-gcc/clib2
echo "   * Patch clib2 files"
for p in `ls $WORKSPACE/_install/recipes/patches/clib2/*.p`; do patch -d $WORKSPACE/_sources/build-gcc/clib2 <$p -p0 >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log; done 
cd clib2
echo "   * Build ($CPU)"
PATH="$PREFIX/bin:$PATH" make -f GNUmakefile.68k >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
cp -r $WORKSPACE/_sources/build-gcc/clib2/include $PREFIX/$TARGET >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
cp -r $WORKSPACE/_sources/build-gcc/clib2/lib $PREFIX/$TARGET >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
ln -sf $PREFIX/$TARGET/lib/ncrt0.o $PREFIX/$TARGET/lib/crt0.o >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
cd $SOURCES