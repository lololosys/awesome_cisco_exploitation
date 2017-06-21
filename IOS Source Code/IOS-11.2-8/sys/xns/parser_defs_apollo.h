/*
 *	P A R S E R _ D E F S _ A P O L L O . H
 *
 * $Id: parser_defs_apollo.h,v 3.1.40.1 1996/05/30 23:49:39 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/parser_defs_apollo.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_apollo.h,v $
 * Revision 3.1.40.1  1996/05/30  23:49:39  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/01  05:14:45  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/29  07:00:41  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:49:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:28:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Apollo command types
 */
#define APOLLO_ACCESSL		 0
#define APOLLO_ROUTE		(APOLLO_ACCESSL+1)
#define APOLLO_ROUTING		(APOLLO_ROUTE+1)
#define APOLLO_PATHS		(APOLLO_ROUTING+1)

#define APOLLOIF_ACC		 0
#define APOLLOIF_NETWORK	 1
#define APOLLOIF_UPDATETIME	 2

enum {
    SHOW_APOLLO_ACCESS,
    SHOW_APOLLO_ARP,
    SHOW_APOLLO_INTERFACE,
    SHOW_APOLLO_ROUTE,
    SHOW_APOLLO_TRAFFIC,
};
