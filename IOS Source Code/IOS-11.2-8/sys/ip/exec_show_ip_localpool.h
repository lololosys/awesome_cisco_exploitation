/* $Id: exec_show_ip_localpool.h,v 3.2.60.1 1996/08/12 16:03:02 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_show_ip_localpool.h,v $
 *------------------------------------------------------------------
 * IP address pool status display commands
 *
 * April 1995, Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_ip_localpool.h,v $
 * Revision 3.2.60.1  1996/08/12  16:03:02  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:32:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/18  19:35:33  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.1  1995/08/17  01:43:17  tli
 * CSCdi38878:  DHCP: change ip local-pool to ip local pool
 *
 * Revision 2.1  1995/06/07  22:24:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ip local-pool [ <poolname> ]
 * 
 * 
 */


EOLNS	(show_ip_localp_eol, show_ip_localp);

STRING (show_ip_localp_pool, show_ip_localp_eol, show_ip_localp_eol,
	OBJ(string,1), NULL /*"Show address allocation for a specific Pool"*/);

KEYWORD_MM (show_ip_localp, show_ip_localp_pool, ALTERNATE,
	    "local-pool", NULL /*"IP Local Address pool"*/,
	    PRIV_USER|PRIV_HIDDEN, 6);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_localp
