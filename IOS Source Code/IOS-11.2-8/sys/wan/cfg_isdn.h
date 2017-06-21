/* $Id: cfg_isdn.h,v 3.5 1996/02/15 18:03:03 gtaylor Exp $
 * $Source: /release/111/cvs/Xsys/wan/cfg_isdn.h,v $
 *------------------------------------------------------------------
 * C F G _ I S D N . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_isdn.h,v $
 * Revision 3.5  1996/02/15  18:03:03  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.4  1996/01/26  01:31:04  hrobison
 * CSCdi43998:  BRI COMMANDS ARE NOT RECOGNIZED IN UNITS WITH MBRI &
 * CE1/CT1
 * Move increment of nprinets so that this only happens if
 * PRI service is configured.  Previously nprinets was
 * incremented if a 4XXX CT1/PRI or CE1/PRI, or 7XXX MIP
 * was installed even if no pri-group was configured.
 * This prevented channelized service from working in
 * the same router with BRI service.  Parser checks for
 * nprinets have been changed to check for either
 * nbrinets or (nt1ports, ne1ports, nst1ports or
 * nse1ports).
 *
 * Revision 3.3  1995/12/07  17:45:38  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  18:01:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:00  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:08:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * isdn leased-line {bri#}
 * no leased-line {bri#}
 */
EOLS	(cfg_isdn_leased_eol, isdn_command, PARSER_ISDN_LEASED_LINE);
INTERFACE (cfg_isdn_leased_if, cfg_isdn_leased_eol, no_alt, OBJ(idb,1),
			IFTYPE_BRI);
KEYWORD	(cfg_isdn_leased, cfg_isdn_leased_if, no_alt, "leased-line",
		 "Sets a BRI interface to support leased lines on B & D channels",
		 PRIV_CONF);

/***************************************************************
 * isdn tei-negotiation {first-call|powerup}
 * no isdn tei-negotiation
 */
EOLS	(cfg_isdn_tei_eol, isdn_command, PARSER_ISDN_TEI);
KEYWORD_ID(cfg_isdn_tei_powerup, cfg_isdn_tei_eol, no_alt,
	   OBJ(int,1), ISDN_TEI_POWERUP,
	   "powerup", "ISDN TEI negotiation should occur when the "
	   "router is powered on",
	   PRIV_CONF);
KEYWORD_ID(cfg_isdn_tei_firstcall, cfg_isdn_tei_eol, cfg_isdn_tei_powerup,
	   OBJ(int,1), ISDN_TEI_FIRST_CALL,
	   "first-call", "ISDN TEI negotiation should occur when the "
	   "first ISDN call is placed or received",
	   PRIV_CONF);
NOPREFIX(cfg_isdn_tei_noprefix, cfg_isdn_tei_firstcall, cfg_isdn_tei_eol);
KEYWORD	(cfg_isdn_tei, cfg_isdn_tei_noprefix, cfg_isdn_leased,
	 "tei-negotiation", "Set when ISDN TEI negotiation should occur",
	 PRIV_CONF);
/* This check should cover both tei and leased line not appearing for PRI */
TEST_INT(cfg_tei_test, cfg_isdn_tei, NONE, no_alt, nbrinets);

/***************************************************************
 * isdn switch-type {none|basic-net3-bt|primary-4ess|primary-5ess|
 *		     basic-5ess|primary-dms100|basic-dms100|vn2|kdd|
 *		     ntt|primary-1tr6|basic-1tr6|basic-ts013|
 *		     basic-nznet3|basic-nwnet3|ccitt}
 *
 *
 */
EOLS	(cis_eol, isdn_command, PARSER_ISDN_SWITCH);
KEYWORD_ID(cis_ccitt, cis_eol, no_alt,
	   OBJ(int,1), CCITT_STYPE,
	   "ccitt", "", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(cis_basicnwnet3, cis_eol, cis_ccitt,
	   OBJ(int,1), BRI_NWNET3_STYPE,
	   "basic-nwnet3", "NET3 switch type for Norway", PRIV_CONF);
KEYWORD_ID(cis_basicnznet3, cis_eol, cis_basicnwnet3,
	   OBJ(int,1), BRI_NZNET3_STYPE,
	   "basic-nznet3", "NET3 switch type for New Zealand", PRIV_CONF);
KEYWORD_ID(cis_basicts013, cis_eol, cis_basicnznet3,
	   OBJ(int,1), BRI_TS013_STYPE,
	   "basic-ts013", "TS013 switch type for Australia", PRIV_CONF);
KEYWORD_ID(cis_basic1tr6, cis_eol, cis_basicts013,
	   OBJ(int,1), BRI_1TR6_STYPE,
	   "basic-1tr6", "1TR6 switch type for Germany", PRIV_CONF);
KEYWORD_ID(cis_primary1tr6, cis_eol, cis_basic1tr6,
	   OBJ(int,1), PRI_1TR6_STYPE,
	   "primary-1tr6", "", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(cis_ntt, cis_eol, cis_primary1tr6,
	   OBJ(int,1), NTT_STYPE,
	   "ntt", "NTT switch type for Japan", PRIV_CONF);
KEYWORD_ID(cis_kdd, cis_eol, cis_ntt,
	   OBJ(int,1), KDD_STYPE,
	   "kdd", "", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(cis_vn3, cis_eol, cis_kdd,
	   OBJ(int,1), VN3_STYPE,
	   "vn3", "VN3 and VN4 switch types for France", PRIV_CONF);
KEYWORD_ID(cis_vn2, cis_eol, cis_vn3,
	   OBJ(int,1), VN2_STYPE,
	   "vn2", "VN2 switch type for France", PRIV_CONF);
KEYWORD_ID(cis_basicni1, cis_eol, cis_vn2,
	   OBJ(int,1), BRI_NI1_STYPE,
	   "basic-ni1", "National ISDN-1 switch type", PRIV_CONF);
KEYWORD_ID(cis_basicdms100, cis_eol, cis_basicni1,
	   OBJ(int,1), BRI_DMS100_STYPE,
	   "basic-dms100", "Northern DMS-100 switch type", PRIV_CONF);
KEYWORD_ID(cis_basic5ess, cis_eol, cis_basicdms100,
	   OBJ(int,1), BRI_5ESS_STYPE,
	   "basic-5ess", "AT&T 5ESS switch type for the U.S.", PRIV_CONF);
KEYWORD_ID(cis_primarydms100, cis_eol, no_alt,
	   OBJ(int,1), PRI_DMS100_STYPE,
	   "primary-dms100", "Northern Telecom switch type for the U.S.", 
	   PRIV_CONF);
KEYWORD_ID(cis_primarynet5, cis_eol, cis_primarydms100,
	   OBJ(int,1), PRI_NET5_STYPE,
	   "primary-net5", "European switch type for NET5", 
	   PRIV_CONF);
KEYWORD_ID(cis_primaryntt, cis_eol, cis_primarynet5,
	   OBJ(int,1), PRI_NTT_STYPE,
	   "primary-ntt", "Japan switch type", PRIV_CONF);
KEYWORD_ID(cis_primaryts014, cis_eol, cis_primaryntt,
	   OBJ(int,1), PRI_TS014_STYPE,
	   "primary-ts014", "Australia switch type", PRIV_CONF);
KEYWORD_ID(cis_primary5ess, cis_eol, cis_primaryts014,
	   OBJ(int,1), PRI_5ESS_STYPE,
	   "primary-5ess", "AT&T 5ESS switch type for the U.S.", PRIV_CONF);
KEYWORD_ID(cis_primary4ess, cis_eol, cis_primary5ess,
	   OBJ(int,1), PRI_4ESS_STYPE,
	   "primary-4ess", "AT&T 4ESS switch type for the U.S.", PRIV_CONF);
KEYWORD_ID(cis_basicnet3, cis_eol,  cis_basic5ess,
	   OBJ(int,1), BRI_NET3_STYPE,
	   "basic-net3", "NET3 switch type for UK and Europe", PRIV_CONF);
TEST_INT(cis_pri_test, cis_basicnet3, cis_primary4ess, NONE, nbrinets);
KEYWORD_ID(cis_none, cis_eol, cis_pri_test,
	   OBJ(int,1), NO_ISDN_SWITCH,
	   "none", "", PRIV_CONF | PRIV_HIDDEN);
NOPREFIX(cis_noprefix, cis_none, cis_eol);
NVGENS	(cis_nvgen, cis_noprefix, 
	 isdn_command, PARSER_ISDN_SWITCH);
KEYWORD	(cfg_isdn_switch, cis_nvgen, cfg_tei_test,
	 "switch-type", "Select the ISDN switch type", PRIV_CONF);


KEYWORD	(cfg_isdn, cfg_isdn_switch, NONE,
	 "isdn", "ISDN configuration commands", PRIV_CONF);

ASSERT(cfg_isdn_test, cfg_isdn, ALTERNATE, (nbrinets || nt1ports || ne1ports || nst1ports || nse1ports));

#undef	ALTERNATE
#define	ALTERNATE	cfg_isdn_test
