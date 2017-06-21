/* $Id: exec_clear_ip.h,v 3.2.60.1 1996/08/12 16:02:52 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_clear_ip.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ I P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_ip.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:52  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:32:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:39  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:15:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(clear_ip_extend_here, no_alt);

/******************************************************************
 * clear ip redirect
 *
 */
EOLNS (clear_ip_redirect_eol, clear_redirect_cache);
KEYWORD_ID (clear_ip_redirect, clear_ip_redirect_eol, clear_ip_extend_here,
	    OBJ(int, 1), FALSE,
	    "redirect", "Redirect cache", PRIV_OPR);

    
/******************************************************************/
KEYWORD (clear_ip, clear_ip_redirect, ALTERNATE,
	"ip", "IP", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_ip
