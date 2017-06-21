/* $Id: exec_debug_stun.h,v 3.3 1996/02/01 23:33:23 widmer Exp $
 * $Source: /release/111/cvs/Xsys/ibm/exec_debug_stun.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ S T U N . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_stun.h,v $
 * Revision 3.3  1996/02/01  23:33:23  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:22:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:54  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.3  1995/06/19  14:49:09  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:56:05  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:20:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug stun tg
 *
 */
EOLS    (debug_stun_tg_eol, sdlc_debug_command, DEBUG_STUNTG);
KEYWORD (debug_stun_tg, debug_stun_tg_eol, no_alt,
         "tg", "Display stun transmission groups", PRIV_OPR);

/******************************************************************
 * debug stun packet [<group> [<address>]]
 *
 * OBJ(int,1) = <group> (0 means no group)
 * OBJ(int,2) = TRUE if <address> is set, else FALSE
 * (*OBJ(paddr,1)) = address
 */
EOLS	(debug_stun_packet_eol, sdlc_debug_command, DEBUG_STUNPACKET);
SET	(debug_stun_packet_true, debug_stun_packet_eol, OBJ(int,2), TRUE);
STUNADDR (debug_stun_pkt_addr, debug_stun_packet_true, debug_stun_packet_eol,
          OBJ(paddr,1), "SDLC address");
EVAL	(debug_stun_pkt_grp_assert, debug_stun_pkt_addr,
	 csb->stungroup = GETOBJ(int,1));
NUMBER	(debug_stun_pkt_grp, debug_stun_pkt_grp_assert, debug_stun_packet_eol,
         OBJ(int,1), 1, 255, "A STUN group number");
KEYWORD	(debug_stun_packet, debug_stun_pkt_grp, debug_stun_tg,
	 "packet", "All STUN activity", PRIV_OPR);

/******************************************************************
 * debug stun events
 *
 */
EOLS	(debug_stun_events_eol, sdlc_debug_command, DEBUG_STUN);
KEYWORD (debug_stun_events, debug_stun_events_eol, debug_stun_packet,
         "events", "STUN connections and status", PRIV_OPR);

/******************************************************************
 * debug stun
 *
 */
KEYWORD_DEBUG(debug_stun, debug_stun_events, ALTERNATE,
	      OBJ(pdb,1), sdlc_debug_arr,
	      "stun", "STUN information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_stun
