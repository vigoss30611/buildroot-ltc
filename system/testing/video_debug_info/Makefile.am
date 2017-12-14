###############################################
#venc_dbg_info process
###############################################
venc_dbg_info_common_SOURCES= venc_debug_info.c
#ACLOCAL_AMFLAGS = -I m4
#get static lib
EXTRA_CFLAGS=  -g -I$(srcdir)/include -I$(srcdir)/ -pthread

bin_PROGRAMS=venc_dbg_info
venc_dbg_info_SOURCES= $(venc_dbg_info_common_SOURCES)
venc_dbg_info_CFLAGS= $(EXTRA_CFLAGS) $(LIBVIDEOBOX_CFLAGS)
venc_dbg_info_LDFLAGS= -version-info 1:0:0 $(LIBVIDEOBOX_LIBS)

