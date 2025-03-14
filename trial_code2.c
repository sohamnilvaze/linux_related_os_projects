#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<ext2fs/ext2_fs.h>

#define EXT2_SUPER_MAGIC 0xEF53
#define BASE_OFFSET 1024
#define DEFAULT_INODE_SIZE 128

void usage(char* progname)
{
	fprintf(stderr , "Usage: %s <device-file>\n",progname);
	exit(1);
}

int main(int argc, char* argv[])
{
	if(argc!=2)
	{
		usage(argv[0]);
	}
	
	int fd = open(argv[1],O_RDONLY);
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}
	
	struct ext2_super_block sb;
	if(lseek(fd,BASE_OFFSET,SEEK_SET) < 0 || read(fd,&sb,sizeof(sb)!=sizeof(sb)))
	{
		perror("read superblock");
		exit(1);
	}
	printf("Superblock Information:-\n");
	printf("Inode count:-%u.\n", sb.s_inodes_count);
	printf("Block count:-%u.\n",sb.s_blocks_count);
	uint32_t block_size = 1024 <<  sb.s_log_block_size;
	uint16_t inode_size = sb.s_inode_size;
	off_t gd_offset=2048;
	
	struct ext2_group_desc bgdesc;
	if(lseek(fd,gd_offset,SEEK_SET) < 0 || read(fd,&bgdesc,sizeof(bgdesc)!=sizeof(bgdesc)))
	{
		perror("read group descriptor.\n");
		exit(1);
	}
	
	printf("Group Descriptor Table Information:-\n");
	printf("Free Inode Count:-%u \n",bgdesc.bg_free_inodes_count);
	printf("Free Blocks Count:-%u \n",bgdesc.bg_free_blocks_count);
	printf("Inode table:- %u\n",bgdesc.bg_inode_table);
	
	off_t inode_table_offset =1024 +  bgdesc.bg_inode_table * block_size;
	struct ext2_inode inode;
	printf("Inodes information.\n");
	for(int i=0;i<=sb.s_inodes_count;i++)
	{
		off_t inode_offset = inode_table_offset + (i-1)*inode_size;
		if(lseek(fd,inode_offset,SEEK_SET) < 0 || read(fd,&inode,sizeof(inode)!=sizeof(inode)))
		{
			perror("Read inode.");
			exit(1);
		}
		if(inode.i_mode == 0)
		{
			continue;
		}
		printf("Inode no:-%d\n",i);
		printf("Mode:- 0x%x\n",inode.i_mode);
		printf("User ID:- %u",inode.i_uid);
		printf("Size:- %u.\n",inode.i_size);
		printf("Blocks:- %u.\n",inode.i_blocks);
	}
	
	
		
		
	
	close(fd);
	return 0;
}
	
	
	
	 
	
