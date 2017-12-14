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
#--  Abstract : Makefile for encoder library
#--
#-------------------------------------------------------------------------------

# Comment/uncomment the following lines to define which control codes to 
# include in the library build
INCLUDE_HEVC = y
INCLUDE_VP9 = n
USE_COVERAGE = n


# Include internal testing with testId-parameter
INCLUDE_TESTING = n

ifneq (,$(findstring system, $(MAKECMDGOALS)))
# Include internal testing for compressed recon frame.
TRACE_COMPRESSED_RECON = y
endif

ifeq (pci,$(findstring pci, $(MAKECMDGOALS)))
# this is just for DEMO
include Baseaddress
endif

# Comment/uncomment the following line to disable/enable debugging
DEBUG=n
TRACE=y

 # don't use IRQs
POLLING = n



# Add your debugging flags (or not)
ifeq ($(DEBUG),y)
  DEBFLAGS = -O0 -g -DDEBUG -D_DEBUG_PRINT -D_ASSERT_USED -Werror -Wno-unused
else
  DEBFLAGS = -O2 -DNDEBUG -Werror -Wno-unused
endif

ifeq ($(TRACE),y)
  TRACEFLAGS = -DTEST_DATA
else
  TRACEFLAGS = 
endif

# Architecture flags for gcc
#ARCH= 
#CROSS_COMPILE=

# C -compiler name, can be replaced by another compiler(replace gcc)
#CC = $(CROSS_COMPILE)gcc
CC = ../../../../../prebuilts/uclibc/bin/arm-buildroot-linux-uclibcgnueabihf-gcc

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

# System model library
MODELLIB = ../../system/models/ench2_asic_model.a

# compiler switches
CFLAGS  = $(ARCH) -Os -Wall -D_GNU_SOURCE -D_REENTRANT -D_THREAD_SAFE \
          $(DEBFLAGS) $(INCLUDE)
CFLAGS += -Wsign-compare -Wmissing-field-initializers -std=c99

GCC_GTEQ_435 := $(shell expr `gcc -dumpversion | sed -e 's/\.\([0-9][0-9]\)/\1/g' -e 's/\.\([0-9]\)/0\1/g' -e 's/^[0-9]\{3,4\}$$/&00/'` \>= 40305)
ifeq "$(GCC_GTEQ_435)" "1"
    CFLAGS += -Wempty-body -Wtype-limits
endif

# Encoder compiler flags

CFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE

ifeq ($(USE_COVERAGE), y)
  CFLAGS += -coverage -fprofile-arcs -ftest-coverage
endif

# show the path to compiler, where to find header files
INCLUDE = -I../source/hevc -I../source/vp9 -I../inc -I../source/common -Iewl \
           -Ikernel_module -Imemalloc \
		   -I./../../../infomedia/include/ \
          -Idebug_trace 



# list of used sourcefiles
SRC_ENC_COMMON := encasiccontroller_v2.c \
                  encasiccontroller.c \
                  queue.c \
            	    sw_put_bits.c \
                  tools.c \
                  encpreprocess.c \
                  error.c

SRC_HEVC := hevcencapi.c\
            sw_picture.c \
            sw_parameter_set.c \
            rate_control_picture.c \
            sw_nal_unit.c \
            sw_slice.c \
            hevcSei.c 


SRC_VP9  := vp9codeframe.c\
            vp9init.c\
            vp9putbits.c\
            vp9header.c\
            vp9picturebuffer.c\
            vp9picparameterset.c\
            vp9entropy.c\
            vp9macroblocktools.c\
            vp9ratecontrol.c\
            vp9encapi.c




SRC_TRACE = enctrace.c
ifeq (pci,$(findstring pci, $(MAKECMDGOALS)))
ifeq ($(TRACE_COMPRESSED_RECON), y)
SRC_TRACE += ../../system/models/video/common/compressor.c
SRC_TRACE += ../../system/models/video/common/compressor_block.c
INCLUDE += -I../../system/models/video/common -I../../system/models/video/hevc -I../../system/models/ewl
endif
endif
#test_data.c

# Source files for test case specific test IDs, compiler flag INTERNAL_TEST
# If these are not included some tests will fail

SRC_TESTING = sw_test_id.c

ifeq ($(INCLUDE_VP9),y)
SRC_TESTING += vp9testid.c
endif

SRC_EWL_PC := ewl_x280_file.c

SRC_EWL_ARM = ewl_x280_common.c ewl_linux_lock.c

ifeq ($(POLLING),y)
    SRC_EWL_ARM += ewl_x280_polling.c
else
    SRC_EWL_ARM += ewl_x280_irq.c
endif

SRCS = encswhwregisters.c

# common parts only for encoder and not for video stab
INCLUDE_ENC_COMMON=y


# Combine the list of all the source files included in the library build
ifeq ($(INCLUDE_HEVC),y)
    SRCS += $(SRC_HEVC)
    INCLUDE_ENC_COMMON=y
endif
ifeq ($(INCLUDE_VP9),y)
    SRCS += $(SRC_VP9)
    INCLUDE_ENC_COMMON=y
endif

ifeq ($(INCLUDE_TESTING),y)
    SRCS += $(SRC_TESTING)
    CONFFLAGS += -DINTERNAL_TEST   
endif
ifeq ($(TRACE_COMPRESSED_RECON),y)  
    CONFFLAGS  += -DINTERNAL_DECOMPRESS_RECON_TEST
ifeq (pci,$(findstring pci, $(MAKECMDGOALS)))    
		CONFFLAGS  += -DREMOVE_COMPRESSOR_C_CODE  
endif		  
endif  

# add common encoder files
ifeq ($(INCLUDE_ENC_COMMON),y)
    SRCS += $(SRC_ENC_COMMON)
endif    

# if tracing flags are defined we need to compile the tracing functions
ifeq ($(DEBUG),y)
    SRCS += $(SRC_TRACE)
endif

# choose EWL source, system model uses its own EWL
ifneq (,$(findstring pclinux, $(MAKECMDGOALS)))
    SRCS +=
else
ifneq (,$(findstring system, $(MAKECMDGOALS)))
    SRCS +=
else
ifneq (,$(findstring testdata, $(MAKECMDGOALS)))
    SRCS +=
else
ifneq (,$(findstring eval, $(MAKECMDGOALS)))
    SRCS +=
else
    SRCS += $(SRC_EWL_ARM)
endif
endif
endif
endif

#source search path
vpath %.c
vpath %.c ../source/common ../source/hevc ../source/vp9 ./ewl ./debug_trace

# name of the outputfile (library)
ENCLIB = libh2enc.a

# MACRO for cleaning object -files
RM  = rm -f

# MACRO for creating library that includes all the object files

# object files will be generated from .c sourcefiles
OBJS    = $(SRCS:.c=.o)


#Here are rules for building codes and generating object library.
all:    tags
	@echo ---------------------------------------
	@echo "Usage: make [ system | testdata | versatile | integrator ]"
	@echo "system     - PC system model (== pclinux)"
	@echo "testdata   - PC system model for test data creation"
	@echo "eval       - PC system model for evaluation with frame limit"
	@echo "versatile  - ARM versatile with FPGA HW"
	@echo "integrator - ARM integrator with FPGA HW"
	@echo "NOTE! Make sure to do 'make clean'"
	@echo "between compiling to different targets!"
	@echo ---------------------------------------

.PHONY: pclinux system testdata integrator versatile clean tags depend

evaluation: eval
eval: CFLAGS += -DEVALUATION_LIMIT=1000
eval: system

pclinux: testdata
system_static: testdata
system: testdata

testdata: CFLAGS += $(TRACEFLAGS)
testdata: .depend $(ENCLIB)

AR=../../../../../prebuilts/uclibc/bin/arm-buildroot-linux-uclibcgnueabihf-ar rcs

integrator: ENVSET  = -DSDRAM_LM_BASE=0x80000000 -DTARGET_SYSTEM=FS_LINUX
integrator: ENVSET  += -DEWL_NO_HW_TIMEOUT -DARM_ARCH_SWAP
integrator: CROSS_COMPILE = ../../../../../output/host/usr/bin/arm-buildroot-linux-uclibcgnueabihf-
integrator: ARCH = -mcpu=generic-armv7-a -mtune=generic-armv7-a
integrator: $(ENCLIB)
	make versatile -C test/hevc    
	#cp test/hevc/hevc_testenc /home/fengwu/b/work/sportdv_fpga/infotm/ramdisk_fpga/system/opt/ipnc

versatile: ENVSET  = -DSDRAM_LM_BASE=0x00000000 \
                     -DENC_MODULE_PATH=\"/tmp/dev/hx280\" \
                     -DMEMALLOC_MODULE_PATH=\"/tmp/dev/memalloc\"
versatile: ENVSET  += -DEWL_NO_HW_TIMEOUT  -DARM_ARCH_SWAP
versatile: CROSS_COMPILE = arm-none-linux-gnueabi-
versatile: ARCH = -mcpu=arm926ej-s -mtune=arm926ej-s
versatile: $(ENCLIB)


versatile_tb: versatile
	$(MAKE) -w -C test/vp9 versatile
	$(MAKE) -w -C test/hevc versatile



pcdemo: CFLAGS  = -O2 -g -Wall -D_GNU_SOURCE -D_REENTRANT -D_THREAD_SAFE \
                  -DDEBUG -D_ASSERT_USED $(INCLUDE) 
pcdemo: $(SRC_HEVC:.c=.o) $(SRC_ENC_COMMON:.c=.o)
	$(AR) $(ENCLIB) $+


.PHONY: pci
pci: 
pci: CROSS_COMPILE=
pci: ARCH=
pci: ENVSET  = -DPC_PCI_FPGA_DEMO \
               -DSDRAM_LM_BASE=$(CHW_BASE_ADDRESS) \
               -DENC_MODULE_PATH=\"/tmp/dev/hx280\" \
               -DMEMALLOC_MODULE_PATH=\"/tmp/dev/memalloc\"
pci: ENVSET  += -DEWL_NO_HW_TIMEOUT -DARM_ARCH_SWAP
pci: ENVSET  += -DENC_DEMO
pci: $(ENCLIB)


system_cov: CC = covc --retain -t!debug_trace/enctrace.c,!debug_trace/enctracestream.c g++
system_cov: $(ENCLIB)


$(ENCLIB): $(OBJS)
	$(AR) ../../../../release_arm/lib_static/$(ENCLIB) $(OBJS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(ENVSET) $(CONFFLAGS) $< -o $@

clean:
	$(RM) $(ENCLIB)
	$(RM) .depend
	$(RM) *.o *.gcno *.gcda       

tags: 
	ctags ../inc/*h ../source/common/*[ch] ../source/hevc/*[ch] ../source/vp9/*[ch]
	#../source/camstab/*[ch]\
            

lint: LINT_DEF=-dVIDEOSTAB_ENABLED
lint: $(SRC_HEVC) $(SRC_VP9) $(SRC_VIDSTAB) $(SRC_VIDSTAB_API) # take out any not relevant sources
        # -e537 removes warnings about multiple include
        # -e641 removes warnings about converting enum to int
	-lint-nt -w2 -e537 -e641 $(LINT_DEF) $(INCLUDE) $^ > pc-lint-report.txt
        
depend .depend: $(SRCS)
	$(CC) $(INCLUDE) $(ENVSET) $(CONFFLAGS) -M  $^ > .depend

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
ifeq (.depend, $(wildcard .depend))
include .depend
endif
endif
