#include "all.h"

static INODE* dir;

PRIVATE void init_proc(int,int,int);

PUBLIC void task_shell()
{
	int time=ticks;
	while(ticks-time<TTY_DELAY);
	
	printf("\n          WELCOME  TO  MYOS - SweetOrange\n");
	printf("                  --Wang Shuocheng--\n");
	printf("#MYOS has 5 terminal! now you're in terminal 1 ^^\n");
	printf("#press: alt+F1 => go to terminal 1\n");
	printf("              ...           \n");
	printf("#press: alt+F5 => go to terminal 5\n");
	printf("#press: shift+up  =>scroll  up  the screen\n");
	printf("#press: shift+down=>scroll down the screen\n");
	printf("#press: (ESC when in terminal 1) or (alt+F1) => input instructions\n\n");
	printf("instructions: run,end,ls,mkdir,rm,cd \n");
	
	//show_dir(root_inode);
	printf("file system is starting...\n");
	while(fs_is_ready==0);
	printf("file system run successfully!\n");
	dir=root_inode;

	cin_cmd();
	enable_cin_cmd=0;
	while(1){
		if(enable_cin_cmd){
			cin_cmd();
			enable_cin_cmd=0;
		}
	}
}

PUBLIC void cin_cmd()
{
	int order=0;
	int pos=0;
	char cmd[80];
	printf(">>");
	cin_getline(cmd);
	int i,j,k,u;
	int len=strlen(cmd);
	for(i=0;i<len;++i)
	{
		if(cmd[i]=='r'&&cmd[i+1]=='u'&&cmd[i+2]=='n'){
			order=1;
			for(u=i+4;u<len;++u)
				if(cmd[u]!=' '){
					pos=u;
					break;
				}
			break;
		}
		else if(cmd[i]=='e'&&cmd[i+1]=='n'&&cmd[i+2]=='d')
		{
			order=2;
			for(u=i+4;u<len;++u)
				if(cmd[u]!=' '){
					pos=u;
					break;
				}
			break;
		}
		else if(cmd[i]=='c'&&cmd[i+1]=='d')
		{
			order=3;
			for(u=i+3;u<len;++u)
				if(cmd[u]!=' '){
					pos=u;
					break;
				}
			break;
		}
		else if(cmd[i]=='r'&&cmd[i+1]=='m')
		{
			order=4;
			for(u=i+3;u<len;++u)
				if(cmd[u]!=' '){
					pos=u;
					break;
				}
			break;
		}
		else if(cmd[i]=='m'&&cmd[i+1]=='k'&&cmd[i+2]=='d'&&cmd[i+3]=='i'&&cmd[i+4]=='r')
		{
			order=5;
			for(u=i+6;u<len;++u)
				if(cmd[u]!=' '){
					pos=u;
					break;
				}
			break;
		}
		else if(cmd[i]=='l'&&cmd[i+1]=='s')
		{
			order=6;
			for(u=i+3;u<len;++u)
				if(cmd[u]!=' '){
					pos=u;
					break;
				}
			break;
		}
	}

	char path[MAX_PATH];
	k=0;
	for(i=pos;i<strlen(cmd);++i)
	{
		if(cmd[i]==' ')
			break;
		path[k++]=cmd[i];
	}
	path[k]='\0';

	if(order==1)//run
	{
		if(pos==0){
			printf("No program is assigned!\n");
		}
		else{
			int num=0;
			PROCESS* p_proc;
			int tty;
			for(tty=0;tty<NUM_CONSOLES;++tty)
			{
				if(tty_table[tty].num_proc==0)
					num++;
			}
			if(num==0){
				printf("TTY are full! We have no tty to run this program!\n");
				return;
			}
			for(i=pos;i<len;++i)
			{
				if(cmd[i]>='1'&&cmd[i]<='6')
				{
					if(num==0)
					{
						printf("TTY are full! We have no tty to run this program!\n");
						return;
					}
					int suc=1;
					int user=cmd[i]-'0';
					for(p_proc=proc_table+NUM_TASKS;p_proc<proc_table+NUM_TASKS+NUM_USERS;++p_proc)
					{
						if(p_proc->id==user-1+USER_ID_BASE&&p_proc->state!=EXIT)
						{
							printf("User Program %d is running already.\n",user);
							suc=0;
							break;
						}
					}
					if(suc)
					{
						for(p_proc=proc_table+NUM_TASKS;p_proc<proc_table+NUM_TASKS+NUM_USERS;++p_proc)
							if(p_proc->state==EXIT) break;
						for(tty=0;tty<NUM_CONSOLES;++tty)
						{
							if(tty_table[tty].num_proc==0)
							{
								printf("User Program %d is running successfully\n",user);
								num--;
								init_proc(user-1,p_proc-proc_table,tty);
								break;
							}
						}
					}
				}
			}
		}
	}
	else if(order==2)//end
	{
		if(pos==0){
			printf("No program is assigned!\n");
		}
		else{
			int num=0;
			PROCESS* p_proc=proc_table+NUM_TASKS;
			for(;p_proc<proc_table+NUM_TASKS+NUM_USERS;++p_proc)
				if(p_proc->state!=EXIT) num++; 
			if(num==0){
				printf("No User Program is running!\n");
				return;
			}
			for(i=pos;i<len;++i)
			{
				if(cmd[i]>='1'&&cmd[i]<='6')
				{
					int suc=0;
					if(num==0)
					{
						printf("No User Program is running!\n");
						return;
					}
					int user=cmd[i]-'0';
					for(p_proc=proc_table+NUM_TASKS;p_proc<proc_table+NUM_TASKS+NUM_USERS;++p_proc)
					{
						int id=p_proc->id%SUB_ID_BASE;
						if(p_proc->state!=EXIT&&id==user-1+USER_ID_BASE)
						{
							tty_table[p_proc->tty].num_proc--;
							//printf("%d\n",tty_table[p_proc->tty].num_proc );
							int t=get_ticks();
							while(p_proc->state==BLOCKED){
								if(get_ticks()-t>400) break;
							}
							p_proc->state=EXIT;
							suc=1;
						}
					}
					if(!suc) printf("User Program %d doesn't run.\n",user );
					else printf("User Program %d is closed successfully.\n",user);
				}
			}
		}
	}
	else if(order==3)//cd
	{
		INODE* pin=deal_path(root_inode,path,DIRECTORY_INODE);
		if(pin!=0)
		{
			dir=pin;
			show_dir(dir);
		}
		else printf("error in cd\n");
	}
	else if(order==4)//rm
	{
		INODE* pin=deal_path(root_inode,path,DIRECTORY_INODE);
		if(pin!=0)
		{
			unlink(path);
		}
		else printf("error in rm\n");
		
	}
	else if(order==5)//mkdir
	{
		if(check_path(path))
			makedir(path);
		else printf("error in mkdir\n");
	}
	else if(order==6)//ls
	{
		show_dir(dir);
	}
	else{
		printf("Instruction Wrong!!\n");
	}
}

PRIVATE void init_proc(int user_table_index,int proc_table_index,int tty)
{
	if(user_table_index<0||user_table_index>6)
	{
		printf("The user program doesn't exist!\n");
		return;
	}
	if(proc_table_index<NUM_TASKS||proc_table_index>=NUM_TASKS+NUM_USERS)
	{
		printf("The index of process table is out of range!\n");
		return;
	}
	PROCESS* p_proc=proc_table+proc_table_index;
	p_proc->state=EXIT;//shut down the original program,if exist
	USER* p_user=user_table+user_table_index;
	char* p_user_stack=user_stack+(proc_table_index-NUM_TASKS+1)*STACK_SIZE_USER;

	u16 selector_ldt=SELECTOR_LDT_FIRST+8*proc_table_index;
	
	u8 privilege=PRIVILEGE_USER;
	u8 rpl=RPL_USER;
	u16 eflags=0x202;

	strcpy(p_proc->p_name,p_user->name);
	p_proc->id=user_table_index+USER_ID_BASE;
	p_proc->fid=-1;//is not subprogram
	/*ldt*/
	p_proc->ldt_sel=selector_ldt;
	memcpy(&p_proc->ldts[0],&gdt[SELECTOR_KERNAL_CS>>3],sizeof(DESCRIPTOR));
	p_proc->ldts[0].attr1=DA_C|privilege<<5;
	memcpy(&p_proc->ldts[1],&gdt[SELECTOR_KERNAL_DS>>3],sizeof(DESCRIPTOR));
	p_proc->ldts[1].attr1=DA_DRW|privilege<<5;
	/*regs*/
	p_proc->regs.eip=(u32)p_user->initial_eip;
	p_proc->regs.esp=(u32)p_user_stack;
	p_proc->regs.eflags=eflags;
	p_proc->regs.cs=(0&SA_RPL_MASK&SA_TI_MASK)|SA_TIL|rpl;
	p_proc->regs.ds=(8&SA_RPL_MASK&SA_TI_MASK)|SA_TIL|rpl;
	p_proc->regs.es=(8&SA_RPL_MASK&SA_TI_MASK)|SA_TIL|rpl;
	p_proc->regs.fs=(8&SA_RPL_MASK&SA_TI_MASK)|SA_TIL|rpl;
	p_proc->regs.ss=(8&SA_RPL_MASK&SA_TI_MASK)|SA_TIL|rpl;
	p_proc->regs.gs=(SELECTOR_KERNAL_GS&SA_RPL_MASK)|rpl;

	int i;
	for(i=0;i<NUM_FILES;++i) p_proc->filp[i]=0;
	//p_proc->tty=proc_table_index-NUM_TASKS+1;
	init_proc_ipc(p_proc);
	p_proc->tty=tty;
	if(tty_table[tty].num_proc==0)
		init_tty(&tty_table[p_proc->tty]);
	tty_table[tty].num_proc++;
	
	p_proc->state=READY;
}