AUTOMAKE_OPTIONS=foreign subdir-objects

hlibjenc_dir = ../../hlibjenc-1.0.0

lib_common_SOURCES=  JencTestBench.c

bin_PROGRAMS=testbench_jenc
testbench_jenc_CFLAGS=-g -O0 -std=c99  -D_GNU_SOURCE -D_REENTRANT -D_THREAD_SAFE -DDEBUG 	-D_DEBUG_PRINT -D_ASSERT_USED \
						-I$(hlibjenc_dir)/inc  \
						-I$(hlibjenc_dir)/linux_reference \
						-I$(hlibjenc_dir)/source  \
						$(LIBFR_CFLAGS) \
						$(LIBJENC_CFLAGS)

testbench_jenc_LDFLAGS= $(LIBFR_LIBS) $(LIBJENC_LIBS) -lrt -lm -lpthread

testbench_jenc_SOURCES=$(lib_common_SOURCES)
