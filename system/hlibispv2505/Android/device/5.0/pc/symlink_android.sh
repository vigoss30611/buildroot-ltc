#!/sbin/busybox sh
BLOCK=`/sbin/blkid_static -w /dev/null -o device -t LABEL=Android`
if [ -n "$BLOCK" ]; then
	/sbin/busybox echo "Symlink $BLOCK to /dev/block/Android"
	/sbin/busybox ln -sf $BLOCK /dev/block/Android
	/sbin/e2fsck_static -y /dev/block/Android
else
	/sbin/busybox echo "WARNING: No filesystem with 'Android' label detected."
	/sbin/busybox echo "WARNING: The /dev/block/Android symlink will not be created."
fi
/sbin/busybox touch /dev/block/.symlink_android_done
