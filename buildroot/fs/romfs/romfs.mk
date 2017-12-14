################################################################################
#
# Build the romfs target filesystem image
#
################################################################################

ROMFS_TARGET=$(IMAGE).romfs

ROOTFS_ROMFS_DEPENDENCIES = host-genromfs

define ROOTFS_ROMFS_CMD
	$(HOST_DIR)/usr/bin/genromfs -d $(TARGET_DIR) -f $@
endef

$(eval $(call ROOTFS_TARGET,romfs))