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
    uint32_t block_size,inodes_per_group,blocks_per_group,groups_count;
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
	groups_count = sb.s_blocks_count / sb.s_blocks_per_group;
	if(sb.s_blocks_count % sb.s_blocks_per_group > 0)
	{
		groups_count++;
	}
	block_size = 1024 << sb.s_log_block_size;
	inode_size = sb.s_inode_size;
	printf("Block size in bytes:- %u.\n",block_size);
	
	off_t gdt_offset = 2 * block_size;
	off_t bitmap_offset = 1024 + block_size*(1+groups_count);
	
	
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
	printf("Inode Access Time:- %u.\n",inode.i_atime);
	printf("Inode Creation TIme:- %u.\n",inode.i_ctime);
	printf("Inode Modification Time:- %u.\n",inode.i_mtime);
	printf("Inode Deletion TIme:- %u.\n",inode.i_dtime);
	
	for(int i=0; i<15;i++)
	{
		printf("Inode Block %d:- %u.\n",i,inode.i_block[i]);
	}
	
	printf("Reading the data block bitmap.\n");
	unsigned char bitmap[block_size];
	lseek64(fd,1024 + 1024*groups_count,SEEK_SET);
	int n_bitmap = read(fd,&bitmap,sizeof(bitmap));
	if(n_bitmap!=sizeof(bitmap))
	{
		perror("read data block bitmap.");
		exit(1);
	}
	bitmap[block_size]='\0';
	
	printf("Bitmap contents:\n");
	for(int i=0; i<sizeof(bitmap);i++)
	{
		printf("%u ",bitmap[i]);
	}
	
	printf("Reading the inode bitmap.\n");
	unsigned char inode_bitmap[block_size];
	lseek64(fd,1024 + 1024*groups_count + block_size,SEEK_SET);
	int n_inode_bitmap = read(fd,&inode_bitmap,sizeof(inode_bitmap));
	if(n_inode_bitmap!=sizeof(inode_bitmap))
	{
		perror("read inode bitmap.");
		exit(1);
	}
	inode_bitmap[block_size]='\0';
	
	printf("Inode Bitmap contents:\n");
	for(int i=0; i<sizeof(inode_bitmap);i++)
	{
		printf("%u ",inode_bitmap[i]);
	}
	
	
	
	
	
	close(fd);
	return 0;
}
	
	
	
	
