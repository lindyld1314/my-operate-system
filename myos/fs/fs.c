#include "all.h"

PRIVATE void makefs();

PUBLIC void task_fs()
{
	int time=ticks;
	while(ticks-time<TTY_DELAY);
	
	MESSAGE msg;
	
	makefs();

	int i;
	for(i=0;i<MAX_FS_TABLE_SIZE;++i)
	{
		memset(f_desc_table+i,0,sizeof(FILE_DESC));
		memset(inode_table+i,0,sizeof(INODE));
	}

	root_inode=get_inode(ROOT_INODE);
	//printf("taskfs %d\n",root_inode->i_num_sects);
	
	fs_is_ready=1;

	while(1)
	{
		send_recv(RECEIVE,ANY,&msg);
		int src=msg.source;
		switch(msg.type)
		{
			case OPEN:{
				msg.fd=do_open(&msg);
				break;
			}
			case CLOSE:{
				msg.ret=do_close(&msg);
				break;
			}
			case WRITE:{
				msg.cnt=do_rdwt(&msg);
				break;
			}
			case READ:{
				msg.cnt=do_rdwt(&msg);
				break;
			}
			case UNLINK:{
				msg.ret=do_unlink(&msg);
				break;
			}
			default:{
				printf("type error in task_fs()\n");
			}
		}
		msg.type=SYS_RET;
		send_recv(SEND,src,&msg);
	}
}

PRIVATE void makefs()
{
	MESSAGE msg;
	//get num of sectors
	msg.type=DEV_IOCTL;
	msg.request=IOCTL_GET_NUM_SEC;
	send_recv(BOTH,TASK_HD,&msg);

	//super block
	sb.magic_number=666;
	sb.inode_size=INODE_SIZE;
	sb.num_inodes=SECTOR_SIZE*8;
	sb.num_inodes_sects=sb.num_inodes*INODE_SIZE/SECTOR_SIZE;
	sb.num_imaps_sects=1;
	sb.num_sects=msg.ret;
	sb.num_smaps_sects=sb.num_sects/(SECTOR_SIZE*8)+1;

	sb.first_sect=1+1+sb.num_imaps_sects+sb.num_smaps_sects+sb.num_inodes_sects;
	//printf("%x,%x,%x\n",sb.num_imaps_sects, sb.num_smaps_sects,sb.num_inodes_sects);
	sb.root_inode=ROOT_INODE;

	INODE tmp_inode;
	sb.inode_isize_off=(int)&tmp_inode.i_size-(int)&tmp_inode;
	sb.inode_istart_off=(int)&tmp_inode.i_start_sect-(int)&tmp_inode;
	
	sb.dir_entry_size=DIR_ENTRY_SIZE;
	DIR_ENTRY tmp_dir_entry;
	sb.dir_entry_inode_off=(int)&tmp_dir_entry.inode-(int)&tmp_dir_entry;
	sb.dir_entry_name_off=(int)&tmp_dir_entry.name-(int)&tmp_dir_entry;

	memset(fsbuf,0x90,SECTOR_SIZE);
	memcpy(fsbuf,&sb,SUPER_BLOCK_SIZE);

	rw_sector(DEV_WRITE,1,1);

	//printf("%x,%x,%x,%x,%x",512,1024,1536,1536+sb.num_smaps_sects*SECTOR_SIZE,1536+(sb.num_smaps_sects+sb.num_inodes_sects)*SECTOR_SIZE);

	//inode map
	memset(fsbuf,0,SECTOR_SIZE);
	/*
		bit 0: reserved
		bit 1: first node,root node
		bit 2: user.tar
	*/
	fsbuf[0]=0x7;//0000 0111,
	rw_sector(DEV_WRITE,2,1);

	//sector map
	memset(fsbuf,0,SECTOR_SIZE*sb.num_smaps_sects);
	int i,j;
	//root dir:NUM_FILE_SECTS, Reserved:1
	int numof1=NUM_FILE_SECTS+1;
	for(i=0;i<numof1/8;++i)
		fsbuf[i]=0xff;
	for(j=0;j<numof1%8;++j)
		fsbuf[i]|=(1<<j);
	//user.tar
	int install_offset=INSTALL_START_SECT-sb.first_sect+1;
	if(install_offset%8==0)
	{
		for(i=install_offset/8;i<(install_offset+INSTALL_NUM_SECTS)/8;++i)
			fsbuf[i]=0xff;
		for(j=0;j<(install_offset+INSTALL_NUM_SECTS)%8;++j)
			fsbuf[i]|=(1<<j);
	}
	else{
		int left=INSTALL_NUM_SECTS;
		for(j=install_offset%8;j<8;++j)
		{
			fsbuf[install_offset/8]|=(1<<j);
			left--;
		}
		for(i=install_offset/8+1;i<install_offset/8+1+left/8;++i)
		{
			fsbuf[i]=0xff;
		}
		for(j=0;j<left%8;++j)
			fsbuf[i]|=(1<<j);
	}

	rw_sector(DEV_WRITE,2+sb.num_imaps_sects,sb.num_smaps_sects);

	//inodes
	memset(fsbuf,0,SECTOR_SIZE);
	// inode /
	INODE* p_inode=(INODE*)fsbuf;
	p_inode->i_mode=DIRECTORY_INODE;
	p_inode->i_size=DIR_ENTRY_SIZE*2;// . user.tar
	p_inode->i_start_sect=sb.first_sect;
	p_inode->i_num_sects=NUM_FILE_SECTS;
	// inode user.tar
	p_inode=(INODE*)(fsbuf+INODE_SIZE);
	p_inode->i_mode=NORMAL_INODE;
	p_inode->i_size=INSTALL_NUM_SECTS*SECTOR_SIZE;
	p_inode->i_start_sect=INSTALL_START_SECT;
	p_inode->i_num_sects=INSTALL_NUM_SECTS;
	rw_sector(DEV_WRITE,2+sb.num_imaps_sects+sb.num_smaps_sects,1);
	
	//  data /
	memset(fsbuf,0,SECTOR_SIZE);
	DIR_ENTRY* p_dir=(DIR_ENTRY*)fsbuf;
	p_dir->inode=1;
	strcpy(p_dir->name,".");
	p_dir++;
	p_dir->inode=2;
	strcpy(p_dir->name,"user.tar");
	rw_sector(DEV_WRITE,sb.first_sect,1);
	printf("make fs successfully\n");
}

PUBLIC int rw_sector(int type,int sec_no,int num_sec)
{
	MESSAGE msg;

	msg.type=type;
	msg.sec_no=sec_no;
	msg.buf=fsbuf;
	msg.cnt=SECTOR_SIZE*num_sec;
	msg.proc_index=TASK_FS;
	send_recv(BOTH,TASK_HD,&msg);
}


PUBLIC INODE* get_dir_inode(const char* path)
{
	INODE* pin=root_inode;
	int i,j,k,u,v,pos,beg;
	int len=strlen(path);
	int sec_beg,sec_num,dir_num;
	DIR_ENTRY* pde;

	if(path[0]=='/'&&len==1)
	{
		return root_inode;
	}
	char pathname[MAX_PATH];
	char filename[MAX_PATH];
	if(path[0]=='/')
	{
		pin=root_inode;
		beg=1;
		pos=len;
		k=0;
		for(i=1;i<len;++i)
		{
			if(path[i]=='/'&&i<len-1)
			{
				if(pin->i_mode!=DIRECTORY_INODE)
					return root_inode;
				k=0;
				for(j=beg;j<i;++j)
					filename[k++]=path[j];
				beg=i+1;
				filename[k]='\0';
				k=0;
				sec_beg=pin->i_start_sect;
				sec_num=pin->i_size/SECTOR_SIZE+1;
				dir_num=pin->i_size/DIR_ENTRY_SIZE;
				rw_sector(DEV_READ,sec_beg,sec_num);
				pde=(DIR_ENTRY*)fsbuf;
				u=0;
				for(;pde<fsbuf+sec_num*SECTOR_SIZE;++pde)
				{
					if(strcmp(pde->name,filename,12)==0)
					{
						pin=get_inode(pde->inode);
						break;
					}
					else if(pde->inode!=0)
					{
						u++;
						if(u==dir_num) return root_inode;
					}
				}
			}
		}
		//printf("%s\n",filename);
	}
	return pin;
} 
//failed,return -1
PUBLIC int strip_path(char* filename,const char* path,INODE** ppinode)
{
	int pos=0;
	int beg=0;
	int i;
	int len=strlen(path);
	for(i=0;i<len;++i)
	{
		if(path[i]=='/')
		{
			beg=0;
			pos=i;
			if(i==len-1)
			{
				printf("error in strip_path()\n");
				return -1;
			}
		}
		else
		{
			//printf("%c",path[i]);
			filename[beg++]=path[i];
		}
	}
	filename[beg]='\0';
	//printf("filename in strip:%s\n",filename);
	char pure_path[MAX_PATH];
	strcpy(pure_path,path);
	if(pos==0)
		pure_path[1]='\0';
	else pure_path[pos]='\0';
	//printf("%s\n",pure_path);
	*ppinode=get_dir_inode(path);
	//printf("aaaaa%d\n",(*ppinode)->i_num);
	if(*ppinode==0) return -1;
	return 0;
}
// failed,return 0
PUBLIC int search_file(char* path)
{
	int i,j,k;
	char filename[MAX_PATH];
	memset(filename,0,MAX_PATH);
	INODE* dir;
	if(strip_path(filename,path,&dir)!=0)
		return 0;
	//printf("filename:%s\n",filename);
	int secno=dir->i_start_sect;
	int numsec=(dir->i_size-1)/SECTOR_SIZE+1;//when isize%section_size==0, it need -1
	int numdir=dir->i_size/DIR_ENTRY_SIZE;
	DIR_ENTRY* pde;
	k=0;
	for(i=0;i<numsec;++i)
	{
		rw_sector(DEV_READ,secno+i,1);
		pde=(DIR_ENTRY*)fsbuf;
		for(j=0;j<SECTOR_SIZE/DIR_ENTRY_SIZE;++j,pde++)
		{
			if(strcmp(filename,pde->name,12)==0)
				return pde->inode;
			if(pde->inode!=0) k++;
			if(k>numdir+1) break;
		}
		if(k>numdir+1) break;
	}
	return 0;
}

PUBLIC INODE* get_inode(int num)
{
	if(num==0) return 0;
	struct inode* p=0;
	struct inode* q=0;
	for(p=inode_table;p<inode_table+MAX_FS_TABLE_SIZE;++p)
	{
		if(p->i_cnt>0)
		{
			if(p->i_num==num)
			{
				p->i_cnt++;
				return p;
			}
		}
		else
		{
			if(q==0)
				q=p;
		}
	}
	if(q==0)
	{
		printf("inode_table is full in get_inode\n");
		return 0;
	}
	q->i_num=num;
	q->i_cnt=1;

	u32 inode_size=INODE_SIZE;
	rw_sector(DEV_READ,2+sb.num_imaps_sects+sb.num_smaps_sects+(num-1)/(SECTOR_SIZE/inode_size),1);
	INODE* p_inode=(INODE*)(fsbuf+((num-1)%(SECTOR_SIZE/inode_size))*inode_size);

	q->i_mode=p_inode->i_mode;
	q->i_size=p_inode->i_size;
	q->i_start_sect=p_inode->i_start_sect;
	q->i_num_sects=p_inode->i_num_sects;
	//printf("aaaaaaaaaaaa%d\n",q->i_cnt);
	//printf("%d,%d,%d,\n", q->i_size,q->i_start_sect,q->i_num_sects);
	return q;
}

PUBLIC void put_inode(INODE* p)
{
	if(p->i_cnt<=0)
		printf("error in put_inode\n");
	else p->i_cnt--;
}

PUBLIC void sync_inode(INODE* p)
{
	u32 inode_size=INODE_SIZE;
	rw_sector(DEV_READ,2+sb.num_imaps_sects+sb.num_smaps_sects+(p->i_num-1)/(SECTOR_SIZE/inode_size),1);
	INODE* inode=(INODE*)(fsbuf+((p->i_num-1)%(SECTOR_SIZE/inode_size))*inode_size);
	inode->i_mode=p->i_mode;
	inode->i_size=p->i_size;
	inode->i_start_sect=p->i_start_sect;
	inode->i_num_sects=p->i_num_sects;
	rw_sector(DEV_WRITE,2+sb.num_imaps_sects+sb.num_smaps_sects+(p->i_num-1)/(SECTOR_SIZE/inode_size),1);
}

PUBLIC void show_dir(INODE* dir_inode)
{
	printf("----file/dir----\n");
	int i,j,k;
	int sec_beg=dir_inode->i_start_sect;
	int sec_num=dir_inode->i_size/SECTOR_SIZE+1;
	int dir_num=dir_inode->i_size/DIR_ENTRY_SIZE;
	k=0;
	for(i=0;i<sec_num;++i)
	{
		rw_sector(DEV_READ,sec_beg+i,1);
		DIR_ENTRY* pde=(DIR_ENTRY*)fsbuf;
		for(j=0;j<SECTOR_SIZE/DIR_ENTRY_SIZE;++j,++pde)
		{
			if(pde->inode!=0)
			{
				printf("   %s\n",pde->name);
				k++;
			}
			if(k==dir_num)
			{
				printf("------end------\n");
				return ;
			}	
		}
	}
}

//0= wrong
PUBLIC INODE* deal_path(INODE* dir,char* path,int type)
{
	INODE* pin=dir;
	int i,j,k,u,v,pos,beg;
	int len=strlen(path);
	int sec_beg,sec_num,dir_num;
	DIR_ENTRY* pde;

	if(path[0]=='/'&&len==1)
	{
		if(type==DIRECTORY_INODE) return root_inode;
		else return 0;
	}
	char pathname[MAX_PATH];
	char filename[MAX_PATH];
	if(path[0]=='/')
	{
		pin=root_inode;
		beg=1;
		pos=len;
		k=0;
		for(i=1;i<len;++i)
		{
			if(path[i]=='/'&&i<len-1)
			{
				if(pin->i_mode!=DIRECTORY_INODE)
					return 0;
				k=0;
				for(j=beg;j<i;++j)
					filename[k++]=path[j];
				beg=i+1;
				filename[k]='\0';
				k=0;
				sec_beg=pin->i_start_sect;
				sec_num=pin->i_size/SECTOR_SIZE+1;
				dir_num=pin->i_size/DIR_ENTRY_SIZE;
				rw_sector(DEV_READ,sec_beg,sec_num);
				pde=(DIR_ENTRY*)fsbuf;
				u=0;
				for(;pde<fsbuf+sec_num*SECTOR_SIZE;++pde)
				{
					if(strcmp(pde->name,filename,12)==0)
					{
						pin=get_inode(pde->inode);
						break;
					}
					else if(pde->inode!=0)
					{
						u++;
						if(u==dir_num) return 0;
					}
				}
			}
			else
			{
				filename[k++]=path[i];
				filename[k]='\0';
			}
		}
		//printf("%s\n",filename);
		sec_beg=pin->i_start_sect;
		sec_num=pin->i_size/SECTOR_SIZE+1;
		dir_num=pin->i_size/DIR_ENTRY_SIZE;
		rw_sector(DEV_READ,sec_beg,sec_num);
		pde=(DIR_ENTRY*)fsbuf;
		k=0;
		for(;pde<fsbuf+sec_num*SECTOR_SIZE;++pde)
		{
			if(strcmp(pde->name,filename,12)==0)
			{
				pin=get_inode(pde->inode);
				return pin;
			}
			else if(pde->inode!=0)
			{
				k++;
				if(k==dir_num) return 0;
			}
		}
	}
	return 0;
}

//for makedir
PUBLIC int check_path(char* path)
{
	INODE* pin=root_inode;
	int i,j,k,u,v,pos,beg;
	int len=strlen(path);
	int sec_beg,sec_num,dir_num;
	DIR_ENTRY* pde;

	if(path[0]=='/'&&len==1)
	{
		return 0;
	}
	char pathname[MAX_PATH];
	char filename[MAX_PATH];
	if(path[0]=='/')
	{
		pin=root_inode;
		beg=1;
		pos=len;
		k=0;
		for(i=1;i<len;++i)
		{
			if(path[i]=='/'&&i<len-1)
			{
				if(pin->i_mode!=DIRECTORY_INODE)
					return 0;
				k=0;
				for(j=beg;j<i;++j)
					filename[k++]=path[j];
				beg=i+1;
				filename[k]='\0';
				k=0;
				sec_beg=pin->i_start_sect;
				sec_num=pin->i_size/SECTOR_SIZE+1;
				dir_num=pin->i_size/DIR_ENTRY_SIZE;
				rw_sector(DEV_READ,sec_beg,sec_num);
				pde=(DIR_ENTRY*)fsbuf;
				u=0;
				for(;pde<fsbuf+sec_num*SECTOR_SIZE;++pde)
				{
					if(strcmp(pde->name,filename,12)==0)
					{
						pin=get_inode(pde->inode);
						//printf("aaaaa%d\n",pin->i_num);
						break;
					}
					else if(pde->inode!=0)
					{
						u++;
						if(u==dir_num) return 0;
					}
				}
			}
			else
			{
				filename[k++]=path[i];
				filename[k]='\0';
			}
		}
		//printf("%s\n",filename);
		sec_beg=pin->i_start_sect;
		sec_num=pin->i_size/SECTOR_SIZE+1;
		dir_num=pin->i_size/DIR_ENTRY_SIZE;
		rw_sector(DEV_READ,sec_beg,sec_num);
		pde=(DIR_ENTRY*)fsbuf;
		k=0;
		for(;pde<fsbuf+sec_num*SECTOR_SIZE;++pde)
		{
			if(strcmp(pde->name,filename,12)==0)
			{
				return 0;
			}
			else if(pde->inode!=0)
			{
				k++;
				if(k==dir_num) return 1;
			}
		}
	}
	return 0;
}