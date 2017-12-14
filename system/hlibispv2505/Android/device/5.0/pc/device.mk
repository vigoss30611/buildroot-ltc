# Change overlay/frameworks/base/core/res/res/values/config.xml
# if you want SW buttons in the phone interface

DEVICE_PACKAGE_OVERLAYS := device/img/pc/overlay

KERNEL_DIR ?= kernel/common

PRODUCT_COPY_FILES += \
	device/img/pc/conf/misc.img:misc.img \
	${KERNEL_DIR}/arch/x86/boot/bzImage:kernel \
	${KERNEL_DIR}/arch/x86/boot/bzImage:kernel-recovery \
	device/img/pc/init.pc.rc:root/init.pc.rc \
	device/img/pc/init.recovery.pc.rc:root/init.recovery.pc.rc \
	device/img/pc/init.recovery.pc.rescue.rc:root/init.recovery.pc.rescue.rc \
	device/img/pc/init.vnc.rc:root/init.vnc.rc \
	device/img/pc/ueventd.pc.rc:root/ueventd.pc.rc \
	device/img/pc/fstab.pc:root/fstab.pc \
	device/img/pc/symlink_android.sh:root/sbin/symlink_android.sh \
	device/img/pc/conf/01-dhcpd-pvr.conf:system/etc/dhcpcd/dhcpcd-hooks/01-dhcpd-pvr.conf \
	device/img/pc/conf/VNC-client-remote-input.idc:system/usr/idc/VNC-client-remote-input.idc \
	device/img/pc/conf/pc_core_hardware.xml:system/etc/permissions/pc_core_hardware.xml \
	frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
	frameworks/native/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
	frameworks/native/data/etc/android.hardware.camera.full.xml:system/etc/permissions/android.hardware.camera.full.xml \
	frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
	frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
	frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
	frameworks/native/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml

#	device/img/pc/conf/grubenv:grubenv \
#	device/img/pc/conf/grub.cfg:grub.cfg \
#	device/img/pc/gptinit.sh:root/sbin/gptinit.sh \
		
# use stagefright for video encode/decode
PRODUCT_COPY_FILES += \
	device/generic/goldfish/camera/media_codecs.xml:system/etc/media_codecs.xml \
	device/img/pc/media_profiles.xml:system/etc/media_profiles.xml \
	frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:system/etc/media_codecs_google_audio.xml \
	frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:system/etc/media_codecs_google_telephony.xml \
	frameworks/av/media/libstagefright/data/media_codecs_google_video.xml:system/etc/media_codecs_google_video.xml

ifneq ($(wildcard device/img/pc/v2500/Felix.ko),)
PRODUCT_COPY_FILES += \
	device/img/pc/v2500/Felix.ko:system/lib/modules/Felix.ko
endif

PRODUCT_PACKAGES += \
	librs_jni \
	e2fsck \
	zoneinfo.dat \
	zoneinfo.idx \
	zoneinfo.version \
	busybox \
	blkid_static \
	e2fsck_static \
	gptinit \
	updater \
	egltrace \
	eglretrace \
	bcc \
	libyuv \
	libadf_dynamic \
	libadfhwc_dynamic

# Tests
PRODUCT_PACKAGES += \
	flatland \
	memtrack_test \
	ES31-CTS \
	dEQP-ondevice \
	es30-mustpass.trie \
	es31-mustpass.trie

# Since Google renamed some of the core apps, the AOSP versions
# don't get built any more.
PRODUCT_PACKAGES += \
	Email \
	Exchange \
	TestingCamera \
	LegacyCamera \
	Gallery

# The healthd board hal
#PRODUCT_PACKAGES += \
#	libhealthd.pc

# Enable more assets
PRODUCT_AAPT_CONFIG := \
	small normal large xlarge ldpi mdpi hdpi xhdpi

# valid = 120 (LOW), 160 (MEDIUM), 213 (TV), 240 (HIGH) and 320 (XHIGH)
PRODUCT_PROPERTY_OVERRIDES += \
	ro.kernel.qemu=1 \
	ro.kernel.qemu.gles=0 \
	ro.sf.lcd_density=160 \
	ro.build.selinux=1 \
	service.adb.root=1
#	 \
#	camera.disable_zsl_mode=1 \
#       ro.opengles.version=196609 
PRODUCT_TAGS += dalvik.gc.type-precise

# 1GB RAM means bigger dalvik heaps
$(call inherit-product, frameworks/native/build/phone-xhdpi-1024-dalvik-heap.mk)

TARGET_PRODUCT_BASE := \
	$(patsubst mini_%,%,$(patsubst full_%,%,$(TARGET_PRODUCT)))

# FIXME: This is temporary. The 'extra' stuff should ultimately be
#        enabled for for 64-bit.
ifeq ($(TARGET_PRODUCT_BASE),pc)
$(call inherit-product-if-exists, device/img/pc/extras/device-extras.mk)
endif

ifeq ($(TARGET_PRODUCT_BASE),pc_x86_64)
PRODUCT_COPY_FILES += system/core/rootdir/init.zygote64_32.rc:root/init.zygote64_32.rc
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.zygote=zygote64_32
endif
