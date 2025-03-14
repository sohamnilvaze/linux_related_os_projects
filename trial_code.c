#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/* Constants for ext2 */
#define EXT2_SUPER_MAGIC 0xEF53
#define BASE_OFFSET 1024    /* Superblock starts at offset 1024 */
#define DEFAULT_INODE_SIZE 128

/* ext2 superblock structure (only the fields we need) */
struct ext2_super_block {
    uint32_t   s_inodes_count;
    uint32_t   s_blocks_count;
    uint32_t   s_r_blocks_count;
    uint32_t   s_free_blocks_count;
    uint32_t   s_free_inodes_count;
    uint32_t   s_first_data_block;
    uint32_t   s_log_block_size;
    uint32_t   s_log_frag_size;
    uint32_t   s_blocks_per_group;
    uint32_t   s_frags_per_group;
    uint32_t   s_inodes_per_group;
    uint32_t   s_mtime;
    uint32_t   s_wtime;
    uint16_t   s_mnt_count;
    uint16_t   s_max_mnt_count;
    uint16_t   s_magic;
    uint16_t   s_state;
    uint16_t   s_errors;
    uint16_t   s_minor_rev_level;
    uint32_t   s_lastcheck;
    uint32_t   s_checkinterval;
    uint32_t   s_creator_os;
    uint32_t   s_rev_level;
    uint16_t   s_def_resuid;
    uint16_t   s_def_resgid;
    uint32_t   s_first_ino;
    uint16_t   s_inode_size;
    uint16_t   s_block_group_nr;
    /* ... remaining fields not used here ... */
};

/* ext2 group descriptor structure */
struct ext2_group_desc {
    uint32_t bg_block_bitmap;      /* Blocks bitmap block */
    uint32_t bg_inode_bitmap;      /* Inodes bitmap block */
    uint32_t bg_inode_table;       /* Inodes table block */
    uint16_t bg_free_blocks_count; /* Free blocks count */
    uint16_t bg_free_inodes_count; /* Free inodes count */
    uint16_t bg_used_dirs_count;   /* Directories count */
    uint16_t bg_pad;
    uint32_t bg_reserved[3];
};

/* ext2 inode structure */
struct ext2_inode {
    uint16_t i_mode;        /* File mode */
    uint16_t i_uid;         /* Owner Uid */
    uint32_t i_size;        /* Size in bytes */
    uint32_t i_atime;       /* Access time */
    uint32_t i_ctime;       /* Creation time */
    uint32_t i_mtime;       /* Modification time */
    uint32_t i_dtime;       /* Deletion Time */
    uint16_t i_gid;         /* Group Id */
    uint16_t i_links_count; /* Links count */
    uint32_t i_blocks;      /* Blocks count */
    uint32_t i_flags;       /* File flags */
    uint32_t i_osd1;        /* OS dependent 1 */
    uint32_t i_block[15];   /* Pointers to blocks */
    uint32_t i_generation;  /* File version (for NFS) */
    uint32_t i_file_acl;    /* File ACL */
    uint32_t i_dir_acl;     /* Directory ACL */
    uint32_t i_faddr;       /* Fragment address */
    uint8_t  i_osd2[12];    /* OS dependent 2 */
};

void usage(char *progname) {
    fprintf(stderr, "Usage: %s <device-file> <inode-number>\n", progname);
    exit(1);
}

int main(int argc, char *argv[]) {
    int fd;
    ssize_t n;
    struct ext2_super_block sb;
    struct ext2_group_desc gd;
    struct ext2_inode inode;
    int inode_num;
    int inode_size;
    uint32_t block_size;
    off_t inode_table_offset;
    off_t inode_offset;

    if(argc != 3)
        usage(argv[0]);

    /* Get device file and inode number. Skip a leading '/' in inode number if present */
    char *dev_file = argv[1];
    char *inode_str = argv[2];
    if(inode_str[0] == '/')
        inode_str++;
    inode_num = atoi(inode_str);
    if(inode_num <= 0) {
        fprintf(stderr, "Invalid inode number\n");
        exit(1);
    }

    /* Open the device file */
    fd = open(dev_file, O_RDONLY);
    if(fd < 0) {
        perror("open");
        exit(1);
    }

    /* Read the superblock */
    if(lseek(fd, BASE_OFFSET, SEEK_SET) < 0) {
        perror("lseek superblock");
        exit(1);
    }
    n = read(fd, &sb, sizeof(sb));
    if(n != sizeof(sb)) {
        perror("read superblock");
        exit(1);
    }
    if(sb.s_magic != EXT2_SUPER_MAGIC) {
        fprintf(stderr, "Not an ext2 filesystem (magic: 0x%x)\n", sb.s_magic);
        exit(1);
    }
    block_size = 1024 << sb.s_log_block_size;
    inode_size = (sb.s_inode_size == 0 ? DEFAULT_INODE_SIZE : sb.s_inode_size);

    /* Determine the location of the group descriptor table.
     * For block size == 1024, it usually starts at offset 2*block_size.
     * For larger block sizes, it typically starts at block_size.
     */
    off_t gd_offset = (block_size == 1024) ? 2 * block_size : block_size;
    if(lseek(fd, gd_offset, SEEK_SET) < 0) {
        perror("lseek group descriptor");
        exit(1);
    }
    n = read(fd, &gd, sizeof(gd));
    if(n != sizeof(gd)) {
        perror("read group descriptor");
        exit(1);
    }

    /* Inode table starts at block bg_inode_table */
    inode_table_offset = gd.bg_inode_table * block_size;

    /* Inodes are numbered starting at 1. Compute the offset in the inode table */
    inode_offset = inode_table_offset + (inode_num - 1) * inode_size;
    if(lseek(fd, inode_offset, SEEK_SET) < 0) {
        perror("lseek inode");
        exit(1);
    }

    n = read(fd, &inode, (inode_size < sizeof(inode) ? inode_size : sizeof(inode)));
    if(n <= 0) {
        perror("read inode");
        exit(1);
    }

    /* Print selected inode fields */
    printf("Inode %d:\n", inode_num);
    printf("  Mode:         0x%x\n", inode.i_mode);
    printf("  UID:          %d\n", inode.i_uid);
    printf("  Size:         %d bytes\n", inode.i_size);
    printf("  GID:          %d\n", inode.i_gid);
    printf("  Links count:  %d\n", inode.i_links_count);
    printf("  Blocks count: %d\n", inode.i_blocks);
    printf("  Block pointers:\n");
    for(int i = 0; i < 15; i++) {
        printf("    [%d]: %d\n", i, inode.i_block[i]);
    }

    close(fd);
    return 0;
}

