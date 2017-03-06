#include "all.h"

PUBLIC int get_ticks()
{
	MESSAGE msg;
	memset(&msg,0,sizeof(MESSAGE));
	msg.type=GET_TICKS;
	send_recv(BOTH,TASK_SYS,&msg);
	return msg.ret;
}

PUBLIC int strlen(const char* str)
{
	int len=0;
	while(*(str+len)!='\0') len++;
	return len;
}

//0 = same
PUBLIC int strcmp(const char* s1,const char* s2,int len)
{
	int i;
	for(i=0;i<len;++i)
	{
		if(s1[i]=='\0'&&s2[i]=='\0')
			return 0;
		if(s1[i]==s2[i])
			continue;
		else return 1;
	}
	return 0;
}

PUBLIC char * itoa(char* str,int num,int base)
{
	char* p = str;
	char ch;
	int	i;
	if(base==16)
	{
		int	flag = 0;
		*p++ = '0';
		*p++ = 'x';
		if(num == 0) *p++ = '0';
		else{	
			for(i=28;i>=0;i-=4){
				ch = (num >> i) & 0xF;
				if(flag || (ch > 0)){
					flag = 1;
					ch += '0';
					if(ch > '9'){
						ch += 7;
					}
					*p++ = ch;
				}
			}
		}
		*p = 0;
	}
	else if(base==10)
	{
		if(num<0){
			*p++='-';
			num=-num;
		}
		if(num==0) *p++='0';
		else{
			char tmp[10];
			i=0;
			while(num>0)
			{
				tmp[i++]=num%10+'0';
				num/=10;
			}
			int j=0;
			for(j=0;j<i;++j) *p++=tmp[i-j-1];
		}
		*p=0;
	}
	return str;
}

PUBLIC void disp_int(int input)
{
	char output[16];
	itoa(output, input,16);
	disp_str(output);
}
