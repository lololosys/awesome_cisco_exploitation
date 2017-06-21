/* $Id: parser_defs_standby.h,v 3.1 1996/02/01 04:58:49 hampton Exp $
 * $Source: /release/111/cvs/Xsys/iprouting/parser_defs_standby.h,v $
 *------------------------------------------------------------------
 * parser_defs_standby.h -- Parser definitions for hot standby protocol
 *
 * Sep 1993 Tony Li
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_standby.h,v $
 * Revision 3.1  1996/02/01  04:58:49  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:52:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:31:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define STANDBY_CMD_IP		0
#define STANDBY_CMD_AUTH	1
#define STANDBY_CMD_PREEMPT	2
#define STANDBY_CMD_PRIORITY	3
#define STANDBY_CMD_TIMERS	4
#define STANDBY_CMD_TRACK	5
#define STANDBY_CMD_USEBIA      6
#define STANDBY_CMD_ALL		255
