#define _LARGEFILE64_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<errno.h>
#include<linux/fs.h>
#include<sys/types.h>
#include<unistd.h>
#include<ext2fs/ext2_fs.h>


int main(int argc,char* argv[])
{
	int fd;
	int count,inode_no,l;
	int bgno,inode_index,inode_size,block_size;
	int inode_offset;
	struct ext2_super_block sb;
	//struct ext2_inode inode;
	//struct ext2_group_desc bgdesc;
	
	//inode_no = atoi(argv[2]);
	fd = open(argv[1],O_RDONLY);
	if(fd==-1)
	{
		perror("Read Error.");
		exit(EXIT_FAILURE);
	}
	lseek64(fd,1024,SEEK_CUR);//Move the pointer to the super block by skipping the boot block
	printf("size of super block = %lu\n",sizeof(struct ext2_super_block));
	count  = read(fd , &sb , sizeof(struct ext2_super_block));
	printf("Magic:- %x\n",sb.s_magic);
	printf("Inodes Count:- %d\n",sb.s_inodes_count);
	printf("Block Size Entry:-%d\n",sb.s_log_block_size);
	
	int inodes_count = sb.s_inodes_count;
	struct ext2_inode inode[inodes_count];
	
	//printf("size of BG DESC:- %lu\n",sizeof(struct ext2_group_desc));
	inode_size = sb.s_inode_size;
	block_size = 1024 << sb.s_log_block_size;
	
	
	int n_groups = sb.s_blocks_count / sb.s_blocks_per_group;//no. of group descriptors  = no. of groups
	if(sb.s_block_count % sb.s_blocks_per_group > 0)
	{
		n_groups++;
	}
	struct ext2_group_desc bgdesc[n_groups];
	bgno = (inode_no - 1) / sb.s_blocks_per_group;
	inode_index = (inode_no -1)% sb.s_blocks_per_group;
	lseek64(fd,1024 + block_size * bgno + sizeof(bgdesc),SEEK_SET);
	count = read(fd,bgdesc,sizeof(struct ext2_group_desc)*n_groups);
	for(int i=0; i<n_groups;i++)
	{
		printf("%d): Inode Table:- %d\n",i,bgdesc[i].bg_inode_table);
	}
	
	inode_offset = bgdesc->bg_inode_table * block_size + inode_index * inode_size;
	lseek64(fd,inode_offset,SEEK_SET);
	read(fd , &inode,sizeof(inode));
	printf("Size of file is:- %d\n",inode->i_size);
	printf("Number of blocks:- %d blocks.\n",inode->i_blocks);
	for (int i=0; i<inode->i_blocks;i++)
	{
		for(int j=0;j<15;j++)
		{
			printf("%d ",inode[i].i_block[j]);
		}
	}
	
	
	close(fd);
	
	return 0;
}
	
	
