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
vpath %.c ../../rv
vpath %.c ../../rv/rm_parser

SRCS = hantro_rv_test.c pptestbench.c params.c cfg.c frame.c
OBJS    = $(SRCS:.c=.o)

SRCS_API = api_rv_test.c pptestbench.c params.c cfg.c frame.c
OBJS_API    = $(SRCS_API:.c=.o)

INCLUDE = -I../../../source/inc \
          -I../../../source/pp \
	  -I../../../source/rv \
	  -I../../rv/rm_parser/include \
	  -I../../../source/config \
	  -I../../../source/common \
	  -I../external_mode \
	  -I../../../linux/memalloc \
          -I../../common/swhw

DECLIBDIR = -L../../../linux/pp  -L../../../linux/dwl -L../../../linux/rv -L../../rv/rm_parser -L../../common/swhw -L../../../../system/models/golden
DECLIB = libppx170.a libdwlx170.a libdecx170.a librvparser.a

TESTBENCH=pp_rv_pipe
TARGET_ENV=

ifeq ($(USE_MD5SUM), y)
        MD5SUM = -DMD5SUM
endif

ifeq ($(shell uname -m),x86_64)
	ifneq (,$(findstring pclinux,$(MAKECMDGOALS)))
		export M32 = -m32
	endif
	ifneq (,$(findstring api,$(MAKECMDGOALS)))
		export M32 = -m32
	endif
	ifneq (,$(findstring coverage,$(MAKECMDGOALS)))
		export M32 = -m32
	endif
	ifneq (,$(findstring verification,$(MAKECMDGOALS)))
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
versatile: CFLAGS = -O -g -mtune=arm926ej-s -fpic -DPP_PIPELINE_ENABLED $(INCLUDE) $(MD5SUM) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE 
versatile: TARGET_ENV=versatile
versatile: LIBS = $(DECLIBDIR) $(EFENCE) -lpthread -ldecx170p -ldwlx170 -ldecx170rv -ltbcommon -lrvparser
versatile: test

.PHONY: pci
pci: CFLAGS = -O -g -fpic -DPP_PIPELINE_ENABLED $(INCLUDE)
pci: TARGET_ENV=pci
pci: LIBS = $(DECLIBDIR) $(EFENCE) -lpthread -lppx170 -ldwlx170 -ldecx170rv -ltbcommon -lrvparser
pci: test

.PHONY: pclinux
pclinux: CC=gcc
pclinux: CFLAGS += -DASIC_TRACE_SUPPORT
pclinux: CFLAGS += -g -DPP_PIPELINE_ENABLED $(INCLUDE) $(M32) $(MD5SUM) -DEXPIRY_DATE=1$(expiry) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE 
pclinux: TESTBENCH=pp_rv_pipe
pclinux: TARGET_ENV=pclinux
pclinux: LIBS = $(DECLIBDIR) -pthread -ldecx170p -ldwlx170 -ldecx170rv -l8170hw -ltbcommon -lrvparser
pclinux: test

.PHONY: api
api: CC=gcc
api: CFLAGS = -g -DPP_PIPELINE_ENABLED $(INCLUDE) $(M32) $(MD5SUM) -DEXPIRY_DATE=1$(expiry) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE 
api: TESTBENCH=pp_rv_pipe
api: TARGET_ENV=pclinux
api: LIBS = $(DECLIBDIR) -ldecx170p -ldwlx170 -ldecx170rv -l8170hw -ltbcommon -lrvparser
api: test_api


.PHONY: coverage
coverage: CC=covc --abs --retain gcc
coverage: CFLAGS = -g -DPP_PIPELINE_ENABLED $(INCLUDE) $(M32) $(MD5SUM) -DEXPIRY_DATE=1$(expiry)
coverage: TESTBENCH=pp_rv_pipe
coverage: TARGET_ENV=coverage
coverage: LIBS = $(DECLIBDIR) -ldecx170p -ldwlx170 -ldecx170rv -l8170hw -ltbcommon -lrvparser
coverage: test

test: lib dwl $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(TESTBENCH)

test_api: lib dwl $(OBJS_API)
	$(CC) $(CFLAGS) $(OBJS_API) $(LIBS) -o $(TESTBENCH)

lib:
	make -C ../../../linux/pp clean
	make -C ../../../linux/pp $(TARGET_ENV) PIPELINE_SUPPORT=-DPP_RVDEC_PIPELINE_SUPPORT IS_8190=1
	make -C ../../../linux/rv $(TARGET_ENV)
	make -C ../../rv/rm_parser $(TARGET_ENV)
	make -C ../../common/swhw $(TARGET_ENV)
	make -C ../../common/utils
	
dwl:
	make -C ../../../linux/dwl $(TARGET_ENV)

clean:
	rm -f *.o
	rm -f .depend
	rm -f $(TESTBENCH)

libclean:
	make -C ../../../linux/pp clean
	make -C ../../../linux/dwl clean
	make -C ../../../linux/rv clean
	make -C ../../rv/rm_parser clean
	make -C ../../common/swhw clean
	make -C ../../common/utils clean

depend .depend dep: $(SRCS)
	$(CC) $(CFLAGS) -M  $^ > .depend

ifeq (.depend, $(wildcard .depend))
include .depend
endif
