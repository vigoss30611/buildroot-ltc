 #!/bin/sh

ps -o pid,rss,comm | sed '/PID/d' | while read p o c; do
# echo "treating $p $o $c"
 if test "$c" != "sh" && ! [ $o -eq 0 ]; then
	 echo "killing: $c($p)"
	 kill -9 $p > /dev/null 2>&1
 fi
done

umount -a
mount none /tmp -t tmpfs
mkdir -p /tmp
mkdir /tmp/old
mkdir /tmp/proc
mkdir /tmp/sys
mkdir /tmp/tmp
cp -pdrf *bin lib* dev usr /tmp
mount none /tmp/proc -t proc
mount none /tmp/sys -t sysfs
pivot_root /tmp /tmp/old

clear
rm -f /tmp/upgrade
rm -f /tmp/burn.ius
wget http://__SERVER_IP__:__SERVER_PORT__/upgrade -P /tmp
wget http://__SERVER_IP__:__SERVER_PORT__/burn.ius -P /tmp
chmod 777 /tmp/upgrade
/tmp/upgrade /tmp/burn.ius

