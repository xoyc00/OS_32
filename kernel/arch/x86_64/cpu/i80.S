.global i80

.extern i80_handler

.macro pushall
    push %rax
    push %rcx
    push %rdx
    push %rbx
    push %rbp
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15
.endm

.macro popall
    pop %rax
    pop %rcx
    pop %rdx
    pop %rbx
    pop %rbp
    pop %rsi
    pop %rdi
    pop %r8
    pop %r9
    pop %r10
    pop %r11
    pop %r12
    pop %r13
    pop %r14
    pop %r15
.endm

i80:
	pushall
	call i80_handler	
	popall
	iretw
