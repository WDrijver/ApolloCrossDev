APOLLOCROSSDEV=VBCC
APOLLODEVPAC=ApolloDevPac

C_COMPILER=$(APOLLOCROSSDEV)/bin/vc
S_COMPILER=$(APOLLOCROSSDEV)/bin/vasmm68k_mot
O_LINKER=$(APOLLOCROSSDEV)/bin/vlink

S_INCLUDE=$(APOLLODEVPAC)/Include
C_INCLUDE= $(APOLLOCROSSDEV)/bin/targets/m68k-amigaos/include
C_INCLUDE_NDK32=$(APOLLOCROSSDEV)/ndk/NDK3.2/Include_H
C_INCLUDE_NDK39=$(APOLLOCROSSDEV)/ndk/NDK3.9/Include/include_h

C_LIBS= $(APOLLOCROSSDEV)/bin/targets/m68k-amigaos/lib
C_LIBS_NDK32=$(APOLLOCROSSDEV)/ndk/NDK3.2/lib
C_LIBS_NDK39=$(APOLLOCROSSDEV)/ndk/NDK3.9/Include/linker_libs

C_FLAGS= +aos68k -I$(C_INCLUDE) -I$(C_INCLUDE_NDK39) -c -o
O_FLAGS= -bamigahunk -x -Bstatic -Cvbcc -nostdlib -mrel $(C_LIBS)/startup.o -L$(C_LIBS) -L$(C_LIBS_NDK39) -lvc -o 
S_FLAGS= -quiet -m68080 -Fhunk -linedebug -o

C_SOURCEDIR=C-Source
S_SOURCEDIR=S-Source

C_OBJECTDIR=C-Build
S_OBJECTDIR=S-Build

C_FILES=$(wildcard $(C_SOURCEDIR)/*.c)
S_FILES=$(wildcard $(S_SOURCEDIR)/*.s)
O_FILES=$(patsubst $(C_SOURCEDIR)/%.c,$(C_OBJECTDIR)/%.o,$(C_FILES)) $(patsubst $(S_SOURCEDIR)/%.s,$(S_OBJECTDIR)/%.o,$(S_FILES))

EXE=Output

all: clear clean build

build: $(EXE)

$(EXE) : $(O_FILES) 
	$(O_LINKER) $(O_FLAGS) $(EXE) $(O_FILES)

$(C_OBJECTDIR)/%.o : $(C_SOURCEDIR)/%.c
	$(C_COMPILER) $(C_FLAGS) $@ $<

$(S_OBJECTDIR)/%.o : $(S_SOURCEDIR)/%.s
	$(S_COMPILER) $(S_FLAGS) $@ $<

clean:
	rm -f $(C_OBJECTDIR)/*.o 
	rm -f $(S_OBJECTDIR)/*.o
	rm -f $(EXE)

.phony: clear

clear:
	clear
	########################################
	#  ApolloCrossDev VBCC Edition v0.2    #
	########################################
	# C-Sources: $(C_FILES)
	# S-Sources: $(S_FILES)
	########################################

