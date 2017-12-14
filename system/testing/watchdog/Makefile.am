AUTOMAKE_OPTIONS=foreign

bin_PROGRAMS = wdt_test
wdt_test_SOURCES = wdt_test.c
wdt_test_CFLAGS= -I./ -I$(top_srcdir)/ -pthread -Wall -Werror
wdt_test_LDFLAGS= -version-info 1:0:0 -pthread

