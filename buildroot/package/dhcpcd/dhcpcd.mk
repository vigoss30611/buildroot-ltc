################################################################################
#
# dhcpcd
#
################################################################################

DHCPCD_VERSION = 6.11.5
DHCPCD_SOURCE = dhcpcd-$(DHCPCD_VERSION).tar.xz
DHCPCD_SITE = http://roy.marples.name/downloads/dhcpcd
DHCPCD_LICENSE = BSD-2c

ifeq ($(BR2_INET_IPV6),)
	DHCPCD_CONFIG_OPT += --disable-ipv6
else
	DHCPCD_CONFIG_OPT += --enable-ipv6
endif

ifeq ($(BR2_USE_MMU),)
	DHCPCD_CONFIG_OPT += --disable-fork
endif

define DHCPCD_CONFIGURE_CMDS
	(cd $(@D); \
	$(TARGET_CONFIGURE_OPTS) ./configure \
		--target=$(BR2_GCC_TARGET_ARCH) \
		--os=linux \
		$(DHCPCD_CONFIG_OPT) )
endef

define DHCPCD_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) \
		-C $(@D) all
endef

define DHCPCD_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/dhcpcd \
		$(TARGET_DIR)/usr/bin/dhcpcd
	$(INSTALL) -D -m 0644 $(@D)/dhcpcd.conf \
		$(TARGET_DIR)/etc/dhcpcd.conf
	$(INSTALL) -D -m 0755 $(@D)/dhcpcd-run-hooks \
		$(TARGET_DIR)/libexec/dhcpcd-run-hooks
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/dhcpcd-hooks install DESTDIR=$(TARGET_DIR)
endef

# NOTE: Even though this package has a configure script, it is not generated
# using the autotools, so we have to use the generic package infrastructure.

$(eval $(generic-package))
