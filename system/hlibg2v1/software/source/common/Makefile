# Copyright 2013 Google Inc. All Rights Reserved.

COMMON_SRCS += software/source/common/bqueue.c \
               software/source/common/commonconfig.c \
               software/source/common/fifo.c \
               software/source/common/raster_buffer_mgr.c \
               software/source/common/regdrv.c \
               software/source/common/sw_stream.c \
               software/source/common/sw_util.c
FIFO_TEST_SRCS += software/source/common/fifo_unittest.c
DEFINES+=-DFIFO_DATATYPE=void*

fifo: $(COMMON_SRCS:.c=.o) $(FIFO_TEST_SRCS:.c=.o)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@
