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
#--  Abstract : Makefile for 6170 H.264 decoder control SW
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
DEBUG = y

# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  CFLAGS   = $(M32) -O0
  DEBFLAGS = -DDEBUG -D_ASSERT_USED -D_RANGE_CHECK -D_ERROR_PRINT
else
  CFLAGS   = $(M32) 
  DEBFLAGS = -DNDEBUG
endif

#DEBFLAGS += -DH264DEC_TRACE # for API tracing (H264DecTrace has to be implemented)
#DEBFLAGS += -D_DEBUG_PRINT # decoder trace
#DEBFLAGS += -D_ERROR_PRINT # decoder error prints
#DEBFLAGS += -DMEMORY_USAGE_TRACE #trace all memory allocations
#DEBFLAGS += -DTRACE_START_MARKER # logic analiser start marker generation
#DEBFLAGS += -DTRACE_PP_CTRL # trace PP control in DEC software

# define the decoder output format
DEBFLAGS += -DDEC_X170_OUTPUT_FORMAT=0 # raster scan output
#DEBFLAGS += -DDEC_X170_OUTPUT_FORMAT=1 # tiled output

# Set length of SW timeout in milliseconds. default: infinite wait (-1)
# This is just a parameter passed to the wrapper layer, so the real
# waiting happens there and is based on that impelmentation
DEBFLAGS+= -DDEC_X170_TIMEOUT_LENGTH=-1

# Set evaluation limit in number of decoded frames
#DEBFLAGS+= -DH264DEC_EVALUATION=1000

# Define the empty picture data TESTING ONLY!!!!!
ifneq ($(SET_EMPTY_PICTURE_DATA),)
        DEBFLAGS += -DSET_EMPTY_PICTURE_DATA=$(SET_EMPTY_PICTURE_DATA)
endif
        

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

# show the path to compiler, where to find header files
INCLUDE = -I. -I$(COMMON_SOURCE_DIR)/h264high -I$(COMMON_SOURCE_DIR)/dwl \
          -I$(COMMON_SOURCE_DIR)/inc -I$(COMMON_SOURCE_DIR)/h264high/legacy \
          -I$(COMMON_SOURCE_DIR)/config -I$(COMMON_SOURCE_DIR)/common


# extra compiler switches
CFLAGS  += -Wall -pedantic

# list of used sourcefiles
SRC_H264 :=	h264decapi.c \
		h264hwd_asic.c \
	        h264hwd_intra_prediction.c \
	        h264hwd_inter_prediction.c \
        	h264hwd_util.c \
        	h264hwd_byte_stream.c \
	        h264hwd_seq_param_set.c \
        	h264hwd_pic_param_set.c \
	        h264hwd_slice_header.c \
	        h264hwd_slice_data.c \
	        h264hwd_macroblock_layer.c \
	        h264hwd_stream.c \
	        h264hwd_vlc.c \
	        h264hwd_cavlc.c \
	        h264hwd_nal_unit.c \
	        h264hwd_neighbour.c \
	        h264hwd_storage.c \
	        h264hwd_slice_group_map.c \
	        h264hwd_dpb.c \
	        h264hwd_vui.c \
	        h264hwd_pic_order_cnt.c \
	        h264hwd_decoder.c \
	        h264hwd_conceal.c \
                h264_pp_pipeline.c \
                h264hwd_cabac.c \
		regdrv.c \
		refbuffer.c \
		tiledref.c \
		h264decapi_e.c

#source search path
vpath %.c
vpath %.c $(COMMON_SOURCE_DIR)/h264high \
          $(COMMON_SOURCE_DIR)/h264high/legacy \
	  $(COMMON_SOURCE_DIR)/common

SRCS = $(SRC_H264)

# object files will be generated from .c sourcefiles
OBJS    := $(SRCS:.c=.o)
OBJDIR  ?= obj

vpath %.o
vpath %.o $(OBJDIR)

# name of the outputfile (library)
DECLIB = libg1dec_h264.so 


.PHONY: ads pclinux pclinux_eval integrator versatile clean lint tar

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
	@mkdir -p $@

ARMCPU=ARM1136J-S
ads: CC=armcc
ads: AR=armar -rcs
ads: ARCH= --cpu $(ARMCPU)
ads: CFLAGS =  -Otime
ads: DEBFLAGS=
ads: $(DECLIB)

ads11: CC=armcc
ads11: AR=armar -rcs
ads11: ARCH=--cpu ARM1136J-S
ads11: CFLAGS =  -Otime
ads11: DEBFLAGS=
ads11: $(DECLIB)

ads9: CC=armcc
ads9: AR=armar -rcs
ads9: ARCH= --cpu ARM926EJ-S
ads9: CFLAGS =  -Otime
ads9: DEBFLAGS=
ads9: $(DECLIB)

coverage: CC = covc --abs --retain gcc
coverage: $(DECLIB)

pclinux: DEBFLAGS += $(ASIC_TRACE_SUPPORT)
pclinux: INCLUDE += -I../../test/common/swhw/
pclinux: $(DECLIB)

pclinux_eval: INCLUDE += -I../../test/common/swhw/
pclinux_eval: DEBFLAGS = -O3 -DNDEBUG
pclinux_eval: $(DECLIB)
pclinux_eval: CC=gcc

versatile: CROSS_COMPILER = arm-none-linux-gnueabi-
versatile: ARCH = -mtune=arm926ej-s
versatile: $(DECLIB)

info:
	@echo ""
	@echo -e " ARCH     = $(strip $(ARCH))"
	@echo -e " CFLAGS   = $(strip $(CFLAGS))"
	@echo -e " DEFINE   = "        
	@for D in $(strip $(DEBFLAGS:-D%=%)) ; do echo -e   "            $$D" ; done        
	@echo -e " INCLUDE  = "        
	@for I in $(strip $(INCLUDE)) ; do echo -e   "            $$I" ; done
      

$(DECLIB): CFLAGS += $(ENABLE_2ND_CHROMA) $(DISABLE_PIC_FREEZE)
        
integrator:  $(SRCS)
	$(CC) -shared -fPIC $(CFLAGS) $(ARCH) $(DEBFLAGS) $(INCLUDE) -o $(LIBDIR)/$(DECLIB) $^ -L$(LIBDIR) -lg1dec_dwl

$(DECLIB): info .depend $(OBJDIR) $(OBJS)
	@echo -e "\n $(DECLIB) OK\n"

$(OBJS): %.o: %.c
	@echo -e "\n [CC]\t $<"
	@$(CC) -c $(CFLAGS) $(ARCH) $(DEBFLAGS) $(INCLUDE) $< -o $(OBJDIR)/$@
	#@echo -e "\n [AR]\t $(OBJDIR)/$@"
	#@$(AR) $(DECLIB) $(OBJDIR)/$@

clean:
	$(RM) $(DECLIB)
	$(RM) .depend
	$(RM) -r $(OBJDIR)

lint: $(SRC_H264)
        # -e537 removes warnings about multiple include
        # -e641 removes warnings about converting enum to int
	-lint-nt -w2 -e537 -e641 $(INCLUDE) $^ > pc-lint-report.txt

depend .depend: $(SRCS)
	@echo -e "\n [DEP]\t $^"
	@$(CC) -M $(DEBFLAGS) $(INCLUDE) $^ > .depend

tar:
	tar -cvf decx170h.tar $(DECLIB)
	tar -uvf decx170h.tar -C $(COMMON_SOURCE_DIR)/inc \
                        h264decapi.h decapicommon.h basetype.h dwl.h

ifeq (,$(findstring clean, $(MAKECMDGOALS)))
ifeq (.depend, $(wildcard .depend))
include .depend
endif
endif
