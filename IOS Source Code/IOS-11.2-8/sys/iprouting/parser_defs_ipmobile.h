/* $Id: parser_defs_ipmobile.h,v 3.1 1996/02/01 04:58:44 hampton Exp $
 * $Source: /release/111/cvs/Xsys/iprouting/parser_defs_ipmobile.h,v $
 *------------------------------------------------------------------
 * parser_defs_ipmobile.h -- Parser definitions for IP mobility
 *
 * Oct 1993 Tony Li
 *
 * Copyright (c) 1994,1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_ipmobile.h,v $
 * Revision 3.1  1996/02/01  04:58:44  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:51:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:30:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define MOBILE_CMD_ARP		1
#define MOBILE_CMD_PROXY	2
#define MOBILE_CMD_BEACONING	3
#define MOBILE_CMD_FOREIGN	4
#define MOBILE_CMD_HOME		5
#define MOBILE_CMD_NODE		6

#define MOBILE_PASS_CLEAR	1
#define MOBILE_PASS_MD5		2
