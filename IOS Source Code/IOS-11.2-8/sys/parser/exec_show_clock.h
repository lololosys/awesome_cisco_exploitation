/* $Id: exec_show_clock.h,v 3.2.62.1 1996/08/12 16:06:16 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_show_clock.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ C L O C K . H
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_clock.h,v $
 * Revision 3.2.62.1  1996/08/12  16:06:16  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:47:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:22:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show {clock [detail] | calendar}
 * OBJ(int,1) = nz if "calendar"
 * OBJ(int,2) = nz if "detail"
 */

EOLNS	(show_clock_eol, show_clock);
KEYWORD_ID(show_calendar, show_clock_eol, NONE,
	   OBJ(int,1), 1, 
	   "calendar", "Display the hardware calendar",
	   PRIV_USER);

ASSERT  (show_calendar_test, show_calendar, ALTERNATE, 
	 reg_invoke_cpu_supports_calendar());

KEYWORD_ID(show_clock_detail, show_clock_eol, show_clock_eol, OBJ(int,2),
	   1, "detail", "Display detailed information", PRIV_USER);
KEYWORD (show_clock, show_clock_detail, show_calendar_test,
	 "clock", "Display the system clock", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_clock
