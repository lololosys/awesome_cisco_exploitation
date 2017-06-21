/* $Id: exec_debug_sscop.h,v 3.2.54.1 1996/07/29 23:38:40 jwjang Exp $
 * $Source: /release/112/cvs/Xsys/atm/exec_debug_sscop.h,v $
 *------------------------------------------------------------------
 * parser/exec_debug_sscop.h:  DEBUG command parsing code
 *
 * November, 1993,  Percy P. Khabardar 
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All Rights Reserved.
 *
 * Debug commands for SSCOP (ATM Signalling).
 *------------------------------------------------------------------
 * $Log: exec_debug_sscop.h,v $
 * Revision 3.2.54.1  1996/07/29  23:38:40  jwjang
 * CSCdi62882:  SSCOP: when vp-tunnel is shutdown the peers SSCOP remains
 * active
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:46:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  09:35:15  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.2  1995/06/19  14:49:04  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:20:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************************
 * debug sscop error
 *
 */
EOLS    (debug_sscop_error_eol, debug_command, DEBUG_SSCOP_ERROR);
KEYWORD (debug_sscop_error, debug_sscop_error_eol, no_alt,
         "error", "SSCOP errors", PRIV_OPR);


/****************************************************************************
 * debug sscop suspend/resume
 *
 */
EOLS    (debug_sscop_suspend_eol, debug_command, DEBUG_SSCOP_SUSPEND);
KEYWORD (debug_sscop_suspend, debug_sscop_suspend_eol, debug_sscop_error,
         "suspend", "Suspend SSCOP", PRIV_OPR | PRIV_HIDDEN);

/****************************************************************************
 * debug sscop packets
 *
 */
EOLS    (debug_sscop_packets_eol, debug_command, DEBUG_SSCOP_PAK);
KEYWORD (debug_sscop_packets, debug_sscop_packets_eol, debug_sscop_suspend,
         "packets", "SSCOP packets", PRIV_OPR);

/***************************************************************************
 * debug sscop events
 *
 */
EOLS    (debug_sscop_events_eol, debug_command, DEBUG_SSCOP_EVENT);
KEYWORD (debug_sscop_events, debug_sscop_events_eol, debug_sscop_packets,
         "events", "SSCOP events", PRIV_OPR);

/***************************************************************************
 * debug sscop
 *
 */
KEYWORD_DEBUG(debug_sscop, debug_sscop_events, ALTERNATE,
	      OBJ(pdb,1), sscop_debug_arr,
	      "sscop", "SSCOP", PRIV_OPR);

#undef  ALTERNATE
#define ALTERNATE       debug_sscop
