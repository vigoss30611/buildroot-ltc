AUTOMAKE_OPTIONS=foreign
lib_common_SOURCES=items.c
AM_CFLAGS=-I$(top_srcdir)/include/  -I. -pthread 
lib_LIBRARIES=libitems.a
libitems_a_SOURCES=${lib_common_SOURCES}
libitems_a_CFLAGS=${AM_CFLAGS}


lib_LTLIBRARIES=libitems.la
libitems_la_LDFLAGS= -version-info 1:0:0
libitems_la_SOURCES=${lib_common_SOURCES}
