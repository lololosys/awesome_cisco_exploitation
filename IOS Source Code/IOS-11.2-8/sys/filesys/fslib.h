/* $Id: fslib.h,v 3.2 1995/11/17 09:08:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/fslib.h,v $
 *------------------------------------------------------------------
 * Include file for the interface to the FSLIB used during booting
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fslib.h,v $
 * Revision 3.2  1995/11/17  09:08:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:57:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__fslib_h__)
#define __fslib_h__

/*----------------------------------------------------------------------------*
 * Device Information Header
 *----------------------------------------------------------------------------*/
#define FSLIB_VOLUME_ID_MAX  32
#define FSLIB_DEVICE_INFO_BLOCK_MAGIC  0x06887635

/*----------------------------------------------------------------------------*
 * File system vector table
 *----------------------------------------------------------------------------*/
typedef struct fslib_vector_table_t {
    ulong version;
    ulong min_fslib_version;
    ulong max_fslib_version;
    struct fslib_vector_table_t  *(*init)(ralib_vector_table_t *ralib_table);
    int   (*open)(const char *file_id, int flags, ulong attrib);
    int   (*close)(int fd);
    int   (*read)(int fd, void *buffer, int byte_count);
    int   (*write)(int fd, void *buffer, int byte_count);
    int   (*lseek)(int fd, int offset, int wherefrom);
    int   (*delete)(const char *file_id);
    int   (*undelete)(const char *dev_id, int index, int mode);
    int   (*squeeze)(const char *dev_id, int flag);
    int   (*stat)(const char *file_id, stat_t *status);
    int   (*fstat)(int fd, stat_t *status);
    int   (*istat)(const char *dev_id, int index, stat_t *status);
    int   (*fsstat)(const char *dev_id, file_sys_stat_t *status);
    int   (*settype)(const char *file_id, ulong file_type);
    int   (*getdents)(int fd, char *buffer_ptr, int buffer_len);
} fslib_vector_table_t;

/*----------------------------------------------------------------------------*
 * External references to functions provided by FSLIB
 *----------------------------------------------------------------------------*/
extern fslib_vector_table_t *FSLIB_init (ralib_vector_table_t *ralib_table);
extern int FSLIB_open (const char *file_id, int flags, ulong mode);
extern int FSLIB_read (int fd, void *buffer, int byte_count);
extern int FSLIB_close (int fd);
extern int FSLIB_lseek (int fd, int offset, int whence);
extern int FSLIB_write (int fd, void *buffer, int byte_count);
extern int FSLIB_fstat (int fd, stat_t *status);
extern int FSLIB_stat (const char *file_id, stat_t *status);
extern int FSLIB_istat (const char *dev_id, int index, stat_t *status);
extern int FSLIB_fsstat (const char *dev_id, file_sys_stat_t *status);
extern int FSLIB_getdents (int fd, char *buffer, int buffer_length);
extern int FSLIB_delete (const char *file_id);
extern int FSLIB_undelete (const char *dev_id, int index, int mode);
extern int FSLIB_squeeze (const char *dev_id, int flag);
extern int FSLIB_settype (const char *file_id, ulong file_type);
extern void FSLIB_geterrortext (int error, char *buffer, int buffer_len);
extern int FSLIB_getdevicenames (char *buffer, int buffer_len);
extern int FSLIB_ioctl (const char *file_id, int func, void *arg);
extern int FSLIB_fioctl (int fd, int func, void *arg);
/*----------------------------------------------------------------------------*
 * Misc utility functions provided by FSLIB
 *----------------------------------------------------------------------------*/
extern int FSLIB_build_dib(fslib_device_info_block_t *dib,
			   ulong sector_size,
			   ulong num_sectors,
			   ulong num_spare_sectors,
			   ulong monlib_length,
			   ulong prog_algorithm,
			   ulong erased_state,
			   const char *volume_id);


#endif /* !defined(__fslib_h__) */

