#include "all.h"

PUBLIC void clock_handler()
{
	ticks++;
	if (re_enter!= 0) {
		return;
	}
	if(p_proc_ready->state==RUN)
		p_proc_ready->state=READY;

	p_proc_ready++;
	if (p_proc_ready >= proc_table + NUM_TASKS + NUM_USERS)
		p_proc_ready = proc_table;

	int i=0;
	while(p_proc_ready->state!=READY)
	{
		p_proc_ready++;
		if (p_proc_ready >= proc_table + NUM_TASKS + NUM_USERS)
			p_proc_ready = proc_table;
		i++;
		if(i>NUM_TASKS + NUM_USERS) break;
	}
	p_proc_ready->state=RUN;
}

PUBLIC void init_clock()
{
	ticks=0;
	disable_int();
	put_irq_handler(CLOCK_IRQ,clock_handler);
	enable_irq(CLOCK_IRQ);
	enable_int();
}