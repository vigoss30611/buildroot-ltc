TARGET_PRODUCT_BASE := \
	$(patsubst mini_%,%,$(patsubst full_%,%,$(TARGET_PRODUCT)))
ifeq ($(TARGET_PRODUCT_BASE),pc)
include $(all-subdir-makefiles)
endif
