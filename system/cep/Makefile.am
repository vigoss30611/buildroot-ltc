###############################################
#commer event provider process
###############################################
cepd_common_SOURCES= cep.c
#ACLOCAL_AMFLAGS = -I m4
#get static lib
EXTRA_CFLAGS=  -I$(srcdir)/include -I$(srcdir)/ -pthread

##get eventd
bin_PROGRAMS=cepd
cepd_SOURCES= $(cepd_common_SOURCES)
cepd_CFLAGS= $(EXTRA_CFLAGS) $(LIBEVENT_CFLAGS)
cepd_LDFLAGS= $(LIBEVENT_LIBS) \
			  $(LIBSYSTEM_LIBS)
