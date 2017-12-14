################################################################################
#
#
#
################################################################################

QLIBVPLAY_VERSION = 1.0.0
QLIBVPLAY_SOURCE =
QLIBVPLAY_SITE  =

QLIBVPLAY_LICENSE =
QLIBVPLAY_LICENSE_FILES = README

QLIBVPLAY_MAINTAINED = YES
QLIBVPLAY_AUTORECONF = YES
QLIBVPLAY_INSTALL_STAGING = YES
QLIBVPLAY_DEPENDENCIES = host-pkgconf  libffmpeg videobox audiobox libdemux libcodecs libh264bitstream

ifeq ($(BR2_PACKAGE_QLIBVPLAY_GDBDEBUG),y)
QLIBVPLAY_CONF_OPT += --enable-gdbdebug
endif

ifeq ($(BR2_PACKAGE_QLIBVPLAY_TEST_UTILS),y)
QLIBVPLAY_CONF_OPT += --enable-testutils
endif

ifeq ($(BR2_PACKAGE_QLIBVPLAY_FRAGMENTED),y)
QLIBVPLAY_CONF_OPT += --enable-fragmented
endif


QLIBVPLAY_CONF_OPT += logger_level=$(BR2_PACKAGE_QLIBVPLAY_LOGLEVEL)
QLIBVPLAY_CONF_OPT += recorder_cache_second=$(BR2_PACKAGE_QLIBVPLAY_RECORDER_CACHE_TIME)
QLIBVPLAY_CONF_OPT += player_cache_second=$(BR2_PACKAGE_QLIBVPLAY_PLAYER_CACHE_TIME)



#QLIBVPLAY_CONF_OPT += logger_level=$(BR2_PACKAGE_QLIBVPLAY_LOGLEVEL)
#QLIBVPLAY_CONF_OPT += logger_level=$(call qstrip,$(BR2_PACKAGE_QLIBVPLAY_LOGLEVEL))

#install headers
define QLIBVPLAY_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/include/qsdk/*.h  $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/qvplay.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
endef
QLIBVPLAY_POST_INSTALL_STAGING_HOOKS  += QLIBVPLAY_POST_INSTALL_STAGING_HEADERS



$(eval $(autotools-package))

