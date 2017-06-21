/* $Id: cfg_int_multiring.h,v 3.2.60.1 1996/08/12 16:02:14 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_multiring.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ M U L T I R I N G . H
 *
 * Source-route bridging support
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_multiring.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:14  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:26:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:10  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/21  18:16:53  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:06:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 *	multiring { <protocol-keyword> | all | other } [all-routes | spanning]
 *	no multiring { <protocol-keyword> | all | other }
 *
 * OBJ(int,1) = keyword ID
 */

EOLNS	(multiring_eol, srb_multiring_command);

KEYWORD_ID (multiring_allroutes, multiring_eol, multiring_eol,
	    OBJ(int,2), EXPLORER_ALLROUTES,
	    "all-routes", "Use all routes explorers", PRIV_CONF);

KEYWORD_ID (multiring_spanning, multiring_eol, multiring_allroutes,
	    OBJ(int,2), EXPLORER_SPANNING,
	    "spanning", "Use spanning tree explorers", PRIV_CONF);

NVGENNS	   (multiring_nvgen, multiring_spanning, srb_multiring_command);

KEYWORD_ID (multiring_other, multiring_nvgen, no_alt,
	    OBJ(int,1), MULTIRING_OTHER,
	    "other", "Any protocol not listed in the above set", PRIV_CONF);

KEYWORD_ID (multiring_xns, multiring_nvgen, multiring_other,
	    OBJ(int,1), MULTIRING_XNS,
	    "xns", "", PRIV_CONF);

KEYWORD_ID (multiring_vines, multiring_nvgen, multiring_xns,
	    OBJ(int,1), MULTIRING_VINES,
	    "vines", "", PRIV_CONF);

KEYWORD_ID (multiring_ip, multiring_nvgen, multiring_vines,
	    OBJ(int,1), MULTIRING_IP,
	    "ip", "", PRIV_CONF);

NOVELL_IPX_KEYWORD_ID(multiring_novell, multiring_nvgen, multiring_ip,
		      OBJ(int,1), MULTIRING_NOVELL, "", PRIV_CONF);

KEYWORD_ID (multiring_decnet, multiring_nvgen, multiring_novell,
	    OBJ(int,1), MULTIRING_DECNET,
	    "decnet", "", PRIV_CONF);

KEYWORD_ID (multiring_clns, multiring_nvgen, multiring_decnet,
	    OBJ(int,1), MULTIRING_CLNS,
	    "clns", "", PRIV_CONF);

KEYWORD_ID (multiring_appletalk, multiring_nvgen, multiring_clns,
	    OBJ(int,1), MULTIRING_APPLE,
	    "appletalk", "", PRIV_CONF);

KEYWORD_ID (multiring_apollo, multiring_nvgen, multiring_appletalk,
	    OBJ(int,1), MULTIRING_APOLLO,
	    "apollo", "", PRIV_CONF);

KEYWORD_ID (multiring_all, multiring_nvgen, multiring_apollo,
	    OBJ(int,1), MULTIRING_ALL,
	    "all", "All protocol types from the below list", PRIV_CONF);

KEYWORD_ID(multiring, multiring_all, NONE,
	   OBJ(int,1), MULTIRING_ALL,
	   "multiring", "Enable RIF usage for a routable protocol", PRIV_CONF);

TEST_IDBSTATUS(multiring_test_fddi, multiring, NONE,
	       NONE, IDB_FDDI);

TEST_IDBSTATUS(multiring_test_tr, multiring, multiring_test_fddi, 
	       ALTERNATE, IDB_TR);

#undef ALTERNATE
#define ALTERNATE	multiring_test_tr
