
/* 
 * Copyright (c) 2006-2007 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include <string.h>
#include <avr/pgmspace.h>
#include "partition.h"
#include "fat16.h"
#include "fat16_config.h"
#include "sd-reader_config.h"
#include "util.h"
#include "sd_raw.h"

#define DEBUG 1

#if USE_DYNAMIC_MEMORY
    #include <stdlib.h>
#endif

/**
 * \addtogroup fat16 FAT16 support
 *
 * This module implements FAT16 read and write access.
 * 
 * The following features are supported:
 * - File names up to 31 characters long.
 * - Unlimited depth of subdirectories.
 * - Short 8.3 and long filenames.
 * - Creating and deleting files.
 * - Reading and writing from and to files.
 * - File resizing.
 * - File sizes of up to 4 gigabytes.
 * 
 * @{
 */
/**
 * \file
 * FAT16 implementation (license: GPLv2 or LGPLv2.1)
 *
 * \author Roland Riegel
 */

/**
 * \addtogroup fat16_config FAT16 configuration
 * Preprocessor defines to configure the FAT16 implementation.
 */

/**
 * \addtogroup fat16_fs FAT16 access
 * Basic functions for handling a FAT16 filesystem.
 */

/**
 * \addtogroup fat16_file FAT16 file functions
 * Functions for managing files.
 */

/**
 * \addtogroup fat16_dir FAT16 directory functions
 * Functions for managing directories.
 */

/**
 * @}
 */

#define FAT16_CLUSTER_FREE 0x0000
#define FAT16_CLUSTER_RESERVED_MIN 0xfff0
#define FAT16_CLUSTER_RESERVED_MAX 0xfff6
#define FAT16_CLUSTER_BAD 0xfff7
#define FAT16_CLUSTER_LAST_MIN 0xfff8
#define FAT16_CLUSTER_LAST_MAX 0xffff

#define FAT16_DIRENTRY_DELETED 0xe5
#define FAT16_DIRENTRY_LFNLAST (1 << 6)
#define FAT16_DIRENTRY_LFNSEQMASK ((1 << 6) - 1)

/* Each entry within the directory table has a size of 32 bytes
 * and either contains a 8.3 DOS-style file name or a part of a
 * long file name, which may consist of several directory table
 * entries at once.
 *
 * multi-byte integer values are stored little-endian!
 *
 * 8.3 file name entry:
 * ====================
 * offset  length  description
 *      0       8  name (space padded)
 *      8       3  extension (space padded)
 *     11       1  attributes (FAT16_ATTRIB_*)
 *
 * long file name (lfn) entry ordering for a single file name:
 * ===========================================================
 * LFN entry n
 *     ...
 * LFN entry 2
 * LFN entry 1
 * 8.3 entry (see above)
 * 
 * lfn entry:
 * ==========
 * offset  length  description
 *      0       1  ordinal field
 *      1       2  unicode character 1
 *      3       3  unicode character 2
 *      5       3  unicode character 3
 *      7       3  unicode character 4
 *      9       3  unicode character 5
 *     11       1  attribute (always 0x0f)
 *     12       1  type (reserved, always 0)
 *     13       1  checksum
 *     14       2  unicode character 6
 *     16       2  unicode character 7
 *     18       2  unicode character 8
 *     20       2  unicode character 9
 *     22       2  unicode character 10
 *     24       2  unicode character 11
 *     26       2  cluster (unused, always 0)
 *     28       2  unicode character 12
 *     30       2  unicode character 13
 * 
 * The ordinal field contains a descending number, from n to 1.
 * For the n'th lfn entry the ordinal field is or'ed with 0x40.
 * For deleted lfn entries, the ordinal field is set to 0xe5.
 */


struct fat16_read_callback_arg
{
    uint16_t entry_cur;
    uint16_t entry_num;
    uint32_t entry_offset;
    uint8_t byte_count;
};

struct fat16_usage_count_callback_arg
{
    uint16_t cluster_count;
    uint8_t buffer_size;
};

#if !USE_DYNAMIC_MEMORY
static struct fat16_fs_struct fat16_fs_handlers[FAT16_FS_COUNT];
static struct fat16_file_struct fat16_file_handlers[FAT16_FILE_COUNT];
static struct fat16_dir_struct fat16_dir_handlers[FAT16_DIR_COUNT];
#endif

static uint8_t fat16_read_header(struct fat16_fs_struct* fs);
static uint8_t fat16_read_root_dir_entry(const struct fat16_fs_struct* fs, uint16_t entry_num, struct fat16_dir_entry_struct* dir_entry);
static uint8_t fat16_dir_entry_seek_callback(uint8_t* buffer, uint32_t offset, void* p);
static uint8_t fat16_dir_entry_read_callback(uint8_t* buffer, uint32_t offset, void* p);
static uint8_t fat16_interpret_dir_entry(struct fat16_dir_entry_struct* dir_entry, const uint8_t* raw_entry);


/**
 * \ingroup fat16_fs
 * Opens a FAT16 filesystem.
 *
 * \param[in] partition Discriptor of partition on which the filesystem resides.
 * \returns 0 on error, a FAT16 filesystem descriptor on success.
 * \see fat16_open
 */
struct fat16_fs_struct* fat16_open(struct partition_struct* partition)
{
    if(!partition ||
#if FAT16_WRITE_SUPPORT
       !partition->device_write ||
       !partition->device_write_interval
#else
       0
#endif
      )
        return 0;

#if USE_DYNAMIC_MEMORY
    struct fat16_fs_struct* fs = malloc(sizeof(*fs));
    if(!fs)
        return 0;
#else
    struct fat16_fs_struct* fs = fat16_fs_handlers;
    uint8_t i;
    for(i = 0; i < FAT16_FS_COUNT; ++i)
    {
        if(!fs->partition)
            break;

        ++fs;
    }
    if(i >= FAT16_FS_COUNT)
        return 0;
#endif

    memset(fs, 0, sizeof(*fs));

    fs->partition = partition;
    if(!fat16_read_header(fs))
    {
#if USE_DYNAMIC_MEMORY
        free(fs);
#else
        fs->partition = 0;
#endif
        return 0;
    }
    
    return fs;
}

/**
 * \ingroup fat16_fs
 * Closes a FAT16 filesystem.
 *
 * When this function returns, the given filesystem descriptor
 * will be invalid.
 *
 * \param[in] fs The filesystem to close.
 * \see fat16_open
 */
void fat16_close(struct fat16_fs_struct* fs)
{
    if(!fs)
        return;

#if USE_DYNAMIC_MEMORY
    free(fs);
#else
    fs->partition = 0;
#endif
}

/**
 * \ingroup fat16_fs
 * Reads and parses the header of a FAT16 filesystem.
 *
 * \param[inout] fs The filesystem for which to parse the header.
 * \returns 0 on failure, 1 on success.
 */
uint8_t fat16_read_header(struct fat16_fs_struct* fs)
{
    if(!fs)
        return 0;

    struct partition_struct* partition = fs->partition;
    if(!partition)
        return 0;

    /* read fat parameters */
    uint8_t buffer[25];
    uint32_t partition_offset = partition->offset * 512;
    if(!partition->device_read(partition_offset + 0x0b, buffer, sizeof(buffer)))
        return 0;

    uint16_t bytes_per_sector = ((uint16_t) buffer[0x00]) |
                                ((uint16_t) buffer[0x01] << 8);
    uint8_t sectors_per_cluster = buffer[0x02];
    uint16_t reserved_sectors = ((uint16_t) buffer[0x03]) |
                                ((uint16_t) buffer[0x04] << 8);
    uint8_t fat_copies = buffer[0x05];
    uint16_t max_root_entries = ((uint16_t) buffer[0x06]) |
                                ((uint16_t) buffer[0x07] << 8);
    uint16_t sector_count_16 = ((uint16_t) buffer[0x08]) |
                               ((uint16_t) buffer[0x09] << 8);
    uint16_t sectors_per_fat = ((uint16_t) buffer[0x0b]) |
                               ((uint16_t) buffer[0x0c] << 8);
    uint32_t sector_count = ((uint32_t) buffer[0x15]) |
                            ((uint32_t) buffer[0x16] << 8) |
                            ((uint32_t) buffer[0x17] << 16) |
                            ((uint32_t) buffer[0x18] << 24);
    
    if(sectors_per_fat == 0)
        /* this is not a FAT16 */
        return 0;

    if(sector_count == 0)
    {
        if(sector_count_16 == 0)
            /* illegal volume size */
            return 0;
        else
            sector_count = sector_count_16;
    }

    /* ensure we really have a FAT16 fs here */
    uint32_t data_sector_count = sector_count
                                 - reserved_sectors
                                 - (uint32_t) sectors_per_fat * fat_copies
                                 - ((max_root_entries * 32 + bytes_per_sector - 1) / bytes_per_sector);
    uint32_t data_cluster_count = data_sector_count / sectors_per_cluster;
    if(data_cluster_count < 4085 || data_cluster_count >= 65525)
        /* this is not a FAT16 */
        return 0;

    partition->type = PARTITION_TYPE_FAT16;

    /* fill header information */
    struct fat16_header_struct* header = &fs->header;
    memset(header, 0, sizeof(*header));
    
    header->size = sector_count * bytes_per_sector;

    header->fat_offset = /* jump to partition */
                         partition_offset +
                         /* jump to fat */
                         (uint32_t) reserved_sectors * bytes_per_sector;
    header->fat_size = (data_cluster_count + 2) * 2;

    header->sector_size = bytes_per_sector;
    header->cluster_size = (uint32_t) bytes_per_sector * sectors_per_cluster;

    header->root_dir_offset = /* jump to fats */
                              header->fat_offset +
                              /* jump to root directory entries */
                              (uint32_t) fat_copies * sectors_per_fat * bytes_per_sector;

    header->cluster_zero_offset = /* jump to root directory entries */
                                  header->root_dir_offset +
                                  /* skip root directory entries */
                                  (uint32_t) max_root_entries * 32;

    return 1;
}

/**
 * \ingroup fat16_fs
 * Reads a directory entry of the root directory.
 *
 * \param[in] fs Descriptor of file system to use.
 * \param[in] entry_num The index of the directory entry to read.
 * \param[out] dir_entry Directory entry descriptor which will get filled.
 * \returns 0 on failure, 1 on success
 * \see fat16_read_sub_dir_entry, fat16_read_dir_entry_by_path
 */
uint8_t fat16_read_root_dir_entry(const struct fat16_fs_struct* fs, uint16_t entry_num, struct fat16_dir_entry_struct* dir_entry)
{
    if(!fs || !dir_entry)
        return 0;

    /* we read from the root directory entry */
    const struct fat16_header_struct* header = &fs->header;
    device_read_interval_t device_read_interval = fs->partition->device_read_interval;
    uint8_t buffer[32];

    /* seek to the n-th entry */
    struct fat16_read_callback_arg arg;
    memset(&arg, 0, sizeof(arg));
    arg.entry_num = entry_num;
    if(!device_read_interval(header->root_dir_offset,
                             buffer,
                             sizeof(buffer),
                             header->cluster_zero_offset - header->root_dir_offset,
                             fat16_dir_entry_seek_callback,
                             &arg) ||
       arg.entry_offset == 0
      )
        return 0;

    /* read entry */
    memset(dir_entry, 0, sizeof(*dir_entry));
    if(!device_read_interval(arg.entry_offset,
                             buffer,
                             sizeof(buffer),
                             arg.byte_count,
                             fat16_dir_entry_read_callback,
                             dir_entry))
        return 0;

    return dir_entry->long_name[0] != '\0' ? 1 : 0;
}


/**
 * \ingroup fat16_fs
 * Callback function for seeking through subdirectory entries.
 */
uint8_t fat16_dir_entry_seek_callback(uint8_t* buffer, uint32_t offset, void* p)
{
  struct fat16_read_callback_arg* arg = (struct fat16_read_callback_arg*)p;

    /* skip deleted or empty entries */
    if(buffer[0] == FAT16_DIRENTRY_DELETED || !buffer[0])
        return 1;

    if(arg->entry_cur == arg->entry_num)
    {
        arg->entry_offset = offset;
        arg->byte_count = buffer[11] == 0x0f ?
                          ((buffer[0] & FAT16_DIRENTRY_LFNSEQMASK) + 1) * 32 :
                          32;
        return 0;
    }

    /* if we read a 8.3 entry, we reached a new directory entry */
    if(buffer[11] != 0x0f)
        ++arg->entry_cur;

    return 1;
}

/**
 * \ingroup fat16_fs
 * Callback function for reading a directory entry.
 */
uint8_t fat16_dir_entry_read_callback(uint8_t* buffer, uint32_t offset, void* p)
{
  struct fat16_dir_entry_struct* dir_entry = (struct fat16_dir_entry_struct*)p;

    /* there should not be any deleted or empty entries */
    if(buffer[0] == FAT16_DIRENTRY_DELETED || !buffer[0])
        return 0;

    if(!dir_entry->entry_offset)
        dir_entry->entry_offset = offset;
    
    switch(fat16_interpret_dir_entry(dir_entry, buffer))
    {
        case 0: /* failure */
            return 0;
        case 1: /* buffer successfully parsed, continue */
            return 1;
        case 2: /* directory entry complete, finish */
            return 0;
    }

    return 0;
}

/**
 * \ingroup fat16_fs
 * Interprets a raw directory entry and puts the contained
 * information into the directory entry.
 * 
 * For a single file there may exist multiple directory
 * entries. All except the last one are lfn entries, which
 * contain parts of the long filename. The last directory
 * entry is a traditional 8.3 style one. It contains all
 * other information like size, cluster, date and time.
 * 
 * \param[in,out] dir_entry The directory entry to fill.
 * \param[in] raw_entry A pointer to 32 bytes of raw data.
 * \returns 0 on failure, 1 on success and 2 if the
 *          directory entry is complete.
 */
uint8_t fat16_interpret_dir_entry(struct fat16_dir_entry_struct* dir_entry, const uint8_t* raw_entry)
{
    if(!dir_entry || !raw_entry || !raw_entry[0])
        return 0;

    char* long_name = dir_entry->long_name;
    if(raw_entry[11] == 0x0f)
    { // skip long filename support
      /*
        uint16_t char_offset = ((raw_entry[0] & 0x3f) - 1) * 13;

        if(char_offset + 12 < sizeof(dir_entry->long_name))
        {
            // Lfn supports unicode, but we do not, for now.
            // So we assume pure ascii and read only every
            // second byte.
            //
            long_name[char_offset + 0] = raw_entry[1];
            long_name[char_offset + 1] = raw_entry[3];
            long_name[char_offset + 2] = raw_entry[5];
            long_name[char_offset + 3] = raw_entry[7];
            long_name[char_offset + 4] = raw_entry[9];
            long_name[char_offset + 5] = raw_entry[14];
            long_name[char_offset + 6] = raw_entry[16];
            long_name[char_offset + 7] = raw_entry[18];
            long_name[char_offset + 8] = raw_entry[20];
            long_name[char_offset + 9] = raw_entry[22];
            long_name[char_offset + 10] = raw_entry[24];
            long_name[char_offset + 11] = raw_entry[28];
            long_name[char_offset + 12] = raw_entry[30];
        }
*/
        return 1;
    }
    else
    {
        /* if we do not have a long name, take the short one */
        uint8_t i;
        for(i = 0; i < 8; ++i)
            {
                if(raw_entry[i] == ' ')
                    break;
                long_name[i] = raw_entry[i];
            }
        if(long_name[0] == 0x05)
                long_name[0] = (char) FAT16_DIRENTRY_DELETED;

        if(raw_entry[8] != ' ')
        {
                long_name[i++] = '.';

                uint8_t j = 8;
                for(; j < 11; ++j)
                {
                    if(raw_entry[j] != ' ')
                    {
                        long_name[i++] = raw_entry[j];
                    }
                    else
                    {
                        break;
                    }
                }
        } 

        long_name[i] = '\0';
                
        /* extract properties of file and store them within the structure */
        dir_entry->attributes = raw_entry[11];
        dir_entry->cluster = ((uint16_t) raw_entry[26]) |
                             ((uint16_t) raw_entry[27] << 8);
        dir_entry->file_size = ((uint32_t) raw_entry[28]) |
                               ((uint32_t) raw_entry[29] << 8) |
                               ((uint32_t) raw_entry[30] << 16) |
                               ((uint32_t) raw_entry[31] << 24);

        return 2;
    }
}

/**
 * \ingroup fat16_file
 * Retrieves the directory entry of a path.
 *
 * The given path may both describe a file or a directory.
 *
 * \param[in] fs The FAT16 filesystem on which to search.
 * \param[in] path The path of which to read the directory entry.
 * \param[out] dir_entry The directory entry to fill.
 * \returns 0 on failure, 1 on success.
 * \see fat16_read_dir
 */
uint8_t fat16_get_dir_entry_of_root(struct fat16_fs_struct* fs, struct fat16_dir_entry_struct* dir_entry)
{
    if(!fs || !dir_entry)
        return 0;

    /* begin with the root directory */
    memset(dir_entry, 0, sizeof(*dir_entry));
    dir_entry->attributes = FAT16_ATTRIB_DIR;
    return 1;
}

/**
 * \ingroup fat16_fs
 * Retrieves the next following cluster of a given cluster.
 *
 * Using the filesystem file allocation table, this function returns
 * the number of the cluster containing the data directly following
 * the data within the cluster with the given number.
 *
 * \param[in] fs The filesystem for which to determine the next cluster.
 * \param[in] cluster_num The number of the cluster for which to determine its successor.
 * \returns The wanted cluster number, or 0 on error.
 */
uint16_t fat16_get_next_cluster(const struct fat16_fs_struct* fs, uint16_t cluster_num)
{
    if(!fs || cluster_num < 2)
        return 0;

    /* read appropriate fat entry */
    uint8_t fat_entry[2];
    if(!fs->partition->device_read(fs->header.fat_offset + 2 * cluster_num, fat_entry, 2))
        return 0;

    /* determine next cluster from fat */
    cluster_num = ((uint16_t) fat_entry[0]) |
                  ((uint16_t) fat_entry[1] << 8);
    
    if(cluster_num == FAT16_CLUSTER_FREE ||
       cluster_num == FAT16_CLUSTER_BAD ||
       (cluster_num >= FAT16_CLUSTER_RESERVED_MIN && cluster_num <= FAT16_CLUSTER_RESERVED_MAX) ||
       (cluster_num >= FAT16_CLUSTER_LAST_MIN && cluster_num <= FAT16_CLUSTER_LAST_MAX))
        return 0;
    
    return cluster_num;
}




/**
 * \ingroup fat16_file
 * Opens a file on a FAT16 filesystem.
 *
 * \param[in] fs The filesystem on which the file to open lies.
 * \param[in] dir_entry The directory entry of the file to open.
 * \returns The file handle, or 0 on failure.
 * \see fat16_close_file
 */
struct fat16_file_struct* fat16_open_file(struct fat16_fs_struct* fs, const struct fat16_dir_entry_struct* dir_entry)
{
  if(!fs || !dir_entry || (dir_entry->attributes & FAT16_ATTRIB_DIR)) {
#if DEBUG
    putstring("bad input");
#endif
        return 0;
  }

#if USE_DYNAMIC_MEMORY
    struct fat16_file_struct* fd = malloc(sizeof(*fd));
    if(!fd)
        return 0;
#else
    struct fat16_file_struct* fd = fat16_file_handlers;
    uint8_t i;
    for(i = 0; i < FAT16_FILE_COUNT; ++i)
    {
        if(!fd->fs)
            break;

        ++fd;
    }
    if(i >= FAT16_FILE_COUNT) {
#if DEBUG
      putstring("too many open files");
#endif
      return 0;
    }
#endif
    
    memcpy(&fd->dir_entry, dir_entry, sizeof(*dir_entry));
    fd->fs = fs;
    fd->pos = 0;
    fd->pos_cluster = dir_entry->cluster;

    return fd;
}

/**
 * \ingroup fat16_file
 * Closes a file.
 *
 * \param[in] fd The file handle of the file to close.
 * \see fat16_open_file
 */
void fat16_close_file(struct fat16_file_struct* fd)
{
    if(fd)
#if USE_DYNAMIC_MEMORY
        free(fd);
#else
        fd->fs = 0;
#endif
}

/**
 * \ingroup fat16_file
 * Reads data from a file.
 * 
 * The data requested is read from the current file location.
 *
 * \param[in] fd The file handle of the file from which to read.
 * \param[out] buffer The buffer into which to write.
 * \param[in] buffer_len The amount of data to read.
 * \returns The number of bytes read, 0 on end of file, or -1 on failure.
 * \see fat16_write_file
 */
int16_t fat16_read_file(struct fat16_file_struct* fd, uint8_t* buffer, uint16_t buffer_len)
{
    /* check arguments */
    if(!fd || !buffer || buffer_len < 1)
        return -1;

    /* determine number of bytes to read */
    if(fd->pos + buffer_len > fd->dir_entry.file_size)
        buffer_len = fd->dir_entry.file_size - fd->pos;
    if(buffer_len == 0)
        return 0;
    
    uint16_t cluster_size = fd->fs->header.cluster_size;
    uint16_t cluster_num = fd->pos_cluster;
    uint16_t buffer_left = buffer_len;
    uint16_t first_cluster_offset = fd->pos % cluster_size;

    //putstring("\n\rcluster offset = ");
    //uart_putdw_dec(first_cluster_offset);
    //putstring(" / "); uart_putdw_dec(cluster_size);
    /* find cluster in which to start reading */
    if(!cluster_num)
    {
        cluster_num = fd->dir_entry.cluster;
        
        if(!cluster_num)
        {
            if(!fd->pos)
                return 0;
            else
                return -1;
        }

        if(fd->pos)
        {
            uint32_t pos = fd->pos;
            while(pos >= cluster_size)
            {
                pos -= cluster_size;
                cluster_num = fat16_get_next_cluster(fd->fs, cluster_num);
                if(!cluster_num)
                    return -1;
            }
        }
    }
    
    /* read data */
    do
    {
        /* calculate data size to copy from cluster */
        uint32_t cluster_offset = fd->fs->header.cluster_zero_offset +
                                  (uint32_t) (cluster_num - 2) * cluster_size + first_cluster_offset;
        uint16_t copy_length = cluster_size - first_cluster_offset;
        if(copy_length > buffer_left)
            copy_length = buffer_left;

        /* read data */
        if(!fd->fs->partition->device_read(cluster_offset, buffer, copy_length))
            return buffer_len - buffer_left;

        /* calculate new file position */
        buffer += copy_length;
        buffer_left -= copy_length;
        fd->pos += copy_length;

        if(first_cluster_offset + copy_length >= cluster_size)
        {
	  //putstring_nl("new cluster");
            /* we are on a cluster boundary, so get the next cluster */
            if((cluster_num = fat16_get_next_cluster(fd->fs, cluster_num)))
            {
                first_cluster_offset = 0;
            }
            else
            {
                fd->pos_cluster = 0;
                return buffer_len - buffer_left;
            }
        }

        fd->pos_cluster = cluster_num;

    } while(buffer_left > 0); /* check if we are done */

    return buffer_len;
}

/**************************************************/
#define SECTORSIZE 512
// this is a terrible hack, read a full sector at a time...speeds things up a bunch
int16_t fat16_start_readsector(struct fat16_file_struct* fd, uint8_t* buffer, uint16_t buffer_len)
{
    /* check arguments */
    if(!fd || !buffer || buffer_len < 1)
        return -1;

    // put the fd on the sector boundary
    fd->pos -= (fd->pos % SECTORSIZE);

    //putstring("position: "); uart_putdw_dec(fd->pos);

    /* determine number of bytes to read */
    if (fd->pos >  fd->dir_entry.file_size)
      return 0;

    if(fd->pos + buffer_len > fd->dir_entry.file_size)
        buffer_len = fd->dir_entry.file_size - fd->pos;
    if(buffer_len == 0)
        return 0;

    // only one sector at a time, so...
    if (buffer_len > SECTORSIZE)
      buffer_len = SECTORSIZE;

    uint16_t cluster_size = fd->fs->header.cluster_size;
    uint16_t cluster_num = fd->pos_cluster;
    uint16_t buffer_left = buffer_len;
    uint16_t first_cluster_offset = fd->pos % cluster_size;

    // putstring("\n\rcluster offset = ");
    //uart_putdw_dec(first_cluster_offset);
    //putstring(" / "); uart_putdw_dec(cluster_size); putstring_nl("");
    /* find cluster in which to start reading */
    if(!cluster_num)
    {
        cluster_num = fd->dir_entry.cluster;
        
        if(!cluster_num)
        {
            if(!fd->pos)
                return 0;
            else
                return -1;
        }

        if(fd->pos)
        {
            uint32_t pos = fd->pos;
            while(pos >= cluster_size)
            {
                pos -= cluster_size;
                cluster_num = fat16_get_next_cluster(fd->fs, cluster_num);
                if(!cluster_num)
                    return -1;
            }
        }
    }

    /* start to read data */
    
    /* calculate data size to copy from cluster */
    uint32_t cluster_offset = fd->fs->header.cluster_zero_offset +
      (uint32_t) (cluster_num - 2) * cluster_size + first_cluster_offset;
    uint16_t copy_length = cluster_size - first_cluster_offset;
    if(copy_length > buffer_left)
      copy_length = buffer_left;

    

    /* read data */
    if(! sd_raw_read_start(cluster_offset, buffer, copy_length))
      return buffer_len - buffer_left;
    
    /* calculate new file position */
    fd->pos += copy_length;
    
    fd->pos_cluster = cluster_num;
    
    return buffer_len;
}
/********************************************************************************/


/**
 * \ingroup fat16_file
 * Repositions the read/write file offset.
 *
 * Changes the file offset where the next call to fat16_read_file()
 * or fat16_write_file() starts reading/writing.
 *
 * If the new offset is beyond the end of the file, fat16_resize_file()
 * is implicitly called, i.e. the file is expanded.
 *
 * The new offset can be given in different ways determined by
 * the \c whence parameter:
 * - \b FAT16_SEEK_SET: \c *offset is relative to the beginning of the file.
 * - \b FAT16_SEEK_CUR: \c *offset is relative to the current file position.
 * - \b FAT16_SEEK_END: \c *offset is relative to the end of the file.
 *
 * The resulting absolute offset is written to the location the \c offset
 * parameter points to.
 * 
 * \param[in] fd The file decriptor of the file on which to seek.
 * \param[in,out] offset A pointer to the new offset, as affected by the \c whence
 *                   parameter. The function writes the new absolute offset
 *                   to this location before it returns.
 * \param[in] whence Affects the way \c offset is interpreted, see above.
 * \returns 0 on failure, 1 on success.
 */
uint8_t fat16_seek_file(struct fat16_file_struct* fd, int32_t* offset, uint8_t whence)
{
    if(!fd || !offset)
        return 0;

    uint32_t new_pos = fd->pos;
    switch(whence)
    {
        case FAT16_SEEK_SET:
            new_pos = *offset;
            break;
        case FAT16_SEEK_CUR:
            new_pos += *offset;
            break;
        case FAT16_SEEK_END:
            new_pos = fd->dir_entry.file_size + *offset;
            break;
        default:
            return 0;
    }

    if(new_pos > fd->dir_entry.file_size)
        return 0;

    fd->pos = new_pos;
    fd->pos_cluster = 0;

    *offset = new_pos;
    return 1;
}

/**
 * \ingroup fat16_dir
 * Opens a directory.
 *
 * \param[in] fs The filesystem on which the directory to open resides.
 * \param[in] dir_entry The directory entry which stands for the directory to open.
 * \returns An opaque directory descriptor on success, 0 on failure.
 * \see fat16_close_dir
 */
struct fat16_dir_struct* fat16_open_dir(struct fat16_fs_struct* fs, const struct fat16_dir_entry_struct* dir_entry)
{
    if(!fs || !dir_entry || !(dir_entry->attributes & FAT16_ATTRIB_DIR))
        return 0;

#if USE_DYNAMIC_MEMORY
    struct fat16_dir_struct* dd = malloc(sizeof(*dd));
    if(!dd)
        return 0;
#else
    struct fat16_dir_struct* dd = fat16_dir_handlers;
    uint8_t i;
    for(i = 0; i < FAT16_DIR_COUNT; ++i)
    {
        if(!dd->fs)
            break;

        ++dd;
    }
    if(i >= FAT16_DIR_COUNT)
        return 0;
#endif
    
    memcpy(&dd->dir_entry, dir_entry, sizeof(*dir_entry));
    dd->fs = fs;
    dd->entry_next = 0;

    return dd;
}

/**
 * \ingroup fat16_dir
 * Closes a directory descriptor.
 *
 * This function destroys a directory descriptor which was
 * previously obtained by calling fat16_open_dir(). When this
 * function returns, the given descriptor will be invalid.
 *
 * \param[in] dd The directory descriptor to close.
 * \see fat16_open_dir
 */
void fat16_close_dir(struct fat16_dir_struct* dd)
{
    if(dd)
#if USE_DYNAMIC_MEMORY
        free(dd);
#else
        dd->fs = 0;
#endif
}

/**
 * \ingroup fat16_dir
 * Reads the next directory entry contained within a parent directory.
 *
 * \param[in] dd The descriptor of the parent directory from which to read the entry.
 * \param[out] dir_entry Pointer to a buffer into which to write the directory entry information.
 * \returns 0 on failure, 1 on success.
 * \see fat16_reset_dir
 */
uint8_t fat16_read_dir(struct fat16_dir_struct* dd, struct fat16_dir_entry_struct* dir_entry)
{
    if(!dd || !dir_entry)
        return 0;

    if(dd->dir_entry.cluster == 0)
    {
        /* read entry from root directory */
        if(fat16_read_root_dir_entry(dd->fs, dd->entry_next, dir_entry))
        {
            ++dd->entry_next;
            return 1;
        }
    }
    else
    {
       return 0; // only support root dir
    }

    /* restart reading */
    dd->entry_next = 0;

    return 0;
}

/**
 * \ingroup fat16_dir
 * Resets a directory handle.
 *
 * Resets the directory handle such that reading restarts
 * with the first directory entry.
 *
 * \param[in] dd The directory handle to reset.
 * \returns 0 on failure, 1 on success.
 * \see fat16_read_dir
 */
uint8_t fat16_reset_dir(struct fat16_dir_struct* dd)
{
    if(!dd)
        return 0;

    dd->entry_next = 0;
    return 1;
}
