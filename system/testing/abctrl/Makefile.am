###############################################
#event process
###############################################
bin_PROGRAMS=abctrl
abctrl_SOURCES= \
				abctrl.c \
				abctrl_capture.c	\
				abctrl_mute.c	\
				abctrl_play.c	\
				abctrl_set.c	\
				abctrl_unmute.c	\
				abctrl_encode.c	\
				abctrl_decode.c	\
				abctrl_read.c

abctrl_CFLAGS= -I./ -I$(srcdir)/include -I$(srcdir)/ \
				$(LIBAUDIOBOX_CFLAGS) $(LIBFR_CFLAGS)\
				$(LIBCODECS_CFLAGS) $(LIBALSA_CFLAGS) $(LIBVIDEODEBUG_CFLAGS)
abctrl_LDFLAGS= -version-info 1:0:0 $(LIBAUDIOBOX_LIBS) \
				$(LIBFR_LIBS) $(LIBCODECS_LIBS)	\
				$(LIBALSA_LIBS) $(LIBVIDEODEBUG_LIBS)

