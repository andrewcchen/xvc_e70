#!/bin/bash

tty=${2:-/dev/ttyACM1}

set-tty-raw $tty
nc -l 2542 <$tty >$tty
