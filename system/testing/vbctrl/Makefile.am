###############################################
#vbctrl process
###############################################
vbctrl_common_SOURCES= vb_ctrl.c marker_and_rtsp.c
#ACLOCAL_AMFLAGS = -I m4
#get static lib
EXTRA_CFLAGS=  -I$(srcdir)/include -I$(srcdir)/ -pthread

##get vbctrl
bin_PROGRAMS=vbctrl
#noinst_PROGRAMS=vbctrl
vbctrl_SOURCES= $(vbctrl_common_SOURCES)
vbctrl_CFLAGS= $(EXTRA_CFLAGS) $(LIBVIDEOBOX_CFLAGS) $(LIBVIDEODEBUG_CFLAGS)
vbctrl_LDFLAGS= -version-info 1:0:0 $(LIBVIDEOBOX_LIBS) $(LIBVIDEODEBUG_LIBS) -lrtsp $(LIBDEMUX_LIBS) $(LIBAUDIOBOX_LIBS) $(LIBCODECS_LIBS)

