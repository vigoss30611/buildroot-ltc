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
#--  Abstract : Makefile for x170 PP decoder control SW
#--
#-------------------------------------------------------------------------------
#--
#--  Version control information, please leave untouched.
#--
#--  $RCSfile: Makefile,v $
#--  $Date: 2010/06/30 07:41:21 $
#--  $Revision: 1.17 $
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
#DEBFLAGS += -DPP_TRACE
#DEBFLAGS += -D_DWL_DEBUG # for wrapper layer trace
#DEBFLAGS += -DPP_ASIC_TRACE # trace ASIC data
#DEBFLAGS += -D_DEBUG_PRINT -D_ERROR_PRINT # decoder trace
#DEBFLAGS += -DMEMORY_USAGE_TRACE #trace all memory allocations
#DEBFLAGS += -D_X170_DEMO_DEVEL # Enable demo development functionality


# Postprocessor and decoder pipeline support

#PIPELINE_SUPPORT += -DPP_H264DEC_PIPELINE_SUPPORT # H.264 decoder support
#PIPELINE_SUPPORT += -DPP_MPEG4DEC_PIPELINE_SUPPORT # MPEG4 decoder support
#PIPELINE_SUPPORT += -DPP_JPEGDEC_PIPELINE_SUPPORT # JPEG decoder support
#PIPELINE_SUPPORT += -DPP_VC1DEC_PIPELINE_SUPPORT # VC-1 decoder support
#PIPELINE_SUPPORT += -DPP_MPEG2DEC_PIPELINE_SUPPORT # MPEG2 decoder support
#PIPELINE_SUPPORT += -DPP_RVDEC_PIPELINE_SUPPORT # REAL decoder support

#Set up environment:

COMMON_SOURCE_DIR = ../../source

# Architecture flags for gcc
ARCH =
CROSS_COMPILER =

# C -compiler name, can be replaced by another compiler(replace gcc)
CC ?= $(CROSS_COMPILER)gcc

# show the path to compiler, where to find header files and libraries
INCLUDE = -I. -I$(COMMON_SOURCE_DIR)/inc -I$(COMMON_SOURCE_DIR)/dwl \
          -I$(COMMON_SOURCE_DIR)/mpeg4 -I$(COMMON_SOURCE_DIR)/jpeg \
	  -I$(COMMON_SOURCE_DIR)/vc1  \
	  -I$(COMMON_SOURCE_DIR)/mpeg2  \
	  -I$(COMMON_SOURCE_DIR)/vp6  \
	  -I$(COMMON_SOURCE_DIR)/vp8  \
	  -I$(COMMON_SOURCE_DIR)/avs  \
          -I$(COMMON_SOURCE_DIR)/pp -I$(COMMON_SOURCE_DIR)/config \
          -I$(COMMON_SOURCE_DIR)/common

IS_8190 :=1
ifeq ($(IS_8190),1)
        INCLUDE += -I$(COMMON_SOURCE_DIR)/h264high \
	           -I$(COMMON_SOURCE_DIR)/rv
else
        INCLUDE += -I$(COMMON_SOURCE_DIR)/h264
endif

# compiler switchCFLAGS  =  -Wall -ansi $(ARCH) -D_GNU_SOURCE $(DEBFLAGS) $(INCLUDE) $(M32)
CFLAGS  =  -Wall  $(ARCH) -D_GNU_SOURCE $(DEBFLAGS) $(INCLUDE) $(M32)
# list of used sourcefiles
SRC_PP := ppapi.c \
          ppinternal.c \
          regdrv.c

SRC_PP_TRACE :=

ifeq ($(DEBUG),y)
	SRCS = $(SRC_PP) $(SRC_PP_TRACE)
else
	SRCS = $(SRC_PP)
endif
#source search path
vpath %.c
vpath %.c $(COMMON_SOURCE_DIR)/pp $(COMMON_SOURCE_DIR)/common

vpath %.o
vpath %.o obj

# object files will be generated from .c sourcefiles
OBJS    = $(SRCS:.c=.o)

# name of the outputfile (library)
DECLIB = libg1dec_pp.so

# MACRO for cleaning object -files
RM  = rm -f

# MACRO for creating library that includes all the object files
#AR  = ar rcv
AR  = $(CROSS_COMPILER)ar rcv


OBJDIR ?= obj
LDLIBS := -lg1dec_h264 -lg1dec_jpeg -lg1dec_dwl #-lg1dec_avs  -lg1dec_mpeg2 -lg1dec_mpeg4 -lg1dec_jpeg -lg1dec_vc1 \
		  -lg1dec_rv -lg1dec_vp6 -lg1dec_vp8 -lg1dec_dwl
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
	mkdir $@

ads11: CC=armcc
ads11: AR=armar -rcv
ads11: CFLAGS =  --cpu ARM1136J-S -O2 -Otime $(INCLUDE)
ads11: $(DECLIB)

ads9: CC=armcc
ads9: AR=armar -rcv
ads9: CFLAGS =  --cpu ARM926EJ-S -O2 -Otime $(INCLUDE)
ads9: $(DECLIB)

coverage: CC = covc --abs --retain gcc
coverage: $(DECLIB)

pclinux: CFLAGS += $(ASIC_TRACE_SUPPORT)
pclinux: $(DECLIB)

pclinux_eval: DEBFLAGS = -O3 -DNDEBUG
pclinux_eval: $(DECLIB)

versatile: CROSS_COMPILER = arm-none-linux-gnueabi-
versatile: ARCH = -mtune=arm926ej-s -fpic
versatile: $(DECLIB)

versatile_plain_lib: CROSS_COMPILER = arm-linux-
versatile_plain_lib: ARCH = -mtune=arm926ejs -fpic
versatile_plain_lib: $(DECLIB)

integrator:  $(SRCS)
	$(CC) -shared -fPIC $(CFLAGS) $(ENVSET)  $(PIPELINE_SUPPORT) -o $(LIBDIR)/$(DECLIB) $^ -L$(LIBDIR) $(LDLIBS)

$(DECLIB): .depend $(OBJDIR) $(OBJS)

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $(ENVSET) $(PIPELINE_SUPPORT) $< -o $(OBJDIR)/$@
	#$(AR) $(DECLIB) $(OBJDIR)/$@


.PHONY: clean
clean:
	$(RM) $(DECLIB)
	$(RM) .depend
	$(RM) -r $(OBJDIR)

lint: $(SRC_PP)
        # -e537 removes warnings about multiple include
	-lint-nt -w2 -e537 $(INCLUDE) $(PIPELINE_SUPPORT) -dPP_H264DEC_PIPELINE_SUPPORT -dPP_PIPELINED_DEC_TYPE_MPEG4 -dPP_PIPELINED_DEC_TYPE_JPEG -dPP_VC1DEC_PIPELINE_SUPPORT $^ > pc-lint-report.txt


depend: $(SRCS)
	$(CC) $(CFLAGS) -M  $^ > .depend

.PHONY: ads9 ads11 pclinux pclinux_eval integrator versatile clean lint tar

ifeq (.depend, $(wildcard .depend))
include .depend
endif
