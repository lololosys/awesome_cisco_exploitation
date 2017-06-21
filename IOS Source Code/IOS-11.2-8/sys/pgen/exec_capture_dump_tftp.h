/* $Id: exec_capture_dump_tftp.h,v 3.2.60.2 1996/08/12 16:06:52 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pgen/exec_capture_dump_tftp.h,v $
 *------------------------------------------------------------------
 * dumps the captured packets into the specified tftp server.
 *
 * Jan, 1995 skoh
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_capture_dump_tftp.h,v $
 * Revision 3.2.60.2  1996/08/12  16:06:52  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.1  1996/05/21  10:00:25  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2  1995/11/17  18:56:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/14  01:51:57  biren
 * CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
 *
 * Revision 2.2  1995/06/21  18:18:22  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:33:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*******************************************************************
 * syntax: capture dump interface <interface> <tftp server> <file name>
 *
 * descr:  This command dumps the captured packets into the specified
 *         tftp server.
 *
 * OBJ(paddr, 1) = <tftp server>
 * OBJ(string, 1) = <file name>
 * OBJ(idb, 1) = <interface>
 *
 */

EOLNS	(capture_dump_cmds_eol, capture_dump_commands);
STRING	(capture_dump_fname, capture_dump_cmds_eol, no_alt,
	 OBJ(string,1), "File name into which to dump");
IPADDR_NAME	(capture_dump_tftp_svr, capture_dump_fname, no_alt,
		 OBJ(paddr,1), "Name or address of TFTP server to dump onto");
INTERFACE	(capture_dump_get_interf, capture_dump_tftp_svr, no_alt,
		 OBJ(idb,1), IFTYPE_CAPTURE);
KEYWORD (capture_dump_interfaces, capture_dump_get_interf, no_alt, 
        common_str_interface, "interface in capture dumping mode", PRIV_USER);
KEYWORD	(capture_dump_cmds, capture_dump_interfaces, no_alt,
	 "dump", "write captured info to a TFTP server", PRIV_USER);
KEYWORD	(exec_capture_dump_tftp, capture_dump_cmds, no_alt, 
	 "capture", "commands for the capturing facility", PRIV_ROOT);

#undef	ALTERNATE
#define ALTERNATE exec_capture_dump_tftp
