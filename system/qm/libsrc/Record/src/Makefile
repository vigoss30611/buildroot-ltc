# Makefile for 19vision video ipc project
# Copyright(C) 2007-2020 19vision Inc.
# Author yi.zhang

include ../../Makefile.param
CFLAGS := $(CFLAGS_BASE)
	
TARGET = libRecord.a

TEST_OBJS := 
OBJS := $(filter-out $(TEST_OBJS), $(patsubst %.c,%.o,$(wildcard *.c)))

OUT_LIB_DIR := $(LIBS_PATH)/$(CHIP_ARCH)/libRecord

INCLUDE =  -I../inc
INCLUDE += -I$(LIBSRC_PATH)/AVServer/inc
INCLUDE += -I$(LIBSRC_PATH)/Hdmng/
INCLUDE += -I$(LIBSRC_PATH)/common/inc
INCLUDE += -I$(LIBSRC_PATH)/mediabuffer/inc
INCLUDE += -I$(LIBSRC_PATH)/SystemMsg/
INCLUDE += -I$(LIBSRC_PATH)/ftp/inc
INCLUDE += -I$(LIBSRC_PATH)/Basic/inc
INCLUDE += -I$(LIBSRC_PATH)/AlarmServer/inc/

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

