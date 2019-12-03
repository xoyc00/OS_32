#!/bin/sh
OPTIND=1
VIDEO="text"

while getopts "v:" opt; do
	case "$opt" in
	v)
		VIDEO=$OPTARG
		;;
	esac
done

case "$VIDEO" in
text)
	echo "Setting text mode..."
	sed -e '5s\.*\.set FLAGS,    ALIGN | MEMINFO\' kernel/arch/i386/boot2 > kernel/arch/i386/boot.S
	;;
rgb)
	echo "Setting RGB mode..."
	sed -e '5s\.*\.set FLAGS,    ALIGN | MEMINFO | VIDMODE\' kernel/arch/i386/boot2 > kernel/arch/i386/boot.S
	;;
esac
