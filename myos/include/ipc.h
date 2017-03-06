#ifndef IPC_H
#define IPC_H

#define SEND		1
#define RECEIVE		2
#define BOTH		3	/* BOTH = (SEND | RECEIVE) */

#define NONE -1


//source,dest
#define ANY NUM_USERS+NUM_TASKS+10
#define INTERRUPT NUM_TASKS+NUM_USERS+5

#define TASK_TTY 0
#define TASK_SHELL 1
#define TASK_SYS 2
#define TASK_HD 3
#define TASK_FS 4

//type
#define HARD_INT 0
#define GET_TICKS 1

#define DEV_OPEN 2
#define DEV_CLOSE 3
#define DEV_READ 4
#define DEV_WRITE 5
#define DEV_IOCTL 6

#define OPEN 7
#define CLOSE 8
#define WRITE 9
#define READ 10
#define UNLINK 11

#define SYS_RET 12

//request
#define IOCTL_GET_NUM_SEC 1

typedef struct s_message{
	int source;
	int type;

	int ret;
	u64 cnt; //byte_cnt
	u32 sec_no;
	void* buf;
	int proc_index;
	int request;
	int flag;
	void* pathname;
	int len;
	int fd;


}MESSAGE;

#endif