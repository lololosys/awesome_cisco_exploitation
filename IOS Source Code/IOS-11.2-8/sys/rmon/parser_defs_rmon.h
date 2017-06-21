/* $Id: parser_defs_rmon.h,v 3.4 1996/02/18 17:57:49 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/rmon/parser_defs_rmon.h,v $
 *------------------------------------------------------------------
 * parser_defs_rmon.h:  Remote Monitoring Parser Definitions
 *
 * August 1995, Jeffrey T. Johnson
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_rmon.h,v $
 * Revision 3.4  1996/02/18  17:57:49  jjohnson
 * CSCdi49292:  rmon events and alarms do not survive a reload
 * implement config commands to configure/nvgen events and alarms
 *
 * Revision 3.3  1996/02/09  07:57:18  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.2  1995/11/17  18:39:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:02:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * KEYWORD_OR flags for rmon config commands
 */
#define RMON_KEY_OWNER		0x0001
#define RMON_KEY_DESCRIPTION	0x0002
#define RMON_KEY_LOG		0x0004
#define RMON_KEY_TRAP		0x0008

/*
 * possible values for rmon show commands
 */
enum {
    RMON_SHOW_STATISTICS = 0,
    RMON_SHOW_HISTORY,
    RMON_SHOW_ALARMS,
    RMON_SHOW_HOSTS,
    RMON_SHOW_TOPN,
    RMON_SHOW_MATRIX,
    RMON_SHOW_FILTER,
    RMON_SHOW_CAPTURE,
    RMON_SHOW_EVENTS,
    RMON_SHOW_TASK
};

/*
 * possible values for rmon interface configuration
 */
enum {
    RMON_DISABLED = 0,
    RMON_NATIVE,
    RMON_PROMISCUOUS
};

/*
 * possible values for rmon global configuration
 */
enum {
    RMON_QUEUESIZE,
    RMON_PRIORITY
};

#define RMON_QUEUE_SIZE_MIN 1
#define RMON_QUEUE_SIZE_DEF 64
#define RMON_QUEUE_SIZE_MAX 0x7fffffff

#define RMON_PRIORITY_DEF	PRIO_NORMAL
