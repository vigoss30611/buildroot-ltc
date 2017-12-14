EXTRA_CFLAGS=  -I$(srcdir)/include -Iinclude -pthread -Wall -Werror \
				$(LIBITEMS_CFLAGS) \
				$(EVENT_CFLAGS) \
				$(LIBAUDIOBOX_CFLAGS) \
				$(LIBFR_CFLAGS) \
				$(LIBCODECS_CFLAGS) \
				$(LIBDEMUX_CFLAGS) \
				$(LIBVIDEOBOX_CFLAGS)

bin_PROGRAMS=decode_display
decode_display_SOURCES = decode_display.c
decode_display_CFLAGS= $(EXTRA_CFLAGS)
decode_display_LDFLAGS= -version-info 1:0:0 \
				$(LIBITEMS_LIBS)  \
				$(LIBSYSTEM_LIBS) \
				$(EVENT_LIBS) \
				$(LIBVIDEOBOX_LIBS) \
				$(LIBAUDIOBOX_LIBS) \
				$(LIBFR_LIBS) \
				$(LIBCODECS_LIBS) \
				$(LIBDEMUX_LIBS)

