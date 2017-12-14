AUTOMAKE_OPTIONS = foreign
EXTRA_CFLAGS = $(UPGRADE_CFLAGS)
#lib_LTLIBRARIES = libfastboot.la

#libfastboot_la_SOURCES = fastboot.c
#libfastboot_la_CFLAGS = -Iinclude -Wall ${KERNEL_INCLUDE}
#libfastboot_la_LDFLAGS = -lpthread -version-info 1:0:0

#lib_LIBRARIES = libfastboot.a
#libfastboot_a_SOURCES = fastboot.c
#libfastboot_a_CFLAGS = -Iinclude -Wall ${KERNEL_INCLUDE}
#libfastboot_a_LDFLAGS = -lpthread

bin_PROGRAMS = fastbootd

fastbootd_SOURCES = config.c commands.c fastbootd.c protocol.c transport.c usb_linux_client.c
fastbootd_CFLAGS = -Iinclude $(EXTRA_CFLAGS)
fastbootd_LDFLAGS = -lrt -lpthread $(UPGRADE_LIBS)
