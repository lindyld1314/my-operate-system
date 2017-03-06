#include "all.h"

PRIVATE INODE* create_file(char* path,int flag);
PRIVATE int alloc_imap_bit();
PRIVATE int alloc_smap_bit(int num);
PRIVATE INODE* new_inode(int inode_no,int free_sect_no,int type);
PRIVATE void new_dir_entry(INODE* dir_inode,int inode_no,char* filename);


PUBLIC int open(const char* pathname)
{
	MESSAGE msg;
	msg.type=OPEN;
	msg.pathname=(void*)pathname;
	msg.len=strlen(pathname);
	send_recv(BOTH,TASK_FS,&msg);
	if(msg.type!=SYS_RET)
		printf("error in open()\n");
	return msg.fd;
}

PUBLIC int close(int fd)
{
	MESSAGE msg;
	msg.type=CLOSE;
	msg.fd=fd;
	send_recv(BOTH, TASK_FS, &msg);
	return msg.ret;
}

PUBLIC int do_open(MESSAGE* m)
{
	int fd=-1;
	char pathname[MAX_PATH];
	int len=m->len;
	int src=m->source;
	memcpy((void*)va2la(proc_table+TASK_FS,pathname),
			(void*)va2la(proc_table+src,m->pathname),len);
	pathname[len]='\0';
	//printf("%s\n",pathname);
	int i;
	for(i=0;i<NUM_FILES;++i)
	{
		if(proc_table[src].filp[i]==0)
		{
			fd=i;
			break;
		}
		if(i==NUM_FILES-1)
		{
			printf("flip[] is full in do_open() in process %d\n",src);
			return -1;
		}
	}
	//printf("fd: %d\n",fd);
	for(i=0;i<MAX_FS_TABLE_SIZE;++i)
	{
		if(f_desc_table[i].p_inode==0)
			break;
		if(i==MAX_FS_TABLE_SIZE-1)
		{
			printf("f_desc_table[] is full in do_open() in process %d\n",src);
			return -1;
		}
	}
	int inode_no=search_file(pathname);

	INODE* p_inode=0;

	if(inode_no)
	{
		char filename[MAX_PATH];
		INODE* dir_inode;
		if(strip_path(filename,pathname,&dir_inode))
			return -1;
		p_inode=get_inode(inode_no);
	}
	else{
		p_inode=create_file(pathname,NORMAL_INODE);
	}

	if(p_inode)
	{
		proc_table[src].filp[fd]=&f_desc_table[i];
		f_desc_table[i].p_inode=p_inode;
		f_desc_table[i].mode=m->flag;
		f_desc_table[i].pos=0;
	}
	else return -1;
	return fd;
}

PUBLIC int do_close(MESSAGE* m)
{
	int fd=m->fd;
	put_inode(proc_table[m->source].filp[fd]->p_inode);
	proc_table[m->source].filp[fd]->p_inode=0;
	proc_table[m->source].filp[fd]=0;
	return 0;
}

PRIVATE INODE* create_file(char* path,int flag)
{
	char filename[MAX_PATH];
	INODE* dir_inode;
	if(strip_path(filename,path,&dir_inode)==-1)
	{
		printf("error in create_file()\n");
		return 0;
	}
	//printf("%d\n",dir_inode->i_num);
	int inode_no=alloc_imap_bit();
	//printf("%d\n",inode_no);
	int free_sect_no=alloc_smap_bit(NUM_FILE_SECTS);
	//printf("%d\n",free_sect_no);
	INODE* p_inode=new_inode(inode_no,free_sect_no,flag);
	//printf("%d\n",p_inode->i_start_sect);
	new_dir_entry(dir_inode,inode_no,filename);
	//printf("%s\n",filename);
	if(flag==DIRECTORY_INODE)
	{
		memset(fsbuf,0,SECTOR_SIZE);
		DIR_ENTRY* pde=(DIR_ENTRY*)fsbuf;
		pde->inode=inode_no;
		//printf("cf:%d\n",inode_no);
		pde->name[0]='.';
		pde->name[1]='\0';
		pde++;
		pde->inode=dir_inode->i_num;
		//printf("cf:%d\n",dir_inode->i_num);
		pde->name[0]='.';
		pde->name[1]='.';
		pde->name[2]='\0';
		rw_sector(DEV_WRITE,free_sect_no,1);
	}
	return p_inode;
}

PRIVATE int alloc_imap_bit()
{
	int i,j,k;

	for(i=0;i<sb.num_imaps_sects;++i)
	{
		rw_sector(DEV_READ,2+i,1);
		for(j=0;j<SECTOR_SIZE;++j)
		{
			if(fsbuf[j]==0xff)
				continue;
			for(k=0;(fsbuf[j]>>k)&1==1;k++);
			fsbuf[j]|=1<<k;
			rw_sector(DEV_WRITE,2+i,1);
			return (i*SECTOR_SIZE+j)*8+k;
		}
	}
	printf("error in alloc_imap_bit()\n");
	return 0;
}

PRIVATE int alloc_smap_bit(int num)
{
	int i,j,k,u;
	int ret=0;
	for(i=0;i<sb.num_smaps_sects;++i)
	{
		rw_sector(DEV_READ,2+sb.num_imaps_sects+i,1);
		for(j=0;j<SECTOR_SIZE;++j)
		{
			if(fsbuf[j]==0xff)
				continue;
			for(k=0;(fsbuf[j]>>k)&1==1;k++)
				;
			if(ret==0)
					ret=(i*SECTOR_SIZE+j)*8+k;
			if(k==0&&num>=8)
			{
				fsbuf[j]=0xff;
				num-=8;
				continue;
			}
			for(u=k;u<8;++u)
			{
				fsbuf[j]|=1<<u;
				num--;
				if(num==0) break;
			}
			if(num==0) break;
		}
		rw_sector(DEV_WRITE,2+sb.num_imaps_sects+i,1);
		if(num==0) return ret+sb.first_sect;
	}
	printf("error in alloc_smap_bit()\n");
	return 0;
}

PRIVATE INODE* new_inode(int inode_no,int free_sect_no,int type)
{
	INODE* inode=get_inode(inode_no);
	inode->i_mode=type;
	inode->i_size=0;
	inode->i_start_sect=free_sect_no;
	inode->i_num_sects=NUM_FILE_SECTS;

	inode->i_num=inode_no;
	inode->i_cnt=1;

	if(type==DIRECTORY_INODE)
	{
		inode->i_size=DIR_ENTRY_SIZE*2;
	}
	sync_inode(inode);
	return inode;
}

PRIVATE void new_dir_entry(INODE* dir_inode,int inode_no,char* filename)
{
	int begin=dir_inode->i_start_sect;
	int num_sect=dir_inode->i_size/SECTOR_SIZE+1;
	int num_dir=dir_inode->i_size/DIR_ENTRY_SIZE;
	//printf("%s,%d,%d\n",filename,begin,num_dir);
	int i,j,k;
	DIR_ENTRY* de;
	DIR_ENTRY* newde=0;
	k=0;
	for(i=0;i<num_sect;++i)
	{
		rw_sector(DEV_READ,begin+i,1);
		de=(DIR_ENTRY*)fsbuf;
		for(j=0;j<SECTOR_SIZE/DIR_ENTRY_SIZE;++j,de++)
		{
			//printf("de->inode: %d \n",de->inode);
			if(de->inode==0)
			{
				newde=de;
				//printf("newde: %d\n",newde);
				break;
			}
			if(de->inode!=0) k++;
			if(k>num_dir+1) break;
		}
		if(newde!=0) break;
		if(k>num_dir+1) break;
	}
	if(newde==0){
		printf("error in new_dir_entry,%d\n",k);
		return;
	}
	newde->inode=inode_no;
	strcpy(newde->name,filename);
	rw_sector(DEV_WRITE,begin+i,1);
	dir_inode->i_size+=DIR_ENTRY_SIZE;
	sync_inode(dir_inode);
}

PUBLIC INODE* makedir(const char* path)
{
	INODE* pin;
	pin=create_file(path,DIRECTORY_INODE);
	return pin;
}