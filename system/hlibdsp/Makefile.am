AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libdsp.la

libdsp_la_SOURCES = ceva-tl421.c
libdsp_la_CFLAGS = -Iinclude -Wall
libdsp_la_LDFLAGS = -lpthread -version-info 1:0:0

lib_LIBRARIES = libdsp.a
libdsp_a_SOURCES = ceva-tl421.c
libdsp_a_CFLAGS = -Iinclude -Wall
libdsp_a_LDFLAGS = -lpthread

bin_PROGRAMS = test_libdsp

test_libdsp_SOURCES = test.c ceva-tl421.c
test_libdsp_CFLAGS = -Iinclude -g
test_libdsp_LDFLAGS = -lrt -lpthread

