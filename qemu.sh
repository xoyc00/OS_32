#!/bin/sh
./iso.sh

qemu-system-i386 -cdrom cd.iso -drive file=test.img,cache=writeback -soundhw pcspk -m 512M -device VGA,vgamem_mb=64 -enable-kvm
