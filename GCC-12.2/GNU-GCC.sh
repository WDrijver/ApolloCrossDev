# ApolloCrossDev Build Script v0.1

EDITION=GNU-GCC
VERSION=0.1
CPU=-j16
SOURCES=_sources
LOGS=_logs
APOLLOCROSSDEV=ApolloCrossDev
TARGET=m68k-amiga-elf

BINUTILS_VERSION=binutils-2.40
GCC_VERSION=gcc-12.2.0
GLIBC_VERSION=glibc-2.37
MPFR_VERSION=mpfr-4.2.0
GMP_VERSION=gmp-6.2.1
MPC_VERSION=mpc-1.3.1
ISL_VERSION=isl-0.24
CLOOG_VERSION=cloog-0.18.1

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
echo "2. Update Linux Packages"
apt -y update >>$LOGFILES/part2.log 2>/dev/null
apt -y install build-essential gawk flex bison expect dejagnu texinfo >>$LOGFILES/part2.log 2>/dev/null

# PART 3: Download GNU-Sources
echo "3. Download GNU-Sources"
echo "   * $BINUTILS_VERSION" 
wget -nc https://ftp.gnu.org/gnu/binutils/$BINUTILS_VERSION.tar.gz -a $LOGFILES/part3.log
echo "   * $GCC_VERSION" 
wget -nc https://ftp.gnu.org/gnu/gcc/$GCC_VERSION/$GCC_VERSION.tar.gz -a $LOGFILES/part3.log
echo "   * $GLIBC_VERSION" 
wget -nc https://ftp.gnu.org/gnu/glibc/$GLIBC_VERSION.tar.xz -a $LOGFILES/part3.log
echo "   * $MPFR_VERSION" 
wget -nc https://ftp.gnu.org/gnu/mpfr/$MPFR_VERSION.tar.xz -a $LOGFILES/part3.log
echo "   * $GMP_VERSION" 
wget -nc https://ftp.gnu.org/gnu/gmp/$GMP_VERSION.tar.xz -a $LOGFILES/part3.log
echo "   * $MPC_VERSION" 
wget -nc https://ftp.gnu.org/gnu/mpc/$MPC_VERSION.tar.gz -a $LOGFILES/part3.log
echo "   * $ISL_VERSION" 
wget -nc ftp://gcc.gnu.org/pub/gcc/infrastructure/$ISL_VERSION.tar.bz2 -a $LOGFILES/part3.log
echo "   * $CLOOG_VERSION" 
wget -nc ftp://gcc.gnu.org/pub/gcc/infrastructure/$CLOOG_VERSION.tar.gz -a $LOGFILES/part3.log

# PART 4: Unpack GNU-Sources
echo "4. Unpack GNU-Sources"
for f in *.tar*; do tar xfk $f >>$LOGFILES/part4.log; done 

# Part 5: Compile BinUtils
echo "5. Compile BinUtils"
mkdir -p build-binutils
echo "   * Configure"
cd build-binutils
../$BINUTILS_VERSION/configure \
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
    --prefix="$PREFIX" \
    --target=$TARGET \
    --with-cpu=68000 >>$LOGFILES/part6.log 2>/dev/null
echo "   * Build ($CPU)"
make $CPU all-gcc >>$LOGFILES/part6.log 2>/dev/null
echo "   * Install ($CPU)"
make $CPU install-strip-gcc >>$LOGFILES/part6.log 2>/dev/null
cd ..

# PART X: Compile GLibC
#echo "X. Compile GLibC"
#mkdir -p build-glibc
#cd build-glibc
#../$GLIBC_VERSION/configure \
#    --prefix="$PREFIX/$TARGET" \
#    --build=$MACHTYPE \
#    --host=$TARGET \
#    --target=$TARGET \
#    --disable-multilib \
#    libc_cv_forced_unwind=yes
#make install-bootstrap-headers=yes install-headers
#make $CPU csu/subdir_lib
#install csu/crt1.o csu/crti.o csu/crtn.o $PREFIX/$TARGET/lib
#$PREFIX/$TARGET-gcc -nostdlib -nostartfiles -shared -x c /dev/null -o $PREFIX/$TARGET/lib/libc.so
#touch $PREFIX/$TARGET/include/gnu/stubs.h
#cd ..     

# PART 7: Compile ELF2Hunk
echo "7. Compile ELF2Hunk"
echo "   * Cloning from GitHub (BartmanAbyss)"
git clone --progress --depth=1 https://github.com/BartmanAbyss/elf2hunk.git 2>>$LOGFILES/part7.log
echo "   * Build ($CPU)"
cd elf2hunk
make $CPU >>$LOGFILES/part7.log 2>/dev/null
cp elf2hunk $PREFIX/bin
cd ..

# PART 8: Download Amiga OS NDK's
echo "8. Download AmigaOS NDK's"
echo "   * Installing LHA" 
apt -y install lhasa >>$LOGFILES/part8.log 2>/dev/null
mkdir NDK3.2
cd NDK3.2
echo "   * Download NDK3.2.lha from AmiNet" 
wget -nc http://aminet.net/dev/misc/NDK3.2.lha -a $LOGFILES/part8.log
echo "   * Extracting Archive" 
lha -xw=$PREFIX/include/NDK3.2 NDK3.2.lha >>$LOGFILES/part8.log
echo "   * Adding default GCC Includes"
cp $PREFIX/lib/gcc/$TARGET/12.2.0/include/*.* $PREFIX/include/NDK3.2/Include_H >>$LOGFILES/part8.log
rm $PREFIX/include/NDK3.2/Include_H/stdint.h
mv $PREFIX/include/NDK3.2/Include_H/stdint-gcc.h $PREFIX/include/NDK3.2/Include_H/stdint.h
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
#echo "   * Adding default GCC Includes"
#cp $PREFIX/lib/gcc/$TARGET/12.2.0/include/*.* $PREFIX/include/NDK3.9/Include >>$LOGFILES/part8.log
#rm $PREFIX/include/NDK3.9/Include/stdint.h
#mv $PREFIX/include/NDK3.9/Include/stdint-gcc.h $PREFIX/include/NDK3.9/Include/stdint.h
cd ..

# FINISH
echo " "
echo "FINISHED"
echo " "
exit
