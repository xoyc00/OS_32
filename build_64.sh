#!/bin/sh
set -e
. ./headers_64.sh

for PROJECT in $PROJECTS_64; do
  (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE install)
done
