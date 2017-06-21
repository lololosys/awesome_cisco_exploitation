/* $Id: exec_debug_bsc.h,v 3.2.60.1 1996/09/10 08:48:45 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/exec_debug_bsc.h,v $
 *------------------------------------------------------------------
 * exec_debug_bsc.h - BSC Debug commands
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_bsc.h,v $
 * Revision 3.2.60.1  1996/09/10  08:48:45  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:49:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  09:35:34  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.2  1995/06/19  14:47:27  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:17:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*----------------------------------------------------------------------------*/
/* debug bsc packet [group <number>] | [buffer-size <number>] 
 *
 */

EOLS	(debug_bsc_packet_eol, bsc_debug_command, DEBUG_BSC_PACKET);
NUMBER  (debug_bsc_packet_size_val, debug_bsc_packet_eol, no_alt, OBJ(int,2), 
         1, 2000, "maximum bytes of data to display for packet debug");
KEYWORD (debug_bsc_packet_size, debug_bsc_packet_size_val, debug_bsc_packet_eol,
         "buffer-size", "BSC packet display buffer size", PRIV_OPR);
NUMBER  (debug_bsc_packet_group_val,debug_bsc_packet_size, debug_bsc_packet_eol,
         OBJ(int,1), 1, 255, "bstun group number between 1 and 255");
KEYWORD (debug_bsc_packet_group, debug_bsc_packet_group_val, debug_bsc_packet_size,
         "group", "BSTUN group number", PRIV_USER);
KEYWORD	(debug_bsc_packet, debug_bsc_packet_group, no_alt,
	 "packet", "BSC packet display", PRIV_OPR);


/*----------------------------------------------------------------------------*/
/* debug bsc event [<group-number>] 
 *
 */

EOLS	(debug_bsc_event_eol, bsc_debug_command, DEBUG_BSC_EVENT);
NUMBER  (debug_bsc_event_group, debug_bsc_event_eol, debug_bsc_event_eol,
         OBJ(int,1), 1, 255, "bstun group number between 1 and 255");
KEYWORD (debug_bsc_event, debug_bsc_event_group, debug_bsc_packet,
         "event", "BSC events", PRIV_OPR);

/*----------------------------------------------------------------------------*/
/* debug bsc
 *
 */

KEYWORD_DEBUG(debug_bsc, debug_bsc_event, ALTERNATE,
	      OBJ(pdb,1), bsc_debug_arr,
	      "bsc", "BSC information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_bsc
