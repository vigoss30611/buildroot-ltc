AUTOMAKE_OPTIONS=foreign
COMMON_CFLAGS= -Werror  -std=c99
COMMON_CXXFLAGS= -Werror -std=c++11
COMMON_CXXFLAGS += -DUSE_MATH_NEON



noinst_PROGRAMS=test_gridtools
test_gridtools_SOURCES=test/main.cpp
test_gridtools_CXXFLAGS=$(COMMON_CXXFLAGS) \
			-I$(top_srcdir)/lib/inc \
			-I$(top_srcdir)


