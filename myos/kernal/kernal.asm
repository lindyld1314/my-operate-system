%include "sconst.inc"
; 导入函数
extern	cstart
extern	exception_handler
extern	spurious_irq
extern  kernal_main
extern 	clock_handler
extern  disp_str
extern  delay

extern  irq_table
extern  sys_call_table

extern	gdt_ptr
extern	idt_ptr
extern	disp_pos
extern  p_proc_ready
extern  tss
extern  re_enter

[BITS 32]
[SECTION .data]

[SECTION .bss]
StackSpace resb 8*1024
StackTop:

[SECTION .text]	; 代码在此

global _start
global restart
global sys_call

global	divide_error
global	single_step_exception
global	nmi
global	breakpoint_exception
global	overflow
global	bounds_check
global	inval_opcode
global	copr_not_available
global	double_fault
global	copr_seg_overrun
global	inval_tss
global	segment_not_present
global	stack_exception
global	general_protection
global	page_fault
global	copr_error
global  hwint00
global  hwint01
global  hwint02
global  hwint03
global  hwint04
global  hwint05
global  hwint06
global  hwint07
global  hwint08
global  hwint09
global  hwint10
global  hwint11
global  hwint12
global  hwint13
global  hwint14
global  hwint15

; GDT 以及相应的描述符是这样的：
;
;    Descriptors         Selectors
;  ┏━━━━━━━━━━━━━━━━━━┓
;  ┃ Dummy Descriptor ┃
;  ┣━━━━━━━━━━━━━━━━━━┫
;  ┃ 	DESC_FLAT_C   ┃   8h = cs
;  ┣━━━━━━━━━━━━━━━━━━┫
;  ┃    DESC_FLAT_RW  ┃  10h = ds, es, fs, ss
;  ┣━━━━━━━━━━━━━━━━━━┫
;  ┃    DESC_VIDEO    ┃  1Bh = gs
;  ┗━━━━━━━━━━━━━━━━━━┛
;  在使用 C 代码的时候一定要保证 ds, es, ss 这几个段寄存器的值是一样的


_start:
	; 把 esp 从 LOADER 挪到 KERNEL
	mov	esp, StackTop	; 堆栈在 bss 段中

	mov	dword [disp_pos], 0

	sgdt	[gdt_ptr]	; cstart() 中将会用到 gdt_ptr
	call	cstart		; 在此函数中改变了gdt_ptr，让它指向新的GDT
	lgdt	[gdt_ptr]	; 使用新的GDT

	lidt	[idt_ptr]

	jmp	SELECTOR_KERNAL_CS:csinit
csinit:
	xor eax,eax
	mov ax,SELECTOR_TSS
	ltr ax

	jmp kernal_main

; ---------------------------------
;save
; ---------------------------------
save:
	pushad
	push ds
	push es
	push fs
	push gs

	mov si,ss ;used for the JMP
	mov ds,si
	mov es,si

	mov esi,esp ;eax = 进程表起始地址
	inc dword [re_enter]
	cmp dword [re_enter],0
	jnz save1
	mov esp,StackTop;use kernal stack
	push restart
	jmp [esi+RETADR-P_STACKBASE];return
save1:
	push restart_re_enter
	jmp [esi+RETADR-P_STACKBASE];return

; ---------------------------------
;restart
; ---------------------------------
restart:
	;load ldt
	mov esp,[p_proc_ready]
	lldt [esp+P_LDT_SEL]
	;save esp of Process_table
	lea eax,[esp+P_STACKTOP]
	mov dword [tss+TSS3_S_SP0],eax
restart_re_enter:
	dec dword [re_enter]
	pop gs
	pop fs
	pop es
	pop ds
	popad
	add esp,4 ;ignore retaddr
	iretd


; ---------------------------------
;system call
;ebx,ecx,edx,ebp can be used as parameters
; ---------------------------------
sys_call:
	call save
	push dword [p_proc_ready];arg4
	sti
	push edx;arg3
	push ecx;arg2
	push ebx;arg1
	call [sys_call_table+eax*4]
	add esp,4*4
	;eax = return value
	mov [esi+EAXREG-P_STACKBASE],eax
	cli
	ret

; ---------------------------------
; 中断和异常 -- 硬件中断
; ---------------------------------
%macro  hwint_master    1
        call save
        ;close the current kind of interrupt
        in al,INT_M_CTLMASK
        or al,(1<<%1)
        out INT_M_CTLMASK,al
        ;send EOI
        mov al,EOI
        out INT_M_CTL,al
        ;open interrupt
        sti
        ;deal with the interrupt
        push %1
        call [irq_table+4*%1]
        pop ecx
        ;close interrupt
        cli
        ;open the current kind of interrupt
        in al,INT_M_CTLMASK
        and al,~(1<<%1)
        out INT_M_CTLMASK,al
        ret
%endmacro
; ---------------------------------


ALIGN   16
hwint00:                ; Interrupt routine for irq 0 (the clock).
        hwint_master    0

ALIGN   16
hwint01:                ; Interrupt routine for irq 1 (keyboard)
        hwint_master    1

ALIGN   16
hwint02:                ; Interrupt routine for irq 2 (cascade!)
        hwint_master    2

ALIGN   16
hwint03:                ; Interrupt routine for irq 3 (second serial)
        hwint_master    3

ALIGN   16
hwint04:                ; Interrupt routine for irq 4 (first serial)
        hwint_master    4

ALIGN   16
hwint05:                ; Interrupt routine for irq 5 (XT winchester)
        hwint_master    5

ALIGN   16
hwint06:                ; Interrupt routine for irq 6 (floppy)
        hwint_master    6

ALIGN   16
hwint07:                ; Interrupt routine for irq 7 (printer)
        hwint_master    7

; ---------------------------------
%macro  hwint_slave     1
        ; push    %1
        ; call    spurious_irq
        ; add     esp, 4
        ; hlt
        call save
        ;close the current kind of interrupt
        in al,INT_S_CTLMASK
        or al,(1<<(%1-8))
        out INT_S_CTLMASK,al
        ;send EOI
        mov al,EOI
        out INT_M_CTL,al
        nop
        nop
        out INT_S_CTL,al
        ;open interrupt
        sti
        ;deal with the interrupt
        push %1
        call [irq_table+4*%1]
        pop ecx
        ;close interrupt
        cli
        ;open the current kind of interrupt
        in al,INT_S_CTLMASK
        and al,~(1<<(%1-8))
        out INT_S_CTLMASK,al
        ret
%endmacro
; ---------------------------------

ALIGN   16
hwint08:                ; Interrupt routine for irq 8 (realtime clock).
        hwint_slave     8

ALIGN   16
hwint09:                ; Interrupt routine for irq 9 (irq 2 redirected)
        hwint_slave     9

ALIGN   16
hwint10:                ; Interrupt routine for irq 10
        hwint_slave     10

ALIGN   16
hwint11:                ; Interrupt routine for irq 11
        hwint_slave     11

ALIGN   16
hwint12:                ; Interrupt routine for irq 12
        hwint_slave     12

ALIGN   16
hwint13:                ; Interrupt routine for irq 13 (FPU exception)
        hwint_slave     13

ALIGN   16
hwint14:                ; Interrupt routine for irq 14 (AT winchester)
        hwint_slave     14

ALIGN   16
hwint15:                ; Interrupt routine for irq 15
        hwint_slave     15



; 中断和异常 -- 异常
divide_error:
	push	0xFFFFFFFF	; no err code
	push	0		; vector_no	= 0
	jmp	exception
single_step_exception:
	push	0xFFFFFFFF	; no err code
	push	1		; vector_no	= 1
	jmp	exception
nmi:
	push	0xFFFFFFFF	; no err code
	push	2		; vector_no	= 2
	jmp	exception
breakpoint_exception:
	push	0xFFFFFFFF	; no err code
	push	3		; vector_no	= 3
	jmp	exception
overflow:
	push	0xFFFFFFFF	; no err code
	push	4		; vector_no	= 4
	jmp	exception
bounds_check:
	push	0xFFFFFFFF	; no err code
	push	5		; vector_no	= 5
	jmp	exception
inval_opcode:
	push	0xFFFFFFFF	; no err code
	push	6		; vector_no	= 6
	jmp	exception
copr_not_available:
	push	0xFFFFFFFF	; no err code
	push	7		; vector_no	= 7
	jmp	exception
double_fault:
	push	8		; vector_no	= 8
	jmp	exception
copr_seg_overrun:
	push	0xFFFFFFFF	; no err code
	push	9		; vector_no	= 9
	jmp	exception
inval_tss:
	push	10		; vector_no	= A
	jmp	exception
segment_not_present:
	push	11		; vector_no	= B
	jmp	exception
stack_exception:
	push	12		; vector_no	= C
	jmp	exception
general_protection:
	push	13		; vector_no	= D
	jmp	exception
page_fault:
	push	14		; vector_no	= E
	jmp	exception
copr_error:
	push	0xFFFFFFFF	; no err code
	push	16		; vector_no	= 10h
	jmp	exception

exception:
	call	exception_handler
	add	esp, 4*2	; 让栈顶指向 EIP，堆栈中从顶向下依次是：EIP、CS、EFLAGS
	jmp $







