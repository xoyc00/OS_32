.global gdt_flush

gdt_flush:
	mov 4(%rsp), %rax
	lgdt (%rax)
	ret
