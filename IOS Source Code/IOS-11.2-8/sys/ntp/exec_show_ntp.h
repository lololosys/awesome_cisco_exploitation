/* $Id: exec_show_ntp.h,v 3.2.62.2 1996/08/12 16:04:52 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ntp/exec_show_ntp.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ N T P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_ntp.h,v $
 * Revision 3.2.62.2  1996/08/12  16:04:52  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.62.1  1996/06/05  18:57:18  widmer
 * CSCdi59571:  PRIV_USER_HIDDEN used without a privilege level
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:50:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:44:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ntp status
 * 
 */

EOLNS    (show_ntp_status_eol, show_ntp_status);
KEYWORD (show_ntp_status, show_ntp_status_eol, no_alt,
	 "status", "NTP status", PRIV_USER);

/******************************************************************
 * show ntp associations [detail]
 *
 * OBJ(int,1) = TRUE if "detail" specified
 * 
 */

EOLNS    (show_ntp_assoc_eol, show_ntp_assoc);
KEYWORD_ID (show_ntp_assoc_detail, show_ntp_assoc_eol, show_ntp_assoc_eol,
	    OBJ(int,1), TRUE,
	    "detail", "Show detail", PRIV_USER);
KEYWORD (show_ntp_assoc, show_ntp_assoc_detail, show_ntp_status, 
	 "associations", "NTP associations", PRIV_USER);

KEYWORD (show_ntp, show_ntp_assoc, ALTERNATE,
	 "ntp", "Network time protocol",
	 PRIV_USER | PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_ntp
