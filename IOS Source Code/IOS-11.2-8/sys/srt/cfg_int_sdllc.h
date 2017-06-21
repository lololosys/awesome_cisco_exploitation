/* $Id: cfg_int_sdllc.h,v 3.2.62.1 1996/05/17 12:12:33 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/cfg_int_sdllc.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ S D L L C . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_sdllc.h,v $
 * Revision 3.2.62.1  1996/05/17  12:12:33  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/04/11  14:38:13  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2  1995/11/17  18:53:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:57:57  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:07:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * sdllc ring-largest-frame
 * sdllc sdlc-largest-frame
 * sdllc traddr
 * sdllc xid
 *
 */

/***************************************************************
 * sdllc partner
 *
 */
EOLS	(cfg_sdllc_prtnr_eol, sdllcif_command, SDLLC_PRTNR);
pdecl( cfg_sdllc_opts);

KEYWORD_ID(cfg_sdllc_outbound, cfg_sdllc_opts, no_alt,
	 OBJ(int,3), TRUE,
	 "outbound", "initiate connection to partner", PRIV_CONF);

KEYWORD_ID(cfg_sdllc_inbound, cfg_sdllc_opts, cfg_sdllc_outbound,
	 OBJ(int,2), TRUE,
	 "inbound", "partner will initiate connection", PRIV_CONF);

NOP	(cfg_sdllc_opts, cfg_sdllc_inbound, cfg_sdllc_prtnr_eol);

HEXDIGIT(cfg_sdllc_prtnr_sdlcaddr, cfg_sdllc_opts, no_alt,
	 OBJ(int,1), 1, 0xFE, "SDLC address in Hex");
ENADDR	(cfg_sdllc_prtnr_enaddr, cfg_sdllc_prtnr_sdlcaddr, no_alt,
	 OBJ(hwaddr,1), "Partners Token Ring MAC address");
NVGENS	(cfg_sdllc_prtnr_nvgen, cfg_sdllc_prtnr_enaddr,
	 sdllcif_command, SDLLC_PRTNR);
KEYWORD	(cfg_sdllc_prtnr, cfg_sdllc_prtnr_nvgen, no_alt,
	 "partner", "MAC address for partner", PRIV_CONF);

/***************************************************************
 * sdllc saps <srcaddr> <dstaddr>
 *
 */
EOLS	(cfg_sdllc_saps_eol, sdllcif_command, SDLLC_SAPS);
HEXDIGIT(cfg_sdllc_saps_dstsap, cfg_sdllc_saps_eol, no_alt,
	 OBJ(int,3), 1, 0xFF, "Destination SAP");
HEXDIGIT(cfg_sdllc_saps_srcsap, cfg_sdllc_saps_dstsap, no_alt,
	 OBJ(int,2), 1, 0xFF, "Source SAP");
HEXDIGIT(cfg_sdllc_saps_addr, cfg_sdllc_saps_srcsap, no_alt,
	 OBJ(int,1), 1, 0xFE, "SDLC address in Hex");
NVGENS	(cfg_sdllc_saps_nvgen, cfg_sdllc_saps_addr,
	 sdllcif_command, SDLLC_SAPS);
KEYWORD	(cfg_sdllc_saps, cfg_sdllc_saps_nvgen, cfg_sdllc_prtnr,
	 "saps", "Configure lsap and dsap for token ring", PRIV_CONF);

/***************************************************************
 * sdllc xid <sdlcaddr> <xid>
 * no sdllc xid <sdlcaddr>
 */
EOLS	(cfg_sdllc_xid_eol, sdllcif_command, SDLLC_XID);
STRING	(cfg_sdllc_xid_string, cfg_sdllc_xid_eol, no_alt,
	 OBJ(string,1), "XID (must be 8 digits)");
NOPREFIX(cfg_sdllc_xid_noprefix, cfg_sdllc_xid_string, cfg_sdllc_xid_eol);
HEXDIGIT(cfg_sdllc_xid_addr, cfg_sdllc_xid_noprefix, no_alt,
	 OBJ(int,1), 1, 0xFE, "SDLC address in Hex");
NVGENS	(cfg_sdllc_xid_nvgen, cfg_sdllc_xid_addr, sdllcif_command, SDLLC_XID);
KEYWORD	(cfg_sdllc_xid, cfg_sdllc_xid_nvgen, cfg_sdllc_saps,
	 "xid",
	 "Specify an XID value to be associated with the"
	 " SDLC station at a specified address",
	 PRIV_CONF);

/***************************************************************
 * sdllc sdlc-largest-frame <sdlcaddr> <number>
 * no sdllc sdlc-largest-frame <sdlcaddr> [<number>]
 */
EOLS	(cfg_sdllc_sdlc_eol, sdllcif_command, SDLLC_SDLCLF);
NUMBER	(cfg_sdllc_sdlc_num, cfg_sdllc_sdlc_eol, no_alt,
	 OBJ(int,2), 0, -1, "Largest I-frame size");

NOPREFIX(cfg_sdllc_sdlc_noprefix, cfg_sdllc_sdlc_num, cfg_sdllc_sdlc_eol);

HEXDIGIT(cfg_sdllc_sdlc_ether, cfg_sdllc_sdlc_noprefix, no_alt,
	 OBJ(int,1), 1, 0xFE, "SDLC station address");

NVGENS	(cfg_sdllc_sdlc_nvgen, cfg_sdllc_sdlc_ether,
	 sdllcif_command, SDLLC_SDLCLF);

KEYWORD	(cfg_sdllc_sdlc, cfg_sdllc_sdlc_nvgen, cfg_sdllc_xid,
	 "sdlc-largest-frame",
	 "Set the largest I-frame size that can be sent or"
	 " received by a SDLC station at a specified address",
	 PRIV_CONF);

/***************************************************************
 * sdllc ring-largest-frame { 516 | 1500 | 2052  | 
 *			     4472 | 8144 | 11407 | 17800 }
 * no sdllc ring-largset-frame
 */
EOLS	(cfg_sdllc_ring_eol, sdllcif_command, SDLLC_TRLF);

KEYWORD_ID(cfg_sdllc_ring_num7, cfg_sdllc_ring_eol, no_alt,
	   OBJ(int,1), RC_LF17800_VAL,
	   "17800", "17800 bytes", PRIV_CONF);
KEYWORD_ID(cfg_sdllc_ring_num6, cfg_sdllc_ring_eol, cfg_sdllc_ring_num7,
	   OBJ(int,1), RC_LF11407_VAL,
	   "11407", "11407 bytes", PRIV_CONF);
KEYWORD_ID(cfg_sdllc_ring_num5, cfg_sdllc_ring_eol, cfg_sdllc_ring_num6,
	   OBJ(int,1), RC_LF8144_VAL,
	   "8144", "8144 bytes", PRIV_CONF);
KEYWORD_ID(cfg_sdllc_ring_num4, cfg_sdllc_ring_eol, cfg_sdllc_ring_num5,
	   OBJ(int,1), RC_LF4472_VAL,
	   "4472", "4472 bytes", PRIV_CONF);
KEYWORD_ID(cfg_sdllc_ring_num3, cfg_sdllc_ring_eol, cfg_sdllc_ring_num4,
	   OBJ(int,1), RC_LF2052_VAL,
	   "2052", "2052 bytes", PRIV_CONF);
KEYWORD_ID(cfg_sdllc_ring_num2, cfg_sdllc_ring_eol, cfg_sdllc_ring_num3,
	   OBJ(int,1), RC_LF1500_VAL,
	   "1500", "1500 bytes", PRIV_CONF);
KEYWORD_ID(cfg_sdllc_ring_num1, cfg_sdllc_ring_eol, cfg_sdllc_ring_num2,
	   OBJ(int,1), RC_LF516_VAL,
	   "516", "516 bytes", PRIV_CONF);

NOPREFIX(cfg_sdllc_ring_noprefix, cfg_sdllc_ring_num1, cfg_sdllc_ring_eol);

NVGENS	(cfg_sdllc_ring_nvgen, cfg_sdllc_ring_noprefix,
	 sdllcif_command, SDLLC_TRLF);

KEYWORD	(cfg_sdllc_ring, cfg_sdllc_ring_nvgen, cfg_sdllc_sdlc,
	 "ring-largest-frame",
	 "Set the largest I-frame size that can be sent"
	 " or received from the LLC2 primary station",
	 PRIV_CONF);

/***************************************************************
 * sdllc traddr <enaddr> <ring> <bridge> <target>
 * no sdllc traddr [<enaddr> <ring> <bridge> <target>]
 */
EOLS	(cfg_sdllc_traddr_eol, sdllcif_command, SDLLC_TRADDR);
NUMBER	(cfg_sdllc_traddr_target, cfg_sdllc_traddr_eol, no_alt,
	 OBJ(int,3), 0, SRB_MAX_RING_NUM,
	 "SDLLC target ring number");
NUMBER	(cfg_sdllc_traddr_bridge, cfg_sdllc_traddr_target, no_alt,
	 OBJ(int,2), 0, SRB_MAX_BRIDGE_NUM,
	 "SDLLC bridge number");
NUMBER	(cfg_sdllc_traddr_ring, cfg_sdllc_traddr_bridge, no_alt,
	 OBJ(int,1), 0, SRB_MAX_RING_NUM,
	 "SDLLC virtual ring number");
ENADDR	(cfg_sdllc_traddr_addr, cfg_sdllc_traddr_ring, no_alt,
	 OBJ(hwaddr,1), "Virtual Token Ring MAC address (must end with 00)");
NOPREFIX(cfg_sdllc_traddr_noprefix, cfg_sdllc_traddr_addr,
	 cfg_sdllc_traddr_eol);
NVGENS	(cfg_sdllc_traddr_nvgen, cfg_sdllc_traddr_noprefix,
	 sdllcif_command, SDLLC_TRADDR);
KEYWORD	(cfg_sdllc_traddr, cfg_sdllc_traddr_nvgen, cfg_sdllc_ring,
	 "traddr",
	 "Enable SDLLC translation and assign MAC address to interface",
	 PRIV_CONF);

/***************************************************************/
KEYWORD	(cfg_sdllc, cfg_sdllc_traddr, NONE, "sdllc",
	 "Configure SDLC to LLC2 translation", PRIV_CONF);

ASSERT	(cfg_sdllc_test, cfg_sdllc, ALTERNATE,
	 ((csb->interface->hwptr->status & IDB_SERIAL) &&
	  !ISCLSSDLC(csb->interface->hwptr)));

#undef	ALTERNATE
#define	ALTERNATE	cfg_sdllc_test

