#!/bin/sh
./build-filesystem.sh

qemu-system-i386 -drive format=raw,file=disk_image.img,if=ide,index=0 -soundhw pcspk -m 512M -enable-kvm -device VGA,vgamem_mb=64
