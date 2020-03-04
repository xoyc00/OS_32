[GLOBAL _start]
[EXTERN main]
section .startup
_start:
	CALL	main
	RET	
