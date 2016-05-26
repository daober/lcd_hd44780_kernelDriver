#!/bin/sh
#
# start display with hd44780 mcu
#

NAME=showtime
DAEMON=/usr/bin/$NAME

case "$1" in
	start)
	 echo -n "Show IP: "
	 modprobe hd44780
	 start-stop-daemon -S -b -x $DAEMON
	 ;;
	stop)
	 start-stop-daemon -K -q -n $NAME
	 echo "Shutting down		" >/dev/hd44780
	 ;;
	restart|reload)
	 $0 stop
	 sleep 1
	 $0 start
	 ;;
	*)
	 echo "Usage: $0 {start|stop|restart}"
	 exit 1
	esac

exit $?
