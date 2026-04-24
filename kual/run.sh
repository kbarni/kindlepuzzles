#!/bin/sh

BASEDIR = "/mnt/us/puzzles/"
BINDIR = $([ -f /lib/ld-linux-armhf.so.3 ] && echo "hf/" || echo "pw2/")

cd $BASEDIR$BINDIR

./$1
