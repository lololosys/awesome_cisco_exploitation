/* $Id: exec_debug_sntp.h,v 3.1.2.2 1996/06/25 22:55:27 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/exec_debug_sntp.h,v $
 *------------------------------------------------------------------
 * Parser chains for SNTP "debug" commands
 *
 * June 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_sntp.h,v $
 * Revision 3.1.2.2  1996/06/25  22:55:27  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add rudimentary clock selection logic.  Get rid of most static storage.
 * Keep track of all broadcast servers.
 *
 * Revision 3.1.2.1  1996/06/23  04:40:23  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add an SNTP client.  This is not included in shipping IOS images.
 *
 * Revision 3.1  1996/06/22  21:42:10  dkatz
 * Add placeholders.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug sntp select
 *
 */
EOLS    (debug_sntp_select_eol, debug_command, DEBUG_SNTP_SELECT);
KEYWORD (debug_sntp_select, debug_sntp_select_eol, no_alt,
	 "select", "SNTP clock selection", PRIV_OPR);

/******************************************************************
 * debug sntp adjust
 *
 */
EOLS    (debug_sntp_adjust_eol, debug_command, DEBUG_SNTP_ADJUST);
KEYWORD (debug_sntp_adjust, debug_sntp_adjust_eol, debug_sntp_select,
	 "adjust", "SNTP clock adjustments", PRIV_OPR);

/******************************************************************
 * debug sntp packets
 *
 */
EOLS    (debug_sntp_packets_eol, debug_command, DEBUG_SNTP_PACKETS);
KEYWORD (debug_sntp_packets, debug_sntp_packets_eol, debug_sntp_adjust, 
	 "packets", "SNTP packets", PRIV_OPR);

/******************************************************************
 * debug sntp
 *
 */
KEYWORD_DEBUG(debug_sntp, debug_sntp_packets, ALTERNATE,
	      OBJ(pdb,1), sntp_debug_arr,
	      "sntp", "SNTP information", PRIV_OPR);

#undef ALTERNATE
#define ALTERNATE debug_sntp
