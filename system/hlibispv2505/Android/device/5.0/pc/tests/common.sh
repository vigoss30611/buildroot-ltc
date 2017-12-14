set -e

sanity_check_environment() {
	export PATH=$PATH:$PWD
	if [ -z "$ADBHOST" ]; then
		echo "ADBHOST must be set. Aborting."
		exit 1
	fi
	if [ -z "`which adb || /bin/true`" ]; then
		echo "No adb in PATH. Aborting."
		exit 1
	fi
	if [ -z "`which extractrd || /bin/true`" ]; then
		echo "No extractrd in PATH. Aborting."
		exit 1
	fi
}

establish_adb_connection() {
	retries=60
	sanity_check_environment
	adb kill-server
	adb start-server >/dev/null
	for i in `seq $retries`; do
		state=`adb get-state`
		if [ "$state" = "device" -o "$state" = "recovery" ]; then
			echo "$0: adb connected to $ADBHOST."
			return 0
		fi
		echo "$0: adb connection failed after attempt $i. Retry in 1s.."
		sleep 1
	done
	echo "$0: adb connection failed after $retries attempts."
	return 1
}

change_state() {
	if [ -z "$1" ]; then
		echo "$0: change state not specified. Aborting."
		return 1
	fi
	establish_adb_connection
	echo "$0: Found device in '$state' state."
	if [ "$1" = "device" ]; then
		adb reboot &
	else
		adb reboot $1 &
	fi
	sleep 3
	echo "$0: Sent \"adb reboot $1\" command. Waiting.."
	establish_adb_connection
	if [ "$1" != "$state" ]; then
		echo "$0: failed to transition from '$state' to '$1'."
		return 1
	fi
	echo "$0: Transitioned to '$1' state successfully."
	return 0
}

wait_state() {
	retries=6
	if [ -z "$1" ]; then
		echo "$0: wait state not specified. Aborting."
		return 1
	fi
	for i in `seq $retries`; do
		establish_adb_connection
		echo "$0: Found device in '$state' state."
		if [ "$state" = "$1" ]; then
			echo "$0: Requested state '$1' reached successfully."
			return 0
		fi
		echo "$0: Requested state '$1' not reached (attempt $i). Retry in 10s.."
		sleep 10
	done
	echo "$0: Failed to reach state '$1'. Aborting."
	return 1
}

sanity_check_android_mount() {
	if [ -z "`adb shell mount | grep /mnt/android || /bin/true`" ]; then
		echo "$0: /mnt/android is not mounted. Aborting."
		exit 1
	fi
}

extract_flashimages() {
	images="boot.img system.img cache.img userdata.img misc.img"
	echo "$0: Extracting \"$images\" from $1.."
	tar -C $tmpdir -xf $1 $images
}

upload_flashimages_safe() {
	tmpdir=`mktemp -d`
	images="boot.img system.img cache.img userdata.img misc.img"
	sanity_check_android_mount
	extract_flashimages $1
	echo "$0: Extracting ramdisk.img from $tmpdir/boot.img.."
	extractrd $tmpdir/boot.img >$tmpdir/ramdisk.img
	echo "$0: Extracting kernel from $tmpdir/boot.img.."
	extractrd -k $tmpdir/boot.img >$tmpdir/kernel
	images="kernel ramdisk.img system.img cache.img userdata.img misc.img"
	for image in $images; do adb push $tmpdir/$image /mnt/android/; done
	echo "$0: Removing $tmpdir."
	rm -rf $tmpdir
}

upload_flashimages_recovery() {
	tmpdir=`mktemp -d`
	images="grubenv grub.cfg recovery.img"
	sanity_check_android_mount
	extract_flashimages $1
	echo "$0: Extracting ramdisk-recovery.img from $tmpdir/recovery.img.."
	extractrd $tmpdir/recovery.img >$tmpdir/ramdisk-recovery.img
	echo "$0: Extracting kernel-recovery from $tmpdir/boot.img.."
	extractrd -k $tmpdir/recovery.img >$tmpdir/kernel-recovery
	images="grubenv grub.cfg kernel-recovery ramdisk-recovery.img"
	for image in $images; do adb push $tmpdir/$image /mnt/android/; done
	echo "$0: Removing $tmpdir."
	rm -rf $tmpdir
}

flash_flashimages() {
	if [ -z "$1" ]; then
		echo "$0: flashimages tarball not specified. Aborting."
		return 1
	fi
	if [ ! -f "$1" ]; then
		echo "$0: Specified flashimages tarball not found. Aborting."
		return 1
	fi

	change_state recovery

	# Flash non-recovery images first
	upload_flashimages_safe $1
	change_state device

	# System booted OK, go back and update recovery
	change_state recovery
	upload_flashimages_recovery $1

	# Back into working system
	change_state device
}
