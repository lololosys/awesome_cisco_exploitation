/* $Id: exec_debug_ilmi.h,v 3.1.62.1 1996/04/25 23:02:41 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/atm/exec_debug_ilmi.h,v $
 *------------------------------------------------------------------
 * Debug Commands for ILMI.
 *
 * October 1995, Kartik Chandrasekhar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_ilmi.h,v $
 * Revision 3.1.62.1  1996/04/25  23:02:41  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.66.1  1996/03/30  03:34:52  rzagst
 * ATM Modularity: Clean up and modularize by platform the ATM parser
 * commands.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.1  1995/11/09  11:00:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/17  19:09:37  kartik
 * placeholder files for the Rhino Branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/******************************************************************
 * debug ATM ILMI
 */
EOLS    (debug_atm_ilmi_eol,debug_command, DEBUG_ATMILMI);
KEYWORD (debug_atm_ilmi, debug_atm_ilmi_eol,no_alt,
	 "ilmi", "Show ILMI events", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_atm_ilmi


