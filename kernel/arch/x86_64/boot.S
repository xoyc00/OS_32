.code32

# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set VIDMODE,  0x4
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

/* for MULTIBOOT_MEMORY_INFO */
.long 0x00000000    /* header_addr */
.long 0x00000000    /* load_addr */
.long 0x00000000    /* load_end_addr */
.long 0x00000000    /* bss_end_addr */
.long 0x00000000    /* entry_addr */

/* for MULTIBOOT_VIDEO_MODE */
.long 0x00000000    /* mode_type */
.long 1024          /* width */
.long 768           /* height */
.long 32            /* depth */

# Reserve a stack for the initial thread.
.section .bss
.align 16
p4_table: .space 4096
p3_table: .space 4096
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp

	push %eax
	push %ebx

	JMP change_to_lm

	cli
	hlt

.size _start, . - _start

.code64
enter_lm:
	# Call the global constructors.
	call _init

	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
1:	hlt
	jmp 1b

.code32
change_to_lm:
	call setup_paging

	mov p4_table, %eax
	mov %eax, %cr3

	mov %cr4, %eax
	or %eax, 1 << 5
	mov %eax, %cr4

	mov $0xC0000080, %ecx
	rdmsr
	or %eax, 1 << 8
	wrmsr

	mov %cr0, %eax
	or %eax, 1 << 31
	mov %eax, %cr0

	jmp enter_lm

setup_paging:
	mov p3_table, %eax
	or $0x3, %eax
	mov %eax, (p4_table)

	mov $0, %ecx

.map_p3_table:
	mov 0x40000000, %eax
	mul %ecx
	or $0x83, %eax
	mov %eax, p3_table(%ecx, 8)

	inc %ecx
	cmp 512, %ecx
	jne .map_p3_table

	ret

error:
	cli	
	hlt

