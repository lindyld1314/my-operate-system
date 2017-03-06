#ifndef FS_H
#define FS_H

#define MAX_FS_TABLE_SIZE 200

typedef struct file_desc{
	int mode;
	int pos;
	struct inode* p_inode;
}FILE_DESC;

typedef struct super_block{
	u32 magic_number;

	u32 inode_size;
	u32 num_inodes;
	u32 num_inodes_sects;
	u32 num_imaps_sects;

	u32 num_sects;
	u32 num_smaps_sects;

	u32 first_sect;
	u32 root_inode;

	u32 inode_isize_off;
	u32 inode_istart_off;

	u32 dir_entry_size;
	u32 dir_entry_inode_off;
	u32 dir_entry_name_off;

}SUPER_BLOCK;

#define SUPER_BLOCK_SIZE sizeof(SUPER_BLOCK)

typedef struct inode
{
	u32 i_mode;
	u32 i_size;
	u32 i_start_sect;
	u32 i_num_sects;
	u8 i_unused[16];

	u32 i_cnt;
	u32 i_num;
}INODE;

#define INODE_SIZE (sizeof(INODE)-8)

typedef struct dir_entry 
{
	int inode;
	char name[12];
}DIR_ENTRY;

#define DIR_ENTRY_SIZE sizeof(DIR_ENTRY)

//inode
#define	INVALID_INODE 0
#define	ROOT_INODE 1

#define NUM_FILE_SECTS 2048 //1MB

//mode of inode
#define DIRECTORY_INODE 1
#define NORMAL_INODE 2
#define SPECIAL_INODE 3

//install
#define INSTALL_START_SECT 0x8000
#define INSTALL_NUM_SECTS 0x800

#define MAX_PATH 100

#endif