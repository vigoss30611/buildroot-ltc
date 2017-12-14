#-------------------------------------------------------------------------------
#--                                                                           --
#--       This software is confidential and proprietary and may be used       --
#--        only as expressly authorized by a licensing agreement from         --
#--                                                                           --
#--                            Hantro Products Oy.                            --
#--                                                                           --
#--      In the event of publication, the following notice is applicable:     --
#--                                                                           --
#--                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                   --
#--                            ALL RIGHTS RESERVED                            --
#--                                                                           --
#--         The entire notice above must be reproduced on all copies.         --
#--                                                                           --
#-------------------------------------------------------------------------------
#--
#--  Abstract : Makefile for the common test bench sources
#--
#-------------------------------------------------------------------------------

# set this to 'n' or 'y' in order to disable/enable debugging
DEBUG = y

# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DDEBUG -D_ASSERT_USED
else
  DEBFLAGS = -O3 -DNDEBUG
endif

# Architecture flags for gcc
ARCH=
CROSS=

# C -compiler name, can be replaced by another compiler(replace gcc)
CC=$(CROSS)gcc

# MACRO for cleaning object -files
RM  = rm -f

# MACRO for creating library that includes all the object files
AR=$(CROSS)ar rcv

# show the path to compiler, where to find header files and libraries
INCLUDE = -I.\
                 -I../../../source/inc/\
		 -I../../../source/config\
		 -I../../../source/common

# compiler switches
CFLAGS  = -Wall -ansi $(ARCH) -D_GNU_SOURCE $(DEBFLAGS) $(INCLUDE) $(M32) -D_FILE_OFFSET_BITS=64
CFLAGS += -DAVS_PLUS_SUPPORTED=1

ifneq ($(YUV_COMBINED),y)
        CFLAGS += -DLUMA_CHROMA_SEPERATED
endif

# list of used sourcefiles
SRCS =  tb_stream_corrupt.c \
        tb_params.c \
        tb_cfg.c \
        tb_md5.c \
        md5.c \
        tb_tiled.c

# HW internal tracing not possible on Versatile
ifeq (,$(findstring versatile,$(MAKECMDGOALS)))
	ifneq ($(VP6),1)
        SRCS += trace.c
	endif
endif

vpath %.c

vpath %.o
vpath %.o obj

# object files will be generated from .c sourcefiles
OBJS = $(SRCS:.c=.o)

# name of the outputfile (library)
LIB = libtbcommon.a

OBJDIR = obj

#Here are rules for building codes and generating object library.
all:
	@echo
	@echo ERROR! run one of:
	@echo "    $$ make versatile"
	@echo "    $$ make pclinux"
	@echo "    $$ make pclinux_eval"	
	@echo "    $$ make coverage"
	@echo

$(OBJDIR):
	mkdir $@

pclinux: $(LIB)
coverage: $(LIB)

ARMCPU=ARM1136J-S
.PHONY: ads
ads: CC=armcc
ads: AR=armar -rcs
ads: CFLAGS = --cpu $(ARMCPU) -g --gnu -O2 -Otime $(INCLUDE)
ads: $(LIB)

ads9: CC=armcc
ads9: AR=armar -rcs
ads9: CFLAGS = --cpu ARM926EJ-S -g --gnu -O2 -Otime $(INCLUDE)
ads9: $(LIB)

ads11: CC=armcc
ads11: AR=armar -rcs
ads11: CFLAGS = --cpu ARM1136J-S -g --gnu -O2 -Otime $(INCLUDE)
ads11: $(LIB)

.PHONY: pclinux_eval
pclinux_eval: DEBFLAGS = -O3 -DNDEBUG
pclinux_eval: $(LIB)

#coverage: CC=covc --abs --retain gcc
coverage: pclinux

versatile: CROSS = arm-none-linux-gnueabi-
versatile: ARCH = -mtune=arm926ej-s
versatile: $(LIB)

linaro: CROSS = arm-linux-gnueabi-
linaro: ARCH = -march=armv7-a
linaro: $(LIB)

$(LIB): .depend $(OBJDIR) $(OBJS)
	$(AR) $(LIB) $(patsubst %,$(OBJDIR)/%, $(OBJS))

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $(ENVSET) $< -o $(OBJDIR)/$@

clean:
	$(RM) $(LIB)
	$(RM) .depend
	$(RM) -r $(OBJDIR)

depend .depend: $(SRCS)
	$(CC) $(CFLAGS) -M  $^ > .depend

.PHONY: pclinux pclinux_eval versatile clean coverage

ifeq (,$(findstring clean, $(MAKECMDGOALS)))
ifeq (.depend, $(wildcard .depend))
include .depend
endif
endif
