/* $Id: exec_show_csu_dsu.h,v 3.2 1996/03/29 03:49:38 sagarwal Exp $
 * $Source: /release/111/cvs/Xsys/les/exec_show_csu_dsu.h,v $
 *------------------------------------------------------------------
 * Show service-module command
 *
 * March 1996, Johnny Chan
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_show_csu_dsu.h,v $
 * Revision 3.2  1996/03/29  03:49:38  sagarwal
 * CSCdi46172:  Add Boa support to 11.1 mainline
 *
 * Revision 3.1  1996/03/28  20:06:21  sagarwal
 * Placeholder files for 2500 Derivative with integrated CSU/DSU
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show service-module [<interface-name>] {cr | performance [interval-range 1-96]}
 *
 * OBJ(int,1) = TRUE if <interface-name> was given
 * OBJ(int,2) = <cr> | performance_statistics
 * OBJ(int,3) = start range, def set to 0
 * OBJ(int,4) = end range, def set to 0
 * csb->interface = idb of specified interface
 */
EOLS    (priv_show_csu_dsu_eol, csu_dsu_show_command, CSU_DSU_SHOW);

GENERAL_NUMBER (priv_show_csu_dsu_num2, priv_show_csu_dsu_eol,
                priv_show_csu_dsu_eol, 
		OBJ(int,4), 1, 96, "List of performance statistics interval is set for 24",
                NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);

WHITESPACE (priv_show_csu_dsu_num1w, priv_show_csu_dsu_num2,
            priv_show_csu_dsu_num2);

CHARACTER (priv_show_csu_dsu_num1a, priv_show_csu_dsu_num1w,
           no_alt, '-');

HELP	(priv_show_csu_dsu_dash_help, priv_show_csu_dsu_num1a,
	 "-");

GENERAL_NUMBER (priv_show_csu_dsu_num1, priv_show_csu_dsu_dash_help, 
                priv_show_csu_dsu_eol,
                OBJ(int,3), 1, 96, "List of performance statistics interval <1-96>",
                NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);

SET (priv_show_csu_dsu_num0b, priv_show_csu_dsu_num1, OBJ(int,4), 0);

SET (priv_show_csu_dsu_num0a, priv_show_csu_dsu_num0b, OBJ(int,3), 0);

KEYWORD_ID(priv_show_csu_dsu_perf, priv_show_csu_dsu_num0a,
           no_alt, OBJ(int,2), CSU_DSU_SHOW_PERF_STAT, "performance-statistics",
           "Performance statistics",
           PRIV_CONF);

ASSERT  (priv_show_csu_dsu_chk_ft1, priv_show_csu_dsu_perf,
	 priv_show_csu_dsu_eol, (GETOBJ(idb,1)->hwptr->csu_dsu_type & CSU_DSU_FT1));

SET (priv_show_csu_dsu_set_def, priv_show_csu_dsu_chk_ft1,
     OBJ(int,2), CSU_DSU_SHOW_DEF);

SET   (priv_show_csu_dsu_set, priv_show_csu_dsu_set_def, OBJ(int,1), TRUE);

INTERFACE (priv_show_csu_dsu_int, priv_show_csu_dsu_set, 
	   priv_show_csu_dsu_eol, OBJ(idb,1),
           (IFTYPE_SERIAL & ~IFTYPE_PCBUS));

SET   (priv_show_csu_dsu_no_set, priv_show_csu_dsu_int, OBJ(int,1), FALSE);

KEYWORD (priv_show_csu_dsu_kw, priv_show_csu_dsu_no_set, no_alt,
	 "service-module", "Service module", PRIV_OPR);

ASSERT (priv_show_csu_dsu, priv_show_csu_dsu_kw, no_alt,
        reg_invoke_csu_dsu_installed(CHK_ANY_CSU_DSU, NULL));

#undef	ALTERNATE
#define	ALTERNATE	priv_show_csu_dsu
