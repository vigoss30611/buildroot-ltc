###############################################
#libevent
###############################################
AUTOMAKE_OPTIONS=foreign
lib_common_SOURCES=libevent.c message_queue_interface.c
LIB_EXTRA_CFLAGS=  -I$(srcdir)/include  -I$(srcdir)  
LIB_EXTRA_LDFLAGS=  -version-info 1:0:0

lib_LTLIBRARIES=libevent.la
libevent_la_CPPFLAGS= $(LIB_EXTRA_CFLAGS)
libevent_la_LDFLAGS=  $(LIB_EXTRA_LDFLAGS)
libevent_la_SOURCES=${lib_common_SOURCES}



###############################################
#event process
###############################################
eventhub_common_SOURCES= eventhub.c list.c stat.c message_queue_interface.c
#ACLOCAL_AMFLAGS = -I m4
#get static lib
EXTRA_CFLAGS=  -I$(srcdir)/include -I$(srcdir)/ -pthread

##get eventd
#bin_PROGRAMS=eventhub
eventhub_SOURCES= $(eventhub_common_SOURCES)
eventhub_CFLAGS= $(EXTRA_CFLAGS)
eventhub_LDFLAGS= -version-info 1:0:0
eventhub_LDADD=$(top_srcdir)/libevent.la

###############################################
#eventsend process
###############################################
eventsend_common_SOURCES= send/event_send.c libevent.c message_queue_interface.c
#ACLOCAL_AMFLAGS = -I m4
#get static lib
#EXTRA_CFLAGS=  -I$(srcdir)/include -I$(srcdir)/ -pthread

##get event_send
bin_PROGRAMS=eventsend eventhub 
if EVENT_TESTING
bin_PROGRAMS+=event_handler_test event_provider_test event_handler_scatter_test event_scatter_test event_handler_rpc_test event_rpc_test event_handler_scatter_rpc_test event_scatter_rpc_test
endif
eventsend_SOURCES= $(eventsend_common_SOURCES)
eventsend_CFLAGS= $(EXTRA_CFLAGS)
eventsend_LDFLAGS= -version-info 1:0:0
eventsend_LDADD=$(top_srcdir)/libevent.la

if EVENT_TESTING
event_handler_test_SOURCES= test/send/event_handler.c
event_handler_test_CFLAGS= $(EXTRA_CFLAGS)
event_handler_test_LDFLAGS= -version-info 1:0:0
event_handler_test_LDADD=$(top_srcdir)/libevent.la

event_provider_test_SOURCES= test/send/event_provider.c
event_provider_test_CFLAGS= $(EXTRA_CFLAGS)
event_provider_test_LDFLAGS= -version-info 1:0:0
event_provider_test_LDADD=$(top_srcdir)/libevent.la

event_handler_scatter_test_SOURCES= test/scatter/event_handler_scatter.c
event_handler_scatter_test_CFLAGS= $(EXTRA_CFLAGS)
event_handler_scatter_test_LDFLAGS= -version-info 1:0:0
event_handler_scatter_test_LDADD=$(top_srcdir)/libevent.la

event_scatter_test_SOURCES= test/scatter/event_scatter.c
event_scatter_test_CFLAGS= $(EXTRA_CFLAGS)
event_scatter_test_LDFLAGS= -version-info 1:0:0
event_scatter_test_LDADD=$(top_srcdir)/libevent.la

event_handler_rpc_test_SOURCES= test/rpc/event_handler_rpc.c
event_handler_rpc_test_CFLAGS= $(EXTRA_CFLAGS)
event_handler_rpc_test_LDFLAGS= -version-info 1:0:0
event_handler_rpc_test_LDADD=$(top_srcdir)/libevent.la

event_rpc_test_SOURCES= test/rpc/event_rpc.c
event_rpc_test_CFLAGS= $(EXTRA_CFLAGS)
event_rpc_test_LDFLAGS= -version-info 1:0:0
event_rpc_test_LDADD=$(top_srcdir)/libevent.la

event_handler_scatter_rpc_test_SOURCES= test/rpc_scatter/event_handler_scatter_rpc.c
event_handler_scatter_rpc_test_CFLAGS= $(EXTRA_CFLAGS)
event_handler_scatter_rpc_test_LDFLAGS= -version-info 1:0:0
event_handler_scatter_rpc_test_LDADD=$(top_srcdir)/libevent.la

event_scatter_rpc_test_SOURCES= test/rpc_scatter/event_scatter_rpc.c
event_scatter_rpc_test_CFLAGS= $(EXTRA_CFLAGS)
event_scatter_rpc_test_LDFLAGS= -version-info 1:0:0
event_scatter_rpc_test_LDADD=$(top_srcdir)/libevent.la
endif
