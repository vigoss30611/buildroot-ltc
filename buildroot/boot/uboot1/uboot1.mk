################################################################################
#
# uboot1
#
################################################################################

UBOOT1_VERSION    = 1.0.0

UBOOT1_LICENSE = GPLv2+
UBOOT1_LICENSE_FILES = COPYING

UBOOT1_INSTALL_IMAGES = YES

UBOOT1_MAINTAINED = YES

UBOOT1_CONFIGURE_OPTS += CONFIG_NOSOFTFLOAT=1
UBOOT1_MAKE_OPTS += \
	CROSS_COMPILE="$(CCACHE) $(TARGET_CROSS)" \
	ARCH=$(UBOOT1_ARCH)

UBOOT1_SRCDIR = ${TOPDIR}/bootloader/uboot1
UBOOT1_ARCH=$(KERNEL_ARCH)

ifeq ($(BR2_PACKAGE_UBOOT1_Q3F),y)
UBOOT1_VER=2.0.0.2
else ifeq ($(BR2_PACKAGE_UBOOT1_APOLLO3),y)
UBOOT1_VER=2.0.0.3
else
UBOOT1_VER=2.0.0.1
endif

define UBOOT1_BUILD_CMDS
	echo "UBOOT1-> "$(UBOOT1_SRCDIR)
	cp -rf  $(UBOOT1_SRCDIR)/*  $(UBOOT1_BUILDDIR)
	@echo "--------------->begin config"
	echo "uboot1 -> "$(UBOOT1_MAKE_OPTS)
	$(TARGET_CONFIGURE_OPTS) $(UBOOT1_CONFIGURE_OPTS) 	\
		$(MAKE) $(UBOOT1_VER) -C $(@D) $(UBOOT1_MAKE_OPTS) 		\
		$(UBOOT1_MAKE_TARGET)
	$(TARGET_CONFIGURE_OPTS) $(UBOOT1_CONFIGURE_OPTS) 	\
		$(MAKE) -C $(@D) $(UBOOT1_MAKE_OPTS) 		\
		$(UBOOT1_MAKE_TARGET)
endef


define UBOOT1_INSTALL_IMAGES_CMDS
	@echo "---> install uboot1"
	cp -fv $(@D)/uboot1.isi   $(BINARIES_DIR)/uboot1.isi
endef


$(eval $(generic-package))

