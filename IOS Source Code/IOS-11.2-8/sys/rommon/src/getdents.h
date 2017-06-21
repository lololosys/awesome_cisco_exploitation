/* $Id: getdents.h,v 3.2 1995/11/17 18:41:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/getdents.h,v $
 *------------------------------------------------------------------
 * getdents.h
 *
 * October 1993, Rob Clevenger
 *
 * Copyright (c) 1994 - 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines for the getdents() routine.
 *------------------------------------------------------------------
 * $Log: getdents.h,v $
 * Revision 3.2  1995/11/17  18:41:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __GETDENTS_H__
#define __GETDENTS_H__

/*
** Structure for the getdents() routine
*/

/* added const to distinguish between file system type */
#define SIMPLE_FS_TYPE 0
#define MONLIB_FS_TYPE 1

#include "../../filesys/rommon_filesys.h"

extern int getdents(int fd, struct dirent_ *dptr, int dirent_len);

#endif  /* __GETDENTS_H__ */
