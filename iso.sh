#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/os_32.kernel isodir/boot/os_32.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "os_32" {
	multiboot /boot/os_32.kernel
}
EOF
grub-mkrescue -o os_32.iso isodir
