
##get sysserver
###############################################
#libvcp7g
###############################################
AUTOMAKE_OPTIONS=foreign

if HAVE_DSPSUPPORT
DSPSUPPORT_SOURCES= vcp7g_dsp.c vcp7g_dbg.c
DSPSUPPORT_CFLAGS= -DAEC_DSP_SUPPORT $(LIBDSP_CFLAGS)
DSPSUPPORT_LIBS= $(LIBDSP_LIBS)
else
DSPSUPPORT_SOURCES=
DSPSUPPORT_CFLAGS=
DSPSUPPORT_LIBS=
endif

if HAVE_ARMSUPPORT
ARMSUPPORT_SOURCES= vcp7g_arm.c
ARMSUPPORT_CFLAGS= -DAEC_ARM_SUPPORT
ARMSUPPORT_LIBS= -L$(top_srcdir)/lib/ -lvcp 
else
ARMSUPPORT_SOURCES=
ARMSUPPORT_CFLAGS=
ARMSUPPORT_LIBS=
endif

#lib_common_SOURCES=vcp7g.c vcp7g_dsp.c vcp7g_arm.c
lib_common_SOURCES= vcp7g.c $(DSPSUPPORT_SOURCES) $(ARMSUPPORT_SOURCES)


LIB_EXTRA_CFLAGS=  -I$(srcdir)/include  -I$(srcdir) \
				   -I$(srcdir)/include/vcp7g/ \
					$(DSPSUPPORT_CFLAGS) \
					$(ARMSUPPORT_CFLAGS) \
					$(LIBCODECS_CFLAGS)

LIB_EXTRA_LDFLAGS=  -version-info 1:0:0 \
					$(LIBSYSTEM_LIBS) \
					-lpthread \
					-lrt \
					$(DSPSUPPORT_LIBS) \
					$(ARMSUPPORT_LIBS) \
					$(LIBCODECS_LIBS)

lib_LIBRARIES = libvcp7g.a
libvcp7g_a_SOURCES = ${lib_common_SOURCES}
libvcp7g_a_CFLAGS= $(LIB_EXTRA_CFLAGS)
libvcp7g_a_LDFLAGS=  $(LIB_EXTRA_LDFLAGS)
#libvcp7g_a_LIBADD=$(top_srcdir)/lib/libvcp.a

###############################################
#vcp7g process
###############################################
AUTOMAKE_OPTIONS=foreign
#EXTRA_CFLAGS=  -I$(srcdir)/include/vcp7g/ -pthread \
#				$(LIBCODECS_CFLAGS)

bin_PROGRAMS= test_libvcp7g

test_libvcp7g_SOURCES = $(lib_common_SOURCES) vcp_example.c
test_libvcp7g_CFLAGS = -O2 -Wall -W -march=armv7-a -mthumb-interwork \
			  -mfloat-abi=hard -mcpu=cortex-a7 -mfpu=neon \
			  -ftree-vectorize -Wno-unused -ffunction-sections \
			  $(LIB_EXTRA_CFLAGS)
test_libvcp7g_LDFLAGS = -version-info 1:0:0 \
				-lpthread \
				-lrt \
			    $(LIB_EXTRA_LDFLAGS)
#test_libvcp7g_LDADD=$(top_srcdir)/lib/libvcp.a
