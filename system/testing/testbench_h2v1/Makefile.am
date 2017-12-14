AUTOMAKE_OPTIONS=foreign
lib_common_SOURCES=get_option.c  test_bench.c

libh2v1_dir = ../../hlibh2v1-1.0.0

noinst_PROGRAMS=testbench_hevc
testbench_hevc_CPPFLAGS= -g -std=c99 -I$(top_srcdir) $(LIBFR_CFLAGS) $(LIBH2V1_CFLAGS) \
						 -I$(libh2v1_dir)/inc \
						 -I$(libh2v1_dir)/source/common \
						 -I$(libh2v1_dir)/source/hevc \
						 -I$(libh2v1_dir)/linux_reference/ewl \
						 -I$(libh2v1_dir)/linux_reference/kernel_module \
						 -I$(libh2v1_dir)/linux_reference/memalloc 
			
testbench_hevc_LDFLAGS= $(LIBFR_LIBS)  $(LIBH2V1_LIBS) -lrt -lm

testbench_hevc_SOURCES=$(lib_common_SOURCES)

