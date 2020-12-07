#!/bin/sh

[ -f atstart.zip ]
rm -r atstart
mkdir atstart
cd atstart
unzip ../atstart.zip
cd ..
make clean
