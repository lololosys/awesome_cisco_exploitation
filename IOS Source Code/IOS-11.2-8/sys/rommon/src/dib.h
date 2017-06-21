/* $Id: dib.h,v 3.2 1995/11/17 18:41:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/dib.h,v $
 *------------------------------------------------------------------
 * dib.h
 *
 * February 1994, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The Device Info Block structure and defines.
 *------------------------------------------------------------------
 * $Log: dib.h,v $
 * Revision 3.2  1995/11/17  18:41:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __DIB_H__
#define __DIB_H__

/*
** The Device Info Block structure and defines
**
** This structure resides in the first sector of a device that
** supports a file system other than the simple file system.
**
** This info compliments of Jack Jenny
*/

#define FS_VOLUME_ID_MAX  32
#define FS_DEVICE_INFO_BLOCK_MAGIC  0x06887635
#define FS_SPARE_SECTOR_MAX  16

typedef struct fs_device_info_block_t
{
    ulong    magic;
    ulong    length;
    ulong    sector_size;
    ulong    prog_algorithm;
    ulong    erased_state;
    ulong    file_system_version;
    ulong    file_system_offset;
    ulong    file_system_length;
    ulong    monlib_offset;
    ulong    monlib_length;
    ulong    monlib_crc;
    ulong    bad_sector_map_offset;
    ulong    bad_sector_map_length;
    ulong    squeeze_log_offset;
    ulong    squeeze_log_length;
    ulong    squeeze_buffer_offset;
    ulong    squeeze_buffer_length;
    char     volume_id[FS_VOLUME_ID_MAX];
    ulong    num_spare_sectors;
    struct {
        ushort bad_sector_num;
        ushort spare_sector_num;
    } spare[FS_SPARE_SECTOR_MAX];
    ulong    unused[22];
} fs_device_info_block_t;

#endif  /* __DIB_H__ */
