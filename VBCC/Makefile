
PWD=$(shell pwd)
BIN=$(PWD)/bin
CFG=$(PWD)/bin/config
NDK=$(PWD)/ndk

C_COMPILER=$(BIN)/vc
S_COMPILER=$(BIN)/vasmm68k_mot
O_LINKER=$(BIN)/vlink

C_INCLUDE_VBCC=$(BIN)/targets/m68k-amigaos/include
C_INCLUDE_NDK32=$(NDK)/NDK3.2/Include_H
C_INCLUDE_NDK39=$(NDK)/NDK3.9/Include/include_h
S_INCLUDE_DEVPAC=$(NDK)/DevPac

C_LIBS_VBCC=$(BIN)/targets/m68k-amigaos/lib
C_LIBS_NDK32=$(NDK)/NDK3.2/lib
C_LIBS_NDK39=$(NDK)/NDK3.9/Include/linker_libs

C_FLAGS= +$(CFG)/aos68k -I$(C_INCLUDE_VBCC) -I$(C_INCLUDE_NDK39) -c -o
O_FLAGS= -bamigahunk -x -Bstatic -Cvbcc -nostdlib -mrel $(C_LIBS_VBCC)/startup.o -L$(C_LIBS_VBCC) -L$(C_LIBS_NDK39) -lvc -o 
S_FLAGS= -quiet -m68080 -Fhunk -linedebug -o

C_SOURCEDIR=$(PWD)/C-Source
S_SOURCEDIR=$(PWD)/S-Source

C_OBJECTDIR=$(PWD)/C-Build
S_OBJECTDIR=$(PWD)/S-Build

C_FILES=$(wildcard $(C_SOURCEDIR)/*.c)
S_FILES=$(wildcard $(S_SOURCEDIR)/*.s)
O_FILES=$(patsubst $(C_SOURCEDIR)/%.c,$(C_OBJECTDIR)/%.o,$(C_FILES)) $(patsubst $(S_SOURCEDIR)/%.s,$(S_OBJECTDIR)/%.o,$(S_FILES))

EXE=Output

all: init clean build

build: $(EXE)

$(EXE) : $(O_FILES) 
	$(O_LINKER) $(O_FLAGS) $(EXE) $(O_FILES)

$(C_OBJECTDIR)/%.o : $(C_SOURCEDIR)/%.c
	$(C_COMPILER) $(C_FLAGS) $@ $<

$(S_OBJECTDIR)/%.o : $(S_SOURCEDIR)/%.s
	$(S_COMPILER) $(S_FLAGS) $@ $<

clean:
	rm -f $(C_OBJECTDIR)/*.o $(S_OBJECTDIR)/*.o $(EXE)
	mkdir -p $(C_OBJECTDIR) $(S_OBJECTDIR)

.phony: init

init:
	clear
	########################################
	#  ApolloCrossDev VBCC Edition v0.2    #
	########################################
	# C-Sources: $(C_FILES)
	# S-Sources: $(S_FILES)
	########################################
	# Directory: $(PWD)
	# Compiler : $(BIN)
	# NDK-Files: $(NDK)


