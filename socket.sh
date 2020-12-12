#!/bin/bash

tty=${1:-/dev/ttyACM0}

if [ ! -e $tty ]; then
	echo >&2 $tty does not exist
	exit 1
fi

while [ -e $tty ]; do
	stty -F $tty raw -echo -echoe -echok -echoctl -echoke
	nc -l -s 127.0.0.1 -p 2542 <$tty >$tty
done;
