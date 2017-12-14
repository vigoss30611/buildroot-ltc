###############################################
#fodet process
###############################################
fodet_common_SOURCES= fodet.c
#ACLOCAL_AMFLAGS = -I m4
#get static lib
EXTRA_CFLAGS=  -I$(srcdir)/include -I$(srcdir)/ -pthread

##get fodet
bin_PROGRAMS=fodet
#noinst_PROGRAMS=fodet
fodet_SOURCES= $(fodet_common_SOURCES)
fodet_CFLAGS= $(EXTRA_CFLAGS) $(LIBVIDEOBOX_CFLAGS)
fodet_LDFLAGS= -version-info 1:0:0 $(LIBVIDEOBOX_LIBS)

