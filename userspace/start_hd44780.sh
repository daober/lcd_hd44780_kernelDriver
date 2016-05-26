#!/bin/sh

while true
do
	if[ -f /tmp/message ]; then
		textid=`md5sum /tmp/message | cut -d" " -f1`
		if[ "$textid" != "$oldtextid" ]; then
		#clear display
		echo "			">/dev/hd44780
		#show message
		cat /tmp/message >/dev/hd44780
		oldtextid=$textid 	#display the text only once
	fi
	sleep 1
	else
		date >/dev/hd44780
		sleep 1
		ip=$(/sbin/ifconfig eth0 | grep inet | cut -d: -f2 | cut -d" " -f1)
		echo "IP:"$ip"		" >/dev/hd44780
		sleep 1
		textid=0
	fi
done
