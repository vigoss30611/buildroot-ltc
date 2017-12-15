# Makefile for 19vision video ipc project
# Copyright(C) 2007-2020 19vision Inc.
# Author yi.zhang

export TOPDIR =$(shell pwd)

include ./Makefile.param
CFLAGS += $(CFLAGS_BASE)

TARGET = libQMHAL.so

ifeq ($(QM_HAL_QIPC_38F),y)
PRODUCT_DIR = src/qipc_38f
endif

ifeq ($(QM_HAL_QIPC_38I),y)
PRODUCT_DIR = src/qipc_38i
endif

ifeq ($(QM_HAL_QIPC_Q3520E_SINGLE),y)
PRODUCT_DIR = src/qipc_q3520e_single
endif

ifeq ($(QM_HAL_SYS_DUMP),y)
PRODUCT_DIR = src/sys_dump
endif

TEST_OBJS := 
OBJS := $(filter-out $(TEST_OBJS), $(patsubst %.c,%.o,$(wildcard $(PRODUCT_DIR)/*.c)))
#OBJS:=


#OUT_LIB_DIR := $(LIBS_PATH)/$(CHIP_ARCH)/QMHAL

INCLUDE += -I./inc
INCLUDE += -I$(SDK_INC_PATH)
#INCLUDE += -I$(LIBINC_PATH)/libqmapi
#INCLUDE += -I$(LIBINC_PATH)/libQMHAL/inc

INCLIBS += -lvideobox -laudiobox -lcodecs -lupgrade -lsys -ldemux -leventhub -litems -lfr -L$(SDK_LIB_PATH)

#INCLIBS += -lvideobox -laudiobox -lcodecs -lupgrade -lsys -ldemux -leventhub -litems -lsensor -lfr -L$(SDK_LIB_PATH)


CFLAGS += -Wall -O2 -fPIC
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -shared -o $@ $(OBJS) $(INCLIBS)
	
#	$(if $(shell find $(OUT_LIB_DIR)), $(shell cd ./), $(shell mkdir $(OUT_LIB_DIR)))
#	cp $(TARGET) $(OUT_LIB_DIR)/
	
%.o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) *.gdb *.elf *.o

