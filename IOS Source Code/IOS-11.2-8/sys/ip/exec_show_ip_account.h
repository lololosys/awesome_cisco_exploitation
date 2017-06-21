/* $Id: exec_show_ip_account.h,v 3.2.60.1 1996/08/12 16:02:58 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_show_ip_account.h,v $
 *------------------------------------------------------------------
 * IP accounting show commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_ip_account.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:58  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:32:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:44  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:24:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show ip accounting [checkpoint]
 * 
 * OBJ(int,1) = TRUE if [checkpoint]
 */
EOLNS	(show_ip_accounting_eol, show_ipaccounting);

KEYWORD_ID (show_ip_acc_viol, show_ip_accounting_eol, show_ip_accounting_eol,
	    OBJ(int,2), TRUE,
	    "access-violations",
	    "show access violations in accounting database", PRIV_USER);

KEYWORD_ID (show_ip_acc_opak, show_ip_accounting_eol, show_ip_acc_viol,
	    OBJ(int,2), FALSE,
	    "output-packets",
	    "show output packets in accounting database", PRIV_USER);

KEYWORD_ID (show_ip_acc_check, show_ip_acc_opak, show_ip_acc_opak,
	    OBJ(int,1), TRUE,
	    "checkpoint", "The checkpointed IP accounting database",
	    PRIV_USER);

KEYWORD (show_ip_acc, show_ip_acc_check, ALTERNATE,
	 "accounting", "The active IP accounting database", PRIV_USER);


#undef	ALTERNATE
#define	ALTERNATE	show_ip_acc
