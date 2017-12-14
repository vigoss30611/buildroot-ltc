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
#--  Abstract : Makefile for 7170 VC-1 decoder control SW
#--
#-------------------------------------------------------------------------------
#--
#--  Version control information, please leave untouched.
#--
#--  $RCSfile: Makefile,v $
#--  $Date: 2010/12/01 12:36:13 $
#--  $Revision: 1.11 $
#--
#-------------------------------------------------------------------------------

# set this to 'n' or 'y' in order to disable/enable debugging
DEBUG = n

# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -DDEBUG -D_ASSERT_USED -D_RANGE_CHECK -D_ERROR_PRINT
else
  DEBFLAGS = -O3 -DNDEBUG
endif

#DEBFLAGS += -D_VC1DEC_TRACE # for API tracing (VC1DecTrace has to be implemented)
#DEBFLAGS += -D_DWL_DEBUG # for wrapper layer trace
#DEBFLAGS += -DVC1_ASIC_TRACE # trace ASIC data
#DEBFLAGS += -DTRACE_START_MARKER # logic analiser start marker generation
#DEBFLAGS += -D_DEBUG_PRINT -D_ERROR_PRINT # decoder trace
#DEBFLAGS += -DMEMORY_USAGE_TRACE #trace all memory allocations

# define the decoder output format
DEBFLAGS += -DDEC_X170_OUTPUT_FORMAT=0 # raster scan output
#DEBFLAGS += -DDEC_X170_OUTPUT_FORMAT=1 # tiled output


# system independent code location
COMMON_SOURCE_DIR = ../../source

# Architecture flags for gcc
ARCH =
CROSS_COMPILER =

# C -compiler name, can be replaced by another compiler(replace gcc)
CC ?= $(CROSS_COMPILER)gcc

# MACRO for cleaning object -files
RM  = rm -f

# MACRO for creating library that includes all the object files
AR  = $(CROSS_COMPILER)ar rcv

# show the path to compiler, where to find header files and libraries
INCLUDE = -I. -I$(COMMON_SOURCE_DIR)/inc -I$(COMMON_SOURCE_DIR)/dwl \
          -I$(COMMON_SOURCE_DIR)/vc1 -I$(COMMON_SOURCE_DIR)/config \
          -I$(COMMON_SOURCE_DIR)/common


# compiler switches
CFLAGS  = -Wall  $(ARCH) -D_GNU_SOURCE $(DEBFLAGS) $(INCLUDE) $(M32)

# list of used sourcefiles
SRC_VC1 :=	vc1decapi.c \
		    vc1hwd_asic.c \
	        vc1hwd_bitplane.c \
	        vc1hwd_decoder.c \
	        vc1hwd_picture_layer.c \
	        vc1hwd_stream.c \
	        vc1hwd_vlc.c \
		vc1hwd_headers.c \
		tiledref.c \
		regdrv.c \
		refbuffer.c \
		bqueue.c 

#source search path
vpath %.c
vpath %.c $(COMMON_SOURCE_DIR)/vc1 $(COMMON_SOURCE_DIR)/common

vpath %.o
vpath %.o $(OBJDIR)

SRCS = $(SRC_VC1)

# object files will be generated from .c sourcefiles
OBJS    = $(SRCS:.c=.o)

# name of the outputfile (library)
DECLIB = libg1dec_vc1.so

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


ads: CC=armcc
ads: AR=armar -rcv
ads: CFLAGS = --cpu ARM1136J-S -O2 -Otime $(INCLUDE)
ads: $(DECLIB)

ads9: CC=armcc
ads9: AR=armar -rcv
ads9: CFLAGS = --cpu ARM926EJ-S -O2 -Otime $(INCLUDE)
ads9: $(DECLIB)

pclinux: CFLAGS += $(ASIC_TRACE_SUPPORT)
pclinux: INCLUDE += -I../../test/common/swhw/
pclinux: $(DECLIB)

pclinux_eval: INCLUDE += -I../../test/common/swhw/
pclinux_eval: DEBFLAGS = -O3 -DNDEBUG
pclinux_eval: $(DECLIB)
pclinux_eval: CC=gcc

versatile: CROSS_COMPILER = arm-none-linux-gnueabi-
versatile: ARCH = -mtune=arm926ej-s -fpic
versatile: $(DECLIB)

versatile_plain_lib: CROSS_COMPILER = arm-linux-
versatile_plain_lib: ARCH = -mtune=arm926ejs -fpic
versatile_plain_lib: $(DECLIB)

integrator:  $(SRCS)
	$(CC) -shared -fPIC $(CFLAGS) $(ENVSET) -o $(LIBDIR)/$(DECLIB) $^ -L$(LIBDIR) -lg1dec_dwl

$(DECLIB): .depend $(OBJDIR) $(OBJS)

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $(ENVSET) $< -o $(OBJDIR)/$@
	$(AR) $(DECLIB) $(OBJDIR)/$@

clean:
	$(RM) $(DECLIB)
	$(RM) .depend
	$(RM) -r $(OBJDIR)

lint: $(SRC_VC1)
        # -e537 removes warnings about multiple include
        # -e641 removes warnings about conevrting enum to int
	-lint-nt -w2 -e537 -e641 $(INCLUDE) $^ > pc-lint-report.txt

depend .depend: $(SRCS)
	$(CC) $(CFLAGS) -M  $^ > .depend

tar:
	tar -cvf decx170v.tar $(DECLIB)
	tar -uvf decx170v.tar -C $(COMMON_SOURCE_DIR)/inc vc1decapi.h basetype.h


.PHONY: ads pclinux pclinux_eval integrator versatile clean lint tar

ifeq (,$(findstring clean, $(MAKECMDGOALS)))
ifeq (.depend, $(wildcard .depend))
include .depend
endif
endif
