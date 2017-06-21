/* $Id: exec_show_rmon.h,v 3.4 1996/02/09 07:57:17 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/rmon/exec_show_rmon.h,v $
 *------------------------------------------------------------------
 * exec_show_rmon.h:  Remote Monitoring Exec Commands
 *
 * August 1995, Jeffrey T. Johnson
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_rmon.h,v $
 * Revision 3.4  1996/02/09  07:57:17  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.3  1996/01/09  01:21:27  jjohnson
 * CSCdi46285:  Need to clean up the RMON user interface
 *
 * Revision 3.2  1995/11/17  18:39:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:02:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 *  show rmon [keyword]...
 *  (this extends the parse chain defined by the rmonlite module
 */
LINK_EXIT(show_rmon_exit, no_alt);

KEYWORD_ORTEST(show_rmon_task, show_rmon_exit, ALTERNATE,
	       OBJ(int,1), (1<<RMON_SHOW_TASK),
	       "task", "Display the RMON task statistics", PRIV_USER);

KEYWORD_ORTEST(show_rmon_capture, show_rmon_exit, show_rmon_task,
	       OBJ(int,1), (1<<RMON_SHOW_CAPTURE),
	       "capture", "Display the RMON buffer capture table", PRIV_USER);

KEYWORD_ORTEST(show_rmon_filter, show_rmon_exit, show_rmon_capture,
	       OBJ(int,1), (1<<RMON_SHOW_FILTER),
	       "filter", "Display the RMON filter table", PRIV_USER);

KEYWORD_ORTEST(show_rmon_matrix, show_rmon_exit, show_rmon_filter,
	       OBJ(int,1), (1<<RMON_SHOW_MATRIX),
	       "matrix", "Display the RMON matrix table", PRIV_USER);

KEYWORD_ORTEST(show_rmon_topn, show_rmon_exit, show_rmon_matrix,
	       OBJ(int,1), (1<<RMON_SHOW_TOPN),
	       "topn", "Display the RMON top-n hosts table", PRIV_USER);

KEYWORD_ORTEST(show_rmon_hosts, show_rmon_exit, show_rmon_topn,
	       OBJ(int,1), (1<<RMON_SHOW_HOSTS),
	       "hosts", "Display the RMON hosts table", PRIV_USER);

KEYWORD_ORTEST(show_rmon_history, show_rmon_exit, show_rmon_hosts,
	       OBJ(int,1), (1<<RMON_SHOW_HISTORY),
	       "history", "Display the RMON history table", PRIV_USER);

KEYWORD_ORTEST(show_rmon_stats, show_rmon_exit, show_rmon_history,
	       OBJ(int,1), (1<<RMON_SHOW_STATISTICS),
	       "statistics", "Display the RMON statistics table", PRIV_USER);


#undef	ALTERNATE
#define	ALTERNATE	show_rmon_stats
