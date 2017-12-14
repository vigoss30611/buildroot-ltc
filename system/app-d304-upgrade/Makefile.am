AUTOMAKE_OPTIONS=foreign
EXTRA_CFLAGS=  -I$(srcdir)/include -I$(srcdir)/ -pthread \
				$(LIBITEMS_CFLAGS) \
				$(LIBUPGRADE_CFLAGS) \
				$(LIBWIFI_CFLAGS)

bin_PROGRAMS=d304upgrade
d304upgrade_SOURCES = app-d304-upgrade.c net_ota_curl.c ota.c
d304upgrade_CFLAGS= $(EXTRA_CFLAGS)
d304upgrade_LDFLAGS= -version-info 1:0:0 \
				$(LIBUPGRADE_LIBS) \
				$(LIBSYSTEM_LIBS) \
				$(LIBITEMS_LIBS) \
				$(LIBWIFI_LIBS) \
				-lcurl \
				-lcrypto
