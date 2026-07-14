# ApolloCrossDev Secondary Install Script v1.1

# INIT Terminal
clear
echo -e "\e[1m\e[37m########## \e[31mApollo\e[1;30mCrossDev \e[36m$COMPILER\e[30m v$VERSION \e[37m ###########\e[0m\e[36m"
echo -e "\e[1m\e[37m#"
echo -e "\e[1m\e[37m# \e[0mBuilding with CPU=$CPU | If Build fails set CPU=-j1\e[0m\e[36m"
echo " "
echo -e "\e[1m\e[37m0. Check root status\e[0m"

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
git clone --progress -b $BRANCH $MASTER 2>>$LOGFILES/part3_err.log

# Part 4: Compile Amiga-GCC
echo -e -n "\e[1m\e[37m4. Compile Amiga-GCC: \e[0m\e[36m"
cd $SOURCES/amiga-gcc
echo -e -n "\e[0m\e[36mMake Clean | "
make clean >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
echo -e -n "\e[0m\e[36mDrop Prefix | "
make drop-prefix PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

echo -e -n "\e[0m\e[36mClone Repos (>1 min)\e[0m\e[36m"
make update $CPU NDK=3.2 PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# Apply Patches from Ioannis Kouretsidis (@JohnStuggi)
echo -e "\e[0m\e[36mApplying Apollo 68080 Patches from Ioannis Kouretsidis (@JohnStuggi)\e[0m"
cd $SOURCES/amiga-gcc/projects/gcc
git apply $ARCHIVES/patches/q4d-candidate.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
git apply $ARCHIVES/patches/q7k-post-q4d-candidate.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# Apply Patches from Morten (@Morten)
echo -e "\e[0m\e[36mApplying Apollo 68080 Patches from Morten (@Morten)"
cd $SOURCES/amiga-gcc/projects/gcc
git apply $ARCHIVES/patches/opt-absolute-volatile-fix.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
git apply $ARCHIVES/patches/opt-pipeline-cc0-fix.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
git apply $ARCHIVES/patches/opt-shift-lsr-signext-fix.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
git apply $ARCHIVES/patches/postinc-size-fix.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

echo -e "\e[0m\e[36mOverview of the Applied Patches: "
git diff --stat 

# Apply Patches for ISL
echo -e "\e[0m\e[36mApplying Patches for ISL"
cd $ARCHIVES/isl
cp -f * $SOURCES/amiga-gcc/projects/gcc/gcc

# Build Compiler
cd $SOURCES/amiga-gcc
echo -e -n "\e[0m\e[36mBuild Compiler (>5 min) | "
make all $CPU NDK=3.2 PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
echo -e "\e[0m\e[36mAdd LibDebug\e[0m]"
make libdebug PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# Part 5: MUI
echo -e "\e[1m\e[37m5. Adding MUI5\e[0m\e[36m"
cd $SOURCES/amiga-gcc
make sdk=mui PREFIX=$PREFIX >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
cp -r -f $SOURCES/amiga-gcc/build/mui/SDK/MUI/C/include/mui/* $PREFIX/$TARGET/include/mui >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log

# Part 6: PortLibs (amiga-gcc takes care of Open-GL, SDL and GDB - we add Freetype, ZLib and BZip2)
echo -e -n "\e[1m\e[37m6. Adding Porting Libs: "
cd $PREFIX

echo -e -n "\e[0m\e[36mGL | "
cp -r -f $PREFIX/include/GL $PREFIX/$TARGET/include/GL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
echo -e -n "\e[0m\e[36mGDB | "
cp -r -f $PREFIX/include/gdb $PREFIX/$TARGET/include/GDB >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mSDL | "
cp -r -f $PREFIX/include/SDL $PREFIX/$TARGET/include/SDL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f $PREFIX/lib/libSDL* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mSDL_mixer | "
cd $ARCHIVES/SDL_mixer
mkdir -p $SOURCES/SDL_mixer
cp -r -f $ARCHIVES/SDL_mixer/* $SOURCES/SDL_mixer
cd $SOURCES/SDL_mixer
PATH=$PREFIX/bin:$PATH \
CC=$PREFIX/bin/m68k-amigaos-gcc \
AR=$PREFIX/bin/m68k-amigaos-ar \
RANLIB=$PREFIX/bin/m68k-amigaos-ranlib \
./configure --host=m68k-amigaos --prefix=$PREFIX/$TARGET >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make CFLAGS="-noixemul -g -O2" LDFLAGS="-noixemul -lSDL" >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f build/.libs/libSDL_mixer.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f *.h $PREFIX/$TARGET/include/SDL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mSDL_net | "
mkdir -p $SOURCES/SDL_net
cp -r -f $ARCHIVES/SDL_net/* $SOURCES/SDL_net
cd $SOURCES/SDL_net
CC=$PREFIX/bin/m68k-amigaos-gcc \
AR=$PREFIX/bin/m68k-amigaos-ar \
RANLIB=$PREFIX/bin/m68k-amigaos-ranlib \
make >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libSDL_net.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r *.h $PREFIX/$TARGET/include/SDL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mSDL_ttf | "
cd $ARCHIVES/SDL_ttf
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mSDL_images | "
cd $ARCHIVES/SDL_images
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mVorbis | "
cd $ARCHIVES/vorbis
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mFreeType2 | "
cd $ARCHIVES/freetype2
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mMPEGA (Dynamic) | "
cd $ARCHIVES/MPEGA-source/include
cp -r -f clib/* $PREFIX/$TARGET/include/clib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libraries/* $PREFIX/$TARGET/include/libraries >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
$PREFIX/bin/fd2pragma -i fd/mpega.fd -c clib/mpega_protos.h -s 38 -t $PREFIX/$TARGET/include/proto >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
$PREFIX/bin/fd2pragma -i fd/mpega.fd -c clib/mpega_protos.h -s 40 -t $PREFIX/$TARGET/include/inline >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mZLib | "
mkdir -p $SOURCES/zlib-source
cp -r -f $ARCHIVES/zlib-source/* $SOURCES/zlib-source
cd $SOURCES/zlib-source
CC=$PREFIX/bin/m68k-amigaos-gcc \
AR=$PREFIX/bin/m68k-amigaos-ar \
RANLIB=$PREFIX/bin/m68k-amigaos-ranlib \
make >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libz.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r zlib.h $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r zconf.h $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mMinizip | "
cd $SOURCES/zlib-source/contrib/minizip
CC=$PREFIX/bin/m68k-amigaos-gcc \
AR=$PREFIX/bin/m68k-amigaos-ar \
RANLIB=$PREFIX/bin/m68k-amigaos-ranlib \
make >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libminizip.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mLua | "
mkdir -p $SOURCES/lua
cp -r -f $ARCHIVES/lua/* $SOURCES/lua
cd $SOURCES/lua
CC=$PREFIX/bin/m68k-amigaos-gcc \
AR=$PREFIX/bin/m68k-amigaos-ar \
RANLIB=$PREFIX/bin/m68k-amigaos-ranlib \
make >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f liblua.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
mkdir -p $PREFIX/$TARGET/include/lua
cp -r *.h $PREFIX/$TARGET/include/lua >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e "\e[0m\e[36mTimidity\e[0m"
mkdir $SOURCES/timidity-source
cp -r -f $ARCHIVES/timidity-source/src/* $SOURCES/timidity-source
cd $SOURCES/timidity-source
make -f Makefile.apollocrossdev >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp libtimidity.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
mkdir -p $PREFIX/$TARGET/include/timidity
cp -f *.h $PREFIX/$TARGET/include/timidity

# PART 7: NDK
echo -e "\e[1m\e[37m7. Development Kits\e[0m\e[36m"
cd $PREFIX/$TARGET
git clone --progress https://github.com/WDrijver/DevPac >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
cd $ARCHIVES
cp -r -f P96/* $PREFIX/$TARGET/include >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
cp -r -f cmake/* $PREFIX/lib >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log

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
git clone --progress https://github.com/WDrijver/bgdbserver >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log
cd $WORKSPACE/$PROJECTS/bgdbserver
make >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log

# PART 10: Cleanup
echo -e "\e[1m\e[37m10. Cleanup\e[0m\e[36m"
rm -rf $SOURCES
cd $PREFIX
rm -rf info
rm -rf man

# FINISH
echo " "
echo -e "\e[1m\e[32mFINISHED\e[0m"
echo " "
exit
