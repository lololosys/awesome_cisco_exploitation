/* $Id: cfg_decnet.h,v 3.3.20.1 1996/08/12 16:01:03 widmer Exp $
 * $Source: /release/112/cvs/Xsys/decnet/cfg_decnet.h,v $
 *------------------------------------------------------------------
 * C F G _ D E C N E T . H
 *
 * Global configuration DECnet commands.
 *
 * OBJ(int,1) is the ATG network number (default 0)
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_decnet.h,v $
 * Revision 3.3.20.1  1996/08/12  16:01:03  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/01  23:33:06  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  08:58:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:23  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:02:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(conf_decnet_extend_here, no_alt);

/*****************************************************************
 * decnet [ address | routing ] <dnaddr>
 * no decnet routing [<dnaddr>]
 *
 *
 * OBJ(int,1) = <net>
 * OBJ(int,2) = <iv-prime>
 * (*OBJ(paddr,1)) = <dnaddr>
 */

EOLS	(conf_decnet_routing_eol, decnet_command, DC_ROUTING);
DNADDR	(conf_decnet_routing_5, conf_decnet_routing_eol, no_alt,
	 OBJ(paddr,1), "System-wide host address");
KEYWORD_ID(conf_decnet_routing_2, conf_decnet_routing_5, conf_decnet_routing_5,
	   OBJ(int,2), TRUE,
	   "iv-prime", "Phase-IV Prime", PRIV_CONF);
EQUAL	(conf_decnet_routing_1a, conf_decnet_routing_eol, NONE,
	 sense, FALSE);
EQUAL	(conf_decnet_routing_1, conf_decnet_routing_2,
	 conf_decnet_routing_1a, sense, TRUE);
NOPREFIX (conf_decnet_norouting, conf_decnet_routing_1,
	  conf_decnet_routing_eol);
NVGENS	(conf_decnet_routing_0, conf_decnet_norouting,
	 decnet_command, DC_ROUTING);
KEYWORD	(conf_decnet_address, conf_decnet_routing_0, conf_decnet_extend_here,
	"address", "", PRIV_CONF | PRIV_NONVGEN | PRIV_HIDDEN);
KEYWORD	(conf_decnet_routing, conf_decnet_routing_0, conf_decnet_address,
	 "routing", "Enable DECnet routing", PRIV_CONF);

/*****************************************************************
 *	decnet path-split-mode {normal | interim}
 *
 * OBJ(int,1) = <net>
 * OBJ(int,2) = DNM_NORMAL, DNM_INTERIM
 */

EOLS	(conf_decnet_psmode_6, decnet_command, DC_PATH_SPLIT_MODE);
KEYWORD_ID(conf_decnet_psmode_4, conf_decnet_psmode_6, no_alt,
	   OBJ(int,2), DNM_INTERIM,
	   "interim", "Keep each high-level session on same path", PRIV_CONF);
KEYWORD_ID(conf_decnet_psmode_2, conf_decnet_psmode_6, conf_decnet_psmode_4,
	   OBJ(int,2), DNM_NORMAL,
	   "normal", "Round-robin path splitting", PRIV_CONF);
NOPREFIX (conf_decnet_psmode_noprefix, conf_decnet_psmode_2,
	  conf_decnet_psmode_6);
NVGENS	(conf_decnet_psmode_1, conf_decnet_psmode_noprefix,
	 decnet_command, DC_PATH_SPLIT_MODE);
KEYWORD	(conf_decnet_psmode, conf_decnet_psmode_1, conf_decnet_routing,
	 "path-split-mode", "Specify splitting mode for equal cost paths",
	 PRIV_CONF);

/*****************************************************************
 *	decnet node-type {area | routing-iv}	
 *
 * OBJ(int,1) = <net>
 * OBJ(int,2) = DNN_AREA, DNN_ROUTING_IV
 */

EOLS	(conf_decnet_nodetype_6, decnet_command, DC_NODE_TYPE);
KEYWORD_ID(conf_decnet_nodetype_4, conf_decnet_nodetype_6, no_alt,
	   OBJ(int,2), DNN_ROUTING_IV,
	   "routing-iv", "Participate only in Phase IV routing (Level 1)",
	   PRIV_CONF);
KEYWORD_ID(conf_decnet_nodetype_2, conf_decnet_nodetype_6,
	   conf_decnet_nodetype_4,
	   OBJ(int,2), DNN_AREA,
	   "area", "Participate in area routing (Level 2)", PRIV_CONF);
NVGENS	(conf_decnet_nodetype_1, conf_decnet_nodetype_2,
	 decnet_command, DC_NODE_TYPE);
KEYWORD	(conf_decnet_nodetype, conf_decnet_nodetype_1, conf_decnet_psmode,
	 "node-type", "Specify router node type", PRIV_CONF);

/*****************************************************************
 * decnet max-visits <value>			
 * no decnet max-visits [<value>]
 *
 * OBJ(int,1) = <net>
 * OBJ(int,2) = <value>
 */

PARAMS_KEYONLY (conf_decnet_maxvisits, conf_decnet_nodetype,
	"max-visits", OBJ(int,2), 1, DN_MAX_VISITS,
	decnet_command, DC_MAX_VISITS,
	"Set maximum visits of a packet on this router",
	"Maximum visits of a packet on this router", PRIV_CONF);


/*****************************************************************
 * decnet max-paths <value>
 * no decnet max-paths [<value>]
 *
 * OBJ(int,1) = <net>
 * OBJ(int,2) = <value>
 */

PARAMS_KEYONLY (conf_decnet_maxpaths, conf_decnet_maxvisits,
	"max-paths", OBJ(int,2), 1, DN_MAX_SPLITS,
	decnet_command, DC_MAX_PATHS,
	"Set maximum number of equal cost paths to be kept",
	"Maximum number of equal cost paths to be kept", PRIV_CONF);

/*****************************************************************
 * decnet max-hops <value>
 * no decnet max-hops [<value>]
 *
 * OBJ(int,1) = <net>
 * OBJ(int,2) = <value>
 */

PARAMS_KEYONLY (conf_decnet_mh, conf_decnet_maxpaths,
	"max-hops", OBJ(int,2), 1, DN_MAX_HOPS,
	decnet_command, DC_MAX_HOPS,
	"Set maximum hops for intra-area routing",
	"Maximum hops for intra-area routing", PRIV_CONF);


/*****************************************************************
 * decnet max-cost <value>			
 * no decnet max-cost [<value>]
 *
 * OBJ(int,1) = <net>
 * OBJ(int,2) = <value>
 */

PARAMS_KEYONLY (conf_decnet_mc, conf_decnet_mh,
	"max-cost", OBJ(int,2), 1, DN_MAX_COST,
	decnet_command, DC_MAX_COST,
	"Set maximum cost for intra-area routing",
	"Maximum cost for intra-area routing", PRIV_CONF);

/*****************************************************************
 * decnet max-area <value>
 * no decnet max-area [<value>]
 *
 * OBJ(int,1) = <net>
 * OBJ(int,2) = <value>
 */

PARAMS_KEYONLY (conf_decnet_maxarea, conf_decnet_mc,
	"max-area", OBJ(int,2), 1, DN_MAX_AREA,
	decnet_command, DC_MAX_AREA,
	"Set highest area number allowed",
	"Highest area number allowed", PRIV_CONF);

/*****************************************************************
 * decnet max-address <value>
 * no decnet max-address [<value>]
 *
 * OBJ(int,1) = <net>
 * OBJ(int,2) = <value>
 */

PARAMS_KEYONLY (conf_decnet_maxaddr, conf_decnet_maxarea,
	"max-address", OBJ(int,2), 1, DN_MAX_NODE,
	decnet_command, DC_MAX_ADDRESS,
	"Set highest node number allowed in current area",
	"Highest node number allowed in current area", PRIV_CONF);

/*****************************************************************
 * decnet <first-network> map <virtual-dnaddr> <second-network>
 *			    <real-dnaddr> [<cost> <hop-count>]
 * no decnet <first-network> map [<virtual-dnaddr> <second-network>
 *			    <real-dnaddr>] [<cost> <hop-count>]
 *
 * In this command, <first-network> is an ATG number.  The number '0'
 * is allowed, so it seems reasonable that if no ATG number is
 * specified, that it default to '0' as in all the other commands.
 *
 * The 'no' version of the command can accept all the arguments to allow
 * the deletion of a single entry.  The <cost> and <hop-count> are
 * completely optional in this case.  It can also be entered with only the
 * <first-network> to delete the entire map for that network.
 *
 * OBJ(int,1) = <first-network>
 * OBJ(int,2) = <second-network>
 * OBJ(int,3) = <cost>
 * OBJ(int,4) = <hop-count>
 * OBJ(int,5) = TRUE if continued, FALSE otherwise.
 * (*OBJ(paddr,1)) = <virtual-dnaddr>
 * (*OBJ(paddr,2)) = <real-dnaddr>
 */

EOLS	(conf_decnet_map_eol, decnet_command, DC_MAP);
SET	(conf_decnet_map_cont_set, conf_decnet_map_eol, OBJ(int,5), TRUE);
CHARACTER (conf_decnet_map_cont1, conf_decnet_map_cont_set,
	   conf_decnet_map_eol,
	   '\\');
CHARACTER (conf_decnet_map_cont, conf_decnet_map_cont_set,
	   conf_decnet_map_cont1,
	   '-');
NUMBER	(conf_decnet_map_hops, conf_decnet_map_cont, no_alt,
	 OBJ(int,4), 1, DN_MAX_HOPS, "Hop count");
NUMBER	(conf_decnet_map_cost, conf_decnet_map_hops, conf_decnet_map_cont,
	 OBJ(int,3), 1, DN_MAX_COST, "Path cost");
DNADDR	(conf_decnet_map_3, conf_decnet_map_cost, no_alt,
	 OBJ(paddr,2), "Remote real DECnet address");
NUMBER	(conf_decnet_map_2, conf_decnet_map_3, no_alt,
	 OBJ(int,2), 0, 3, "Remote ATG network number");

/* Don't change the name of the below transition without also changing
 * it in decnet/decnet.c.  This is used by the map command for
 * processing continuation lines.
 */
DNADDR	(conf_decnet_map_fromaddr, conf_decnet_map_2, no_alt,
	 OBJ(paddr,1), "Local virtual DECnet address");


TESTVAR	(conf_decnet_map_noprefix, conf_decnet_map_eol, NONE,
	 NONE, NONE, conf_decnet_map_fromaddr,
	 sense, FALSE);
NVGENS	(conf_decnet_map_0, conf_decnet_map_noprefix,
	 decnet_command, DC_MAP);
KEYWORD	(conf_decnet_map, conf_decnet_map_0, conf_decnet_maxaddr,
	 "map", "Establish an ATG address mapping", PRIV_CONF);

/*****************************************************************
 * decnet area-max-hops <value>
 * no decnet area-max-hops [<value>]
 *
 * OBJ(int,1) = <net>
 * OBJ(int,2) = <value>
 */
PARAMS_KEYONLY (conf_decnet_amh, conf_decnet_map,
	"area-max-hops", OBJ(int,2), 1, DN_MAX_AREA_HOPS,
	decnet_command, DC_AREA_MAX_HOPS,
	"Set maximum hops for inter-area routing",
	"Maximum hops for inter-area routing", PRIV_CONF);

/*****************************************************************
 * decnet area-max-cost <value>		
 * no decnet area-max-cost [<value>]
 *
 * OBJ(int,1) = <net>
 * OBJ(int,2) = <value>
 */
PARAMS_KEYONLY (conf_decnet_amc, conf_decnet_amh,
	"area-max-cost", OBJ(int,2), 1, DN_MAX_AREA_COST,
	decnet_command, DC_AREA_MAX_COST,
	"Set maximum cost for inter-area routing",
	"Maximum cost for inter-area routing", PRIV_CONF);

/*****************************************************************
 *
 * Optional ATG network number.  The default is 0.
 */
NUMBER	(conf_decnet_atg, conf_decnet_amc, conf_decnet_amc,
	 OBJ(int,1), 0, 3, "ATG network number");
/* NVGEN from here to allow the numbers to be checked as valid ATG
 * numbers and that ATG is enabled.  See nv_decnet().
 */
NVGENNS	(conf_decnet_nvgen, conf_decnet_atg, decnet_command);

KEYWORD	(configure_decnet, conf_decnet_nvgen, ALTERNATE, "decnet",
	 "Global DECnet configuration subcommands", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE configure_decnet
