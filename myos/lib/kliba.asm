%include "sconst.inc"

extern disp_pos

[SECTION .text]

global disp_str
global disp_color_str

global out_byte
global in_byte

global enable_irq
global disable_irq

global disable_int
global enable_int

global port_read
global port_write


; ========================================================================
;outbyte(u16 port,u8 value)
; ========================================================================
out_byte:
	push edx
	push eax

	mov edx,[esp+4+8]
	mov al,[esp+4+4+8]
	out dx,al
	nop
	nop

	pop eax
	pop edx

	ret
; ========================================================================
;u8 in_byte(u16 port)
; ========================================================================
in_byte:
	push edx

	mov edx,[esp+4+4]
	xor eax,eax
	in al,dx
	nop
	nop

	pop edx
	ret
; ========================================================================
;void disable_irq(int irq);
; ========================================================================
disable_irq:
	mov ecx,[esp+4]
	pushf
	cli
	mov ah,1
	rol ah,cl
	cmp cl,8
	jae disable_slave
	;disable master
	in al,INT_M_CTLMASK
	or al,ah
	out INT_M_CTLMASK,al
	popf
	ret
disable_slave:
	in al,INT_S_CTLMASK
	or al,ah
	out INT_S_CTLMASK,al
	popf
	ret

; ========================================================================
;void enable_irq(int irq);
; ========================================================================
enable_irq:
	mov ecx,[esp+4]
	pushf
	cli
	mov ah,11111110b
	rol ah,cl
	cmp cl,8
	jae enable_slave
	;enable_master
	in al,INT_M_CTLMASK
	and al,ah
	out INT_M_CTLMASK,al
	popf
	ret
enable_slave:
	in al,INT_S_CTLMASK
	and al,ah
	out INT_S_CTLMASK,al
	popf
	ret

; ========================================================================
;enable_int()
; ========================================================================
enable_int:
	sti
	ret

; ========================================================================
;disable_int()
; ========================================================================
disable_int:
	cli
	ret



; ========================================================================
;                  void disp_str(char * info);
; ========================================================================
disp_str:
	push	ebp
	mov	ebp, esp

	mov	esi, [ebp + 8]	; pszInfo
	mov	edi, [disp_pos]
	mov	ah, 0Fh
.1:
	lodsb
	test	al, al
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
	mov	[disp_pos], edi

	pop	ebp
	ret

; ========================================================================
;                  void disp_color_str(char * info, int color);
; ========================================================================
disp_color_str:
	push	ebp
	mov	ebp, esp

	mov	esi, [ebp + 8]	; pszInfo
	mov	edi, [disp_pos]
	mov	ah, [ebp + 12]	; color
.1:
	lodsb
	test	al, al
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
	mov	[disp_pos], edi

	pop	ebp
	ret

; ========================================================================
;                  void port_read(u16 port, void* buf, int n);
; ========================================================================
port_read:
	mov	edx, [esp + 4]		; port
	mov	edi, [esp + 4 + 4]	; buf
	mov	ecx, [esp + 4 + 4 + 4]	; n
	shr	ecx, 1
	cld
	rep	insw
	ret

; ========================================================================
;                  void port_write(u16 port, void* buf, int n);
; ========================================================================
port_write:
	mov	edx, [esp + 4]		; port
	mov	esi, [esp + 4 + 4]	; buf
	mov	ecx, [esp + 4 + 4 + 4]	; n
	shr	ecx, 1
	cld
	rep	outsw
	ret