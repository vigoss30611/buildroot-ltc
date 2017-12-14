################################################################################
#
# qm
#
################################################################################

QM_VERSION    = lite

QM_LICENSE = GPLv2+
QM_LICENSE_FILES = COPYING

QM_INSTALL_IMAGES = YES

QM_MAINTAINED = YES
QM_DEPENDENCIES = host-pkgconf hlibitems hlibfr qlibwifi qlibsys eventhub audiobox libcodecs videobox libdemux qlibvplay qlibupgrade qm-hal libiconv

QM_CONFIGURE_OPTS =
QM_MAKE_OPTS += \
	CROSS_COMPILE="$(CCACHE) $(TARGET_CROSS)" \
	ARCH=$(UBOOT_ARCH)

QM_HEADERS_TO_INSTALL = libsrc/common/inc/QMAPIType.h
QM_HEADERS_TO_INSTALL += libsrc/common/inc/QMAPINetSdk.h
QM_HEADERS_TO_INSTALL += libsrc/common/inc/QMAPICommon.h
QM_HEADERS_TO_INSTALL += libsrc/common/inc/QMAPIDataType.h
QM_HEADERS_TO_INSTALL += libsrc/common/inc/QMAPI.h
QM_HEADERS_TO_INSTALL += libsrc/common/inc/QMAPIErrno.h
QM_HEADERS_TO_INSTALL += libsrc/common/inc/TLNVSDK.h

QM_HEADERS_TO_INSTALL += libsrc/Basic/inc/qm_event.h
QM_HEADERS_TO_INSTALL += libsrc/Basic/inc/basic.h

QM_HEADERS_TO_INSTALL += libsrc/Audio/inc/AudioPrompt.h
QM_HEADERS_TO_INSTALL += libsrc/Audio/inc/Q3Audio.h

QM_HEADERS_TO_INSTALL += libsrc/AVServer/inc/QMAPIAV.h
QM_HEADERS_TO_INSTALL += libsrc/AlarmServer/inc/QMAPIAlarmServer.h
QM_HEADERS_TO_INSTALL += libsrc/VideoMarker/inc/VideoMarker.h
QM_HEADERS_TO_INSTALL += libsrc/Camera/inc/libCamera.h
QM_HEADERS_TO_INSTALL += libsrc/Record/inc/QMAPIRecord.h

QM_HEADERS_TO_INSTALL += libsrc/NetWork/inc/QMAPINetWork.h
QM_HEADERS_TO_INSTALL += libsrc/NetWork/inc/q3_wireless.h
QM_HEADERS_TO_INSTALL += libsrc/NetWork/inc/sys_fun_interface.h
QM_HEADERS_TO_INSTALL += libsrc/NetWork/inc/checkip.h
QM_HEADERS_TO_INSTALL += libsrc/ftp/inc/libftp.h

QM_HEADERS_TO_INSTALL += libsrc/SYSCfg/inc/sysconfig.h
QM_HEADERS_TO_INSTALL += libsrc/Hdmng/hd_mng.h
QM_HEADERS_TO_INSTALL += libsrc/Upgrade/inc/QMAPIUpgrade.h

QM_HEADERS_TO_INSTALL += libsrc/SRDK/inc/tl_common.h
QM_HEADERS_TO_INSTALL += libsrc/SRDK/inc/tlevent.h
QM_HEADERS_TO_INSTALL += libsrc/SRDK/inc/tl_input_osd_module.h
QM_HEADERS_TO_INSTALL += libsrc/SRDK/inc/TLNVSDK.h
QM_HEADERS_TO_INSTALL += libsrc/SRDK/inc/tl_authen_interface.h

QM_HEADERS_TO_INSTALL += libsrc/Log/inc/logmng.h
QM_HEADERS_TO_INSTALL += libsrc/Log/inc/libsyslogapi.h
QM_HEADERS_TO_INSTALL += libsrc/SysTime/inc/systime.h
QM_HEADERS_TO_INSTALL += libsrc/SystemMsg/system_msg.h

ifeq ($(BR2_QM_ENCODE_CONTROL_ENABLE),y)
	QM_CFLAGS_OPTS += -DCONFIG_ENCODE_CONTROL_ENABLE
endif

QM_MAKE_OPTS += "QM_CFLAGS = $(QM_CFLAGS_OPTS)"

#install compile
define QM_INSTALL_COMPILE_HEADERS
@echo "++++++++++++++++install staging++++++++++++++++"
mkdir -p $(QM_BUILDDIR)/libinc/
	for i in $(QM_HEADERS_TO_INSTALL); do \
		cp -rfv $(@D)/$$i $(QM_BUILDDIR)/libinc/`basename $$i`; \
	done;
endef

define QM_BUILD_CMDS
	echo "QM-> "$(QM_SRCDIR)
    cp -rf  $(QM_SRCDIR)/*  $(QM_BUILDDIR)
	$(QM_INSTALL_COMPILE_HEADERS)
	@echo "--------------->begin build"
	echo "QM -> "$(QM_MAKE_OPTS)
	$(TARGET_CONFIGURE_OPTS) $(QM_CONFIGURE_OPTS) 	\
		$(MAKE) -C $(@D) $(QM_MAKE_OPTS) 		\
		$(QM_MAKE_TARGET)
endef

define QM_INSTALL_TARGET_CMDS
	@echo "---> install qm"
	cp -fv $(@D)/libs/Q3/libQM.so  $(TARGET_DIR)/usr/lib/
	cp -fv $(@D)/libs/Q3/main/qm_app  $(TARGET_DIR)/usr/bin/qm_app
	cp -fv $(@D)/libs/Q3/Daemon/qm_daemon  $(TARGET_DIR)/usr/bin/qm_daemon
	mkdir -p $(TARGET_DIR)/root
	mkdir -p $(TARGET_DIR)/root/qm_app
	mkdir -p $(TARGET_DIR)/root/qm_app/res
	cp -rfv $(@D)/S904APP $(TARGET_DIR)/etc/init.d/
	cp -rfv $(@D)/res/thttpd.conf $(TARGET_DIR)/etc/
	cp -rfv $(@D)/res/web $(TARGET_DIR)/root/qm_app/res/
	cp -rfv $(@D)/res/sound $(TARGET_DIR)/root/qm_app/res/

	cp -fv $(@D)/libs/Q3/ProtocolService/libProtocolService.so  $(TARGET_DIR)/usr/lib/libProtocolService.so
	#cp -fv $(@D)/libs/Q3/libhik/libhik.so  $(TARGET_DIR)/usr/lib/libhik.so
	cp -fv $(@D)/libs/Q3/libonvif/libonvif.so  $(TARGET_DIR)/usr/lib/libonvif.so
	#cp -fv $(@D)/libs/Q3/libxmai/libxmai.so  $(TARGET_DIR)/usr/lib/libxmai.so
	#cp -fv $(@D)/libs/Q3/libzhinuo/libzhinuo.so  $(TARGET_DIR)/usr/lib/libzhinuo.so
	cp -fv $(@D)/libs/Q3/libthttpd/libthttpd.so  $(TARGET_DIR)/usr/lib/libthttpd.so
endef


$(eval $(generic-package))

