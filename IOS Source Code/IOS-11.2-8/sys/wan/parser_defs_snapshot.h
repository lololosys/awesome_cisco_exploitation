/* $Id: parser_defs_snapshot.h,v 3.1 1996/02/01 05:14:38 hampton Exp $
 * $Source: /release/111/cvs/Xsys/wan/parser_defs_snapshot.h,v $
 *------------------------------------------------------------------
 * Parser definitions for Snapshot support.
 *
 * April 1994, Joel P. Bion
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Parser definitions for Snapshot support.
 *------------------------------------------------------------------
 * $Log: parser_defs_snapshot.h,v $
 * Revision 3.1  1996/02/01  05:14:38  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:52:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:31:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef _PARSER_DEFS_SNAPSHOT_H_
#define _PARSER_DEFS_SNAPSHOT_H_

#define SNAPSHOT_KEYWORD 		"snapshot"

#define SNAPSHOT_CLIENT				(0)
#define SNAPSHOT_KEYWORD_CLIENT 		"client"
#define SNAPSHOT_SERVER				(SNAPSHOT_CLIENT + 1)
#define SNAPSHOT_KEYWORD_SERVER			"server"
/* prevent parser conflicts with 'sna' commands */
#define SNAPSHOT_UNIQ				4


/* Allows for active interval end + post-active time in server */
#define SNAPSHOT_QUIET_INTERVAL_MIN	(8)
#define SNAPSHOT_QUIET_INTERVAL_MAX	(100000)

/* Allow for post-active quiet interval on server hitting us... */
#define SNAPSHOT_ACTIVE_INTERVAL_MIN	(5)
#define SNAPSHOT_ACTIVE_INTERVAL_MAX	(1000)



#define SNAPSHOT_KEYWORD_SUPPRESS_STATECHANGE_UPDATES \
				   "suppress-statechange-update"
#define SNAPSHOT_KEYWORD_DIALER "dialer"
#define SNAPSHOT_KEYWORD_QUIET_TIME "quiet-time"

#define SNAPSHOT_SHOW_HELP "Snapshot parameters and statistics"
#define SNAPSHOT_DEBUG_HELP "Snapshot activity"
#define SNAPSHOT_INTERFACE_HELP \
  "Configure snapshot support on the interface"

#endif
