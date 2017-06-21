/* $Id: exec_show_logging.h,v 3.3.20.2 1996/08/12 16:05:11 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_show_logging.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ L O G G I N G . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_logging.h,v $
 * Revision 3.3.20.2  1996/08/12  16:05:11  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.20.1  1996/05/21  06:36:31  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.3  1996/01/29  07:09:07  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:46:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:10  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:25:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show logging
 * 
 */
EOLNS	(show_logging_eol, display_logger);
LINK_TRANS(show_logging_extend_here, show_logging_eol);
KEYWORD (show_logging, show_logging_extend_here, ALTERNATE,
	 "logging", "Show the contents of logging buffers",
	 PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_logging
