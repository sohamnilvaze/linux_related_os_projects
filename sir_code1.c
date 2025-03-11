#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<errno.h>
#include<linux/fs.h>
#include<sys/types.h>
#include<unistd.h>
#include<ext2fs/ext2_fs.h>
#define _LARGEFILE64_SOURCE

int main(int argc, char* argv[])
{
	int fd;
	int counts, ngroups,l;
	struct ext2_super_block sb;
	struct ext2_group_desc bgdesc[1024];
	
	fd = open(arv[1],O_RDONLY);
	if(fd==-1)
	{
		error("Read Error.");
		exit(errno);
	}
	lseek(fd,1024,SEEK_CUR);//Move the pointer to the super block by skipping the boot block
	printf("size of super block = %lu\n",sizeof(struct ext2_super_block);
	count  = read(fd , &sb , sizeof(struct ext2_super_block));
	printf("Magic:- %x\n",sb.s_magic);
	printf("Inodes Count:- %d\n",sb.s_inodes_count);
	printf("Block Size Entry:-%d\n",sb.s_log_block_size);
	
	printf("size of BG DESC:- %lu\n",sizeof(struct ext2_group_desc));
	
	n_groups = sb.s_block_count / sb.s_blocks_per_group;//no. of group descriptors  = no. of groups
	if(sb.s_block_count % sb.s_blocks_per_group > 0)
	{
		n_groups++;
	}
	lseek(fd,1024 + 1024 << sb.s_log_block_size,SEEK_SET);
	count = read(fd,bgdesc,sizeof(struct ext2_group_desc)*n_groups);
	for(i=0; i<n_groups;i++)
	{
		printf("%d): Inode Table:- %d\n",i,bgdesc[i].bg_inode_table);
	}
	close(fd);
	
	
	return 0;
}
