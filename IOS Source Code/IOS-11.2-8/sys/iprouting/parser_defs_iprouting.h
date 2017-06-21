/*
 * $Id: parser_defs_iprouting.h,v 3.1 1996/02/01 04:58:45 hampton Exp $
 * $Source: /release/111/cvs/Xsys/iprouting/parser_defs_iprouting.h,v $
 *
 * parser_defs_iprouting.h
 *
 * Paul Traina, August 1994
 * 
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_iprouting.h,v $
 * Revision 3.1  1996/02/01  04:58:45  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/29  07:00:56  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:51:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:30:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/* iprouting_command defines */

#define IPR_SLOW_CONVERGE	0
#define	IPR_CLASSLESS		1

#define	IPCACHE_AGER		0
#define	IPCACHE_INVALIDATE	1

#define CLEAR_IP_CACHE          0
#define CLEAR_IP_ROUTE          1
