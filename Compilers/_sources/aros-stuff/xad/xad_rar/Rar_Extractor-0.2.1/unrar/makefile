# Makefile for project "librarextract.a"
# 

# Uncomment the next line if you are cross compiling
#CROSS_COMPILE = m68k-amigaos-

CC     = $(CROSS_COMPILE)gcc 
CXX    = $(CROSS_COMPILE)c++ 
AS     = $(CROSS_COMPILE)as 
LD     = $(CROSS_COMPILE)ld 
RANLIB = $(CROSS_COMPILE)ranlib 
RM     = delete
# RM     = rm

# Change these as required
OPTIMIZE = -O3
DEBUG = #-g -DDEBUG
CXXFLAGS = $(OPTIMIZE) $(DEBUG) -Wall

# Flags passed to gcc during linking
LINK =

# Name of the "thing" to build
TARGET = librarextract.a

# Additional linker libraries
LIBS = 

# Source code files used in this project
# Add any additional files to this line

SRCS = abstract_file.cpp archive.cpp arcread.cpp crc.cpp getbits.cpp model.cpp Rar_Extractor.cpp \
  rarvm.cpp rawread.cpp rdwrfn.cpp rs.cpp suballoc.cpp unpack.cpp unpack15.cpp unpack20.cpp


OBJS = $(SRCS:.cpp=.o)
# Rules for building
$(TARGET): $(OBJS)
	 $(CROSS_COMPILE)ar rcs $(TARGET) $(OBJS)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS)
