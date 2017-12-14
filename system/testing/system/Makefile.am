bin_PROGRAMS=sysset
sysset_SOURCES= sysset.c	\
			   bl_test.c	\
			   led_test.c	\
			   gpio_test.c	\
			   wdt_test.c	\
			   sys_info_test.c	\
			   storage_test.c	\
			   process_test.c	\
			   setting_test.c

sysset_CFLAGS= -I./ -I$(srcdir) $(LIBSYSTEM_CFLAGS) -pthread
sysset_LDFLAGS= -version-info 1:0:0 $(LIBSYSTEM_LIBS) -pthread
