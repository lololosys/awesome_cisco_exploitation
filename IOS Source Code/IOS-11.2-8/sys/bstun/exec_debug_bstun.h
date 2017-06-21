/* $Id: exec_debug_bstun.h,v 3.3.20.1 1996/09/10 08:48:47 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/exec_debug_bstun.h,v $
 *------------------------------------------------------------------
 * exec_debug_bstun.h - BSTUN Debug commands
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_bstun.h,v $
 * Revision 3.3.20.1  1996/09/10  08:48:47  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/01  23:32:56  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  08:49:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  09:35:35  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.3  1995/06/19  14:47:28  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:33  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:17:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug bsc packet [group <number>] | [buffer-size <number>] 
 *
 */

EOLS	(debug_bstun_packet_eol, bstun_debug_command, DEBUG_BSTUN_PACKET);
NUMBER  (debug_bstun_packet_size_val, debug_bstun_packet_eol, no_alt, OBJ(int,2), 
         1, 2000, "maximum bytes of data to display for packet debug");
KEYWORD (debug_bstun_packet_size, debug_bstun_packet_size_val, debug_bstun_packet_eol,
         "buffer-size", "BSTUN packet display buffer size", PRIV_OPR);
NUMBER  (debug_bstun_packet_group_val,debug_bstun_packet_size, debug_bstun_packet_eol,
         OBJ(int,1), 1, 255, "bstun group number between 1 and 255");
KEYWORD (debug_bstun_packet_group, debug_bstun_packet_group_val, debug_bstun_packet_size,
         "group", "BSTUN group number", PRIV_USER);
KEYWORD	(debug_bstun_packet, debug_bstun_packet_group, no_alt,
	 "packet", "BSTUN packet display", PRIV_OPR);


/******************************************************************
 * debug bstun event [<group-number>]
 *
 */

EOLS	(debug_bstun_event_eol, bstun_debug_command, DEBUG_BSTUN_EVENT);
NUMBER  (debug_bstun_event_group, debug_bstun_event_eol, debug_bstun_event_eol,
         OBJ(int,1), 1, 255, "bstun group number between 1 and 255");
KEYWORD (debug_bstun_event, debug_bstun_event_group, debug_bstun_packet,
         "event", "BSTUN connections and status", PRIV_OPR);

/******************************************************************
 * debug bstun
 *
 */
KEYWORD_DEBUG(debug_bstun, debug_bstun_event, ALTERNATE,
	      OBJ(pdb,1), bstun_debug_arr,
	      "bstun", "BSTUN information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_bstun
