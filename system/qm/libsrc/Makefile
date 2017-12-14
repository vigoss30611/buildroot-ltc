include Makefile.param

OUT_LIB_DIR = $(LIBS_PATH)/$(CHIP_ARCH)/

all:
	$(if $(shell find $(OUT_LIB_DIR)), $(shell cd ./), $(shell mkdir -p $(OUT_LIB_DIR)))
	make -C Log/src
	make -C Hdmng
	make -C SystemMsg
	make -C mediabuffer/src
	make -C Basic/src
	make -C Upgrade/src
	make -C SYSCfg/src
	make -C Audio/src
	make -C VideoMarker/src
	make -C AVServer/src
	make -C Camera/src
	make -C Record/src
	make -C NetWork/src
	make -C AlarmServer/src
	make -C SRDK/src
	make -C SysTime/src
	make -C ftp/
	make -C libs/Q3
	make -C Daemon

clean:
	make -C Log/src clean
	make -C Hdmng clean
	make -C SystemMsg clean 
	make -C mediabuffer/src clean
	make -C Basic/src clean
	make -C Upgrade/src clean
	make -C SYSCfg/src clean
	make -C Audio/src clean
	make -C VideoMarker/src clean
	make -C AVServer/src clean
	make -C Record/src clean
	make -C Camera/src clean
	make -C NetWork/src clean
	make -C AlarmServer/src clean
	make -C SRDK/src clean
	make -C SysTime/src clean
	make -C ftp/ clean
	make -C libs/Q3 clean
	make -C Daemon clean
