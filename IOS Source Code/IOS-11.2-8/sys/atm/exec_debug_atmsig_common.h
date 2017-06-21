/* $Id: exec_debug_atmsig_common.h,v 3.1.4.1 1996/04/25 23:02:39 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/atm/exec_debug_atmsig_common.h,v $
 *------------------------------------------------------------------
 * exec_debug_atmsig_common.h:  DEBUG command parsing code
 *
 * March, 1996,  Rob Zagst
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_atmsig_common.h,v $
 * Revision 3.1.4.1  1996/04/25  23:02:39  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/30  05:53:42  rzagst
 * Place holder for atm modularity files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************************
 * debug atm sig-all
 *
 */
EOLS    (debug_atmsig_all_eol, debug_command, DEBUG_ATMSIG_ALL);
KEYWORD_DEBUG (debug_atmsig_all, debug_atmsig_all_eol,
	       debug_atmsig_smap_error,OBJ(pdb,1),atmsig_debug_arr,
         "sig-all", "ATM Signalling all", PRIV_OPR);

/****************************************************************************
 * debug atm sig-ie
 *
 */
EOLS    (debug_atmsig_ie_eol, debug_command, DEBUG_ATMSIG_IE);
KEYWORD_DEBUG (debug_atmsig_ie, debug_atmsig_ie_eol, debug_atmsig_all,
	       OBJ(pdb,1),atmsig_debug_arr,
         "sig-ie", "ATM Signalling information elements", PRIV_OPR);

/****************************************************************************
 * debug atm sig-error
 *
 */
EOLS    (debug_atmsig_error_eol, debug_command, DEBUG_ATMSIG_ERROR);
KEYWORD_DEBUG (debug_atmsig_error, debug_atmsig_error_eol, debug_atmsig_ie,
	       OBJ(pdb,1),atmsig_debug_arr,
         "sig-error", "ATM Signalling errors", PRIV_OPR);

/****************************************************************************
 * debug atm sig-packets
 *
 */
EOLS    (debug_atmsig_packets_eol, debug_command, DEBUG_ATMSIG_PAK);
KEYWORD_DEBUG (debug_atmsig_packets, debug_atmsig_packets_eol,
	       debug_atmsig_error,OBJ(pdb,1),atmsig_debug_arr,
         "sig-packets", "ATM Signalling packets", PRIV_OPR);

/***************************************************************************
 * debug atm sig-events
 *
 */
EOLS    (debug_atmsig_events_eol, debug_command, DEBUG_ATMSIG_EVENT);
KEYWORD_DEBUG (debug_atmsig_events, debug_atmsig_events_eol,
	       debug_atmsig_packets,OBJ(pdb,1),atmsig_debug_arr,
         "sig-events", "ATM Signalling events", PRIV_OPR);


#undef  ALTERNATE
#define ALTERNATE       debug_atmsig_events

