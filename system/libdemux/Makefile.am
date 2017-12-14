AUTOMAKE_OPTIONS=foreign
lib_common_SOURCES=demux.c
libdemux_a_CFLAGS=-I$(top_srcdir)/include/ -I. -pthread -Wall -Werror \
				$(LIBAVFORMAT_CFLAGS) \
				$(LIBAVCODEC_CFLAGS)

lib_LIBRARIES=libdemux.a
libdemux_a_SOURCES=${lib_common_SOURCES}
libdemux_a_CFLAGS += ${AM_CFLAGS}

lib_LTLIBRARIES=libdemux.la
libdemux_la_CFLAGS = -I$(top_srcdir)/include/ -I. -pthread -Wall -Werror\
				$(LIBAVFORMAT_CFLAGS) \
				$(LIBAVCODEC_CFLAGS)
libdemux_la_LDFLAGS= -avoid-version $(AM_LDFLAGS) \
					$(LIBAVFORMAT_LIBS) \
					$(LIBAVCODEC_LIBS)

libdemux_la_SOURCES=${lib_common_SOURCES}


