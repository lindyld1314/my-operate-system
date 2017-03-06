#ifndef	TYPE_H_
#define	TYPE_H_

typedef	unsigned long long	u64;
/*32 bits,4 bytes*/
typedef	unsigned int		u32;
/*16 bits,2 bytes*/
typedef	unsigned short		u16;
/*8 bits, 1 byte*/
typedef	unsigned char		u8;

typedef	void (*int_handler)	();
typedef void (*task_f) ();
typedef void (*user_f) ();
typedef void (*irq_handler) (int);
typedef void* system_call;
#endif