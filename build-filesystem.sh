#!/bin/sh

set -e
. ./imgfiles.sh

printf "Setting up disk image..."
dd if=/dev/zero of=disk_image.img bs=512k count="${DISK_SIZE:-513}" status=none
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
sudo parted -s "${dev}" mklabel msdos mkpart primary fat32 512k 100% -a minimal set 1 boot on || die "couldn't partition disk"
echo "done"

printf "destroying old filesystem... "
sudo dd if=/dev/zero of="${dev}"p1 bs=512k count=1 status=none || die "couldn't destroy old filesystem"
echo "done"

printf "creating new filesystem... "
sudo mkdosfs -F 32 "${dev}"p1 || die "couldn't create filesystem"
echo "done"

printf "mounting filesystem... "
mkdir -p mnt
sudo mount -o uid=1000,gid=1000 "${dev}"p1 mnt/ || die "couldn't mount filesystem"
echo "done"

sudo cp -a isodir/. mnt/

sudo grub-install --boot-directory=mnt/boot --target=i386-pc --modules="fat part_msdos" "${dev}"
