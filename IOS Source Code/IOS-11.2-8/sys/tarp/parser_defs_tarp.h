/* $Id: parser_defs_tarp.h,v 3.1.36.2 1996/05/17 21:03:25 asastry Exp $
 * $Source: /release/112/cvs/Xsys/tarp/parser_defs_tarp.h,v $
 *------------------------------------------------------------------
 * TARP (Target ID address Resolution Protocol) parser support
 *
 * October 1995, Arun Sastry
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_tarp.h,v $
 * Revision 3.1.36.2  1996/05/17  21:03:25  asastry
 * CSCdi57772:  Missed out a line in the log; add it.
 * Branch: California_branch
 *
 * Revision 3.1.36.1  1996/05/17  20:55:37  asastry
 * CSCdi57772:  Fix misc. TARP bugs found during interoperability testing
 *
 *               - Don't change NSEL during TARP packet propagation
 *               - Implement loop detection as per amended TARP spec
 *               - Don't propagate TARP packet if TTL is zero (after
 *                 decrement)
 *
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/01  04:59:22  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/29  07:01:04  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:53:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:20:50  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/11  02:12:19  asastry
 * Placeholder file for TARP
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Global configuration commands for TARP
 */
enum {
      TARP_RUNNING, TARP_LDB_TIMER, TARP_TID_TIMER, TARP_T1_RESP_TIMER, 
      TARP_T2_RESP_TIMER, TARP_LDB_ZERO_SEQ_TIMER, TARP_ARP_TIMER, 
      TARP_POST_TYPE2_TIMER, TARP_ENABLE, TARP_PROPAGATE_CMD, 
      TARP_ORIGINATE_CMD, TARP_DISABLE_CACHE_CMD,
      TARP_BLACKLIST_CMD, TARP_MAP, TARP_SEQUENCE, TARP_URC,
      TARP_PROTOCOL_TYPE, TARP_NSEL_TYPE, TARP_LIFETIME, TARP_TID,
      TARP_STATIC_ADJ_CMD
};

/*
 * TARP exec commands
 */
enum {
    CMD_TARP_RESOLVE,
    CMD_TARP_QUERY,
};

/*
 * TARP show commands
 */
enum {
    SHOW_TARP_GLOBAL,
    SHOW_TARP_INTERFACE,
    SHOW_TARP_COUNTERS,
    SHOW_TARP_TID_CACHE,
    SHOW_TARP_LDB_CACHE,
    SHOW_TARP_BLACKLIST_ADJ,
    SHOW_TARP_STATIC_ADJ,
    SHOW_TARP_STATIC_MAP,
    SHOW_TARP_HOST,
};
