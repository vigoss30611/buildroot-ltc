###############################################
#cma process
###############################################
cma_common_SOURCES=test.c
#ACLOCAL_AMFLAGS = -I m4
#get static lib
EXTRA_CFLAGS=  -I$(srcdir)/include -I$(srcdir)/ -pthread

##get cma
bin_PROGRAMS=cma
#noinst_PROGRAMS=cma
cma_SOURCES= $(cma_common_SOURCES)
cma_CFLAGS= $(EXTRA_CFLAGS) 
cma_LDFLAGS= -version-info 1:0:0

