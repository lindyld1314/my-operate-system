#include "all.h"

void initial_sem()
{
	int i;
	for(i=0;i<NUM_SEM;++i)
	{
		sys_sem[i].value=0;
		sys_sem[i].used=0;
		sys_sem[i].up=0;
		sys_sem[i].down=0;
	}
}

int sys_get_sem(int value)
{
	int i;
	disable_int();
	for(i=0;i<NUM_SEM;++i)
	{
		if(sys_sem[i].used==0)
		{
			sys_sem[i].used=1;	
			sys_sem[i].value=value;
			sys_sem[i].own=p_proc_ready->id;
			sys_sem[i].up=0;
			sys_sem[i].down=0;
			enable_int();
			return i;
		}
	}
	enable_int();
	return -1;
}

void sys_free_sem(int s)
{
	if(s<0||s>=NUM_SEM) return;
	if(sys_sem[s].used==0) return;
	if(sys_sem[s].own!=p_proc_ready->id) return;
	if(sys_sem[s].up!=sys_sem[s].down){
		printf("There are still some resource need to be free!,free_sem error!\n");
		return;
	}
	sys_sem[s].used=0;
}


void sys_sem_down(int s)
{
	if(s<0||s>=NUM_SEM) return;
	if(sys_sem[s].used==0) return;
	disable_int();

	if(sys_sem[s].value<=0)
	{
		int pos=-sys_sem[s].value;
		sys_sem[s].queue[pos]=p_proc_ready;
		p_proc_ready->state=BLOCKED;
		clock_handler();
	}

	sys_sem[s].value--;
	sys_sem[s].down++;
	enable_int();
}

void sys_sem_up(int s)
{

	if(s<0||s>=NUM_SEM) return;
	if(sys_sem[s].used==0) return;
	disable_int();

	if(sys_sem[s].value<0)
	{
		int i;
		int len=-sys_sem[s].value;
		if(sys_sem[s].queue[0]->state==BLOCKED)
			sys_sem[s].queue[0]->state=READY;
		else printf("wake up a process in sem's queue, ERROR!,%d\n",sys_sem[s].queue[0]->state);
		for(i=0;i<len-1;++i) sys_sem[s].queue[i]=sys_sem[s].queue[i+1];
		clock_handler();
	}
	else if(sys_sem[s].up>=sys_sem[s].down)
	{
		clock_handler();
	}
	
	sys_sem[s].value++;
	sys_sem[s].up++;
	enable_int();
}


