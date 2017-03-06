#include "all.h"

u32 keymap[NUM_SCAN_CODES * MAP_COLS] = {

/* scan-code			!Shift		Shift		E0 XX	*/
/* ==================================================================== */
/* 0x00 - none		*/	0,			0,			0,
/* 0x01 - ESC		*/	ESC,		ESC,		0,
/* 0x02 - '1'		*/	'1',		'!',		0,
/* 0x03 - '2'		*/	'2',		'@',		0,
/* 0x04 - '3'		*/	'3',		'#',		0,
/* 0x05 - '4'		*/	'4',		'$',		0,
/* 0x06 - '5'		*/	'5',		'%',		0,
/* 0x07 - '6'		*/	'6',		'^',		0,
/* 0x08 - '7'		*/	'7',		'&',		0,
/* 0x09 - '8'		*/	'8',		'*',		0,
/* 0x0A - '9'		*/	'9',		'(',		0,
/* 0x0B - '0'		*/	'0',		')',		0,
/* 0x0C - '-'		*/	'-',		'_',		0,
/* 0x0D - '='		*/	'=',		'+',		0,
/* 0x0E - BS		*/	BACKSPACE,	BACKSPACE,	0,
/* 0x0F - TAB		*/	TAB,		TAB,		0,
/* 0x10 - 'q'		*/	'q',		'Q',		0,
/* 0x11 - 'w'		*/	'w',		'W',		0,
/* 0x12 - 'e'		*/	'e',		'E',		0,
/* 0x13 - 'r'		*/	'r',		'R',		0,
/* 0x14 - 't'		*/	't',		'T',		0,
/* 0x15 - 'y'		*/	'y',		'Y',		0,
/* 0x16 - 'u'		*/	'u',		'U',		0,
/* 0x17 - 'i'		*/	'i',		'I',		0,
/* 0x18 - 'o'		*/	'o',		'O',		0,
/* 0x19 - 'p'		*/	'p',		'P',		0,
/* 0x1A - '['		*/	'[',		'{',		0,
/* 0x1B - ']'		*/	']',		'}',		0,
/* 0x1C - CR/LF		*/	ENTER,		ENTER,		PAD_ENTER,
/* 0x1D - l. Ctrl	*/	CTRL_L,		CTRL_L,		CTRL_R,
/* 0x1E - 'a'		*/	'a',		'A',		0,
/* 0x1F - 's'		*/	's',		'S',		0,
/* 0x20 - 'd'		*/	'd',		'D',		0,
/* 0x21 - 'f'		*/	'f',		'F',		0,
/* 0x22 - 'g'		*/	'g',		'G',		0,
/* 0x23 - 'h'		*/	'h',		'H',		0,
/* 0x24 - 'j'		*/	'j',		'J',		0,
/* 0x25 - 'k'		*/	'k',		'K',		0,
/* 0x26 - 'l'		*/	'l',		'L',		0,
/* 0x27 - ';'		*/	';',		':',		0,
/* 0x28 - '\''		*/	'\'',		'"',		0,
/* 0x29 - '`'		*/	'`',		'~',		0,
/* 0x2A - l. SHIFT	*/	SHIFT_L,	SHIFT_L,	0,
/* 0x2B - '\'		*/	'\\',		'|',		0,
/* 0x2C - 'z'		*/	'z',		'Z',		0,
/* 0x2D - 'x'		*/	'x',		'X',		0,
/* 0x2E - 'c'		*/	'c',		'C',		0,
/* 0x2F - 'v'		*/	'v',		'V',		0,
/* 0x30 - 'b'		*/	'b',		'B',		0,
/* 0x31 - 'n'		*/	'n',		'N',		0,
/* 0x32 - 'm'		*/	'm',		'M',		0,
/* 0x33 - ','		*/	',',		'<',		0,
/* 0x34 - '.'		*/	'.',		'>',		0,
/* 0x35 - '/'		*/	'/',		'?',		PAD_SLASH,
/* 0x36 - r. SHIFT	*/	SHIFT_R,	SHIFT_R,	0,
/* 0x37 - '*'		*/	'*',		'*',    	0,
/* 0x38 - ALT		*/	ALT_L,		ALT_L,  	ALT_R,
/* 0x39 - ' '		*/	' ',		' ',		0,
/* 0x3A - CapsLock	*/	CAPS_LOCK,	CAPS_LOCK,	0,
/* 0x3B - F1		*/	F1,			F1,			0,
/* 0x3C - F2		*/	F2,			F2,			0,
/* 0x3D - F3		*/	F3,			F3,			0,
/* 0x3E - F4		*/	F4,			F4,			0,
/* 0x3F - F5		*/	F5,			F5,			0,
/* 0x40 - F6		*/	F6,			F6,			0,
/* 0x41 - F7		*/	F7,			F7,			0,
/* 0x42 - F8		*/	F8,			F8,			0,
/* 0x43 - F9		*/	F9,			F9,			0,
/* 0x44 - F10		*/	F10,		F10,		0,
/* 0x45 - NumLock	*/	NUM_LOCK,	NUM_LOCK,	0,
/* 0x46 - ScrLock	*/	SCROLL_LOCK,SCROLL_LOCK,0,
/* 0x47 - Home		*/	PAD_HOME,	'7',		HOME,
/* 0x48 - CurUp		*/	PAD_UP,		'8',		UP,
/* 0x49 - PgUp		*/	PAD_PAGEUP,	'9',		PAGEUP,
/* 0x4A - '-'		*/	PAD_MINUS,	'-',		0,
/* 0x4B - Left		*/	PAD_LEFT,	'4',		LEFT,
/* 0x4C - MID		*/	PAD_MID,	'5',		0,
/* 0x4D - Right		*/	PAD_RIGHT,	'6',		RIGHT,
/* 0x4E - '+'		*/	PAD_PLUS,	'+',		0,
/* 0x4F - End		*/	PAD_END,	'1',		END,
/* 0x50 - Down		*/	PAD_DOWN,	'2',		DOWN,
/* 0x51 - PgDown	*/	PAD_PAGEDOWN,'3',		PAGEDOWN,
/* 0x52 - Insert	*/	PAD_INS,	'0',		INSERT,
/* 0x53 - Delete	*/	PAD_DOT,	'.',		DELETE,
/* 0x54 - Enter		*/	0,			0,			0,
/* 0x55 - ???		*/	0,			0,			0,
/* 0x56 - ???		*/	0,			0,			0,
/* 0x57 - F11		*/	F11,		F11,		0,	
/* 0x58 - F12		*/	F12,		F12,		0,	
/* 0x59 - ???		*/	0,			0,			0,	
/* 0x5A - ???		*/	0,			0,			0,	
/* 0x5B - ???		*/	0,			0,			GUI_L,	
/* 0x5C - ???		*/	0,			0,			GUI_R,	
/* 0x5D - ???		*/	0,			0,			APPS,	
/* 0x5E - ???		*/	0,			0,			0,	
/* 0x5F - ???		*/	0,			0,			0,
/* 0x60 - ???		*/	0,			0,			0,
/* 0x61 - ???		*/	0,			0,			0,	
/* 0x62 - ???		*/	0,			0,			0,	
/* 0x63 - ???		*/	0,			0,			0,	
/* 0x64 - ???		*/	0,			0,			0,	
/* 0x65 - ???		*/	0,			0,			0,	
/* 0x66 - ???		*/	0,			0,			0,	
/* 0x67 - ???		*/	0,			0,			0,	
/* 0x68 - ???		*/	0,			0,			0,	
/* 0x69 - ???		*/	0,			0,			0,	
/* 0x6A - ???		*/	0,			0,			0,	
/* 0x6B - ???		*/	0,			0,			0,	
/* 0x6C - ???		*/	0,			0,			0,	
/* 0x6D - ???		*/	0,			0,			0,	
/* 0x6E - ???		*/	0,			0,			0,	
/* 0x6F - ???		*/	0,			0,			0,	
/* 0x70 - ???		*/	0,			0,			0,	
/* 0x71 - ???		*/	0,			0,			0,	
/* 0x72 - ???		*/	0,			0,			0,	
/* 0x73 - ???		*/	0,			0,			0,	
/* 0x74 - ???		*/	0,			0,			0,	
/* 0x75 - ???		*/	0,			0,			0,	
/* 0x76 - ???		*/	0,			0,			0,	
/* 0x77 - ???		*/	0,			0,			0,	
/* 0x78 - ???		*/	0,			0,			0,	
/* 0x78 - ???		*/	0,			0,			0,	
/* 0x7A - ???		*/	0,			0,			0,	
/* 0x7B - ???		*/	0,			0,			0,	
/* 0x7C - ???		*/	0,			0,			0,	
/* 0x7D - ???		*/	0,			0,			0,	
/* 0x7E - ???		*/	0,			0,			0,
/* 0x7F - ???		*/	0,			0,			0
};

PRIVATE KB_INPUT kb;

PRIVATE int code_with_E0;
PRIVATE int shift_l;
PRIVATE int shift_r;
PRIVATE int alt_l;
PRIVATE int alt_r;
PRIVATE int ctrl_l;
PRIVATE int ctrl_r;
PRIVATE int caps_lock;
//PRIVATE int num_lock;
//PRIVATE int scroll_lock;
PRIVATE char pause_code[5]={0x1d,0x45,0xe1,0x9d,0xc5};

PUBLIC void keyboard_handler()
{
	u8 scan_code=in_byte(KB_IO_PORT);
	if(kb.count<KB_BUF_SIZE)
	{
		*(kb.p_tail)=scan_code;
		kb.p_tail++;
		if(kb.p_tail>=kb.buf+KB_BUF_SIZE)
			kb.p_tail=kb.buf;
		kb.count++;
	}
}

PUBLIC void init_keyboard()
{
	disable_int();
	kb.count=0;
	kb.p_head=kb.p_tail=kb.buf;

	code_with_E0=0;
	shift_r=shift_l=0;
	alt_r=alt_l=0;
	ctrl_l=ctrl_r=0;

	caps_lock=0;

	put_irq_handler(KEYBOARD_IRQ,keyboard_handler);
	enable_irq(KEYBOARD_IRQ);
	enable_int();
}

PRIVATE char get_byte_from_kbuf()
{
	while(kb.count<=0);

	disable_int();

	char code=*(kb.p_head);
	kb.p_head++;
	if(kb.p_head>=kb.buf+KB_BUF_SIZE)
		kb.p_head=kb.buf;
	kb.count--;

	enable_int();

	return code;
}


PUBLIC void keyboard_read(TTY* p_tty)
{
	u8 scan_code; 
	int is_make;	//1 = is make code,  0 = is break code
	u32 key=0; 		//the result we read

	if(kb.count>0)
	{	
		code_with_E0=0;
		scan_code=get_byte_from_kbuf();

		//if is Pause 
		if(scan_code==0xE1){
			int i;
			int is_pause=1;
			/* if the followed code is 0x1d,0x45,0xe1,0x9d,0xc5,
					then  we obtain key PAUSE */
			for(i=0;i<5;++i)
				if(get_byte_from_kbuf()!=pause_code[i])//pause_code[5]={0x1d,0x45,0xe1,0x9d,0xc5};
				{
					is_pause=0;
					break;
				}
			if(is_pause) key=PAUSEBREAK;
		}
		/*code like: E0,XX */
		else if(scan_code==0xE0){
			scan_code=get_byte_from_kbuf();
			/*print screen make code: E0 2A E0 37
						   break code: E0,B7,E0,AA*/
			if(scan_code==0x2a)
			{
				if(get_byte_from_kbuf()==0xe0)
					if(get_byte_from_kbuf()==0x37)
					{
						key=PRINTSCREEN;
						is_make=1;
					}
			}
			else if(scan_code==0xb7)
			{
				if(get_byte_from_kbuf()==0xe0)
					if(get_byte_from_kbuf()==0xaa)
					{
						key=PRINTSCREEN;
						is_make=0;
					}
			}
			else code_with_E0=1;

		}
		if(key!=PAUSEBREAK&&key!=PRINTSCREEN)
		{
			//if scan_code<80h,then the code is make code
			is_make=(scan_code<FLAG_BREAK)?1:0;
			int col=0;
			int caps=shift_l||shift_r;
			if(caps_lock){
				if(keymap[(scan_code&0x7f)*MAP_COLS]>='a'&&
					keymap[(scan_code&0x7f)*MAP_COLS]<='z')
					caps=1;
			}
			if(caps) col=1;
			if(code_with_E0) col=2;
			key=keymap[(scan_code&0x7f)*MAP_COLS+col];

			switch(key)
			{
				case SHIFT_L:{
					shift_l=is_make;
					break;
				}
				case SHIFT_R:{
					shift_r=is_make;
					break;
				}
				case CTRL_L:{
					ctrl_l=is_make;
					break;
				}
				case CTRL_R:{
					ctrl_r=is_make;
					break;
				}
				case ALT_L:{
					alt_l=is_make;
					break;
				}
				case ALT_R:{
					alt_r=is_make;
					break;
				}
				case CAPS_LOCK:{
					if(is_make) caps_lock=!caps_lock;
					break;
				}
				default:break;
			}

			if(is_make)
			{
				//set flags of key
				key|=shift_l?FLAG_SHIFT_L:0;//200h
				key|=shift_r?FLAG_SHIFT_R:0;//400h
				key|=ctrl_l?FLAG_CTRL_L:0;	//800h
				key|=ctrl_r?FLAG_CTRL_R:0;	//1000h
				key|=alt_l?FLAG_ALT_L:0;	//2000h
				key|=alt_r?FLAG_ALT_R:0;	//4000h
				//function in tty.c, used to deal with the key we obtained
				deal_with_key(p_tty,key);
			}
		}
	}
}