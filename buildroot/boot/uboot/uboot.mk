################################################################################
#
# uboot
#
################################################################################

UBOOT_VERSION    = lite

UBOOT_LICENSE = GPLv2+
UBOOT_LICENSE_FILES = COPYING

UBOOT_INSTALL_IMAGES = YES

UBOOT_MAINTAINED = YES

UBOOT_CONFIGURE_OPTS += CONFIG_NOSOFTFLOAT=1
UBOOT_MAKE_OPTS += \
	CROSS_COMPILE="$(CCACHE) $(TARGET_CROSS)" \
	ARCH=$(UBOOT_ARCH)

UBOOT_SRCDIR = $(BR2_PACKAGE_UBOOT_LOCAL_PATH)
ifeq ($(BR2_PACKAGE_UBOOT_BOOT_DEVICE_SPI_FLASH),y)
UBOOT_MAKE_OPTS          += "BOARD_BOOT_DEV_FLASH=y"
else ifeq ($(BR2_PACKAGE_UBOOT_BOOT_DEVICE_EMMC),y)
UBOOT_MAKE_OPTS          += "BOARD_BOOT_DEV_EMMC=y"
else ifeq ($(BR2_PACKAGE_UBOOT_BOOT_DEVICE_MMC),y)
UBOOT_MAKE_OPTS          += "not support"
else
#UBOOT_MAKE_OPTS          +=  
UBOOT_MAKE_OPTS          += "MODE=SIMPLE ERASE_SPIFLASH=n"
endif

ifeq ($(BR2_TARGET_UBOOT_BOOT_PLAT_FPGA),y)
UBOOT_MAKE_OPTS += "BOARD_PLAT_FPGA=y"
else
UBOOT_MAKE_OPTS += "BOARD_PLAT_ASIC=y"
endif

ifeq ($(BR2_PACKAGE_UBOOT_BURN_DEVICE_MMC),y)
UBOOT_MAKE_OPTS += "BOARD_HAVE_SD_BURN=y"
endif

ifeq ($(BR2_PACKAGE_UBOOT_BURN_DEVICE_UDC),y)
UBOOT_MAKE_OPTS += "BOARD_HAVE_UDC_BURN=y"
endif

ifeq ($(BR2_PACKAGE_IROM_SKIP_MMC_CHECK),y)
UBOOT_MAKE_OPTS += "BOARD_IROM_SKIP_MMC_CHECK=y"
endif

UBOOT_ARCH=$(KERNEL_ARCH)



define UBOOT_BUILD_CMDS
	echo "UBOOT-> "$(UBOOT_SRCDIR)
	cp -rf  $(UBOOT_SRCDIR)/*  $(UBOOT_BUILDDIR)
	@echo "--------------->begin build"
	echo "uboot -> "$(UBOOT_MAKE_OPTS)
	$(TARGET_CONFIGURE_OPTS) $(UBOOT_CONFIGURE_OPTS) 	\
		$(MAKE) -C $(@D) $(UBOOT_MAKE_OPTS) 		\
		$(UBOOT_MAKE_TARGET)
endef


define UBOOT_INSTALL_IMAGES_CMDS
	@echo "---> install uboot"
	cp -fv $(@D)/out/uboot_lite.isi   $(BINARIES_DIR)/uboot0.isi
endef


$(eval $(generic-package))

