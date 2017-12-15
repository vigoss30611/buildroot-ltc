#!/bin/sh

### BEGIN INIT INFO
# Provides:          defaultconfig
# Required-Start:    checkroot
# Required-Stop:
# Default-Start:     S
# Default-Stop:
# Short-Description: Uses default configuration if none exists.
### END INIT INFO

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
NAME=default-config
DESC="Default configuration"

do_check() {

    if [ ! -f "/config/shadow" ]; then
        cp -f /etc/shadow.default /config/shadow
    fi

    if [ ! -f "/config/lighttpd-htdigest.user" ]; then
        cp -f /etc/lighttpd-htdigest.user.default /config/lighttpd-htdigest.user
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
