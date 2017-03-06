#ifndef GLOBAL_H
#define GLOBAL_H

extern int	disp_pos;

extern int ticks;
#define TTY_DELAY 20
/*48 bytes  0-15 limit, 16-47 base*/
extern u8 gdt_ptr[];
/*gdt descriptor*/
extern DESCRIPTOR gdt[];

/*48 bytes  0-15 limit, 16-47 base*/
extern u8 idt_ptr[];	
/*idt descriptor*/
extern GATE idt[];

/*interrupt reentry*/
extern u32 re_enter;

/*tss*/
extern TSS tss;

/*pointer of current process*/
extern PROCESS* p_proc_ready;

/*valid process table
	0 = is not valid  ,will not run
	1 = valid         ,will run
*/
//extern int proc_valid_table[];

/*process table*/
extern PROCESS proc_table[];

/*TASK*/
extern char task_stack[];
extern TASK task_table[];
/*USER PROGRAM*/
extern char user_stack[];
extern USER user_table[];

/*interrupt handler table*/
extern irq_handler irq_table[];

/*system call table*/
extern system_call sys_call_table[];

/*tty,console*/
extern TTY tty_table[];
extern CONSOLE console_table[];
extern int current_console;
extern int enable_cin_cmd;
//used in cin_getline()
extern int allow_cin;

extern SEMAPHORE sys_sem[];

extern u8* fsbuf;
extern const int FSBUF_SIZE;

extern 	struct file_desc f_desc_table[];
extern	struct inode inode_table[];
extern	struct super_block sb;

extern	struct inode* root_inode;

extern int fs_is_ready;
#endif
