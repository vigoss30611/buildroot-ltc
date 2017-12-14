AUTOMAKE_OPTIONS=foreign

bin_PROGRAMS = ota_test
ota_test_SOURCES = ota_test.c
ota_test_CFLAGS= -I./ -I$(top_srcdir)/ $(LIBSYSTEM_CFLAGS) $(LIBUPGRADE_CFLAGS) -pthread -Wall -Werror
ota_test_LDFLAGS= -version-info 1:0:0 $(LIBSYSTEM_LIBS) $(LIBUPGRADE_LIBS) -pthread

