# Copyright 2013 Google Inc. All Rights Reserved.
#
# Abstract : Makefile containing common settings and rules used to compile
#            Hantro G2 decoder software.

# General off-the-shelf environment settings
ENV ?= x86_linux  # default
USE_64BIT_ENV ?= n
RESOLUTION_1080P ?= n
CACHE_COVERED ?= n
ifeq ($(strip $(ENV)),x86_linux)
  ARCH ?=
  CROSS ?=
  AR  = $(CROSS)ar rcs
  CC  = $(CROSS)gcc
  ifeq ($(strip $(USE_64BIT_ENV)),n)
    CFLAGS += -m32
    LDFLAGS += -m32
  endif
  USE_MODEL_SIMULATION ?= y
endif
ifeq ($(strip $(ENV)),arm_linux)
  ARCH ?=
  CROSS ?= arm-none-linux-gnueabi-
  AR  = $(CROSS)ar rcs
  CC  = $(CROSS)gcc
  CFLAGS += -fpic
  INCLUDE += -Isoftware/linux/memalloc \
             -Isoftware/linux/ldriver/
  DEFINES += -DDEC_MODULE_PATH=\"/tmp/dev/hantrodec\" \
             -DMEMALLOC_MODULE_PATH=\"/tmp/dev/memalloc\"
  USE_MODEL_SIMULATION ?= n
endif
ifeq ($(strip $(ENV)),x86_linux_pci)
  ARCH ?=
  CROSS ?=
  AR  = $(CROSS)ar rcs
  CC  = $(CROSS)gcc
  INCLUDE += -Isoftware/linux/memalloc \
             -Isoftware/linux/ldriver \
             -Isoftware/linux/pcidriver
  DEFINES += -DDEC_MODULE_PATH=\"/tmp/dev/hantrodec\" \
             -DMEMALLOC_MODULE_PATH=\"/tmp/dev/memalloc\"
  USE_MODEL_SIMULATION ?= n
endif

ifeq ($(strip $(USE_64BIT_ENV)),y)
  DEFINES += -DUSE_64BIT_ENV
endif

ifeq ($(strip $(RESOLUTION_1080P)),y)
  DEFINES += -DRESOLUTION_1080P
endif

ifeq ($(strip $(CACHE_COVERED)),y)
  DEFINES += -DCACHE_COVERED
endif

USE_HW_PIC_DIMENSIONS ?= n
ifeq ($(USE_HW_PIC_DIMENSIONS), y)
  DEFINES += -DHW_PIC_DIMENSIONS
endif

# Define for using prebuilt library
USE_MODEL_LIB ?= n

DEFINES += -DFIFO_DATATYPE=void*
# Common error flags for all targets
CFLAGS  += -Wall -ansi -std=c99 -pedantic
# DWL uses variadic macros for debug prints
CFLAGS += -Wno-variadic-macros

# Common libraries
LDFLAGS += -L$(OBJDIR) -pthread

# MACRO for cleaning object -files
RM  = rm -f

# Common configuration settings
RELEASE ?= n
USE_COVERAGE ?= n
USE_PROFILING ?= n
USE_SW_PERFORMANCE ?= n
# set this to 'y' for enabling IRQ mode for the decoder. You will need
# the hx170dec kernel driver loaded and a /dev/hx170 device node created
USE_DEC_IRQ ?= n
# set this 'y' for enabling sw reg tracing. NOTE! not all sw reagisters are
# traced; only hw "return" values.
USE_INTERNAL_TEST ?= n
# set this to 'y' to enable asic traces
USE_ASIC_TRACE ?= n
# set this tot 'y' to enable webm support, needs nestegg lib
WEBM_ENABLED ?= n
NESTEGG ?= $(HOME)/nestegg
# set this to 'y' to enable SDL support, needs sdl lib
USE_SDL ?= n
SDL_CFLAGS ?= $(shell sdl-config --cflags)
SDL_LDFLAGS ?= $(shell sdl-config --libs)
DOWN_SCALER ?= y
USE_TB_PP ?= y
USE_EXTERNAL_BUFFER ?= y
# Flags for >2GB file support.
DEFINES += -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE

ifeq ($(strip $(USE_SDL)),y)
  DEFINES += -DSDL_ENABLED
  CFLAGS += $(SDL_CFLAGS)
  LDFLAGS += $(SDL_LDFLAGS)
  LIBS += -lSDL -ldl -lrt -lm
endif

ifeq ($(strip $(RELEASE)),n)
  CFLAGS   += -g -O0
  DEFINES += -DDEBUG -D_ASSERT_USED -D_RANGE_CHECK -D_ERROR_PRINT
  BUILDCONFIG = debug
  STRIP ?= n
else
  CFLAGS   += -O2
  DEFINES += -DNDEBUG
  BUILDCONFIG = release
  STRIP ?= y
endif

# Directory where object and library files are placed.
BUILDROOT=out
OBJDIR=$(strip $(BUILDROOT))/$(strip $(ENV))/$(strip $(BUILDCONFIG))

ifeq ($(USE_ASIC_TRACE), y)
  DEFINES += -DASIC_TRACE_SUPPORT
endif

ifeq ($(USE_COVERAGE), y)
  CFLAGS += -coverage -fprofile-arcs -ftest-coverage
  LDFLAGS += -coverage
endif

ifeq ($(USE_PROFILING), y)
  CFLAGS += -pg
  LDFLAGS += -pg
endif

ifeq ($(USE_MODEL_SIMULATION), y)
  DEFINES += -DMODEL_SIMULATION
endif

ifeq ($(USE_DEC_IRQ), y)
  DEFINES +=  -DDWL_USE_DEC_IRQ
endif

ifeq ($(DOWN_SCALER), y)
  DEFINES += -DDOWN_SCALER
endif

ifeq ($(USE_TB_PP), y)
  DEFINES += -DTB_PP
endif

ifeq ($(USE_EXTERNAL_BUFFER), y)
  DEFINES += -DUSE_EXTERNAL_BUFFER
endif

  DEFINES += -DCLEAR_OUT_BUFFER

# Define the decoder output format.
# TODO(vmr): List the possible values.
DEFINES += -DDEC_X170_OUTPUT_FORMAT=0 # raster scan output

# Set length of SW timeout in milliseconds. default: infinite wait (-1)
# This is just a parameter passed to the wrapper layer, so the real
# waiting happens there and is based on that implementation
DEFINES += -DDEC_X170_TIMEOUT_LENGTH=-1

ifeq ($(USE_SW_PERFORMANCE), y)
  DEFINES += -DSW_PERFORMANCE
endif

ifeq ($(DISABLE_PIC_FREEZE_FLAG), y)
  DISABLE_PIC_FREEZE=-D_DISABLE_PIC_FREEZE
endif
ifeq ($(WEBM_ENABLED), y)
  DEFINES += -DWEBM_ENABLED
  INCLUDE += -I$(NESTEGG)/include
  LIBS    += $(NESTEGG)/lib/libnestegg.a
endif
