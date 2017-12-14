AUTOMAKE_OPTIONS=foreign
libra_dana_common_SOURCES=   base64.c  danaMain.c \
						./lib/api/av_shm.c

#ACLOCAL_AMFLAGS = -I m4
#get static lib
EXTRA_CFLAGS=  -I$(srcdir)/include -I$(srcdir)/ -I./inc \
			   -I. -I$(top_srcdir) -I$(top_srcdir)/include/ \
			   -L$(srcdir)/lib/linux_arm_q3/ \
			   -g3 -ggdb3 -O0 -march=armv4 \
			   $(LIBVIDEOBOX_CFLAGS) \
			   $(LIBQVPLAY_CFLAGS) \
			   $(LIBFR_CFLAGS)	\
			   $(LIBAUDIOBOX_CFLAGS) \
			   $(LIBCODECS_CFLAGS)

##get libra_dana
bin_PROGRAMS=libra_dana
libra_dana_SOURCES= $(libra_dana_common_SOURCES)
libra_dana_CFLAGS= $(EXTRA_CFLAGS)
libra_dana_LDFLAGS= $(LIBVIDEOBOX_LIBS) \
						$(LIBFR_LIBS) \
						$(LIBAUDIOBOX_LIBS) \
						$(LIBCODECS_LIBS) \
				-lpthread \
				-ldana_video \
				-ldl
