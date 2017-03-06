#ifndef CONSOLE_H
#define CONSOLE_H

#define CRTC_ADDR_REG 0x3d4
#define CRTC_DATA_REG 0x3d5
#define CURSOR_H 0xe
#define CURSOR_L 0xf
#define START_ADDR_H 0xc
#define START_ADDR_L 0xd

#define SCREEN_WIDTH 80
#define SCREEN_SIZE 25*80

typedef struct s_console
{
	//used to scroll
	u32 current_start_addr;

	u32 start_addr;
	u32 v_mem_size;
	u32 cursor;
}CONSOLE;

#endif