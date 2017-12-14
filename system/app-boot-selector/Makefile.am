###############################################
#commer event provider process
###############################################
boot_selector_common_SOURCES= boot_selector.c
#ACLOCAL_AMFLAGS = -I m4
#get static lib
EXTRA_CFLAGS=  -I$(srcdir)/include -I$(srcdir)/ -pthread

##get eventd
bin_PROGRAMS=boot_selector
boot_selector_SOURCES= $(boot_selector_common_SOURCES)
boot_selector_CFLAGS= -g -O0 $(EXTRA_CFLAGS) $(LIBEVENT_CFLAGS) $(LIBSYSTEM_CFLAGS)
boot_selector_LDFLAGS= $(LIBEVENT_LIBS) $(LIBSYSTEM_LIBS) 

