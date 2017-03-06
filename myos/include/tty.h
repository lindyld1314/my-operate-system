#ifndef TTY_H
#define TTY_H

#define TTY_BUF_SIZE 256

struct s_console;

typedef struct s_tty
{
	//number of process of this tty
	int num_proc;

	u32 buf[TTY_BUF_SIZE];
	u32* p_head;
	u32* p_tail;
	int count;

	struct s_console* p_console;

	//1 = cin now , 0 = not cin
	int is_cin;
	//'\n' is end
	u32 cin_buf[TTY_BUF_SIZE];
	//the number of char
	u32 cin_cnt;
}TTY;


#endif