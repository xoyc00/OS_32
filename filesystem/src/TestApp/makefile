all:
	nasm -felf32 start.asm -o start.o
	i386-elf-gcc -c TestApp.c -o TestApp.o -ffreestanding -nostdlib -I../../../libk/include/
	i386-elf-gcc -T link.ld start.o TestApp.o -o TestApp.bin -ffreestanding -nostdlib -I../../../libk/include/ -L../../../libk/ -lc -lgcc
