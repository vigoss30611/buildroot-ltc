AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libhid.la

libhid_la_SOURCES = hid.c
libhid_la_CFLAGS = -Iinclude -Wall ${KERNEL_INCLUDE}
libhid_la_LDFLAGS = -lpthread -version-info 1:0:0

lib_LIBRARIES = libhid.a
libhid_a_SOURCES = hid.c
libhid_a_CFLAGS = -Iinclude -Wall ${KERNEL_INCLUDE}
libhid_a_LDFLAGS = -lpthread

bin_PROGRAMS = test_libhid

test_libhid_SOURCES = hid_test.c hid.c
test_libhid_CFLAGS = -Iinclude 
test_libhid_LDFLAGS = -lrt -lpthread 
