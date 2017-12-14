export CHIP_ARCH=Q3

include ./Makefile.param

all:
	make -C thttpd-2.25b/src
	make -C ProtocolService/src
	make -C PpcsP2p/src
	make -C RtspServer
	make -C netserver/src
	make -C onvif/cgi/onvif_ti	
	make -C main/src
	
clean:
	make -C thttpd-2.25b/src clean
	make -C ProtocolService/src clean
	make -C PpcsP2p/src clean
	make -C RtspServer clean
	make -C netserver/src clean
	make -C onvif/cgi/onvif_ti clean
	make -C main/src clean
