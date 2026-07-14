# ApolloCrossDev Primary Install Script v1.1 - MacOS

#Clear Terminal
printf '\33c\e[3J'

VERSION=1.1
CPU=-j16

WORKSPACE="`pwd`"
COMPILERS=Compilers
PROJECTS=Projects
COMPILER=GCC-6.50-Dev
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

source GCC-Install-Dev-MacOS.sh