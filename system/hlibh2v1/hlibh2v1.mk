################################################################################
#
# hlibh2v1
#
################################################################################

HLIBH2V1_VERSION = 1.0.0
HLIBH2V1_SOURCE =
HLIBH2V1_SITE  =

HLIBH2V1_LICENSE =
HLIBH2V1_LICENSE_FILES = README

HLIBH2V1_MAINTAINED = YES
HLIBH2V1_AUTORECONF = YES
HLIBH2V1_INSTALL_STAGING = YES
HLIBH2V1_DEPENDENCIES =  host-pkgconf

ifeq ($(BR2_PACKAGE_HLIBH2V1_GDBDEBUG),y)
HLIBH2V1_CONF_OPT += --enable-gdbdebug
endif


ifeq ($(BR2_PACKAGE_HLIBH2V1_POLL),y)
HLIBH2V1_CONF_OPT += --enable-poll
endif

ifeq ($(BR2_PACKAGE_HLIBH2V1_TESTBENCH),y)
HLIBH2V1_CONF_OPT += --enable-testbench
endif

#install headers
define HLIBH2V1_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/hlibh2v1
	cp -rfv $(@D)/inc/*.h  $(STAGING_DIR)/usr/include/hlibh2v1
	cp -rfv $(@D)/source/common/*.h  $(STAGING_DIR)/usr/include/hlibh2v1
	cp -rfv $(@D)/source/hevc/*.h  $(STAGING_DIR)/usr/include/hlibh2v1
	cp -rfv $(@D)/hlibh2v1.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	echo "---------------------------------------"
endef
HLIBH2V1_POST_INSTALL_STAGING_HOOKS  += HLIBH2V1_POST_INSTALL_STAGING_HEADERS



$(eval $(autotools-package))

