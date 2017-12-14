###############################################
#mvdetect process
###############################################
mvdetect_common_SOURCES= mv.c
#ACLOCAL_AMFLAGS = -I m4
#get static lib
EXTRA_CFLAGS=  -g -I$(srcdir)/include -I$(srcdir)/ -pthread

bin_PROGRAMS=mvdetect
mvdetect_SOURCES= $(mvdetect_common_SOURCES)
mvdetect_CFLAGS= $(EXTRA_CFLAGS) $(LIBVIDEOBOX_CFLAGS)
mvdetect_LDFLAGS= -version-info 1:0:0 $(LIBVIDEOBOX_LIBS)

