/* $Id: exec_debug_serial.h,v 3.2.62.1 1996/05/21 10:09:29 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/exec_debug_serial.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ S E R I A L . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_serial.h,v $
 * Revision 3.2.62.1  1996/05/21  10:09:29  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2  1995/11/17  18:01:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:07  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.3  1995/06/19  14:48:53  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:56:01  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:19:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug serial packet
 *
 */
EOLS	(debug_serial_packet_eol, debug_command, DEBUG_SERIALPACKET);
KEYWORD (debug_serial_packet, debug_serial_packet_eol, no_alt,
	 "packet", "Serial network interface packets", PRIV_OPR);

/******************************************************************
 * debug serial interface
 *
 */
EOLS	(debug_serialif_eol, debug_command, DEBUG_SERIAL);
KEYWORD (debug_serialif, debug_serialif_eol, debug_serial_packet,
	 common_str_interface, "Serial interface events", PRIV_OPR);

/******************************************************************
 * debug serial
 *
 */
KEYWORD_DEBUG(debug_serial, debug_serialif, ALTERNATE,
	      OBJ(pdb,1), serial_debug_arr,
	      "serial", "Serial interface information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_serial
