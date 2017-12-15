#!/bin/sh

### BEGIN INIT INFO
# Provides:          ioboard
# Required-Start:
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Initializes IO Board
### END INIT INFO

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
DAEMON=/usr/bin/board-init.sh
NAME=board-init
DESC="IO Board Initialization"

case "$1" in
  start)
        echo -n "Starting $DESC: "
        $DAEMON
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
