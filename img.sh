#!/bin/sh
./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

mkdir -p filesystem
mkdir -p filesystem/user
mkdir -p filesystem/bin

cp apps/test_app/test.psc filesystem/bin/test.psc

cp sysroot/boot/os_32.ker isodir/boot/os_32.ker
cp -r filesystem/. isodir
cat > isodir/boot/grub/grub.cfg << EOF

default=0
timeout=30

menuentry "os_32" {
	multiboot /boot/os_32.ker
}
EOF

mkdir -p mnt
dd if=/dev/zero of=image.img bs=1M count=256
device=$(sudo losetup --find --show image.img)
sudo parted -s "${device}" mklabel msdos mkpart primary fat32 32k 100% -a minimal set 1 boot on
sudo mkfs.vfat -F32 -I ${device}p1
sudo mount -t vfat ${device}p1 mnt/
sudo cp -r isodir/. mnt/
sudo grub-install --target=i386-pc --boot-directory=mnt/boot --recheck --modules="fat" --force ${device}
sudo umount mnt/
sudo losetup -d ${device}
