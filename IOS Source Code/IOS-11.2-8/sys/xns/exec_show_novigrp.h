/* $Id: exec_show_novigrp.h,v 3.1.2.5 1996/08/16 00:14:29 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_show_novigrp.h,v $
 *------------------------------------------------------------------
 * IPX EIGRP show commands.
 *
 * April, 1996, Steven Luong
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_novigrp.h,v $
 * Revision 3.1.2.5  1996/08/16  00:14:29  hampton
 * Change parser comments to reflect ipx command usage.  [CSCdi66208]
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/08/12  16:10:07  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.3  1996/07/24  13:08:19  boyle
 * CSCdi19400:  show ipx eigrp topology should give an option for AS number
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/21  10:11:19  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.2.1  1996/05/03  02:16:35  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.1  1996/04/13  01:24:44  sluong
 * Add placeholders
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ipx eigrp traffic [<as>]
 *
 * OBJ(int,1) = <asystem>
 */
EOLNS (show_nov_igrp_traffic_eols, novigrp_show_traffic);
NUMBER (show_nov_igrp_traffic_as, show_nov_igrp_traffic_eols,
        show_nov_igrp_traffic_eols, OBJ(int,1), 1, 0xffff, "AS Number");
KEYWORD (show_nov_igrp_traffic, show_nov_igrp_traffic_as, no_alt,
         "traffic", "IPX-EIGRP Traffic Statistics", PRIV_USER);

/******************************************************************
 * show ipx eigrp event [<as>]
 *
 * OBJ(int,1) = <asystem>
 */
EOLS (show_nov_igrp_event_eols, novigrp_show_event, IGRP2_EVENT_CUR);
NUMBER (show_nov_igrp_event_as, show_nov_igrp_event_eols,
        show_nov_igrp_event_eols, OBJ(int,1), 1, 0xffff, "AS Number");
KEYWORD (show_nov_igrp_event, show_nov_igrp_event_as, show_nov_igrp_traffic,
         "events", "IPX-EIGRP Events", PRIV_USER | PRIV_HIDDEN);

/******************************************************************
 * show ipx eigrp sia-event [<asystem>]
 *
 * OBJ(int,1) = <asystem>
 */
EOLS (show_nov_igrp_siaevent_eols, novigrp_show_event, IGRP2_EVENT_SIA);
NUMBER (show_nov_igrp_siaevent_as, show_nov_igrp_siaevent_eols,
        show_nov_igrp_siaevent_eols, OBJ(int,1), 1, 0xffff, "AS Number");
KEYWORD (show_nov_igrp_siaevent, show_nov_igrp_siaevent_as,
         show_nov_igrp_event,
         "sia-event", "IPX-EIGRP SIA event", PRIV_USER | PRIV_HIDDEN);

/******************************************************************
 * show ipx eigrp topology [<AS>] [<network> <mask>
 *                                    | all-links | active | zero-successors]
 *
 * OBJ(int,1) = <asystem>
 * OBJ(int,2) = <IPX address>
 * OBJ(int,3) = <Network mask>
 */
EOLS (show_nov_igrp_top_eols, novigrp_show_topology, IGRP2_TOP_FS);
EOLS (show_nov_igrp_top_as_eols, novigrp_show_topology, IGRP2_TOP_AS);
NUMBER (show_nov_igrp_top_as_num, show_nov_igrp_top_as_eols, no_alt,
	OBJ(int,1), 1, 0xffff, "Autonomous system number");
KEYWORD(show_nov_igrp_top_as, show_nov_igrp_top_as_num,
	show_nov_igrp_top_eols,
	"as", "Show only entries for this autonomous system", PRIV_USER);
EOLS (show_nov_igrp_top_all_eols, novigrp_show_topology, IGRP2_TOP_ALL);
KEYWORD(show_nov_igrp_top_all, show_nov_igrp_top_all_eols,
	show_nov_igrp_top_as,
	"all-links", "Show all links in topology table", PRIV_USER);
EOLS (show_nov_igrp_top_zero_eols, novigrp_show_topology, IGRP2_TOP_ZERO);
KEYWORD (show_nov_igrp_top_zero, show_nov_igrp_top_zero_eols,
	 show_nov_igrp_top_all,
	 "zero-successors", "Show only zero successor entries", PRIV_USER);
EOLS (show_nov_igrp_top_pend_eols, novigrp_show_topology, IGRP2_TOP_PENDING);
KEYWORD (show_nov_igrp_top_pend, show_nov_igrp_top_pend_eols,
	 show_nov_igrp_top_zero,
	 "pending", "Show only entries pending transmission", PRIV_USER);
EOLS (show_nov_igrp_top_summ_eols, novigrp_show_topology, IGRP2_TOP_SUMMARY);
KEYWORD (show_nov_igrp_top_summ, show_nov_igrp_top_summ_eols,
	 show_nov_igrp_top_pend,
	 "summary", "Show a summary of the topology table", PRIV_USER);
EOLS    (show_nov_igrp_top_active_eol, novigrp_show_topology, 
	 IGRP2_TOP_ACTIVE);
KEYWORD (show_nov_igrp_top_active, show_nov_igrp_top_active_eol,
	 show_nov_igrp_top_summ,
	 "active", "Show only active entries", PRIV_USER);
HEXDIGIT (show_nov_igrp_top_mask, show_nov_igrp_top_eols, 
	  show_nov_igrp_top_eols,
	 OBJ(int,3), 0x80000000, 0xffffffff, "IPX network mask");
HEXNUM  (show_nov_igrp_top_addr, show_nov_igrp_top_mask,
	show_nov_igrp_top_active,
	OBJ(int,2), "Network to display information about");
KEYWORD (show_nov_igrp_top, show_nov_igrp_top_addr, show_nov_igrp_siaevent,
         "topology", "IPX EIGRP Topology Table", PRIV_USER);

/******************************************************************
  * show ipx eigrp neighbors|interface  [servers | detail] <asystem> [<interface><unit>]
 *
 * OBJ(int,1) = <asystem>
 * OBJ(idb,1) = <interface><unit>
 * OBJ(int,2) != 0 if server detail wanted
 * OBJ(int,3) != 0 if neighbor detail wanted
 * OBJ(int,4) != 0 if "neighbor" specified; == 0 if common_str_interface specified
 */
pdecl(show_nov_igrp_nbrs_int);
EOLNS (show_nov_igrp_nbrs_eols, novigrp_show_neighbors);
NUMBER (show_nov_igrp_nbrs_as, show_nov_igrp_nbrs_int,
        show_nov_igrp_nbrs_eols, OBJ(int,1), 1, 0xffff, "AS Number");
INTERFACE (show_nov_igrp_nbrs_int, show_nov_igrp_nbrs_as,
           show_nov_igrp_nbrs_as, OBJ(idb,1), IFTYPE_ANYSUB);

KEYWORD_ID (show_nov_igrp_nbrs_det, show_nov_igrp_nbrs_int,
	    show_nov_igrp_nbrs_int, OBJ(int,3), TRUE,
	    "detail", "Show detailed peer information", PRIV_USER);
KEYWORD_ID (show_nov_igrp_nbrs_servers, show_nov_igrp_nbrs_det,
	    show_nov_igrp_nbrs_det, OBJ(int,2), TRUE,
	    "servers", "Show neighbor's server advertisements", PRIV_USER);
KEYWORD (show_nov_igrp_int, show_nov_igrp_nbrs_det, show_nov_igrp_top,
 	 "interfaces", "IPX EIGRP Interfaces", PRIV_USER);
KEYWORD_ID (show_nov_igrp_nbrs, show_nov_igrp_nbrs_servers, show_nov_igrp_int,
 	    OBJ(int,4), TRUE, "neighbors", "IPX EIGRP Neighbors", PRIV_USER);


KEYWORD (show_nov_igrp, show_nov_igrp_nbrs, ALTERNATE,
         "eigrp", "IPX EIGRP show commands", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_nov_igrp
