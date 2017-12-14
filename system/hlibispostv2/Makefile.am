
lib_LTLIBRARIES = libispostv2.la

libispostv2_la_SOURCES = hlib_ispost.c

libispostv2_la_CFLAGS = -I./ -g -Wall -Werror

libispostv2_la_LDFLAGS = -version-info 1:0:0

noinst_PROGRAMS = test_libispostv2

test_libispostv2_CFLAGS = -I./ -g -Wall -Werror

test_libispostv2_LDFLAGS = -pthread -lrt
test_libispostv2_SOURCES = \
				   			test.c	\
							hlib_ispost.c

AUTOMAKE_OPTIONS=foreign
