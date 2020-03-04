.global execute
execute:
	PUSHA
	mov 4(%esp), %eax
	call *%eax
	POPA
	ret
