/* $Id: cfg_int_x25_sw.h,v 3.1.2.2 1996/05/21 10:10:32 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/cfg_int_x25_sw.h,v $
 *------------------------------------------------------------------
 * X.25 switch client interface parsing support
 *
 * April 1996, Gilbert Glick
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_x25_sw.h,v $
 * Revision 3.1.2.2  1996/05/21  10:10:32  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.2.1  1996/04/11  23:47:41  gglick
 * CSCdi54514:  X.25 parser modularity files
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/11  23:40:02  gglick
 * Initial commit of X.25 parser modularization files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/************************************************************************
 *      ...[options]
 * (switch PVC command option external linkage)
 */

LINK_EXIT(ci_x25sw_pvc_exit, no_alt);

/* csb->uiarray[0] is set to the permissible PVC options */
SET	(x25sw_pvc_opt, ci_x25sw_pvc_exit, uiarray[0], X25OPT_SW_OPTIONS);

/************************************************************************
 *	(x25 pvc <#>) interface <ifc> pvc <#>...
 * PVC switched locally
 *
 *  OBJ(idb,1) is set to the destination interface
 *  csb->uiarray[2] is set to the destination PVC number
 */

NUMBER	(x25sw_lcl_pvc_val, x25sw_pvc_opt, NONE,
	 uiarray[2], 1, 4095, "Destination channel number");
KEYWORD	(x25sw_lcl_pvc, x25sw_lcl_pvc_val, NONE,
	 "pvc", "Specify destination PVC", PRIV_CONF);
INTERFACE(x25sw_lcl_ifc_id, x25sw_lcl_pvc, NONE, OBJ(idb,1), IFTYPE_SERIAL);

KEYWORD	(x25sw_pvc_local, x25sw_lcl_ifc_id, NONE,
	 common_str_interface, "Switch PVC to a local X.25 interface", PRIV_CONF);

/************************************************************************
 *	(x25 pvc <#>) xot <dest-addr> interface Serial <ID> pvc <#>
 *			  [xot-source <ifc>]
 *			  [xot-keepalive-period <#>]
 *			  [xot-keepalive-tries <#>]...
 * PVC switched over XOT
 *
 *  OBJ(paddr,1) is set to the remote XOT host's address
 *  OBJ(string,1) is set to the remote XOT serial interface ID (e.g. '1')
 *  csb->uiarray[2] is set to the remote XOT serial interface PVC number
 *  OBJ(idb,1) is set to the source interface to use, if specified
 *  csb->uiarray[3] is set to the XOT keepalive period to use, if specified
 *  csb->uiarray[4] is set to the XOT keepalive tries to use, if specified
 */

/* xot-keepalive-tries <number> */
NUMBER	(x25sw_xot_tries_val, x25sw_pvc_opt, NONE,
	 uiarray[4], 3, 1000, "Keepalive tries");
KEYWORD	(x25sw_xot_tries, x25sw_xot_tries_val, x25sw_pvc_opt,
	 "xot-keepalive-tries", "Specify XOT/TCP keepalive limit",
	 PRIV_CONF);

/* xot-keepalive-period <number> */
NUMBER	(x25sw_xot_period_val, x25sw_xot_tries, NONE,
	 uiarray[3], 1, 3600, "Keepalive period in seconds");
KEYWORD	(x25sw_xot_period, x25sw_xot_period_val, x25sw_xot_tries,
	 "xot-keepalive-period", "Specify XOT/TCP keepalive period to use",
	 PRIV_CONF);

/* xot-source <interface> */
INTERFACE (x25sw_xot_src_ifc, x25sw_xot_period, NONE,
	   OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD	(x25sw_xot_src, x25sw_xot_src_ifc, x25sw_xot_period,
	 "xot-source", "Specify source interface for the XOT connection",
	 PRIV_CONF);

/* <dest> interface Serial <ID> pvc <#> */
NUMBER	(x25sw_xot_pvc, x25sw_xot_src, NONE,
	 uiarray[2], 1, 4095, "Remote host channel number");
KEYWORD	(x25sw_xot_pvc_kwd, x25sw_xot_pvc, NONE,
	 "pvc", "Specify remote host PVC", PRIV_CONF);
STRING	(x25sw_xot_serial_id, x25sw_xot_pvc_kwd, NONE,
	 OBJ(string,1), "Remote interface ID");
KEYWORD	(x25sw_xot_serial_kwd, x25sw_xot_serial_id, NONE,
	 "Serial", "Remote interface type", PRIV_CONF);
KEYWORD	(x25sw_xot_ifc_kwd, x25sw_xot_serial_kwd, NONE,
	 common_str_interface, "Specify remote interface name", PRIV_CONF);
IPADDR	(x25sw_xot_host, x25sw_xot_ifc_kwd, NONE,
	 OBJ(paddr,1), "IP address of remote XOT host");

KEYWORD	(x25sw_pvc_tunnel, x25sw_xot_host, NONE,
	 "tunnel", "", PRIV_CONF | PRIV_HIDDEN);
KEYWORD	(x25sw_pvc_xot, x25sw_xot_host, x25sw_pvc_tunnel,
	 "xot", "Switch PVC using XOT (X.25-Over-TCP)", PRIV_CONF);

/************************************************************************/

NOP	(x25sw_pvc_xot_or_local, x25sw_pvc_xot, x25sw_pvc_local);
SET	(x25sw_pvc_set, x25sw_pvc_xot_or_local, uiarray[1], LINK_X25);

/* OBJ(int,1) is 0 for a map, non-zero for a PVC */
SPLIT	(x25sw_mappvc_split, NONE, x25sw_pvc_set, OBJ(int,1), 0);

NOP	(x25sw_mappvc_enter, x25sw_mappvc_split, ALTERNATE);

#undef	ALTERNATE
#define	ALTERNATE	x25sw_mappvc_enter
