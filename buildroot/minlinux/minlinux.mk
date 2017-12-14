################################################################################
#
# Linux kernel target
#
################################################################################

MINLINUX_VERSION = $(call qstrip,$(BR2_MINLINUX_KERNEL_VERSION))
MINLINUX_LICENSE = GPLv2
MINLINUX_LICENSE_FILES = COPYING

# Compute MINLINUX_SOURCE and MINLINUX_SITE from the configuration
ifeq ($(MINLINUX_VERSION),local)
MINLINUX_SRCDIR = $(BR2_MINLINUX_KERNEL_LOCAL_PATH)
MINLINUX_MAINTAINED = YES
else ifeq ($(MINLINUX_VERSION),custom)
MINLINUX_TARBALL = $(call qstrip,$(BR2_MINLINUX_KERNEL_CUSTOM_TARBALL_LOCATION))
MINLINUX_SITE = $(patsubst %/,%,$(dir $(MINLINUX_TARBALL)))
MINLINUX_SOURCE = $(notdir $(MINLINUX_TARBALL))
else ifeq ($(BR2_MINLINUX_KERNEL_CUSTOM_GIT),y)
MINLINUX_SITE = $(call qstrip,$(BR2_MINLINUX_KERNEL_CUSTOM_REPO_URL))
MINLINUX_SITE_METHOD = git
else ifeq ($(BR2_MINLINUX_KERNEL_CUSTOM_HG),y)
MINLINUX_SITE = $(call qstrip,$(BR2_MINLINUX_KERNEL_CUSTOM_REPO_URL))
MINLINUX_SITE_METHOD = hg
else
MINLINUX_SOURCE = linux-$(MINLINUX_VERSION).tar.xz
# In X.Y.Z, get X and Y. We replace dots and dashes by spaces in order
# to use the $(word) function. We support versions such as 3.1,
# 2.6.32, 2.6.32-rc1, 3.0-rc6, etc.
ifeq ($(findstring x2.6.,x$(MINLINUX_VERSION)),x2.6.)
MINLINUX_SITE = $(BR2_KERNEL_MIRROR)/linux/kernel/v2.6/
else
MINLINUX_SITE = $(BR2_KERNEL_MIRROR)/linux/kernel/v3.x/
endif
# release candidates are in testing/ subdir
ifneq ($(findstring -rc,$(MINLINUX_VERSION)),)
MINLINUX_SITE := $(MINLINUX_SITE)testing/
endif # -rc
endif

MINLINUX_PATCHES = $(call qstrip,$(BR2_MINLINUX_KERNEL_PATCH))

MINLINUX_INSTALL_IMAGES = YES
MINLINUX_DEPENDENCIES  += host-kmod host-lzop

ifeq ($(BR2_MINLINUX_KERNEL_UBOOT_IMAGE),y)
	MINLINUX_DEPENDENCIES += host-uboot-tools
endif

MINLINUX_MAKE_FLAGS = \
	HOSTCC="$(HOSTCC)" \
	HOSTCFLAGS="$(HOSTCFLAGS)" \
	ARCH=$(KERNEL_ARCH) \
	INSTALL_MOD_PATH=$(TARGET_DIR) \
	INSTALL_MOD_STRIP="--strip-debug" \
	CROSS_COMPILE="$(CCACHE) $(TARGET_CROSS)" \
	DEPMOD=$(HOST_DIR)/sbin/depmod

# Get the real Linux version, which tells us where kernel modules are
# going to be installed in the target filesystem.
MINLINUX_VERSION_PROBED = $(shell $(MAKE) $(MINLINUX_MAKE_FLAGS) -C $(MINLINUX_SRCDIR) \
					   O=$(MINLINUX_BUILDDIR) --no-print-directory -s kernelrelease)

ifeq ($(BR2_MINLINUX_KERNEL_USE_INTREE_DTS),y)
MINKERNEL_DTS_NAME = $(call qstrip,$(BR2_MINLINUX_KERNEL_INTREE_DTS_NAME))
else ifeq ($(BR2_MINLINUX_KERNEL_USE_CUSTOM_DTS),y)
MINKERNEL_DTS_NAME = $(basename $(notdir $(call qstrip,$(BR2_MINLINUX_KERNEL_CUSTOM_DTS_PATH))))
endif

ifeq ($(BR2_MINLINUX_KERNEL_DTS_SUPPORT)$(MINKERNEL_DTS_NAME),y)
$(error No kernel device tree source specified, check your \
BR2_MINLINUX_KERNEL_USE_INTREE_DTS / BR2_MINLINUX_KERNEL_USE_CUSTOM_DTS settings)
endif

ifeq ($(BR2_MINLINUX_KERNEL_APPENDED_DTB),y)
ifneq ($(words $(MINKERNEL_DTS_NAME)),1)
$(error Kernel with appended device tree needs exactly one DTS source.\
  Check BR2_MINLINUX_KERNEL_INTREE_DTS_NAME or BR2_MINLINUX_KERNEL_CUSTOM_DTS_PATH.)
endif
endif

MINKERNEL_DTBS = $(addsuffix .dtb,$(MINKERNEL_DTS_NAME))

ifeq ($(BR2_MINLINUX_KERNEL_IMAGE_TARGET_CUSTOM),y)
MINLINUX_IMAGE_NAME=$(call qstrip,$(BR2_MINLINUX_KERNEL_IMAGE_TARGET_NAME))
else
ifeq ($(BR2_MINLINUX_KERNEL_UIMAGE),y)
MINLINUX_IMAGE_NAME=uImage
else ifeq ($(BR2_MINLINUX_KERNEL_APPENDED_UIMAGE),y)
MINLINUX_IMAGE_NAME=uImage
else ifeq ($(BR2_MINLINUX_KERNEL_BZIMAGE),y)
MINLINUX_IMAGE_NAME=bzImage
else ifeq ($(BR2_MINLINUX_KERNEL_ZIMAGE),y)
MINLINUX_IMAGE_NAME=zImage
else ifeq ($(BR2_MINLINUX_KERNEL_APPENDED_ZIMAGE),y)
MINLINUX_IMAGE_NAME=zImage
else ifeq ($(BR2_MINLINUX_KERNEL_CUIMAGE),y)
MINLINUX_IMAGE_NAME=cuImage.$(MINKERNEL_DTS_NAME)
else ifeq ($(BR2_MINLINUX_KERNEL_SIMPLEIMAGE),y)
MINLINUX_IMAGE_NAME=simpleImage.$(MINKERNEL_DTS_NAME)
else ifeq ($(BR2_MINLINUX_KERNEL_LINUX_BIN),y)
MINLINUX_IMAGE_NAME=linux.bin
else ifeq ($(BR2_MINLINUX_KERNEL_VMLINUX_BIN),y)
MINLINUX_IMAGE_NAME=vmlinux.bin
else ifeq ($(BR2_MINLINUX_KERNEL_VMLINUX),y)
MINLINUX_IMAGE_NAME=vmlinux
else ifeq ($(BR2_MINLINUX_KERNEL_VMLINUZ),y)
MINLINUX_IMAGE_NAME=vmlinuz
endif
endif
MINLINUX_IMAGE_NAME_COPY=min$(MINLINUX_IMAGE_NAME)

MINLINUX_KERNEL_UIMAGE_LOADADDR=$(call qstrip,$(BR2_MINLINUX_KERNEL_UIMAGE_LOADADDR))
ifneq ($(MINLINUX_KERNEL_UIMAGE_LOADADDR),)
MINLINUX_MAKE_FLAGS+=LOADADDR="$(MINLINUX_KERNEL_UIMAGE_LOADADDR)"
endif

# Compute the arch path, since i386 and x86_64 are in arch/x86 and not
# in arch/$(KERNEL_ARCH). Even if the kernel creates symbolic links
# for bzImage, arch/i386 and arch/x86_64 do not exist when copying the
# defconfig file.
ifeq ($(KERNEL_ARCH),i386)
MINKERNEL_ARCH_PATH=arch/x86
else ifeq ($(KERNEL_ARCH),x86_64)
MINKERNEL_ARCH_PATH=arch/x86
else
MINKERNEL_ARCH_PATH=arch/$(KERNEL_ARCH)
endif

ifeq ($(BR2_MINLINUX_KERNEL_VMLINUX),y)
MINLINUX_IMAGE_PATH=$(MINLINUX_DIR)/$(MINLINUX_IMAGE_NAME)
else ifeq ($(BR2_MINLINUX_KERNEL_VMLINUZ),y)
MINLINUX_IMAGE_PATH=$(MINLINUX_DIR)/$(MINLINUX_IMAGE_NAME)
else
ifeq ($(KERNEL_ARCH),avr32)
MINLINUX_IMAGE_PATH=$(MINLINUX_BUILDDIR)/$(MINKERNEL_ARCH_PATH)/boot/images/$(MINLINUX_IMAGE_NAME)
else
MINLINUX_IMAGE_PATH=$(MINLINUX_BUILDDIR)/$(MINKERNEL_ARCH_PATH)/boot/$(MINLINUX_IMAGE_NAME)
endif
endif # BR2_MINLINUX_KERNEL_VMLINUX

define MINLINUX_DOWNLOAD_PATCHES
	$(if $(MINLINUX_PATCHES),
		@$(call MESSAGE,"Download additional patches"))
	$(foreach patch,$(filter ftp://% http://%,$(MINLINUX_PATCHES)),\
		$(call DOWNLOAD,$(patch))$(sep))
endef

MINLINUX_POST_DOWNLOAD_HOOKS += MINLINUX_DOWNLOAD_PATCHES

define MINLINUX_APPLY_PATCHES
	for p in $(MINLINUX_PATCHES) ; do \
		if echo $$p | grep -q -E "^ftp://|^http://" ; then \
			buildroot/support/scripts/apply-patches.sh $(MINLINUX_SRCDIR) $(DL_DIR) `basename $$p` ; \
		elif test -d $$p ; then \
			buildroot/support/scripts/apply-patches.sh $(MINLINUX_SRCDIR) $$p linux-\*.patch ; \
		else \
			buildroot/support/scripts/apply-patches.sh $(MINLINUX_SRCDIR) `dirname $$p` `basename $$p` ; \
		fi \
	done
endef

MINLINUX_POST_PATCH_HOOKS += MINLINUX_APPLY_PATCHES


ifeq ($(BR2_MINLINUX_KERNEL_USE_DEFCONFIG),y)
MINKERNEL_SOURCE_CONFIG = $(MINLINUX_SRCDIR)/$(MINKERNEL_ARCH_PATH)/configs/$(call qstrip,$(BR2_MINLINUX_KERNEL_DEFCONFIG))_defconfig
else ifeq ($(BR2_MINLINUX_KERNEL_USE_CUSTOM_CONFIG),y)
MINKERNEL_SOURCE_CONFIG = $(BR2_MINLINUX_KERNEL_CUSTOM_CONFIG_FILE)
endif

define MINLINUX_CONFIGURE_CMDS
	cp $(MINKERNEL_SOURCE_CONFIG) $(MINLINUX_SRCDIR)/$(MINKERNEL_ARCH_PATH)/configs/buildroot_defconfig
	$(TARGET_MAKE_ENV) $(MAKE1) $(MINLINUX_MAKE_FLAGS) -C $(MINLINUX_SRCDIR) \
		O=$(MINLINUX_BUILDDIR) buildroot_defconfig
	rm $(MINLINUX_SRCDIR)/$(MINKERNEL_ARCH_PATH)/configs/buildroot_defconfig
	$(if $(BR2_arm)$(BR2_armeb),
		$(call KCONFIG_ENABLE_OPT,CONFIG_AEABI,$(@D)/.config))
	# As the kernel gets compiled before root filesystems are
	# built, we create a fake cpio file. It'll be
	# replaced later by the real cpio archive, and the kernel will be
	# rebuilt using the linux26-rebuild-with-initramfs target.
	$(if $(BR2_TARGET_ROOTFS_INITRAMFS),
		touch $(BINARIES_DIR)/rootfs.cpio
		$(call KCONFIG_ENABLE_OPT,CONFIG_BLK_DEV_INITRD,$(@D)/.config)
		$(call KCONFIG_SET_OPT,CONFIG_INITRAMFS_SOURCE,\"$(BINARIES_DIR)/rootfs.cpio\",$(@D)/.config)
		$(call KCONFIG_SET_OPT,CONFIG_INITRAMFS_ROOT_UID,0,$(@D)/.config)
		$(call KCONFIG_SET_OPT,CONFIG_INITRAMFS_ROOT_GID,0,$(@D)/.config))
	$(if $(BR2_ROOTFS_DEVICE_CREATION_STATIC),,
		$(call KCONFIG_ENABLE_OPT,CONFIG_DEVTMPFS,$(@D)/.config)
		$(call KCONFIG_ENABLE_OPT,CONFIG_DEVTMPFS_MOUNT,$(@D)/.config))
	$(if $(BR2_ROOTFS_DEVICE_CREATION_DYNAMIC_MDEV),
		$(call KCONFIG_SET_OPT,CONFIG_UEVENT_HELPER_PATH,\"/sbin/mdev\",$(@D)/.config))
	$(if $(BR2_PACKAGE_KTAP),
		$(call KCONFIG_ENABLE_OPT,CONFIG_DEBUG_FS,$(@D)/.config)
		$(call KCONFIG_ENABLE_OPT,CONFIG_EVENT_TRACING,$(@D)/.config)
		$(call KCONFIG_ENABLE_OPT,CONFIG_PERF_EVENTS,$(@D)/.config)
		$(call KCONFIG_ENABLE_OPT,CONFIG_FUNCTION_TRACER,$(@D)/.config))
	$(if $(BR2_PACKAGE_SYSTEMD),
		$(call KCONFIG_ENABLE_OPT,CONFIG_CGROUPS,$(@D)/.config))
	$(if $(BR2_MINLINUX_KERNEL_APPENDED_DTB),
		$(call KCONFIG_ENABLE_OPT,CONFIG_ARM_APPENDED_DTB,$(@D)/.config))
	yes '' | $(TARGET_MAKE_ENV) $(MAKE1) $(MINLINUX_MAKE_FLAGS) -C $(MINLINUX_SRCDIR) \
		O=$(MINLINUX_BUILDDIR) oldconfig
endef

ifeq ($(BR2_MINLINUX_KERNEL_DTS_SUPPORT),y)
ifeq ($(BR2_MINLINUX_KERNEL_DTB_IS_SELF_BUILT),)
define MINLINUX_BUILD_DTB
	$(TARGET_MAKE_ENV) $(MAKE) $(MINLINUX_MAKE_FLAGS) -C $(MINLINUX_SRCDIR) $(MINKERNEL_DTBS) \
		O=$(MINLINUX_BUILDDIR)
endef
define MINLINUX_INSTALL_DTB
	# dtbs moved from arch/<ARCH>/boot to arch/<ARCH>/boot/dts since 3.8-rc1
	cp $(addprefix \
		$(MINLINUX_SRCDIR)/$(MINKERNEL_ARCH_PATH)/boot/$(if $(wildcard \
		$(addprefix $(MINLINUX_SRCDIR)/$(MINKERNEL_ARCH_PATH)/boot/dts/,$(MINKERNEL_DTBS))),dts/),$(MINKERNEL_DTBS)) \
		$(BINARIES_DIR)/
endef
define MINLINUX_INSTALL_DTB_TARGET
	# dtbs moved from arch/<ARCH>/boot to arch/<ARCH>/boot/dts since 3.8-rc1
	cp $(addprefix \
		$(MINLINUX_SRCDIR)/$(MINKERNEL_ARCH_PATH)/boot/$(if $(wildcard \
		$(addprefix $(MINLINUX_SRCDIR)/$(MINKERNEL_ARCH_PATH)/boot/dts/,$(MINKERNEL_DTBS))),dts/),$(MINKERNEL_DTBS)) \
		$(TARGET_DIR)/boot/
endef
endif
endif

ifeq ($(BR2_MINLINUX_KERNEL_APPENDED_DTB),y)
# dtbs moved from arch/$ARCH/boot to arch/$ARCH/boot/dts since 3.8-rc1
define MINLINUX_APPEND_DTB
	if [ -e $(MINLINUX_BUILDDIR)/$(MINKERNEL_ARCH_PATH)/boot/$(MINKERNEL_DTS_NAME).dtb ]; then \
		cat $(MINLINUX_BUILDDIR)/$(MINKERNEL_ARCH_PATH)/boot/$(MINKERNEL_DTS_NAME).dtb; \
	else \
		cat $(MINLINUX_BUILDDIR)/$(MINKERNEL_ARCH_PATH)/boot/dts/$(MINKERNEL_DTS_NAME).dtb; \
	fi >> $(MINLINUX_BUILDDIR)/$(MINKERNEL_ARCH_PATH)/boot/zImage
endef
ifeq ($(BR2_MINLINUX_KERNEL_APPENDED_UIMAGE),y)
# We need to generate a new u-boot image that takes into
# account the extra-size added by the device tree at the end
# of the image. To do so, we first need to retrieve both load
# address and entry point for the kernel from the already
# generate uboot image before using mkimage -l.
MINLINUX_APPEND_DTB += $(sep) MKIMAGE_ARGS=`$(MKIMAGE) -l $(MINLINUX_IMAGE_PATH) |\
        sed -n -e 's/Image Name:[ ]*\(.*\)/-n \1/p' -e 's/Load Address:/-a/p' -e 's/Entry Point:/-e/p'`; \
        $(MKIMAGE) -A $(MKIMAGE_ARCH) -O linux \
        -T kernel -C none $${MKIMAGE_ARGS} \
        -d $(MINLINUX_BUILDDIR)/$(MINKERNEL_ARCH_PATH)/boot/zImage $(MINLINUX_IMAGE_PATH);
endif
endif

# Compilation. We make sure the kernel gets rebuilt when the
# configuration has changed.
define MINLINUX_BUILD_CMDS
	$(if $(BR2_MINLINUX_KERNEL_USE_CUSTOM_DTS),
		cp $(BR2_MINLINUX_KERNEL_CUSTOM_DTS_PATH) $(MINLINUX_BUILDDIR)/$(MINKERNEL_ARCH_PATH)/boot/dts/)
	$(TARGET_MAKE_ENV) $(MAKE) $(MINLINUX_MAKE_FLAGS) -C $(MINLINUX_SRCDIR) \
		O=$(MINLINUX_BUILDDIR) $(MINLINUX_IMAGE_NAME)
	@if grep -q "CONFIG_MODULES=y" $(@D)/.config; then 	\
		$(TARGET_MAKE_ENV) $(MAKE) $(MINLINUX_MAKE_FLAGS) -C $(MINLINUX_SRCDIR) \
		O=$(MINLINUX_BUILDDIR) modules ;	\
	fi
	$(MINLINUX_BUILD_DTB)
	$(MINLINUX_APPEND_DTB)
endef


ifeq ($(BR2_MINLINUX_KERNEL_INSTALL_TARGET),y)
define MINLINUX_INSTALL_KERNEL_IMAGE_TO_TARGET
	cp $(TARGET_DIR)/boot/$(MINLINUX_IMAGE_NAME) $(TARGET_DIR)/boot/$(MINLINUX_IMAGE_NAME_COPY)
	install -m 0644 -D $(MINLINUX_IMAGE_PATH) $(TARGET_DIR)/boot/$(MINLINUX_IMAGE_NAME_COPY)
	$(MINLINUX_INSTALL_DTB_TARGET)
endef
endif


define MINLINUX_INSTALL_HOST_TOOLS
	# Installing dtc (device tree compiler) as host tool, if selected
	if grep -q "CONFIG_DTC=y" $(@D)/.config; then 	\
		$(INSTALL) -D -m 0755 $(@D)/scripts/dtc/dtc $(HOST_DIR)/usr/bin/dtc ;	\
	fi
endef


define MINLINUX_INSTALL_IMAGES_CMDS
	cp $(MINLINUX_IMAGE_PATH) $(BINARIES_DIR)/$(MINLINUX_IMAGE_NAME_COPY)
	$(MINLINUX_INSTALL_DTB)
endef

define MINLINUX_INSTALL_TARGET_CMDS
	$(MINLINUX_INSTALL_KERNEL_IMAGE_TO_TARGET)
	# Install modules and remove symbolic links pointing to build
	# directories, not relevant on the target
	@if grep -q "CONFIG_MODULES=y" $(@D)/.config; then 	\
		$(TARGET_MAKE_ENV) $(MAKE1) $(MINLINUX_MAKE_FLAGS) -C $(MINLINUX_SRCDIR) \
		O=$(MINLINUX_BUILDDIR) modules_install; \
		rm -f $(TARGET_DIR)/lib/modules/$(MINLINUX_VERSION_PROBED)/build ;		\
		rm -f $(TARGET_DIR)/lib/modules/$(MINLINUX_VERSION_PROBED)/source ;	\
	fi
	$(MINLINUX_INSTALL_HOST_TOOLS)
endef

include $(sort $(wildcard buildroot/minlinux/minlinux-ext-*.mk))

$(eval $(generic-package))

ifeq ($(BR2_MINLINUX_KERNEL),y)
minlinux-menuconfig minlinux-xconfig minlinux-gconfig minlinux-nconfig minlinux26-menuconfig minlinux26-xconfig minlinux26-gconfig minlinux26-nconfig: dirs minlinux-configure
	$(MAKE) $(MINLINUX_MAKE_FLAGS) -C $(MINLINUX_SRCDIR) \
		O=$(MINLINUX_BUILDDIR) \
		$(subst minlinux-,,$(subst minlinux26-,,$@))
	rm -f $(MINLINUX_DIR)/.stamp_{built,target_installed,images_installed}

minlinux-savedefconfig minlinux26-savedefconfig: dirs minlinux-configure
	$(MAKE) $(MINLINUX_MAKE_FLAGS) -C $(MINLINUX_SRCDIR) \
		O=$(MINLINUX_BUILDDIR) \
		$(subst minlinux-,,$(subst minlinux26-,,$@))

ifeq ($(BR2_MINLINUX_KERNEL_USE_CUSTOM_CONFIG),y)
minlinux-update-config minlinux26-update-config: minlinux-configure $(MINLINUX_DIR)/.config
	cp -f $(MINLINUX_DIR)/.config $(BR2_MINLINUX_KERNEL_CUSTOM_CONFIG_FILE)

minlinux-update-defconfig minlinux26-update-defconfig: minlinux-savedefconfig
	cp -f $(MINLINUX_DIR)/defconfig $(BR2_MINLINUX_KERNEL_CUSTOM_CONFIG_FILE)
else
minlinux-update-config minlinux26-update-config: ;
minlinux-update-defconfig minlinux26-update-defconfig: ;
endif
endif

# Support for rebuilding the kernel after the cpio archive has
# been generated in $(BINARIES_DIR)/rootfs.cpio.
$(MINLINUX_DIR)/.stamp_initramfs_rebuilt: $(MINLINUX_DIR)/.stamp_target_installed $(MINLINUX_DIR)/.stamp_images_installed $(BINARIES_DIR)/rootfs.cpio
	@$(call MESSAGE,"Rebuilding kernel with initramfs")
	# Build the kernel.
	$(TARGET_MAKE_ENV) $(MAKE) $(MINLINUX_MAKE_FLAGS) -C $(MINLINUX_SRCDIR) \
		O=$(MINLINUX_BUILDDIR) $(MINLINUX_IMAGE_NAME)
	$(MINLINUX_APPEND_DTB)
	# Copy the kernel image to its final destination
	cp $(MINLINUX_IMAGE_PATH) $(BINARIES_DIR)
	# If there is a .ub file copy it to the final destination
	test ! -f $(MINLINUX_IMAGE_PATH).ub || cp $(MINLINUX_IMAGE_PATH).ub $(BINARIES_DIR)
	$(Q)touch $@

# The initramfs building code must make sure this target gets called
# after it generated the initramfs list of files.
minlinux-rebuild-with-initramfs minlinux26-rebuild-with-initramfs: $(MINLINUX_DIR)/.stamp_initramfs_rebuilt

# Checks to give errors that the user can understand
ifeq ($(filter source,$(MAKECMDGOALS)),)
ifeq ($(BR2_MINLINUX_KERNEL_USE_DEFCONFIG),y)
ifeq ($(call qstrip,$(BR2_MINLINUX_KERNEL_DEFCONFIG)),)
$(error No kernel defconfig name specified, check your BR2_MINLINUX_KERNEL_DEFCONFIG setting)
endif
endif

ifeq ($(BR2_MINLINUX_KERNEL_USE_CUSTOM_CONFIG),y)
ifeq ($(call qstrip,$(BR2_MINLINUX_KERNEL_CUSTOM_CONFIG_FILE)),)
$(error No kernel configuration file specified, check your BR2_MINLINUX_KERNEL_CUSTOM_CONFIG_FILE setting)
endif
endif

endif
