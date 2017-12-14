# Change overlay/frameworks/base/core/res/res/values/config.xml
# if you want SW buttons in the phone interface

DEVICE_PACKAGE_OVERLAYS := device/img/pc/overlay

PRODUCT_COPY_FILES += \
	device/img/pc/init.pc.rc:root/init.pc.rc \
	device/img/pc/ueventd.pc.rc:root/ueventd.pc.rc \
	device/img/pc/fstab.pc:root/fstab.pc \
	device/img/pc/conf/01-dhcpd-pvr.conf:system/etc/dhcpcd/dhcpcd-hooks/01-dhcpd-pvr.conf \
	device/img/pc/conf/media_profiles.xml:system/etc/media_profiles.xml \
	device/img/pc/conf/media_codecs.xml:system/etc/media_codecs.xml \
	device/img/pc/conf/pc_core_hardware.xml:system/etc/permissions/pc_core_hardware.xml \
	frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
	frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
	frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
	frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
	frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
	frameworks/native/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml

PRODUCT_PACKAGES += \
	librs_jni \
	zoneinfo.dat \
	zoneinfo.idx \
	zoneinfo.version \
	egltrace \
	eglretrace

# Since Google renamed some of the core apps, the AOSP versions
# don't get built any more.
PRODUCT_PACKAGES += \
	Email \
	Exchange

# Enable more assets
PRODUCT_AAPT_CONFIG := \
	small normal large xlarge ldpi mdpi hdpi xhdpi

# valid = 120 (LOW), 160 (MEDIUM), 213 (TV), 240 (HIGH) and 320 (XHIGH)
PRODUCT_PROPERTY_OVERRIDES += \
	ro.kernel.qemu=1 \
	ro.kernel.qemu.gles=0 \
	ro.opengles.version=196608 \
	ro.sf.lcd_density=160 \
	ro.build.selinux=1 \
	service.adb.root=1 \
	camera.disable_zsl_mode=1

PRODUCT_TAGS += dalvik.gc.type-precise

# 1GB RAM means bigger dalvik heaps
$(call inherit-product, frameworks/native/build/phone-xhdpi-1024-dalvik-heap.mk)
$(call inherit-product-if-exists, device/img/pc/extras/device-extras.mk)
