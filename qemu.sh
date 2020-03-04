#!/bin/sh
qemu-system-i386 -drive id=disk,file=image.img,format=raw,if=none -device ahci,id=ahci -device ide-drive,drive=disk,bus=ahci.0 -soundhw pcspk -m 2G -device VGA,vgamem_mb=64 -enable-kvm
