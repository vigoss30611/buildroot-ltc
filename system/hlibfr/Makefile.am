AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libfr.la

if CC_BUFFER
cache_opt = -DFR_CC_BUFFER 
endif

if CC_WRITETHROUGH
cache_opt = -DFR_CC_WRITETHROUGH 
endif

if CC_WRITEBACK
cache_opt = -DFR_CC_WRITEBACK 
endif

if NEW_MMAP
mmap_opt = -DFR_NEW_MAP=1
else
mmap_opt = -DFR_NEW_MAP=0
endif

libfr_la_SOURCES = fr.c
libfr_la_CFLAGS = $(cache_opt) $(mmap_opt) -Iinclude -Wall
libfr_la_LDFLAGS = -lpthread -version-info 1:0:0

lib_LIBRARIES = libfr.a
libfr_a_SOURCES = fr.c
libfr_a_CFLAGS = $(cache_opt) $(mmap_opt) -Iinclude -Wall
libfr_a_LDFLAGS = -lpthread

noinst_PROGRAMS = test_libfr

test_libfr_SOURCES = test.c fr.c
test_libfr_CFLAGS = $(cache_opt) $(mmap_opt) -Iinclude 
test_libfr_LDFLAGS = -lrt -lpthread

