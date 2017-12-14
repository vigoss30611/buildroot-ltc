AUTOMAKE_OPTIONS=foreign
lib_common_SOURCES= 	linux_reference/ewl/ewl_x280_irq.c  \
							linux_reference/ewl/ewl_x280_common.c  \
							linux_reference/ewl/ewl_linux_lock.c  \
							source/common/encswhwregisters.c  \
							source/common/encasiccontroller.c  \
							source/common/queue.c  \
							source/common/encpreprocess.c  \
							source/common/tools.c  \
							source/common/encasiccontroller_v2.c  \
							source/common/sw_put_bits.c  \
							source/common/error.c  \
							source/hevc/hevcencapi.c  \
							source/hevc/hevcSei.c  \
							source/hevc/sw_slice.c  \
							source/hevc/sw_nal_unit.c  \
							source/hevc/sw_test_id.c  \
							source/hevc/rate_control_picture.c  \
							source/hevc/sw_picture.c  \
							source/hevc/sw_parameter_set.c

if  HAVE_GDBDEBUG
GDB_FLAGS= -g -O0
else
GDB_FLAGS= -Os
endif

#if  HAVE_POLL
#lib_common_SOURCES+= linux_reference/ewl/ewl_x280_polling.c
#else
#lib_common_SOURCES+= linux_reference/ewl/ewl_x280_irq.c
#endif

if HAVE_TESTBENCH
#export ewl api to testbench,just for testbench,please remove it for nornal release
HIDDEN_FLAGS=
else
HIDDEN_FLAGS= -fvisibility=hidden
endif


#ACLOCAL_AMFLAGS = -I m4
#get static lib
EXTRA_CFLAGS=   $(GDB_FLAGS)  -std=c99 -D_GNU_SOURCE $(HIDDEN_FLAGS) -DNDEBUG  -Wno-unused   -D_REENTRANT -D_THREAD_SAFE  -D_DEBUG_PRINT -D_ASSERT_USED  -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE\
			   -DEWL_NO_HW_TIMEOUT -DVIDEOSTAB_ENABLED -DH2_HAVE_VIDSTABDEBUG_H \
			   $(LIBHWHEADER_CFLAGS) \
			   $(LIBFR_CFLAGS) \
				-I$(top_srcdir)/source/hevc  \
				-I$(top_srcdir)/source/common  \
				-I$(top_srcdir)/linux/ewl  \
				-I$(top_srcdir)/linux_reference/kernel_module  \
				-I$(top_srcdir)/linux_reference/memalloc  \
				-I$(top_srcdir)/inc  \
				-I$(top_srcdir)/source/camstab


EXTRA_LDLAGS=  -avoid-version\
			   $(LIBFR_LIBS)


#get shared lib
lib_LTLIBRARIES=libh2v1.la
libh2v1_la_CPPFLAGS= $(EXTRA_CFLAGS)
libh2v1_la_LDFLAGS=  -avoid-version
libh2v1_la_SOURCES=$(lib_common_SOURCES)



###get testbench
if HAVE_TESTBENCH
bin_PROGRAMS=testbench_hevc_v1
#testbench_hevc_v1_SOURCES=linux_reference/test/hevc/get_option.c linux_reference/test/hevc/test_bench.c  $(lib_common_SOURCES)
testbench_hevc_v1_SOURCES=linux_reference/test/hevc/get_option.c linux_reference/test/hevc/test_bench.c
testbench_hevc_v1_CFLAGS= $(EXTRA_CFLAGS)
testbench_hevc_v1_LDADD= $(lib_LTLIBRARIES)
testbench_hevc_v1_LDFLAGS=-lm -lrt -lpthread $(LIBFR_LIBS)
endif
