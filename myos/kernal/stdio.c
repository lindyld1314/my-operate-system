#include "all.h"

PUBLIC int printf(const char *format,...){
	int i;
	char buf[256];
	char* args=(char*)((char*)(&format)+4);

	char* p;
	for(p=buf;*format;format++)
	{
		if(*format!='%'){
			*p++=*format;
		}
		else{
			format++;
			switch(*format)
			{
				case 'x':{
					itoa(p,*((int*)args),16);
					args+=4;
					p+=strlen(p);
					break;
				}
				case 'd':{
					itoa(p,*((int*)args),10);
					args+=4;
					p+=strlen(p);
					break;
				}
				case 's':{
					strcpy(p,*((int*)args));
					args+=4;
					p+=strlen(p);
					break;
				}
				case 'c':{
					*p=(char)(*args);
					args+=4;
					p++;
					break;
				}
				default:break;
			}
		}

	}
	write2vga(buf,p-buf);
}

PUBLIC int cin_getline(char* str)
{
	TTY *p_tty=&tty_table[p_proc_ready->tty];
	while(allow_cin!=1);
	//begin to input
	allow_cin=0;
	p_tty->cin_cnt=0;
	p_tty->is_cin=1;
	while(p_tty->is_cin==1);
	int i=0;
	for(i=0;i<p_tty->cin_cnt;++i)
	{
		str[i]=p_tty->cin_buf[i];
	}
	str[i]='\0';
	p_tty->cin_cnt=0;
	allow_cin=1;
}