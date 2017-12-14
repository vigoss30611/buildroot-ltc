
lib_LTLIBRARIES = libispost.la

libispost_la_SOURCES = hlib_ispost.c

libispost_la_CFLAGS = -I./ -g -Wall -Werror

libispost_la_LDFLAGS = -version-info 1:0:0

bin_PROGRAMS = test_libispost

test_libispost_CFLAGS = -I./ -g -Wall -Werror

test_libispost_LDFLAGS = -pthread -lrt
test_libispost_SOURCES = \
				   			test.c	\
							hlib_ispost.c

AUTOMAKE_OPTIONS=foreign
