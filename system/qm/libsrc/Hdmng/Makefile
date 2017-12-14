# Makefile for 19vision video ipc project
# Copyright(C) 2007-2020 19vision Inc.
# Author yi.zhang

include ../Makefile.param
CFLAGS := $(CFLAGS_BASE)
TARGET = libhdmng.a

TEST_OBJS := 
OBJS := $(filter-out $(TEST_OBJS), $(patsubst %.c,%.o,$(wildcard *.c)))

INSTALLDIR = $(EXEC_DIR)
OUT_LIB_DIR :=   $(LIBS_PATH)/$(CHIP_ARCH)/libhdmng

INCLUDE :=
INCLUDE += -I$(LIBSRC_PATH)/common/inc
INCLUDE += -I$(LIBSRC_PATH)/SystemMsg/
INCLUDE += -I$(LIBSRC_PATH)/Audio/inc
INCLUDE += -I$(LIBSRC_PATH)/Basic/inc

CFLAGS += -Wall -O2
CFLAGS += -ffunction-sections -fdata-sections
all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) -rcv $@ $(OBJS)
	$(if $(shell find $(OUT_LIB_DIR)), $(shell cd ./), $(shell mkdir $(OUT_LIB_DIR)))
	cp $(TARGET) $(OUT_LIB_DIR)/
	
%.o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) *.gdb *.elf *.o
