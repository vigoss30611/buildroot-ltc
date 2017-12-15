#!/bin/sh

### BEGIN INIT INFO
# Provides:          lcdloop
# Required-Start:    factorysetup
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: LCD update daemon
### END INIT INFO

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
DAEMON=/usr/bin/lcd-loop
NAME=lcd-loop
DESC="LCD update daemon"

do_start() {
        start-stop-daemon -b -S -c pi -x "$DAEMON"
}

do_stop() {
        killall "$NAME" 2>/dev/null || true
}

case "$1" in
  start)
        echo -n "Starting $DESC: "
	do_start
        echo "$NAME."
        ;;
  stop)
        echo -n "Stopping $DESC: "
	do_stop
        echo "$NAME."
        ;;
  restart|force-reload)
        echo -n "Restarting $DESC: "
        do_stop
        do_start
        echo "$NAME."
        ;;
  *)
        N=/etc/init.d/$NAME
        echo "Usage: $N {start|stop|restart|force-reload}" >&2
        exit 1
        ;;
esac

exit 0
