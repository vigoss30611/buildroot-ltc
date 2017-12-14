# Makefile for 19vision video ipc project
# Copyright(C) 2007-2020 19vision Inc.
# Author yi.zhang

include ../../Makefile.param
CFLAGS := $(CFLAGS_BASE)

TARGET = libProtocolService.so

TEST_OBJS := 
DEPEND_FILES += $(wildcard ./*.c)
DEPEND_FILES += $(wildcard ./Onvif/*.c)
DEPEND_FILES += $(wildcard ./Hik/*.c)
DEPEND_FILES += $(wildcard ./XMai/*.c)
DEPEND_FILES += $(wildcard ./ZhiNuo/*.c)
OBJS := $(DEPEND_FILES:.c=.o)
#OBJS := $(filter-out $(TEST_OBJS), $(patsubst %.c,%.o,$(wildcard *.c)))

OUT_LIB_DIR := $(LIBS_PATH)/$(CHIP_ARCH)/ProtocolService

INCLUDE += -I ./
INCLUDE += -I ./Onvif/inc
INCLUDE += -I ./Hik/inc
INCLUDE += -I ./TopSee/inc
INCLUDE += -I ./XMai/inc
INCLUDE += -I ./HanBang/inc
INCLUDE += -I ./ZhiNuo/inc
INCLUDE += -I ./rtsp/inc
INCLUDE += -I ../inc
INCLUDE += -I $(SDK_INC_PATH)
INCLUDE += -I$(LIBINC_PATH)/
INCLUDE += -I $(IPC_PATH)/onvif/include
INCLUDE += -I $(IPC_PATH)/hik/inc
INCLUDE += -I $(IPC_PATH)/hik/inc/encrypt

ifeq ($(CFLAGS_THTTPD_SERVER),y)
INCLUDE += -I$(IPC_PATH)/thttpd-2.25b/inc
EXT_LIBS += $(LIBS_PATH)/$(CHIP_ARCH)/libthttpd/libthttpd.so
endif

CFLAGS += -Wall -O2 -fPIC
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -shared -o $@ $(OBJS) $(EXT_LIBS)
	$(if $(shell find $(OUT_LIB_DIR)), $(shell cd ./), $(shell mkdir $(OUT_LIB_DIR)))
	cp $(TARGET) $(OUT_LIB_DIR)/
	
%.o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) *.gdb *.elf *.o

