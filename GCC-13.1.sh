#!/bin/bash
# ApolloCrossDev Primary Install Script v1.2

VERSION=1.2
CPU=-j16

WORKSPACE="`pwd`"
COMPILERS=Compilers
PROJECTS=Projects
COMPILER=GCC-13.1
TARGET=m68k-amigaos
PREFIX=$WORKSPACE/$COMPILERS/$COMPILER

MASTER=https://github.com/WDrijver/amiga-gcc
BRANCH=amiga-gcc-13.1

ARCHIVES=$WORKSPACE/$COMPILERS/_archives
LOGFILES=$PREFIX/_logs
BUILDS=$PREFIX/_builds
SOURCES=$PREFIX/_sources

export PATH=$PREFIX/bin:$PATH

source GCC-Install.sh

