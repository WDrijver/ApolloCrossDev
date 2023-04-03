APOLLO_GCC=ApolloCrossDev
APOLLO_DEVPAC=ApolloDevPac

C_COMPILER=$(APOLLO_GCC)/bin/m68k-amiga-elf-gcc
C_INCLUDE_NDK32=$(APOLLO_GCC)/include/NDK3.2/Include_H
C_INCLUDE_NDK39=$(APOLLO_GCC)/include/NDK3.9/Include/include_h

S_COMPILER=$(APOLLO_GCC)/bin/vasmm68k_mot
S_INCLUDE=$(APOLLO_DEVPAC)/Include

C_FLAGS= -I$(C_INCLUDE_NDK39) -shared-libgcc -o 
S_FLAGS= -Fhunk -m68040 -quiet -I$(S_INCLUDE) -o
O_FLAGS= -g -o

C_SOURCEDIR=C-Source
S_SOURCEDIR=S-Source

C_OBJECTDIR=C-Build
S_OBJECTDIR=S-Build

C_FILES=$(wildcard $(C_SOURCEDIR)/*.c)
S_FILES=$(wildcard $(S_SOURCEDIR)/*.s)
O_FILES=$(patsubst $(C_SOURCEDIR)/%.c,$(C_OBJECTDIR)/%.o,$(C_FILES)) $(patsubst $(S_SOURCEDIR)/%.s,$(S_OBJECTDIR)/%.o,$(S_FILES))

EXE=Output

RM:=rm -f 
PATHSEP:=/

$(info ########################################)
$(info #  Apollo-GCC v1.0                     #)
$(info ########################################)
$(info  )
$(info $(O_FILES))
$(info  )

all: $(EXE)

$(EXE) : $(O_FILES) 
	$(C_COMPILER) $(O_FLAGS) $(EXE) $(O_FILES)

$(C_OBJECTDIR)/%.o : $(C_SOURCEDIR)/%.c
	$(C_COMPILER) $(C_FLAGS) $@ $< 

$(S_OBJECTDIR)/%.o : $(S_SOURCEDIR)/%.s
	$(S_COMPILER) $(S_FLAGS) $@ $<

clean:
	-$(RM) $(C_OBJECTDIR)$(PATHSEP)*.o
	-$(RM) $(S_OBJECTDIR)$(PATHSEP)*.o
	-$(RM) $(subst /,$(PATHSEP),$(EXE))

