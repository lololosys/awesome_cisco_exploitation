/* $Id: exec_show_rmon_lite.h,v 3.2.32.1 1996/08/12 16:07:00 widmer Exp $
 * $Source: /release/112/cvs/Xsys/rmon/exec_show_rmon_lite.h,v $
 *------------------------------------------------------------------
 * exec_show_rmon_lite.h:  Remote Monitoring Exec Commands
 *
 * February 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_rmon_lite.h,v $
 * Revision 3.2.32.1  1996/08/12  16:07:00  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1996/02/09  07:57:18  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.1  1996/02/09  06:12:06  jjohnson
 * Initial placeholder for rmon user interface enhancements
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 *  show rmon [keyword]...
 * 
 */
EOLNS	(show_rmon_eol, rmon_lite_show_command);

pdecl(show_rmon_opts);

LINK_TRANS(show_rmon_return_here, show_rmon_opts);
LINK_TRANS(show_rmon_extend_here, NONE);

KEYWORD_ORTEST(show_rmon_alarms, show_rmon_opts, show_rmon_extend_here,
	       OBJ(int,1), (1<<RMON_SHOW_ALARMS),
	       "alarms", "Display the RMON alarm table", PRIV_USER);

KEYWORD_ORTEST(show_rmon_events, show_rmon_opts, show_rmon_alarms,
	       OBJ(int,1), (1<<RMON_SHOW_EVENTS),
	       "events", "Display the RMON event table", PRIV_USER);

NOP	(show_rmon_opts, show_rmon_events, show_rmon_eol);

KEYWORD (show_rmon, show_rmon_opts, ALTERNATE,
	 "rmon", "rmon statistics", PRIV_USER);


#undef	ALTERNATE
#define	ALTERNATE	show_rmon
