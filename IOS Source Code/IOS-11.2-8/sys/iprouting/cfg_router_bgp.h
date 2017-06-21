/* $Id: cfg_router_bgp.h,v 3.5.42.4 1996/08/12 16:03:41 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_bgp.h,v $
 *------------------------------------------------------------------
 * cfg_router_bgp.h
 *
 * November 1993, Paul Traina
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_router_bgp.h,v $
 * Revision 3.5.42.4  1996/08/12  16:03:41  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.5.42.3  1996/05/07  18:05:04  snyder
 * CSCdi56844:  spellink errors
 * Branch: California_branch
 *
 * neigbor -> neighbor
 * neighor -> neighbor
 *
 * Revision 3.5.42.2  1996/04/16  19:01:23  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.5.42.1  1996/04/05  03:46:31  rchandra
 * CSCdi53755:  BGP: flap dampening settings not flexible enough
 * Branch: California_branch
 * - add route-map support to set flap dampening parameters
 *
 * Revision 3.5  1995/12/22  21:52:28  rchandra
 * CSCdi46047:  BGP: memory corruption when bgp confederation peers are
 * removed
 * - set bgp_confed_peers to null to avoid memory corruption
 * - order confederation peers
 * - add support to remove individual confederation peers
 * - split confederation peers command to multiple lines (25 per line)
 *
 * Revision 3.4  1995/11/28  19:50:40  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.3  1995/11/21  21:24:31  snyder
 * CSCdi44272:  more spellink errors
 *              reuseing -> reusing
 *
 * Revision 3.2  1995/11/17  17:32:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/25  08:30:51  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.2  1995/07/12  17:52:18  rchandra
 * CSCdi36969:  BGP : Add knob to disable client to client reflection
 * - 'bgp client-to-client reflection' command added
 * - when access-list change callback is made, instead of cleaning the
 *   cache immediately, flag the cache for purging by bgp_router.
 *
 * Revision 2.1  1995/07/01  12:15:49  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.2  1995/06/30  20:24:42  rchandra
 * CSCdi36620:  BGP requires IBGP neighbors to be fully meshed
 *
 * Revision 2.1  1995/06/07  22:12:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * bgp deterministic-med
 */
EOLS (router_bgp_determ_med_eol, bgp_command, BGP_DETERM_MED);
KEYWORD (router_bgp_determ_med, router_bgp_determ_med_eol,
	 no_alt, "deterministic-med",
	 "Pick the best-MED path among paths advertised from the"
	 " neighboring AS", PRIV_CONF);

/**********************************************************************
 * bgp dampening <half-life-time> [<reuse> <suppress> <maxtime>]
 * no bgp dampening
 *
 * OBJ(int, 1) = half-life time
 * OBJ(int, 2) = reuse limit
 * OBJ(int, 3) = suppress limit
 * OBJ(int, 4) = maximumum suppress time
 *
 */

EOLNS	(router_bgp_damp_eols, bgp_dampening_command);

EOLS	(router_bgp_damp_rmap_str_eols, bgp_dampening_command, 
	 BGP_RMAP_DAMPENING);
STRING	(router_bgp_damp_rmap_str, router_bgp_damp_rmap_str_eols, no_alt,
	 OBJ(string, 1), "route-map name");
KEYWORD	(router_bgp_damp_rmap, router_bgp_damp_rmap_str, router_bgp_damp_eols,
	 "route-map", "Route-map to specify criteria for dampening",PRIV_CONF);

NUMBER	(router_bgp_damp_hold, router_bgp_damp_eols, no_alt,
	 OBJ(int, 4), 1, 0xFF, "Maximum duration to suppress a stable route");
NUMBER	(router_bgp_damp_sup, router_bgp_damp_hold, no_alt,
	 OBJ(int, 3), 1, BGP_MAX_PENALTY,"Value to start suppressing a route");
NUMBER	(router_bgp_damp_reuse, router_bgp_damp_sup, router_bgp_damp_eols,
	 OBJ(int, 2), 1, BGP_MAX_PENALTY, "Value to start reusing a route");

NUMBER	(router_bgp_damp_halflife, router_bgp_damp_reuse, router_bgp_damp_rmap,
	 OBJ(int, 1), 1, BGP_MAX_HALFLIFE, "Half-life time for the penalty");

NOPREFIX(router_bgp_damp_noprefix, router_bgp_damp_halflife,
	 router_bgp_damp_eols);
NVGENNS	(router_bgp_damp_nv, router_bgp_damp_noprefix, bgp_dampening_command);
KEYWORD	(router_bgp_damp, router_bgp_damp_nv, router_bgp_determ_med,
	 "dampening", "Enable route-flap dampening", PRIV_CONF);

/**********************************************************************
 * bgp common-administration <AS-numbers>		!!(OBSOLETE)!!
 * no bgp common-administration
 *
 * OBJ(int,1) = value
 *
 */
pdecl	(router_bgp_com_admin_asnum);
EOLNS 	(router_bgp_com_admin_eols, bgp_confed_peers_command);
FUNC	(router_bgp_com_admin_func,
	 router_bgp_com_admin_asnum,
	 bgp_confed_peers_command);
NUMBER	(router_bgp_com_admin_asnum, router_bgp_com_admin_func,
	 router_bgp_com_admin_eols,
	 OBJ(int, 1), 1, MAXUSHORT, NULL);
NOPREFIX(router_bgp_com_admin_noprefix, router_bgp_com_admin_asnum,
	 router_bgp_com_admin_eols);
KEYWORD	(router_bgp_com_admin, router_bgp_com_admin_noprefix, router_bgp_damp,
	 "common-administration", NULL, PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN);

/**********************************************************************
 * bgp confederation peers <AS-numbers>
 * no bgp confederation peers
 *
 * OBJ(int,1) = value
 *
 */
pdecl	(router_bgp_confed_peers_asnum);
EOLNS 	(router_bgp_confed_peers_eols, bgp_confed_peers_command);
FUNC	(router_bgp_confed_peers_func,
	 router_bgp_confed_peers_asnum,
	 bgp_confed_peers_command);
NUMBER	(router_bgp_confed_peers_asnum, router_bgp_confed_peers_func,
	 router_bgp_confed_peers_eols,
	 OBJ(int, 1), 1, MAXUSHORT, "AS number");
NVGENNS	(router_bgp_confed_peers_nv, router_bgp_confed_peers_asnum,
	 bgp_confed_peers_command);
KEYWORD	(router_bgp_confed_peers, router_bgp_confed_peers_nv, no_alt,
	 "peers", "Peer ASs in BGP confederation", PRIV_CONF);

/**********************************************************************
 * bgp confederation identifier <AS-number>
 * bgp confederation identifier
 *
 * OBJ(int,1) = value
 *
 */
PARAMS_KEYONLY (router_bgp_confed_id, router_bgp_confed_peers,
		"identifier", OBJ(int,1), 1, MAXUSHORT,
		bgp_command, BGP_CONFED_ID,
		"as number",
		"Set routing domain confederation AS", PRIV_CONF);

KEYWORD	       (router_bgp_confed, router_bgp_confed_id, router_bgp_com_admin,
		"confederation", "AS confederation parameters", PRIV_CONF);

/**********************************************************************
 * [no] bgp client-to-client reflection
 *
 *
 */

EOLS	(router_bgp_client_eol, bgp_command, BGP_CLIENT_REFLECTION);
KEYWORD (router_bgp_client_refl, router_bgp_client_eol, no_alt,
	 "reflection", "reflection of routes allowed", PRIV_CONF);
KEYWORD (router_bgp_client, router_bgp_client_refl, router_bgp_confed,
	 "client-to-client", "Configure client to client route reflection",
	 PRIV_CONF);


/**********************************************************************
 * bgp cluster-id <cluster-id>
 *
 * OBJ(int,1) = cluster-id
 *
 */
EOLS	(router_bgp_cluster_eol, bgp_command, BGP_CLUSTER_ID);
NUMBER	(router_bgp_cluster_id, router_bgp_cluster_eol, no_alt,
	 OBJ(int, 1), 1, MAXULONG, "Route-Reflector Cluster-id");
NOPREFIX(router_bgp_cluster_noprefix, router_bgp_cluster_id,
	 router_bgp_cluster_eol);
NVGENS	(router_bgp_cluster_nv, router_bgp_cluster_noprefix, bgp_command,
	 BGP_CLUSTER_ID);
KEYWORD	(router_bgp_cluster, router_bgp_cluster_nv, router_bgp_client,
	 "cluster-id", "Configure Route-Reflector Cluster-id", PRIV_CONF);

/**********************************************************************
 * bgp default local-preference <value>
 * no bgp default local-preference
 *
 * OBJ(int,1) = value
 *
 */
PARAMS_KEYONLY	(router_bgp_default_localpref, no_alt,
		 "local-preference", OBJ(int,1), 0, MAXULONG,
		 bgp_command, BGP_DEF_LOCALPREF,
		 "local preference (higher=more preferred)",
		 "Configure default local preference value", PRIV_CONF);

KEYWORD (router_bgp_default, router_bgp_default_localpref, router_bgp_cluster,
	 "default", "Configure BGP defaults", PRIV_CONF);

/**********************************************************************
 * bgp always-compare-med
 */
EOLS (router_bgp_always_compare_med_eol, bgp_command, BGP_ALWAYS_COMPARE_MED);
KEYWORD (router_bgp_always_compare_med, router_bgp_always_compare_med_eol,
	 router_bgp_default, "always-compare-med",
	 "Allow comparing MED from different neighbors", PRIV_CONF);

/**********************************************************************
 * bgp process-dpa
 */
EOLS (router_bgp_dpa_eol, bgp_command, BGP_DPA_ENABLED);
KEYWORD (router_bgp_dpa, router_bgp_dpa_eol, router_bgp_always_compare_med, 
	 "process-dpa", "Process Destination Preference Attribute "
	 , PRIV_CONF|PRIV_HIDDEN);

/**********************************************************************
 * bgp fast-external-fallover
 */
EOLS (router_bgp_fastreset_eol, bgp_command, BGP_FAST_RESET);
KEYWORD (router_bgp_fastreset, router_bgp_fastreset_eol,
	 router_bgp_dpa, "fast-external-fallover",
	 "Immediately reset session if a link to a directly connected "
	 "external peer goes down", PRIV_CONF);

/**********************************************************************
 * bgp router-id x.x.x.x
 * OBJ(paddr,1) = router ID
 */

EOLS (router_bgp_router_id_eol, bgp_command, BGP_ROUTER_ID);
IPADDR (router_bgp_router_id_ipaddr, router_bgp_router_id_eol, no_alt,
	OBJ(paddr,1), "Manually configured router identifier");
NOPREFIX(router_bgp_router_id_no, router_bgp_router_id_ipaddr,
	 router_bgp_router_id_eol);
NVGENS (router_bgp_router_id_nv, router_bgp_router_id_no, bgp_command,
	BGP_ROUTER_ID);
KEYWORD (router_bgp_router_id, router_bgp_router_id_nv, router_bgp_fastreset,
	 "router-id", "Override configured router identifier",
	 PRIV_CONF);

/**********************************************************************
 * bgp redistribute-internal
 */
EOLS (router_bgp_redistibgp_eol, bgp_command, BGP_REDIST_IBGP);
KEYWORD (router_bgp_redistibgp, router_bgp_redistibgp_eol,
	 router_bgp_router_id, "redistribute-internal",
	 "Allow redistribution of iBGP into IGPs (dangerous)", PRIV_CONF);

/**********************************************************************
 * bgp
 */

KEYWORD (router_bgp_kw, router_bgp_redistibgp, NONE,
	 "bgp", "BGP specific commands", PRIV_CONF);

ASSERT	(router_bgp, router_bgp_kw, ALTERNATE,
	 (((pdbtype *) csb->protocol)->family == PDB_IP &&
	  ((pdbtype *) csb->protocol)->proctype == PROC_BGP));

#undef	ALTERNATE
#define	ALTERNATE	router_bgp
