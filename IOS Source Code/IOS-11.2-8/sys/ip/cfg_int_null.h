/* $Id: cfg_int_null.h,v 3.2 1995/11/17 09:32:15 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/cfg_int_null.h,v $
 *------------------------------------------------------------------
 * NULL interface commands
 *
 * October 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_null.h,v $
 * Revision 3.2  1995/11/17  09:32:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:35  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:06:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS	(null_eol, ip_unreachable_command, IP_UNREACHABLE);
KEYWORD	(null_ip_unreachable, null_eol, no_alt,
	 "unreachables", "Enable sending ICMP Unreachable messages",
	 PRIV_CONF|PRIV_SUBIF);
KEYWORD	(null_ip_kwd, null_ip_unreachable, NONE,
	 "ip", "Interface Internet Protocol config commands",
	 PRIV_CONF|PRIV_SUBIF);
ASSERT	(null_ip, null_ip_kwd, ALTERNATE,
	 main_if_or_vc(csb->interface));

#undef	ALTERNATE
#define	ALTERNATE	null_ip
