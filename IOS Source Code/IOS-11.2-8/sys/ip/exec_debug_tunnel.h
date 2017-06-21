/* $Id: exec_debug_tunnel.h,v 3.2 1995/11/17 09:32:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/exec_debug_tunnel.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ T U N N E L . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_tunnel.h,v $
 * Revision 3.2  1995/11/17  09:32:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:43  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.2  1995/06/19  14:49:21  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:20:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * debug tunnel
 *
 */
EOLS	(debug_tunnel_eol, debug_command, DEBUG_TUNNEL);
KEYWORD_DEBUG(debug_tunnel, debug_tunnel_eol, ALTERNATE,
	      OBJ(pdb,1), tunnel_debug_arr,
	      "tunnel", "Generic Tunnel Interface", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_tunnel
