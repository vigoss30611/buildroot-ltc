$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/emulator.mk)
$(call inherit-product, device/img/pc/device.mk)

PRODUCT_NAME := full_pc
PRODUCT_DEVICE := pc
PRODUCT_BRAND := Android
PRODUCT_MODEL := Android on PC FPGA

# Override this (set by full.mk) otherwise we'll build for
# non-en_US languages. This wastes some time.
PRODUCT_LOCALES := en_US

PRODUCT_CHARACTERISTICS := tablet,nosdcard
