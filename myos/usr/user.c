#include "all.h"
void ulib_delay(int);

/*======================================================================*
                               user1
 *======================================================================*/

void User1()
{
	int sem;
	sem=get_sem(0);
	if(sem==-1)
	{
		printf("can not get a semaphore!\nloop forever!\n");
		while(1);
	}

	int fruit_disk;
	char word[100];
	char wish[80]="Father will live one year after anther for ever!";

	if(fork())
	{
		while(1)
		{
			ulib_delay(10);
			P(sem);
			P(sem);
			printf("I get my big son's wish: %s\n",word);
			printf("I get my little son's fruit: %d\n",fruit_disk);
			fruit_disk=0;
		}
	}
	else if(fork())
	{
		int begin=0,end=0;
		int len=48;
		int i;
		while(1)
		{
			ulib_delay(5);
			for(i=begin;i<len;++i)
			{
				if(wish[i]!=' ')
					word[end++]=wish[i];
				else break;
			}
			if(end<=len)
			{
				word[end++]=' ';
			}
			word[end]='\0';
			begin=i+1;
			printf("   big son give wish:  %s\n",word);
			V(sem);
		}
	}
	else
	{
		int fruit=1;
		while(1)
		{
			ulib_delay(5);
			printf("   little son give fruit <%d> ^^\n",fruit);
			fruit_disk=fruit;
			fruit++;
			V(sem);
		}	
	}
}

/*======================================================================*
                               user2
 *======================================================================*/
//IPC problem reader-writer, reader first
void User2()
{
	int reader_delay1=5;
	int reader_delay2=5;
	int writer_delay1=8;
	int writer_delay2=10;

	int i,pid;
	char database[21];
	database[20]='\0';
	for(i=0;i<20;++i) database[i]='A';

	int mutex=get_sem(1);
	int db=get_sem(1);
	int rc=0;
	if(db==-1)
	{
		printf("can not get a semaphore!\nloop forever!\n");
		while(1);
	}

	pid=fork();
	if(pid==-1)
	{
		printf("fork() error!\nloop forever!\n");
		while(1);
	}
	//reader
	if(pid)
	{
		//while(1);
		int reader1;
		reader1=fork();
		if(reader1==-1)
		{
			printf("fork() error!\nloop forever!\n");
			while(1);
		}
		if(reader1)
		{
			int reader3;
			reader3=fork();
			if(reader3==-1)
			{
				printf("fork() error!\nloop forever!\n");
				while(1);
			}
			//reader1
			if(reader3)
			{
				//while(1);
				while(1)
				{
					ulib_delay(reader_delay1);
					P(mutex);
					rc++;
					if(rc==1) P(db);
					V(mutex);
					ulib_delay(reader_delay2);
					printf("reader1:  %s\n",database);
					P(mutex);
					rc--;
					if(rc==0) V(db);
					V(mutex);
				}
			}
			//reader3
			else
			{
				//while(1);
				while(1)
				{
					ulib_delay(reader_delay1);
					P(mutex);
					rc++;
					if(rc==1) P(db);
					V(mutex);
					ulib_delay(reader_delay2);
					printf("reader3:  %s\n",database);
					P(mutex);
					rc--;
					if(rc==0) V(db);
					V(mutex);
				}
			}
			
		}
		//reader2   //80,100
		else
		{
			//while(1);
			while(1)
			{
				ulib_delay(reader_delay1);
				P(mutex);
				rc++;
				if(rc==1) P(db);
				V(mutex);
				ulib_delay(reader_delay2);
				printf("reader2:  %s\n",database);
				P(mutex);
				rc--;
				if(rc==0) V(db);
				V(mutex);
			}
		}
	}
	//writer
	else{
		//while(1);
		int writer1;
		writer1=fork();
		if(writer1==-1)
		{
			printf("fork() error!\nloop forever!\n");
			while(1);
		}
		//writer1
		if(writer1)
		{
			//while(1);
			int pos=0;
			char put='B';
			while(1)
			{
				ulib_delay(writer_delay1);
				P(db);
				//change the data
				ulib_delay(writer_delay2);
				if(pos>=20)
				{
					put++;
					pos=0;
				}
				database[pos++]=put;
				V(db);
			}
		}
		//writer2
		else
		{
			//while(1);
			int pos=19;
			char put='Z';
			while(1)
			{
				ulib_delay(writer_delay1);
				P(db);
				//change the data
				ulib_delay(writer_delay2);
				if(pos<0)
				{
					put--;
					pos=19;
				}
				database[pos--]=put;
				V(db);
			}
		}
		
	}
}

/*======================================================================*
                               user3
 *======================================================================*/
void User3()
{
	printf("USER3 BEGIN! then we will test fork(),wait(),exit() ^_^ \n");
	int pid,pid2;
	int ans;
	char ch;
	pid=fork();
	if(pid==-1){
		printf("Error in fork() in the father_process!\n");
		exit('E');
	}
	if(pid)
	{
		printf("I am in the father_process,begin to wait()!\n");
		ch=wait();
		printf("wait() end! the return value is %c\n",ch);
		while(1){
			printf("A..");
			ulib_delay(12);
		}
	}
	else
	{
		printf("      I am in the sub_process\n");
		printf("      Now I will fork() again in the sub_process\n");
		pid2=fork();
		if(pid2==-1)
		{
			printf("      Error in fork() in the sub_process\n");
			exit('A');
		}
		if(pid2)
		{
			printf("      I am in the sub_process,begin to wait()\n");
			ans=wait();
			printf("      wait() end! the return value is %d\n",ans);
		}
		else{
			printf("            I am in the sub_process of the sub_process\n");
			printf("            exit('20') will be executed right now.\n");
			exit(20);
		}

		printf("      exit('A') will be executed right now.\n");
		exit('A');
	}
}

/*======================================================================*
                               user4
 *======================================================================*/
void User4()
{
	int i=0;
	printf("This is USER4!\n");
	ulib_delay(15);
	char buf[]="qwertyuiopasdfghjklzxcvbnm";
	char ans[20];

	printf("create file: /test.c\n");
	int fd=open("/test.c");
	printf("create succeed,fd: %d\n",fd);
	printf("write \"qwertyuiop\" to file\n");
	write(fd,buf,10);
	close(fd);
	printf("write succeed!close file\n\n");

	printf("open file: /test.c\n");
	fd=open("/test.c");
	printf("open succeed,fd: %d\n",fd);
	printf("read file\n");
	read(fd,ans,10);
	ans[10]='\0';
	printf("result of read: %s\n",ans);
	close(fd);
	printf("read succeed!close file\n\n");

	// printf("delete /test.c\n");
	// unlink("/test.c");
	// printf("haha,delete success!!!\n\n");

	while(1){
		i++;
		printf("E.");
		ulib_delay(12);
	}
}

/*======================================================================*
                               user5
 *======================================================================*/
void User5()
{
	int i=0;
	printf("This is USER5!\n\n");
	ulib_delay(5);
	while(1){
		i++;
		printf("D.");
		ulib_delay(12);
		printf("ticks: %d\n",get_ticks());
	}
}

/*======================================================================*
                               user6
 *======================================================================*/
void User6()
{
	int i=0;
	printf("This is USER6!\n\n");
	ulib_delay(5);
	while(1){
		i++;
		printf("F.");
		ulib_delay(5);
	}
}
