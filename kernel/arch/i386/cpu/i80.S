.global i80

.extern i80_handler

i80:
	push %eax
	pusha
	call i80_handler	
	popa
	pop %eax
	iret
