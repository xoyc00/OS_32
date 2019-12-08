#!/bin/sh
./iso.sh

qemu-system-i386 -boot menu=on -cdrom cd.iso -drive file=fat:rw:test_filesystem/,format=raw -soundhw pcspk -m 2G -device VGA,vgamem_mb=64 -enable-kvm
