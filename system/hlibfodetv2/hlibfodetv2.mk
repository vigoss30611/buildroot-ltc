HLIBFODETV2_VERSION = 1.0.0
HLIBFODETV2_SOURCE = 
HLIBFODETV2_SITE  = 

HLIBFODETV2_LICENSE = 
HLIBFODETV2_LICENSE_FILES = README

HLIBFODETV2_MAINTAINED = YES
HLIBFODETV2_AUTORECONF = YES
HLIBFODETV2_INSTALL_STAGING = YES
HLIBFODETV2_DEPENDENCIES = host-pkgconf

define HLIBFODETV2_POST_INSTALL_STAGING_HEADERS
	mkdir -p $(STAGING_DIR)/usr/include/hlibfodetv2
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	cp -rfv $(@D)/*.h  $(STAGING_DIR)/usr/include/hlibfodetv2
	cp -rfv $(@D)/hlibfodetv2.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
    
	mkdir -p $(TARGET_DIR)/root/.fodet
	cp -rfv $(@D)/FodetData/fodet_db_haar.bin $(TARGET_DIR)/root/.fodet/fodet_db_haar.bin

endef
HLIBFODETV2_POST_INSTALL_STAGING_HOOKS  += HLIBFODETV2_POST_INSTALL_STAGING_HEADERS

$(eval $(autotools-package))

