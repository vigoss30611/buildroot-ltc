AUTOMAKE_OPTIONS=foreign
EXTRA_CFLAGS=  -I$(srcdir)/include -I$(srcdir)/ -pthread \
				$(LIBITEMS_CFLAGS) 

lib_common_SOURCES=libupgrade.c ius_util.c spiblk_util.c emmc_util.c 
LIB_EXTRA_CFLAGS =  -I$(srcdir)/include  -I. 
LIB_EXTRA_CFLAGS += $(EXTRA_CFLAGS)
LIB_EXTRA_LDFLAGS=  -version-info 1:0:0 \
					$(LIBSYSTEM_LIBS) \
					$(LIBITEMS_LIBS) \
					-lz

lib_LIBRARIES=libupgrade.a
libupgrade_a_CPPFLAGS= $(LIB_EXTRA_CFLAGS)
libupgrade_a_LDFLAGS=  $(LIB_EXTRA_LDFLAGS)
libupgrade_a_SOURCES=${lib_common_SOURCES}


lib_LTLIBRARIES=libupgrade.la
libupgrade_la_CPPFLAGS= $(LIB_EXTRA_CFLAGS)
libupgrade_la_LDFLAGS=  $(LIB_EXTRA_LDFLAGS)
libupgrade_la_SOURCES=${lib_common_SOURCES}

bin_PROGRAMS=upgrade
upgrade_SOURCES = upgrade.c libupgrade.c ius_util.c spiblk_util.c emmc_util.c 
upgrade_CFLAGS= $(EXTRA_CFLAGS)
upgrade_LDFLAGS= -version-info 1:0:0 \
				$(LIBSYSTEM_LIBS) \
				$(LIBITEMS_LIBS) \
				-lz
