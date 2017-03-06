#ifndef	CONST_H
#define	CONST_H

#define EXTERN extern
/* 函数类型 */
#define	PUBLIC			/* PUBLIC is the opposite of PRIVATE */
#define	PRIVATE	static	/* PRIVATE x limits the scope of x */
/* Boolean */
#define	TRUE	1
#define	FALSE	0
#define NULL (void*)0
/* GDT 和 IDT 中描述符的个数 */
#define	GDT_SIZE 128
#define IDT_SIZE 256
/* 权限 */
#define PRIVILEGE_KRNL 0
#define PRIVILEGE_TASK 1
#define PRIVILEGE_USER 3
/* RPL */
#define	RPL_KRNL	SA_RPL0
#define	RPL_TASK	SA_RPL1
#define	RPL_USER	SA_RPL3
/*8259*/
#define INT_M_20H 0x20
#define INT_M_21H 0x21
#define INT_S_A0H 0xA0
#define INT_S_A1H 0xA1

/*number of system call*/
#define NUM_SYS_CALL 20
#define INT_VECTOR_SYS_CALL 0x90

/*tty,console*/
#define NUM_CONSOLES 5

#define VIDEO_MEMORY_BASE 0xb8000
#define VIDEO_MEMORY_SIZE 0x8000

/* Hardware interrupts */
#define	NUM_IRQ			16	/* Number of IRQs */

#define	CLOCK_IRQ		0
#define	KEYBOARD_IRQ	1
#define	CASCADE_IRQ		2	/* cascade enable for 2nd AT controller */
#define	ETHER_IRQ		3	/* default ethernet interrupt vector */
#define	SECONDARY_IRQ	3	/* RS232 interrupt vector for port 2 */
#define	RS232_IRQ		4	/* RS232 interrupt vector for port 1 */
#define	XT_WINI_IRQ		5	/* xt winchester */
#define	FLOPPY_IRQ		6	/* floppy disk */
#define	PRINTER_IRQ		7
#define	AT_WINI_IRQ		14	/* at winchester */


#endif