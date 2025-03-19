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
#define BLOCK_OFFSET(block) (BASE_OFFSET + (block-1)*1024)
#define DEFAULT_INODE_SIZE 128
#define EXT2_NDIR_BLOCKS 12
#define EXT2_IND_BLOCKS EXT2_NDIR_BLOCKS
#define EXT2_DIND_BLOCKS (EXT2_IND_BLOCKS + 1)
#define EXT2_TIND_BLOCKS (EXT2_DIND_BLOCKS + 1)

/*
void usage(char *progname) {
    fprintf(stderr, "Usage: %s <device-file>\n", progname);
    exit(1);
}

static void read_inode(fd,inode_no,group,inode)
int fd;
int inode_no;
const struct ext2_group_desc* group;
struct ext2_inode * inode;
{
	//int fd;
	//int inode_no;
	//const struct ext2_group_desc * group;
	//struct ext2_inode inode;
	printf("%u\n",group->bg_inode_table);
	lseek64(fd , BLOCK_OFFSET(group->bg_inode_table) + (inode_no-1) * sizeof(struct ext2_inode),SEEK_SET);
	read(fd,inode,sizeof(struct ext2_inode));
	return;
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
	
	if(argc!=2)
	{
		usage(argv[0]);
	}
	
	char* dev_file = argv[1];
	
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
	
	/*off_t gdt_offset = 2 * block_size;
	lseek64(fd,gdt_offset,SEEK_SET);
	int n_grp_desc = read(fd,&gd,sizeof(struct ext2_group_desc));
	if(n_grp_desc != sizeof(struct ext2_group_desc))
	{
		perror("read group descriptor");
		exit(1);
	}
	printf("Reading the inode of root from the disk.\n");
	read_inode(fd,12,&gd,&inode);
	for(int i=0; i<EXT2_N_BLOCKS;i++)
	{
		if(i < EXT2_NDIR_BLOCKS)
		{
			printf("Block %d :- %u.\n",i,inode.i_block[i]);
			unsigned char block_content[block_size];
			lseek64(fd,BLOCK_OFFSET(inode.i_block[i]),SEEK_SET);
			read(fd,&block_content, sizeof(block_content));
			block_content[block_size] = '\0';
			for(int j=0; j<sizeof(block_content);j++)
			{
				printf("%c ",block_content[j]);
			}
		}
		else if(i == EXT2_IND_BLOCKS)
		{
			printf("Single Indirect Block:- %u.\n",inode.i_block[i]);
			unsigned char block_content[block_size];
			lseek64(fd,BLOCK_OFFSET(inode.i_block[i]),SEEK_SET);
			read(fd,&block_content, sizeof(block_content));
			block_content[block_size] = '\0';
			for(int j=0; j<sizeof(block_content);j++)
			{
				printf("%c ",block_content[j]);
				
			}
		}
		else if(i == EXT2_DIND_BLOCKS)
		{
			printf("Double Indirect Block:- %u.\n",inode.i_block[i]);
			unsigned char block_content[block_size];
			lseek64(fd,BLOCK_OFFSET(inode.i_block[i]),SEEK_SET);
			read(fd,&block_content, sizeof(block_content));
			block_content[block_size] = '\0';
			for(int j=0; j<sizeof(block_content);j++)
			{
				printf("%c ",block_content[j]);
			}
		}
		else if(i == EXT2_TIND_BLOCKS)
		{
			printf("Triple Indirect Block:- %u.\n",inode.i_block[i]);
			unsigned char block_content[block_size];
			lseek64(fd,BLOCK_OFFSET(inode.i_block[i]),SEEK_SET);
			read(fd,&block_content, sizeof(block_content));
			block_content[block_size] = '\0';
			for(int j=0; j<sizeof(block_content);j++)
			{
				printf("%c ",block_content[j]);
			}
		}
	}
	
	close(fd);
	return 0;
}*/

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
	printf("Inode Access Time:- %u.\n",inode.i_atime);
	printf("Inode Creation Time:- %u.\n",inode.i_ctime);
	printf("Inode Modification Time:- %u.\n",inode.i_mtime);
	printf("Inode Deletion TIme:- %u.\n",inode.i_dtime);
	
	for(int i=1; i<=EXT2_NDIR_BLOCKS;i++)
	{
		printf("Inode Block %d:- %u.\n",i,inode.i_block[i]);
	}
	printf("Printing the contents of each assigned inode.\n");
	for(int i=0; i<EXT2_NDIR_BLOCKS;i++)
	{	
		uint32_t offset = inode.i_block[i];
		unsigned char block_content[block_size];
		lseek64(fd,block_size*offset,SEEK_SET);
		int n_block_content = read(fd,&block_content,sizeof(block_content));
		if(n_block_content!=sizeof(block_content))
		{
			perror("read inode block content.");
			exit(1);
		}
		printf("Data Block at index %d at %u contents:-\n",i,offset);
		for(int i=0; i<sizeof(block_content);i++)
		{
			printf("%c",block_content[i]);
		}
		printf("\n");
	}
	
	printf("Reading the single indirect block.\n");
	unsigned char single_indirect_block[block_size];
	lseek64(fd,BLOCK_OFFSET(inode.i_block[EXT2_IND_BLOCKS]),SEEK_SET);
	int n_single_indirect_content = read(fd,&single_indirect_block,sizeof(single_indirect_block));
	if(n_single_indirect_content!=sizeof(single_indirect_block))
	{
		perror("read single indirect block.\n");
		exit(1);
	}
	printf("\nReading the contents of Single Indirect Block:-\n");
	for(int i=0; i<sizeof(single_indirect_block);i++)
	{
		printf("%c ",single_indirect_block[i]);
		if(single_indirect_block[i]!=0)
		{
			unsigned char single_indirect_block_content[block_size];
			lseek64(fd,BLOCK_OFFSET(single_indirect_block[i]),SEEK_SET);
			int n_single_indirect_block_content = read(fd,&single_indirect_block_content,sizeof(single_indirect_block_content));
			if(n_single_indirect_block_content!=sizeof(single_indirect_block_content))
			{
				perror("read single indirect block contents.");
				exit(1);
			}
			for(int i=0;i<sizeof(single_indirect_block_content);i++)
			{
				printf("%c",single_indirect_block_content[i]);
			}
		}
	}
	
	
		
		
	close(fd);
	return 0;
}
	
	
	
