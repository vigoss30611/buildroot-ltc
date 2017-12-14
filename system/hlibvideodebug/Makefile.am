AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libvideodebug.la

libvideodebug_la_SOURCES = video_debug.c
libvideodebug_la_CFLAGS = -I$(srcdir)/include -Wall ${KERNEL_INCLUDE}
libvideodebug_la_LDFLAGS = -lpthread -version-info 1:0:0

lib_LIBRARIES = libvideodebug.a
libvideodebug_a_SOURCES = video_debug.c
libvideodebug_a_CFLAGS = -I$(srcdir)/include -Wall ${KERNEL_INCLUDE}
libvideodebug_a_LDFLAGS = -lpthread

bin_PROGRAMS = test_libvideodebug
test_libvideodebug_SOURCES = test_video_debug.c video_debug.c
test_libvideodebug_CFLAGS = -I$(srcdir)/include -g $(LIBVIDEOBOX_CFLAGS)
test_libvideodebug_LDFLAGS = -lrt -lpthread $(LIBVIDEOBOX_LIBS)
