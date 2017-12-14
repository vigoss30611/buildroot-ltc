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
#--  Abstract : Makefile for x170 MPEG4 decoder control SW
#--
#-------------------------------------------------------------------------------
#--
#--  Version control information, please leave untouched.
#--
#--  $RCSfile: Makefile,v $
#--  $Date: 2010/12/01 12:36:13 $
#--  $Revision: 1.23 $
#--
#-------------------------------------------------------------------------------

# Comment/uncomment the following line to disable/enable debugging
DEBUG = n

# Add your debugging flags (or not) to CFLAGS. Do not change these!
ifeq ($(DEBUG),y)
  DEBFLAGS = -DDEBUG -D_ASSERT_USED
else
  DEBFLAGS = -O3 -DNDEBUG
endif

# Add your debugging flag (or not) to CFLAGS. Enable, if needed
#DEBFLAGS += -DMP4DEC_TRACE
#DEBFLAGS += -D_DWL_DEBUG # for wrapper layer trace
#DEBFLAGS += -DMP4_ASIC_TRACE # trace ASIC data
#DEBFLAGS += -D_DEBUG_PRINT -D_ERROR_PRINT -D_MPEG4APITRACE -D_DEC_PP_USAGE  # decoder trace
#DEBFLAGS += -DMEMORY_USAGE_TRACE #trace all memory allocations
#DEBFLAGS += -D_X170_DEMO_DEVEL # Enable demo development functionality

#Set up environment:

#Output mode can be: 
# DEC_X170_OUTPUT_FORMAT_RASTER_SCAN
# DEC_X170_OUTPUT_FORMAT_TILED

#SETTING_FLAGS = -DDEC_X170_OUTPUT_FORMAT=DEC_X170_OUTPUT_FORMAT_TILED
SETTING_FLAGS = -DDEC_X170_OUTPUT_FORMAT=DEC_X170_OUTPUT_FORMAT_RASTER_SCAN

#Set RLC buffer size / MB. 384 is the theoretical maximum. H.264 uses 220/MB.
#MAX value 384
#Good compromize value 220

SETTING_FLAGS += -D_MP4_RLC_BUFFER_SIZE=384
#SETTING_FLAGS += -D_MP4_RLC_BUFFER_SIZE=220

COMMON_SOURCE_DIR = ../../source

# Architecture flags for gcc
ARCH =
CROSS_COMPILER =

# C -compiler name, can be replaced by another compiler(replace gcc)
CC ?= $(CROSS_COMPILER)gcc

# show the path to compiler, where to find header files and libraries
INCLUDE = -I. -I$(COMMON_SOURCE_DIR)/inc -I$(COMMON_SOURCE_DIR)/dwl \
          -I$(COMMON_SOURCE_DIR)/mpeg4 -I$(COMMON_SOURCE_DIR)/config \
          -I$(COMMON_SOURCE_DIR)/common

# compiler switches
CFLAGS  =  -Wall $(ARCH) -D_GNU_SOURCE $(DEBFLAGS) $(INCLUDE) \
$(SETTING_FLAGS) $(M32)

# list of used sourcefiles
SRC_MPEG4 := \
	mp4dechwd_error_conceal.c \
	mp4decapi.c \
	mp4decapi_internal.c \
	mp4dechwd_rvlc.c \
	mp4dechwd_strmdec.c \
	mp4dechwd_headers.c \
	mp4dechwd_motiontexture.c \
	mp4dechwd_shortvideo.c \
	mp4dechwd_utils.c \
	mp4dechwd_videopacket.c \
	mp4dechwd_vlc.c \
	mp4dechwd_vop.c \
	regdrv.c \
	tiledref.c \
	refbuffer.c \
	workaround.c \
	bqueue.c

ifeq ($(CUSTOM_FMT_SUPPORT),y)
	SRC_MPEG4 += \
		mp4dechwd_custom.c
else
	SRC_MPEG4 += \
		mp4dechwd_generic.c
endif

SRC_MPEG4_TRACE := mpeg4asicdbgtrace.c

ifeq ($(DEBUG),y)
	SRCS = $(SRC_MPEG4) $(SRC_MPEG4_TRACE)
else
	SRCS = $(SRC_MPEG4)
endif
#source search path
vpath %.c
vpath %.c $(COMMON_SOURCE_DIR)/mpeg4 $(COMMON_SOURCE_DIR)/common

vpath %.o
vpath %.o $(OBJDIR)

# object files will be generated from .c sourcefiles
OBJS    = $(SRCS:.c=.o)

# name of the outputfile (library)
DECLIB = libg1dec_mpeg4.so

# MACRO for cleaning object -files
RM  = rm -f

# MACRO for creating library that includes all the object files
#AR  = ar rcv
AR  = $(CROSS_COMPILER)ar rcv


OBJDIR ?= obj

#Here are rules for building codes and generating object library.
all:
	@echo
	@echo ERROR! run one of:
	@echo "    $$ make integrator"
	@echo "    $$ make versatile"
	@echo "    $$ make pclinux"
	@echo "    $$ make pc_plain_lib"
	@echo "    $$ make ads"
	@echo "    $$ make pclinux_eval"	
	@echo

$(OBJDIR):
	mkdir -p $@

ads11: CC=armcc
ads11: AR=armar -rcv
ads11: CFLAGS = --cpu ARM1136J-S -O2 -Otime $(INCLUDE)
ads11: $(DECLIB)

ads9: CC=armcc
ads9: AR=armar -rcv
ads9: CFLAGS = --cpu ARM926EJ-S -O2 -Otime $(INCLUDE)
ads9: $(DECLIB)

coverage: CC = covc --abs --retain gcc
coverage: $(DECLIB)

pclinux: CFLAGS += -DASIC_TRACE_SUPPORT
pclinux: INCLUDE += -I../../test/common/swhw/
pclinux: $(DECLIB)
pclinux: CC=gcc

versatile: CROSS_COMPILER = arm-none-linux-gnueabi-
versatile: ARCH = -mtune=arm926ej-s -fpic
versatile: $(DECLIB)

versatile_plain_lib: CROSS_COMPILER = arm-linux-
versatile_plain_lib: ARCH = -mtune=arm926ejs -fpic
versatile_plain_lib: $(DECLIB)

pclinux_eval: INCLUDE += -I../../test/common/swhw/
pclinux_eval: DEBFLAGS = -O3 -DNDEBUG
pclinux_eval: $(DECLIB)
pclinux_eval: CC=gcc

integrator:  $(SRCS)
	$(CC) -shared -fPIC $(CFLAGS) $(ENVSET) -o $(LIBDIR)/$(DECLIB) $^ -L$(LIBDIR) -lg1dec_dwl

$(DECLIB): .depend $(OBJDIR) $(OBJS)

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $(ENVSET) $< -o $(OBJDIR)/$@
	#$(AR) $(DECLIB) $(OBJDIR)/$@

.PHONY: clean
clean:
	$(RM) $(DECLIB)
	$(RM) .depend
	$(RM) -r $(OBJDIR)

lint: $(SRC_MPEG4)
        # -e537 removes warnings about multiple include
        # -e641 removes warnings about conevrting enum to int
	-lint-nt -w2 -e537 -e641 $(INCLUDE) $^ > pc-lint-report.txt


depend: $(SRCS)
	$(CC) $(CFLAGS) -M  $^ > .depend


.PHONY: ads9 ads11 pclinux integrator versatile pclinux_eval clean lint tar

ifeq (.depend, $(wildcard .depend))
include .depend
endif
