#!/bin/sh
./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

mkdir -p filesystem
mkdir -p filesystem/user
mkdir -p filesystem/bin

cp sysroot/boot/os_32.ker isodir/boot/os_32.ker
cp -r filesystem/. isodir
cat > isodir/boot/grub/grub.cfg << EOF

default=0
timeout=30

menuentry "os_32" {
	multiboot /boot/os_32.ker
}
EOF

dd if=/dev/zero of=image.img bs=1M count=256
mkfs.vfat -F32 image.img
sudo mount -t vfat image.img mnt/
sudo cp -r isodir/. mnt/
sudo umount mnt/

device=$(sudo losetup --find --show image.img)
sudo mount -t vfat ${device} mnt/
sudo grub-install --target=i386-pc --boot-directory=mnt/boot --modules="fat" --force ${device}
sudo umount mnt/
sudo losetup -d ${device}
