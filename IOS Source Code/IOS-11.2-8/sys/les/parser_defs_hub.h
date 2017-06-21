/* $Id: parser_defs_hub.h,v 3.1 1996/02/01 04:58:52 hampton Exp $
 * $Source: /release/111/cvs/Xsys/les/parser_defs_hub.h,v $
 *------------------------------------------------------------------
 * Hub parser definitions
 *
 * March 1994, Robert Widmer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Gurus: Steve Garcia, Viet Nguyen
 *------------------------------------------------------------------
 * $Log: parser_defs_hub.h,v $
 * Revision 3.1  1996/02/01  04:58:52  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:50:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:20:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:30:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

enum HUB {
    HUB_UNSPECIFIED = 0,
    HUB_ETHER
};

/* HUB parameters */
enum HUB_PARAMS {
    HUB_INVALID = 0,
    HUB_CONFIG,
    HUB_SHUTDOWN,
    HUB_LINK_TEST,
    HUB_AUTOMATIC_POLARITY,
    HUB_SOURCE_ADDRESS,
    HUB_SNMP_TRAP
};
