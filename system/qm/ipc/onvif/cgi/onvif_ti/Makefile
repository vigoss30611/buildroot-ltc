# Makefile for 19vision video ipc project
# Copyright(C) 2007-2020 19vision Inc.
# Author yi.zhang

include ../../../Makefile.param

RANLIB = true
ARFLAGS = rcv

ROOT_DIR := .
DEPFILE := dep

SRC_ROOT            :=   $(ROOT_DIR)/../../
LIBS_PATH           :=   $(ROOT_DIR)/lib
OBJ_ROOT            :=   $(ROOT_DIR)/obj

OUT_LIB_DIR :=   $(ROOT_DIR)/../../../../libs/$(CHIP_ARCH)/libonvif


TARGET	= libonvif.a
TARGET_SO	= libonvif.so

DEPEND_FILES  = $(wildcard ../../misc/*.c) onvif.c
DEPEND_FILES += soapC.c
DEPEND_FILES += soapServer.c
DEPEND_FILES += ../../gsoap2.8/stdsoap2.c
DEPEND_OBJS	  = $(DEPEND_FILES:.c=.o)

CFLAGS := -Wall -fPIC -Os -ffunction-sections  -fdata-sections -DWITH_NOIDREF -fvisibility=hidden -Wl,-gc-sections 
IFLAG 	= -I../../gsoap2.8 
IFLAGS  += -I../../include
IFLAGS  += -I$(LIBINC_PATH)/
IFLAGS  += -I$(IPC_PATH)/thttpd-2.25b/inc

compile: $(TARGET)


#
#	Build archive of objects
#
all:$(TARGET)
$(TARGET): $(DEPEND_OBJS)
	@echo "*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@echo "* CREATE $(OUT_LIB_DIR)"
	$(if $(shell find $(OUT_LIB_DIR)), $(shell cd ./), $(shell mkdir $(OUT_LIB_DIR)))			
	$(CC) -w $(CFLAGS) $(IFLAGS) $(IFLAG) -fPIC -shared -fvisibility=hidden -Wl,-gc-sections -o $(OUT_LIB_DIR)/$(TARGET_SO) $(DEPEND_OBJS)
	$(STRIP) $(OUT_LIB_DIR)/$(TARGET_SO)

clean:
	rm -f $(NAME) $(TARGET) $(DEPEND_OBJS)
	rm -f *.o ../../gsoap/stdsoap2.o

$(DEPEND_OBJS):$(DEPEND_FILES)
#
#	Dependencies
#
#
#	Transition rules (add -o to put object in right directory)
#
.c.o:
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $(IFLAG) $<

