################################################################################
#
# qm hal
#
################################################################################

QM_HAL_VERSION    = lite

QM_HAL_LICENSE = GPLv2+
QM_HAL_LICENSE_FILES = COPYING
QM_HAL_INSTALL_STAGING = YES
QM_HAL_INSTALL_IMAGES = YES

QM_HAL_MAINTAINED = YES
QM_HAL_DEPENDENCIES = host-pkgconf hlibitems hlibfr qlibwifi qlibsys eventhub audiobox libcodecs videobox libdemux qlibvplay qlibupgrade

QM_HAL_CONFIGURE_OPTS =
QM_HAL_MAKE_OPTS += \
	CROSS_COMPILE="$(CCACHE) $(TARGET_CROSS)" \
	ARCH=$(UBOOT_ARCH)

ifeq ($(BR2_QM_HAL_QIPC_38F),y)
	QM_HAL_MAKE_OPTS += "QM_HAL_QIPC_38F=y"

endif

ifeq ($(BR2_QM_HAL_QIPC_38I),y)
	QM_HAL_MAKE_OPTS += "QM_HAL_QIPC_38I=y"
endif

ifeq ($(BR2_QM_HAL_QIPC_Q3520E_SINGLE),y)
	QM_HAL_MAKE_OPTS += "QM_HAL_QIPC_Q3520E_SINGLE=y"
endif

ifeq ($(BR2_QM_HAL_SYS_DUMP),y)
	QM_HAL_MAKE_OPTS += "QM_HAL_SYS_DUMP=y"
endif

define QM_HAL_BUILD_CMDS
	echo "QM-> "$(QM_HAL_SRCDIR)
    cp -rf  $(QM_HAL_SRCDIR)/*  $(QM_HAL_BUILDDIR)
	@echo "--------------->begin build"
	echo "QM -> "$(QM_MAKE_OPTS)
	$(TARGET_CONFIGURE_OPTS) $(QM_HAL_CONFIGURE_OPTS) 	\
		$(MAKE) -C $(@D) $(QM_HAL_MAKE_OPTS) 		\
		$(QM_HAL_MAKE_TARGET)
endef


QM_HAL_HEADERS_TO_INSTALL = inc/libQMHAL.h
QM_HAL_LIBS_TO_INSTALL = libQMHAL.so

#install staging
define QM_HAL_INSTALL_STAGING_HEADERS_AND_SHARED_LIBRARY
	@echo "++++++++++++++++install staging++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/qm-hal
	for i in $(QM_HAL_HEADERS_TO_INSTALL); do \
		cp -rfv $(@D)/$$i $(STAGING_DIR)/usr/include/qm-hal/`basename $$i`; \
	done; \
	for i in $(QM_HAL_LIBS_TO_INSTALL); do \
		$(INSTALL) -D -m 0755 $(@D)/$$i $(STAGING_DIR)/usr/lib/`basename $$i`; \
	done
endef
QM_HAL_POST_INSTALL_STAGING_HOOKS  += QM_HAL_INSTALL_STAGING_HEADERS_AND_SHARED_LIBRARY

#install target
QM_HAL_INSTALL_TARGET_OPT = DESTDIR=$(TARGET_INITRD_DIR) install
define QM_HAL_POST_INSTALL_TARGET_SHARED
	@echo "++++++++++++++++install target +++++++++++++++++++++++"
	for i in $(QM_HAL_LIBS_TO_INSTALL); do \
		cp -rfv $(@D)/$$i $(TARGET_DIR)/usr/lib/`basename $$i`; \
	done
endef
QM_HAL_POST_INSTALL_TARGET_HOOKS  += QM_HAL_POST_INSTALL_TARGET_SHARED


$(eval $(generic-package))

