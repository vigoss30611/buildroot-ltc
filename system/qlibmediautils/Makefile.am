AUTOMAKE_OPTIONS=foreign

COMMON_CFLAGS= -Werror -Os -std=c99
#set source files
if QLIBMEDIAUTILS_CALIBRATION
COMMON_CFLAGS+= -DMEDIAUTILS_CALIBRATION
LIB_JPEGEXIF_SOURCE= lib/jpegexif.c
endif
 
if QLIBMEDIAUTILS_SWSCALE
COMMON_CFLAGS+= -DMEDIAUTILS_SWSCALE
LIB_SWSCALE_SOURCE= lib/jpegutils.c lib/swscale.c
endif
if QLIBMEDIAUTILS_SWCROP
COMMON_CFLAGS+= -DMEDIAUTILS_SWSCROP
LIB_SWCROP_SOURCE = lib/swscrop.c
endif

LIB_COMMON_SOURCES = $(LIB_JPEGEXIF_SOURCE) $(LIB_SWSCALE_SOURCE) $(LIB_SWCROP_SOURCE)

if QLIBMEDIAUTILS_BUILD_LIB
lib_LTLIBRARIES=libqmediautils.la
endif

libqmediautils_la_SOURCES=$(LIB_COMMON_SOURCES)
libqmediautils_la_CFLAGS= $(COMMON_CFLAGS) \
			-I$(top_srcdir)/include \
			-I$(top_srcdir)

#set flags
if QLIBMEDIAUTILS_SWSCALE
libqmediautils_la_CFLAGS += $(LIBSWSCALE_CFLAGS) \
			$(LIBAVUTIL_CFLAGS) \
			$(LIBAVFORMAT_CFLAGS)
endif

libqmediautils_la_LDFLAGS=  -avoid-version

#set libs
#if QLIBMEDIAUTILS_CALIBRATION
#QLIBMEDIAUTILS_CALIBRATION_LIB =${top_srcdir}/../libexif-0.6.21/libexif/libexif.la
#endif


if QLIBMEDIAUTILS_SWSCALE
QLIBMEDIAUTILS_SWSCALE_LIB = $(LIBSWSCALE_LIBS) \
						 $(LIBAVUTIL_LIBS) \
						 $(LIBAVFORMAT_LIBS)
endif

if QLIBMEDIAUTILS_SWCROP
QLIBMEDIAUTILS_SWCROP_LIB = $(LIBSWSCALE_LIBS) \
						 $(LIBAVUTIL_LIBS) \
						 $(LIBAVFORMAT_LIBS) \
						 $(LIBAVFILTER_LIBS)
endif

libqmediautils_la_LIBADD = $(QLIBMEDIAUTILS_CALIBRATION_LIB) $(QLIBMEDIAUTILS_SWSCALE_LIB) $(QLIBMEDIAUTILS_SWCROP_LIB)

#lib_LIBRARIES = libqmediautils.a

#libqmediautils_a_SOURCES=$(LIB_COMMON_SOURCES)
#libqmediautils_a_CXXFLAGS= $(COMMON_CFLAGS) \
#				$(LIBEXIF_CFLAGS)\
#				-I$(top_srcdir)/include -I$(top_srcdir)\
#				${LIBEXIF_CFLAGS}
#libqmediautils_a_LDFLAGS=-avoid-version -static

#libqmedia_a_LIBADD=${top_srcdir}/../libexif-0.6.21/libexif/libexif.a

#test tools
if QLIBMEDIAUTILS_TESTING
bin_PROGRAMS=exiftest
endif
exiftest_SOURCES=test/exiftest.c
exiftest_CFLAGS=$(COMMON_CFLAGS) \
			$(LIBAVUTIL_CFLAGS) \
			$(LIBEXIF_CFLAGS) \
		-I$(top_srcdir)/include/  \
		-I$(top_srcdir)
exiftest_LDFLAGS=  $(LIBSWSCALE_LIBS) $(LIBAVUTIL_LIBS) $(LIBAVFORMAT_LIBS)  $(LIBEXIF_LIBS)
exiftest_LDADD=${top_srcdir}/libqmediautils.la

noinst_PROGRAMS=duuuumy
duuuumy_CFLAGS=$(COMMON_CFLAGS) \
		-I$(top_srcdir)/dummy_include/
duuuumy_SOURCES=test/dummy.c
