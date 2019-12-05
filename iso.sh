#!/bin/sh

set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/os_32.kernel isodir/boot/os_32.kernel
cat > isodir/boot/grub/grub.cfg << EOF

default=0
timeout=30

menuentry "os_32" {
	multiboot /boot/os_32.kernel
}
EOF

grub-mkrescue -o cd.iso isodir
