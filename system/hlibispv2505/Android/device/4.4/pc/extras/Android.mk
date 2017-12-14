ifneq (,$(findstring $(TARGET_DEVICE),pc))
include $(all-subdir-makefiles)
endif
