/* $Id: exec_debug_pcbus.h,v 3.2.60.1 1996/05/21 09:56:02 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_debug_pcbus.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ P C B U S . H
 *
 * April 1994, Johnny Chan
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_pcbus.h,v $
 * Revision 3.2.60.1  1996/05/21  09:56:02  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2  1995/11/17  17:36:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:54:16  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.3  1995/06/19  14:48:38  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:58  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:19:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/******************************************************************
 * debug pcbus packet
 *
 */
EOLS	(debug_pcbus_packet_eol, debug_command, DEBUG_PCBUSPACKET);
KEYWORD (debug_pcbus_packet, debug_pcbus_packet_eol, no_alt,
	 "packet", "PCbus network interface packets", PRIV_OPR);

/******************************************************************
 * debug pcbus interface
 *
 */
EOLS	(debug_pcbusif_eol, debug_command, DEBUG_PCBUS);
KEYWORD (debug_pcbusif, debug_pcbusif_eol, debug_pcbus_packet,
	 common_str_interface, "PCbus interface events", PRIV_OPR);

/******************************************************************
 * debug pcbus
 *
 */
KEYWORD_DEBUG(debug_pcbus, debug_pcbusif, ALTERNATE,
	      OBJ(pdb,1), pcbus_debug_arr,
	      "pcbus", "PCbus interface information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_pcbus
