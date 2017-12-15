#!/bin/sh

### BEGIN INIT INFO
# Provides:          bfgminer
# Required-Start:    ioboard factorysetup
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Starts bfgminer.
### END INIT INFO

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin

use_bfgminer=true
if [ -f /config/miner.conf ]; then
	. /config/miner.conf
fi
if [ "$use_bfgminer" = true ] ; then
	DAEMON=/home/pi/bfgminer/bfgminer
	NAME=bfgminer
	DESC="BFGMiner daemon"
	EXTRA_OPT="-Q 10"
else
	DAEMON=/usr/bin/cgminer
	NAME=cgminer
	DESC="Cgminer daemon"
	EXTRA_OPT=
fi

set -e

test -x "$DAEMON" || exit 0

do_start() {
	start-stop-daemon -b -S -c pi -d $(dirname "$DAEMON") -x /usr/bin/screen -- -S cgminer -t cgminer -m -d sh -c "while true; do waas >/dev/null 2>/dev/null ; $DAEMON --api-listen --api-allow="W:127.0.0.1/24" -c /config/cgminer.conf $EXTRA_OPT --scrypt -S titan:auto; done"
}

kill_sessions() {
	for session in $(su pi -c "screen -ls cgminer | awk '/cgminer/ {print \$1}'")
	do
		su pi -c "screen -S "${session}" -X quit";
	done
}

do_stop() {
	kill_sessions
	killall -9 bfgminer cgminer 2>/dev/null || true
	killall -9 bfgminer cgminer 2>/dev/null || true
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
