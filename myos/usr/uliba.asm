[SECTION .text]

global uliba_disp_color_str

; ========================================================================
;void disp_color_str(int pos,char * info, int color);
; ========================================================================
uliba_disp_color_str:
	mov	edi, [esp+4]	;pos
	mov	esi, [esp+8]	;pszInfo
	mov	ah, [esp+12]	;color
.1:
	lodsb
	test al, al
	jz	.2
	cmp	al, 0Ah	; 是回车吗?
	jnz	.3
	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.1
.3:
	mov	[gs:edi], ax
	add	edi, 2
	jmp	.1
.2:
	ret
