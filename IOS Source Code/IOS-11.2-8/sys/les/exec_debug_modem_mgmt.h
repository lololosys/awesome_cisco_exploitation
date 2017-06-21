/* $Id: exec_debug_modem_mgmt.h,v 3.1.68.1 1996/06/16 21:14:15 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_debug_modem_mgmt.h,v $
 *------------------------------------------------------------------
 * A dummy file for brasil branch.
 * E X E C _ D E B U G _ M O D E M _ M G M T . H
 *
 * November, 1995 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_modem_mgmt.h,v $
 * Revision 3.1.68.1  1996/06/16  21:14:15  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/06  19:12:22  jchlin
 * Placeholders for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
LINK_TRANS(debug_modem_mgmt_extend_here, no_alt);

/******************************************************************
 * debug modem-mgmt csm
 *
 */
EOLS	(debug_modem_mgmt_csm_eol, debug_command,
	 DEBUG_MODEM_MGMT_CSM);

KEYWORD (debug_modem_mgmt_csm, debug_modem_mgmt_csm_eol,
	 debug_modem_mgmt_extend_here, "csm", "Modem Management Call Switching Module",
	 PRIV_USER);

SET (debug_modem_mgmt_set, debug_modem_mgmt_csm, 
     OBJ(pdb,1), modem_mgmt_debug_arr);

KEYWORD_MM (debug_modem_mgmt, debug_modem_mgmt_set, ALTERNATE,
            MODEM_MGMT_KEYWORD, "Modem Management debugging", PRIV_USER, 6);

#undef	ALTERNATE
#define	ALTERNATE	debug_modem_mgmt

