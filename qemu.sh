#!/bin/sh
qemu-system-i386 -drive file=image.img,format=raw -drive file=part2.img,format=raw -soundhw pcspk -m 2G -device VGA,vgamem_mb=64 -enable-kvm
