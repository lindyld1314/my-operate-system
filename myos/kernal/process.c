#include "all.h"

PUBLIC void init_proc_ipc(PROCESS* p_proc)
{
	p_proc->index=p_proc-proc_table;
	p_proc->flags=0;
	p_proc->has_int_msg=0;
	p_proc->p_msg=0;
	p_proc->recv_from=NONE;
	p_proc->send_to=NONE;
	p_proc->q_sending=0;
	p_proc->next_sending=0;
}

PUBLIC void block(PROCESS* p_proc)
{	
	p_proc->state=BLOCKED;
	clock_handler();
}

PUBLIC void blocked()
{
	p_proc_ready->state = BLOCKED;
	clock_handler();
}

PUBLIC void wakeup(PROCESS* p_proc)
{
	if(p_proc->state==BLOCKED)
		p_proc->state = READY;
}

PUBLIC int wait()
{
	int return_value=sys_wait();
	return return_value;
}

PUBLIC int do_fork()
{
	//cli, 
	disable_int();
	PROCESS* p_proc=proc_table;
	while(p_proc<proc_table+NUM_USERS+NUM_TASKS&&p_proc->state!=EXIT) p_proc++;
	if(p_proc==proc_table+NUM_TASKS+NUM_USERS) return -1;
	memcpy((void*)p_proc,(void*)p_proc_ready,sizeof(PROCESS));
	tty_table[p_proc->tty].num_proc++;
	p_proc->state=EXIT;
	//SUB_ID_BASE=1000
	p_proc->id=SUB_ID_BASE+p_proc_ready->id;
	p_proc->fid=p_proc_ready-proc_table;
	char* stack=user_stack+((p_proc-proc_table)-NUM_TASKS+1)*STACK_SIZE_USER;
	(p_proc->regs).esp=(u32)stack;
	memcpy((void*)stack,(void*)((p_proc_ready->regs).esp),STACK_SIZE_USER);
	(p_proc->regs).eax=0;
	p_proc->state=READY;
	enable_int();
	return p_proc->id;
}

PUBLIC void do_wait()
{
	p_proc_ready->state=BLOCKED;
	clock_handler();
}

PUBLIC void do_exit(int x)
{
	if(p_proc_ready->fid==-1){
		p_proc_ready->state=EXIT;
		clock_handler();
		return;
	}
	tty_table[p_proc_ready->tty].num_proc--;
	proc_table[p_proc_ready->fid].regs.eax=x;
	proc_table[p_proc_ready->fid].state=READY;
	p_proc_ready->state=EXIT;
	clock_handler();
}
