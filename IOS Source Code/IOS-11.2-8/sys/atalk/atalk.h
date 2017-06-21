/* $Id: atalk.h,v 3.2.58.1 1996/05/30 23:42:05 hampton Exp $
 * $Source: /release/112/cvs/Xsys/atalk/atalk.h,v $
 *------------------------------------------------------------------
 * atalk.h -- definitions for Appletalk protocol family
 *
 * August 1988, Bill Palmer
 * 
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atalk.h,v $
 * Revision 3.2.58.1  1996/05/30  23:42:05  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:44:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:09:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ATALK_H__
#define __ATALK_H__

#define AARPBYTES 28			/* AARP pkt is 28 bytes (for ether) */

#define AT_MIN_ACL 1
#define AT_MAX_ACL 100

extern const uchar apple_snap[];

#endif __ATALK_H__
