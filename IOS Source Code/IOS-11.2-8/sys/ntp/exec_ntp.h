/* $Id: exec_ntp.h,v 3.2.62.1 1996/08/12 16:04:51 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ntp/exec_ntp.h,v $
 *------------------------------------------------------------------
 * E X E C _ N T P . H
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_ntp.h,v $
 * Revision 3.2.62.1  1996/08/12  16:04:51  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:50:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:44:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * ntp leap {add | delete}
 *
 * OBJ(int,1) = leap bit value
 */

EOLNS    (ntp_leap_eol, ntp_leap_command);
KEYWORD_ID(ntp_leap_del, ntp_leap_eol, no_alt,
	   OBJ(int,1),  LEAP_DELSECOND,
	   "delete", NULL, PRIV_OPR);
KEYWORD_ID(ntp_leap_add, ntp_leap_eol, ntp_leap_del,
	   OBJ(int,1), LEAP_ADDSECOND,
	   "add", NULL, PRIV_OPR);
KEYWORD (ntp_leap, ntp_leap_add, no_alt,
	 "leap", NULL, PRIV_OPR|PRIV_HIDDEN);

KEYWORD (exec_ntp, ntp_leap, no_alt,
	 "ntp", NULL, PRIV_OPR|PRIV_HIDDEN);

TEST_BOOLEAN(exec_ntp_test, exec_ntp, NONE, ALTERNATE, ntp_running);

#undef	ALTERNATE
#define	ALTERNATE	exec_ntp_test
