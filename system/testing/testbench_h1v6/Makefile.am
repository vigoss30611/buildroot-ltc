AUTOMAKE_OPTIONS=foreign subdir-objects

hlibh1v6_dir = ../../hlibh1v6-1.0.0

lib_common_SOURCES=  h264/H264TestBench.c \
					 h264/EncGetOption.c 
					 
#lib_common_SOURCES= jpeg/EncGetOption.c \
#					 jpeg/JpegTestBench.c 
					 
#noinst_PROGRAMS=testbench_h1v6
noinst_PROGRAMS=testbench_h1v6
testbench_h1v6_CPPFLAGS=-g -std=c99  -O1 -D_GNU_SOURCE -D_REENTRANT -D_THREAD_SAFE -O1 	-DDEBUG 	-D_DEBUG_PRINT -D_ASSERT_USED \
			   -DEWL_NO_HW_TIMEOUT -DVIDEOSTAB_ENABLED -DH1_HAVE_VIDSTABDEBUG_H \
						-I$(hlibh1v6_dir)/inc  \
						-I$(hlibh1v6_dir)/source/common \
						-I$(hlibh1v6_dir)/source/h264 \
						-I$(hlibh1v6_dir)/linux/ewl \
						-I$(hlibh1v6_dir)/linux_reference/kernel_module  \                
						-I$(hlibh1v6_dir)/linux_reference/memalloc  \ 
						-I$(hlibh1v6_dir)/source/jpeg  \  
						-I$(hlibh1v6_dir)/source/camstab
						$(LIBFR_CFLAGS) \
						$(LIBSH1V6_CFLAGS) 
							
testbench_h1v6_LDFLAGS= $(LIBFR_LIBS) $(LIBH1V6_LIBS) -lrt -lm -lpthread

testbench_h1v6_SOURCES=$(lib_common_SOURCES)
