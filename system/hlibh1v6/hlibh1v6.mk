################################################################################
#
# hlibh1v6
#
################################################################################

HLIBH1V6_VERSION = 1.0.0
HLIBH1V6_SOURCE =
HLIBH1V6_SITE  =

HLIBH1V6_LICENSE =
HLIBH1V6_LICENSE_FILES = README

HLIBH1V6_MAINTAINED = YES
HLIBH1V6_AUTORECONF = YES
HLIBH1V6_INSTALL_STAGING = YES
HLIBH1V6_DEPENDENCIES =  host-pkgconf

ifeq ($(BR2_PACKAGE_HLIBH1V6_GDBDEBUG),y)
HLIBH1V6_CONF_OPT += --enable-gdbdebug
endif

ifeq ($(BR2_PACKAGE_HLIBH1V6_POLL),y)
HLIBH1V6_CONF_OPT += --enable-poll
endif

ifeq ($(BR2_PACKAGE_HLIBH1V6_TESTBENCH),y)
HLIBH1V6_CONF_OPT += --enable-testbench
endif

#install headers
define HLIBH1V6_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/hlibh1v6
	cp -rfv $(@D)/inc/*.h  $(STAGING_DIR)/usr/include/hlibh1v6
	cp -rfv $(@D)/hlibh1v6.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	echo "---------------------------------------"
endef
HLIBH1V6_POST_INSTALL_STAGING_HOOKS  += HLIBH1V6_POST_INSTALL_STAGING_HEADERS



$(eval $(autotools-package))

