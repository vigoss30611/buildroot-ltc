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

SRCS = dectestbench.c
OBJS    = $(SRCS:.c=.o)

INCLUDE = -I../../source/inc -I../../source/jpeg -I../../source/dwl \
	  -I../../source/config -I../../source/common -I../common/swhw


DECLIBDIR = -L../../linux/jpeg -L../../linux/dwl -L../common/swhw \
	    -L../../../system/models/golden -L../common/utils
DECLIB = libx170j.a
DWLLIB = libdwlx170.a
TBLIB = libtbcommon.a
MODLIB = lib8170hw.a

TESTDEC=jpegdec
TARGET_ENV=

# Enable SW performance  measurement
#CFLAGS = -DSW_PERFORMANCE

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

CFLAGS += -DAVS_PLUS_SUPPORTED=1

export CONFIG_FRAME_STRIDE
ifeq ($(CONFIG_FRAME_STRIDE),y)
	CFLAGS += -DCONFIG_FRAME_STRIDE
endif

export YUV_COMBINED
ifneq ($(YUV_COMBINED),y)
        CFLAGS += -DLUMA_CHROMA_SEPERATED
endif

export ASIC_TRACE_SUPPORT
ifeq ($(ASIC_TRACE_SUPPORT), y)
	CFLAGS+=-DASIC_TRACE_SUPPORT
endif

all: versatile

.PHONY: versatile
versatile: CC=arm-none-linux-gnueabi-gcc
versatile: CFLAGS += -O -g -mtune=arm926ej-s -fpic $(INCLUDE) -DLINUX -DSDRAM_LM_BASE=0x00000000 \
                      -DDEC_MODULE_PATH=\"/dev/hx170dec\" -DDEC_IO_BASE=0xC0000000
versatile: TESTDEC=jx170dec_versatile
versatile: TARGET_ENV=versatile
versatile: LIBS = $(DECLIBDIR) -lpthread -lx170j -ldwlx170 -ltbcommon -lutils $(EFENCE)
versatile: test

.PHONY: integrator
integrator: CC=arm-linux-gcc
integrator: CFLAGS += -O -g -mtune=arm920t -fpic $(INCLUDE) -DLINUX -DSDRAM_LM_BASE=0x80000000 \
                      -DDEC_MODULE_PATH=\"/dev/hx170dec\" -DDEC_IO_BASE=0xD0000000
integrator: TESTDEC=jx170dec_integrator
integrator: TARGET_ENV=integrator
integrator: LIBS = $(DECLIBDIR) -lpthread -lx170j -ldwlx170 -ltbcommon -lutils $(EFENCE)
integrator: test


.PHONY: ads
ads: CC=armcc
ads: CFLAGS = -g --cpu ARM926EJ-S --no_inline -O3 -Otime $(INCLUDE)
ads: TESTDEC=jx170dec_ads
ads: TARGET_ENV=ads
ads: LIBS = -L../../linux/jpeg/$(DECLIB) -L../../linux/dwl/$(DWLLIB) -L../../../system/models/golden/$(MODLIB) -L../common/swhw/$(TBLIB) -D_ADS 
ads: test

.PHONY: ads11
ads11: CC=armcc
ads11: CFLAGS = -g --cpu ARM1136J-S --no_inline -O3 -Otime $(INCLUDE)
ads11: TESTDEC=jx170dec_ads11
ads11: TARGET_ENV=ads11
ads11: LIBS = -L../../linux/jpeg/$(DECLIB) -L../../linux/dwl/$(DWLLIB) -L../../../system/models/golden/$(MODLIB) -L../common/swhw/$(TBLIB) -D_ADS
ads11: test

.PHONY: ads9
ads9: CC=armcc
ads9: CFLAGS = -g --cpu ARM926EJ-S --no_inline -O3 -Otime $(INCLUDE)
ads9: TESTDEC=jx170dec_ads9
ads9: TARGET_ENV=ads9
ads9: LIBS = -L../../linux/jpeg/$(DECLIB) -L../../linux/dwl/$(DWLLIB) -L../../../system/models/golden/$(MODLIB) -L../common/swhw/$(TBLIB) -D_ADS
ads9: test

.PHONY: pclinux
pclinux: CC=gcc
pclinux: CFLAGS += -O -g $(INCLUDE) $(M32) -DLINUX -DEXPIRY_DATE=1$(expiry)
pclinux: TESTDEC=jx170dec_pclinux
pclinux: TARGET_ENV=pclinux
pclinux: LIBS = $(DECLIBDIR) -pthread -lx170j -ldwlx170 -l8170hw -ltbcommon -lutils $(EFENCE)
pclinux: test

.PHONY: verification
verification: SYSTEM_VERIFICATION_TEST = -DSYSTEM_VERIFICATION
verification: CFLAGS+=-DSYSTEM_VERIFICATION
verification: pclinux

.PHONY: pclinux_eval_8170
pclinux_eval_8170: CFLAGS += -DJPEG_EVALUATION_8170
pclinux_eval_8170: pclinux_eval

.PHONY: pclinux_eval_8190
pclinux_eval_8190: CFLAGS += -DJPEG_EVALUATION_8190
pclinux_eval_8190: pclinux_eval

.PHONY: pclinux_eval_9170
pclinux_eval_9170: CFLAGS += -DJPEG_EVALUATION_9170
pclinux_eval_9170: pclinux_eval

.PHONY: pclinux_eval_9190
pclinux_eval_9190: CFLAGS += -DJPEG_EVALUATION_9190
pclinux_eval_9190: pclinux_eval

.PHONY: pclinux_eval_G1
pclinux_eval_G1: CFLAGS += -DJPEG_EVALUATION_G1
pclinux_eval_G1: pclinux_eval

.PHONY: pclinux_eval
pclinux_eval: CC=gcc
pclinux_eval: CFLAGS += $(INCLUDE) $(M32) -DLINUX -DJPEG_EVALUATION -DEXPIRY_DATE=1$(expiry)
pclinux_eval: TESTDEC=jx170dec_pclinux_eval
pclinux_eval: TARGET_ENV=pclinux_eval
pclinux_eval: LIBS = $(DECLIBDIR) -lx170j -ldwlx170 -l8170hw -ltbcommon -lutils $(EFENCE)
pclinux_eval: DEBFLAGS = -O3 -DNDEBUG
pclinux_eval: test

export SYSTEM_VERIFICATION_TEST

test: lib $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(TESTDEC) 

lib:
	make -C ../../linux/jpeg $(TARGET_ENV)
	make -C ../../linux/dwl $(TARGET_ENV)
	make -C ../common/swhw $(TARGET_ENV)
	make -C ../common/utils
        
clean:
	rm -f *.o
	rm -f .depend
	rm -f jx170dec_ads
	rm -f jx170dec_ads9
	rm -f jx170dec_ads11
	rm -f jx170dec_pclinux
	rm -f jx170dec_versatile
	rm -f jx170dec_integrator
	rm -f jx170dec_pclinux_eval	
        
libclean:
	make -C ../../linux/jpeg clean
	make -C ../../linux/dwl clean
	make -C ../common/swhw clean
	make -C ../common/utils clean

depend .depend dep: $(SRCS)
	$(CC) $(CFLAGS) -M  $^ > .depend
        
ifeq (.depend, $(wildcard .depend))
include .depend
endif
