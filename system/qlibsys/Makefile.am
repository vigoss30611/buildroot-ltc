AUTOMAKE_OPTIONS=foreign
lib_common_SOURCES=system.c setting.c
libsys_a_CFLAGS=-I$(top_srcdir)/include/ -Iinclude/qsdk/ -pthread -Wall -Werror \
				$(LIBITEMS_CFLAGS) 
lib_LIBRARIES=libsys.a
libsys_a_SOURCES=${lib_common_SOURCES}
libsys_a_CFLAGS += ${AM_CFLAGS}

lib_LTLIBRARIES=libsys.la
libsys_la_CFLAGS = -I$(top_srcdir)/include/ -Iinclude/qsdk/ -pthread -Wall -Werror \
				$(LIBITEMS_LIBS) 
libsys_la_LDFLAGS= -version-info 1:0:0 $(AM_LDFLAGS) -lcJSON
libsys_la_SOURCES=${lib_common_SOURCES}

noinst_PROGRAMS = test_sys
test_sys_SOURCES = setting.c test/test.c
test_sys_CFLAGS = -I$(top_srcdir)/include -Iinclude/qsdk/ -Wall -Werror
test_sys_LDFLAGS = -lcJSON -lpthread

