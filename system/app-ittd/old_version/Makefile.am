###############################################
#IQ Test process
###############################################
ittd_common_SOURCES= IQTest.c
AUTOMAKE_OPTIONS=foreign
EXTRA_CFLAGS=  -I$(srcdir)/include -I$(srcdir)/ -pthread -lrtsp -lfr -lvideobox -laudiobox -lcodecs -ldemux

##get ittd
bin_PROGRAMS=ittd
ittd_SOURCES= $(ittd_common_SOURCES)
ittd_CFLAGS= $(EXTRA_CFLAGS)
ittd_LDFLAGS= $(LIBRTSP_LIBS)
