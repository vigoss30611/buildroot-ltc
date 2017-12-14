#!/sbin/busybox sh
if [ -b /dev/block/sda ]; then
	/sbin/busybox echo "Reinitialize /dev/block/sda and format as ext4.."
	/sbin/gptinit /dev/block/sda
	if [ $? = 0 ]; then
		/sbin/busybox echo "GPT initialization complete."
		/sbin/busybox touch /dev/block/.gptinit_done
	else
		/sbin/busybox echo "FATAL: gptinit failed. Rebooting."
		/sbin/busybox echo b >/proc/sysrq-trigger
	fi
else
	/sbin/busybox echo "FATAL: No /dev/block/sda. Rebooting."
	/sbin/busybox echo b >/proc/sysrq-trigger
fi
