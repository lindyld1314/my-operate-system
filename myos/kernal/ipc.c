#include "all.h"
//calculate the linear address of a certain segment of a process
PUBLIC int proc_seg_linear(PROCESS* p_proc,int index)
{
	DESCRIPTOR* d=&(p_proc->ldts[index]);
	return d->base_high<<24|d->base_mid<<16|d->base_low;
}
//virtual address->linear address
PUBLIC void* va2la(PROCESS* p_proc,void* va)
{
	u32 seg_base=proc_seg_linear(p_proc,INDEX_LDT_RW);
	u32 la=seg_base+(u32)va;
	return (void*)la;
}
//clear up a message
PUBLIC void reset_msg(MESSAGE* m)
{
	memset(m,0,sizeof(MESSAGE));
}
//check deadlock,  deadlock=1, no deadlock=0
//index of src_proc, index of dst_proc
PRIVATE int deadlock(int src,int dst)
{
	PROCESS* p_proc=proc_table+dst;
	while(1)
	{
		if(p_proc->flags&SENDING)//is sending
		{
			if(p_proc->send_to==src)//deadlock
			{
				p_proc=proc_table+dst;
				printf("deadlock chain:%s",p_proc->p_name);
				do{
					p_proc=proc_table+p_proc->send_to;
					printf("->%s",p_proc->p_name);
				}while(p_proc!=proc_table+src);
				printf("-> end\n");
				return 1;
			}
			p_proc=proc_table+p_proc->send_to;
		}
		else break;
	}
	return 0;
}

PRIVATE int msg_send(PROCESS* caller_proc,int dst,MESSAGE* m)
{
	//printf("1111\n");
	PROCESS* sender=caller_proc;
	PROCESS* receiver=proc_table+dst;
	if(deadlock(sender->index,dst))
	{
		printf("deadlock!\n");
		while(1);
	}
	/*receiver is waiting this message*/
	if((receiver->flags&RECEIVING)&&
		(receiver->recv_from==sender->index||receiver->recv_from==ANY) )
	{
		memcpy(va2la(receiver,receiver->p_msg),
				va2la(sender,m),sizeof(MESSAGE));
		//receiver->p_msg=0;
		receiver->recv_from=NONE;
		receiver->flags&=~RECEIVING;
		wakeup(receiver);
		//printf("wakeup receiver\n");
	}
	else
	{
		sender->flags|=SENDING;
		PROCESS* p_proc;
		if(receiver->q_sending)
		{
			p_proc=receiver->q_sending;
			while(p_proc->next_sending)
			{
				p_proc=p_proc->next_sending;
			}
			p_proc->next_sending=sender;
		}
		else receiver->q_sending=sender;
		sender->next_sending=0;
		block(sender);
		//printf("block sender\n");
	}
	return 0;
}

PRIVATE int msg_receive(PROCESS* caller_proc,int src,MESSAGE* m)
{
	//printf("2222\n");
	PROCESS* receiver=caller_proc;
	PROCESS* sender=proc_table+src;
	PROCESS* prev=0;
	int k=0;

	if(receiver->has_int_msg&&(src==ANY||src==INTERRUPT))
	{
		MESSAGE msg;
		reset_msg(&msg);
		msg.source=INTERRUPT;
		msg.type=HARD_INT;
		memcpy(va2la(receiver,m),&msg,sizeof(MESSAGE));
		receiver->has_int_msg=0;
		return 0;
	}
	if(src==ANY)
	{
		if(receiver->q_sending)
		{
			sender=receiver->q_sending;
			k=1;
		}
	}
	else
	{
		if((sender->flags&SENDING)&&(sender->send_to==receiver->index))
		{
			k=1;
			PROCESS* p_proc=receiver->q_sending;
			while(p_proc)
			{
				if(p_proc==sender)
					break;
				prev=p_proc;
				p_proc=p_proc->next_sending;
			}
		}
	}

	if(k)
	{
		if(sender==receiver->q_sending)
		{
			receiver->q_sending=sender->next_sending;
			sender->next_sending=0;
		}
		else{
			prev->next_sending=sender->next_sending;
			sender->next_sending=0;
		}
		memcpy(va2la(receiver,m),va2la(sender,sender->p_msg),sizeof(MESSAGE));
		sender->p_msg=0;
		sender->send_to=NONE;
		sender->flags&=~SENDING;
		wakeup(sender);
		//printf("wakeup sender\n");
	}
	else{
		receiver->flags|=RECEIVING;
		receiver->p_msg=m;
		receiver->recv_from=src;
		block(receiver);
		//printf("block receiver\n");
	}
	//printf("3333\n");
	return 0;
}

PUBLIC int sys_sendrec(int func,int src_dst,MESSAGE* m,PROCESS* p_proc)
{
	MESSAGE* mla=(MESSAGE*)va2la(p_proc,m);
	mla->source=p_proc-proc_table;
	switch(func)
	{
		case SEND:{
			return msg_send(p_proc,src_dst,m);
		}
		case RECEIVE:{
			return msg_receive(p_proc,src_dst,m);
		}
		default:{
			printf("function error in sys_sendrec!\n");
			break;
		}
	}
	return 0;
}

//inform a task process that an interrupt occurred.
//para:index of task in the proc_table
PUBLIC void inform_int(int task)
{
	PROCESS* p_proc=proc_table+task;
	if((p_proc->flags&RECEIVING)&&(p_proc->recv_from==INTERRUPT||p_proc->recv_from==ANY))
	{
		p_proc->p_msg->source=INTERRUPT;
		p_proc->p_msg->type=HARD_INT;
		//p_proc->p_msg=0;
		p_proc->has_int_msg=0;
		p_proc->flags&=~RECEIVING;
		p_proc->recv_from=NONE;
		wakeup(p_proc);
	}
	else{
		p_proc->has_int_msg=1;
	}
}

PUBLIC int send_recv(int func,int src_dst,MESSAGE* m)
{
	switch(func)
	{
		case BOTH:{
			sendrec(SEND,src_dst,m);
			//printf("1\n");
			sendrec(RECEIVE,src_dst,m);
			//printf("2\n");
			break;
		}
		case SEND:{
			sendrec(SEND,src_dst,m);
			break;
		}
		case RECEIVE:{
			memset(m,0,sizeof(MESSAGE));
			sendrec(func,src_dst,m);
			break;
		}
		default:
			printf("function error in send_recv\n");
	}
	return 0;
}

PUBLIC void task_sys()
{
	int time=ticks;
	while(ticks-time<TTY_DELAY);

	MESSAGE msg;
	while(1)
	{
		send_recv(RECEIVE,ANY,&msg);
		int src=msg.source;
		switch(msg.type)
		{
			case GET_TICKS:{
				msg.ret=ticks;
				send_recv(SEND,src,&msg);
				break;
			}
			default:{
				printf("message typr error in task_sys()!\n");
			}
		}
	}
}