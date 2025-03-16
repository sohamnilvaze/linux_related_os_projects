#define _LARGEFILE64_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<ext2fs/ext2_fs.h>
#include<string.h>

#define EXT2_SUPER_MAGIC 0xEF53
#define BASE_OFFSET 1024
#define DEFAULT_INODE_SIZE 128
#define BLOCK_OFFSET(block) (BASE_OFFSET + (block-1)*block_size)
#define EXT2_NAME_LEN 255

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
    struct ext2_dir_entry* entry;
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
	
	printf("Reading the first data block.\n");
	lseek64(fd,BLOCK_OFFSET(inode.i_block[0]),SEEK_SET);
	unsigned char data_block_0[block_size];
	int n_first_entry = read(fd,&data_block_0,sizeof(data_block_0));
	if(n_first_entry!=sizeof(data_block_0))
	{
		perror("first entry error.");
		exit(1);
	}
	
	printf("First entry contents:\n");
	for(int i=0; i<sizeof(data_block_0);i++)
	{
		printf("%u ",data_block_0[i]);
	}
	
	int size=0;
	entry = (struct ext2_dir_entry*)data_block_0;
	while(size < inode.i_size)
	{
		char file_name[EXT2_NAME_LEN+1];
		memcpy(file_name,entry->name,entry->name_len);
		file_name[entry->name_len] = '\0';
		printf("%10u %s.\n",entry->inode,file_name);
		printf("rec_len:- %u.\n",entry->rec_len);
		entry = (void*) entry + entry->rec_len;
		size+=entry->rec_len;
	}
		
	
	
	
	
	
	
	
	close(fd);
	return 0;
}
	
	
	
	
