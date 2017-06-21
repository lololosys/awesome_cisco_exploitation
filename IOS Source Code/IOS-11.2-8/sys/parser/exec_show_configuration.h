/* $Id: exec_show_configuration.h,v 3.3.20.1 1996/08/12 16:06:17 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_show_configuration.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ C O N F I G U R A T I O N . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_configuration.h,v $
 * Revision 3.3.20.1  1996/08/12  16:06:17  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:18  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:47:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:22:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show startup-config
 * 
 */

EOLNS	(show_startup_eol, nv_review);
KEYWORD (show_startup, show_startup_eol, ALTERNATE,
	 "startup-config", "Contents of startup configuration",
	 PRIV_ROOT);

/******************************************************************
 * show running-config
 * 
 */
EOLS	(show_running_eol, write_terminal, WRITE_TERMINAL);
KEYWORD (show_running, show_running_eol, show_startup,
	 "running-config", "Current operating configuration",
	 PRIV_ROOT);

/******************************************************************
 * show configuration -- OBSOLETE
 * 
 */
EOLNS	(show_config_eol, nv_review);
KEYWORD (show_config, show_config_eol, show_running,
	 "configuration", "Contents of Non-Volatile memory", PRIV_ROOT);


#undef	ALTERNATE
#define	ALTERNATE	show_config
