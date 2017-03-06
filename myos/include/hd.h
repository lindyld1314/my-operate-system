#ifndef HD_H
#define HD_H

/*
DEVICE register
7 1	
6 L	 LBA MODE
5 1	
4 DRV  0=master hd ,1=slave hd
3 HS3  \
2 HS2   | lba bit24~bit27
1 HS1   |
0 HS0  /
*/
#define MAKE_DEVICE_REG(lba,drv,lba_highest) ((lba<<6)|(drv<<4)|(lba_highest&0xf)|0xa0)

typedef struct s_hd_cmd{
	u8	features;
	u8	count;
	u8	lba_low;
	u8	lba_mid;
	u8	lba_high;
	u8	device;
	u8	command;
}HD_CMD;

//command block register
#define REG_DATA 0x1F0
#define REG_FEATURES 0x1F1
#define REG_ERROR REG_FEATURES
#define REG_SECTOR_COUNT 0x1F2
#define REG_LBA_LOW	0x1F3
#define REG_LBA_MID	0x1F4
#define REG_LBA_HIGH 0x1F5
#define REG_DEVICE 0x1F6
#define REG_STATUS 0x1F7
#define REG_COMMAND	REG_STATUS
//control block register
#define REG_DEVICE_CONTROL 0x3F6
#define REG_ALTERNATE_STATUS REG_DEVICE_CONTROL
#endif
//command
#define COMMAND_ATA_IDENTIFY 0xEC
#define COMMAND_ATA_READ 0x20
#define COMMAND_ATA_WRITE 0x30
//status register
/*
7 BSY    busy, if BSY=1,other bits are invalid
6 DRDY   drive ready
5 DF/SE  device fault/ stream fault
4 #      command dependent
3 DRQ    data request
2 -      obsolete
1 -      obsolete
0 ERR    error
*/
#define	STATUS_BSY	0x80
#define	STATUS_DRDY	0x40
#define	STATUS_DFSE	0x20
#define	STATUS_DSC	0x10
#define	STATUS_DRQ	0x08
#define	STATUS_CORR	0x04
#define	STATUS_IDX	0x02
#define	STATUS_ERR	0x01

#define SECTOR_SIZE	512
#define SECTOR_BITS	SECTOR_SIZE*8
#define SECTOR_SIZE_SHIFT 9