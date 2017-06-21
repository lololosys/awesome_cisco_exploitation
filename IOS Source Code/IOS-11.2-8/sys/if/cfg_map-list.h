/* $Id: cfg_map-list.h,v 3.2.60.4 1996/08/12 16:02:20 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_map-list.h,v $
 *------------------------------------------------------------------
 * map-list command
 *
 * October 1993, Lionel Bustini
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_map-list.h,v $
 * Revision 3.2.60.4  1996/08/12  16:02:20  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.3  1996/05/21  17:47:15  skoh
 * CSCdi57812:  Change local-addr to source-addr in map-list for FR SVCs
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/05/21  17:38:30  skoh
 * CSCdi57654:  remove or hide the priority-dlci-group option in maplists
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/25  23:13:11  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.2.64.1  1996/04/24  04:06:41  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.2  1995/11/17  09:26:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:12  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:11:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] map-list <name> 
 * 
 */

EOLNS	(cfg_maplist_eol, maplist_command);

NUMBER  (cmap_fr_vc_priority_num, cfg_maplist_eol, no_alt,
	OBJ(int,3), 1, 4, "Priority Value");

KEYWORD(cmap_fr_vc_priority, cmap_fr_vc_priority_num, cfg_maplist_eol,
	"priority-dlci-group", "Priority : 1-high, 2-medium, 3-normal, 4-low",
	PRIV_CONF | PRIV_HIDDEN);

NUMBER  (cmap_fr_vc_cd_subaddr_num, cmap_fr_vc_priority, no_alt,
	OBJ(int,5), 1, 9, "Called Party Subaddress Number");

KEYWORD(cmap_fr_vc_cd_subaddr, cmap_fr_vc_cd_subaddr_num, cfg_maplist_eol,
	"cdps", "Called Party Subaddress", PRIV_CONF);

NUMBER  (cmap_fr_vc_cp_subaddr_num, cmap_fr_vc_cd_subaddr, no_alt,
	OBJ(int,4), 1, 9, "Calling Party Subaddress Number");

KEYWORD(cmap_fr_vc_cp_subaddr, cmap_fr_vc_cp_subaddr_num, cfg_maplist_eol,
	"clps", "Calling Party Subaddress", PRIV_CONF);

X121ADDR(cfg_fr_x121_dest_addr, cmap_fr_vc_cp_subaddr, no_alt,
	OBJ(paddr,2), "Destination X121 address");

KEYWORD_ID(cfg_fr_dest_x121, cfg_fr_x121_dest_addr, ALTERNATE,
	OBJ(int,2), FR_X121_ADDR, "X121", "Destination X121 address", PRIV_CONF);

E164ADDR(cfg_fr_e164_dest_addr, cmap_fr_vc_priority, no_alt,
	OBJ(paddr,2), "Destination E.164 address");

KEYWORD_ID(cfg_fr_dest_e164, cfg_fr_e164_dest_addr, cfg_fr_dest_x121,
	OBJ(int,2), FR_E164_ADDR, "E164", "Destination E164 Address", PRIV_CONF);

KEYWORD (cfg_fr_dest_addr, cfg_fr_dest_e164, no_alt,
	"dest-addr", "Destination Address in X121/E164 format", PRIV_CONF);

X121ADDR(cfg_fr_x121_addr, cfg_fr_dest_addr, no_alt, OBJ(paddr,1), "X121 address");

KEYWORD_ID(cfg_fr_x121, cfg_fr_x121_addr, ALTERNATE,
	OBJ(int,1), FR_X121_ADDR, "X121", "X121 address", PRIV_CONF);

E164ADDR(cfg_fr_e164_addr, cfg_fr_dest_addr, no_alt, OBJ(paddr,1), "E.164 address");

KEYWORD_ID(cfg_fr_e164, cfg_fr_e164_addr, cfg_fr_x121,
	OBJ(int,1), FR_E164_ADDR, "E164", "E164 Address", PRIV_CONF);

KEYWORD (cfg_fr_local_addr, cfg_fr_e164, cfg_maplist_eol,
	"source-addr", "Address of Interface in X121/E164 format", PRIV_CONF);

NOPREFIX(cfg_maplist_noprefix, cfg_fr_local_addr, cfg_maplist_eol);

STRING	(cfg_maplist_name, cfg_maplist_noprefix, no_alt,
	 OBJ(string, 1), "Static map list name");

NVGENNS	(cfg_maplist_nvgens, cfg_maplist_name, maplist_command);

KEYWORD_ID(cfg_maplist, cfg_maplist_nvgens, ALTERNATE,
           leave_subcommand, TRUE,
           "map-list", "Configure static map list", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE       cfg_maplist
