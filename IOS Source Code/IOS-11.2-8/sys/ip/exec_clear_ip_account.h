/* $Id: exec_clear_ip_account.h,v 3.2.58.1 1996/08/12 16:02:53 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_clear_ip_account.h,v $
 *------------------------------------------------------------------
 * IP accounting commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_clear_ip_account.h,v $
 * Revision 3.2.58.1  1996/08/12  16:02:53  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:32:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:40  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:16:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * clear ip accounting [checkpoint]
 *
 * OBJ(int,1) = TRUE if clearing checkpoint information
 */
EOLNS	(clear_ip_eol, clear_ip_accounting);
KEYWORD_ID (clear_ip_acc_ckpoint, clear_ip_eol, clear_ip_eol,
	OBJ(int,1), TRUE,
	"checkpoint", "Clear IP accounting checkpoint database", PRIV_OPR);
KEYWORD_ID (clear_ip_acc, clear_ip_acc_ckpoint, ALTERNATE,
	    OBJ(int,1), FALSE,
	    "accounting", "Clear IP accounting database", PRIV_OPR);


#undef	ALTERNATE
#define	ALTERNATE	clear_ip_acc
