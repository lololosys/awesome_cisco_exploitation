/* $Id: exec_show_sntp.h,v 3.1.2.2 1996/08/12 16:04:53 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ntp/exec_show_sntp.h,v $
 *------------------------------------------------------------------
 * Parser chains for SNTP "show" commands
 *
 * June 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_sntp.h,v $
 * Revision 3.1.2.2  1996/08/12  16:04:53  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/06/23  04:40:23  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add an SNTP client.  This is not included in shipping IOS images.
 *
 * Revision 3.1  1996/06/22  21:42:09  dkatz
 * Add placeholders.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show sntp
 *
 */

EOLNS    (show_sntp_eol, show_sntp);

KEYWORD (show_sntp, show_sntp_eol, ALTERNATE,
	 "sntp", "Simple network time protocol",
	 PRIV_USER | PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_sntp
