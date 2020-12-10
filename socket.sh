#!/bin/bash

tty=${2:-/dev/ttyACM1}

set-tty-raw $tty
nc -l 2542 <$tty >$tty
#tee xxx.in <$tty | nc -l 2542 | tee xxx.out >$tty
