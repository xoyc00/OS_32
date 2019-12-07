# OS_32
A simple 32-bit operating system.

(Currently unnamed!)

# Cross-compiler
To build a GCC cross-compiler, follow the steps here: https://wiki.osdev.org/GCC_Cross-Compiler
Note that in order to use the included makefiles you should export TARGET=1386-elf rather then TARGET=1686-elf.
# Compiling
The system can be compiled via the following commands:
	
		1. source add_i386-elf_to_path.sh
	
		2. ./build.sh

To create an iso, run:
	
		./iso.sh
		
You can run this iso in either VirtualBox or VMWare, most likely any other emulator or virtual machine program also.
It will also run on physical hardware, (requires PS/2 keyboard and mouse).
