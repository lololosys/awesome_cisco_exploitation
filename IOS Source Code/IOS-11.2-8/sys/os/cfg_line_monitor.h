/* $Id: cfg_line_monitor.h,v 3.2 1995/11/17 18:45:17 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/cfg_line_monitor.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ M O N I T O R . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_monitor.h,v $
 * Revision 3.2  1995/11/17  18:45:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:50:59  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:10:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * monitor
 * no monitor
 *
 */
EOLS	(cl_mon_eol, monitor_command, TERM_MONITOR);
KEYWORD	(cl_monitor, cl_mon_eol, ALTERNATE,
	 "monitor", "Copy debug output to the current terminal line",
	 PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	cl_monitor
