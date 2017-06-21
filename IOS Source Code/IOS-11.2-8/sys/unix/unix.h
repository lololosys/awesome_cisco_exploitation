/* $Id: unix.h,v 3.2 1995/11/17 17:55:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/unix/unix.h,v $
 *------------------------------------------------------------------
 * unix.h -- cisco on UNIX -- prototypes and definitions
 *
 * 30-December-1991, Kirk Lougheed
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix.h,v $
 * Revision 3.2  1995/11/17  17:55:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:42:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:11:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * cisco type definitions
 * Needed only by unix_io.c since it doesn't use cisco include files
 */
#ifndef __TTYSRV_H__
#define boolean int
#define tt_soc long
#define forktype void

#define TRUE 1
#define FALSE 0
#endif

/*
 * cisco jackets for UNIX system calls
 */
extern boolean unix_stdinBLOCK(void);
extern void unix_exit(int);
extern void unix_putc(int);
extern void unix_setcooked(void);
extern void unix_setraw(void);
extern int unix_brk(int);
extern int unix_stdinread(char *, int);
extern void serial_setbuffers(tt_soc *,int,int);
forktype unix_console(tt_soc *);
void unix_console_putc(tt_soc *, int);
extern boolean unix_write_file(char *, unsigned char *, long);
extern boolean unix_read_file(char *, unsigned char *, long *, long);
extern void unix_remove_file(char *);
extern int unix_read(int, char*, unsigned int);
