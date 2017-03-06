#include "all.h"

PUBLIC int ticks;

PUBLIC int	disp_pos;

/*48 bytes  0-15 limit, 16-47 base*/
PUBLIC u8 gdt_ptr[6];
/*gdt descriptor*/
PUBLIC DESCRIPTOR gdt[GDT_SIZE];

/*48 bytes  0-15 limit, 16-47 base*/
PUBLIC u8 idt_ptr[6];	
/*idt descriptor*/
PUBLIC GATE idt[IDT_SIZE];

/*interrupt reentry*/
PUBLIC u32 re_enter;

/*tss*/
PUBLIC TSS tss;

/*pointer of current process*/
PUBLIC PROCESS* p_proc_ready;

/*valid process table
	0 = is not valid  ,will not run
	1 = valid         ,will run
*/
//PUBLIC int proc_valid_table[NUM_TASKS+NUM_USERS];

/*process table*/
PUBLIC PROCESS proc_table[NUM_TASKS+NUM_USERS];

/*TASK*/
PUBLIC char task_stack[TASK_STACK_SIZE_TOTAL];
PUBLIC TASK task_table[NUM_TASKS]={
	{task_tty,STACK_SIZE_TASK,"tty"},
	{task_shell,STACK_SIZE_TASK,"shell"},
	{task_sys,STACK_SIZE_TASK,"ipc"},
	{task_hd,STACK_SIZE_TASK,"hd"},
	{task_fs,STACK_SIZE_TASK,"fs"}
};

/*USER PROGRAM*/
PUBLIC char user_stack[USER_STACK_SIZE_TOTAL];
PUBLIC USER user_table[NUM_USERS]={
	{User1,STACK_SIZE_USER,"User1"},
	{User2,STACK_SIZE_USER,"User2"},
	{User3,STACK_SIZE_USER,"User3"},
	{User4,STACK_SIZE_USER,"User4"},
	{User5,STACK_SIZE_USER,"User5"},
	{User6,STACK_SIZE_USER,"User6"}
};

/*interrupt handler table*/
PUBLIC irq_handler irq_table[NUM_IRQ];

/*system call table*/
PUBLIC system_call sys_call_table[NUM_SYS_CALL]={
	sys_write,do_fork,do_wait,do_exit,sys_get_sem,sys_free_sem,sys_sem_down,sys_sem_up,sys_sendrec
};


/*tty,console table*/
PUBLIC TTY tty_table[NUM_CONSOLES];
PUBLIC CONSOLE console_table[NUM_CONSOLES];
PUBLIC int current_console;
PUBLIC int enable_cin_cmd;
//used in cin_getline()
PUBLIC int allow_cin;

PUBLIC SEMAPHORE sys_sem[NUM_SEM];

PUBLIC u8* fsbuf=(u8*)0x600000;
PUBLIC const int FSBUF_SIZE=0x100000;

PUBLIC struct file_desc f_desc_table[MAX_FS_TABLE_SIZE];
PUBLIC struct inode inode_table[MAX_FS_TABLE_SIZE];
PUBLIC struct super_block sb;

PUBLIC struct inode* root_inode;

PUBLIC int fs_is_ready=0;

