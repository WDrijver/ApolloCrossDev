# ApolloCrossDev Makefile Template v1.2
# 05-11-2026
# Willem Drijver
#
# make          compile, link, upload to the Apollo V4 (AMIGAHOST), clean up
# make debug    as make, plus bgdbserver and a <project>_Debug script
# make compile  compile and link only, no upload
#
# The project folder can be anywhere: set APOLLOCROSSDEV to the
# ApolloCrossDev folder (export APOLLOCROSSDEV=~/ApolloCrossDev in ~/.bashrc
# or ~/.zshrc). Without it, the project must be in ApolloCrossDev/Projects/.

# Define Shell Environment
PWD = $(shell pwd)
DIR = $(notdir $(shell pwd))
APOLLO_V4_HOST = "${AMIGAHOST}:Ram Disk/"

# Define ApolloCrossDev Folder and Compiler
ACD_ROOT ?= $(or $(APOLLOCROSSDEV),$(PWD)/../..)
PREFIX 	= $(ACD_ROOT)/Compilers/GCC-6.50-Dev
TARGET 	= m68k-amigaos
ACP 	= $(ACD_ROOT)/Projects/ApolloExplorer/acp/acp
GDB		= $(ACD_ROOT)/Projects/bgdbserver/bgdbserver
ICONS	= $(ACD_ROOT)/Projects/_icons

#Define Project Name (replace <project-name>)
PROJECT_NAME	= <project-name>
PROJECT_DIR		= $(PWD)

# Define GNU C-Compiler Type (C = Default, remove/add # to enable/disable 
C_COMPILER	= $(PREFIX)/bin/m68k-amigaos-gcc
# C_COMPILER	= $(PREFIX)/bin/m68k-amigaos-g++

# Define Standard C-Libraries and Headers (libnix = Default, remove/add # to enable/disable 
C_LIB		= -noixemul
# C_LIB 	= -mcrt=clib2
C_INCL_CLB	= $(PREFIX)/$(TARGET)/libnix/include
# C_INCL_CLB  = $(PREFIX)/$(TARGET)/clib2/include

# Define ApolloLib: its sources are compiled into the program (into LIB_OBJECTDIR)
# with the project's flags. Put # before APOLLOLIB to build without ApolloLib.
APOLLOLIB = $(ACD_ROOT)/Projects/ApolloLib

# Define Additional Libraries - SDL, MUI and Mathemetics, remove/add # to enable/disable 
# C_LIBS_SDL	= -lSDL -lSDL_ttf -lfreetype
# C_LIBS_MUI	= -lmui
# C_LIBS_MATH	= -lm
C_LIBS_ALL 	= $(C_LIBS_SDL) $(C_LIBS_MUI) $(C_LIBS_MATH) 

# Define SDK/NDK Headers, remove/add # to enable/disable 
C_INCL_SYS	= $(PREFIX)/$(TARGET)/sys-include
# C_INCL_NDK  = $(PREFIX)/$(TARGET)/ndk13-include
# C_INCL_NDK  = $(PREFIX)/$(TARGET)/ndk39-include
C_INCL_NDK  = $(PREFIX)/$(TARGET)/ndk32-include/Include_H
# C_INCL_GL	= $(PREFIX)/$(TARGET)/include/GL
# C_INCL_SDL	= $(PREFIX)/$(TARGET)/include/SDL
# C_INCL_MUI	= $(PREFIX)/$(TARGET)/include/MUI
C_INCL_ALL 	= -I. -I$(C_INCL_CLB) -I$(C_INCL_SYS) -I$(C_INCL_NDK) $(if $(APOLLOLIB),-I$(APOLLOLIB))

# Define C Options
C_OPTIONS 	= -O2 -m68080 -m68881 -ffast-math -fomit-frame-pointer -DREGPARM=4
# -O = basic optimization | -O2 = advanced optimization (default and advised) | -O3 = agressive optimization | -Os = small size | =Ofast = speed optimization (same as -O2)
# -m68080 = target for Apollo V4 (default and advised) | -m68881 = enable FPU (default and advised) | -ffast-math = faster but less accurate math = -fomit-frame-pointer = free up register
C_DEFINES 	= 
C_DEBUG		= -g
C_FLAGS = $(C_LIB) $(C_OPTIONS) $(C_DEBUG) $(C_DEFINES) $(C_INCL_ALL) $(C_LIBS_ALL)

# Define ASM Compiler
S_COMPILER = $(PREFIX)/bin/vasmm68k_mot

# Define ASM Includes
S_INCLUDE 	= $(PREFIX)/$(TARGET)/DevPac/Include
S_LIBS		= $(PREFIX)/$(TARGET)/DevPac/Libs	

# Define ASM Options
S_DEBUG		= -linedebug -kick1hunks
S_FLAGS 	= -Fhunk -m68080 -m68881 -opt-speed $(S_DEBUG) -I$(S_INCLUDE)

# Define Object Linker
O_LINKER = $(PREFIX)/bin/m68k-amigaos-gcc

# Define Source and Object Directory
C_SOURCEDIR = $(PROJECT_DIR)
S_SOURCEDIR = $(PROJECT_DIR)
C_OBJECTDIR = $(PROJECT_DIR)
S_OBJECTDIR = $(PROJECT_DIR)
LIB_OBJECTDIR = $(PROJECT_DIR)/.apollolib

# Define Source and Object Filelist
C_FILES = $(wildcard $(C_SOURCEDIR)/*.c)
S_FILES = $(wildcard $(S_SOURCEDIR)/*.s)
LIB_C_FILES = $(if $(APOLLOLIB),$(wildcard $(APOLLOLIB)/*.c))
LIB_S_FILES = $(if $(APOLLOLIB),$(wildcard $(APOLLOLIB)/*.s))
APP_O_FILES = $(patsubst $(C_SOURCEDIR)/%.c,$(C_OBJECTDIR)/%.o,$(C_FILES)) $(patsubst $(S_SOURCEDIR)/%.s,$(S_OBJECTDIR)/%.o,$(S_FILES))
LIB_O_FILES = $(patsubst $(APOLLOLIB)/%.c,$(LIB_OBJECTDIR)/%.o,$(LIB_C_FILES)) $(patsubst $(APOLLOLIB)/%.s,$(LIB_OBJECTDIR)/%.o,$(LIB_S_FILES))
O_FILES = $(APP_O_FILES) $(LIB_O_FILES)

EXE		= $(PROJECT_DIR)/$(PROJECT_NAME)

# Make Project (Init -> Build -> Clean -> Exit)
all: init cleanbefore build upload_all cleanafter exit

debug: init cleanbefore build upload_debug cleanafter exit

compile: init cleanbefore build cleanafter exit

init:
	@clear 2>/dev/null || true
	@echo "\e[37m------------------------------------------------------------------\e[0m"
	@echo "\e[1;31mApollo\e[1;30mCrossDev \e[37mGCC-6.50 Compiler v1.2\e[0m"    
	@echo "\e[36mC-Sources : \e[37m$(C_FILES)\e[0m"
	@echo "\e[36mS-Sources : \e[37m$(S_FILES)\e[0m"
	@echo "\e[36mApolloLib : \e[37m$(if $(APOLLOLIB),$(APOLLOLIB),off)\e[0m"
	@echo "\e[36mExecutable: \e[37m$(EXE)\e[0m"
	@echo "\e[37m------------------------------------------------------------------\e[0m"

cleanbefore:
	@rm -f $(C_OBJECTDIR)/*.o $(S_OBJECTDIR)/*.o $(PROJECT_DIR)/$(PROJECT_NAME)*.txt $(EXE)
	@rm -rf $(LIB_OBJECTDIR)

build: $(EXE)
	@echo "\e[37mStep #2 - \e[31mLinker \e[37mFinished\e[0m"

# Link all Objects to one Executable
$(EXE) : $(O_FILES) 
	@$(O_LINKER) $(O_FILES) $(C_FLAGS) -o $(EXE) >>/dev/null
	@echo "\e[37mStep #1 - \e[31mCompiler \e[37mFinished\e[0m"

# Compile all C-Sources to Objects 
$(C_OBJECTDIR)/%.o : $(C_SOURCEDIR)/%.c
	@$(C_COMPILER) -c $(C_FLAGS) -o $@ $< >>/dev/null

# Compile all ASM-Sources to Objects
$(S_OBJECTDIR)/%.o : $(S_SOURCEDIR)/%.s
	@$(S_COMPILER) $(S_FLAGS) -o $@ $< >>/dev/null

# Compile ApolloLib with the project's flags
$(LIB_OBJECTDIR)/%.o : $(APOLLOLIB)/%.c
	@mkdir -p $(LIB_OBJECTDIR)
	@$(C_COMPILER) -c $(C_FLAGS) -o $@ $< >>/dev/null

$(LIB_OBJECTDIR)/%.o : $(APOLLOLIB)/%.s
	@mkdir -p $(LIB_OBJECTDIR)
	@$(S_COMPILER) $(S_FLAGS) -o $@ $< >>/dev/null

upload_all:
	@echo "\e[37mStep #3 - \e[31mApollo\e[30mExplorer \e[37mUpload\e[30m"
	@$(ACP) $(EXE) $(APOLLO_V4_HOST)
	@cp $(ICONS)/ApolloIcon.info $(PROJECT_DIR)/$(PROJECT_NAME).info
	@$(ACP) $(PROJECT_DIR)/$(PROJECT_NAME).info $(APOLLO_V4_HOST)

upload_debug:
	@echo "\e[37mStep #3 - \e[31mApollo\e[30mExplorer \e[37mUpload\e[30m"
	@$(ACP) $(EXE) $(APOLLO_V4_HOST)
	@$(ACP) $(GDB) $(APOLLO_V4_HOST)
	@cp $(ICONS)/ApolloIcon.info $(PROJECT_DIR)/$(PROJECT_NAME).info
	@cp $(ICONS)/ApolloIcon_Debug.info $(PROJECT_DIR)/$(PROJECT_NAME)_Debug.info
	@$(ACP) $(PROJECT_DIR)/$(PROJECT_NAME).info $(APOLLO_V4_HOST)
	@$(ACP) $(PROJECT_DIR)/$(PROJECT_NAME)_Debug.info $(APOLLO_V4_HOST)	
	@echo "bgdbserver $(PROJECT_NAME)" > $(EXE)_Debug
	@$(ACP) $(EXE)_Debug $(APOLLO_V4_HOST)

cleanafter:
	@rm -f $(C_OBJECTDIR)/*.o $(S_OBJECTDIR)/*.o $(PROJECT_DIR)/*.info $(EXE)_Debug
	@rm -rf $(LIB_OBJECTDIR)

exit:
	@echo "\e[37m------------------------------------------------------------------\e[0m"
	@echo "\e[1;31mApollo\e[1;30mCrossDev \e[32mCompile Successful\e[0m"
	@echo "\e[37m------------------------------------------------------------------\e[0m"
	@echo " "
