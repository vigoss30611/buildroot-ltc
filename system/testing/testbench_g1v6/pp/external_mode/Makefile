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

SRCS = pptestbench.c \
    cfg.c \
    frame.c \
    main.c \
    params.c

OBJS    = $(SRCS:.c=.o)

INCLUDE = -I../../../source/inc -I../../../source/pp/ -I../../../source/config \
          -I../../../source/common  -I../../common/swhw -I../../../linux/memalloc/

DECLIBDIR = -L../../../linux/dwl -L../../../linux/pp -L../../common/swhw \
            -L../../../../system/models/golden -L../../common/utils
DECLIB =  libdecx170p.a
DWLLIB = libdwlx170.a
TBLIB = libtbcommon.a

ALLTESTDECS= px170dec_versatile px170dec_integrator \
             px170dec_ads11 px170dec_pclinux px170dec_pclinux_eval \
	     px170dec_ads9
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
versatile: CFLAGS = -O -g -mtune=arm926ej-s -fpic $(INCLUDE) $(MD5SUM) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE 
versatile: TESTDEC=px170dec_versatile
versatile: TARGET_ENV=versatile
versatile: LIBS = $(DECLIBDIR) -lpthread -ldecx170p -ldwlx170 -ltbcommon -lutils $(EFENCE) $(MD5SUM)
versatile: test


.PHONY: integrator
integrator: CC=arm-linux-gcc
integrator: CFLAGS = -O -g -mtune=arm920tdmi -fpic  $(INCLUDE) $(MD5SUM)
integrator: TESTDEC=px170dec_integrator
integrator: TARGET_ENV=integrator
integrator: LIBS = $(DECLIBDIR) -lpthread -ldecx170p -ldwlx170 -ltbcommon -lutils $(EFENCE)
integrator: test


.PHONY: ads11
ads11: CC=armcc
ads11: CFLAGS = -cpu ARM1136J-S -O3 -Otime $(INCLUDE)
ads11: TESTDEC=px170dec_ads11
ads11: TARGET_ENV=ads11
ads11: LIBS = -l../../../linux/pp/$(DECLIB) -l../../../linux/dwl/$(DWLLIB) -I../../common/swhw/$(TBLIB)
ads11: test

.PHONY: ads9
ads9: CC=armcc
ads9: CFLAGS = -cpu ARM926EJ-S -O3 -Otime $(INCLUDE)
ads9: TESTDEC=px170dec_ads9
ads9: TARGET_ENV=ads9
ads9: LIBS = -l../../../linux/pp/$(DECLIB) -l../../../linux/dwl/$(DWLLIB) -I../../common/swhw/$(TBLIB)
ads9: test


.PHONY: pclinux
pclinux: CC=gcc
pclinux: CFLAGS+=-DASIC_TRACE_SUPPORT
pclinux: CFLAGS += -g $(INCLUDE) $(MD5SUM) $(M32) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -DEXPIRY_DATE=1$(expiry) 
pclinux: TESTDEC=px170dec_pclinux
pclinux: TARGET_ENV=pclinux
pclinux: LIBS = $(DECLIBDIR) -pthread -ldecx170p -ldwlx170 -l8170hw -ltbcommon -lutils $(EFENCE)
pclinux: test

.PHONY: coverage
coverage: CC=covc --abs --retain gcc
coverage: CFLAGS = -g $(INCLUDE) $(MD5SUM) $(M32)
coverage: TESTDEC=px170dec_coverage
coverage: TARGET_ENV=coverage
coverage: LIBS = $(DECLIBDIR) -ldecx170p -ldwlx170 -l8170hw -ltbcommon -lutils $(EFENCE)
coverage: test

.PHONY: verification
verification: SYSTEM_VERIFICATION_TEST = -DSYSTEM_VERIFICATION
verification: pclinux

.PHONY: pclinux_eval_8170
pclinux_eval_8170: CFLAGS += -DPP_EVALUATION_8170
pclinux_eval_9190: _PRODUCT_8170 = y
pclinux_eval_8170: pclinux_eval

.PHONY: pclinux_eval_8190
pclinux_eval_8190: CFLAGS += -DPP_EVALUATION_8190
pclinux_eval_9190: _PRODUCT_8190 = y
pclinux_eval_8190: pclinux_eval

.PHONY: pclinux_eval_9170
pclinux_eval_9170: CFLAGS += -DPP_EVALUATION_9170
pclinux_eval_9190: _PRODUCT_9170 = y
pclinux_eval_9170: pclinux_eval

.PHONY: pclinux_eval_9190
pclinux_eval_9190: CFLAGS += -DPP_EVALUATION_9190
pclinux_eval_9190: _PRODUCT_9190 = y
pclinux_eval_9190: pclinux_eval

.PHONY: pclinux_eval_G1
pclinux_eval_G1: CFLAGS += -DPP_EVALUATION_G1
pclinux_eval_G1: _PRODUCT_G1 = y
pclinux_eval_G1: pclinux_eval

.PHONY: pclinux_eval
pclinux_eval: CC=gcc
pclinux_eval: CFLAGS += $(INCLUDE) $(MD5SUM) $(M32) -DPP_EVALUATION -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -DEXPIRY_DATE=1$(expiry)
pclinux_eval: TESTDEC=px170dec_pclinux_eval
pclinux_eval: TARGET_ENV=pclinux_eval
pclinux_eval: LIBS = $(DECLIBDIR) -ldecx170p -ldwlx170 -l8170hw -ltbcommon -lutils $(EFENCE)
pclinux_eval: DEBFLAGS = -O3 -DNDEBUG
pclinux_eval: test


export _PRODUCT_8170
export _PRODUCT_8190
export _PRODUCT_9170
export _PRODUCT_9190
export _PRODUCT_G1


test: lib  $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(TESTDEC)

lib:
	make -C ../../../linux/pp $(TARGET_ENV) PIPELINE_SUPPORT=-DNO_PIPELINE
	make -C ../../../linux/dwl $(TARGET_ENV)
	make -C ../../common/swhw $(TARGET_ENV)
	make -C ../../common/utils

clean:
	rm -f *.o
	rm -f .depend
	rm -f $(ALLTESTDECS)

libclean:
	make -C ../../../linux/dwl clean
	make -C ../../../linux/pp clean
	make -C ../../common/swhw clean
	make -C ../../common/utils clean

depend .depend dep: $(SRCS)
	$(CC) $(CFLAGS) -M  $^ > .depend

ifeq (.depend, $(wildcard .depend))
include .depend
endif
