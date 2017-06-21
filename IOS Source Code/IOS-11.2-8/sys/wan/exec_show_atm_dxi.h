/* $Id: exec_show_atm_dxi.h,v 3.2.62.1 1996/05/21 10:09:30 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/exec_show_atm_dxi.h,v $
 *------------------------------------------------------------------
 * Support for ATM-DXI encapsulation and RFC1483 support
 *
 * October 1994, Shankar Natarajan
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_atm_dxi.h,v $
 * Revision 3.2.62.1  1996/05/21  10:09:30  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2  1995/11/17  18:02:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:08  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/21  18:17:50  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:22:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*****************************************************************
 * show atm dxi pvc [<interface>] [vpi] [vci]
 */
EOLS 	(show_atm_dxi_pvc_eol, show_atm_dxi, SHOW_ATM_DXI_PVC);

pdecl 	(show_atm_dxi_pvc_opts);

NUMBER 	(show_atm_dxi_pvc_vci, show_atm_dxi_pvc_eol, no_alt,
	OBJ(int,2), 0, 63,"VCI");

NUMBER 	(show_atm_dxi_pvc_vpi, show_atm_dxi_pvc_vci, no_alt,
	OBJ(int,1), 0, 15,"VPI");

INTERFACE (show_atm_dxi_pvc_int, show_atm_dxi_pvc_opts, show_atm_dxi_pvc_vpi,
	OBJ(idb,1), IFTYPE_SERIAL | IFTYPE_HSSI | IFTYPE_TUNNEL);

KEYWORD (show_atm_dxi_pvc_interface, show_atm_dxi_pvc_int, 
	show_atm_dxi_pvc_vpi, common_str_interface, "", PRIV_USER);

NOP (show_atm_dxi_pvc_opts, show_atm_dxi_pvc_interface, show_atm_dxi_pvc_eol);

KEYWORD (show_atm_dxi_pvc, show_atm_dxi_pvc_opts, no_alt,
	"pvc", "show atm-dxi pvc statistics", PRIV_USER);

/******************************************************************
 * show atm dxi map
 */

EOLS	(show_atm_dxi_map_eol, show_atm_dxi, SHOW_ATM_DXI_MAP);

KEYWORD (show_atm_dxi_map, show_atm_dxi_map_eol, show_atm_dxi_pvc,
	 "map", "atm-dxi map table", PRIV_USER);

KEYWORD (show_atm_dxi, show_atm_dxi_map, ALTERNATE,
	 "dxi", "atm-dxi information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_atm_dxi
