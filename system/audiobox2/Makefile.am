
lib_LTLIBRARIES = libaudiobox.la

libaudiobox_la_SOURCES = audio_streamops.c \
					  audio_rpc.c		\
					  audio_chn.c

libaudiobox_la_CFLAGS = -I$(top_srcdir)/include \
					 -g -Wall -Werror\
					$(LIBFR_CFLAGS) $(LIBEVENT_CFLAGS) \
					$(LIBALSA_CFLAGS) $(LIBVCP7G_CFLAGS)


libaudiobox_la_LDFLAGS = $(LIBFR_LIBS) $(LIBEVENT_LIBS)	\
					$(LIBALSA_LIBS) $(LIBVCP7G_LIBS)

bin_PROGRAMS = audiobox

audiobox_CFLAGS = -I$(top_srcdir)/include \
				-g -Wall -Werror\
				$(LIBFR_CFLAGS)	$(LIBEVENT_CFLAGS) \
				$(LIBALSA_CFLAGS) \
				$(LIBCODECS_CFLAGS) \
				$(LIBVCP7G_CFLAGS)

audiobox_LDFLAGS = -pthread -lrt -lasound	\
				$(LIBFR_LIBS) $(LIBEVENT_LIBS)	\
				$(LIBALSA_LIBS) \
				$(LIBCODECS_LIBS) \
				$(LIBVCP7G_LIBS)


audiobox_SOURCES = \
				   audiobox.c			\
				   audiobox_hal.c 			\
				   audiobox_listener.c  \
				   audiobox_ctl.c			\
				   audiobox_service.c	\
				   audio_rpc.c			\
				   audiobox_softvol.c \
				   audio_streamops.c \
				   audio_chn.c

AUTOMAKE_OPTIONS=foreign
