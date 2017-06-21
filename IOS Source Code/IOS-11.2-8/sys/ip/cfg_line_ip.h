/* $Id: cfg_line_ip.h,v 3.2.60.2 1996/09/09 19:52:32 sjackson Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_line_ip.h,v $
 *------------------------------------------------------------------
 * cfg_line_ip.h - IP specific line configuration commands
 *
 * July 1994 Tony Li
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_line_ip.h,v $
 * Revision 3.2.60.2  1996/09/09  19:52:32  sjackson
 * CSCdi67473:  big buffers created not trimmed (queued on TCP input with
 * rtelnet)
 * Incorporate code review comments
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/09/05  01:18:14  sjackson
 * CSCdi67473:  big buffers created not trimmed (queued on TCP input with
 * rtelnet)
 * Enable coalescing of TCP input packets by default for reverse telnet
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:32:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:38  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.2  1995/06/21  18:17:04  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:09:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * [term] ip tcp input-coalesce-threshold <size>
 * [term] ip netmask-format (hexadecimal | decimal | bit-count)
 *
 * OBJ(int,1) = selected format or queue size
 */

PARAMS_KEYONLY (clip_tcp_queue, no_alt,
	 "input-coalesce-threshold", OBJ(int, 1), 5, 255, line_command,
	 TERM_IP_TCP_QUEUE_SIZE,
	 "Set the threshold that triggers packet coalescing (20 default)",
	 "Number of packets", PRIV_USER);
KEYWORD (clip_tcp, clip_tcp_queue, no_alt, "tcp", "TCP options", PRIV_USER);

EOLS (clip_netmask_eol, line_command, TERM_IP_NETMASK_FORMAT);
KEYWORD_ID (clip_netmask_bitcount, clip_netmask_eol, no_alt,
	    OBJ(int, 1), MASK_BITCOUNT,
	    "bit-count", "Display netmask as number of significant bits",
	    PRIV_USER);
KEYWORD_ID (clip_netmask_decimal, clip_netmask_eol, clip_netmask_bitcount,
	    OBJ(int, 1), MASK_DOTTED_DECIMAL,
	    "decimal", "Display netmask in dotted decimal", PRIV_USER);
KEYWORD_ID (clip_netmask_hex, clip_netmask_eol, clip_netmask_decimal,
	    OBJ(int, 1), MASK_HEXADECIMAL,
	    "hexadecimal", "Display netmask in hexadecimal", PRIV_USER);
KEYWORD	(clip_netmask, clip_netmask_hex, clip_tcp,
	 "netmask-format", "Change display of netmasks", PRIV_USER);

KEYWORD (cl_ip, clip_netmask, ALTERNATE,
	 "ip", "IP options", PRIV_USER);

#undef  ALTERNATE
#define ALTERNATE	cl_ip

