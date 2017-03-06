#include "all.h"

PUBLIC int unlink(const char* path)
{
	MESSAGE msg;
	msg.type=UNLINK;
	msg.pathname=path;
	msg.len=strlen(path);
	send_recv(BOTH,TASK_FS,&msg);
	//printf("444\n");
	return msg.ret;
}

PUBLIC int do_unlink(MESSAGE* m)
{
	int i,j,k;
	char pathname[MAX_PATH];
	int len=m->len;
	int src=m->source;
	memcpy((void*)va2la(proc_table+TASK_FS,pathname),(void*)va2la(proc_table+src,m->pathname),len);
	pathname[len]='\0';

	if(pathname[0]=='/'&&len==1)
	{
		printf("error in do_unlink(),CAN NOT DELETE /\n");
		return -1;
	}
	int inode_no=search_file(pathname);
	if(inode_no==0)
	{
		printf("error in do_unlink(),pathname is wrong\n");
		return -1;
	}
	char filename[MAX_PATH];
	INODE* dir_inode;
	if(strip_path(filename,pathname,&dir_inode)==-1)
	{
		printf("error in do_unlink(),pathname is wrong\n");
		return -1;
	}
	INODE* pin=get_inode(inode_no);
	//printf("%d,%d\n", pin->i_num,pin->i_cnt);
	//printf("ino %d",inode_no);
	if(pin->i_cnt>2&&pin->i_mode==NORMAL_INODE)
	{
		//printf("%d\n", pin->i_cnt);
		printf("error in do_unlink(),file is being used now\n");
		return -1;
	}
	if(pin->i_mode==DIRECTORY_INODE&&pin->i_size>DIR_ENTRY_SIZE*2)
	{
		printf("error in do_unlink(),directory has files\n");
		return -1;
	}
	//free imap
	rw_sector(DEV_READ,2,1);
	fsbuf[inode_no/8]&=~(1<<(inode_no%8));
	rw_sector(DEV_WRITE,2,1);

	//free smap
	int sec_beg=pin->i_start_sect-sb.first_sect+1;
	int sec_end=sec_beg+pin->i_num_sects;
	int left=pin->i_num_sects;
	int pos=sec_beg/8;
	rw_sector(DEV_READ,3,sb.num_smaps_sects);
	for(i=sec_beg%8;i<8;++i)
	{
		fsbuf[pos]&=~(1<<i);
		left--;
	}
	pos++;
	for(;pos<sec_end/8;++pos)
	{
		fsbuf[pos]=0;
		left-=8;
	}
	if(left)
	{
		for(i=0;i<sec_end%8;++i)
			fsbuf[pos]&=~(1<<i);
	}
	rw_sector(DEV_WRITE,3,sb.num_smaps_sects);

	//printf("11111111111\n");
	//free inode
	pin->i_mode=0;
	pin->i_size=0;
	pin->i_start_sect=0;
	pin->i_num_sects=0;
	sync_inode(pin);
	pin->i_cnt=0;
	//printf("22222222222\n");
	//free dir_entry in dir_inode
	sec_beg=dir_inode->i_start_sect;
	int sec_num=dir_inode->i_size/SECTOR_SIZE+1;
	int dir_num=dir_inode->i_size/DIR_ENTRY_SIZE;
	dir_inode->i_size-=DIR_ENTRY_SIZE;
	sync_inode(dir_inode);
	k=0;
	for(i=0;i<sec_num;++i)
	{
		rw_sector(DEV_READ,sec_beg+i,1);
		DIR_ENTRY* pde=(DIR_ENTRY*)fsbuf;
		for(j=0;j<SECTOR_SIZE/DIR_ENTRY_SIZE;++j,++pde)
		{
			if(pde->inode==pin->i_num)
			{
				//printf("11111111\n");
				memset((void*)pde,0,DIR_ENTRY_SIZE);
				rw_sector(DEV_WRITE,sec_beg+i,1);
				return 0;
			}
		}
	}
	printf("error in do_unlink(),free dir_entry in dir_inode\n");
	return -1;
}