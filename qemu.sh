#!/bin/sh
./build-filesystem.sh

qemu-system-i386 -drive format=raw,file=disk_image.img -m 512M -enable-kvm
