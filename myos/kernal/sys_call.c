#include "all.h"

PUBLIC int sys_write(char* buf,int len,int null,PROCESS* p_proc)
{
	TTY* p_tty=&tty_table[p_proc->tty];
	while(len)
	{
		out_char(p_tty->p_console,*buf++,0xf);
		len--;
	}
}

