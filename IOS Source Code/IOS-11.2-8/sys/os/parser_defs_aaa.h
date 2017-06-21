/* $Id: parser_defs_aaa.h,v 3.1.40.2 1996/06/26 22:44:13 billw Exp $
 * $Source: /release/112/cvs/Xsys/os/parser_defs_aaa.h,v $
 *------------------------------------------------------------------
 * parser_cfg_aaa.h -- Parser defines for AAA authentication
 *
 * September 1994, Dave Carrel
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_aaa.h,v $
 * Revision 3.1.40.2  1996/06/26  22:44:13  billw
 * CSCdi59376:  AAA local prompts not configurable
 * Branch: California_branch
 *
 * Revision 3.1.40.1  1996/04/19  15:24:45  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.58.1  1996/04/04  19:14:51  carrel
 * cvs fixes
 *
 * Revision 3.1.16.1  1996/03/05  04:44:02  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.1  1996/02/01  04:59:00  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:48:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:20:30  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:28:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define PARSER_AUTHEN_NEW_MODEL		1
#define PARSER_AUTHEN_LISTS		2
#define PARSER_AUTHEN_LOCAL_O		3
#define PARSER_AUTHEN_PROMPTS		4

#define PARSER_ACCT_SUPPRESS_NULL_USER	8
#define PARSER_ACCT_NESTED		9

enum prompt_types {AAA_PROMPT_PASSWORD, AAA_PROMPT_USER};

#define PARSER_AUTHOR_LIST		100
#define PARSER_AUTHOR_CONFIG_CMD        101

#define PARSER_ACCT_LIST		200

#define PARSED_METH_NONE	GETOBJ(int,22)
#define PARSED_METH_KRB5	GETOBJ(int,21)
#define PARSED_METH_LINE	GETOBJ(int,20)
#define PARSED_METH_ENABLE	GETOBJ(int,19)
#define PARSED_METH_LOCAL	GETOBJ(int,18)
#define PARSED_METH_TACPLUS	GETOBJ(int,17)
#define PARSED_METH_IFNEEDED	GETOBJ(int,16)
#define PARSED_METH_GUEST	GETOBJ(int,15)
#define PARSED_METH_AUTHGUEST	GETOBJ(int,14)
#define PARSED_METH_RADIUS	GETOBJ(int,13)
#define PARSED_METH_RCMD	GETOBJ(int,12)
#define PARSED_METH_KRB5_TELNET	GETOBJ(int,11)
