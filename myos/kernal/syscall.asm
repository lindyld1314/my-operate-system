%include "sconst.inc"

INT_VECTOR_SYS_CALL equ 90h

global write2vga

global fork
global sys_wait
global exit

global get_sem
global free_sem
global P
global V

global sendrec

[BITS 32]
[SECTION .text]

;write2vga(char*,int)
write2vga:
	mov eax,0
	mov ebx,[esp+4]
	mov ecx,[esp+8]
	int INT_VECTOR_SYS_CALL
	ret

fork:
	mov eax,1
	int INT_VECTOR_SYS_CALL
	ret

sys_wait:
	mov eax,2
	int INT_VECTOR_SYS_CALL
	ret

exit:
	mov eax,3
	mov ebx,[esp+4]
	int INT_VECTOR_SYS_CALL
	ret

get_sem:
	mov eax,4
	mov ebx,[esp+4]
	int INT_VECTOR_SYS_CALL
	ret

free_sem:
	mov eax,5
	mov ebx,[esp+4]
	int INT_VECTOR_SYS_CALL
	ret

P:
	mov eax,6
	mov ebx,[esp+4]
	int INT_VECTOR_SYS_CALL
	ret

V:
	mov eax,7
	mov ebx,[esp+4]
	int INT_VECTOR_SYS_CALL
	ret

sendrec:
	mov eax,8
	mov ebx,[esp+4]
	mov ecx,[esp+8]
	mov edx,[esp+12]
	int INT_VECTOR_SYS_CALL
	ret