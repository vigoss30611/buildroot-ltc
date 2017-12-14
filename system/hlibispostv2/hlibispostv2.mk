HLIBISPOSTV2_VERSION = 1.0.0
HLIBISPOSTV2_SOURCE =
HLIBISPOSTV2_SITE  =

HLIBISPOSTV2_LICENSE =
HLIBISPOSTV2_LICENSE_FILES = README

HLIBISPOSTV2_MAINTAINED = YES
HLIBISPOSTV2_AUTORECONF = YES
HLIBISPOSTV2_INSTALL_STAGING = YES
HLIBISPOSTV2_DEPENDENCIES = host-pkgconf

define HLIBISPOSTV2_POST_INSTALL_STAGING_HEADERS
	mkdir -p $(STAGING_DIR)/usr/include/hlibispostv2
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	cp -rfv $(@D)/*.h  $(STAGING_DIR)/usr/include/hlibispostv2
  cp -rfv $(@D)/hlibispostv2.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	mkdir -p $(TARGET_DIR)/root/.ispost

	cp -rfv $(@D)/IspostData/lc_v1_0-0-0-0_hermite32_640x480-640x480.bin $(TARGET_DIR)/root/.ispost/lc_v1_0-0-0-0_hermite32_640x480-640x480.bin
	cp -rfv $(@D)/IspostData/lc_v1_0-0-0-0_hermite32_1280x720-1280x720.bin $(TARGET_DIR)/root/.ispost/lc_v1_0-0-0-0_hermite32_1280x720-1280x720.bin
	cp -rfv $(@D)/IspostData/lc_v1_0-0-0-0_hermite32_1920x1080-1920x1080.bin $(TARGET_DIR)/root/.ispost/lc_v1_0-0-0-0_hermite32_1920x1080-1920x1080.bin
	
	case $$(ls -lh $(TOPDIR)/output/product | awk -F/ '{print $$NF}') in \
		fc01) \
			cp -rfv $(@D)/IspostData/lc_v1_0-0-0-0_hermite32_1920x1088-1920x1088.bin $(TARGET_DIR)/root/.ispost/lc_v1_0-0-0-0_hermite32_1920x1088-1920x1088.bin \
		;; \
		qiwo_304 | q3evb_v1.1_spi | libra_v2.0 | q3evb_v1.1) \
			cp -rfv $(@D)/IspostData/lc_v1_hermite32_1920x1088_scup_0~30.bin $(TARGET_DIR)/root/.ispost/lc_v1_hermite32_1920x1088_scup_0~30.bin \
		;; \
		q3fevb_va | q3fevb_va_eMMC) \
			cp -rfv $(@D)/IspostData/lc_v1_0-0-0-0_hermite16_1920x1088-1920x1088.bin $(TARGET_DIR)/root/.ispost \
		;; \
		*)	\
	  ;;	\
	esac
endef
HLIBISPOSTV2_POST_INSTALL_STAGING_HOOKS  += HLIBISPOSTV2_POST_INSTALL_STAGING_HEADERS

$(eval $(autotools-package))

