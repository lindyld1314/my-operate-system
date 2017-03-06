#include "all.h"

PRIVATE void set_cursor(u32 pos)
{
	disable_int();
	out_byte(CRTC_ADDR_REG,CURSOR_H);
	out_byte(CRTC_DATA_REG,(pos>>8)&0xff);
	out_byte(CRTC_ADDR_REG,CURSOR_L);
	out_byte(CRTC_DATA_REG,pos&0xff);
	enable_int();
}

PRIVATE void set_start_addr(u32 addr)
{
	disable_int();
	out_byte(CRTC_ADDR_REG,START_ADDR_H);
	out_byte(CRTC_DATA_REG,(addr>>8)&0xff);
	out_byte(CRTC_ADDR_REG,START_ADDR_L);
	out_byte(CRTC_DATA_REG,addr&0xff);
	enable_int();
}

PUBLIC void show_save_char(TTY* p_tty,char ch)
{
	CONSOLE* p_con=p_tty->p_console;
	u8* p_v_mem=(u8*)(VIDEO_MEMORY_BASE+p_con->cursor*2);
	
	switch(ch)
	{
		case '\n':{
			if(p_con->cursor+SCREEN_WIDTH<p_con->current_start_addr+p_con->v_mem_size)
				p_con->cursor=((p_con->cursor-p_con->start_addr)/SCREEN_WIDTH+1)*SCREEN_WIDTH+p_con->start_addr;
			p_tty->is_cin=0;
			break;
		}
		case '\b':{
			if(p_tty->cin_cnt>0)
			{
				p_con->cursor--;
				*(p_v_mem-2)=' ';
				*(p_v_mem-1)=0xe;
				 p_tty->cin_cnt--;
			}		
			break;
		}
		default:{
			if(p_con->cursor<p_con->start_addr+p_con->v_mem_size-1)
			{
				*p_v_mem++=ch;
				*p_v_mem=0xe;
				p_con->cursor++;
				p_tty->cin_buf[p_tty->cin_cnt]=ch;
				p_tty->cin_cnt++;
			}
			break;
		}
	}

	while(p_con->cursor>p_con->current_start_addr+SCREEN_SIZE)
		scroll_screen(p_con,DOWN);
	set_cursor(p_con->cursor);
}

PUBLIC void out_char(CONSOLE* p_console,char ch,char color)
{
	u8* p_v_mem=(u8*)(VIDEO_MEMORY_BASE+p_console->cursor*2);
	
	switch(ch)
	{
		case '\n':{
			if(p_console->cursor+SCREEN_WIDTH<p_console->start_addr+p_console->v_mem_size)
				p_console->cursor=((p_console->cursor-p_console->start_addr)/SCREEN_WIDTH+1)*SCREEN_WIDTH+p_console->start_addr;
			break;
		}
		case '\b':{
			p_console->cursor--;
			*(p_v_mem-2)=' ';
			*(p_v_mem-1)=color;
			break;
		}
		default:{
			if(p_console->cursor<p_console->start_addr+p_console->v_mem_size-1)
			{
				*p_v_mem++=ch;
				*p_v_mem=color;
				p_console->cursor++;
			}
			break;
		}
	}

	if(p_console->cursor+SCREEN_WIDTH>p_console->start_addr+p_console->v_mem_size)
	{
		clear_screen(p_console);
		p_console->current_start_addr=p_console->start_addr;
		p_console->cursor=p_console->start_addr+2;
		if(p_console==console_table+current_console)
			set_start_addr(p_console->current_start_addr);
		printf("Clear screen successfully!\n");
		return;
	}

	while(p_console->cursor>p_console->current_start_addr+SCREEN_SIZE)
		scroll_screen(p_console,DOWN);
	set_cursor(p_console->cursor);
}

PUBLIC void clear_screen(CONSOLE* p_console)
{
	u32 begin=p_console->start_addr+2;
	u32 end=p_console->start_addr+p_console->v_mem_size;
	u8* p_clear=(u8*)(VIDEO_MEMORY_BASE+begin*2);
	for(;p_clear<(u8*)(VIDEO_MEMORY_BASE+end*2);)
	{
		*p_clear++=' ';
		*p_clear++=0xf;
	}
}

PUBLIC void init_screen(TTY* p_tty)
{
	int cur_tty=p_tty-tty_table;
	p_tty->p_console=console_table+cur_tty;

	int v_mem_size=VIDEO_MEMORY_SIZE>>1; //size in word 

	int size=v_mem_size/NUM_CONSOLES;
	p_tty->p_console->start_addr=cur_tty*size;
	p_tty->p_console->current_start_addr=p_tty->p_console->start_addr;
	p_tty->p_console->v_mem_size=size;
	p_tty->p_console->cursor=p_tty->p_console->start_addr;

	//clear screen
	clear_screen(p_tty->p_console);

	if(cur_tty==0)
	{
		p_tty->p_console->cursor=disp_pos/2;
		disp_pos=0;
		out_char(p_tty->p_console,cur_tty+'1',0x12);
		out_char(p_tty->p_console,'#',0x12);
	}
	else{
		out_char(p_tty->p_console,cur_tty+'1',0x12);
		out_char(p_tty->p_console,'#',0x12);
	}
	set_cursor(p_tty->p_console->cursor);
}

PUBLIC void select_console(int console_index)
{
	if(console_index==0&&allow_cin==0) return;
	if(current_console==0&&tty_table[0].is_cin==1) return;
	if(console_index<0||console_index>=NUM_CONSOLES) return;
	//global variable----current_console
	current_console=console_index;
	set_cursor(console_table[console_index].cursor);
	set_start_addr(console_table[console_index].current_start_addr);
	if(current_console==0) enable_cin_cmd=1;
	else enable_cin_cmd=0;
}

PUBLIC void scroll_screen(CONSOLE* p_console,int dir)
{
	if(dir==UP)
	{
		if(p_console->current_start_addr>p_console->start_addr)
		{
			p_console->current_start_addr-=SCREEN_WIDTH;
		}
	}
	else if(dir==DOWN)
	{	
		if(p_console->current_start_addr+SCREEN_SIZE<p_console->start_addr+p_console->v_mem_size)
		{
			p_console->current_start_addr+=SCREEN_WIDTH;
		}
	}
	if(console_table+current_console==p_console)
	{
		set_start_addr(p_console->current_start_addr);
		set_cursor(p_console->cursor);
	}
}