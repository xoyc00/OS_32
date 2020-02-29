#!/bin/sh
set -e
. ./config_64.sh

mkdir -p "$SYSROOT"

for PROJECT in $SYSTEM_HEADER_PROJECTS_64; do
  (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE install-headers)
done
