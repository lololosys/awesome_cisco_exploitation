/* $Id: rommon_filesys.h,v 3.2 1995/11/17 09:08:38 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/rommon_filesys.h,v $
 *------------------------------------------------------------------
 * Cut out from filesys.h the stuff that are shared with rom monitor
 *
 * May 1995, Yan Ke
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rommon_filesys.h,v $
 * Revision 3.2  1995/11/17  09:08:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:57:26  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:32:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__rommon_filesys_h__)
#define __rommon_filesys_h__

/*----------------------------------------------------------------------------*
 * Constants for file names
 *----------------------------------------------------------------------------*/
#define FS_DEV_ID_MAX    31
#define FS_FILE_NAME_MAX 63
#define MAXNAMELEN       FS_FILE_NAME_MAX
#define FS_FILE_ID_MAX   (FS_DEV_ID_MAX + 1 + FS_FILE_NAME_MAX)
#define MONLIB_TYPE_FS   1

#define FSLIB_DEV_ID_MAX    FS_DEV_ID_MAX
#define FSLIB_FILE_NAME_MAX FS_FILE_NAME_MAX
#define FSLIB_FILE_ID_MAX   FS_FILE_ID_MAX

/*----------------------------------------------------------------------------*
 * Typedef's to make this compatible with Unix.
 *----------------------------------------------------------------------------*/
typedef ulong   ino_t;
typedef long    off_t;
typedef ushort  uid_t;
typedef ushort  gid_t;

#if !defined(STANDALONE)
typedef ulong   mode_t;
typedef ulong   time_t;
typedef ulong   nlink_t;
#endif

/*----------------------------------------------------------------------------*
 * File Status structure (used by stat() fstat() and istat())
 *
 * Do not change the size of the structure without corresponding
 * rom monitor change. New stuff can be added in unused block.
 *
 *----------------------------------------------------------------------------*/
typedef struct stat_ {
    /* standard Unix fields */
    ino_t     st_ino;
    mode_t    st_mode;
    nlink_t   st_nlink;
    uid_t     st_uid;
    gid_t     st_gid;
    off_t     st_size;
    time_t    st_atime;
    time_t    st_mtime;
    time_t    st_ctime;
    long      st_blksize;
    long      st_blocks;
    /* special cisco fields */
    ulong     namlen;
    char      name[MAXNAMELEN + 1];
    ulong     type;
    ulong     flags;
    ulong     crc;
    uchar     valid;
    uchar     deleted;
    int       devnum;			/* the device that the file lives on */
    int       fstype;			/* the file system type (i.e. FS_SIMP) */
	void      *server_addr;		/* server address in case of remote device */
    /* other stuff to be added at a later date */
    uchar     unused[32];
} stat_t;

/*----------------------------------------------------------------------------*
 * File Directory Entry structure (used by getdents())
 *
 * Do not change the alrady defined fields without corresponding
 * change of the rom monitor. New stuff can be added in unused
 * block and beyond.
 *----------------------------------------------------------------------------*/
typedef struct dirent_ {
    /* standard Unix fields */
    ulong     d_reclen;
    off_t     d_off;
    ulong     d_fileno;
    ulong     d_namlen;
    char      d_name[MAXNAMELEN + 1];
    /* special cisco fields */
    ulong     size;
    ulong     type;
    ulong     time;
    ulong     crc;
    uchar     valid;
    uchar     deleted;
    int       fileno;		/* unique file number in file system */
    ulong     achecksum;	/* actual checksum */
    ushort    flags;		/* file flags */
    int       filetype;		/* distinguish between simple fs and new fs */
    uchar     unused[10];
} dirent_t;

#endif /* !defined(__rommon_filesys_h__) */
