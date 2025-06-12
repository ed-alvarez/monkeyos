#include "kernel.h"
#include "string.h"
#include "fs.h"

//  Filesystem Initialization
void fs_mkfs(void)
{
    struct fs_superblock sb = {
        .magic = FS_MAGIC,
        .total_sectors = blk_capacity / SECTOR_SIZE,
        .inode_table_start = 1,
        .data_start = FS_DATA_START_SECTOR,
        .max_inodes = FS_MAX_FILES};

    // Write superblock to sector 0
    read_write_disk(&sb, 0, true);

    // Zero out inode table (sectors 1–9)
    char zero[SECTOR_SIZE] = {0};
    for (int i = 0; i < FS_INODE_SECTORS; i++)
    {
        read_write_disk(zero, 1 + i, true);
    }

    printf("fs: formatted filesystem\n");
}

void fs_write(const char *filename, const char *data, uint32_t size)
{
    struct fs_inode inodes[FS_MAX_FILES];
    for (int i = 0; i < FS_INODE_SECTORS; i++)
    {
        read_write_disk(((char *)inodes) + i * SECTOR_SIZE, 1 + i, false);
    }

    // Find free inode
    struct fs_inode *free_inode = NULL;
    for (int i = 0; i < FS_MAX_FILES; i++)
    {
        if (inodes[i].size == 0)
        {
            free_inode = &inodes[i];
            break;
        }
    }

    if (!free_inode)
    {
        printf("fs: no free inodes\n");
        return;
    }

    strncpy(free_inode->name, filename, MAX_FILENAME);
    free_inode->size = size;
    free_inode->first_sector = FS_DATA_START_SECTOR + free_inode - inodes;

    // Write file data to data sector
    read_write_disk((void *)data, free_inode->first_sector, true);

    // Save updated inode table
    for (int i = 0; i < FS_INODE_SECTORS; i++)
    {
        read_write_disk(((char *)inodes) + i * SECTOR_SIZE, 1 + i, true);
    }

    printf("fs: wrote '%s' to sector %d\n", filename, free_inode->first_sector);
}

void fs_read(const char *filename, char *buf)
{
    struct fs_inode inodes[FS_MAX_FILES];
    for (int i = 0; i < FS_INODE_SECTORS; i++)
    {
        read_write_disk(((char *)inodes) + i * SECTOR_SIZE, 1 + i, false);
    }

    for (int i = 0; i < FS_MAX_FILES; i++)
    {
        if (strncmp(inodes[i].name, filename, MAX_FILENAME) == 0)
        {
            read_write_disk(buf, inodes[i].first_sector, false);
            return;
        }
    }

    printf("fs: file '%s' not found\n", filename);
}

void fs_ls(void)
{
    struct fs_inode inodes[FS_MAX_FILES];

    // Load all inode sectors
    for (int i = 0; i < FS_INODE_SECTORS; i++)
    {
        read_write_disk(((char *)inodes) + i * SECTOR_SIZE, 1 + i, false);
    }

    printf("Files:\n");
    int count = 0;
    for (int i = 0; i < FS_MAX_FILES; i++)
    {
        if (inodes[i].size > 0)
        {
            printf("  %s (%d bytes)\n", inodes[i].name, inodes[i].size);
            count++;
        }
    }

    if (count == 0)
    {
        printf("  (no files)\n");
    }
}

void fs_touch(const char *filename)
{
    char empty[1] = {0};          // empty content
    fs_write(filename, empty, 1); // 1 byte to ensure file is stored
}
