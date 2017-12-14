# Makefile for 19vision video ipc project
# Copyright(C) 2007-2020 19vision Inc.
# Author David

ifeq ($(CHIP_ARCH),Q3)
include ../../Makefile.param
CFLAGS += -DTL_Q3_PLATFORM
endif

TARGET = netserver.a

TEST_OBJS := 
OBJS := $(filter-out $(TEST_OBJS), $(patsubst %.c,%.o,$(wildcard *.c)))

OUT_LIB_DIR := $(LIBS_PATH)/$(CHIP_ARCH)/netserver

INCLUDE =  -I../inc

INCLUDE += -I$(LIBINC_PATH)/

CFLAGS += -Wall -O2
all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) -rcv $@ $(OBJS)
	$(if $(shell find $(OUT_LIB_DIR)), $(shell cd ./), $(shell mkdir $(OUT_LIB_DIR)))
	cp $(TARGET) $(OUT_LIB_DIR)/
	
%.o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) *.gdb *.elf *.o

