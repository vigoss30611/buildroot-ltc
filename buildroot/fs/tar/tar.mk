################################################################################
#
# tar to archive target filesystem
#
################################################################################

TAR_OPTS := $(BR2_TARGET_ROOTFS_TAR_OPTIONS)

ifeq ($(BR2_FS_MAKE_SYSTEM),y)
define ROOTFS_TAR_CMD
 tar -c$(TAR_OPTS)f $@ -C $(BASE_DIR) $(notdir $(TARGET_DIR))
endef
else
define ROOTFS_TAR_CMD
 tar -c$(TAR_OPTS)f $@ -C $(TARGET_DIR) .
endef
endif

$(eval $(call ROOTFS_TARGET,tar))
