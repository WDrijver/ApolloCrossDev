# ApolloCrossDev GCC-12.20 Install Script v0.6
# 
# Installation:
# 1. Enter Compilers/GCC-12.20 directory
# 2. Type "./GCC-12.20.sh" and hit ENTER
#
# Instructions:
# 1. Create Projects/<mysource> directory
# 2. Copy Projects/make-gcc1220 into <mysource> 
# 3. Read make-gcc1220 for compile instructions

EDITION=GCC-12.2
VERSION=0.2
CPU=-j16

WORKSPACE="`pwd`"
SOURCES=$WORKSPACE/_sources
LOGFILES=$WORKSPACE/_logs
PREFIX=$WORKSPACE/ApolloCrossDev
TARGET=m68k-amiga-elf
export PATH=$PREFIX/bin:$PATH

BINUTILS_VERSION=binutils-2.40
BINUTILS_DOWNLOAD=https://ftp.gnu.org/gnu/binutils/$BINUTILS_VERSION.tar.gz
GCC_VERSION=gcc-12.2.0
GCC_DOWNLOAD=https://ftp.gnu.org/gnu/gcc/$GCC_VERSION/$GCC_VERSION.tar.gz
GLIBC_VERSION=glibc-2.37
GLIBC_DOWNLOAD=https://ftp.gnu.org/gnu/glibc/$GLIBC_VERSION.tar.xz
MPFR_VERSION=mpfr-4.2.0
MPFR_DOWNLOAD=https://ftp.gnu.org/gnu/mpfr/$MPFR_VERSION.tar.xz
GMP_VERSION=gmp-6.2.1
GMP_DOWNLOAD=https://ftp.gnu.org/gnu/gmp/$GMP_VERSION.tar.xz
MPC_VERSION=mpc-1.3.1
MPC_DOWNLOAD=https://ftp.gnu.org/gnu/mpc/$MPC_VERSION.tar.gz
ISL_VERSION=isl-0.24
ISL_DOWNLOAD=ftp://gcc.gnu.org/pub/gcc/infrastructure/$ISL_VERSION.tar.bz2
CLOOG_VERSION=cloog-0.18.1
CLOOG_DOWNLOAD=ftp://gcc.gnu.org/pub/gcc/infrastructure/$CLOOG_VERSION.tar.gz
ELF2HUNK_DOWNLOAD=https://github.com/BartmanAbyss/elf2hunk.git
NDK32_DOWNLOAD=http://aminet.net/dev/misc/NDK3.2.lha
NDK39_DOWNLOAD=https://os.amigaworld.de/download.php?id=3
NEWLIB_VERSION=newlib-4.2.0
NEWLIB_DOWNLOAD=https://sourceware.org/git/newlib-cygwin.git
LIBNIX_VERSION=libnix-2.1
LIBNIX_DOWNLOAD=https://github.com/bebbo/libnix

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
apt -y install build-essential gawk flex bison expect dejagnu texinfo lhasa >>$LOGFILES/part2.log 2>/dev/null

# PART 3: Download GNU-Sources
echo "3. Download GNU-Sources"
echo "   * $BINUTILS_VERSION" 
wget -nc $BINUTILS_DOWNLOAD -a $LOGFILES/part3.log
echo "   * $GCC_VERSION" 
wget -nc $GCC_DOWNLOAD -a $LOGFILES/part3.log
echo "   * $GLIBC_VERSION" 
wget -nc $GLIBC_DOWNLOAD -a $LOGFILES/part3.log
echo "   * $MPFR_VERSION" 
wget -nc $MPFR_DOWNLOAD -a $LOGFILES/part3.log
echo "   * $GMP_VERSION" 
wget -nc $GMP_DOWNLOAD -a $LOGFILES/part3.log
echo "   * $MPC_VERSION" 
wget -nc $MPC_DOWNLOAD -a $LOGFILES/part3.log
echo "   * $ISL_VERSION" 
wget -nc $ISL_DOWNLOAD -a $LOGFILES/part3.log
echo "   * $CLOOG_VERSION" 
wget -nc $CLOOG_DOWNLOAD -a $LOGFILES/part3.log
echo "   * $NEWLIB_VERSION" 
mkdir -p $NEWLIB_VERSION
cd $NEWLIB_VERSION
git clone --progress $NEWLIB_DOWNLOAD 2>>$LOGFILES/part7.log
cd ..
mkdir -p $LIBNIX_VERSION
cd $LIBNIX_VERSION
git clone --progress $LIBNIX_DOWNLOAD 2>>$LOGFILES/part7.log
cd ..

# PART 4: Unpack GNU-Sources
echo "4. Unpack GNU-Sources"
for f in *.tar*; do tar xfk $f >>$LOGFILES/part4.log; done 

# Part 5: Compile BinUtils
echo "5. Compile BinUtils"
mkdir -p build-binutils
echo "   * Configure"
cd build-binutils
LDFLAGS="-static -static-libgcc -static-libstdc++" ../$BINUTILS_VERSION/configure \
    --disable-interprocess-agent \
    --disable-libcc \
    --disable-shared \
    --disable-werror \
    --without-guile \
    --without-expat \
    --without-zstd \
    --without-lzma \
    --enable-static \
    --prefix="$PREFIX" \
    --target=$TARGET >>$LOGFILES/part5.log 2>/dev/null
echo "   * Build ($CPU)"
make $CPU >>$LOGFILES/part5.log 2>/dev/null
echo "   * Install ($CPU)"
make $CPU install-strip >>$LOGFILES/part5.log 2>/dev/null
cd ..

# Part 6: Compile GCC
echo "6. Compile GCC"
echo "   * Make symbolic links"
cd $GCC_VERSION
ln -sf `ls -1d ../mpfr-*/` mpfr
ln -sf `ls -1d ../gmp-*/` gmp
ln -sf `ls -1d ../mpc-*/` mpc
ln -sf `ls -1d ../isl-*/` isl
ln -sf `ls -1d ../cloog-*/` cloog
cd ..
mkdir -p build-gcc
cd build-gcc
echo "   * Configure"
../$GCC_VERSION/configure \
	--enable-version-specific-runtime-libs \
    --disable-libssp \
    --disable-nls \
    --disable-threads \
    --disable-libmudflap \
    --disable-libgomp  \
	--with-newlib \
    --with-headers=../$NEWLIB_VERSION/newlib-cygwin/newlib/libc/include/ \
    --disable-shared \
	--disable-libquadmath \
    --disable-libatomic \
    --with-cpu=68000 \
    --prefix="$PREFIX" \
    --target=$TARGET >>$LOGFILES/part6.log 2>/dev/null
echo "   * Build ($CPU)"
make $CPU all-gcc >>$LOGFILES/part6.log 2>/dev/null
echo "   * Install ($CPU)"
make $CPU install-gcc >>$LOGFILES/part6.log 2>/dev/null
echo "   * Build libgcc ($CPU)"
make $CPU all-target-libgcc >>$LOGFILES/part6.log 2>/dev/null
echo "   * Install libgcc ($CPU)"
make $CPU install-target-libgcc >>$LOGFILES/part6.log 2>/dev/null
cd ..
exit
# PART X: Compile Newlib
echo "X. Compile Newlib"
mkdir -p build-newlib
cd build-newlib
echo "   * Configure"
../$NEWLIB_VERSION/newlib-cygwin/configure \
    --enable-newlib-io-c99-formats \
    --enable-newlib-reent-small \
    --enable-newlib-mb \
    --disable-shared \
    --enable-static \
    --enable-newlib-multithread \
    --disable-newlib-mb \
	--disable-newlib-atexit-alloc \
    --enable-target-optspace \
    --enable-fast-install \
    --prefix="$PREFIX" \
    --target=$TARGET \
#    >>$LOGFILES/part6.log 2>/dev/null
echo "   * Build ($CPU)"
make $CPU
# >>$LOGFILES/part6.log 2>/dev/null
make $CPU install
# >>$LOGFILES/part6.log 2>/dev/null
cd ..

# PART X: Compile Libnix
echo "X. Compile Libnix"
mkdir -p build-libnix
cd build-libnix
echo "   * Copy include files"
mkdir -p $PREFIX/$TARGET/libnix
mkdir -p $PREFIX/$TARGET/libnix/include
cp -r ../$LIBNIX_VERSION/libnix/sources/headers/* $PREFIX/$TARGET/libnix/include/
echo "   * Build ($CPU)"
make $CPU libnix -f ../$LIBNIX_VERSION/libnix/Makefile.gcc6 root=../$LIBNIX_VERSION/libnix all
# >>$LOGFILES/part6.log 2>/dev/null
make $CPU libnix -f ../$LIBNIX_VERSION/libnix/Makefile.gcc6 root=../$LIBNIX_VERSION/libnix install
# >>$LOGFILES/part6.log 2>/dev/null
cd ..

#	$(L0)"make libnix"$(L1) CFLAGS="$(CFLAGS_FOR_TARGET)" $(MAKE) -C $(BUILD)/libnix -f $(PROJECTS)/libnix/Makefile.gcc6 root=$(PROJECTS)/libnix all $(L2)
#	$(L0)"install libnix"$(L1) $(MAKE) -C $(BUILD)/libnix -f $(PROJECTS)/libnix/Makefile.gcc6 root=$(PROJECTS)/libnix install $(L2)


# PART 7: Compile ELF2Hunk
echo "7. Compile ELF2Hunk"
echo "   * Cloning from GitHub (BartmanAbyss)"
git clone --progress --depth=1 $ELF2HUNK_DOWNLOAD 2>>$LOGFILES/part7.log
echo "   * Build ($CPU)"
cd elf2hunk
make $CPU >>$LOGFILES/part7.log 2>/dev/null
cp elf2hunk $PREFIX/bin
cd ..

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

# FINISH
echo " "
echo "FINISHED"
echo " "
exit



#CLEAN ME UP
echo "   * Adding default GCC Includes"
cp $PREFIX/lib/gcc/$TARGET/12.2.0/include/*.* $PREFIX/include/NDK3.2/Include_H >>$LOGFILES/part8.log
rm $PREFIX/include/NDK3.2/Include_H/stdint.h
mv $PREFIX/include/NDK3.2/Include_H/stdint-gcc.h $PREFIX/include/NDK3.2/Include_H/stdint.h

# PART X: Compile GLibC
echo "X. Compile GLibC"
mkdir -p build-glibc
cd build-glibc
../$GLIBC_VERSION/configure \
    --prefix="$PREFIX/$TARGET" \
    --build=$MACHTYPE \
    --host=$TARGET \
    --target=$TARGET \
    --disable-multilib \
    libc_cv_forced_unwind=yes
make install-bootstrap-headers=yes install-headers
make $CPU csu/subdir_lib
install csu/crt1.o csu/crti.o csu/crtn.o $PREFIX/$TARGET/lib
$PREFIX/$TARGET-gcc -nostdlib -nostartfiles -shared -x c /dev/null -o $PREFIX/$TARGET/lib/libc.so
touch $PREFIX/$TARGET/include/gnu/stubs.h
cd ..  

Build GCC according to Abyss
    --disable-clocale \
    --disable-gcov \
    --disable-libada \
    --disable-libgomp \
    --disable-libsanitizer \
    --disable-libssp \
    --disable-libvtv \
    --disable-multilib \
    --disable-nls \
    --disable-threads \
    --enable-languages=c,c++ \
    --enable-lto \
    --enable-static \
    --with-cpu=68000 \