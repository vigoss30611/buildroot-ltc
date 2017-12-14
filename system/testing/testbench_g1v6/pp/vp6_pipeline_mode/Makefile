#-------------------------------------------------------------------------------
#-                                                                            --
#-       This software is confidential and proprietary and may be used        --
#-        only as expressly authorized by a licensing agreement from          --
#-                                                                            --
#-                            Hantro Products Oy.                             --
#-                                                                            --
#-                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
#-                            ALL RIGHTS RESERVED                             --
#-                                                                            --
#-                 The entire notice above must be reproduced                 --
#-                  on all copies and should not be removed.                  --
#-                                                                            --
#-------------------------------------------------------------------------------
#-
#--  Abstract : Makefile for decoder testbench
#--
#-------------------------------------------------------------------------------

# set this to 'y' for md5 checksum calculation of the frames instead of yuv
USE_MD5SUM = n

#source search path
vpath %.c
vpath %.c ../external_mode
vpath %.c ../../vp6

SRCS = on2vp6dec.c pptestbench.c params.c cfg.c frame.c
OBJS    = $(SRCS:.c=.o)

INCLUDE = -I../../../source/inc \
          -I../../../source/pp \
	  -I../../../source/vp6 \
	  -I../../../source/config \
	  -I../../../source/common \
	  -I../external_mode \
	  -I../../../linux/memalloc \
          -I../../common/swhw

LIBDECDIR = -L../../../linux/pp \
            -L../../../linux/dwl \
            -L../../../linux/vp6 \
            -L../../common/swhw \
	    -L../../../../system/models/golden

LIBDEC = dec8190vp6

TESTBENCH=pp_vp6_pipe

TARGET_ENV=

ifeq ($(USE_MD5SUM), y)
        MD5SUM = -DMD5SUM
endif

ifeq ($(shell uname -m),x86_64)
	ifneq (,$(findstring pclinux,$(MAKECMDGOALS)))
		export M32 = -m32
	endif
	ifneq (,$(findstring coverage,$(MAKECMDGOALS)))
		export M32 = -m32
	endif
endif

export ASIC_TRACE_SUPPORT
ifeq ($(ASIC_TRACE_SUPPORT), y)
	CFLAGS+=-DASIC_TRACE_SUPPORT
endif

CFLAGS += -DAVS_PLUS_SUPPORTED=1

export CONFIG_FRAME_STRIDE
ifeq ($(CONFIG_FRAME_STRIDE),y)
        CFLAGS += -DCONFIG_FRAME_STRIDE
endif

export YUV_COMBINED
ifneq ($(YUV_COMBINED),y)
        CFLAGS += -DLUMA_CHROMA_SEPERATED
endif

all: versatile

.PHONY: versatile
versatile: CC=arm-none-linux-gnueabi-gcc
versatile: CFLAGS = -O0 -g -mtune=arm926ej-s -fpic -DPP_PIPELINE_ENABLED $(INCLUDE) $(MD5SUM) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE 
versatile: TARGET_ENV=versatile
versatile: DWL_TARGET_ENV=versatile
versatile: LIBS = $(LIBDECDIR) $(EFENCE) -lpthread -ldecx170p -ldwlx170 -ldec8190vp6 -ltbcommon
versatile: test

.PHONY: pci
pci: CFLAGS = -O0 -g -fpic -DPP_PIPELINE_ENABLED $(INCLUDE)
pci: TARGET_ENV=pci
pci: DWL_TARGET_ENV=pci
pci: LIBS = $(LIBDECDIR) $(EFENCE) -lpthread -lppx170 -ldwlx170 -ldec8190vp6 -ltbcommon
pci: test

.PHONY: pclinux
pclinux: CC=gcc
pclinux: CFLAGS += -DASIC_TRACE_SUPPORT
pclinux: CFLAGS += $(M32) -g -O0 -DPP_PIPELINE_ENABLED $(INCLUDE) $(MD5SUM) -DEXPIRY_DATE=1$(expiry) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE 
pclinux: TARGET_ENV=pclinux
pclinux: LIBS = $(LIBDECDIR) -pthread -ldecx170p -ldwlx170 -ldec8190vp6 -l8170hw -ltbcommon
pclinux: test

test: lib $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(TESTBENCH)

lib:
	make -C ../../../linux/pp clean
	make -C ../../../linux/pp $(TARGET_ENV) PIPELINE_SUPPORT=-DPP_VP6DEC_PIPELINE_SUPPORT
	make -C ../../../linux/vp6 $(TARGET_ENV)
	make -C ../../../linux/dwl $(TARGET_ENV)
	make -C ../../common/swhw $(TARGET_ENV)
	make -C ../../common/utils

clean:
	rm -f *.o core.*
	rm -f .depend
	rm -f $(TESTBENCH)

libclean:
	make -C ../../../linux/pp clean
	make -C ../../../linux/dwl clean
	make -C ../../../linux/vp6 clean
	make -C ../../common/swhw clean
	make -C ../../common/utils clean

depend .depend dep: $(SRCS)
	$(CC) $(CFLAGS) -M  $^ > .depend

ifeq (.depend, $(wildcard .depend))
include .depend
endif
