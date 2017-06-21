/* $Id: cfg_int_atm_dxi.h,v 3.3 1996/02/13 04:22:03 shankar Exp $
 * $Source: /release/111/cvs/Xsys/wan/cfg_int_atm_dxi.h,v $
 *------------------------------------------------------------------
 * Support for ATM-DXI encapsulation and RFC1483 support
 *
 *  October 1994, Shankar Natarajan
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_atm_dxi.h,v $
 * Revision 3.3  1996/02/13  04:22:03  shankar
 * CSCdi48025:  DXI ATM - DXI MAP DISSAPEARS AFTER RELOAD
 *
 * Revision 3.2  1995/11/17  18:00:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:47:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:46  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.3  1995/06/21  18:16:34  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/21  03:12:42  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  22:03:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/***************************************************************
 * atm-dxi zap
 */
EOLS    (ci_atm_dxi_zap_eol, atm_dxi_command, ATM_DXI_ZAP);
KEYWORD (ci_atm_dxi_zap, ci_atm_dxi_zap_eol, no_alt,
	"zap", "Clear (zap) all atm-dxi maps",
	PRIV_CONF | PRIV_HIDDEN);

/***************************************************************
 * atm-dxi interface-dfa <number> [broadcast]
 * no atm-dxi interface-dfa <number>
*/
EOLS	(ci_atm_dxi_dfa_eol, atm_dxi_command, ATM_DXI_DFA);

pdecl(ci_atm_dxi_dfa_opts);

KEYWORD_ID(ci_atmdxi_int_enc_def_nlpid, ci_atm_dxi_dfa_eol, 
	no_alt, OBJ(int,3), ATM_DXI_VC_NLPID, "nlpid",
	"NLPID Encapsulation", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_atmdxi_int_enc_def_mux, ci_atm_dxi_dfa_eol, 
	ci_atmdxi_int_enc_def_nlpid, OBJ(int,3), ATM_DXI_VC_MUX, "mux",
	"MUX Encapsulation", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_atmdxi_int_enc_def_snap, ci_atm_dxi_dfa_eol, 
	ci_atmdxi_int_enc_def_mux, OBJ(int,3), ATM_DXI_VC_SNAP, "snap",
	"LLC/SNAP Encapsulation", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_atmdxi_int_broadcast, ci_atmdxi_int_enc_def_snap,
	ci_atmdxi_int_enc_def_snap, OBJ(int,4), TRUE,
	"broadcast", "Broadcasts should be forwarded to this address",
	PRIV_CONF|PRIV_SUBIF);

NOP	(ci_atm_dxi_dfa_opts, ci_atmdxi_int_broadcast, ci_atm_dxi_dfa_eol);

SET     (ci_atm_dxi_intf_dfa_encaps_def, ci_atm_dxi_dfa_opts, OBJ(int,3), 
	ATM_DXI_VC_SNAP);

NUMBER  (ci_atm_dxi_intf_atmdxi_map_vci, ci_atm_dxi_intf_dfa_encaps_def, 
	no_alt, OBJ(int,2), 0, 63, "VCI");

NUMBER  (ci_atm_dxi_dfa_num, ci_atm_dxi_intf_atmdxi_map_vci, no_alt,
	OBJ(int,1), 0, 15, "VPI");

NVGENS	(ci_atm_dxi_dfa_nvgen, ci_atm_dxi_dfa_num,
	 atm_dxi_command, ATM_DXI_DFA);

KEYWORD	(ci_atm_dxi_int_dfa, ci_atm_dxi_dfa_nvgen, ci_atm_dxi_zap,
	"interface-dfa", "Define a DFA as part of a subinterface",
	PRIV_CONF | PRIV_SUBIF);

/***************************************************************
 * atm-dxi map <protocol> <protocoladdr> <vpi> <vci> 
 * no atm-dxi map <protocol> <protocoladdr> <vpi> <vci>
 *
 * OBJ(int,1) = <vpi>
 * OBJ(int,2) = <vci>
 * OBJ(int,3) = broadcast flag
 * OBJ(int,10) = <protocol>
 * (*OBJ(paddr,1)) = <protocoladdr>
 */

EOLS    (ci_atm_dxi_map_eol, atm_dxi_command, ATM_DXI_MAP);

KEYWORD_ID(ci_atm_dxi_atmdxi_broadcast, ci_atm_dxi_map_eol, 
	ci_atm_dxi_map_eol, OBJ(int,3), TRUE,
	"broadcast", "Broadcasts should be forwarded to this address",
	PRIV_CONF|PRIV_SUBIF);

NUMBER  (ci_atm_dxi_atmdxi_map_vci, ci_atm_dxi_atmdxi_broadcast, no_alt,
	OBJ(int,2), 0, 63, "VCI");

NUMBER  (ci_atm_dxi_atmdxi_map_vpi, ci_atm_dxi_atmdxi_map_vci, no_alt,
	OBJ(int,1), 0, 15, "VPI");

NOPREFIX (ci_atm_dxi_atmdxi_map_noprefix, ci_atm_dxi_atmdxi_map_vpi,
	ci_atm_dxi_map_eol);

NOPREFIX (ci_atm_dxi_atmdxi_clnsbridge_no, ci_atm_dxi_atmdxi_broadcast,
	ci_atm_dxi_map_eol);

NUMBER  (ci_atmdxi_vci, ci_atm_dxi_atmdxi_clnsbridge_no, no_alt,
	OBJ(int,2), 0, 63, "VCI");

NUMBER  (ci_atmdxi_vpi, ci_atmdxi_vci, no_alt,
	OBJ(int,1), 0, 15, "VPI");

IFELSE (ci_atm_dxi_atmdxi_clns_or_bridge, ci_atmdxi_vpi,
	ci_atm_dxi_atmdxi_map_noprefix, (((GETOBJ(int,10)) == LINK_CLNS) ||
			((GETOBJ(int,10)) == LINK_BRIDGE)));

PROTOADDR(ci_atm_dxi_atmdxi_map_protoaddr, ci_atm_dxi_atmdxi_clns_or_bridge,
	no_alt, OBJ(int,10), OBJ(paddr,1),
	PMATCH_DGRAM PMATCH_EXCLUDE PMATCH_AARP PMATCH_ARP);

NVGENS  (ci_atm_dxi_atmdxi_map_nvgen, ci_atm_dxi_atmdxi_map_protoaddr,
	atm_dxi_command, ATM_DXI_MAP);

KEYWORD (ci_atmdxi, ci_atm_dxi_atmdxi_map_nvgen, ci_atm_dxi_int_dfa,
	"map", "Map a given VPI and VCI to a ATM-DXI DFA", 
	PRIV_CONF|PRIV_SUBIF);

/***************************************************************
 * atm-dxi dxi-pvc <vpi> <vci> encaps-type
 * no atm-dxi dxi-pvc <vpi> <vci> encaps-type
 *
 * OBJ(int,1) = <vpi>
 * OBJ(int,2) = <vci>
 * OBJ(int,3) = encaps_type
 */

EOLS    (ci_atm_dxi_pvc_eol, atm_dxi_command, ATM_DXI_PVC);

KEYWORD_ID(ci_atm_dxi_pvc_nlpid, ci_atm_dxi_pvc_eol, no_alt,
	OBJ(int,3), ATM_DXI_VC_NLPID, "nlpid",
	"NLPID Encapsulation", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_atm_dxi_pvc_mux, ci_atm_dxi_pvc_eol, ci_atm_dxi_pvc_nlpid,
	OBJ(int,3), ATM_DXI_VC_MUX, "mux",
	"MUX Encapsulation", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_atm_dxi_pvc_snap, ci_atm_dxi_pvc_eol, ci_atm_dxi_pvc_mux,
	OBJ(int,3), ATM_DXI_VC_SNAP, "snap",
	"LLC/SNAP Encapsulation", PRIV_CONF|PRIV_SUBIF);

NOP	(ci_atm_dxi_pvc_opts, ci_atm_dxi_pvc_snap, ci_atm_dxi_pvc_eol);

SET     (ci_atm_dxi_pvc_default, ci_atm_dxi_pvc_opts, OBJ(int,3),
	ATM_DXI_VC_SNAP);

SET     (ci_atm_dxi_pvc_encaps_def, ci_atm_dxi_pvc_default, OBJ(int,4), 
	ATM_DXI_VC_SNAP);

NUMBER  (ci_atm_dxi_pvc_vci, ci_atm_dxi_pvc_encaps_def, 
	no_alt, OBJ(int,2), 0, 63, "VCI");

NUMBER  (ci_atm_dxi_pvc_num, ci_atm_dxi_pvc_vci, no_alt,
	OBJ(int,1), 0, 15, "VPI");

NVGENS	(ci_atm_dxi_pvc_nvgen, ci_atm_dxi_pvc_num,
	atm_dxi_command, ATM_DXI_PVC);

KEYWORD	(ci_atm_dxi_pvc_confg, ci_atm_dxi_pvc_nvgen, ci_atmdxi,
	"pvc", "define PVC attributes", PRIV_CONF | PRIV_SUBIF);

KEYWORD_ID(ci_atm_dxi, ci_atm_dxi_pvc_confg, NONE,
	   OBJ(int,5), FALSE,
	   "dxi", "ATM-DXI configuration commands",
	   PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_atm_dxi_old, ci_atm_dxi_pvc_confg, ci_atm_dxi,
	   OBJ(int,5), TRUE,
	   "atm-dxi", "", PRIV_ROOT|PRIV_HIDDEN);

TEST_IDBSTATUS(ci_atm_dxi_serial_check, ci_atm_dxi_old, NONE, ALTERNATE,
	       IDB_SERIAL);

#undef  ALTERNATE
#define ALTERNATE       ci_atm_dxi_serial_check
