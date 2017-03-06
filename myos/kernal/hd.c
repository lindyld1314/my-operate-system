#include "all.h"

PRIVATE void init_hd();
PRIVATE void hd_open();
PRIVATE void hd_close();
PRIVATE void hd_rdwt(MESSAGE*);
PRIVATE void hd_ioctl(MESSAGE*);

PRIVATE void hd_identify(int drive);
PRIVATE void hd_cmd_out(HD_CMD* cmd);
PRIVATE void interrupt_wait();
PRIVATE int waitfor(int mask,int val,int timeout);
PRIVATE void print_identify_info(u16* hdinfo);

PRIVATE	u8 hd_status;
PRIVATE	u8 hdbuf[SECTOR_SIZE*2];

//task_hd
PUBLIC void task_hd()
{
	int time=ticks;
	while(ticks-time<TTY_DELAY);
	
	MESSAGE msg;
	init_hd();

	while(1)
	{
		send_recv(RECEIVE,ANY,&msg);
		int src=msg.source;

		switch(msg.type)
		{
			case DEV_OPEN:{
				hd_open();
				break;
			}
			case DEV_CLOSE:{
				hd_close();
				break;
			}
			case DEV_READ:{
				hd_rdwt(&msg);
				break;
			}
			case DEV_WRITE:{
				hd_rdwt(&msg);
				break;
			}
			case DEV_IOCTL:{
				hd_ioctl(&msg);
				break;
			}
			default:{
				printf("message type error in task_hd()\n");
				break;
			}
		}
		send_recv(SEND,src,&msg);
	}
}

PUBLIC void hd_handler(int irq)
{
	hd_status=in_byte(REG_STATUS);
	inform_int(TASK_HD);
}

PRIVATE void init_hd()
{
	put_irq_handler(AT_WINI_IRQ,hd_handler);
	enable_irq(CASCADE_IRQ);/* cascade enable for 2nd AT controller,master 2 */
	enable_irq(AT_WINI_IRQ);/* at winchester,slave 14 */
}

PRIVATE void hd_open()
{
	hd_identify(0);
}

PRIVATE void hd_close()
{
	printf("hd_close!\n");
	;
}

PRIVATE void hd_rdwt(MESSAGE* m)
{
	HD_CMD cmd;
	cmd.features=0;
	cmd.count=(m->cnt+SECTOR_SIZE-1)/SECTOR_SIZE;
	cmd.lba_low=(m->sec_no)&0xff;
	cmd.lba_mid=(m->sec_no>>8)&0xff;
	cmd.lba_high=(m->sec_no>>16)&0xff;
	cmd.device=MAKE_DEVICE_REG(1,0,(m->sec_no>>24)&0xf);
	cmd.command=(m->type==DEV_READ)?COMMAND_ATA_READ:COMMAND_ATA_WRITE;
	hd_cmd_out(&cmd);

	int byte_left=m->cnt;
	void* la=(void*)va2la(proc_table+m->proc_index,m->buf);
	while(byte_left)
	{
		int bytes=SECTOR_SIZE>byte_left?byte_left:SECTOR_SIZE;
		if(m->type==DEV_READ)
		{
			interrupt_wait();
			port_read(REG_DATA,hdbuf,SECTOR_SIZE);
			memcpy(la,(void*)va2la(proc_table+TASK_HD,hdbuf),bytes);
		}
		else
		{
			if(!waitfor(STATUS_DRQ,STATUS_DRQ,2000))
				printf("ERROR in hd_rdwt in DEV_WRITE\n");
			port_write(REG_DATA,la,bytes);
			interrupt_wait();
		}
		la+=SECTOR_SIZE;
		byte_left-=SECTOR_SIZE;
	}
}
PRIVATE void hd_ioctl(MESSAGE* m)
{
	if(m->request==IOCTL_GET_NUM_SEC)
	{
		HD_CMD cmd;
		cmd.device=MAKE_DEVICE_REG(0,0,0);
		cmd.command=COMMAND_ATA_IDENTIFY;
		hd_cmd_out(&cmd);
		interrupt_wait();
		port_read(REG_DATA,hdbuf,SECTOR_SIZE);
		u16* hdinfo=(u16*)hdbuf;
		int sectors = ((int)hdinfo[61] << 16) + hdinfo[60];
		m->ret=sectors;
	}
	else printf("request error in hd_ioctl\n");
}

//wait an interrupt occurred
PRIVATE void interrupt_wait()
{
	MESSAGE msg;
	send_recv(RECEIVE,INTERRUPT,&msg);
}

//wait for a certain status
PRIVATE int waitfor(int mask,int val,int timeout)
{
	int time=get_ticks();
	while(get_ticks()-time<timeout)
		if((in_byte(REG_STATUS)&mask)==val)
			return 1;
	return 0;
}

PRIVATE void hd_cmd_out(HD_CMD* cmd)
{
	//when BSY=0, we can out cmd
	if(!waitfor(STATUS_BSY,0,2000))
		printf("hd_cmd_out() time out!\n");
	//open interrupt
	out_byte(REG_DEVICE_CONTROL,0);
	//out data
	out_byte(REG_FEATURES,cmd->features);
	out_byte(REG_SECTOR_COUNT,cmd->count);
	out_byte(REG_LBA_LOW,cmd->lba_low);
	out_byte(REG_LBA_MID,cmd->lba_mid);
	out_byte(REG_LBA_HIGH,cmd->lba_high);
	out_byte(REG_DEVICE,cmd->device);
	//out cmd
	out_byte(REG_COMMAND,cmd->command);
}

//master hd:drive=0,  slave hd:drive=1;
PRIVATE void hd_identify(int drive)
{
	HD_CMD cmd;
	cmd.device=MAKE_DEVICE_REG(0,drive,0);
	cmd.command=COMMAND_ATA_IDENTIFY;
	hd_cmd_out(&cmd);
	interrupt_wait();
	port_read(REG_DATA,hdbuf,SECTOR_SIZE);
	print_identify_info((u16*)hdbuf);
}
//this function is copyed from orange's directly
PRIVATE void print_identify_info(u16* hdinfo)
{
	int i, k;
	char s[64];

	struct iden_info_ascii {
		int idx;
		int len;
		char * desc;
	} iinfo[] = {{10, 20, "HD SN"}, /* Serial number in ASCII */
		     {27, 40, "HD Model"} /* Model number in ASCII */ };

	for (k = 0; k < sizeof(iinfo)/sizeof(iinfo[0]); k++) {
		char * p = (char*)&hdinfo[iinfo[k].idx];
		for (i = 0; i < iinfo[k].len/2; i++) {
			s[i*2+1] = *p++;
			s[i*2] = *p++;
		}
		s[i*2] = 0;
		printf("%s: %s\n", iinfo[k].desc, s);
	}

	int capabilities = hdinfo[49];
	printf("LBA supported: %s\n",
	       (capabilities & 0x0200) ? "Yes" : "No");

	int cmd_set_supported = hdinfo[83];
	printf("LBA48 supported: %s\n",
	       (cmd_set_supported & 0x0400) ? "Yes" : "No");

	int sectors = ((int)hdinfo[61] << 16) + hdinfo[60];
	printf("HD size: %dMB\n", sectors * 512 / 1000000);
	//printf("%x\n",sectors);
}





