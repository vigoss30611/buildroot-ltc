$(call inherit-product, device/img/pc/device.mk)
# seems odd but there's no mini_common.mk specific to x86
$(call inherit-product, device/generic/armv7-a-neon/mini_common.mk)

PRODUCT_NAME := mini_pc
PRODUCT_DEVICE := pc
PRODUCT_BRAND := Android
PRODUCT_MODEL := Full Android on PC

PRODUCT_CHARACTERISTICS := nosdcard
