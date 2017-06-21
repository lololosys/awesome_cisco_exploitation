/* $Id: exec_debug_ntp.h,v 3.2.62.1 1996/04/16 19:02:57 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/ntp/exec_debug_ntp.h,v $
 *------------------------------------------------------------------
 * NTP debug command parser chains
 *
 * Dave Katz, November 1992
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_debug_ntp.h,v $
 * Revision 3.2.62.1  1996/04/16  19:02:57  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:50:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  14:46:41  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  21:44:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug ntp validity
 *
 */
EOLS    (debug_ntp_validity_eol, debug_command, DEBUG_NTP_VALIDITY);
KEYWORD (debug_ntp_validity, debug_ntp_validity_eol, no_alt,
	 "validity", "NTP peer clock validity", PRIV_OPR);

/******************************************************************
 * debug ntp refclock
 *
 */
EOLS    (debug_ntp_refclock_eol, debug_command, DEBUG_NTP_REFCLOCK);
KEYWORD (debug_ntp_refclock, debug_ntp_refclock_eol, debug_ntp_validity,
	 "refclock", "NTP reference clocks", PRIV_OPR);

/******************************************************************
 * debug ntp sync
 *
 */
EOLS    (debug_ntp_sync_eol, debug_command, DEBUG_NTP_SYNC);
KEYWORD (debug_ntp_sync, debug_ntp_sync_eol, debug_ntp_refclock,
	 "sync", "NTP clock synchronization", PRIV_OPR);

/******************************************************************
 * debug ntp select
 *
 */
EOLS    (debug_ntp_select_eol, debug_command, DEBUG_NTP_SELECT);
KEYWORD (debug_ntp_select, debug_ntp_select_eol, debug_ntp_sync,
	 "select", "NTP clock selection", PRIV_OPR);

/******************************************************************
 * debug ntp params
 *
 */
EOLS    (debug_ntp_params_eol, debug_command, DEBUG_NTP_PARAMS);
KEYWORD (debug_ntp_params, debug_ntp_params_eol, debug_ntp_select,
	 "params", "NTP clock parameters", PRIV_OPR);

/******************************************************************
 * debug ntp loopfilter
 *
 */
EOLS    (debug_ntp_loopfilter_eol, debug_command, DEBUG_NTP_LOOPFILTER);
KEYWORD (debug_ntp_loopfilter, debug_ntp_loopfilter_eol, debug_ntp_params,
	 "loopfilter", "NTP loop filter", PRIV_OPR);

/******************************************************************
 * debug ntp events
 *
 */
EOLS    (debug_ntp_event_eol, debug_command, DEBUG_NTP_EVENT);
KEYWORD (debug_ntp_event, debug_ntp_event_eol, debug_ntp_loopfilter,
	 "events", "NTP events", PRIV_OPR);

/******************************************************************
 * debug ntp adjust
 *
 */
EOLS    (debug_ntp_adjust_eol, debug_command, DEBUG_NTP_ADJUST);
KEYWORD (debug_ntp_adjust, debug_ntp_adjust_eol, debug_ntp_event,
	 "adjust", "NTP clock adjustments", PRIV_OPR);

/******************************************************************
 * debug ntp packets
 *
 */
EOLS    (debug_ntp_packets_eol, debug_command, DEBUG_NTP_PACKETS);
KEYWORD (debug_ntp_packets, debug_ntp_packets_eol, debug_ntp_adjust, 
	 "packets", "NTP packets", PRIV_OPR);

/******************************************************************
 * debug ntp
 *
 */
KEYWORD_DEBUG(debug_ntp, debug_ntp_packets, ALTERNATE,
	      OBJ(pdb,1), ntp_debug_arr,
	      "ntp", "NTP information", PRIV_OPR);

#undef ALTERNATE
#define ALTERNATE debug_ntp
