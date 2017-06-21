/* $Id: parser_defs_cdp.h,v 3.1 1996/02/01 04:57:20 hampton Exp $
 * $Source: /release/111/cvs/Xsys/cdp/parser_defs_cdp.h,v $
 *------------------------------------------------------------------
 * Definitions of constants used in CDP parse chains
 *
 * October 1994, Arun Sastry
 *
 * Copyright (c) 1994,1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: parser_defs_cdp.h,v $
 * Revision 3.1  1996/02/01  04:57:20  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/29  07:00:49  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:49:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:29:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CDP_RUNNING                0
#define CDP_CONFIG_TIMER           (CDP_RUNNING + 1)
#define CDP_CONFIG_HOLDTIME        (CDP_CONFIG_TIMER + 1)
#define CDP_ENABLE                 (CDP_CONFIG_HOLDTIME + 1)

/* CDP show commands */
enum {
    SHOW_CDP_GLOBAL,
    SHOW_CDP_COUNTERS,
    SHOW_CDP_HOST,
    SHOW_CDP_INTERFACE,
    SHOW_CDP_NEIGHBORS,
};
