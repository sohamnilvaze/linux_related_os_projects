#define _LARGEFILE64_SOURCE
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

void usage(char *progname) {
    fprintf(stderr, "Usage: %s <device-file> <inode-number>\n", progname);
    exit(1);
}

int main(int argc, char* argv[])
{
	int fd;
    ssize_t n;
    struct ext2_super_block sb;
    struct ext2_group_desc gd;
    struct ext2_inode inode;
    int inode_num;
    int inode_size;
    uint32_t block_size,inodes_per_group,blocks_per_group;
    off_t inode_table_offset;
    off_t inode_offset; 
	
	if(argc!=3)
	{
		usage(argv[0]);
	}
	
	char* dev_file = argv[1];
	char* inode_str = argv[2];
	inode_num = atoi(inode_str);
	if(inode_num<0)
	{
		fprintf(stderr,"Invalid inode number");
		exit(1);
	}
	
	printf("Reading the device file.\n");
	fd = open(dev_file,O_RDONLY);
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}
	
	printf("Reading the superblock.\n");
	if(lseek(fd,BASE_OFFSET,SEEK_SET) < 0)
	{
		perror("lseek superblock.");
		exit(1);
	}
	int n_sb = read(fd,&sb,sizeof(sb));
	if(n_sb!=sizeof(sb))
	{
		perror("read superblock.");
		exit(1);
	}
	printf("Superblock magic number:- %u.\n",sb.s_magic);
	if(sb.s_magic!=EXT2_SUPER_MAGIC)
	{
		fprintf(stderr,"Not an ext2 filesystem.\n");
		exit(1);
	}
	
	inodes_per_group = sb.s_inodes_per_group;
	blocks_per_group = sb.s_blocks_per_group;
	block_size = 1024 << sb.s_log_block_size;
	inode_size = sb.s_inode_size;
	
	off_t gdt_offset = 2 * block_size;
	
	int group_no = (inode_num - 1) / inodes_per_group;
	int inode_no = (inode_num - 1) % inodes_per_group;
	if(lseek64(fd,gdt_offset + block_size*group_no,SEEK_SET)<0)
	{
		perror("lseek group descriptor.");
		exit(1);
	}
	n=read(fd,&gd,sizeof(gd));
	if(n!=sizeof(gd))
	{
		perror("read group descriptor");
		exit(1);
	}
	
	
	inode_table_offset = gd.bg_inode_table * block_size;
	inode_offset = inode_table_offset + (inode_num-1)*inode_size; 		
	
	printf("Reading the inode.\n");
	lseek64(fd,inode_offset,SEEK_SET);
	int n_inode = read(fd,&inode,sizeof(inode));
	if(n_inode!=sizeof(inode))
	{
		perror("read inode.");
		exit(1);
	}
	printf("Inode details:-\n");
	printf("Inode Mode:- %u.\n",inode.i_mode);
	printf("Inode size:- %u.\n",inode.i_size);
	printf("Inode group id:- %u.\n",inode.i_gid);
	printf("Inode Block Count:- %u.\n",inode.i_blocks);
	for(int i=0; i<inode.i_blocks;i++)
	{
		printf("Inode Block %d:- %u.\n",i,inode.i_block[i]);
	}
	
	
	/**int fd = open(argv[1],O_RDONLY);
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}
	//Accessing the superblock
	printf("Accessing the superblock.\n");
	struct ext2_super_block sb;
	if(lseek(fd,BASE_OFFSET,SEEK_SET) < 0 || read(fd,&sb,sizeof(sb)!=sizeof(sb)))
	{
		perror("read superblock");
		exit(1);
	}
	read(fd,&sb,sizeof(sb));
	//Reading the information from superblock
	printf("Reading Superblock information.\n");
	uint32_t total_inodes_count = sb.s_inodes_count;
	uint32_t total_blocks_count = sb.s_blocks_count;
	uint32_t blocks_per_group = sb.s_blocks_per_group;
	uint32_t inodes_per_group = sb.s_inodes_per_group;
	uint32_t block_size = 1024 << sb.s_log_block_size;
	printf("Total inodes count:- %u.\n",total_inodes_count);
	printf("Total blocks count:- %u.\n",total_blocks_count);
	printf("Total blocks per group:- %u.\n",blocks_per_group);
	printf("Total inodes_per_group:- %u.\n",inodes_per_group);
	printf("Total block size:- %u.\n",block_size);
	
	uint32_t total_groups = total_blocks_count / blocks_per_group;
	if(total_blocks_count % blocks_per_group > 0)
	{
		total_groups++;
	}
	printf("Total groups:- %u.\n",total_groups);
	
	//Accessing the group descriptor table 
	printf("Accessing group descriptor tables.\n");
	struct ext2_group_desc* gdesc_table=malloc(sizeof(struct ext2_group_desc)*total_groups);
	int gdesc_table_offset = 1024 + 1024;
	if(lseek64(fd,gdesc_table_offset,SEEK_SET) < 0 || read(fd,gdesc_table,sizeof(gdesc_table))!=sizeof(gdesc_table))
	{
		perror("Read group descriptor.\n");
		exit(1);
	}
	
	//Reading the group descriptor entries
	printf("Reading group descriptor table entries.\n");
	for(int i=1; i<=total_groups;i++)
	{
		printf("Group descriptor Entry:-%d\n",i);
		printf("Inode table:- %u.\n",gdesc_table[i].bg_inode_table);
		printf("Inode Bitmap:- %u.\n",gdesc_table[i].bg_inode_bitmap);
		printf("Block Bitmap:- %u.\n",gdesc_table[i].bg_block_bitmap);
	}**/
	
	close(fd);
	return 0;
}
	
	
	
	
