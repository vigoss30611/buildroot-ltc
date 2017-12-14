# Makefile for 19vision video ipc project
# Copyright(C) 2007-2020 19vision Inc.
# Author yi.zhang

include ../../Makefile.param
CFLAGS := $(CFLAGS_BASE)

TARGET = libmediabuffer.a

TEST_OBJS := 
OBJS := $(filter-out $(TEST_OBJS), $(patsubst %.c,%.o,$(wildcard *.c)))
OBJS += $(filter-out $(TEST_OBJS), $(patsubst %.cpp,%.o,$(wildcard *.cpp)))

OUT_LIB_DIR := $(LIBS_PATH)/$(CHIP_ARCH)/libmediabuffer

INCLUDE =  -I./libinc
INCLUDE += -I../inc
INCLUDE += -I$(SDK_INC_PATH)
INCLUDE += -I$(LIBINC_PATH)/libmediabuffer

CFLAGS += -Wall -O2
all: $(TARGET)

CFLAGS+= -fPIC -DHI_OS_LINUX -DHI_OS_TYPE=HI_OS_LINUX -DCONFIG_NETWORK_USE_SYSTEM -DHI_LINUX_SUPPORT_UCLIBC -DHI_OS_SUPPORT_UCLINUX -DHI_MEMORY_DEBUG

$(TARGET): $(OBJS)
	$(AR) -rcv $@ $(OBJS)
	$(if $(shell find $(OUT_LIB_DIR)), $(shell cd ./), $(shell mkdir $(OUT_LIB_DIR)))
	cp $(TARGET) $(OUT_LIB_DIR)/
	
%.o: %.cpp
	$(GXX) $(INCLUDE) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(GXX) $(INCLUDE) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) *.gdb *.elf *.o

