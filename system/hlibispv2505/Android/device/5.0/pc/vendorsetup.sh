pack-update-pc()
{
	BOARD_DIR=$ANDROID_BUILD_TOP/device/img/pc

	unset boot
	unset wipe
	unset all

	[ "$1" = "--boot" ] && boot=1
	[ "$1" = "--wipe" ] && wipe=1
	[ "$1" = "--all" ]  && wipe=1 && all=1

	# These images are always packaged ('normal' filesystem)
	IMAGES="kernel ramdisk.img"
	[ -z "$boot" ] && IMAGES="$IMAGES system.img"

	# If the user specified --wipe, reset stateful files too
	[ -n "$wipe" ] && IMAGES="$IMAGES cache.img userdata.img"

	# If the user specified --all, include the bootloader and recovery files
	[ -n "$all" ]  && IMAGES="$IMAGES misc.img grub.cfg grubenv kernel-recovery ramdisk-recovery.img"

	# Make sure all the requested images have been built
	for IMAGE in $IMAGES; do
		IMAGE=$ANDROID_PRODUCT_OUT/$IMAGE
		if [ ! -f "$IMAGE" ]; then
			echo "$IMAGE is missing. Aborting."
			return
		fi
	done

	# If the user specified --all, we assume they want to replace the
	# recovery kernel with the one in this tree (non-'all' updates will not
	# replace the recovery kernel for robustness)

	echo "Packing $IMAGES into update.zip.."
	pushd $ANDROID_PRODUCT_OUT >/dev/null
		rm -f update.zip
		mkdir -p META-INF/com/google/android
		cp -fp $BOARD_DIR/updater-script \
			META-INF/com/google/android/updater-script
		cp -fp system/bin/updater META-INF/com/google/android/update-binary
		zip -r -1 update.zip META-INF/com/google/android $IMAGES
	popd >/dev/null
	echo "Done."

	echo -n "Signing $ANDROID_PRODUCT_OUT/update.zip with testkeys (this can take a while).."
	mv $ANDROID_PRODUCT_OUT/update.zip \
	   $ANDROID_PRODUCT_OUT/update.zip.unsigned
	java -jar $ANDROID_HOST_OUT/framework/signapk.jar -w \
			  $ANDROID_BUILD_TOP/build/target/product/security/testkey.x509.pem \
			  $ANDROID_BUILD_TOP/build/target/product/security/testkey.pk8 \
			  $ANDROID_PRODUCT_OUT/update.zip.unsigned \
			  $ANDROID_PRODUCT_OUT/update.zip
	echo "done."
}

# 32-bit only builds
add_lunch_combo mini_pc-userdebug
add_lunch_combo full_pc-userdebug
add_lunch_combo mini_pc-eng
add_lunch_combo full_pc-eng

# combo 64-bit/32-bit builds
#add_lunch_combo mini_pc-userdebug
#add_lunch_combo full_pc-userdebug
#add_lunch_combo mini_pc-eng
#add_lunch_combo full_pc-eng
