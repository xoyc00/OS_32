#!/bin/sh
set -e
. ./config_64.sh

for PROJECT in $PROJECTS_64; do
  (cd $PROJECT && $MAKE clean)
done

rm -rf sysroot
rm -rf isodir
rm -rf image.img
rm -rf vbox.vdi
