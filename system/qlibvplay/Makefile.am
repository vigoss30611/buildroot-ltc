AUTOMAKE_OPTIONS=foreign

if  HAVE_GDBDEBUG
GDB_CFLAGS= -g -O0
else
GDB_CFLAGS=
endif
if  HAVE_FRAGMENTED
FRAGMENTED_CFLAGS= -DFRAGMENTED_MP4_SUPPORT
else
FRAGMENTED_CFLAGS=
endif

DEFAULT_VISIBILITY_FLAGS=-fvisibility=hidden

#DLL_EXPORT_DEFINE=-DDLL_EXPORT='__attribute__((visibility(\"default\")))'

if HAVE_RECORDER_CACHE_TIME
#CACHE_TIME_CFLAGS=-DRECORDER_CACHE_SECOND=5
RECORDER_CACHE_TIME_CFLAGS=-DRECORDER_CACHE_SECOND=$(recorder_cache_second)
else
RECORDER_CACHE_TIME_CFLAGS=
endif

if HAVE_PLAYER_CACHE_TIME
PLAYER_CACHE_TIME_CFLAGS=-DPLAYER_CACHE_SECOND=$(player_cache_second)
else
PLAYER_CACHE_TIME_CFLAGS=
endif

CCCXX_CFLAGS = -Werror $(GDB_CFLAGS)  $(FRAGMENTED_CFLAGS) -DLOGGER_LEVEL_SET=$(logger_level)  -D__STDC_CONSTANT_MACROS\
			   $(RECORDER_CACHE_TIME_CFLAGS) $(PLAYER_CACHE_TIME_CFLAGS)

COMMON_CXXFLAGS=  -std=c++11  $(CCCXX_CFLAGS) $(DEFAULT_VISIBILITY_FLAGS)
COMMON_CFLAGS=    -std=c99    $(CCCXX_CFLAGS) $(DEFAULT_VISIBILITY_FLAGS)

#LIB_COMMON_SOURCES=lib/muxer.cpp  lib/vplaylib.cpp lib/acodec.cpp lib/vplayrec.cpp lib/frfifoloop.cpp  lib/qcodecs.cpp
LIB_COMMON_SOURCES=lib/threadcore.cpp lib/mediamuxer.cpp \
				lib/apicalleraudio.cpp  \
				lib/apicallervideo.cpp  \
				lib/apicallerextra.cpp  \
				lib/qmediafifo.cpp \
				lib/common.cpp \
				lib/qplayer.cpp \
				lib/qaudioplayer.cpp \
				lib/speedctrl.cpp \
				lib/qrecorder.cpp


lib_LTLIBRARIES=libqvplay.la
#lib_LTLIBRARIES=libqvplay.la
noinst_LIBRARIES=libqvplay.a
libqvplay_la_SOURCES=$(LIB_COMMON_SOURCES)
libqvplay_la_CXXFLAGS= $(COMMON_CXXFLAGS) \
				   -I$(top_srcdir)/include  -I$(top_srcdir)  \
				   $(LIBAVCODEC_CFLAGS) \
				   $(LIBAVFORMAT_CFLAGS) \
				   $(LIBAVCODEC_CFLAGS) \
				   $(LIBAUDIO_CFLAGS) \
				   $(LIBAUDIOBOX_CFLAGS) \
				   $(LIBFR_CFLAGS) \
				   $(LIBVB_CFLAGS) \
				   $(LIBCODECS_CFLAGS) \
				   $(LIBDEMUX_CFLAGS)


libqvplay_la_LDFLAGS=  -avoid-version\
				   $(LIBAVCODEC_LIBS) \
				   $(LIBAVFORMAT_LIBS)     \
				   $(LIBAVUTIL_LIBS) \
				   $(LIBSWREAMPLE_LIBS) \
				   $(LIBAUDIO_LIBS) \
				   $(LIBAUDIOBOX_LIBS) \
				   $(LIBFR_LIBS) \
				   $(LIBVB_LIBS) \
				   $(LIBCODECS_LIBS) \
				   $(LIBDEMUX_LIBS)


libqvplay_a_SOURCES=$(LIB_COMMON_SOURCES)

libqvplay_a_CXXFLAGS= $(COMMON_CXXFLAGS) \
				   -I$(top_srcdir)/include  -I$(top_srcdir)  \
				   $(LIBAVCODEC_CFLAGS) \
				   $(LIBAVFORMAT_CFLAGS) \
				   $(LIBAVCODEC_CFLAGS) \
				   $(LIBSWREAMPLE_CFLAGS) \
				   $(LIBAUDIO_CFLAGS) \
				   $(LIBAUDIOBOX_CFLAGS) \
				   $(LIBFR_CFLAGS) \
				   $(LIBVB_CFLAGS) \
				   $(LIBDEMUX_CFLAGS)

libqvplay_a_LDFLAGS=  -avoid-version\
					   $(LIBAVFORMAT_LIBS)     \
				   $(LIBAVUTIL_LIBS) \
				   $(LIBAUDIO_LIBS) \
				   $(LIBAUDIOBOX_LIBS) \
				   $(LIBSWREAMPLE_LIBS) \
				   $(LIBFR_LIBS) \
				   $(LIBVB_LIBS) \
				   -lh264bitstream\
				   $(LIBDEMUX_LIBS)

if  HAVE_TESTUTILS
bin_PROGRAMS= vrec   vplayer demux fifotest demux2
else
noinst_PROGRAMS= vrec   vplayer demux demux2
endif


vrec_SOURCES=test/vrec.c
vrec_CFLAGS= $(COMMON_CFLAGS) -I$(top_srcdir)/include  -I$(top_srcdir)
vrec_LDFLAGS= -lh264bitstream
vrec_LDADD=  $(top_srcdir)/libqvplay.la

vplayer_SOURCES=test/vplayer.c
vplayer_CFLAGS= $(COMMON_CFLAGS) -I$(top_srcdir)/include  -I$(top_srcdir)
vplayer_LDFLAGS= -lh264bitstream
vplayer_LDADD=  $(top_srcdir)/libqvplay.la


demux_SOURCES= test/demux.c
demux_CFLAGS = -std=c99 $(LIBAVCODEC_FLAGS) $(LIBAVFORMAT_CFLAGS) $(LIBAVCODEC_CFLAGS)
demux_LDFLAGS=   $(LIBAVCODEC_FLAGS) $(LIBAVFORMAT_LIBS)  $(LIBAVCODEC_LIBS) $(LIBAVUTIL_LIBS)  -lm

demux2_SOURCES= test/demux2.cpp
demux2_CXXFLAGS = $(COMMON_CXXFLAGS) \
				   -I$(top_srcdir)/include  -I$(top_srcdir)  \
				   $(LIBAVCODEC_CFLAGS) \
				   $(LIBAVFORMAT_CFLAGS) \
				   $(LIBAVCODEC_CFLAGS) \
				   $(LIBSWREAMPLE_CFLAGS) \
				   $(LIBAUDIO_CFLAGS) \
				   $(LIBAUDIOBOX_CFLAGS) \
				   $(LIBFR_CFLAGS) \
				   $(LIBVB_CFLAGS) \
				   $(LIBDEMUX_CFLAGS)

demux2_LDFLAGS=   -avoid-version\
					   $(LIBAVFORMAT_LIBS)     \
				   $(LIBAVUTIL_LIBS) \
				   $(LIBAUDIO_LIBS) \
				   $(LIBAUDIOBOX_LIBS) \
				   $(LIBSWREAMPLE_LIBS) \
				   $(LIBFR_LIBS) \
				   $(LIBVB_LIBS) \
				   -lh264bitstream\
				   $(LIBDEMUX_LIBS)

fifotest_SOURCES=	test/fifotest.cpp lib/qmediafifo.cpp lib/common.cpp
fifotest_CXXFLAGS=  -I$(top_srcdir)/include  -I$(top_srcdir) $(COMMON_CXXFLAGS) \
				   $(LIBFR_CFLAGS)

fifotest_LDFLAGS=  -lm -pthread \
				   $(LIBFR_LIBS) \
				   $(LIBVB_LIBS)
