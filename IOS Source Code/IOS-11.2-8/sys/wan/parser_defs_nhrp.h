/* $Id: parser_defs_nhrp.h,v 3.1 1996/02/01 05:14:34 hampton Exp $
 * $Source: /release/111/cvs/Xsys/wan/parser_defs_nhrp.h,v $
 *------------------------------------------------------------------
 * Next Hop Routing Protocol.
 *
 * September 1994, Bruce Cole
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_nhrp.h,v $
 * Revision 3.1  1996/02/01  05:14:34  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:51:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:20:44  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:30:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Interface specific command codes
 */
#define NHRP_CMD_NET	0
#define NHRP_CMD_MAP	1
#define NHRP_CMD_NHS	2
#define NHRP_CMD_INTEREST 3
#define NHRP_CMD_HOLD	4
#define NHRP_CMD_RECORD 5
#define NHRP_CMD_AUTH	6
#define NHRP_CMD_RESP	7
#define NHRP_CMD_USE	8
#define NHRP_CMD_MAX	9

/*
 * Show command codes
 */
enum {
    NHRP_SHOW_CACHE,
    NHRP_SHOW_TRAFFIC,
    NHRP_SHOW_PURGE
};

/*
 * Arguments to show cache command
 */
#define NHRP_SHOW_DYN	1
#define NHRP_SHOW_STAT	2
#define NHRP_SHOW_INC	3
