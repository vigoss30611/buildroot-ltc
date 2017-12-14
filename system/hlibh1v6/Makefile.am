AUTOMAKE_OPTIONS=foreign
lib_common_SOURCES= linux_reference/ewl/ewl_x280_irq.c \
				linux_reference/ewl/ewl_x280_common.c \
				linux_reference/ewl/ewl_linux_lock.c \
				source/camstab/vidstabalg.c \
				source/camstab/vidstabapi.c \
				source/camstab/vidstabinternal.c \
				source/camstab/vidstabcommon.c \
				source/h264/H264Mad.c \
				source/h264/H264CodeFrame.c \
				source/h264/H264RateControl.c \
				source/h264/h264encapi_ext.c \
				source/h264/H264Init.c \
				source/h264/H264PictureBuffer.c \
				source/h264/H264NalUnit.c \
				source/h264/H264PutBits.c \
				source/h264/H264SequenceParameterSet.c \
				source/h264/H264TestId.c \
				source/h264/H264Sei.c \
				source/h264/H264PictureParameterSet.c \
				source/h264/H264Cabac.c \
				source/h264/H264EncApi.c \
				source/h264/H264Slice.c \
				source/common/encswhwregisters.c \
				source/common/encasiccontroller.c \
				source/common/encpreprocess.c \
				source/common/encasiccontroller_v2.c \
				source/common/encsyn.c \
				source/jpeg/EncJpegPutBits.c \
				source/jpeg/EncJpegCodeFrame.c \
				source/jpeg/EncJpeg.c \
				source/jpeg/EncJpegInit.c \
				source/jpeg/JpegEncApi.c

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
EXTRA_CFLAGS= -std=c99 $(GDB_FLAGS) $(HIDDEN_FLAGS) -D_GNU_SOURCE -D_REENTRANT -D_THREAD_SAFE -DDEBUG -D_DEBUG_PRINT -D_ASSERT_USED \
			   -DEWL_NO_HW_TIMEOUT -DVIDEOSTAB_ENABLED -DH1_HAVE_VIDSTABDEBUG_H \
			   $(LIBFR_CFLAGS) \
				-I$(top_srcdir)/source/jpeg  \
				-I$(top_srcdir)/source/common  \
				-I$(top_srcdir)/source/h264  \
				-I$(top_srcdir)/linux/ewl  \
				-I$(top_srcdir)/linux_reference/kernel_module  \
				-I$(top_srcdir)/linux_reference/memalloc  \
				-I$(top_srcdir)/inc  \
				-I$(top_srcdir)/source/camstab


EXTRA_LDLAGS=  -avoid-version $(LIBFR_LIBS)


#get shared lib
lib_LTLIBRARIES=libh1v6.la
libh1v6_la_CPPFLAGS= $(EXTRA_CFLAGS)
libh1v6_la_LDFLAGS=  -avoid-version
libh1v6_la_SOURCES=$(lib_common_SOURCES)


###get testbench
if HAVE_TESTBENCH
bin_PROGRAMS=testbench_h1v6_264  testbench_h1v6_jpeg
testbench_h1v6_264_SOURCES=linux_reference/test/h264/EncGetOption.c linux_reference/test/h264/H264TestBench.c
testbench_h1v6_264_CFLAGS= $(EXTRA_CFLAGS)
testbench_h1v6_264_LDADD= $(lib_LTLIBRARIES)
testbench_h1v6_264_LDFLAGS=-lm -lrt -lpthread $(LIBFR_LIBS)

testbench_h1v6_jpeg_SOURCES=linux_reference/test/jpeg/JpegTestBench.c linux_reference/test/jpeg/EncGetOption.c
testbench_h1v6_jpeg_CFLAGS= $(EXTRA_CFLAGS)
testbench_h1v6_jpeg_LDADD= $(lib_LTLIBRARIES)
testbench_h1v6_jpeg_LDFLAGS=-lm -lrt -lpthread $(LIBFR_LIBS)
endif
