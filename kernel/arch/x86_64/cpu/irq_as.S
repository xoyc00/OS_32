.global irq0
.global irq1
.global irq2
.global irq3
.global irq4
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

irq0:
  pushall
  call irq0_handler
  popall
  iretw
 
irq1:
  pushall
  call irq1_handler
  popall
  iretw
 
irq2:
  pushall
  call irq2_handler
  popall
  iretw
 
irq3:
  pushall
  call irq3_handler
  popall
  iretw
 
irq4:
  pushall
  call irq4_handler
  popall
  iretw
 
irq5:
  pushall
  call irq5_handler
  popall
  iretw
 
irq6:
  pushall
  call irq6_handler
  popall
  iretw
 
irq7:
  pushall
  call irq7_handler
  popall
  iretw
 
irq8:
  pushall
  call irq8_handler
  popall
  iretw
 
irq9:
  pushall
  call irq9_handler
  popall
  iretw
 
irq10:
  pushall
  call irq10_handler
  popall
  iretw
 
irq11:
  pushall
  call irq11_handler
  popall
  iretw
 
irq12:
  pushall
  call irq12_handler
  popall
  iretw
 
irq13:
  pushall
  call irq13_handler
  popall
  iretw
 
irq14:
  pushall
  call irq14_handler
  popall
  iretw
 
irq15:
  pushall
  call irq15_handler
  popall
  iretw
 
load_idt:
	mov 4(%rsp,1), %rdx
	lidt (%rdx)
	sti
	ret
