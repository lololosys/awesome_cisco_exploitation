/* $Id: parser_defs_community.h,v 3.1 1996/02/01 04:58:40 hampton Exp $
 * $Source: /release/111/cvs/Xsys/iprouting/parser_defs_community.h,v $
 *------------------------------------------------------------------
 * parser_defs_community.h -- community list related parser defines.
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_community.h,v $
 * Revision 3.1  1996/02/01  04:58:40  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:49:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:29:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

enum {
    PARSE_COMMUNITYLIST_INIT = 1,
    PARSE_COMMUNITYLIST_ENTRY,
    PARSE_COMMUNITYLIST_END,
    PARSE_COMMUNITYLIST_REXP
};
