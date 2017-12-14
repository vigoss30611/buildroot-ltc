# Makefile for 19vision video ipc project
# Copyright(C) 2007-2020 19vision Inc.
# Author yi.zhang

include ../Makefile.param
CFLAGS := $(CFLAGS_BASE)

TARGET = libsystem_msg.a

OBJS = system_msg.o
INSTALLDIR = $(EXEC_DIR)
OUT_LIB_DIR :=   $(LIBS_PATH)/$(CHIP_ARCH)/libsystem_msg
INCLUDE =  -I$(LIBINC_PATH)/libqmapi
INCLUDE += -I$(LIBINC_PATH)/libsystem_msg/inc

CFLAGS += -Wall -Werror -O2
all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) -rcv $@ $(OBJS)
	$(if $(shell find $(OUT_LIB_DIR)), $(shell cd ./), $(shell mkdir $(OUT_LIB_DIR)))
	cp $(TARGET) $(OUT_LIB_DIR)/
	
%.o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) *.gdb *.elf *.o
