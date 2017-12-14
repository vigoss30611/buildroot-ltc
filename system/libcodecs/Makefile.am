AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libcodecs.la

if HAVE_DSPSUPPORT
DSPSUPPORT_CFLAGS= -DDSP_AAC_SUPPORT $(LIBDSP_CFLAGS)
DSPSUPPORT_LIBS= $(LIBDSP_LIBS)
else
DSPSUPPORT_CFLAGS=
DSPSUPPORT_LIBS=
endif

libcodecs_la_SOURCES = \
					  codecs.c 	\
					  ffmpeg.c 
					  
libcodecs_la_CFLAGS = $(DSPSUPPORT_CFLAGS) -Iinclude -Wall -Werror -g -O0\
					$(LIBAVCODEC_CFLAGS)		\
					$(LIBAVFORMAT_CFLAGS)		\
					$(LIBSWREAMPLE_CFLAGS)		

libcodecs_la_LDFLAGS = -lpthread	\
					$(LIBAVCODEC_LIBS)	\
					$(LIBAVFORMAT_LIBS)	\
					$(LIBSWREAMPLE_LIBS)	\
					$(DSPSUPPORT_LIBS)

lib_LIBRARIES = libcodecs.a
libcodecs_a_SOURCES = \
					 codecs.c	\
					 ffmpeg.c

libcodecs_a_CFLAGS = $(DSPSUPPORT_CFLAGS) -Iinclude -Wall -Werror -g \
					$(LIBAVCODEC_CFLAGS)		\
					$(LIBAVFORMAT_CFLAGS)		\
					$(LIBSWREAMPLE_CFLAGS)		

libcodecs_a_LDFLAGS = -lpthread		\
					$(LIBAVCODEC_LIBS)	\
					$(LIBAVFORMAT_LIBS)	\
					$(LIBSWREAMPLE_LIBS)	\
					$(DSPSUPPORT_LIBS)
