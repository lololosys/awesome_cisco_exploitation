/* $Id: exec_show_ilmi.h,v 3.1.60.2 1996/06/18 06:30:59 kartik Exp $
 * $Source: /release/112/cvs/Xsys/atm/exec_show_ilmi.h,v $
 *------------------------------------------------------------------
 * Show Commands for ILMI.
 *
 * October 1995, Kartik Chandrasekhar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_ilmi.h,v $
 * Revision 3.1.60.2  1996/06/18  06:30:59  kartik
 * Branch: California_branch
 * CSCdi50421:  show atm ilmi command returns error when no interface
 * provided
 *              Added more details to show.
 *
 * Revision 3.1.60.1  1996/04/25  23:02:44  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/03/30  03:34:55  rzagst
 * ATM Modularity: Clean up and modularize by platform the ATM parser
 * commands.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.1  1995/11/09  11:00:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/17  19:09:38  kartik
 * placeholder files for the Rhino Branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ATM ilmi <interface>
 * OBJ(idb,1) = <interface>
 */
EOLS	(show_atm_ilmi_eol, ilmi_show_commands, ILMI_SHOW_ATM_IF_ILMI);
INTERFACE (show_atm_ilmi_int, show_atm_ilmi_eol, show_atm_ilmi_eol,
	   OBJ(idb,1), (IFTYPE_ATM | IFTYPE_SUBIF));
KEYWORD	(show_atm_ilmi, show_atm_ilmi_int, ALTERNATE,
	 "ilmi", "ATM ILMI Information", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_atm_ilmi

















