#include "common.h"

#define FS_MAGIC 0xDEADBEEF
#define FS_INODE_SECTORS 9
#define FS_MAX_FILES (FS_INODE_SECTORS * (SECTOR_SIZE / sizeof(struct fs_inode)))
#define FS_DATA_START_SECTOR 10
#define MAX_FILENAME 28

struct fs_superblock
{
    uint32_t magic;
    uint32_t total_sectors;
    uint32_t inode_table_start;
    uint32_t data_start;
    uint32_t max_inodes;
};

struct fs_inode
{
    char name[MAX_FILENAME];
    uint32_t size;
    uint32_t first_sector;
};

void fs_mkfs(void);
void fs_ls(void);
void fs_touch(const char *filename);