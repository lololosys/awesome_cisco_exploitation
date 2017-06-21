/* $Id: fstat.h,v 3.2 1995/11/17 18:41:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/fstat.h,v $
 *------------------------------------------------------------------
 * fstat.h
 *
 * October 1993, Rob Clevenger
 *
 * Copyright (c) 1994 - 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The structure and defines for the stat and
 * fstat system calls.
 *------------------------------------------------------------------
 * $Log: fstat.h,v $
 * Revision 3.2  1995/11/17  18:41:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __FSTAT_H__
#define __FSTAT_H__

/*
** The structure and defines for the stat and
** fstat system calls.
*/

#include "../../filesys/rommon_filesys.h"


/* related prototypes */
extern int stat(char *path, struct stat_ *statptr);
extern int fstat(int fd, struct stat_ *statptr);

#endif   /* __FSTAT_H__ */
