#-------------------------------------------------------------------------------
#-                                                                            --
#-       This software is confidential and proprietary and may be used        --
#-        only as expressly authorized by a licensing agreement from          --
#-                                                                            --
#-                            Hantro Products Oy.                             --
#-                                                                            --
#-                   (C) COPYRIGHT 2012 HANTRO PRODUCTS OY                    --
#-                            ALL RIGHTS RESERVED                             --
#-                                                                            --
#-                 The entire notice above must be reproduced                 --
#-                  on all copies and should not be removed.                  --
#-                                                                            --
#-------------------------------------------------------------------------------
#-
#--  Abstract : Makefile for test utilities.
#--
#-------------------------------------------------------------------------------

SRCS = fifo.c async_task.c
ASYNC_UTEST_SRCS = async_task_unittest.c
FIFO_UTEST_SRCS = fifo_unittest.c
OBJS = $(SRCS:.c=.o)
FIFO_UTEST_OBJS = $(FIFO_UTEST_SRCS:.c=.o)
ASYNC_UTEST_OBJS = $(ASYNC_UTEST_SRCS:.c=.o)
LIB = libutils.a
INCLUDE = -I../../../source/inc
ifdef FIFO_DATATYPE
DEFINES=-DFIFO_DATATYPE=$(FIFO_DATATYPE)
else
DEFINES=-DFIFO_DATATYPE=i32
endif

.PHONY: all
all: lib

utils: CFLAGS=-g -c -Wall -Wextra -pedantic $(INCLUDE) $(DEFINES) $(M32)
utils: LIBS=-lpthread -lrt
utils: $(OBJS)
	$(CC) $(CFLAGS) $(LIBS)

lib: utils
	$(AR) rcv $(LIB) $(OBJS)

fifo_test: EXEC=fifo
fifo_test: CFLAGS=-g -Wall $(INCLUDE) $(DEFINES) $(M32)
fifo_test: LIBS=-lpthread -lrt
fifo_test: lib utils $(FIFO_UTEST_OBJS)
	$(CC) $(CFLAGS) $(FIFO_UTEST_OBJS) $(LIBS) $(LIB) -o $(EXEC)

async_task_test: EXEC=async_task
async_task_test: CFLAGS=-g -Wall $(INCLUDE) $(DEFINES) $(M32)
async_task_test: LIBS=-lpthread
async_task_test: lib utils $(ASYNC_UTEST_OBJS)
	$(CC) $(CFLAGS) $(ASYNC_UTEST_OBJS) $(LIBS) $(LIB) -o $(EXEC)

clean:
	rm -f fifo
	rm -f async_task
	rm -f $(LIB)
	rm -f *.o
