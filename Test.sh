# ApolloCrossDev Primary Install Script v1.1 - MacOS

#Clear Terminal
printf '\33c\e[3J'

VERSION=1.1
CPU=-j16

WORKSPACE="`pwd`"
COMPILERS=Compilers
PROJECTS=Projects
COMPILER=GCC-6.50-Stable
TARGET=m68k-amigaos
PREFIX=$WORKSPACE/$COMPILERS/$COMPILER

MASTER=https://github.com/WDrijver/amiga-gcc
BRANCH=amiga-gcc-stable

ARCHIVES=$WORKSPACE/$COMPILERS/_archives
LOGFILES=$PREFIX/_logs
BUILDS=$PREFIX/_builds
SOURCES=$PREFIX/_sources

export PATH=$PREFIX/bin:$PATH

sudo xcodebuild -license accept
export PATH=$(brew --prefix bison)/bin:$PATH

# ApolloCrossDev Secondary Install Script v1.1 - MacOS

# INIT Terminal
clear
echo -e "\033[1m\033[37m########## \033[31mApollo\033[1;30mCrossDev \033[36m$EDITION\033[30m v$VERSION \033[37m ###########\033[0m\033[36m"
echo -e "\033[1m\033[37m#"
echo -e "\033[1m\033[37m# \033[0mBuilding with CPU=$CPU | If Build fails set CPU=-j1\033[0m\033[36m"
echo " "
echo -e "\033[1m\033[37m0. Sudo Password\033[0m"


# Part 8: ApolloExplorer
echo -e "\033[1m\033[37m8. ApolloExplorer (acp)\033[0m\033[36m"
cd $WORKSPACE/$PROJECTS/ApolloExplorer/acp
qmake >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log
if [ $? -ne 0 ]; then
    echo -e "\033[1m\033[31mQt qmake command not found\033[0;30m"
    printf 'Do you want to install Qt 6 from homebrew? (y/n):'
    read answer
    if [ "$answer" != "${answer#[Yy]}" ] ;then 
        echo -e "Installing Qt 6 from homebrew (be patient)\033[0;30m"
        brew install -qy qt@6 >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log
    else 
        echo -e "Please install Qt 6 (homebrew, Qt online installer or build from source)"
        echo -e "Make sure qmake is in your \$PATH variable\033[0m"
        exit
    fi
fi
qmake MACOSX_DEPLOYMENT_TARGET="12.7.6" >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log
make -j16 >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log
grep -i "error" $LOGFILES/part8.log
if [ $? -eq 0 ]; then
    echo -e "\033[1m\033[31mError(s) found, check $LOGFILES/part8.log\033[0m"
    exit
fi
make clean >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log
