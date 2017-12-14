$(call inherit-product, device/img/pc/device.mk)
$(call inherit-product, device/generic/x86/mini_x86.mk)
$(call inherit-product, build/target/product/core_64_bit.mk)

PRODUCT_NAME := mini_pc_x86_64
PRODUCT_DEVICE := pc
PRODUCT_BRAND := Android
PRODUCT_MODEL := Mini Android on PC (64-bit)

PRODUCT_CHARACTERISTICS := nosdcard
