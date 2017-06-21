/* $Id: cfg_int_vines.h,v 3.4.8.2 1996/08/12 16:09:07 widmer Exp $
 * $Source: /release/112/cvs/Xsys/vines/cfg_int_vines.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ V I N E S . H
 * 
 * Vines interface subcommands.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_vines.h,v $
 * Revision 3.4.8.2  1996/08/12  16:09:07  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.8.1  1996/06/04  01:46:54  ehk
 * CSCdi57934:  VINES: LAN Extender will not work with VINES.
 * Branch: California_branch
 * set default vines encapsulation to ARPA on LEX port and
 * made it user configurable.
 *
 * Revision 3.4  1996/02/27  19:17:43  ehk
 * CSCdi49568:  vines access-group should not accept 200-299 lists
 *
 * upper limit change of the vines access-group to 200 (VINESMAXEACC).
 *
 * Revision 3.3  1996/02/01  23:34:52  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:57:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:03  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/21  18:16:59  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:08:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * vines arp-enable [dynamic]
 * no vines arp-enable [dynamic]
 */
EOLNS	   (ci_v_arp_eol, varp_command);
KEYWORD_ID (ci_v_arp_dynamic, ci_v_arp_eol, ci_v_arp_eol,
	    OBJ(int,1), TRUE,
	    "dynamic", "Enable ARP dynamically", PRIV_CONF|PRIV_SUBIF);
NVGENNS	   (ci_v_arp_nvgen, ci_v_arp_dynamic, varp_command);
KEYWORD	   (ci_vines_arp, ci_v_arp_nvgen, no_alt,
	    "arp-enable", "Supply VINES addresses to booting clients",
	    PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * [no] vines update deltas
 *
 * vines update interval <milliseconds>
 * no vines update interval
 */

EOLS	 (ci_v_u_interval_eol, vinesif_command, VINES_UPDATEINTERVAL);
SET	 (ci_v_u_interval_set, ci_v_u_interval_eol, OBJ(int,2), TRUE);
NUMBER	 (ci_v_u_interval_num, ci_v_u_interval_set, ci_v_u_interval_eol,
	  OBJ(int,1), 0, MAXUINT, "Update interval in seconds");
NVGENS	 (ci_v_u_interval_nvgen, ci_v_u_interval_num, vinesif_command,
	  VINES_UPDATEINTERVAL);
KEYWORD	 (ci_v_u_interval, ci_v_u_interval_nvgen, no_alt,
	  "interval", "Set RTP update interval",
	  PRIV_CONF|PRIV_SUBIF);

EOLS	 (ci_v_u_delta_eol, vinesif_command, VINES_UPDATEDELTAS);
NVGENS	 (ci_v_u_delta_nvgen, ci_v_u_delta_eol,
	  vinesif_command, VINES_UPDATEDELTAS);
KEYWORD	 (ci_v_u_delta, ci_v_u_delta_nvgen, ci_v_u_interval,
	  "deltas", "Send only RTP delta updates",
	  PRIV_CONF|PRIV_SUBIF);

KEYWORD	 (ci_vines_update, ci_v_u_delta, ci_vines_arp,
	  "update", "Set RTP update characteristics for this interface",
	  PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * vines svc line <line>
 * vines svc slot <slot>
 * no vines svc line
 * no vines svc slot
 */
EOLS	 (ci_v_svc_s_eol, vinesif_command, VINES_SVC_CFG_SLOT);
NUMBER   (ci_v_svc_s_num, ci_v_svc_s_eol, no_alt,
	  OBJ(int,1), 0, 15, NULL /*"Slot number"*/);
NOPREFIX (ci_v_svc_s_no, ci_v_svc_s_num, ci_v_svc_s_eol);
NVGENS	 (ci_v_svc_s_nv, ci_v_svc_s_no, vinesif_command, VINES_SVC_CFG_SLOT);
KEYWORD  (ci_v_svc_slot, ci_v_svc_s_nv, no_alt,
	  "slot", NULL /*"Interface \"slot\" number"*/, PRIV_CONF|PRIV_SUBIF);

EOLS	 (ci_v_svc_l_eol, vinesif_command, VINES_SVC_CFG_LINE);
NUMBER   (ci_v_svc_l_num, ci_v_svc_l_eol, no_alt,
	  OBJ(int,1), 0, 4095, NULL /*"Line number"*/);
NOPREFIX (ci_v_svc_l_no, ci_v_svc_l_num, ci_v_svc_l_eol);
NVGENS	 (ci_v_svc_l_nv, ci_v_svc_l_no, vinesif_command, VINES_SVC_CFG_LINE);
KEYWORD  (ci_v_svc_line, ci_v_svc_l_nv, ci_v_svc_slot,
	  "line", NULL /*"Serial line number"*/, PRIV_CONF|PRIV_SUBIF);

KEYWORD	 (ci_vines_svc, ci_v_svc_line, ci_vines_update,
	  "svc", NULL /*"Make visible to VINES services"*/,
	  PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);


/************************************************************************
 * vines split-horizon
 * no vines split-horizon
 */
EOLS	(ci_vines_spliteol, vinesif_command, VINES_SPLIT);
KEYWORD	(ci_vines_split, ci_vines_spliteol, ci_vines_svc,
	 "split-horizon", "Split-horizon routing updates",
	 PRIV_CONF|PRIV_SUBIF);


/************************************************************************
 * vines serverless [broadcast | dynamic]
 * no vines serverless [broadcast | dynamic]
 */
EOLS	(ci_v_sv_eol, vinesif_command, VINES_SERVERLESS);
KEYWORD_ID(ci_v_sv_bcast, ci_v_sv_eol, ci_v_sv_eol,
	   OBJ(int,2), TRUE,
	   "broadcast", "Send broadcasts to all attached networks",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(ci_v_sv_dynamic, ci_v_sv_eol, ci_v_sv_bcast,
	   OBJ(int,1), TRUE,
	   "dynamic", "Enable serverless support dynamically",
	   PRIV_CONF|PRIV_SUBIF);
NVGENS	(ci_v_sv_nvgen, ci_v_sv_dynamic,
	 vinesif_command, VINES_SERVERLESS);
KEYWORD	(ci_vines_serverless, ci_v_sv_nvgen, ci_vines_split,
	 "serverless", "The attached network has no Banyan server",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * vines route-cache
 * no vines route-cache
 */
EOLS	(ci_vines_routeceol, vinesif_command, VINES_ROUTEC);
KEYWORD	(ci_vines_routec, ci_vines_routeceol, ci_vines_serverless,
	 "route-cache", "Fast switch VINES packets out this interface",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * vines redirect <seconds>
 * no vines redirect [<seconds>]
 */
PARAMS_KEYONLY	(ci_vines_redirect, ci_vines_routec, "redirect",
		 OBJ(int,1), 0, MAXUINT,
		 vinesif_command, VINES_REDIRECTCMD,
		 "Set redirect interval",
		 "Redirect interval, in milliseconds (0 for never)",
		 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * vines propagate [dynamic]
 * no vines propagate [dynamic]
 */
EOLS	   (ci_v_prop_eol, vinesif_command, VINES_PROPAGATE);
KEYWORD_ID (ci_v_prop_dynamic, ci_v_prop_eol, ci_v_prop_eol,
	    OBJ(int,1), TRUE,
	    "dynamic", "Enable propagation dynamically",
	    PRIV_CONF|PRIV_SUBIF);
NVGENS	   (ci_v_prop_nvgen, ci_v_prop_dynamic,
	    vinesif_command, VINES_PROPAGATE);
KEYWORD	   (ci_vines_prop, ci_v_prop_nvgen, ci_vines_redirect,
	    "propagate", "Ignore 'class' field during broadcast forwarding",
	    PRIV_CONF|PRIV_SUBIF);


/************************************************************************
 * vines output-network-filter <number>
 * no vines output-network-filter
 */
EOLS	(ci_vines_onf_eol, vinesif_command, VINES_NET_FILTER_OUT);
NUMBER	(ci_vines_onf_list, ci_vines_onf_eol, no_alt,
	 OBJ(int,1), VINESMINSACC, VINESMAXSACC, "Access list for filtering");
NOPREFIX(ci_vines_onf_no, ci_vines_onf_list, ci_vines_onf_eol);
NVGENS	(ci_vines_onf_nvgen, ci_vines_onf_no, vinesif_command,
	 VINES_NET_FILTER_OUT);
KEYWORD	(ci_vines_onf, ci_vines_onf_nvgen, ci_vines_prop,
	 "output-network-filter", "Filter networks sent in routing updates",
	 PRIV_CONF|PRIV_SUBIF);


/************************************************************************
 * vines neighbor <net:host> <mac-address> <encapsulation> [<metric>]
 * no vines neighbor <net:host> <mac-address> [<encapsulation>]
 */

EOLS	 (ci_vines_neighbor_eol, vinesif_command, VINES_NEIGHBOR);
KEYWORD_ID (ci_v_n_panic, ci_vines_neighbor_eol, ci_vines_neighbor_eol,
	    OBJ(int,9), TRUE,
	    "panic", "Point all routes at this entry",
	    PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);
SET	 (ci_vines_neighbor_metric_set, ci_v_n_panic, OBJ(int,4), TRUE);
NUMBER   (ci_v_n_fractional, ci_vines_neighbor_metric_set,
	  ci_vines_neighbor_metric_set,
	  OBJ(int,3), 0, 9999,
	  "metric - fractional part (in 10,000ths)");
NUMBER   (ci_v_n_whole, ci_v_n_fractional, ci_vines_neighbor_metric_set,
	  OBJ(int,2), 0, 4095,
	  "metric - whole number part");
ENCAPS	 (ci_vines_neighbor_encaptr, ci_v_n_whole, no_alt,
	  OBJ(int,1), ET_VINES_TR, "vines-tr", "VINES tokenring encapsulation",
	  IDB_TR);
ENCAPS	 (ci_vines_neighbor_encaparpa, ci_v_n_whole, ci_vines_neighbor_encaptr,
	  OBJ(int,1), ET_ARPA, "arpa", "Standard Ethernet version 2.0",
	  IDB_ETHER);
KEYWORD_ID (ci_vines_neighbor_encapsnap, ci_v_n_whole,
	    ci_vines_neighbor_encaparpa,
	    OBJ(int,1), ET_SNAP,
	    "snap", "802.2 encapsulation", PRIV_CONF|PRIV_SUBIF);
NOPREFIX (ci_vines_neighbor_noencap, ci_vines_neighbor_encapsnap,
	  ci_vines_neighbor_eol);
ENADDR   (ci_vines_neighbor_mac, ci_vines_neighbor_noencap, no_alt,
	  OBJ(hwaddr,1), "MAC address");
TESTVAR	(ci_v_n_lan_test, ci_vines_neighbor_mac,
	 NONE, NONE, NONE, ci_v_n_whole,
	 OBJ(int,5), 0);
VINADDR	 (ci_vines_neighbor_addr, ci_v_n_lan_test, no_alt,
	  OBJ(paddr,1), "VINES address");
NVGENS	 (ci_vines_neighbor_nvgen, ci_vines_neighbor_addr, vinesif_command,
	  VINES_NEIGHBOR);
KEYWORD  (ci_vines_neighbor_kwd, ci_vines_neighbor_nvgen, no_alt,
	  "neighbor", "Create a static neighbor entry",
	  PRIV_CONF|PRIV_SUBIF/*|PRIV_HIDDEN*/);

SET      (ci_v_n_ppp_set_station, ci_vines_neighbor_kwd,
	  OBJ(int,5), STATION_PPP);
SET      (ci_v_n_ppp_set_et, ci_v_n_ppp_set_station,
	  OBJ(int,1), ET_PPP);
ASSERT   (ci_v_n_serial_ppp, ci_v_n_ppp_set_et, no_alt,
	  (csb->interface->hwptr->enctype == ET_PPP));

SET      (ci_v_n_hdlc_set_station, ci_vines_neighbor_kwd,
	  OBJ(int,5), STATION_CISCO_HDLC);
SET      (ci_v_n_hdlc_set_et, ci_v_n_hdlc_set_station, OBJ(int,1), ET_HDLC);
ASSERT   (ci_v_n_serial_hdlc, ci_v_n_hdlc_set_et, ci_v_n_serial_ppp,
	  (csb->interface->hwptr->enctype == ET_HDLC));
TEST_IDBSTATUS(ci_v_n_serial_test, ci_v_n_serial_hdlc, NONE, ci_vines_onf,
	       IDB_SERIAL);

TEST_IDBSTATUS(ci_vines_neighbor, ci_vines_neighbor_kwd, NONE,
	       ci_v_n_serial_test, (IDB_ETHER|IDB_FDDI|IDB_TR));

/************************************************************************
 * vines metric [<number>]
 * no vines metric
 */
EOLS	 (ci_v_m_eol, vinesif_command, VINES_COST);
SET	 (ci_v_m_set, ci_v_m_eol, OBJ(int,3), TRUE);
NUMBER   (ci_v_m_fractional, ci_v_m_set, ci_v_m_set,
	  OBJ(int,2), 0, 9999, "Fractional part (in 10,000ths)");
NUMBER   (ci_v_m_whole, ci_v_m_fractional, ci_v_m_eol,
	  OBJ(int,1), 0, 4095, "Whole number part");
NVGENS	 (ci_v_m_nvgen, ci_v_m_whole, vinesif_command, VINES_COST);
KEYWORD	 (ci_vines_metric, ci_v_m_nvgen, ci_vines_neighbor,
	  "metric", "VINES processing on the selected interface",
	  PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * vines input-router-filter <number>
 * no vines input-router-filter
 */
PARAMS_KEYONLY(ci_vines_irf, ci_vines_metric, "input-router-filter",
	       OBJ(int,1), VINESMINSACC, VINESMAXSACC,
	       vinesif_command, VINES_ROUTER_FILTER_IN,
	       "Filter source of received routing updates",
	       "Access list for filtering",
	       PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * vines input-network-filter <number>
 * no vines input-network-filter
 */
PARAMS_KEYONLY(ci_vines_inf, ci_vines_irf, "input-network-filter",
	       OBJ(int,1), VINESMINSACC, VINESMAXSACC,
	       vinesif_command, VINES_NET_FILTER_IN,
	       "Filter networks in received routing updates",
	       "Access list for filtering",
	       PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * [no] vines encapsulation [{ arpa | snap | vines-tr }]
 *
 * OBJ(int,1) = ET_ARPA, ET_SNAP, ET_VINES_TR
 *
 * The 'no' version sets encapsulation to 'arpa' for ethernets,
 * 'vines-tr' for token ring, and 'snap' for all other interfaces.
 * This is equivalent to stating 'vines encapsulation'.
 */

EOLS	 (ci_vines_encapeol, vinesif_command, VINES_ENCAPS);
ENCAPS	 (ci_vines_encaptr, ci_vines_encapeol, ci_vines_encapeol,
	  OBJ(int,1), ET_VINES_TR,
	  "vines-tr", "VINES tokenring encapsulation", IDB_TR);
ENCAPS	 (ci_vines_encaparpa, ci_vines_encapeol, ci_vines_encaptr,
	  OBJ(int,1), ET_ARPA,
	  "arpa", "Standard Ethernet version 2.0", IDB_ETHER|IDB_LEX);
KEYWORD_ID(ci_vines_encapsnap, ci_vines_encapeol, ci_vines_encaparpa,
	   OBJ(int,1), ET_SNAP,
	   "snap", "802.2 encapsulation", PRIV_CONF|PRIV_SUBIF);
NOPREFIX (ci_vines_noencap, ci_vines_encapsnap, ci_vines_encapeol);
NVGENS	 (ci_vines_encapnvgen, ci_vines_noencap, vinesif_command,
	  VINES_ENCAPS);
KEYWORD	 (ci_vines_encap, ci_vines_encapnvgen, ci_vines_inf,
	  "encapsulation", "VINES encapsulation", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * vines access-group <number>
 * no vines access-group [<number>]
 */
PARAMS_KEYONLY(ci_vines_accessgrp, ci_vines_encap, "access-group",
	       OBJ(int,1), VINESMINACC, VINESMAXEACC,
	       vinesif_command, VINES_ACCESSGROUP,
	       "Apply a VINES access list",
	       "A valid VINES access list number", PRIV_CONF|PRIV_SUBIF);

KEYWORD	(ci_vines, ci_vines_accessgrp, ALTERNATE,
	 "vines", "VINES interface subcommands", PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_vines
