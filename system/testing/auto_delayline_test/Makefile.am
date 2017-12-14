###############################################
#delayline auto test process
###############################################
EXTRA_CFLAGS= -I./ -I$(srcdir)/include -I$(srcdir)/ -Iinclude -Wall -Werror \
	       $(LIBITEMS_CFLAGS) \
	       $(LIBWIFI_CFLAGS)	\
	       $(EVENT_CFLAGS)
##get auto_delayline
bin_PROGRAMS=auto_delayline

auto_delayline_SOURCES= auto_delayline.c
auto_delayline_CFLAGS= $(EXTRA_CFLAGS)
auto_delayline_LDFLAGS=	-version-info 2:0:0	\
				$(LIBFR_LIBS)	\
				$(LIBALSA_LIBS)	\
				$(LIBSYSTEM_LIBS) \
				$(LIBITEMS_LIBS)  \
				$(LIBWIFI_LIBS)	\
				$(EVENT_LIBS)

