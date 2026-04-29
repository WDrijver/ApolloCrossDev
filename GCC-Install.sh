# ApolloCrossDev Secondary Install Script v1.0

set -e

patch_amiga_gcc_sources() {
    # Fix fd2sfd: 'false' is a reserved keyword in C23 (GCC 14+).
    sed -i 's/typedef enum { false, nodef, real_error } Error;/typedef enum { no_error, nodef, real_error } Error;/' \
        "$SOURCES/amiga-gcc/projects/fd2sfd/fd2inline.c"
    sed -i 's/\bfalse\b/no_error/g' \
        "$SOURCES/amiga-gcc/projects/fd2sfd/fd2inline.c"

    # Fix readline signal handler prototypes for newer host compilers.
    sed -E -i 's/^([[:space:]]*static[[:space:]]+RETSIGTYPE[[:space:]]+rl_signal_handler)[[:space:]]*\(\);/\1 (int);/' \
        "$SOURCES/amiga-gcc/projects/binutils/readline/readline/signals.c"
    sed -E -i 's/^([[:space:]]*static[[:space:]]+RETSIGTYPE[[:space:]]+rl_sigwinch_handler)[[:space:]]*\(\);/\1 (int);/' \
        "$SOURCES/amiga-gcc/projects/binutils/readline/readline/signals.c"
    sed -E -i 's/^typedef[[:space:]]+RETSIGTYPE[[:space:]]+SigHandler[[:space:]]*\(\);/typedef RETSIGTYPE SigHandler (int);/' \
        "$SOURCES/amiga-gcc/projects/binutils/readline/readline/signals.c"
    sed -E -i 's/^#  define SIGHANDLER_RETURN return \(0\)/#  define SIGHANDLER_RETURN return/' \
        "$SOURCES/amiga-gcc/projects/binutils/readline/readline/signals.c"

    # Work around GCC 15 ICE while compiling libstdc++ cxx11-shim_facets.
    if ! grep -q "ApolloCrossDev workaround: avoid host GCC 15 ICE" "$SOURCES/amiga-gcc/projects/gcc/libstdc++-v3/src/c++11/cxx11-shim_facets.cc"; then
        sed -i '/in code that uses the other std::string ABI from the replacing code\./a\
// ApolloCrossDev workaround: avoid host GCC 15 ICE when compiling this TU.\
#pragma GCC push_options\
#pragma GCC optimize ("O0")' \
            "$SOURCES/amiga-gcc/projects/gcc/libstdc++-v3/src/c++11/cxx11-shim_facets.cc"
    fi
}

patch_timidity_sources() {
    # Keep clean builds warning-free: align stream pointer type with call sites.
    sed -i 's/static void compute_data(MidSong \*song, sint8 \*\*stream, sint32 count)/static void compute_data(MidSong *song, unsigned char **stream, sint32 count)/' \
        "$SOURCES/timidity-source/playmidi.c"
}

patch_bgdbserver_sources() {
    # Fix bgdbserver warnings with newer host compilers and Amiga varargs macros.
    sed -i 's/void addBreakpoint(UWORD \* addr, short isTemp, UWORD \* restore);/void addBreakpoint(volatile UWORD * addr, short isTemp, volatile UWORD * restore);/' src/breakpoint.h
    sed -i 's/void delBreakpoint(UWORD \* addr);/void delBreakpoint(volatile UWORD * addr);/' src/breakpoint.h
    if ! grep -q "#include <stdlib.h>" src/breakpoint.c; then
        sed -i '/#include "breakpoint.h"/a #include <stdlib.h>' src/breakpoint.c
    fi
    sed -i 's/UWORD \* address;/volatile UWORD * address;/' src/breakpoint.c
    sed -i 's/UWORD \* restore;/volatile UWORD * restore;/' src/breakpoint.c
    sed -i 's/static short findBp(UWORD \* addr)/static short findBp(volatile UWORD * addr)/' src/breakpoint.c
    sed -i 's/void addBreakpoint(UWORD \* addr, short isTmp, UWORD \* restore)/void addBreakpoint(volatile UWORD * addr, short isTmp, volatile UWORD * restore)/' src/breakpoint.c
    sed -i 's/void delBreakpoint(UWORD \* addr)/void delBreakpoint(volatile UWORD * addr)/' src/breakpoint.c
    sed -i 's/Printf("break at 0x%08lx\\n", pc);/Printf("break at 0x%08lx\\n", (ULONG)pc);/' src/main.c
    sed -i 's/Printf("can'"'"'t break at 0x%08lx - %02lx\\n", pc, chk);/Printf("can'"'"'t break at 0x%08lx - %02lx\\n", (ULONG)pc, (ULONG)chk);/' src/main.c
    sed -i 's/Printf("unknown packet %s\\n", cmd);/Printf("unknown packet %s\\n", (ULONG)cmd);/' src/main.c
    sed -i 's/Printf("Running GDB server on port %s\\n", p + 1);/Printf("Running GDB server on port %s\\n", (ULONG)(p + 1));/' src/main.c
    sed -i 's/Printf("Starting RSH server on port %s\\n", sport + 1);/Printf("Starting RSH server on port %s\\n", (ULONG)(sport + 1));/' src/main.c
}

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
# Ensure cross-tool binaries are always resolvable in subsequent make steps.
export PATH="$PREFIX/bin:$PATH"
cd $SOURCES

# PART 2: Update Linux Packages 
echo -e "\e[1m\e[37m2. Update Linux Packages\e[0m\e[36m"
sudo apt -y update >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log
sudo apt -y install make wget git gcc g++ lhasa libgmp-dev libmpfr-dev libmpc-dev flex bison gettext texinfo ncurses-dev autoconf rsync libreadline-dev >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log
sudo apt -y install build-essential devscripts debhelper qtbase5-dev qtbase5-dev-tools qt5-qmake libqt5x11extras5-dev qttools5-dev-tools >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log

# PART 3: Clone Amiga-GCC
echo -e "\e[1m\e[37m3. Clone Amiga-GCC (Stefan -Bebbo- Franke)\e[0m\e[36m"
if [ -d "$SOURCES/amiga-gcc/.git" ]; then
    cd "$SOURCES/amiga-gcc"
    git fetch --all --prune >>$LOGFILES/part3.log 2>>$LOGFILES/part3_err.log
    git checkout "$BRANCH" >>$LOGFILES/part3.log 2>>$LOGFILES/part3_err.log
    git reset --hard "origin/$BRANCH" >>$LOGFILES/part3.log 2>>$LOGFILES/part3_err.log
    cd "$SOURCES"
else
    git clone --progress -b "$BRANCH" "$MASTER" >>$LOGFILES/part3.log 2>>$LOGFILES/part3_err.log
fi

# Part 4: Compile Amiga-GCC
echo -e -n "\e[1m\e[37m4. Compile Amiga-GCC: \e[0m\e[36m"
cd $SOURCES/amiga-gcc
echo -e -n "\e[0m\e[36mMake Clean | "
make clean >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
echo -e -n "\e[0m\e[36mDrop Prefix | "
make drop-prefix PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
echo -e -n "\e[0m\e[36mClone Repos (>1 min) | "
make update $CPU NDK=3.2 PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
patch_amiga_gcc_sources
echo -e -n "\e[0m\e[36mBuild Compiler (>5 min) | "
CFLAGS="-DVOID_SIGHANDLER -U_GLIBCXX_ASSERTIONS -Wno-error=incompatible-pointer-types" \
CXXFLAGS="-DVOID_SIGHANDLER -U_GLIBCXX_ASSERTIONS -Wno-error=incompatible-pointer-types" \
CPPFLAGS="-DVOID_SIGHANDLER" \
make all -j1 NDK=3.2 PREFIX=$PREFIX \
    CFLAGS_FOR_BUILD="-std=gnu17 -DVOID_SIGHANDLER -U_GLIBCXX_ASSERTIONS -Wno-error=incompatible-pointer-types" \
    HOST_CFLAGS="-std=gnu17 -DVOID_SIGHANDLER -U_GLIBCXX_ASSERTIONS -Wno-error=incompatible-pointer-types" \
    HOST_CXXFLAGS="-std=gnu++17 -DVOID_SIGHANDLER -U_GLIBCXX_ASSERTIONS -Wno-error=incompatible-pointer-types" \
    STAGE1_CHECKING="--disable-checking" >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# GCC 15 host can hit an ICE in libstdc++-v3/src/c++11/cxx11-shim_facets.cc; retry once with lower target C++ optimization.
if grep -q "cxx11-shim_facets.cc:.*internal compiler error" "$LOGFILES/part4.log"; then
    echo "retry make all with reduced target C++ optimization after ICE in cxx11-shim_facets.cc" >>$LOGFILES/part4.log
    CFLAGS="-DVOID_SIGHANDLER -U_GLIBCXX_ASSERTIONS -Wno-error=incompatible-pointer-types" \
    CXXFLAGS="-DVOID_SIGHANDLER -U_GLIBCXX_ASSERTIONS -Wno-error=incompatible-pointer-types" \
    CPPFLAGS="-DVOID_SIGHANDLER" \
    make all -j1 NDK=3.2 PREFIX=$PREFIX \
        CFLAGS_FOR_BUILD="-std=gnu17 -DVOID_SIGHANDLER -U_GLIBCXX_ASSERTIONS -Wno-error=incompatible-pointer-types" \
        HOST_CFLAGS="-std=gnu17 -DVOID_SIGHANDLER -U_GLIBCXX_ASSERTIONS -Wno-error=incompatible-pointer-types" \
        HOST_CXXFLAGS="-std=gnu++17 -DVOID_SIGHANDLER -U_GLIBCXX_ASSERTIONS -Wno-error=incompatible-pointer-types" \
        CXXFLAGS_FOR_TARGET="-O0 -fomit-frame-pointer" \
        STAGE1_CHECKING="--disable-checking" >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
fi
echo -e "\e[0m\e[36mAdd LibDebug\e[0m]"
make libdebug PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# Part 5: MUI
echo -e "\e[1m\e[37m5. Adding MUI5\e[0m\e[36m"
cd $SOURCES/amiga-gcc
make sdk=mui PREFIX=$PREFIX >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
mkdir -p $PREFIX/$TARGET/include/mui
cp -r -f $SOURCES/amiga-gcc/build/mui/SDK/MUI/C/include/mui/* $PREFIX/$TARGET/include/mui >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log

# Part 6: PortLibs (amiga-gcc takes care of Open-GL, SDL and GDB - we add Freetype, ZLib and BZip2)
echo -e -n "\e[1m\e[37m6. Adding Porting Libs: "
cd $PREFIX
mkdir -p $PREFIX/$TARGET/include $PREFIX/$TARGET/lib $PREFIX/$TARGET/include/clib $PREFIX/$TARGET/include/libraries
echo -e -n "\e[0m\e[36mGL | "
if [ -d $PREFIX/include/GL ]; then cp -r -f $PREFIX/include/GL $PREFIX/$TARGET/include/GL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log; fi
echo -e -n "\e[0m\e[36mGDB | "
if [ -d $PREFIX/include/gdb ]; then cp -r -f $PREFIX/include/gdb $PREFIX/$TARGET/include/GDB >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log; fi
echo -e -n "\e[0m\e[36mSDL | "
if [ -d $PREFIX/include/SDL ]; then cp -r -f $PREFIX/include/SDL $PREFIX/$TARGET/include/SDL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log; fi
if ls $PREFIX/lib/libSDL* >/dev/null 2>&1; then cp -r -f $PREFIX/lib/libSDL* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log; fi
if ls $PREFIX/lib/libSDL* >/dev/null 2>&1; then rm -r -f $PREFIX/lib/libSDL* >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log; fi
if [ -d $PREFIX/include ]; then rm -r -f $PREFIX/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log; fi

echo -e -n "\e[0m\e[36mSDL-TTF | "
cd $ARCHIVES/SDL-TTF
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mSDL-Mixer | "
cd $ARCHIVES/SDL-Mixer
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mSDL-Images | "
cd $ARCHIVES/SDL-Images
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mFreeType2 | "
cd $ARCHIVES/freetype2
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mVorbis | "
cd $ARCHIVES/vorbis
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mMPEGA (Dynamic) | "
cd $ARCHIVES/MPEGA-source/include
cp -r -f clib/* $PREFIX/$TARGET/include/clib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libraries/* $PREFIX/$TARGET/include/libraries >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
$PREFIX/bin/fd2pragma -i fd/mpega.fd -c clib/mpega_protos.h -s 38 -t $PREFIX/$TARGET/include/proto >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
$PREFIX/bin/fd2pragma -i fd/mpega.fd -c clib/mpega_protos.h -s 40 -t $PREFIX/$TARGET/include/inline >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mZLib | "
cp -r -f $ARCHIVES/zlib-source $SOURCES/zlib-source
cd $SOURCES/zlib-source
CC=$PREFIX/bin/m68k-amigaos-gcc \
AR=$PREFIX/bin/m68k-amigaos-ar \
RANLIB=$PREFIX/bin/m68k-amigaos-ranlib \
CFLAGS="-noixemul -m68040 -O2 -ffast-math -fomit-frame-pointer" \
./configure >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make all $CPU >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libz.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
mkdir -p $PREFIX/$TARGET/include/zlib
cp -r zlib.h $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r zconf.h $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e "\e[0m\e[36mTimidity\e[0m"
mkdir -p $SOURCES/timidity-source
cp -r -f $ARCHIVES/timidity-source/src/* $SOURCES/timidity-source
patch_timidity_sources
cd $SOURCES/timidity-source
make -f Makefile.apollocrossdev >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp libtimidity.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
mkdir -p $PREFIX/$TARGET/include/timidity
cp -f *.h $PREFIX/$TARGET/include/timidity

# PART 7: NDK
echo -e "\e[1m\e[37m7. Development Kits\e[0m\e[36m"
cd $PREFIX/$TARGET
if [ ! -d $PREFIX/$TARGET/DevPac/.git ]; then git clone --progress https://github.com/WDrijver/DevPac >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log; fi
cd $ARCHIVES
cp -r -f P96/autodocs P96/e P96/fd P96/inline P96/lvo_p96.i P96/pragmas P96/proto $PREFIX/$TARGET/include >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
cp -r -f P96/clib/* $PREFIX/$TARGET/include/clib >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
cp -r -f P96/libraries/* $PREFIX/$TARGET/include/libraries >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
cp -r -f cmake/* $PREFIX/lib >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log

# Part 8: ApolloExplorer
echo -e "\e[1m\e[37m8. ApolloExplorer\e[0m\e[36m"
cd $WORKSPACE/$PROJECTS
if [ ! -d $WORKSPACE/$PROJECTS/ApolloExplorer/.git ]; then git clone --progress https://github.com/ronybeck/ApolloExplorer >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log; fi
cd $WORKSPACE/$PROJECTS/ApolloExplorer
qmake >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log
make >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log

# Part 9: BGDBServer
echo -e "\e[1m\e[37m9. BGDG Server\e[0m\e[36m"
cd $WORKSPACE/$PROJECTS
if [ ! -d $WORKSPACE/$PROJECTS/bgdbserver/.git ]; then git clone --progress https://github.com/WDrijver/bgdbserver >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log; fi
cd $WORKSPACE/$PROJECTS/bgdbserver
patch_bgdbserver_sources
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
