PRODUCT_COPY_FILES += \
	device/img/pc/extras/conf/init.pc_extras.rc:root/init.pc_extras.rc \
	device/img/pc/extras/conf/audio_policy.conf:system/etc/audio_policy.conf \
	device/img/pc/extras/conf/gps.conf:system/etc/gps.conf \
	device/img/pc/extras/conf/Vendor_0eef_Product_7224.idc:system/usr/idc/Vendor_0eef_Product_7224.idc \
	device/img/pc/extras/conf/Vendor_2149_Product_2306.idc:system/usr/idc/Vendor_2149_Product_2306.idc \
	frameworks/native/data/etc/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml \
	frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
	frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
	frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
	frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
	frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:system/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \

# Audio stuff
PRODUCT_PACKAGES += \
	audio.primary.pc \
	libaudioutils \
	tinyplay \
	tinycap \
	tinymix

# The healthd board hal
PRODUCT_PACKAGES += \
	libhealthd.pc

# The local gps daemon	
PRODUCT_PACKAGES += \
	gps.pc \
	local_gps

PRODUCT_PACKAGES += \
    Gallery2 \
    Gallery \
    stagefright

PRODUCT_PROPERTY_OVERRIDES += \
	wifi.interface=wlan0 \
	wifi.supplicant_scan_interval=15

# Houdini support
# You need to patch Android to make this work
#ENABLE_HOUDINI_SUPPORT := 1

ifeq ($(ENABLE_HOUDINI_SUPPORT),1)
PRODUCT_COPY_FILES += \
	device/img/pc/extras/libhoudini/libhoudini.so:system/lib/libhoudini.so \
	device/img/pc/extras/libhoudini/libdvm_houdini.so:system/lib/libdvm_houdini.so

#check.xml
HOUDINI_SUPPORT_FILES := \
	cpuinfo libandroid_runtime.so libandroid.so libbinder.so libcamera_client.so \
	libc_orig.so libcrypto.so libc.so libcutils.so libdl.so libEGL.so libemoji.so libETC1.so \
	libexpat.so libfacelock_jni.so libfilterfw.so libfilterpack_facedetect.so libfilterpack_imageproc.so \
	libgabi++.so libgcomm_jni.so libGLESv1_CM.so libGLESv2.so libgui.so libhardware_legacy.so \
	libhardware.so libharfbuzz.so libhwui.so libicui18n.so libicuuc.so libjnigraphics.so \
	libjni_mosaic.so libjpeg.so liblog.so libmedia.so libm.so libnativehelper.so libnetutils.so \
	libnfc_ndef.so libOpenMAXAL.so libOpenSLES.so libpixelflinger.so libskia.so libsonivox.so \
	libsqlite.so libssl.so libstagefright_foundation.so libstagefright.so libstdc++.so libstlport.so \
	libui.so libusbhost.so libutils.so libvideochat_jni.so libvideochat_stabilize.so libvoicesearch.so \
	libvorbisidec.so libwpa_client.so libWVphoneAPI.so libz.so linker

PRODUCT_COPY_FILES += \
	$(foreach _file,$(HOUDINI_SUPPORT_FILES), \
		device/img/pc/extras/libhoudini/arm/$(_file):system/lib/arm/$(_file))

PRODUCT_PROPERTY_OVERRIDES += \
	ro.product.cpu.abi2=armeabi-v7a \
	ro.product.cpu.abi3=armeabi
endif
