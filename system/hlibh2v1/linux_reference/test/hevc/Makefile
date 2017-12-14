#-------------------------------------------------------------------------------
#-                                                                            --
#-       This software is confidential and proprietary and may be used        --
#-        only as expressly authorized by a licensing agreement from          --
#-                                                                            --
#-                            Verisilicon.					                          --
#-                                                                            --
#-                   (C) COPYRIGHT 2014 VERISILICON.                          --
#-                            ALL RIGHTS RESERVED                             --
#-                                                                            --
#-                 The entire notice above must be reproduced                 --
#-                  on all copies and should not be removed.                  --
#-                                                                            --
#-------------------------------------------------------------------------------
#-
#--  Abstract : Makefile for encoder testbench
#--
#-------------------------------------------------------------------------------

# Comment/uncomment the following line to disable/enable debugging
DEBUG = n
USE_COVERAGE = n

# Include API extension and test ID for H2 internal testing
INTERNAL_TEST = n

#To create version.h
CREATE_VERSION_H = n
ifeq ($(CREATE_VERSION_H),y)
VERSION_H = version.h
else
VERSION_H =
endif

# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O0 -g -DDEBUG -DTEST_DATA
else
  DEBFLAGS = -O2 -DNDEBUG
endif

#DEBFLAGS+=-DNO_OUTPUT_WRITE

# Architecture flags for gcc
#ARCH = 
#CROSS_COMPILE =

ifeq ($(shell uname -m),x86_64)
	ifneq (,$(findstring pclinux,$(MAKECMDGOALS)))
		export ARCH = -m32
	endif
	ifneq (,$(findstring system,$(MAKECMDGOALS)))
		export ARCH = -m32
	endif
	ifneq (,$(findstring testdata,$(MAKECMDGOALS)))
		export ARCH = -m32
	endif
	ifneq (,$(findstring eval,$(MAKECMDGOALS)))
		export ARCH = -m32
	endif
endif


# the path where to find header files
INCFLAGS = -I../../../inc -I../../../source/hevc \
           -I../../../source/common \
           -I../../debug_trace

CC = $(CROSS_COMPILE_TST)gcc

# compiler switches
CFLAGS  = $(ARCH) -Wall -D_GNU_SOURCE -D_REENTRANT -D_THREAD_SAFE \
           -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 $(DEBFLAGS) $(INCFLAGS)

GCC_GTEQ_440 := $(shell expr `gcc -dumpversion | sed -e 's/\.\([0-9][0-9]\)/\1/g' -e 's/\.\([0-9]\)/0\1/g' -e 's/^[0-9]\{3,4\}$$/&00/'` \>= 40400)
ifeq "$(GCC_GTEQ_440)" "1"
    CFLAGS += -Wno-unused-result
endif

# list of used sourcefiles
SRCS = test_bench_q3.c get_option.c

ifeq ($(INTERNAL_TEST),y)
# for internal tests
SRCS += HEVCencapi_ext.c
CFLAGS += -DINTERNAL_TEST
endif

ifeq ($(USE_COVERAGE), y)
  CFLAGS += -coverage -fprofile-arcs -ftest-coverage
endif

vpath %.c
vpath %.c ../../../source/hevc 

OBJS = $(SRCS:.c=.o)

# name of the control code library 
LIB = ../../libh2enc.a

# System model library
MODELLIB = ../../../../system/models/ench2_asic_model.a

# name of the output executable
TARGET = hevc_testenc_q3

# MACRO for cleaning object -files
RM  = rm -f


#Here are rules for building codes and generating executable
all:    $(VERSION_H) tags
	@echo ---------------------------------------
	@echo "Usage: make [ system | eval | versatile ]"
	@echo "system     - PC system model (== pclinux)"
	@echo "eval       - PC system model for evaluation with frame limit"
	@echo "integrator - ARM integrator with FPGA HW"
	@echo "versatile  - ARM versatile with FPGA HW"
	@echo "NOTE! Make sure to do 'make libclean'"
	@echo "between compiling to different targets!"
	@echo ---------------------------------------

# Use other makefiles to build the libraries
$(MODELLIB):
	$(MAKE) -w -C ../../../../system/models

$(LIB):
	$(MAKE) -w -C ../.. $(TARGETENV)

version.h:../../../../.git/HEAD ../../../../.git/index
	echo "const char *git_version = \"$(shell git rev-parse HEAD)\";" > $@



.PHONY: eval
evaluation: eval
eval: TARGET = hevc_enc_h2_eval
eval: TARGETENV = eval
eval: CFLAGS += -DEVALUATION_LIMIT=1000 -DPSNR
eval: $(MODELLIB) $(LIB) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIB) $(MODELLIB) -lm -o $(TARGET)

pclinux: system

.PHONY: system
system: TARGETENV = system
system: CFLAGS += -DPSNR
system: $(VERSION_H) .depend $(MODELLIB) $(LIB) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIB) $(MODELLIB) -lm -o $(TARGET)

.PHONY: system_multifile
system_multifile: TARGET = hevc_testenc_multifile
system_multifile: TARGETENV = system
system_multifile: CFLAGS += -DPSNR
system_multifile: CFLAGS += -DMULTIFILEINPUT
system_multifile: $(VERSION_H) .depend $(MODELLIB) $(LIB) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIB) $(MODELLIB) -lm -o $(TARGET)

.PHONY: system_static
system_static: TARGETENV = system
system_static: CFLAGS += -DPSNR
system_static: $(VERSION_H) .depend $(MODELLIB) $(LIB) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIB) $(MODELLIB) -lm -o $(TARGET) -static

.PHONY: versatile
versatile: TARGETENV = versatile
versatile: CROSS_COMPILE_TST = ../../../../../../../prebuilts/uclibc/bin/arm-buildroot-linux-uclibcgnueabihf-
versatile: ARCH = -mcpu=generic-armv7-a -mtune=generic-armv7-a
versatile: LIB = -lh2enc -lpthread -lInfotmMediaBufferAlloc -lInfotmMediaION  -L../../../../../../../sportdv/release_arm/lib_static \
					-L../../../../../../../sportdv/release_arm/infomedia/lib 
versatile: $(VERSION_H) $(OBJS)
	$(MAKE) -w -C ../.. $@ \
            CROSS_COMPILE=$(CROSS_COMPILE_TST) ARCH="$(ARCH)"
	$(CC) $(CFLAGS) $(OBJS) $(LIB) -lm -o $(TARGET) 
	mkdir -p ../../../../../../../sportdv/image/rootfs_target_fs/opt/ipnc
	cp $(TARGET) ../../../../../../../sportdv/image/rootfs_target_fs/opt/ipnc

.PHONY: versatile_multifile
versatile_multifile: TARGETENV = versatile
versatile_multifile: CROSS_COMPILE = arm-none-linux-gnueabi-
versatile_multifile: ARCH = -mcpu=arm926ej-s -mtune=arm926ej-s
versatile_multifile: LIB += -lpthread
versatile_multifile: CFLAGS += -DMULTIFILEINPUT
versatile_multifile: TARGET = hevc_testenc_multifile
versatile_multifile: $(OBJS)
	$(MAKE) -w -C ../.. versatile \
            CROSS_COMPILE=$(CROSS_COMPILE) ARCH="$(ARCH)"
	$(CC) $(CFLAGS) $(OBJS) $(LIB) -o $(TARGET)

.PHONY: integrator
integrator: TARGETENV = integrator
integrator: CROSS_COMPILE= arm-linux-
integrator: ARCH = -mcpu=arm9tdmi -mtune=arm9tdmi
integrator: LIB += -lpthread
integrator: $(OBJS)
	$(MAKE) -w -C ../.. $@ \
            CROSS_COMPILE=$(CROSS_COMPILE) ARCH="$(ARCH)"
	$(CC) $(CFLAGS) $(OBJS) $(LIB) -o $(TARGET)

.PHONY: pci
pci: TARGETENV = pci
pci: CROSS_COMPILE=
pci: ARCH=
pci: CFLAGS+=  -DSDRAM_LM_BASE=$(CHW_BASE_ADDRESS) \
               -DENC_MODULE_PATH=\"/tmp/dev/hx280\" \
               -DMEMALLOC_MODULE_PATH=\"/tmp/dev/memalloc\"
pci: $(VERSION_H) .depend $(LIB) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIB) -lm -o $(TARGET)


system_cov: CC = covc --retain -t!HEVCTestBench.c,!EncGetOption.c g++
system_cov: TARGETENV = system_cov
system_cov: $(MODELLIB) $(LIB) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIB) $(MODELLIB) -o $(TARGET)

        
.PHONY: clean
clean:
	$(RM) .depend
	$(RM) *.o *.gcno *.gcda core* *~ $(TARGET) $(TARGET).* $(VERSION_H)

.PHONY: libclean
libclean: clean
	$(MAKE) -w -C ../.. clean

.PHONY: tags
tags:
	ctags ../../../inc/*.h *.c ../../../source/hevc/*.[ch] \
            ../../../source/common/*.[ch] ../../ewl/*c
            
depend .depend: $(SRCS)
	@echo -e "\n [DEP]\t $^"
	@$(CC) -M $(DEBFLAGS) $(INCFLAGS) $^ > .depend

ifeq (,$(findstring clean, $(MAKECMDGOALS)))
ifeq (.depend, $(wildcard .depend))
include .depend
endif
endif

