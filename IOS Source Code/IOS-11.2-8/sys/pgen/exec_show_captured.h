/* $Id: exec_show_captured.h,v 3.2.60.2 1996/08/12 16:06:54 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pgen/exec_show_captured.h,v $
 *------------------------------------------------------------------
 * Shows the captured packets on specified interface
 *
 * Jan 1995, skoh 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_captured.h,v $
 * Revision 3.2.60.2  1996/08/12  16:06:54  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.1  1996/05/21  10:00:28  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2  1995/11/17  18:56:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/14  01:52:00  biren
 * CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
 *
 * Revision 2.2  1995/06/21  18:18:24  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:34:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*******************************************************************
 * syntax: show capture interface <interface> 
 *
 * descr:  This command shows the captured packets on specified interface
 *
 * OBJ(idb, 1) = <interface>
 *
 */


EOLNS	(show_captured_info_eol, show_capturedinfo);	   
INTERFACE (show_captured_info_get_interf, show_captured_info_eol, no_alt,
	   OBJ(idb,1), IFTYPE_CAPTURE);
KEYWORD (show_captured_interfaces, show_captured_info_get_interf, 
	 ALTERNATE, common_str_interface, "interface in capture mode", PRIV_USER);
KEYWORD (show_captured_info, show_captured_interfaces,
	 ALTERNATE, "captured", "capturing mode information", PRIV_USER);

#undef  ALTERNATE
#define ALTERNATE	show_captured_info

