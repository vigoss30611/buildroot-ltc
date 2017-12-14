#-------------------------------------------------------------------------------
#-                                                                            --
#-       This software is confidential and proprietary and may be used        --
#-        only as expressly authorized by a licensing agreement from          --
#-                                                                            --
#-                            Hantro Products Oy.                             --
#-                                                                            --
#-                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
#-                            ALL RIGHTS RESERVED                             --
#-                                                                            --
#-                 The entire notice above must be reproduced                 --
#-                  on all copies and should not be removed.                  --
#-                                                                            --
#-------------------------------------------------------------------------------
#-
#--  Abstract : Makefile for x170 JPEG decoder control SW
#--
#-------------------------------------------------------------------------------
#--
#--  Version control information, please leave untouched.
#--
#--  $RCSfile: Makefile,v $
#--  $Date: 2010/06/30 11:18:56 $
#--  $Revision: 1.10 $
#--
#-------------------------------------------------------------------------------

# Comment/uncomment the following line to disable/enable debugging
DEBUG = y

# Error resilience tool
ERROR_RESILIENCE  = n

# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -DDEBUG -D_ASSERT_USED -D_RANGE_CHECK -D_ERROR_PRINT
else
  DEBFLAGS = -O3 -DNDEBUG
endif

# Add error resilience flag (or not) to CFLAGS
ifeq ($(ERROR_RESILIENCE),y)
  DEBFLAGS += -DJPEGDEC_ERROR_RESILIENCE
endif

#DEBFLAGS += -DJPEGDEC_TRACE # for API tracing (JpegDecTrace has to be implemented)
#DEBFLAGS += -DJPEGDEC_ASIC_TRACE # for JPEG/HW data tracing
#DEBFLAGS += -DJPEGDEC_TRACE_INTERNAL # for JPEG/HW data tracing
#DEBFLAGS += -DJPEGDEC_API_TRC # for control trace
#DEBFLAGS += -D_DEBUG_PRINT -D_ERROR_PRINT # decoder trace
#DEBFLAGS += -DMEMORY_USAGE_TRACE #trace all memory allocations
#DEBFLAGS += -DJPEGDEC_INTEGRATOR # for integrator
#DEBFLAGS += -DJPEGDEC_PP_TRACE # for PP Tracing
#DEBFLAGS += -D_ASSERT_USED

# Define this for performance measurement
#DEBFLAGS += -DJPEGDEC_PERFORMANCE # for performance

# Define this for testing error cases
#DEBFLAGS += -DJPEGDEC_RESET_OUTPUT

COMMON_SOURCE_DIR = ../../source

# Architecture flags for gcc
ARCH =
CROSS_COMPILER =

# C -compiler name, can be replaced by another compiler(replace gcc)
CC = $(CROSS_COMPILER)gcc

# show the path to compiler, where to find header files and libraries
INCLUDE = -I. -I$(COMMON_SOURCE_DIR)/inc -I$(COMMON_SOURCE_DIR)/dwl \
          -I$(COMMON_SOURCE_DIR)/jpeg -I$(COMMON_SOURCE_DIR)/config \
          -I$(COMMON_SOURCE_DIR)/common \
	  -I$(COMMON_SOURCE_DIR)/../linux/memalloc


# compiler switches
CFLAGS  = -pipe -Wall -ansi $(ARCH) -D_GNU_SOURCE $(DEBFLAGS) $(INCLUDE) $(M32)

# list of used sourcefiles
SRC_JPEG :=	regdrv.c\
		jpegdecapi.c\
		jpegdechdrs.c\
		jpegdecinternal.c\
		jpegdecscan.c\
		jpegdecutils.c

SRC_JPEG_TRACE := jpegasicdbgtrace.c

ifeq ($(DEBUG),y)
	SRCS = $(SRC_JPEG) $(SRC_JPEG_TRACE)
else
	SRCS = $(SRC_JPEG)
endif

#source search path
vpath %.c
vpath %.c $(COMMON_SOURCE_DIR)/jpeg $(COMMON_SOURCE_DIR)/common

vpath %.o
vpath %.o obj

# object files will be generated from .c sourcefiles
OBJS    = $(SRCS:.c=.o)

# name of the outputfile (library)
DECLIB = libx170j.a

# MACRO for cleaning object -files
RM  = rm -f

# MACRO for creating library that includes all the object files
#AR  = ar rcv
AR  = $(CROSS_COMPILER)ar rcs

OBJDIR := obj

#Here are rules for building codes and generating object library.
all:
	@echo
	@echo ERROR! run one of:
	@echo "    $$ make integrator"
	@echo "    $$ make versatile"
	@echo "    $$ make pclinux"
	@echo "    $$ make ads"
	@echo "    $$ make pclinux_eval"	
	@echo

$(OBJDIR):
	mkdir $@

.PHONY: ads
ads: CC=armcc
ads: AR=armar -rcv
ads: CFLAGS =  -cpu ARM926EJ-S -O2 -Otime $(INCLUDE)
ads: $(DECLIB)

ads11: CC=armcc
ads11: AR=armar -r -c -v
ads11: CFLAGS = --cpu ARM1136J-S -O2 -Otime $(INCLUDE)
ads11: $(DECLIB)

ads9: CC=armcc
ads9: AR=armar -r -c -v
ads9: CFLAGS = --cpu ARM926EJ-S -O2 -Otime $(INCLUDE)
ads9: $(DECLIB)

pc_plain_lib: OBJS = $(SRC_JPEG:.c=.o)
pc_plain_lib: $(DECLIB)

.PHONY: pclinux
pclinux: CFLAGS += -DPJPEG_COMPONENT_TRACE
pclinux: $(DECLIB)

.PHONY: integrator
integrator: ENVSET  = -DSDRAM_LM_BASE=0x80000000 \
                      -DDEC_MODULE_PATH=\"/dev/hx170dec\" \
                      -DDEC_IO_BASE=0xD0000000
integrator: INCLUDE += -Ildriver/kernel_24x # linux kernel 2.4.x
integrator: ARCH = -mtune=arm9tdmi -fpic
integrator: CROSS_COMPILER = arm-linux-
integrator: $(DECLIB)

.PHONY: versatile
versatile: ENVSET  = -DSDRAM_LM_BASE=0x00000000 \
                     -DDEC_MODULE_PATH=\"/dev/hx170dec\" \
                     -DDEC_IO_BASE=0xC0000000
versatile: CROSS_COMPILER = arm-none-linux-gnueabi-
versatile: ARCH = -mtune=arm926ej-s -fpic
versatile: $(DECLIB)

.PHONY: pclinux_eval
pclinux_eval: INCLUDE += -I../../test/common/swhw/
pclinux_eval: DEBFLAGS = -O3 -DNDEBUG
pclinux_eval: $(DECLIB)
pclinux_eval: CC=gcc


$(DECLIB): depend $(OBJDIR) $(OBJS)

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $(ENVSET) $< -o $(OBJDIR)/$@
	$(AR) $(DECLIB) $(OBJDIR)/$@


.PHONY: clean
clean:
	$(RM) $(DECLIB)
	$(RM) .depend
	$(RM) -r $(OBJDIR)


lint: $(SRC_JPEG)
        # -e537 removes warnings about multiple include
        # -e641 removes warnings about conevrting enum to int
	-lint-nt -w2 -e537 -e641 $(INCLUDE) $^ > pc-lint-report.txt


depend: $(SRCS)
	$(CC) $(CFLAGS) -M  $^ > .depend

ifeq (.depend, $(wildcard .depend))
include .depend
endif
