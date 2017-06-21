/* $Id: cfg_rif.h,v 3.3.20.1 1996/08/12 16:02:23 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_rif.h,v $
 *------------------------------------------------------------------
 * C F G _ R I F . H
 *
 * Source-route bridging support
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_rif.h,v $
 * Revision 3.3.20.1  1996/08/12  16:02:23  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/01  23:33:33  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:26:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:14  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:12:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * rif <macaddr> [<rif-string>] [{ <interface> | ring-group <ring> }]
 * no rif <macaddr> [{ <interface> | ring-group <ring> }]
 *
 * (*OBJ(hwaddr,1)) = <macaddr>
 * OBJ(int,1) = number of <rif-string> words
 * csb->uiarray[0..N-1] = <rif-string> words
 * OBJ(int,2) = <ring>
 * OBJ(idb,1) = <interface> 
 *
 * The RIF string could
 * also be done by gathering it with a STRING macro and
 * have the command action routine parse it.  This would make
 * error handling much easier.
 */

pdecl(rif_mac_1);

EOLS	(rif_mac_eol, tr_rif_command, RIF_ADDRESS);

SET	(rif_mac_interface_set, rif_mac_eol, OBJ(int,4), TRUE);
INTERFACE(rif_mac_interface, rif_mac_interface_set,
	  rif_mac_eol, OBJ(idb,1), IFTYPE_TOKENRING);

NUMBER	(rif_mac_ring_num, rif_mac_eol, no_alt, OBJ(int,3), 1, 4095,
	 "Ring group number");
KEYWORD	(rif_mac_ring, rif_mac_ring_num, rif_mac_interface,
	 "ring-group", "Specify a ring group as this entry's origin",
	 PRIV_CONF);

/* XXX <rif-string> - this loops to gather all the RIF numbers.  */
WHITESPACE(rif_mac_3b, rif_mac_ring, rif_mac_1);
HELP	(rif_mac_3a, rif_mac_1, "H.H...\t");
CHARACTER(rif_mac_3, rif_mac_3a, rif_mac_3b, '.');
/* Make sure that the user doesn't attempt to give more than we can handle */
PRINT	(rif_mac_2b, no_alt, "RIF string too long");
TESTVAR	(rif_mac_2a, rif_mac_2b, rif_mac_3, 
	 NONE, NONE, NONE, OBJ(int,1), SRB_MAXRIF/2);
EVAL	(rif_mac_2, rif_mac_2a,
	 csb->uiarray[GETOBJ(int,1) ++] = GETOBJ(int,2));
HELP	(rif_mac_1a, rif_mac_2, "H.H...\t");
HEXADECIMAL(rif_mac_1, rif_mac_1a, no_alt, OBJ(int,2));
HEXADECIMAL(rif_mac_0, rif_mac_1a, rif_mac_ring, OBJ(int,2));
HELP	(rif_mac_rif, rif_mac_0, "  H.H...\tRIF entry\n");

/* <macaddr> */
ENADDR	(rif_mac_enaddr, rif_mac_rif, no_alt, OBJ(hwaddr,1),
	 "MAC address to enter in RIF cache");
NVGENS	(rif_mac, rif_mac_enaddr, tr_rif_command, RIF_ADDRESS);

/*****************************************************************
 *	rif timeout <minutes>
 *	no rif timeout [<minutes>]
 */

PARAMS_KEYONLY (rif_timeout, rif_mac,
		"timeout", OBJ(int,1), 1, 120, 
		tr_rif_command, RIF_TIMEOUT,
		"Set timeout on RIF cache entries", "Minutes", PRIV_CONF);


/*****************************************************************
 * rif validate-age
 */
EOLS	(rif_validate_age_eol, tr_rif_command, RIF_VALIDATE_ENABLE_AGE);
KEYWORD	(rif_validation_age, rif_validate_age_eol, rif_timeout,
	 "validate-age", "Remove invalidated and aged out rif entries",
	 PRIV_CONF);

/*****************************************************************
 * rif validate-route-cache
 */
EOLNS	(rif_validate_rc_eol, parser_set_keyword);
KEYWORD_ID(rif_validate_rc, rif_validate_rc_eol, rif_validation_age,
	   OBJ(int,1), (int)&rif_validate_route_cache,
	   "validate-route-cache",
	   "Enable invalidating all route-caches when "
	   "RIF cache has been changed",
	   PRIV_CONF);

/*****************************************************************
 * rif validate-enable
 */
EOLS	(rif_validate_en_eol, tr_rif_command, RIF_VALIDATE_ENABLE);
KEYWORD	(rif_validate_en, rif_validate_en_eol, rif_validate_rc,
	 "validate-enable", "Enable rif validation",
	 PRIV_CONF);

/*****************************************************************
 *	rif validate-time <seconds>
 *	no rif validate-time [<seconds>]
 */

PARAMS_KEYONLY (rif_validate, rif_validate_en,
		"validate-time", OBJ(int,1), 1, 120, 
		tr_rif_command, RIF_VALIDATE,
		"Set validation timer on a proxy", "Seconds", PRIV_CONF);

/*****************************************************************
 * rif always-forward
 */
EOLS	(rif_always_forward_eol, tr_rif_command, RIF_ALWAYS_FORWARD);
KEYWORD	(rif_always_forward, rif_always_forward_eol, rif_validate,
	 "always-forward", "Always store RIFs in forward direction",
	 PRIV_CONF);

/*****************************************************************
 * rif xid-explorer
 */

EOLNS	(rif_xid_exp_eol, parser_set_keyword);
KEYWORD_ID(rif_xid_exp, rif_xid_exp_eol, rif_always_forward, 
	   OBJ(int,1), (int)&tr_rif_xid_explorer,
	   "xid-explorer", "Send IEEE XID explorer instead of TEST", PRIV_CONF);

/*****************************************************************/
KEYWORD	(rif, rif_xid_exp, NONE,
	 "rif", "Source-route RIF cache", PRIV_CONF);

ASSERT (rif_xid, rif, ALTERNATE, (bridge_enable || nTRs));

#undef ALTERNATE
#define ALTERNATE	rif_xid
