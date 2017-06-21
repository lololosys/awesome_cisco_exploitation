/* $Id: monfsio.h,v 3.2 1995/11/17 18:42:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/monfsio.h,v $
 *------------------------------------------------------------------
 * monfsio.h
 *
 * November 1993, Rob Clevenger
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Monitor file system I/O defines.
 *------------------------------------------------------------------
 * $Log: monfsio.h,v $
 * Revision 3.2  1995/11/17  18:42:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __MONFSIO_H__
#define __MONFSIO_H__

/*
** Structures and defines for file system I/O in the monitor.
*/

struct mon_fsio {
    int usrfd;        /* the file descriptor returned to the user */
    int fsfd;         /* the file descriptor used by fslib */
    int mode;         /* the mode the file was opened with (O_RDONLY) */
    int devnum;       /* the device number */
    int flags;        /* housekeeping flags for this table */
    char *start;      /* logical start of the file (not always zero) */
    char *rfptr;      /* raw (logical) file pointer for raw I/O */
    long size;        /* the size in bytes */
};

#define MAXOPENFILE 6

extern struct mon_fsio openfiles[MAXOPENFILE];
extern unsigned long monlib_upper_bound;

/* defines for file flags */
#define MFS_RAW       0x1   /* file is a raw device */
#define MFS_FS        0x2   /* file is in a file system */
#define MFS_NET       0x4   /* netbooted file */

#endif  /* __MONFSIO_H__ */
