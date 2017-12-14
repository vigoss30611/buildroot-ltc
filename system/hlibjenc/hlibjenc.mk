################################################################################
#
# hlibjenc
#
################################################################################

HLIBJENC_VERSION = 1.0.0
HLIBJENC_SOURCE =
HLIBJENC_SITE  =

HLIBJENC_LICENSE =
HLIBJENC_LICENSE_FILES = README

HLIBJENC_MAINTAINED = YES
HLIBJENC_AUTORECONF = YES
HLIBJENC_INSTALL_STAGING = YES
HLIBJENC_DEPENDENCIES =  host-pkgconf

#install headers
define HLIBJENC_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/hlibjenc
	cp -rfv $(@D)/inc/*.h  $(STAGING_DIR)/usr/include/hlibjenc
	cp -rfv $(@D)/hlibjenc.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	echo "---------------------------------------"
endef
HLIBJENC_POST_INSTALL_STAGING_HOOKS  += HLIBJENC_POST_INSTALL_STAGING_HEADERS



$(eval $(autotools-package))

