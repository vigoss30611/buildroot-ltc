################################################################################
#
# eventhub
#
################################################################################

EVENTHUB_VERSION = 1.0.0
EVENTHUB_SOURCE = 
EVENTHUB_SITE  = 

EVENTHUB_LICENSE = 
EVENTHUB_LICENSE_FILES = README

EVENTHUB_MAINTAINED = YES
EVENTHUB_AUTORECONF = YES
EVENTHUB_INSTALL_STAGING = YES
EVENTHUB_DEPENDENCIES = host-pkgconf

ifeq ($(BR2_PACKAGE_TESTING_EVENT), y)
EVENTHUB_CONF_OPT += --enable-testing
endif

#install headers
define EVENTHUB_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/event.h  $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/qsdk/*  $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/event.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
endef

EVENTHUB_POST_INSTALL_STAGING_HOOKS  += EVENTHUB_POST_INSTALL_STAGING_HEADERS

$(eval $(autotools-package))

