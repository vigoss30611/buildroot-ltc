
lib_LTLIBRARIES = libfodetv2.la

libfodetv2_la_SOURCES = hlib_fodet.c

libfodetv2_la_CFLAGS = -I./ -g -Wall -Werror

libfodetv2_la_LDFLAGS = -version-info 1:0:0

bin_PROGRAMS = test_libfodetv2

test_libfodetv2_CFLAGS = -I./ -g -Wall -Werror

test_libfodetv2_LDFLAGS = -pthread -lrt
test_libfodetv2_SOURCES = \
   			test.c	\
			hlib_fodet.c

AUTOMAKE_OPTIONS=foreign
