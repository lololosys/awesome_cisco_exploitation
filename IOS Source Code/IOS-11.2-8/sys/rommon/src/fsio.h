/* $Id: fsio.h,v 3.3 1996/01/17 23:31:17 mansonw Exp $

/* $Id: fsio.h,v 3.3 1996/01/17 23:31:17 mansonw Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/rommon/src/fsio.h,v $
 *------------------------------------------------------------------
 * fsio.h
 *
 * August 1993, Rob Clevenger
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * General file system I/O defines.
 *------------------------------------------------------------------
 * $Log: fsio.h,v $
 * Revision 3.3  1996/01/17  23:31:17  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:41:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:23:48  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:37:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __FSIO_H__
#define __FSIO_H__

/*
** General file system I/O defines.
*/
/* defines for lseek */
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

/* Defines for open - only O_RDONLY is supported */
#define O_RDONLY 1

/* magic numbers */
#define OMAGIC         0x0107    /* a_magic - old impure format */
#define COMPRESSMAGIC  0x1f9d    /* 1st word of compressed file */

/* Sun machine types */
#define M_OLDSUN2       0       /* old sun-2 executable files */
#define M_68010         1       /* runs on either 68010 or 68020 */
#define M_68020         2       /* runs only on 68020 */

/* cisco extensions to be applied to a_machtype */
#define HIST_MACHTYPE_MASK  0xff;  /* mask for historical machtype */
  /* cisco cpu type is in the upper 8 bits */
#define CISCO_CPUTYPE(x)   ((x >> 8) & 0xff)

struct cisco_exec {
        unsigned short  a_machtype;     /* machine type */
        unsigned short  a_magic;        /* magic number */
        unsigned long   a_text;         /* size of text segment */
        unsigned long   a_data;         /* size of initialized data */
        unsigned long   a_bss;          /* size of uninitialized data */
        unsigned long   a_syms;         /* size of symbol table */
        unsigned long   a_entry;        /* entry point */
        unsigned long   a_trsize;       /* size of text relocation */
        unsigned long   a_drsize;       /* size of data relocation */
};

#define CISCOEXEC_HDRSIZ (sizeof(struct cisco_exec))

/* related prototypes */
#ifndef UNIX
extern int open(char *path, int mode);
extern int ismagic(unsigned long *ptr);
extern int close(int fd);
extern void closeall();
extern int read(int fd, char *bufptr, int numbytes);
extern int write(int fd, char *bufptr, int numbytes);
extern int lseek(int fd, long offset, int whence);
extern int raw_read(int devnum, char *src, char *bufptr, int numbytes);
extern int raw_write(int devnum, char *bufptr, char *dest, int numbytes);
extern int raw_stat(int devnum);
extern int raw_ctl(int devnum);
extern int raw_yield(int devnum);
extern int raw_time(int devnum);
#endif

#endif  /* __FSIO_H__ */
