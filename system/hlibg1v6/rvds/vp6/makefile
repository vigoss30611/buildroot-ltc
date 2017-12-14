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
#--  $RCSfile: makefile,v $
#--  $Date: 2011/02/01 07:42:29 $
#--  $Revision: 1.11 $
#--
#-------------------------------------------------------------------------------

# set this to 'n' or 'y' in order to disable/enable debugging
DEBUG = y

# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  CFLAGS   = -g -O0
  DEBFLAGS = -DDEBUG -D_ASSERT_USED -D_ERROR_PRINT
else
  CFLAGS   = -g -O3
  DEBFLAGS = -DNDEBUG
endif

#DEBFLAGS += -DVP6DEC_TRACE # for API tracing (VP6DecTrace has to be implemented)
#DEBFLAGS += -D_DWL_DEBUG # for wrapper layer trace
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
#DEBFLAGS+= -DVP6DEC_EVALUATION=1000

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
CC = $(CROSS_COMPILER)gcc

# MACRO for cleaning object -files
RM  = rm -f

# MACRO for creating library that includes all the object files
AR  = $(CROSS_COMPILER)ar rcs

# show the path to compiler, where to find header files
INCLUDE = -I. -I$(COMMON_SOURCE_DIR)/inc \
          -I$(COMMON_SOURCE_DIR)/vp6 \
          -I$(COMMON_SOURCE_DIR)/config -I$(COMMON_SOURCE_DIR)/common


# extra compiler switches
CFLAGS  += -Wall $(M32)

# list of used sourcefiles
SRC_VP6 :=	regdrv.c \
                vp6booldec.c \
                vp6dec.c \
                vp6huffdec.c \
                vp6hwd_api.c \
                vp6hwd_asic.c \
                vp6strmbuffer.c \
                vp6decodemode.c \
                vp6decodemv.c \
                vp6scanorder.c \
                vp6gconst.c \
		vp6_pp_pipeline.c \
		tiledref.c \
		refbuffer.c \
		bqueue.c



#source search path
vpath %.c
vpath %.c $(COMMON_SOURCE_DIR)/vp6 $(COMMON_SOURCE_DIR)/common

#SRCS = $(SRC_VP6) dwl_x170_vp6_pc.c 

SRCS = $(SRC_VP6)

# object files will be generated from .c sourcefiles
OBJS    := $(SRCS:.c=.o)
OBJDIR  := obj

vpath %.o
vpath %.o $(OBJDIR)

# name of the outputfile (library)
DECLIB = libdec8190vp6.a


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
	@mkdir $@

ARMCPU=ARM1136J-S
ads: CC=armcc
ads: AR=armar -rcs
ads: ARCH= --cpu $(ARMCPU)
ads: CFLAGS = --asm -g -O3 -Otime
ads: DEBFLAGS=
ads: $(DECLIB)

ads11: CC=armcc
ads11: AR=armar -rcs
ads11: ARCH=--cpu ARM1136J-S
ads11: CFLAGS = -g -O3 -Otime
ads11: DEBFLAGS=
ads11: $(DECLIB)

ads9: CC=armcc
ads9: AR=armar -rcs
ads9: ARCH= --cpu ARM926EJ-S
ads9: CFLAGS = -g -O3 -Otime
ads9: DEBFLAGS=
ads9: $(DECLIB)

coverage: CC = covc --abs --retain gcc
coverage: $(DECLIB)

pclinux: DEBFLAGS += $(ASIC_TRACE_SUPPORT)
pclinux: INCLUDE += -I../../test/common/swhw 
pclinux: $(DECLIB)

pclinux_eval: DEBFLAGS = -O3 -DNDEBUG
pclinux_eval: INCLUDE += -I../../test/common/swhw 
pclinux_eval: $(DECLIB)

integrator: ARCH = -mtune=arm9tdmi
integrator: CROSS_COMPILER = arm-linux-
integrator: $(DECLIB)

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
      

$(DECLIB): info .depend $(OBJDIR) $(OBJS)
	@echo -e "\n $(DECLIB) OK\n"
        

$(OBJS): %.o: %.c
	@echo -e "\n [CC]\t $<"
	@$(CC) -c $(CFLAGS) $(ARCH) $(DEBFLAGS) $(INCLUDE) $< -o $(OBJDIR)/$@
	@echo -e "\n [AR]\t $(OBJDIR)/$@"
	@$(AR) $(DECLIB) $(OBJDIR)/$@

clean:
	$(RM) $(DECLIB)
	$(RM) .depend
	$(RM) -r $(OBJDIR)

lint: $(SRC_VP6)
        # -e537 removes warnings about multiple include
        # -e641 removes warnings about converting enum to int
	-lint-nt -w2 -e537 -e641 $(INCLUDE) $^ > pc-lint-report.txt

depend .depend: $(SRCS)
	@echo -e "\n [DEP]\t $^"
	@$(CC) -M $(DEBFLAGS) $(INCLUDE) $^ > .depend

tar:
	tar -cvf dec8190vp6.tar $(DECLIB)
	tar -uvf dec8190vp6.tar -C $(COMMON_SOURCE_DIR)/inc \
                        vp6decapi.h decapicommon.h basetype.h dwl.h

ifeq (,$(findstring clean, $(MAKECMDGOALS)))
ifeq (.depend, $(wildcard .depend))
include .depend
endif
endif
