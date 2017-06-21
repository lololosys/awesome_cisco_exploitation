/* $Id: exec_clear_capture.h,v 3.2.62.1 1996/05/21 10:00:26 thille Exp $
 * $Source: /release/112/cvs/Xsys/pgen/exec_clear_capture.h,v $
 *------------------------------------------------------------------
 * Clears buffer of the interface in capturing mode.
 *
 * Jan 1995, skoh
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_capture.h,v $
 * Revision 3.2.62.1  1996/05/21  10:00:26  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2  1995/11/17  18:56:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/14  01:51:57  biren
 * CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
 *
 * Revision 2.2  1995/06/21  18:18:23  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:33:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * syntax: clear capture interface <interface>
 *
 * descr:  This command clears buffer of the interface in capturing mode.
 *
 * OBJ(idb,1) = <interface>
 *
 */

EOLS (clear_capture_eol, capture_clear_command, CLEAR_CAPTURE);
KEYWORD (clear_capture_all, clear_capture_eol, no_alt,
	 "*", "clear capture buffer in all interfaces", PRIV_USER);
INTERFACE (clear_capture_get_int, clear_capture_eol, clear_capture_all,
	   OBJ(idb,1), IFTYPE_CAPTURE);
KEYWORD (clear_capture_interface, clear_capture_get_int, no_alt,
	 common_str_interface, "clear capture buffer in a interface", PRIV_USER);
KEYWORD (exec_clear_capture, clear_capture_interface, no_alt,
	 "capture", "clear packets captured in buffer", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	exec_clear_capture
