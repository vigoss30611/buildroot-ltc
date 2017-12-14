# Copyright 2013 Google Inc. All Rights Reserved.
#
# Abstract : Makefile for the common test bench sources

# list of used sourcefiles
COMMON_SRCS += software/test/common/swhw/tb_stream_corrupt.c \
               software/test/common/swhw/tb_params.c \
               software/test/common/swhw/tb_cfg.c \
               software/test/common/swhw/tb_md5.c \
               software/test/common/swhw/md5.c \
               software/test/common/swhw/tb_tiled.c

# HW internal tracing not possible on fpga
ifeq ($(strip $(ENV)),x86_linux)
  COMMON_SRCS += software/test/common/swhw/trace.c
endif

