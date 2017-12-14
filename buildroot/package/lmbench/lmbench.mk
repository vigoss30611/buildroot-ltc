################################################################################
#
# lmbench
#
################################################################################

LMBENCH_VERSION = 3.0-a9
LMBENCH_SOURCE = lmbench-$(LMBENCH_VERSION).tgz
LMBENCH_SITE = http://downloads.sourceforge.net/project/lmbench/development/lmbench-$(LMBENCH_VERSION)/
LMBENCH_LICENSE =  lmbench license (based on GPLv2)
LMBENCH_LICENSE_FILES = COPYING COPYING-2

LMBENCH_SCRIPTS = config config-run gnu-os info info-template lmbench os results version
LMBENCH_DEST = $(TARGET_DIR)/usr/share/lmbench

LMBENCH_CFLAGS = $(TARGET_CFLAGS)
LMBENCH_LDLIBS = $(TARGET_LDFLAGS)

ifeq ($(BR2_PACKAGE_LIBTIRPC),y)
LMBENCH_DEPENDENCIES += libtirpc
LMBENCH_CFLAGS += -I$(STAGING_DIR)/usr/include/tirpc/
LMBENCH_LDFLAGS += -ltirpc
endif

define LMBENCH_CONFIGURE_CMDS
	$(call CONFIG_UPDATE,$(@D))
	sed -i 's/CFLAGS=/CFLAGS+=/g' $(@D)/src/Makefile
	sed -i '/cd .*doc/d' $(@D)/src/Makefile
	sed -i '/include/d' $(@D)/src/Makefile
	touch $@
endef

define LMBENCH_BUILD_CMDS
	$(MAKE) CFLAGS="$(LMBENCH_CFLAGS)" LDFLAGS="$(LMBENCH_LDFLAGS)" OS=$(ARCH) CC="$(TARGET_CC)" -C $(@D)/src
endef

define LMBENCH_INSTALL_TARGET_CMDS
	mkdir -p $(LMBENCH_DEST)
	$(MAKE) CFLAGS="$(TARGET_CFLAGS)" OS=$(ARCH) CC="$(TARGET_CC)" BASE=$(LMBENCH_DEST) -C $(@D)/src install
	mkdir -p $(LMBENCH_DEST)/scripts
	$(foreach S,$(LMBENCH_SCRIPTS),\
		cp $(LMBENCH_SRCDIR)/scripts/$(S) $(LMBENCH_DEST)/scripts/;
	)
	sed -i "s|\.\./scripts|\$${LMBASE}/scripts|g" $(LMBENCH_DEST)/scripts/*
	sed -i "s|\.\./bin/\$$OS|\$${LMBASE}/bin|g" $(LMBENCH_DEST)/scripts/*
	cp $(LMBENCH_SRCDIR)/src/webpage-lm.tar $(LMBENCH_DEST)/
	cp -f buildroot/package/lmbench/lmbench $(TARGET_DIR)/usr/bin/
endef

$(eval $(generic-package))
