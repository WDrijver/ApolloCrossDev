CC = gcc
OSTYPE = $(shell uname -s)
DEBUG ?= 0

CFLAGS = -Wall -DHAVE_CONFIG_H
CFLAGS += -Ilzma -Ilzma/common -Ilzma/liblzma -Ilzma/liblzma/api -Ilzma/liblzma/check -Ilzma/liblzma/common -Ilzma/liblzma/delta -Ilzma/liblzma/lz -Ilzma/liblzma/lzma -Ilzma/liblzma/rangecoder -Ilzma/liblzma/simple

ifeq ($(DEBUG),1)
    CFLAGS += -g -ggdb -DDEBUG
else
    CFLAGS += -O2 -DNDEBUG
endif

LDFLAGS = -L. -llzma -nostartfiles

ifeq ($(OSTYPE),Morphos)
	LDFLAGS += -noixemul
	CFLAGS += -noixemul
endif

LZMA_OBJ = \
	lzma/common/tuklib_physmem.o \
	lzma/liblzma/check/check.o \
	lzma/liblzma/check/crc32_fast.o \
	lzma/liblzma/check/crc32_table.o \
	lzma/liblzma/check/crc64_fast.o \
	lzma/liblzma/check/crc64_table.o \
	lzma/liblzma/check/sha256.o \
	lzma/liblzma/common/alone_decoder.o \
	lzma/liblzma/common/auto_decoder.o \
	lzma/liblzma/common/block_buffer_decoder.o \
	lzma/liblzma/common/block_decoder.o \
	lzma/liblzma/common/block_header_decoder.o \
	lzma/liblzma/common/block_util.o \
	lzma/liblzma/common/common.o \
	lzma/liblzma/common/easy_decoder_memusage.o \
	lzma/liblzma/common/easy_preset.o \
	lzma/liblzma/common/filter_buffer_decoder.o \
	lzma/liblzma/common/filter_common.o \
	lzma/liblzma/common/filter_decoder.o \
	lzma/liblzma/common/filter_flags_decoder.o \
	lzma/liblzma/common/hardware_physmem.o \
	lzma/liblzma/common/index.o \
	lzma/liblzma/common/index_decoder.o \
	lzma/liblzma/common/index_hash.o \
	lzma/liblzma/common/stream_buffer_decoder.o \
	lzma/liblzma/common/stream_decoder.o \
	lzma/liblzma/common/stream_flags_common.o \
	lzma/liblzma/common/stream_flags_decoder.o \
	lzma/liblzma/common/vli_decoder.o \
	lzma/liblzma/common/vli_size.o \
	lzma/liblzma/delta/delta_common.o \
	lzma/liblzma/delta/delta_decoder.o \
	lzma/liblzma/lz/lz_decoder.o \
	lzma/liblzma/lzma/fastpos_table.o \
	lzma/liblzma/lzma/lzma2_decoder.o \
	lzma/liblzma/lzma/lzma_decoder.o \
	lzma/liblzma/lzma/lzma_encoder_presets.o \
	lzma/liblzma/rangecoder/price_table.o \
	lzma/liblzma/simple/arm.o \
	lzma/liblzma/simple/armthumb.o \
	lzma/liblzma/simple/ia64.o \
	lzma/liblzma/simple/powerpc.o \
	lzma/liblzma/simple/simple_coder.o \
	lzma/liblzma/simple/simple_decoder.o \
	lzma/liblzma/simple/sparc.o \
	lzma/liblzma/simple/x86.o

OBJ = \
	extheader.o \
	xz.o

all: liblzma.a xz

lzma/liblzma/check/%.o: lzma/liblzma/check/%.c
	$(CC) $(CFLAGS) -std=c99 -c $^ -o $@

lzma/liblzma/common/%.o: lzma/liblzma/common/%.c
	$(CC) $(CFLAGS) -std=c99 -c $^ -o $@

lzma/liblzma/delta/%.o: lzma/liblzma/delta/%.c
	$(CC) $(CFLAGS) -std=c99 -c $^ -o $@

lzma/liblzma/lzma/%.o: lzma/liblzma/lzma/%.c
	$(CC) $(CFLAGS) -std=c99 -c $^ -o $@

lzma/liblzma/rangecoder/%.o: lzma/liblzma/rangecoder/%.c
	$(CC) $(CFLAGS) -std=c99 -c $^ -o $@

lzma/liblzma/simple/%.o: lzma/liblzma/simple/%.c
	$(CC) $(CFLAGS) -std=c99 -c $^ -o $@

liblzma.a: $(LZMA_OBJ)
	$(AR) rcs $@ $?

xz: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(LZMA_OBJ)
