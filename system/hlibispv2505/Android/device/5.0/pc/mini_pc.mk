$(call inherit-product, device/img/pc/device.mk)
$(call inherit-product, device/generic/x86/mini_x86.mk)

PRODUCT_NAME := mini_pc
PRODUCT_DEVICE := pc
PRODUCT_BRAND := Android
PRODUCT_MODEL := Mini Android on PC

PRODUCT_CHARACTERISTICS := nosdcard
