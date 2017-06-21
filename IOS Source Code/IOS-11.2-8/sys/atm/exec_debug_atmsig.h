/* $Id: exec_debug_atmsig.h,v 3.2 1995/11/17 08:46:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atm/exec_debug_atmsig.h,v $
 *------------------------------------------------------------------
 * parser/exec_debug_atmsig.h:  DEBUG command parsing code
 *
 * November, 1993,  Percy P. Khabardar 
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All Rights Reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_atmsig.h,v $
 * Revision 3.2  1995/11/17  08:46:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  09:35:14  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.2  1995/06/19  14:47:19  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:17:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************************
 * debug atm smap-all
 *
 */
EOLS    (debug_atmsig_smap_all_eol, debug_command, DEBUG_ATMSIG_SMAP_ALL);
KEYWORD (debug_atmsig_smap_all, debug_atmsig_smap_all_eol, no_alt,
         "smap-all", "ATM Signalling Static Map all", PRIV_OPR);

/****************************************************************************
 * debug atm smap-events
 *
 */
EOLS    (debug_atmsig_smap_events_eol, debug_command, DEBUG_ATMSIG_SMAP_EV);
KEYWORD (debug_atmsig_smap_events, debug_atmsig_smap_events_eol, 
         debug_atmsig_smap_all,
         "smap-events", "ATM Signalling Static Map events", PRIV_OPR);

/****************************************************************************
 * debug atm smap-error
 *
 */
EOLS    (debug_atmsig_smap_error_eol, debug_command, DEBUG_ATMSIG_SMAP_ER);
KEYWORD (debug_atmsig_smap_error, debug_atmsig_smap_error_eol, 
         debug_atmsig_smap_events, "smap-error",
         "ATM Signalling Static Map errors", PRIV_OPR);

/****************************************************************************
 * debug atm sig-all
 *
 */
EOLS    (debug_atmsig_all_eol, debug_command, DEBUG_ATMSIG_ALL);
KEYWORD (debug_atmsig_all, debug_atmsig_all_eol, debug_atmsig_smap_error,
         "sig-all", "ATM Signalling all", PRIV_OPR);

/****************************************************************************
 * debug atm sig-ie
 *
 */
EOLS    (debug_atmsig_ie_eol, debug_command, DEBUG_ATMSIG_IE);
KEYWORD (debug_atmsig_ie, debug_atmsig_ie_eol, debug_atmsig_all,
         "sig-ie", "ATM Signalling information elements", PRIV_OPR);

/****************************************************************************
 * debug atm sig-error
 *
 */
EOLS    (debug_atmsig_error_eol, debug_command, DEBUG_ATMSIG_ERROR);
KEYWORD (debug_atmsig_error, debug_atmsig_error_eol, debug_atmsig_ie,
         "sig-error", "ATM Signalling errors", PRIV_OPR);

/****************************************************************************
 * debug atm sig-packets
 *
 */
EOLS    (debug_atmsig_packets_eol, debug_command, DEBUG_ATMSIG_PAK);
KEYWORD (debug_atmsig_packets, debug_atmsig_packets_eol, debug_atmsig_error,
         "sig-packets", "ATM Signalling packets", PRIV_OPR);

/***************************************************************************
 * debug atm sig-events
 *
 */
EOLS    (debug_atmsig_events_eol, debug_command, DEBUG_ATMSIG_EVENT);
KEYWORD (debug_atmsig_events, debug_atmsig_events_eol, debug_atmsig_packets,
         "sig-events", "ATM Signalling events", PRIV_OPR);

/***************************************************************************
 * debug atm
 *
 */
KEYWORD_DEBUG(debug_atmsig, debug_atmsig_events, ALTERNATE,
	      OBJ(pdb,1), atmsig_debug_arr,
	      "atm", "ATM Signalling", PRIV_OPR);

#undef  ALTERNATE
#define ALTERNATE       debug_atmsig
