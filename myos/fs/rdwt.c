#include "all.h"

PUBLIC int read(int fd,char* buf,int cnt)
{
	MESSAGE m;
	m.type=READ;
	m.fd=fd;
	m.buf=buf;
	m.cnt=cnt;
	send_recv(BOTH,TASK_FS,&m);
	return m.cnt;
}

PUBLIC int write(int fd,char* buf,int cnt)
{
	MESSAGE m;
	m.type=WRITE;
	m.fd=fd;
	m.buf=buf;
	m.cnt=cnt;
	send_recv(BOTH,TASK_FS,&m);
	return m.cnt;
}

PUBLIC int do_rdwt(MESSAGE* m)
{
	int src=m->source;
	int len=m->cnt;
	int fd=m->fd;
	char* buf=m->buf;

	if(fd<0||fd>NUM_FILES||len<=0){
		printf("error in do_rdwt\n");
		return 0;
	}
	FILE_DESC* pfd=proc_table[src].filp[fd];
	int pos=pfd->pos;
	INODE* pin=pfd->p_inode;
	if(pin->i_mode==NORMAL_INODE)
	{
		int end;
		if(m->type==READ)
			end=pos+len>pin->i_size?pin->i_size:pos+len;
		else end=pos+len>(pin->i_num_sects*SECTOR_SIZE)?(pin->i_num_sects*SECTOR_SIZE):pos+len;

		int off=pos%SECTOR_SIZE;
		int sec_beg=pin->i_start_sect+pos/SECTOR_SIZE;
		int sec_end=pin->i_start_sect+end/SECTOR_SIZE;

		if(m->type==READ)
		{
			rw_sector(DEV_READ,sec_beg,sec_end-sec_beg+1);
			memcpy((void*)va2la(proc_table+src,buf),(void*)va2la(proc_table+TASK_FS,fsbuf+off),end-pos);
		}
		else{
			rw_sector(DEV_READ,sec_beg,sec_end-sec_beg+1);
			memcpy((void*)va2la(proc_table+TASK_FS,fsbuf+off),(void*)va2la(proc_table+src,buf),end-pos);
			rw_sector(DEV_WRITE,sec_beg,sec_end-sec_beg+1);
		}
		pfd->pos=end;
		if(pfd->pos > pin->i_size) pin->i_size = pfd->pos;
		sync_inode(pin);
	}
	else{
		printf("mode error in do_rdwt()\n");
		return 0;
	}
}