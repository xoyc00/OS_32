#!/bin/sh
./build-filesystem.sh

qemu-system-i386 -hda disk_image.img -m 512M -enable-kvm
