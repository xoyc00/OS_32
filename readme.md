# Pyramid OS
A simple 32-bit, non-unix-like, ring-0 only operating system.

# Cross-compiler
To build a GCC cross-compiler, follow the steps here: https://wiki.osdev.org/GCC_Cross-Compiler.

Note that in order to use the included makefiles you should export TARGET=1386-elf rather then TARGET=1686-elf.

# Adding files
To add files to the operating system's filesystem, create a folder called "filesystem" (without the quotations) and add any files you need to that.

Note that the desktop wallpaper is loaded from the file /user/wallpaper.bmp.

# Compiling
The system can be compiled via the following commands:
	
		1. source add_i386-elf_to_path.sh
	
		2. ./build.sh

To create an img, run:
	
		./img.sh
		
To run in qemu, run:
		
		./qemu.sh

To create a virtualbox hard disk image, run:
		
		./vdi.sh
