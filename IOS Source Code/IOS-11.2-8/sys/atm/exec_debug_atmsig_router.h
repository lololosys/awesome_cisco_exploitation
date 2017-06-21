/* $Id: exec_debug_atmsig_router.h,v 3.1.4.1 1996/04/25 23:02:40 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/atm/exec_debug_atmsig_router.h,v $
 *------------------------------------------------------------------
 * exec_debug_atmsig_router.h:  DEBUG command parsing code
 *
 * March, 1996,  Rob Zagst 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_atmsig_router.h,v $
 * Revision 3.1.4.1  1996/04/25  23:02:40  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/30  05:56:13  rzagst
 * Place holder for atm modularity files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************************
 * debug atm smap-all
 *
 */
EOLS    (debug_atmsig_smap_all_eol, debug_command, DEBUG_ATMSIG_SMAP_ALL);
KEYWORD_DEBUG (debug_atmsig_smap_all, debug_atmsig_smap_all_eol, ALTERNATE,
	       OBJ(pdb,1),atmsig_debug_arr,
         "smap-all", "ATM Signalling Static Map all", PRIV_OPR);

/****************************************************************************
 * debug atm smap-events
 *
 */
EOLS    (debug_atmsig_smap_events_eol, debug_command, DEBUG_ATMSIG_SMAP_EV);
KEYWORD_DEBUG (debug_atmsig_smap_events, debug_atmsig_smap_events_eol, 
         debug_atmsig_smap_all,OBJ(pdb,1),atmsig_debug_arr,
         "smap-events", "ATM Signalling Static Map events", PRIV_OPR);

/****************************************************************************
 * debug atm smap-error
 *
 */
EOLS    (debug_atmsig_smap_error_eol, debug_command, DEBUG_ATMSIG_SMAP_ER);
KEYWORD_DEBUG (debug_atmsig_smap_error, debug_atmsig_smap_error_eol, 
         debug_atmsig_smap_events,OBJ(pdb,1),atmsig_debug_arr,"smap-error",
         "ATM Signalling Static Map errors", PRIV_OPR);

#undef  ALTERNATE
#define ALTERNATE       debug_atmsig_smap_events

