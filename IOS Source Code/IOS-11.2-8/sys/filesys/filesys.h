/* $Id: filesys.h,v 3.2 1995/11/17 09:07:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/filesys.h,v $
 *------------------------------------------------------------------
 * Include file for the flash file system
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: filesys.h,v $
 * Revision 3.2  1995/11/17  09:07:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:56  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:31:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__filesys_h__)
#define __filesys_h__

#if defined(STANDALONE)
#include <stdlib.h>
#include <stdio.h>
#endif

/*----------------------------------------------------------------------------*
 * Constants for error buffer
 *----------------------------------------------------------------------------*/
#define FS_ERR_BUF_MAX		80

/*----------------------------------------------------------------------------*
 * Constants for device name buffer
 *----------------------------------------------------------------------------*/
#define FS_DEV_NAMES_MAX	128

#include "rommon_filesys.h"

/*----------------------------------------------------------------------------*
 * Device Information Header
 *----------------------------------------------------------------------------*/
#define FSLIB_VOLUME_ID_MAX  32
#define FSLIB_DEVICE_INFO_BLOCK_MAGIC  0x06887635
#define FSLIB_SPARE_SECTOR_MAX  16

typedef struct fslib_device_info_block_t
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
    char     volume_id[FSLIB_VOLUME_ID_MAX];
    ulong    num_spare_sectors;
    struct {
	ushort bad_sector_num;
	ushort spare_sector_num;
    } spare[FSLIB_SPARE_SECTOR_MAX];
    ulong    unused[22];
} fslib_device_info_block_t;

/*----------------------------------------------------------------------------*
 * File System Status structure (used by fsstat())
 *----------------------------------------------------------------------------*/
typedef struct file_sys_stat_ {
    /* current status information */
    int    writable;
    int    file_open_for_write;
    int    incomplete_stats;
    int    unrecovered_errors;
    int    squeeze_in_progress;

    /* usage information */
    ulong  files_ok;
    ulong  files_ok_bytes;
    ulong  files_deleted;
    ulong  files_deleted_bytes;
    ulong  files_with_errors;
    ulong  files_with_errors_bytes;
    ulong  bytes_used;
    ulong  bytes_available;
    ulong  bad_sectors;
    ulong  spared_sectors;

    /* static information */
    int    dev_num;
    ulong  file_hdr_size;

    /* device information -- it is all in the device info block */
    fslib_device_info_block_t  dib;

} file_sys_stat_t;

/*----------------------------------------------------------------------------*
 * External references to functions provided by file system
 *----------------------------------------------------------------------------*/
extern void FS_init (void);
extern int FS_open (const char *file_id, int flags, ulong mode);
extern int FS_read (int fd, void *buffer, int byte_count);
extern int FS_close (int fd);
extern int FS_lseek (int fd, int offset, int whence);
extern int FS_write (int fd, void *buffer, int byte_count);
extern int FS_fstat (int fd, stat_t *status);
extern int FS_stat (const char *file_id, stat_t *status);
extern int FS_istat (const char *dev_id, int index, stat_t *status);
extern int FS_fsstat (const char *dev_id, file_sys_stat_t *status);
extern int FS_getdents (int fd, char *buffer, int buffer_length);
extern int FS_delete (const char *file_id);
extern int FS_undelete (const char *dev_id, int index, int mode);
extern int FS_squeeze (const char *dev_id, int flag);
extern int FS_settype (const char *file_id, ulong file_type);
extern void FS_geterrortext (int error_code, char *buffer, int buffer_len);
extern int FS_getdevicenames (char *buffer, int buffer_len);
extern int FS_ioctl (const char *file_id, int func, void *arg);
extern int FS_register (const char *device, int err_range);

extern void fileid2filename (const char *file_id, char *file_name, int len);
extern void fileid2devid (const char *file_id, char *dev_id, int len);
/*
 * The following two routines will be replaced by above two.
 */
extern const char *get_file_name_by_file_id (const char *);
extern const char *get_dev_id_by_file_id (const char *);

extern int RFSS_init(boolean);

/*----------------------------------------------------------------------------*
 * Definitions for the "func" and "arg" parameters to ioctl/fioctl()
 *----------------------------------------------------------------------------*/

#define IOCTL_SET_FILE_SIZE	1	/* set file size */
#define IOCTL_SET_FILE_TYPE	2	/* set file type */
#define IOCTL_GET_DEV_TYPE	3	/* get device type */
#define	IOCTL_FORMAT_FLASH	4	/* format device */
#define	IOCTL_PRINT_CHIP	5	/* print chip info */
#define	IOCTL_SET_VERBOSE	6	/* set verbose flag */
#define IOCTL_SET_PRIVATE_RW	7	/* set private file access ok flag */
#define IOCTL_GET_BUSY_INFO	8	/* get busy information */

#define IOCTL_FILE_TYPE_CONFIG	1	/* config file type */
#define IOCTL_FILE_TYPE_IMAGE	2	/* image file type */

#define IOCTL_DEV_TYPE_FLASH		0x0001	/* flash device */
#define IOCTL_DEV_TYPE_NV		0x0002	/* nvram device */
#define IOCTL_DEV_TYPE_TFTP		0x0004	/* tftp device */
#define IOCTL_DEV_TYPE_RCP		0x0008	/* rcp device */
#define IOCTL_DEV_TYPE_LOCAL		0x0010	/* local device */
#define IOCTL_DEV_TYPE_REMOVABLE	0x0020	/* removable device */

#define	IOCTL_FORMAT_QUIET	0x0001	/* do not print if set */

#define	IOCTL_FORMAT_VOLUME_MAX	64	/* maximum length of volume id */

typedef struct format_parm_ {
    uint flag;			/* control flags */
    int spare;			/* number of spare sectors */
    char *monlib;		/* file-id/dev-id of monlib image */
    char *monlib_bndl;		/* start of bundled monlib */
    int monlib_bndl_size;	/* bundled monlib size */
    char volume_id[IOCTL_FORMAT_VOLUME_MAX + 1];
} format_parm_t;

/*----------------------------------------------------------------------------*
 * Defines for the "flags" parameter to open()
 *----------------------------------------------------------------------------*/
#define O_RDONLY  0x01
#define O_WRONLY  0x02
#define O_RDWR    (O_RDONLY | O_WRONLY)
#define O_CREAT   0x04
#define O_APPEND  0x08
#define O_TRUNC   0x10
#define O_RW_MASK 0xff

#define O_VERBOSE_MASK  0x100
#define O_VERBOSE_OFF   0x100	/* turn off verbose */
#define O_VERBOSE_ON    0x000
#define O_CRC_MASK      0x200
#define O_CRC_OFF       0x200	/* turn off crc checking */
#define O_CRC_ON        0x000

/*----------------------------------------------------------------------------*
 * Defines for the "wherefrom" parameter to seek()
 *----------------------------------------------------------------------------*/
#define SEEK_SET  0
#define SEEK_CUR  1
#define SEEK_END  2

/*----------------------------------------------------------------------------*
 * Defines for the "mode" parameter to undelete()
 *----------------------------------------------------------------------------*/
#define UNDELETE_AUTO    0
#define UNDELETE_UNIQUE  1

/*----------------------------------------------------------------------------*
 * Defines for the "busy_p" parameter to get_busy_info()
 *----------------------------------------------------------------------------*/
#define FS_BUSY_IDLE	 0
#define FS_BUSY_SQUEEZE	 1
#define FS_BUSY_MAX      1	/* keep this variable up to date if you
				   add to this table */

/*----------------------------------------------------------------------------*
 * Defines for the "flag" parameter to squeeze()
 *----------------------------------------------------------------------------*/
#define SQUEEZE_NORMAL    0
#define SQUEEZE_NO_LOG  0x0001
#define SQUEEZE_QUIET	0x0002

/*
 * Value for file types.
 * XXX - those should now be replaced by IOCTL_...
 */
#define	FILE_TYPE_SYS_CONFIG	1		/* config file type */
#define	FILE_TYPE_SYS_IMAGE	2		/* image file type */
#define FILE_TYPE_SYS_UNKNOWN	0xffffffff	/* same as erase value */


/*----------------------------------------------------------------------------*
 * 
 *----------------------------------------------------------------------------*/
#define FS_ERROR_CODE_GRANULARITY    1000
#define FS_STD_ERROR_CODE_ALLOCATION FS_ERROR_CODE_GRANULARITY


/*----------------------------------------------------------------------------*
 * File system error codes
 *----------------------------------------------------------------------------*/
#define FS_OK                                          0
#define FS_ERROR                                      -1
#define FS_ERROR_TRUNCATED			      -2
#define FS_ERROR_UNKNOWN_DEVICE                       -3

#endif /* !defined(__filesys_h__) */
