/* $Id: exec_show_ip_local_pool.h,v 3.2.60.1 1996/08/12 16:03:01 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_show_ip_local_pool.h,v $
 *------------------------------------------------------------------
 * IP address pool status display commands
 *
 * April 1995, Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_ip_local_pool.h,v $
 * Revision 3.2.60.1  1996/08/12  16:03:01  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:32:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/18  19:35:32  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.1  1995/08/17  01:43:16  tli
 * CSCdi38878:  DHCP: change ip local-pool to ip local pool
 *
 * Revision 2.1  1995/06/07  22:24:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ip local pool [ <poolname> ]
 */
EOLNS	(show_ip_local_pool_eol, show_ip_localp);

STRING (show_ip_local_pool_scope, show_ip_local_pool_eol,
	show_ip_local_pool_eol,
	OBJ(string,1), "Show address allocation for a specific Pool");

KEYWORD (show_ip_local_pool, show_ip_local_pool_scope, ALTERNATE,
	 "pool", "IP Local Address pool statistics", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_local_pool
