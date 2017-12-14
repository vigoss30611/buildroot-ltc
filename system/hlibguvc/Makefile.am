AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libguvc.la

libguvc_la_SOURCES = guvc.c
libguvc_la_CFLAGS = -I$(srcdir)/include -Wall ${KERNEL_INCLUDE}
libguvc_la_LDFLAGS = -lpthread -version-info 1:0:0

lib_LIBRARIES = libguvc.a
libguvc_a_SOURCES = guvc.c
libguvc_a_CFLAGS = -I$(srcdir)/include -Wall ${KERNEL_INCLUDE}
libguvc_a_LDFLAGS = -lpthread

bin_PROGRAMS = test_libguvc

test_libguvc_SOURCES = guvc_test.c guvc.c
test_libguvc_CFLAGS = -I$(srcdir)/include -g $(LIBVIDEOBOX_CFLAGS)
test_libguvc_LDFLAGS = -lrt -lpthread $(LIBVIDEOBOX_LIBS)
