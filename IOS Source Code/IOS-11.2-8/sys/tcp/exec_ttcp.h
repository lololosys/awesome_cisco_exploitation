/* $Id: exec_ttcp.h,v 3.2.62.1 1996/08/12 16:08:10 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tcp/exec_ttcp.h,v $
 *------------------------------------------------------------------
 * E X E C _ T T C P . H
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_ttcp.h,v $
 * Revision 3.2.62.1  1996/08/12  16:08:10  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:47:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:56:34  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:27:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ttcp { receive | transmit <ipaddr> }
 *
 * OBJ(int, 1) = (0 -> dialog, 1 -> receive, 2 -> transmit)
 */

EOLNS	(exec_ttcp_eol, ttcp_command);

IPADDR_NAME (exec_ttcp_host, exec_ttcp_eol, no_alt,
	 OBJ(paddr,1), NULL /*"Remote hostname or IP address"*/);
KEYWORD_ID (exec_ttcp_xmit, exec_ttcp_host, exec_ttcp_eol,
	    OBJ(int,1), 2,
	    "transmit", NULL /*"run the ttcp transmitter"*/, PRIV_OPR);

KEYWORD_ID (exec_ttcp_rcv, exec_ttcp_eol, exec_ttcp_xmit,
	    OBJ(int,1), 1,
	    "receive", NULL /*"run the ttcp receiver"*/, PRIV_OPR);

KEYWORD_ID (exec_ttcp, exec_ttcp_rcv, ALTERNATE,
	    OBJ(int,1), 0,
	    "ttcp", NULL /*"Run ttcp"*/, PRIV_OPR | PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	exec_ttcp

