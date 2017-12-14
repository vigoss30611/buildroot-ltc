AUTOMAKE_OPTIONS=foreign
EXTRA_CFLAGS=  -I$(srcdir)/include -Iinclude -pthread -Wall -Werror \
				$(LIBITEMS_CFLAGS) \
				$(LIBWIFI_CFLAGS) \
				$(EVENT_CFLAGS) \
				$(LIBAUDIOBOX_CFLAGS) \
				$(LIBFR_CFLAGS) \
				$(LIBCODECS_CFLAGS) \
				$(LIBDEMUX_CFLAGS) \
				$(LIBVIDEOBOX_CFLAGS) \
				$(LIBQVPLAY_CFLAGS)

##get sysserver
bin_PROGRAMS=d304ft
d304ft_SOURCES = d304ft.c
d304ft_CFLAGS= $(EXTRA_CFLAGS)
d304ft_LDFLAGS= -version-info 1:0:0 \
				$(LIBITEMS_LIBS)  \
				$(LIBWIFI_LIBS) \
				$(LIBSYSTEM_LIBS) \
				$(EVENT_LIBS) \
				$(LIBVIDEOBOX_LIBS) \
				$(LIBAUDIOBOX_LIBS) \
				$(LIBFR_LIBS) \
				$(LIBDEMUX_LIBS) \
				-lqvplay \
				-lcodecs
