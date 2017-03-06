#include "all.h"

/*
	ICW1: 00010001
		bit 0: 1=need OCW1
		bit 1: 0=have slave 8259
		bit 2: 0=8 bytes interrupt vector
		bit 3: 0=edge triggered
		bit 4: 1=ICW1
		bit 5-7:must be 0

	ICW2:  
		bit 0-2: 0
		bit 3-7: int_vector

	ICW3 Master: 00000100
		bit i: 1=IR i has slave
	
	ICW3 Slave: 00000 010
		
	ICW4: 00000001

	OCW1:
		bit i: 0=IR i is ipen

*/

PUBLIC void init_8259()
{
	/*Master ICW1*/
	out_byte(INT_M_20H,0x11);
	/*Master ICW2*/
	out_byte(INT_M_21H,INT_VECTOR_IRQ0);
	/*Master ICW3*/
	out_byte(INT_M_21H,0x4);
	/*Master ICW4*/
	out_byte(INT_M_21H,0x1);
	/*Master OCW1*/
	out_byte(INT_M_21H,0XFF);

	/*Slave ICW1*/
	out_byte(INT_S_A0H,0x11);
	/*Slave ICW2*/
	out_byte(INT_S_A1H,INT_VECTOR_IRQ8);
	/*Slave ICW3*/
	out_byte(INT_S_A1H,0x2);
	/*Slave ICW4*/
	out_byte(INT_S_A1H,0x1);
	/*Slave OCW1*/
	out_byte(INT_S_A1H,0XFF);

	int i;
	for(i=0;i<NUM_IRQ;++i)
		irq_table[i]=spurious_irq;

}

PUBLIC void spurious_irq(int irq)
{
    disp_str("spurious_irq: ");
    disp_int(irq);
    disp_str("\n");
}

/*set irq handler, and close it*/
PUBLIC void put_irq_handler(int irq,irq_handler handler)
{
	disable_irq(irq);
	irq_table[irq]=handler;
}