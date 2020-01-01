.global i80

.extern i80_handler

i80:
	pusha
	call i80_handler	
	popa
	iret
