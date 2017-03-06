#include "all.h"

PRIVATE void init_proc(int,int);

PUBLIC int kernal_main()
{
	initial_sem();
	
	int i,j;
	for(i=0;i<NUM_USERS+NUM_TASKS;++i)
		proc_table[i].state=EXIT;
	for(i=0;i<NUM_CONSOLES;++i)
		tty_table[i].num_proc=0;
	/*task*/
	PROCESS* p_proc=proc_table;
	TASK* p_task=task_table;
	char* p_task_stack=task_stack+TASK_STACK_SIZE_TOTAL;

	u16 selector_ldt=SELECTOR_LDT_FIRST;
	
	u8 privilege;
	u8 rpl;
	u16 eflags;

	for(i=0;i<NUM_TASKS;++i)
	{
		privilege=PRIVILEGE_TASK;
		rpl=RPL_TASK;
		eflags=0x1202;

		strcpy(p_proc->p_name,p_task->name);
		p_proc->regs.eip=(u32)p_task->initial_eip;
		p_proc->regs.esp=(u32)p_task_stack;
		p_proc->regs.eflags=eflags;
		p_task_stack-=p_task->stacksize;
		
		p_proc->id=i;
		p_proc->fid=-1;//is not subprogram
		/*ldt*/
		p_proc->ldt_sel=selector_ldt;
		memcpy(&p_proc->ldts[0],&gdt[SELECTOR_KERNAL_CS>>3],sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1=DA_C|privilege<<5;
		memcpy(&p_proc->ldts[1],&gdt[SELECTOR_KERNAL_DS>>3],sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1=DA_DRW|privilege<<5;
		/*regs*/
		p_proc->regs.cs=(0&SA_RPL_MASK&SA_TI_MASK)|SA_TIL|rpl;
		p_proc->regs.ds=(8&SA_RPL_MASK&SA_TI_MASK)|SA_TIL|rpl;
		p_proc->regs.es=(8&SA_RPL_MASK&SA_TI_MASK)|SA_TIL|rpl;
		p_proc->regs.fs=(8&SA_RPL_MASK&SA_TI_MASK)|SA_TIL|rpl;
		p_proc->regs.ss=(8&SA_RPL_MASK&SA_TI_MASK)|SA_TIL|rpl;
		p_proc->regs.gs=(SELECTOR_KERNAL_GS&SA_RPL_MASK)|rpl;

		for(j=0;j<NUM_FILES;++j) p_proc->filp[j]=0;

		init_proc_ipc(p_proc);
		p_proc->tty=0;
		tty_table[0].num_proc++;
		//p_proc->wait_sem=0;
		p_proc->state=READY;
		p_task++;
		p_proc++;
		selector_ldt+=8;
	}
	re_enter=0;
	p_proc_ready=proc_table;

	allow_cin=1;
	enable_cin_cmd=0;

	init_clock();

	init_keyboard();

	restart();
	
	while(1);
}


