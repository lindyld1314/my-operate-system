; LOADER.BIN 被加载到的位置
BaseOfLoader		equ	 09000h
OffsetOfLoader		equ	  0100h

;物理地址
BaseOfLoaderPhyAddr	equ	BaseOfLoader * 10h

; KERNEL.BIN 被加载到的位置
BaseOfKernalFile	equ	 07000h
OffsetOfKernalFile	equ	     0h
; 物理地址
BaseOfKernalFilePhyAddr	equ	BaseOfKernalFile * 10h
;KernalEntryPointPhyAddr	equ	030400h
KernalEntryPointPhyAddr	equ	1000h

; 页目录开始地址:2M
PageDirBase		equ	200000h
;页表开始地址:2M + 4K
PageTblBase		equ	201000h