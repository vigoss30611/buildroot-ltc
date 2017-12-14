$(call inherit-product, device/img/pc/device.mk)
$(call inherit-product, build/target/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full.mk)

PRODUCT_NAME := full_pc_x86_64
PRODUCT_DEVICE := pc
PRODUCT_BRAND := Android
PRODUCT_MODEL := Full Android on PC (64-bit)

# Override this (set by full.mk) otherwise we'll build for
# non-en_US languages. This wastes some time.
PRODUCT_LOCALES := en_US

PRODUCT_CHARACTERISTICS := tablet,nosdcard
