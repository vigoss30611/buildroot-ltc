AUTOMAKE_OPTIONS=foreign subdir-objects
lib_LTLIBRARIES=libwifi.la
libwifi_la_CFLAGS=-I$(top_srcdir)/include/  -I. -pthread -Wall -Werror \
				$(EVENT_CFLAGS)
libwifi_la_LDFLAGS= -avoid-version\
				$(EVENT_LIBS)
libwifi_la_SOURCES=wifi_manager.c

AM_CFLAGS=-I$(top_srcdir)/include/  -I. -pthread -Wall -Werror \
				-D_GNU_SOURCE \
				$(LIBITEMS_CFLAGS) \
				$(EVENT_CFLAGS)

bin_PROGRAMS=wifi ap_state
bin_common_SOURCES = wifi.c lib_wifi.c ifc_utils.c dhcpclient.c dhcpmsg.c packet.c WPA/wpa_cli.c WPA/utils/common.c WPA/utils/edit_readline.c \
			WPA/utils/eloop.c WPA/utils/os_unix.c WPA/utils/trace.c WPA/utils/wpa_ctrl.c WPA/utils/wpa_debug.c
bin_common_SOURCES += wifi_hal.c
wifi_SOURCES = ${bin_common_SOURCES}

wifi_CFLAGS= $(AM_CFLAGS)
wifi_LDFLAGS=  \
				$(LIBITEMS_LIBS)  \
				$(EVENT_LIBS)  \
				$(LIBSYSTEM_LIBS) \
				-lreadline\
				-lssl \
				-lrt \
				-lcrypto \
				-liconv

noinst_PROGRAMS=wifi_test
wifi_test_SOURCES = wifi_test.c ${libwifi_la_SOURCES}
wifi_test_CFLAGS= $(AM_CFLAGS) -ggdb
wifi_test_LDFLAGS=  \
				$(LIBITEMS_LIBS)  \
				$(EVENT_LIBS)  \
				$(LIBSYSTEM_LIBS) \
				-lreadline\
				-lssl \
				-lrt \
				-liconv

ap_state_SOURCES = ap_state.c
ap_state_CFLAGS= $(AM_CFLAGS)
ap_state_LDFLAGS=  \
				-lreadline\
				$(EVENT_LIBS)
