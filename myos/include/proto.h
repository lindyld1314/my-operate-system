#ifndef PROTO_H
#define PROTO_H 

/*kliba.asm*/
PUBLIC void	out_byte(u16 ,u8);
PUBLIC u8	in_byte(u16 );
PUBLIC void	disp_str(char*);
PUBLIC void	disp_color_str(char*, int);
PUBLIC void disable_int();
PUBLIC void enable_int();
PUBLIC void port_read(u16,void*,int);
PUBLIC void port_write(u16,void*,int);

/*string.h*/
PUBLIC void* memcpy(void* p_dst,void* p_src,int size);
PUBLIC void memset(void* p_dst, char ch, int size);

/*protect.c*/
PUBLIC void	init_prot();
PUBLIC u32 seg2phys(u16 seg);

/*i8259*/
PUBLIC void	init_8259();
PUBLIC void put_irq_handler(int,irq_handler);
PUBLIC void spurious_irq(int);

/*klib.c*/
//PUBLIC void delay(int);
//PUBLIC void clear_screen();
PUBLIC char* itoa(char*,int,int);
PUBLIC void disp_int(int);
PUBLIC int strlen(const char*);
PUBLIC int strcmp(const char*,const char*,int);

/* kernal.asm */
PUBLIC void restart();
PUBLIC void sys_call();

/* user.c */
void User1();
void User2();
void User3();
void User4();
void User5();
void User6();

/* clock.c */
PUBLIC void clock_handler();
PUBLIC void init_clock();

/*sys_call.c*/
PUBLIC int sys_write(char*,int,int,PROCESS*);


/*syscall.asm*/
PUBLIC void write2vga(char*,int);//0
PUBLIC int fork();
PUBLIC int sys_wait();
PUBLIC void exit(int);
PUBLIC int get_sem(int);
PUBLIC void free_sem(int);
PUBLIC void P(int);
PUBLIC void V(int);

/*process.c*/
PUBLIC void init_proc_ipc(PROCESS*);
PUBLIC void block(PROCESS*);
PUBLIC void blocked();
PUBLIC void wakeup(PROCESS*);
PUBLIC int wait();
PUBLIC int do_fork();
PUBLIC void do_wait();
PUBLIC void do_exit(int);

/*keyboard.c*/
PUBLIC void keyboard_handler();
PUBLIC void init_keyboard();
PUBLIC void keyboard_read(TTY*);

/*tty.c*/
PUBLIC void task_tty();
PUBLIC void deal_with_key(TTY*,u32);
PUBLIC void init_tty(TTY*);

/*console.c*/
PUBLIC void show_save_char(TTY*,char);
PUBLIC void out_char(CONSOLE*,char,char);
PUBLIC void init_screen(TTY*);
PUBLIC void select_console(int);
PUBLIC void scroll_screen(CONSOLE*,int);
PUBLIC void clear_screen(CONSOLE*);

/*stdio.c*/
PUBLIC int cin_getline(char*);
PUBLIC int printf(const char *,...);

/*shell.c*/
PUBLIC void task_shell();
PUBLIC void cin_cmd();

/*semaphore.c*/
PUBLIC void initial_sem();
PUBLIC int sys_get_sem(int);
PUBLIC void sys_free_sem(int);
PUBLIC void sem_sleep(int);
PUBLIC void sem_wakeup(int);
PUBLIC void sys_sem_down(int);
PUBLIC void sys_sem_up(int);

/*ipc.c*/
PUBLIC int proc_seg_linear(PROCESS* p_proc,int index);
PUBLIC void* va2la(PROCESS* p_proc,void* va);
PUBLIC void reset_msg(MESSAGE* m);
PUBLIC int sys_sendrec(int func,int src_dst,MESSAGE* m,PROCESS* p_proc);
PUBLIC void inform_int(int task);
PUBLIC void task_sys();
PUBLIC int send_recv(int func,int src_dst,MESSAGE* m);

/*hd.c*/
PUBLIC void task_hd();
PUBLIC void hd_handler(int irq);


/*fs.c*/
PUBLIC void task_fs();
PUBLIC int rw_sector(int,int,int);

PUBLIC int open(const char*);
PUBLIC int close(int);
PUBLIC int do_open(MESSAGE*);
PUBLIC int do_close(MESSAGE*);
PUBLIC INODE* get_dir_inode(const char* path);
PUBLIC int strip_path(char* filename,const char* path,INODE** ppinode);
PUBLIC int search_file(char* path);
PUBLIC INODE* get_inode(int num);
PUBLIC void put_inode(INODE* p);
PUBLIC void sync_inode(INODE* p);

PUBLIC int do_rdwt(MESSAGE*);
PUBLIC int read(int fd,char* buf,int cnt);
PUBLIC int write(int fd,char* buf,int cnt);

PUBLIC int unlink(const char* path);
PUBLIC int do_unlink(MESSAGE* m);

PUBLIC void show_dir(INODE* dir_inode);
PUBLIC INODE* makedir(const char* path);
PUBLIC INODE* deal_path(INODE* dir,char* path,int type);
PUBLIC int check_path(char* path);
#endif