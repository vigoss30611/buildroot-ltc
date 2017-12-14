################################################################################
#
# hlibh2v8
#
################################################################################

HLIBH2V4_VERSION = 1.0.0
HLIBH2V4_SOURCE =
HLIBH2V4_SITE  =

HLIBH2V4_LICENSE =
HLIBH2V4_LICENSE_FILES = README

HLIBH2V4_MAINTAINED = YES
HLIBH2V4_AUTORECONF = YES
HLIBH2V4_INSTALL_STAGING = YES
HLIBH2V4_DEPENDENCIES =  host-pkgconf  hlibfr

ifeq ($(BR2_PACKAGE_HLIBH2V4_GDBDEBUG),y)
HLIBH2V4_CONF_OPT += --enable-gdbdebug
endif


ifeq ($(BR2_PACKAGE_HLIBH2V4_POLL),y)
HLIBH2V4_CONF_OPT += --enable-poll
endif

ifeq ($(BR2_PACKAGE_HLIBH2V4_TESTBENCH),y)
HLIBH2V4_CONF_OPT += --enable-testbench
endif

#install headers
define HLIBH2V4_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/hlibh2v4
	cp -rfv $(@D)/inc/*.h  $(STAGING_DIR)/usr/include/hlibh2v4
	cp -rfv $(@D)/source/common/*.h  $(STAGING_DIR)/usr/include/hlibh2v4
	cp -rfv $(@D)/source/hevc/*.h  $(STAGING_DIR)/usr/include/hlibh2v4
	cp -rfv $(@D)/hlibh2v4.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	echo "---------------------------------------"
endef
HLIBH2V4_POST_INSTALL_STAGING_HOOKS  += HLIBH2V4_POST_INSTALL_STAGING_HEADERS



$(eval $(autotools-package))

