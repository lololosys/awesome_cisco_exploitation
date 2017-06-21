/* $Id: exec_show_syslog_history.h,v 3.1.48.2 1996/08/12 16:05:19 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_show_syslog_history.h,v $
 *------------------------------------------------------------------
 * exec_show_syslog_history.h
 *
 * October 1995, Anke Dosedal
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_syslog_history.h,v $
 * Revision 3.1.48.2  1996/08/12  16:05:19  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.48.1  1996/05/21  06:36:32  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.1  1996/01/19  03:24:23  anke
 * Placeholder for Syslog MIB
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show logging history
 * 
 */

EOLNS	(show_log_history_eol, display_log_history);
KEYWORD (show_log_history, show_log_history_eol, no_alt,
	 "history", "Show the contents of syslog history table",
	 PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_log_history
