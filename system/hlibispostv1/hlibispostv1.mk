HLIBISPOSTV1_VERSION = 1.0.0
HLIBISPOSTV1_SOURCE = 
HLIBISPOSTV1_SITE  = 

HLIBISPOSTV1_LICENSE = 
HLIBISPOSTV1_LICENSE_FILES = README

HLIBISPOSTV1_MAINTAINED = YES
HLIBISPOSTV1_AUTORECONF = YES
HLIBISPOSTV1_INSTALL_STAGING = YES
HLIBISPOSTV1_DEPENDENCIES = host-pkgconf

define HLIBISPOSTV1_POST_INSTALL_STAGING_HEADERS
	mkdir -p $(STAGING_DIR)/usr/include/hlibispostv1
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	cp -rfv $(@D)/*.h  $(STAGING_DIR)/usr/include/hlibispostv1
    cp -rfv $(@D)/hlibispostv1.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
 
	mkdir -p $(TARGET_DIR)/root/.ispost
	cp -rfv $(@D)/IspostData/lc_v1_hermite32_640x480.bin $(TARGET_DIR)/root/.ispost/lc_v1_hermite32_640x480.bin
	cp -rfv $(@D)/IspostData/lc_v1_hermite32_1280x720.bin $(TARGET_DIR)/root/.ispost/lc_v1_hermite32_1280x720.bin
	cp -rfv $(@D)/IspostData/lc_v1_hermite32_1920x1080.bin $(TARGET_DIR)/root/.ispost/lc_v1_hermite32_1920x1080.bin
	cp -rfv $(@D)/IspostData/lc_v1_hermite32_1920x1088_scup_0~30.bin $(TARGET_DIR)/root/.ispost/lc_v1_hermite32_1920x1088_scup_0~30.bin
endef
HLIBISPOSTV1_POST_INSTALL_STAGING_HOOKS  += HLIBISPOSTV1_POST_INSTALL_STAGING_HEADERS

$(eval $(autotools-package))

