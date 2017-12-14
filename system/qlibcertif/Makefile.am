AUTOMAKE_OPTIONS=foreign
lib_common_SOURCES=certificate.c

lib_LIBRARIES=libcertif.a
libcertif_a_SOURCES = ${lib_common_SOURCES}
libcertif_a_CFLAGS = -I$(top_srcdir)/include/ \
				   -Iinclude/qsdk/ -pthread \
				   -Wall -Werror
if HAVE_CERTIFTEST
libcertif_a_CFLAGS += -DCERTIF_TEST
endif
libcertif_a_LDFLAGS= -version-info 1:0:0 \
					 -lssl -lcrypto

lib_LTLIBRARIES=libcertif.la
libcertif_la_SOURCES = ${lib_common_SOURCES}
libcertif_la_CFLAGS = -I$(top_srcdir)/include/ \
					  -Iinclude/qsdk/ -pthread \
					  -Wall -Werror
if HAVE_CERTIFTEST
libcertif_la_CFLAGS += -DCERTIF_TEST
endif
libcertif_la_LDFLAGS= -version-info 1:0:0 \
					  -lssl -lcrypto

if HAVE_CERTIFTEST
bin_PROGRAMS = certif_test_hacked
certif_test_hacked_SOURCES = certif_test_hacked.c
certif_test_hacked_CFLAGS = -I$(top_srcdir)/include -Iinclude/qsdk/ -Wall -Werror -DCERTIF_TEST
certif_test_hacked_LDFLAGS = -lssl -lcrypto $(LIBSYSTEM_LIBS)

bin_PROGRAMS += certif_test
certif_test_SOURCES = certificate.c certif_test.c
certif_test_CFLAGS = -I$(top_srcdir)/include -Iinclude/qsdk/ -Wall -Werror -DCERTIF_TEST
certif_test_LDFLAGS = -lssl -lcrypto $(LIBSYSTEM_LIBS)
endif
