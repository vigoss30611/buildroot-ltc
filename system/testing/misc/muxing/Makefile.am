AUTOMAKE_OPTIONS=foreign
lib_common_SOURCES=muxing.c
AM_CFLAGS=-g -I$(top_srcdir)/include/  -I$(top_srcdir)/ -pthread  $(LIBAVCODEC_FLAGS) $(LIBAVFORMAT_CFLAGS) $(LIBAVCODEC_CFLAGS)

##get testbench
noinst_PROGRAMS=demuxing 


demuxing_SOURCES=demuxing.c
demuxing_LDFLAGS=   $(LIBAVCODEC_FLAGS) $(LIBAVFORMAT_LIBS)  $(LIBAVCODEC_LIBS) $(LIBAVUTIL_LIBS)  -lm


