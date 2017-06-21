/* $Id: exec_show_ip_tcpcompress.h,v 3.3.28.1 1996/08/12 16:08:04 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tcp/exec_show_ip_tcpcompress.h,v $
 *------------------------------------------------------------------
 * TCP/IP header compression show commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_ip_tcpcompress.h,v $
 * Revision 3.3.28.1  1996/08/12  16:08:04  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/18  02:15:29  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  18:47:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:56:31  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:24:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show ip tcp header-compression <interface>
 * 
 * OBJ(idb,1) = <interface>
 */
EOLNS	(show_ip_tcp_hc_eol, show_header_compression);
INTERFACE(show_ip_tcp_hc_int, show_ip_tcp_hc_eol, show_ip_tcp_hc_eol,
	  OBJ(idb,1), IFTYPE_HWIDB     );
KEYWORD (show_ip_tcp_hc, show_ip_tcp_hc_int, no_alt,
	 "header-compression", "TCP/IP header-compression statistics",
	 PRIV_USER);
KEYWORD (show_ip_tcp, show_ip_tcp_hc, ALTERNATE,
	 "tcp", "TCP/IP header-compression statistics", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_tcp
