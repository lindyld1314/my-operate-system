#include "all.h"


PUBLIC void init_tty(TTY* p_tty)
{
	p_tty->count=0;
	p_tty->p_head=p_tty->buf;
	p_tty->p_tail=p_tty->buf;

	p_tty->is_cin=0;
	p_tty->cin_cnt=0;
	
	init_screen(p_tty);
}

PRIVATE void add_key(TTY* p_tty,u32 key)
{
	if(p_tty->count<TTY_BUF_SIZE)
		{
			*(p_tty->p_tail)=key;
			p_tty->p_tail++;
			if(p_tty->p_tail>=p_tty->buf+TTY_BUF_SIZE)
				p_tty->p_tail=p_tty->buf;
			p_tty->count++;
		}
}

PRIVATE void tty_read(TTY* p_tty){
	if(p_tty->p_console==&console_table[current_console])
	{
		keyboard_read(p_tty);
	}
}

PRIVATE void tty_write(TTY* p_tty)
{
	if(p_tty->count>0)
	{
		u8 ch=*(p_tty->p_head);
		p_tty->p_head++;
		if(p_tty->p_head>=p_tty->buf+TTY_BUF_SIZE)
			p_tty->p_head=p_tty->buf;
		p_tty->count--;
		if(p_tty->is_cin==1) show_save_char(p_tty,ch);
	}
}

PUBLIC void deal_with_key(TTY* p_tty,u32 key)
{
	//if key can be printed
	if(!(key&FLAG_EXT))
	{
		if(key==FLAG_SHIFT_L||key==FLAG_SHIFT_R
			||key==FLAG_CTRL_L||key==FLAG_CTRL_R
			||key==FLAG_ALT_L||key==FLAG_ALT_R);
		else add_key(p_tty,key);
	}
	else{
		int raw_key=key&MASK_RAW;
		switch(raw_key)
		{
			case ENTER:{
				add_key(p_tty,'\n');
				break;
			}
			case BACKSPACE:{
				add_key(p_tty,'\b');
				break;
			}
			case ESC:{
				if(enable_cin_cmd==0&&current_console==0)
					enable_cin_cmd=1;
			}
			case F1:
			case F2:
			case F3:
			case F4:
			case F5:
			case F6:
			case F7:
			case F8:
			case F9:
			case F10:
			case F11:
			case F12:{
				if((key&FLAG_ALT_L)||(key&FLAG_ALT_R))
					select_console(raw_key-F1);
				break;
			}
			case UP:{
				if((key&FLAG_SHIFT_L)||(key&FLAG_SHIFT_R))
					scroll_screen(p_tty->p_console,UP);
				break;
			}
			case DOWN:{
				if((key&FLAG_SHIFT_L)||(key&FLAG_SHIFT_R))
					scroll_screen(p_tty->p_console,DOWN);
				break;
			}
		}
	}
}


PUBLIC void task_tty()
{
	TTY* p_tty;
	init_keyboard();
	for(p_tty=tty_table;p_tty<tty_table+NUM_CONSOLES;++p_tty)
	{
		init_tty(p_tty);
	}
	select_console(0);
	while(1)
	{
		for(p_tty=tty_table;p_tty<tty_table+NUM_CONSOLES;++p_tty)
		{
			if(p_tty->is_cin==1&&p_tty->p_console!=console_table) select_console(p_tty->p_console-console_table);
			tty_read(p_tty);
			tty_write(p_tty);	
		}
	}
}