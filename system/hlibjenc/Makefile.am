AUTOMAKE_OPTIONS=foreign
lib_common_SOURCES= linux_reference/ewl_jpeg_enc.c \
				source/JpegEncApi.c \
				source/JpegEncCodeFrame.c \
				source/JpegEnc.c \
				source/JpegEncPutBits.c


#ACLOCAL_AMFLAGS = -I m4
#get static lib
EXTRA_CFLAGS= -g -std=c99 -O0 -fvisibility=hidden -D_GNU_SOURCE -D_REENTRANT -D_THREAD_SAFE -DDEBUG -D_DEBUG_PRINT -D_ASSERT_USED \
			   -DEWL_NO_HW_TIMEOUT -DVIDEOSTAB_ENABLED -DH1_HAVE_VIDSTABDEBUG_H \
			   $(LIBFR_CFLAGS) \
				-I$(top_srcdir)/source \
				-I$(top_srcdir)/linux_reference \
				-I$(top_srcdir)/inc  \
				-I$(top_srcdir)/source


EXTRA_LDLAGS=  -avoid-version $(LIBFR_LIBS)


#get shared lib
lib_LTLIBRARIES=libjenc.la
libjenc_la_CFLAGS= $(EXTRA_CFLAGS)
libjenc_la_LDFLAGS=  -avoid-version
libjenc_la_SOURCES=$(lib_common_SOURCES)

##get testbench
#bin_PROGRAMS=testbench
#testbench_SOURCES=test_log.c
