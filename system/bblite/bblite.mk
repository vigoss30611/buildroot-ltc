################################################################################
#
# bblite
#
################################################################################

BBLITE_VERSION = 1.21.1
BBLITE_SOURCE = busybox-$(BBLITE_VERSION).tar.bz2
BBLITE_LICENSE = GPLv2
BBLITE_LICENSE_FILES = LICENSE

BBLITE_CFLAGS = \
	$(TARGET_CFLAGS)

BBLITE_LDFLAGS = \
	$(TARGET_LDFLAGS)

# Link against libtirpc if available so that we can leverage its RPC
# support for NFS mounting with Busybox
ifeq ($(BR2_PACKAGE_LIBTIRPC),y)
BBLITE_DEPENDENCIES += libtirpc
BBLITE_CFLAGS += -I$(STAGING_DIR)/usr/include/tirpc/
# Don't use LDFLAGS for -ltirpc, because LDFLAGS is used for
# the non-final link of modules as well.
BBLITE_CFLAGS_bblite += -ltirpc
endif

BBLITE_BUILD_CONFIG = $(BBLITE_DIR)/.config
# Allows the build system to tweak CFLAGS
BBLITE_MAKE_ENV = \
	$(TARGET_MAKE_ENV) \
	CFLAGS="$(BBLITE_CFLAGS)" \
	CFLAGS_bblite="$(BBLITE_CFLAGS_bblite)"
BBLITE_MAKE_OPTS = \
	CC="$(TARGET_CC)" \
	ARCH=$(KERNEL_ARCH) \
	PREFIX="$(TARGET_INITRD_DIR)" \
	EXTRA_LDFLAGS="$(BBLITE_LDFLAGS)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	CONFIG_PREFIX="$(TARGET_INITRD_DIR)" \
	SKIP_STRIP=y

ifndef BBLITE_CONFIG_FILE
	BBLITE_CONFIG_FILE = $(call qstrip,$(BR2_PACKAGE_BBLITE_CONFIG))
endif

define BBLITE_PERMISSIONS
/bin/busybox			 f 4755	0 0 - - - - -
/usr/share/udhcpc/default.script f 755  0 0 - - - - -
endef

# If mdev will be used for device creation enable it and copy S10mdev to /etc/init.d
ifeq ($(BR2_ROOTFS_DEVICE_CREATION_DYNAMIC_MDEV),y)
define BBLITE_INSTALL_MDEV_SCRIPT
	[ -f $(TARGET_DIR)/etc/init.d/S10mdev ] || \
		install -D -m $(@D)/S10mdev \
			$(TARGET_DIR)/etc/init.d/S10mdev
endef
define BBLITE_INSTALL_MDEV_CONF
	[ -f $(TARGET_DIR)/etc/mdev.conf ] || \
		install -D -m 0644 $(@D)/mdev.conf \
			$(TARGET_DIR)/etc/mdev.conf
endef
define BBLITE_SET_MDEV
	$(call KCONFIG_ENABLE_OPT,CONFIG_MDEV,$(BBLITE_BUILD_CONFIG))
	$(call KCONFIG_ENABLE_OPT,CONFIG_FEATURE_MDEV_CONF,$(BBLITE_BUILD_CONFIG))
	$(call KCONFIG_ENABLE_OPT,CONFIG_FEATURE_MDEV_EXEC,$(BBLITE_BUILD_CONFIG))
	$(call KCONFIG_ENABLE_OPT,CONFIG_FEATURE_MDEV_LOAD_FIRMWARE,$(BBLITE_BUILD_CONFIG))
endef
endif

ifeq ($(BR2_USE_MMU),y)
define BBLITE_SET_MMU
	$(call KCONFIG_DISABLE_OPT,CONFIG_NOMMU,$(BBLITE_BUILD_CONFIG))
endef
else
define BBLITE_SET_MMU
	$(call KCONFIG_ENABLE_OPT,CONFIG_NOMMU,$(BBLITE_BUILD_CONFIG))
	$(call KCONFIG_DISABLE_OPT,CONFIG_SWAPONOFF,$(BBLITE_BUILD_CONFIG))
	$(call KCONFIG_DISABLE_OPT,CONFIG_ASH,$(BBLITE_BUILD_CONFIG))
	$(call KCONFIG_ENABLE_OPT,CONFIG_HUSH,$(BBLITE_BUILD_CONFIG))
endef
endif

ifeq ($(BR2_LARGEFILE),y)
define BBLITE_SET_LARGEFILE
	$(call KCONFIG_ENABLE_OPT,CONFIG_LFS,$(BBLITE_BUILD_CONFIG))
	$(call KCONFIG_ENABLE_OPT,CONFIG_FDISK_SUPPORT_LARGE_DISKS,$(BBLITE_BUILD_CONFIG))
endef
else
define BBLITE_SET_LARGEFILE
	$(call KCONFIG_DISABLE_OPT,CONFIG_LFS,$(BBLITE_BUILD_CONFIG))
	$(call KCONFIG_DISABLE_OPT,CONFIG_FDISK_SUPPORT_LARGE_DISKS,$(BBLITE_BUILD_CONFIG))
endef
endif

# If IPv6 is enabled then enable basic ifupdown support for it
ifeq ($(BR2_INET_IPV6),y)
define BBLITE_SET_IPV6
	$(call KCONFIG_ENABLE_OPT,CONFIG_FEATURE_IPV6,$(BBLITE_BUILD_CONFIG))
	$(call KCONFIG_ENABLE_OPT,CONFIG_FEATURE_IFUPDOWN_IPV6,$(BBLITE_BUILD_CONFIG))
endef
else
define BBLITE_SET_IPV6
	$(call KCONFIG_DISABLE_OPT,CONFIG_FEATURE_IPV6,$(BBLITE_BUILD_CONFIG))
	$(call KCONFIG_DISABLE_OPT,CONFIG_FEATURE_IFUPDOWN_IPV6,$(BBLITE_BUILD_CONFIG))
endef
endif

# If we're using static libs do the same for busybox
ifeq ($(BR2_PREFER_STATIC_LIB),y)
define BBLITE_PREFER_STATIC
	$(call KCONFIG_ENABLE_OPT,CONFIG_STATIC,$(BBLITE_BUILD_CONFIG))
endef
endif

# Disable usage of inetd if netkit-base package is selected
ifeq ($(BR2_PACKAGE_NETKITBASE),y)
define BBLITE_NETKITBASE
	$(call KCONFIG_DISABLE_OPT,CONFIG_INETD,$(BBLITE_BUILD_CONFIG))
endef
endif

# Disable usage of telnetd if netkit-telnetd package is selected
ifeq ($(BR2_PACKAGE_NETKITTELNET),y)
define BBLITE_NETKITTELNET
	$(call KCONFIG_DISABLE_OPT,CONFIG_TELNETD,$(BBLITE_BUILD_CONFIG))
endef
endif

define BBLITE_COPY_CONFIG
	cp -f $(BBLITE_CONFIG_FILE) $(BBLITE_BUILD_CONFIG)
endef

# Disable shadow passwords support if unsupported by the C library
ifeq ($(BR2_TOOLCHAIN_HAS_SHADOW_PASSWORDS),)
define BBLITE_INTERNAL_SHADOW_PASSWORDS
	$(call KCONFIG_ENABLE_OPT,CONFIG_USE_BB_PWD_GRP,$(BBLITE_BUILD_CONFIG))
	$(call KCONFIG_ENABLE_OPT,CONFIG_USE_BB_SHADOW,$(BBLITE_BUILD_CONFIG))
endef
endif

ifeq ($(BR2_INIT_BBLITE),y)
define BBLITE_SET_INIT
	$(call KCONFIG_ENABLE_OPT,CONFIG_INIT,$(BBLITE_BUILD_CONFIG))
endef
endif

define BBLITE_INSTALL_LOGGING_SCRIPT
	if grep -q CONFIG_SYSLOGD=y $(@D)/.config; then \
		[ -f $(TARGET_DIR)/etc/init.d/S01logging ] || \
			$(INSTALL) -m 0755 -D $(@D)/S01logging \
				$(TARGET_DIR)/etc/init.d/S01logging; \
	else rm -f $(TARGET_DIR)/etc/init.d/S01logging; fi
endef

ifeq ($(BR2_PACKAGE_BBLITE_WATCHDOG),y)
define BBLITE_SET_WATCHDOG
        $(call KCONFIG_ENABLE_OPT,CONFIG_WATCHDOG,$(BBLITE_BUILD_CONFIG))
endef
define BBLITE_INSTALL_WATCHDOG_SCRIPT
	[ -f $(TARGET_DIR)/etc/init.d/S15watchdog ] || \
		install -D -m 0755 $(@D)/S15watchdog \
			$(TARGET_DIR)/etc/init.d/S15watchdog && \
		sed -i s/PERIOD/$(call qstrip,$(BR2_PACKAGE_BBLITE_WATCHDOG_PERIOD))/ \
			$(TARGET_DIR)/etc/init.d/S15watchdog
endef
endif

# We do this here to avoid busting a modified .config in configure
BBLITE_POST_EXTRACT_HOOKS += BBLITE_COPY_CONFIG

define BBLITE_CONFIGURE_CMDS
	$(BBLITE_SET_MMU)
	$(BBLITE_SET_LARGEFILE)
	$(BBLITE_SET_IPV6)
	$(BBLITE_PREFER_STATIC)
	$(BBLITE_SET_MDEV)
	$(BBLITE_NETKITBASE)
	$(BBLITE_NETKITTELNET)
	$(BBLITE_INTERNAL_SHADOW_PASSWORDS)
	$(BBLITE_SET_INIT)
	$(BBLITE_SET_WATCHDOG)
	echo $(@D)
	@yes "" | $(MAKE) ARCH=$(KERNEL_ARCH) CROSS_COMPILE="$(TARGET_CROSS)" \
		-C $(@D) oldconfig
endef

define BBLITE_BUILD_CMDS
	$(BBLITE_MAKE_ENV) $(MAKE) $(BBLITE_MAKE_OPTS) -C $(@D)
endef

define BBLITE_INSTALL_TARGET_CMDS
	$(BBLITE_MAKE_ENV) $(MAKE) $(BBLITE_MAKE_OPTS) -C $(@D) install
	$(BBLITE_INSTALL_MDEV_SCRIPT)
	$(BBLITE_INSTALL_MDEV_CONF)
	$(BBLITE_INSTALL_LOGGING_SCRIPT)
	$(BBLITE_INSTALL_WATCHDOG_SCRIPT)
endef

$(eval $(generic-package))

bblite-menuconfig bblite-xconfig bblite-gconfig: bblite-patch
	$(BBLITE_MAKE_ENV) $(MAKE) $(BBLITE_MAKE_OPTS) -C $(BBLITE_DIR) \
		$(subst bblite-,,$@)
	rm -f $(BBLITE_DIR)/.stamp_built
	rm -f $(BBLITE_DIR)/.stamp_target_installed

bblite-update-config: bblite-configure
	cp -f $(BBLITE_BUILD_CONFIG) $(BBLITE_CONFIG_FILE)
