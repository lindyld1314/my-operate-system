#ifndef PROCESS_H
#define PROCESS_H

#define NUM_FILES 10

typedef struct s_stackframe
{
	u32 gs;
	u32 fs;
	u32 es;
	u32 ds;
	u32 edi;
	u32 esi;
	u32 ebp;
	u32 kernal_esp;
	u32 ebx;
	u32 edx;
	u32 ecx;
	u32 eax;
	u32 retaddr;
	u32 eip;
	u32 cs;
	u32 eflags;
	u32 esp;
	u32 ss;
}STACK_FRAME;

typedef struct s_process
{
	STACK_FRAME regs;
	u16 ldt_sel;
	DESCRIPTOR 	ldts[LDT_SIZE];
	u32 id;
	int fid;// if is not subprogram,fid=-1. else fid=father_p_proc - proc_table
	char p_name[16];

	int tty;
	int state;
	
	//ipc
	int index;//index of proc_table
	int flags; //0,sending,receiving
	int has_int_msg;
	MESSAGE* p_msg;
	int recv_from;
	int send_to;
	struct s_process* q_sending;//point to head of the sending_queue
	struct s_process* next_sending;
	//fs
	struct file_desc* filp[NUM_FILES];
}PROCESS;

typedef struct s_task
{
	task_f initial_eip; //typedef void (*task_f) ();
	int stacksize;
	char name[32];
}TASK;

typedef struct s_user
{
	user_f initial_eip;
	int stacksize;
	char name[32];
}USER;

#define NUM_TASKS 5
#define NUM_USERS 20

//bug!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define STACK_SIZE_TASK 0x3000
#define STACK_SIZE_USER 0x3000

#define TASK_STACK_SIZE_TOTAL STACK_SIZE_TASK*NUM_TASKS
#define USER_STACK_SIZE_TOTAL STACK_SIZE_USER*NUM_USERS

#define USER_ID_BASE 100
#define SUB_ID_BASE 1000

#define EXIT 0
#define READY 1
#define RUN 2
#define BLOCKED 3

#define SENDING 1
#define RECEIVING 2

#endif