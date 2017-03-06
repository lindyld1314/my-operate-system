org  0100h
jmp	LABEL_START	

%include	"fat12.inc"
%include	"load.inc"
%include	"define.inc"

; GDT -----------------------------------------------------------------------------------
;                   		   段基址        段界限     , 属性
LABEL_GDT:			Descriptor   0,         0, 			0						; 空描述符
LABEL_DESC_FLAT_C:	Descriptor   0,         0fffffh, 	DA_CR  | DA_32 | DA_LIMIT_4K; 0 ~ 4G
LABEL_DESC_FLAT_RW:	Descriptor   0,         0fffffh, 	DA_DRW | DA_32 | DA_LIMIT_4K; 0 ~ 4G
LABEL_DESC_VIDEO:	Descriptor	 0B8000h,   0ffffh, 	DA_DRW | DA_DPL3	; 显存首地址
; GDT ----------------------------------------------------------------------------------

GdtLen		equ	$ - LABEL_GDT
GdtPtr		dw	GdtLen - 1				; 段界限
			dd	BaseOfLoaderPhyAddr + LABEL_GDT		; 基地址

; GDT 选择子 ----------------------------------------------------------------------------------
SelectorFlatC		equ	LABEL_DESC_FLAT_C	- LABEL_GDT
SelectorFlatRW		equ	LABEL_DESC_FLAT_RW	- LABEL_GDT
SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT + SA_RPL3
; GDT 选择子 ----------------------------------------------------------------------------------

LABEL_START:	
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, 100h

	; 得到内存数,int 15H
	mov	ebx, 0				; ebx = 后续值, 开始时需为 0
	mov	di, _MemChkBuf		; es:di 指向一个地址范围描述符结构（Address Range Descriptor Structure）
.MemChkLoop:
	mov	eax, 0E820h			; eax = 0000E820h
	mov	ecx, 20				; ecx = 地址范围描述符结构的大小
	mov	edx, 0534D4150h		; edx = 'SMAP'
	int	15h					; int 15h
	jc	.MemChkFail
	add	di, 20
	inc	dword [_dwMCRNumber]	; dwMCRNumber = ARDS 的个数
	cmp	ebx, 0
	jne	.MemChkLoop
	jmp	.MemChkOK
.MemChkFail:
	mov	dword [_dwMCRNumber], 0
.MemChkOK:

	; 下面在根目录寻找 Kernal.BIN
	mov	word [wSectorNo], SectorNoOfRootDirectory	
	xor	ah, ah	; ┓
	xor	dl, dl	; ┣ 软驱复位
	int	13h		; ┛
LABEL_SEARCH_IN_ROOT_DIR_BEGIN:
	cmp	word [wRootDirSizeForLoop], 0	; ┓
	jz	LABEL_NO_KernalBIN				; ┣ 判断根目录区是不是已经读完, 如果读完表示没有找到 Kernal.BIN
	dec	word [wRootDirSizeForLoop]		; ┛
	mov	ax, BaseOfKernalFile
	mov	es, ax					; es <- BaseOfKernalFile
	mov	bx, OffsetOfKernalFile	; bx <- OffsetOfKernalFile	于是, es:bx = BaseOfKernalFile:OffsetOfKernalFile = BaseOfKernalFile * 10h + OffsetOfKernalFile
	mov	ax, [wSectorNo]			; ax <- Root Directory 中的某 Sector 号
	mov	cl, 1
	call	ReadSector

	mov	si, KernalFileName		; ds:si -> "Kernal  BIN"
	mov	di, OffsetOfKernalFile	; es:di -> BaseOfKernalFile:???? = BaseOfKernalFile*10h+????
	cld
	mov	dx, 10h
LABEL_SEARCH_FOR_KernalBIN:
	cmp	dx, 0								; ┓
	jz	LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR	; ┣ 循环次数控制, 如果已经读完了一个 Sector, 就跳到下一个 Sector
	dec	dx									; ┛
	mov	cx, 11
LABEL_CMP_FILENAME:
	cmp	cx, 0					; ┓
	jz	LABEL_FILENAME_FOUND	; ┣ 循环次数控制, 如果比较了 11 个字符都相等, 表示找到
	dec	cx						; ┛
	lodsb					; ds:si -> al
	cmp	al, byte [es:di]	; if al == es:di
	jz	LABEL_GO_ON
	jmp	LABEL_DIFFERENT
LABEL_GO_ON:
	inc	di
	jmp	LABEL_CMP_FILENAME	;	继续循环

LABEL_DIFFERENT:
	and	di, 0FFE0h				; else┓	这时di的值不知道是什么, di &= e0 为了让它是 20h 的倍数
	add	di, 20h					;     ┃
	mov	si, KernalFileName		;     ┣ di += 20h  下一个目录条目
	jmp	LABEL_SEARCH_FOR_KernalBIN;   ┛

LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR:
	add	word [wSectorNo], 1
	jmp	LABEL_SEARCH_IN_ROOT_DIR_BEGIN

LABEL_NO_KernalBIN:
	jmp	$						; 没有找到 Kernal.BIN, 死循环在这里

LABEL_FILENAME_FOUND:			; 找到 Kernal.BIN 后便来到这里继续
	mov	ax, RootDirSectors
	and	di, 0FFF0h		; di -> 当前条目的开始

	push	eax
	mov	eax, [es : di + 01Ch]		; ┓
	mov	dword [dwKernalSize], eax	; ┛保存 Kernal.BIN 文件大小
	pop	eax

	add	di, 01Ah		; di -> 首 Sector
	mov	cx, word [es:di]
	push	cx			; 保存此 Sector 在 FAT 中的序号
	add	cx, ax
	add	cx, DeltaSectorNo	; 这时 cl 里面是 LOADER.BIN 的起始扇区号 (从 0 开始数的序号)
	mov	ax, BaseOfKernalFile
	mov	es, ax					; es <- BaseOfKernalFile
	mov	bx, OffsetOfKernalFile	; bx <- OffsetOfKernalFile	于是, es:bx = BaseOfKernalFile:OffsetOfKernalFile = BaseOfKernalFile * 10h + OffsetOfKernalFile
	mov	ax, cx					; ax <- Sector 号

LABEL_GOON_LOADING_FILE:
	mov	cl, 1
	call	ReadSector
	pop	ax				; 取出此 Sector 在 FAT 中的序号
	call	GetFATEntry
	cmp	ax, 0FFFh
	jz	LABEL_FILE_LOADED
	push	ax			; 保存 Sector 在 FAT 中的序号
	mov	dx, RootDirSectors
	add	ax, dx
	add	ax, DeltaSectorNo
	add	bx, [BPB_BytsPerSec]
	jc LABEL_GOON_LOADING_FILE1
	jmp LABEL_GOON_LOADING_FILE2
LABEL_GOON_LOADING_FILE1:
	push ax
	mov ax,es
	add ax,1000h
	mov es,ax
	pop ax
LABEL_GOON_LOADING_FILE2:
	jmp	LABEL_GOON_LOADING_FILE

LABEL_FILE_LOADED:
	; 关闭软驱马达
	push dx
	push ax
	mov	dx, 03F2h
	mov	al, 0
	out	dx, al
	pop ax
	pop	dx
	
	;mov ax,0b800h
	;mov es,ax
	;mov byte [es:0],'A'
	;mov byte [es:1],0eh
; 下面准备跳入保护模式 -------------------------------------------

; 加载 GDTR
	lgdt	[GdtPtr]

; 关中断
	cli

; 打开地址线A20
	in	al, 92h
	or	al, 00000010b
	out	92h, al

; 准备切换到保护模式
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax

; 真正进入保护模式
	jmp	dword SelectorFlatC:(BaseOfLoaderPhyAddr+LABEL_PM_START)


;============================================================================
;变量
;----------------------------------------------------------------------------
wRootDirSizeForLoop	dw	RootDirSectors	; Root Directory 占用的扇区数
wSectorNo			dw	0		; 要读取的扇区号
bOdd				db	0		; 奇数还是偶数
dwKernalSize		dd	0		; Kernal.BIN 文件大小

;============================================================================
;字符串
;----------------------------------------------------------------------------
KernalFileName		db	"KERNAL  BIN", 0	; KERNAL.BIN 之文件名
;============================================================================

;----------------------------------------------------------------------------
; 函数名: ReadSector
;----------------------------------------------------------------------------
; 作用:
;	从序号(Directory Entry 中的 Sector 号)为 ax 的的 Sector 开始, 将 cl 个 Sector 读入 es:bx 中
ReadSector:
	push	bp
	mov	bp, sp
	sub	esp, 2			; 辟出两个字节的堆栈区域保存要读的扇区数: byte [bp-2]

	mov	byte [bp-2], cl
	push	bx			; 保存 bx
	mov	bl, [BPB_SecPerTrk]	; bl: 除数
	div	bl				; y 在 al 中, z 在 ah 中
	inc	ah				; z ++
	mov	cl, ah			; cl <- 起始扇区号
	mov	dh, al			; dh <- y
	shr	al, 1			; y >> 1 (其实是 y/BPB_NumHeads, 这里BPB_NumHeads=2)
	mov	ch, al			; ch <- 柱面号
	and	dh, 1			; dh & 1 = 磁头号
	pop	bx			; 恢复 bx
	; 至此, "柱面号, 起始扇区, 磁头号" 全部得到
	mov	dl, [BS_DrvNum]		; 驱动器号 (0 表示 A 盘)
.GoOnReading:
	mov	ah, 2				; 读
	mov	al, byte [bp-2]		; 读 al 个扇区
	int	13h
	jc	.GoOnReading		; 如果读取错误 CF 会被置为 1, 这时就不停地读, 直到正确为止

	add	esp, 2
	pop	bp

	ret

;----------------------------------------------------------------------------
; 函数名: GetFATEntry
;----------------------------------------------------------------------------
; 作用:
;	找到序号为 ax 的 Sector 在 FAT 中的条目, 结果放在 ax 中
;	需要注意的是, 中间需要读 FAT 的扇区到 es:bx 处, 所以函数一开始保存了 es 和 bx
GetFATEntry:
	push	es
	push	bx
	push	ax
	mov	ax, BaseOfKernalFile	; ┓
	sub	ax, 0100h				; ┣ 在 BaseOfKernalFile 后面留出 4K 空间用于存放 FAT
	mov	es, ax					; ┛
	pop	ax
	mov	byte [bOdd], 0
	mov	bx, 3
	mul	bx			; dx:ax = ax * 3
	mov	bx, 2
	div	bx			; dx:ax / 2  ==>  ax <- 商, dx <- 余数
	cmp	dx, 0
	jz	LABEL_EVEN
	mov	byte [bOdd], 1
LABEL_EVEN:;偶数
	xor	dx, dx			; 现在 ax 中是 FATEntry 在 FAT 中的偏移量. 下面来计算 FATEntry 在哪个扇区中(FAT占用不止一个扇区)
	mov	bx, [BPB_BytsPerSec]
	div	bx				; dx:ax / BPB_BytsPerSec  ==>	ax <- 商   (FATEntry 所在的扇区相对于 FAT 来说的扇区号)
						;				dx <- 余数 (FATEntry 在扇区内的偏移)。
	push	dx
	mov	bx, 0			; bx <- 0	于是, es:bx = (BaseOfKernalFile - 100):00 = (BaseOfKernalFile - 100) * 10h
	add	ax, SectorNoOfFAT1	; 此句执行之后的 ax 就是 FATEntry 所在的扇区号
	mov	cl, 2
	call	ReadSector		; 读取 FATEntry 所在的扇区, 一次读两个, 避免在边界发生错误, 因为一个 FATEntry 可能跨越两个扇区
	pop	dx
	add	bx, dx
	mov	ax, [es:bx]
	cmp	byte [bOdd], 1
	jnz	LABEL_EVEN_2
	shr	ax, 4
LABEL_EVEN_2:
	and	ax, 0FFFh

LABEL_GET_FAT_ENRY_OK:

	pop	bx
	pop	es
	ret
;----------------------------------------------------------------------------

; 从此以后的代码在保护模式下执行 ----------------------------------------------------
; 32 位代码段. 由实模式跳入 ---------------------------------------------------------
[SECTION .s32]
ALIGN	32
[BITS	32]

LABEL_PM_START:
	mov	ax, SelectorVideo
	mov	gs, ax
	mov	ax, SelectorFlatRW
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	ss, ax
	mov	esp, TopOfStack

	call	SetupPaging

	call	InitKernal

	;***************************************************************
	jmp	SelectorFlatC:KernalEntryPointPhyAddr	; 正式进入内核
	;***************************************************************

; ------------------------------------------------------------------------
; 内存拷贝，仿 memcpy
; ------------------------------------------------------------------------
; void* MemCpy(void* es:pDest, void* ds:pSrc, int iSize);
; ------------------------------------------------------------------------
MemCpy:
	push	ebp
	mov	ebp, esp

	push	esi
	push	edi
	push	ecx

	mov	edi, [ebp + 8]	; Destination
	mov	esi, [ebp + 12]	; Source
	mov	ecx, [ebp + 16]	; Counter
.1:
	cmp	ecx, 0		; 判断计数器
	jz	.2		; 计数器为零时跳出

	mov	al, [ds:esi]		; ┓
	inc	esi					; ┃
							; ┣ 逐字节移动
	mov	byte [es:edi], al	; ┃
	inc	edi					; ┛

	dec	ecx		; 计数器减一
	jmp	.1		; 循环
.2:
	mov	eax, [ebp + 8]	; 返回值

	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp

	ret			; 函数结束，返回
; MemCpy 结束-------------------------------------------------------------


; 启动分页机制 --------------------------------------------------------------
SetupPaging:
	;calculate the size of memory
	push	esi
	push	edi
	push	ecx

	mov	esi, MemChkBuf
	mov	ecx, [dwMCRNumber]	;for(int i=0;i<[MCRNumber];i++) // 每次得到一个ARDS(Address Range Descriptor Structure)结构
.loop:						;{
	mov	edx, 5				;	for(int j=0;j<5;j++)	// 每次得到一个ARDS中的成员，共5个成员
	mov	edi, ARDStruct			;	{			
.11:						;
	mov eax,dword [esi]
	stosd					;		ARDStruct[j*4] = MemChkBuf[j*4];
	add	esi, 4				;
	dec	edx					;
	cmp	edx, 0				;
	jnz	.11					;	}
	cmp	dword [dwType], 1	;	if(Type == AddressRangeMemory) // AddressRangeMemory : 1, AddressRangeReserved : 2
	jne	.22					;	{
	mov	eax, [dwBaseAddrLow]	;
	add	eax, [dwLengthLow]		;
	cmp	eax, [dwMemSize]		;		if(BaseAddrLow + LengthLow > MemSize)
	jb	.22						;
	mov	[dwMemSize], eax		;			MemSize = BaseAddrLow + LengthLow;
.22:						;	}
	loop	.loop			;}
					
	pop	ecx
	pop	edi
	pop	esi

	; 根据内存大小计算应初始化多少PDE以及多少页表
	xor	edx, edx
	mov	eax, [dwMemSize]
	mov	ebx, 400000h	; 400000h = 4M = 4096 * 1024, 一个页表对应的内存大小
	div	ebx
	mov	ecx, eax	; 此时 ecx 为页表的个数，也即 PDE 应该的个数
	test	edx, edx
	jz	.no_remainder
	inc	ecx		; 如果余数不为 0 就需增加一个页表
.no_remainder:
	push	ecx		; 暂存页表个数

	; 为简化处理, 所有线性地址对应相等的物理地址. 并且不考虑内存空洞.

	; 首先初始化页目录
	mov	ax, SelectorFlatRW
	mov	es, ax
	mov	edi, PageDirBase	; 此段首地址为 PageDirBase
	xor	eax, eax
	mov	eax, PageTblBase | PG_P  | PG_USU | PG_RWW
.1:
	stosd
	add	eax, 4096		; 为了简化, 所有页表在内存中是连续的.
	loop	.1

	; 再初始化所有页表
	pop	eax				; 页表个数
	mov	ebx, 1024		; 每个页表 1024 个 PTE
	mul	ebx
	mov	ecx, eax		; PTE个数 = 页表个数 * 1024
	mov	edi, PageTblBase	; 此段首地址为 PageTblBase
	xor	eax, eax
	mov	eax, PG_P  | PG_USU | PG_RWW
.2:
	stosd
	add	eax, 4096		; 每一页指向 4K 的空间
	loop	.2

	mov	eax, PageDirBase
	mov	cr3, eax
	mov	eax, cr0
	or	eax, 80000000h
	mov	cr0, eax
	jmp	short .3
.3:
	nop

	ret
; 分页机制启动完毕 ----------------------------------------------------------



; InitKernal ---------------------------------------------------------------------------------
; 将 Kernal.BIN 的内容经过整理对齐后放到新的位置
; --------------------------------------------------------------------------------------------
InitKernal:	; 遍历每一个 Program Header，根据 Program Header 中的信息来确定把什么放进内存，放到什么位置，以及放多少。
	xor	esi, esi
	mov	cx, word [BaseOfKernalFilePhyAddr + 2Ch]; ┓ ecx <- pELFHdr->e_phnum
	movzx	ecx, cx								; ┛
	mov	esi, [BaseOfKernalFilePhyAddr + 1Ch]	; esi <- pELFHdr->e_phoff
	add	esi, BaseOfKernalFilePhyAddr			; esi <- OffsetOfKernal + pELFHdr->e_phoff
.Begin:
	mov	eax, [esi + 0]
	cmp	eax, 0						; PT_NULL
	jz	.NoAction
	push	dword [esi + 010h]		; size	┓
	mov	eax, [esi + 04h]				;	┃
	add	eax, BaseOfKernalFilePhyAddr	;	┣ ::memcpy(	(void*)(pPHdr->p_vaddr),
	push	eax						; src	┃		uchCode + pPHdr->p_offset,
	push	dword [esi + 08h]		; dst	┃		pPHdr->p_filesz;
	call	MemCpy						;	┃
	add	esp, 12							;	┛
.NoAction:
	add	esi, 020h						; esi += pELFHdr->e_phentsize
	dec	ecx
	jnz	.Begin

	ret
; InitKernal ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


; SECTION .data1 之开始 ---------------------------------------------------------------------------------------------
[SECTION .data1]
ALIGN	32

LABEL_DATA:
	; 实模式下使用这些符号
	; 变量
	_dwMCRNumber:		dd	0	; Memory Check Result
	_dwMemSize:			dd	0
	_ARDStruct:			; Address Range Descriptor Structure
		_dwBaseAddrLow:		dd	0
		_dwBaseAddrHigh:	dd	0
		_dwLengthLow:		dd	0
		_dwLengthHigh:		dd	0
		_dwType:			dd	0
	_MemChkBuf:	times	256	db	0
	
	; 保护模式下使用这些符号
	dwMemSize			equ	BaseOfLoaderPhyAddr + _dwMemSize
	dwMCRNumber			equ	BaseOfLoaderPhyAddr + _dwMCRNumber
	ARDStruct			equ	BaseOfLoaderPhyAddr + _ARDStruct
		dwBaseAddrLow		equ	BaseOfLoaderPhyAddr + _dwBaseAddrLow
		dwBaseAddrHigh		equ	BaseOfLoaderPhyAddr + _dwBaseAddrHigh
		dwLengthLow			equ	BaseOfLoaderPhyAddr + _dwLengthLow
		dwLengthHigh		equ	BaseOfLoaderPhyAddr + _dwLengthHigh
		dwType				equ	BaseOfLoaderPhyAddr + _dwType
	MemChkBuf			equ	BaseOfLoaderPhyAddr + _MemChkBuf
	
	; 堆栈就在数据段的末尾
	StackSpace:	times	1000h	db	0
	TopOfStack	equ	BaseOfLoaderPhyAddr + $	; 栈顶
