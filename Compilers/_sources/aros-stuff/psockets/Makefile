CC = gcc
AR = ar
DEBUG = 1
OSTYPE = $(shell uname -s)
RM = rm -f

CFLAGS = -Wall -Iinclude
LDFLAGS = -L. -lpsockets

ifeq ($(DEBUG), 1)
CFLAGS += -g -DDEBUG
else
CFLAGS += -O2
endif

ifeq ($(OSTYPE), MorphOS)
CFLAGS += -noixemul
LDFLAGS += -noixemul
endif

ifeq ($(OSTYPE), AROS)
#CFLAGS +=
#LDFLAGS +=
endif

ifeq ($(OSTYPE), AmigaOS)
#CFLAGS +=
#LDFLAGS +=
endif

LIB_OBJ = \
	gai_strerror.o \
	getaddrinfo.o \
	getnameinfo.o \
	inet_aton.o \
	inet_ntop_ipv4.o \
	inet_pton_ipv4.o \
	poll.o

TEST_OBJ = \
	testga.o

.PHONY: all clean

all: libpsockets.a testga

libpsockets.a: $(LIB_OBJ)
	$(AR) rcs $@ $?

testga: $(TEST_OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	$(RM) $(LIB_OBJ) $(TEST_OBJ)
