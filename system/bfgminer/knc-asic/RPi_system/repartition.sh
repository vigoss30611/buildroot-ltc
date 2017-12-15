#!/bin/sh

### BEGIN INIT INFO
# Provides:          repartition
# Required-Start:    checkroot
# Required-Stop:
# Default-Start:     S
# Default-Stop:
# Short-Description: Re-partitions SD card at first boot
### END INIT INFO

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
NAME=repartition
DESC="SD card repartition check"

FLAGFILE1="/repartition"
FLAGFILE2="/repartition-step2"
DISK="/dev/mmcblk0"
PARTITION="/dev/mmcblk0p2"

print_table() {

    cat <<EOF
# partition table of /dev/mmcblk0
unit: sectors

/dev/mmcblk0p1 : start=     8192, size=   114688, Id= c
/dev/mmcblk0p2 : start=   122880, size=         , Id=83
/dev/mmcblk0p3 : start=        0, size=        0, Id= 0
/dev/mmcblk0p4 : start=        0, size=        0, Id= 0
EOF

}

do_check() {

    if [ -f $FLAGFILE1 ]; then
        print_table | sfdisk --no-reread $DISK
        touch $FLAGFILE2
        rm -f $FLAGFILE1
        reboot
    elif [ -f $FLAGFILE2 ]; then
        resize2fs $PARTITION
        rm -f $FLAGFILE2
    fi

}

case "$1" in
  start)
        echo -n "Starting $DESC: "
        do_check
        echo "$NAME."
        ;;
  stop)
        echo -n "Stopping $DESC: "
        echo "$NAME."
        ;;
  restart|force-reload)
        echo -n "Restarting $DESC: "
        echo "$NAME."
        ;;
  *)
        N=/etc/init.d/$NAME
        echo "Usage: $N {start|stop|restart|force-reload}" >&2
        exit 1
        ;;
esac

exit 0
