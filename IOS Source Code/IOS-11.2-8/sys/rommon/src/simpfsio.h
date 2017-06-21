/* $Id: simpfsio.h,v 3.2.58.2 1996/06/16 21:18:29 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/simpfsio.h,v $
 *------------------------------------------------------------------
 * simpfsio.h
 *
 * February 1994, Rob Clevenger
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Support for the simple file system routines.
 *------------------------------------------------------------------
 * $Log: simpfsio.h,v $
 * Revision 3.2.58.2  1996/06/16  21:18:29  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2.58.1  1996/03/21  23:28:01  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:21:20  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:43:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __SIMPFSIO_H__
#define __SIMPFSIO_H__

/*
** Support for the file system routines.
** This is the structure used in the open file table.
** A flag field of zero means that the structure is unused.
*/

struct simp_fsio {
    int devnum;       /* the device number */
    char *start;      /* start location of file */
    char *fptr;       /* current location in file */
    long size;        /* file size */
    char *end;        /* end of file */
    int mode;         /* mode file was opened with */
    int flags;        /* housekeeping flags for this table */
    int fsfd;         /* the index of this entry */
};

#define MAXFSFD 6

extern struct simp_fsio sfsiotbl[MAXFSFD];

/* defines for file flags */
#define SFS_UNCLAIMED   0x0   /* table entry is availlable */
#define SFS_UNKNOWN     0x1   /* file is undefined */
#define SFS_SIMP        0x2   /* simple file system */

/*
** Defines for the simple file header
*/

#include "../../les/flash_fs_les.h"

#ifndef UNIX
extern int sfs_open(char *path, int mode, int devnum, int offset, long devsiz);
extern int sfs_close(int devnum);
extern int sfs_read(int devnum, unsigned char *bufptr, int numbytes);
extern int sfs_write(int devnum, unsigned char *bufptr, int numbytes);
extern int sfs_lseek(int devnum, long offset, int whence);
extern int sfs_fstat(int devnum, struct stat_ *statptr);
extern int sfs_getdents(int devnum, struct dirent_ *dptr, int dirent_len);
extern unsigned short fchksum_body(unsigned long chksum, unsigned short *ptr, 
                                  int size);
#endif

#endif  /* __SIMPFSIO_H__ */
