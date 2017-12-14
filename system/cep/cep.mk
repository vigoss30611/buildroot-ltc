################################################################################
#
#common event provider
#
################################################################################

CEP_VERSION = 1.0.0
CEP_SOURCE = 
CEP_SITE  = 

CEP_LICENSE = 
CEP_LICENSE_FILES = README

CEP_MAINTAINED = YES
CEP_AUTORECONF = YES
CEP_INSTALL_STAGING = YES
CEP_DEPENDENCIES = host-pkgconf qlibsys

#install headers
define CEP_POST_INSTALL_STAGING_HEADERS
	echo "--------++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/cep.h  $(STAGING_DIR)/usr/include/qsdk
endef

CEP_POST_INSTALL_STAGING_HOOKS  += CEP_POST_INSTALL_STAGING_HEADERS

$(eval $(autotools-package))

