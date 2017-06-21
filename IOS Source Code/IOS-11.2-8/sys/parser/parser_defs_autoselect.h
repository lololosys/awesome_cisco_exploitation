/* $Id: parser_defs_autoselect.h,v 3.4 1996/02/23 20:12:01 billw Exp $
 * $Source: /release/111/cvs/Xsys/parser/parser_defs_autoselect.h,v $
 *------------------------------------------------------------------
 * Parser definitions for autoselect
 *
 * June 1994, Tim Kolar
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: parser_defs_autoselect.h,v $
 * Revision 3.4  1996/02/23  20:12:01  billw
 * CSCdi49461:  autoselect during-login causes premature authorization
 * Add comments rto go with p2.
 *
 * Revision 3.3  1996/02/23  12:13:16  irfan
 * CSCdi49461:  autoselect during-login causes premature authorization
 * Add the flag as needed by last commit.
 *
 * Revision 3.2  1995/11/17  18:49:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/23  18:49:09  eschaffe
 * CSCdi37678:  cant use per user access lists on vtyasync translate w/o
 * login
 *
 * Revision 2.1  1995/06/07  22:28:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 *
 * 
 */

#define PARSER_AUTOSELECT_ARAP          0x00000001
#define PARSER_AUTOSELECT_SLIP          0x00000002
#define PARSER_AUTOSELECT_PPP           0x00000004
#define PARSER_AUTOSELECT_DURING_LOGIN  0x00000008
#define AUTOSELECTING                   0x00000010
#define AUTOSELECTED                    0x00000020
#define PT_ONESTEP_CONNECTION           0x00000040
/*
 * AUTOSELECT_AUTHORIZE is sticky, in that it survives a process going
 * through death().  It's used so that authorization can pass on an
 * autocommand that might otherwise fail authorization.  It's set at
 * the same time AUTOSELECTED is set, but is not cleared until the next
 * time the autoselect process runs, or until autoselect is turned off.
 */
#define AUTOSELECT_AUTHORIZE		0x00000080
#define PARSER_AUTOSELECT_ALL  (PARSER_AUTOSELECT_ARAP | \
				PARSER_AUTOSELECT_SLIP | \
				PARSER_AUTOSELECT_PPP )


