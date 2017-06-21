/* $Id: exec_clear_csu_dsu.h,v 3.2 1996/03/29 03:49:37 sagarwal Exp $
 * $Source: /release/111/cvs/Xsys/les/exec_clear_csu_dsu.h,v $
 *------------------------------------------------------------------
 * Clear service-module command
 *
 * March 1996, Johnny Chan
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_clear_csu_dsu.h,v $
 * Revision 3.2  1996/03/29  03:49:37  sagarwal
 * CSCdi46172:  Add Boa support to 11.1 mainline
 *
 * Revision 3.1  1996/03/28  20:06:18  sagarwal
 * Placeholder files for 2500 Derivative with integrated CSU/DSU
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear service-module [<interface-name>]
 *
 * OBJ(int,1) = TRUE if <interface-name> was given
 * csb->interface = idb of specified interface
 */
EOLS    (priv_clear_csu_dsu_eol, csu_dsu_clear_command, CSU_DSU_CLEAR);

INTERFACE (priv_clear_csu_dsu_int, priv_clear_csu_dsu_eol, 
	   no_alt, OBJ(idb,1),
           (IFTYPE_SERIAL & ~IFTYPE_PCBUS));

KEYWORD (priv_clear_csu_dsu_kw, priv_clear_csu_dsu_int, ALTERNATE,
	 "service-module", "Service module", PRIV_OPR);

ASSERT (priv_clear_csu_dsu, priv_clear_csu_dsu_kw, no_alt,
        reg_invoke_csu_dsu_installed(CHK_ANY_CSU_DSU, NULL));

#undef	ALTERNATE
#define	ALTERNATE	priv_clear_csu_dsu
