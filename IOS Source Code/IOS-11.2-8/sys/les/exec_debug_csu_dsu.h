/* $Id: exec_debug_csu_dsu.h,v 3.2 1996/03/29 03:49:37 sagarwal Exp $
 * $Source: /release/111/cvs/Xsys/les/exec_debug_csu_dsu.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ C S U _ D S U . H                  
 *
 * March 1996, Johnny Chan
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_csu_dsu.h,v $
 * Revision 3.2  1996/03/29  03:49:37  sagarwal
 * CSCdi46172:  Add Boa support to 11.1 mainline
 *
 * Revision 3.1  1996/03/28  20:06:19  sagarwal
 * Placeholder files for 2500 Derivative with integrated CSU/DSU
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug service-module
 *
 */
EOLS	(debug_csu_dsu_eol, debug_command, DEBUG_SERVICE_MODULE);
KEYWORD_DEBUG(debug_csu_dsu, debug_csu_dsu_eol,
	      ALTERNATE,
	      OBJ(pdb,1), csu_dsu_debug_arr,
	      "service-module", "Service module events",
	      PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_csu_dsu
