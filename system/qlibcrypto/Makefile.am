AUTOMAKE_OPTIONS=foreign

lib_LTLIBRARIES = libcrypto_rsa.la

libcrypto_rsa_la_SOURCES = crypto.c 

libcrypto_rsa_la_CFLAGS = -I$(top_srcdir)/include \
					 -g -Wall -Werror  \
					$(LIBFR_CFLAGS) $(LIBEVENT_CFLAGS) \
					$(LIBCRYPTO_CFLAGS)

libcrypto_rsa_la_LDFLAGS = $(LIBFR_LIBS) $(LIBEVENT_LIBS)	\
				  -lssl \
				  -lcrypto \
					$(LIBCRYPTO_LIBS)

AM_CFLAGS=-I$(top_srcdir)/include/  -I. -pthread -Wall -Werror \
				-D_GNU_SOURCE \
				$(LIBITEMS_CFLAGS) \
				$(EVENT_CFLAGS)
				


bin_PROGRAMS=crypto_test
crypto_test_SOURCES = crypto_test.c ${libcrypto_rsa_la_SOURCES}
crypto_test_CFLAGS= $(AM_CFLAGS) -ggdb
crypto_test_LDFLAGS= -version-info 1:0:0 \
				$(LIBITEMS_LIBS)  \
				$(EVENT_LIBS)  \
				$(LIBSYSTEM_LIBS) \
				-lssl \
				-lcrypto \
				-lrt \
				-liconv 

noinst_PROGRAMS=create_key
create_key_SOURCES = create_key.c ${libcrypto_rsa_la_SOURCES}
create_key_CFLAGS= $(AM_CFLAGS) -ggdb
create_key_LDFLAGS= -version-info 1:0:0 \
				$(LIBITEMS_LIBS)  \
				$(EVENT_LIBS)  \
				$(LIBSYSTEM_LIBS) \
				-lssl \
				-lcrypto \
				-lrt \
				-liconv 

#noinst_PROGRAMS=ssl_server
#ssl_server_SOURCES = ssl_server.c ${libcrypto_rsa_la_SOURCES}
#ssl_server_CFLAGS= $(AM_CFLAGS) -ggdb
#ssl_server_LDFLAGS= -version-info 1:0:0 \
#				$(LIBITEMS_LIBS)  \
#				$(EVENT_LIBS)  \
#				$(LIBSYSTEM_LIBS) \
#				-lssl \
#				-lcrypto \
#				-lrt \
#				-liconv 

#bin_PROGRAMS=ssl_client
#ssl_client_SOURCES = ssl_client.c ${libcrypto_rsa_la_SOURCES}
#ssl_client_CFLAGS= $(AM_CFLAGS) -ggdb
#ssl_client_LDFLAGS= -version-info 1:0:0 \
#				$(LIBITEMS_LIBS)  \
#				$(EVENT_LIBS)  \
#				$(LIBSYSTEM_LIBS) \
#				-lssl \
#				-lcrypto \
#				-lrt \
#				-liconv 
