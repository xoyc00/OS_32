.global isr0
.global isr1
.global isr2
.global isr3
.global isr4
.global isr5
.global isr6
.global isr7
.global isr8
.global isr9
.global isr10
.global isr11
.global isr12
.global isr13
.global isr14
.global isr15
.global isr16
.global isr17
.global isr18
.global isr19
.global isr20
.global isr21
.global isr22
.global isr23
.global isr24
.global isr25
.global isr26
.global isr27
.global isr28
.global isr29
.global isr30
.global isr31

.global isr_wrapper
.align 4

.global irq0
.global irq1
.global irq2
.global irq3
.global irq4
.global irq5
.global irq6
.global irq7
.global irq8
.global irq9
.global irq10
.global irq11
.global irq12
.global irq13
.global irq14
.global irq15
 
.global load_idt
 
.global irq0_handler
.global irq1_handler
.global irq2_handler
.global irq3_handler
.global irq4_handler
.global irq5_handler
.global irq6_handler
.global irq7_handler
.global irq8_handler
.global irq9_handler
.global irq10_handler
.global irq11_handler
.global irq12_handler
.global irq13_handler
.global irq14_handler
.global irq15_handler

.extern isr_handler

isr_wrapper:
	cld
	call isr_handler
	popal
	iret

// 0: Divide By Zero Exception
isr0:
	pusha
	push $0
	call isr_handler
	popa
	ret

// 1: Debug Exception
isr1:
	pushal
	push $1
	jmp isr_wrapper

// 2: Non Maskable Interrupt Exception
isr2:
	pusha
	push $2
	call isr_handler
	popa
	ret

// 3: Int 3 Exception
isr3:
	pusha
    push $3
	call isr_handler
	popa
	ret

// 4: INTO Exception
isr4:
	pusha
    push $4
	call isr_handler
	popa
	ret

// 5: Out of Bounds Exception
isr5:
	pusha
    push $5
	call isr_handler
	popa
	ret

// 6: Invalid Opcode Exception
isr6:
	pusha
    push $6
	call isr_handler
	popa
	ret

// 7: Coprocessor Not Available Exception
isr7:
	pusha
    push $7
	call isr_handler
	popa
	ret

// 8: Double Fault Exception (With Error Code!)
isr8:
	pusha
    push $8
	call isr_handler
	popa
	ret

// 9: Coprocessor Segment Overrun Exception
isr9:
	pusha
    push $9
	call isr_handler
	popa
	ret

// 10: Bad TSS Exception (With Error Code!)
isr10:
	pusha
    push $10
	call isr_handler
	popa
	ret

// 11: Segment Not Present Exception (With Error Code!)
isr11:
    pusha
    push $11
	call isr_handler
	popa
	ret

// 12: Stack Fault Exception (With Error Code!)
isr12:
    pusha
    push $12
	call isr_handler
	popa
	ret

// 13: General Protection Fault Exception (With Error Code!)
isr13:
    pusha
    push $13
	call isr_handler
	popa
	ret

// 14: Page Fault Exception (With Error Code!)
isr14:
    pusha
    push $14
	call isr_handler
	popa
	ret

// 15: Reserved Exception
isr15:
    pusha
    push $15
	call isr_handler
	popa
	ret

// 16: Floating Point Exception
isr16:
    pusha
    push $16
	call isr_handler
	popa
	ret

// 17: Alignment Check Exception
isr17:
    pusha
    push $17
	call isr_handler
	popa
	ret

// 18: Machine Check Exception
isr18:
    pusha
    push $18
	call isr_handler
	popa
	ret

// 19: Reserved
isr19:
    pusha
    push $19
	call isr_handler
	popa
	ret

// 20: Reserved
isr20:
    pusha
    push $20
	call isr_handler
	popa
	ret

// 21: Reserved
isr21:
    pusha
    push $21
	call isr_handler
	popa
	ret

// 22: Reserved
isr22:
    pusha
    push $22
	call isr_handler
	popa
	ret

// 23: Reserved
isr23:
    pusha
    push $23
	call isr_handler
	popa
	ret

// 24: Reserved
isr24:
    pusha
    push $24
	call isr_handler
	popa
	ret

// 25: Reserved
isr25:
    pusha
    push $25
	call isr_handler
	popa
	ret

// 26: Reserved
isr26:
    pusha
    push $26
	call isr_handler
	popa
	ret

// 27: Reserved
isr27:
    pusha
    push $27
	call isr_handler
	popa
	ret

// 28: Reserved
isr28:
    pusha
    push $28
	call isr_handler
	popa
	ret

// 29: Reserved
isr29:
    pusha
    push $29
	call isr_handler
	popa
	ret

// 30: Reserved
isr30:
    pusha
    push $30
	call isr_handler
	popa
	ret

// 31: Reserved
isr31:
    pusha
    push $31
	call isr_handler
	popa
	ret

irq0:
  pusha
  call irq0_handler
  popa
  iret
 
irq1:
  pusha
  call irq1_handler
  popa
  iret
 
irq2:
  pusha
  call irq2_handler
  popa
  iret
 
irq3:
  pusha
  call irq3_handler
  popa
  iret
 
irq4:
  pusha
  call irq4_handler
  popa
  iret
 
irq5:
  pusha
  call irq5_handler
  popa
  iret
 
irq6:
  pusha
  call irq6_handler
  popa
  iret
 
irq7:
  pusha
  call irq7_handler
  popa
  iret
 
irq8:
  pusha
  call irq8_handler
  popa
  iret
 
irq9:
  pusha
  call irq9_handler
  popa
  iret
 
irq10:
  pusha
  call irq10_handler
  popa
  iret
 
irq11:
  pusha
  call irq11_handler
  popa
  iret
 
irq12:
  pusha
  call irq12_handler
  popa
  iret
 
irq13:
  pusha
  call irq13_handler
  popa
  iret
 
irq14:
  pusha
  call irq14_handler
  popa
  iret
 
irq15:
  pusha
  call irq15_handler
  popa
  iret
 
load_idt:
	mov 4(%esp,1), %edx
	lidt (%edx)
	sti
	ret
