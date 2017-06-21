/* $Id: exec_tunnel.h,v 3.3.40.1 1996/08/12 16:03:05 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_tunnel.h,v $
 *------------------------------------------------------------------
 * exec_tunnel.h - exec command for setting up an async-mobility connection
 *
 * September, 1994  -- Bill Westfield
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_tunnel.h,v $
 * Revision 3.3.40.1  1996/08/12  16:03:05  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/12/15  22:27:45  widmer
 * CSCdi45685:  Need priv flag for interactive commands
 * Add Priv_Interactive to flag interactive commands
 *
 * Revision 3.2  1995/11/17  09:32:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:47  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:27:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * tunnel <host> [debug]
 *  OBJ(int,3) = TRUE if debug specified
 *
 */

EOLNS(exec_tunnel_eol, connect_command);

KEYWORD_ID(exec_tunnel_debug, exec_tunnel_eol, exec_tunnel_eol,
	   OBJ(int,3), TRUE,
	   "debug", "Enable tunnel debugging output", PRIV_USER);
STRING	(exec_tunnel_host, exec_tunnel_debug, no_alt,
	 OBJ(string,1), "Address or hostname of a remote system");
SET	(exec_tunnel_stream_set, exec_tunnel_host,
	 OBJ(int,5), TRUE);
SET	(exec_tunnel_port_set, exec_tunnel_stream_set,
	 OBJ(int,1), TUNNEL_PORT);
KEYWORD_ID(exec_tunnel, exec_tunnel_port_set, ALTERNATE,
	   OBJ(int,9), CMD_CONNECT_TUNNEL,
	   "tunnel", "Open a tunnel connection",
	   PRIV_USER | PRIV_INTERACTIVE);

#undef	ALTERNATE
#define	ALTERNATE	exec_tunnel
