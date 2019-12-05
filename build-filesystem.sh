#!/bin/sh

set -e
. ./imgfiles.sh


die() {
    echo "die: $*"
    exit 1
}

printf "Setting up disk image..."
dd if=/dev/zero of=disk_image.img bs=1M count="${DISK_SIZE:-257}" status=none
chown 1000:1000 disk_image.img
echo "done"

echo "Creating loopback device..."
dev=$(sudo losetup --find --partscan --show disk_image.img)
echo "Loopback device is located at ${dev}"

cleanup() {
    if [ -d mnt ]; then
        printf "unmounting filesystem... "
        sudo umount mnt || ( sleep 1 && sync && umount mnt )
        echo "done"
    fi

    if [ -e "${dev}" ]; then
        printf "cleaning up loopback device... "
        sudo losetup -d "${dev}"
        echo "done"
    fi
}
trap cleanup EXIT

printf "creating partition table... "
sudo parted -s "${dev}" mklabel gpt mkpart BIOSBOOT ext3 1MiB 8MiB mkpart OS fat32 8MiB 256MiB set 1 bios_grub || die "couldn't partition disk"
echo "done"

printf "destroying old filesystem... "
sudo dd if=/dev/zero of="${dev}"p2 bs=1M count=1 status=none || die "couldn't destroy old filesystem"
echo "done"

printf "creating new filesystem... "
sudo mkdosfs -F32 "${dev}"p2 || die "couldn't create filesystem"
echo "done"

printf "mounting filesystem... "
mkdir -p mnt
sudo mount -o uid=1000,gid=1000 "${dev}"p2 mnt/ || die "couldn't mount filesystem"
echo "done"

sudo cp -a isodir/. mnt

sudo grub-install --boot-directory=mnt/boot --target=i386-pc --modules="fat part_msdos part_gpt multiboot" "${dev}"
