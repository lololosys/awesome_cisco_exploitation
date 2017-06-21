/* $Id: exec_show_ip_eigrp.h,v 3.2 1996/11/12 04:35:47 dkatz Exp $
 * $Source: /release/111/cvs/Xsys/iprouting/exec_show_ip_eigrp.h,v $
 *------------------------------------------------------------------
 * IP EIGRP show command parse chains
 *
 * November 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_ip_eigrp.h,v $
 * Revision 3.2  1996/11/12 04:35:47  dkatz
 * CSCdi76003:  EIGRP isnt modular
 * Clean up the tendrils reaching into EIGRP from the generic IP routing
 * code.
 *
 * Revision 3.1  1996/11/12 03:46:56  dkatz
 * Add placeholders.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ip eigrp event [<asystem>] [<start #> <end #>]
 *
 * OBJ(int,1) = <asystem>
 * OBJ(int,2) = <start #>
 * OBJ(int,3) = <end #>
 */
EOLS (show_ip_igrp2_event_eols, ipigrp2_show_event, IGRP2_EVENT_CUR);
NUMBER (show_ip_igrp2_event_end, show_ip_igrp2_event_eols, no_alt, OBJ(int,3),
 	1, MAXUINT, "Ending event number");
NUMBER (show_ip_igrp2_event_start, show_ip_igrp2_event_end,
 	show_ip_igrp2_event_eols, OBJ(int,2), 1, MAXUINT,
 	"Starting event number");
NUMBER (show_ip_igrp2_event_as, show_ip_igrp2_event_start,
 	show_ip_igrp2_event_eols, OBJ(int,1), 1, 0xffff, "AS Number");
KEYWORD (show_ip_igrp2_event, show_ip_igrp2_event_as, no_alt,
 	 "events", "IP-EIGRP Events", PRIV_USER | PRIV_HIDDEN);

/******************************************************************
 * show ip eigrp sia-event [<asystem>] [<start #> <end #>]
 *
 * OBJ(int,1) = <asystem>
 * OBJ(int,2) = <start #>
 * OBJ(int,3) = <end #>
 */
EOLS (show_ip_igrp2_siaevent_eols, ipigrp2_show_event, IGRP2_EVENT_SIA);
NUMBER (show_ip_igrp2_siaevent_end, show_ip_igrp2_siaevent_eols, no_alt,
 	OBJ(int,3), 1, MAXUINT, "Ending event number");
NUMBER (show_ip_igrp2_siaevent_start, show_ip_igrp2_siaevent_end,
 	show_ip_igrp2_siaevent_eols, OBJ(int,2), 1, MAXUINT,
 	"Starting event number");
NUMBER (show_ip_igrp2_siaevent_as, show_ip_igrp2_siaevent_start,
 	show_ip_igrp2_siaevent_eols, OBJ(int,1), 1, 0xffff, "AS Number");
KEYWORD (show_ip_igrp2_siaevent, show_ip_igrp2_siaevent_as,
	 show_ip_igrp2_event,
	 "sia-event", "IP-EIGRP SIA event", PRIV_USER | PRIV_HIDDEN);

/******************************************************************
 * show ip eigrp timers [<asystem>]
 *
 * OBJ(int,1) = <asystem>
 */
EOLS (show_ip_igrp2_timers_eols, ipigrp2_show_timers, 0);
NUMBER (show_ip_igrp2_timers_as, show_ip_igrp2_timers_eols,
	show_ip_igrp2_timers_eols, OBJ(int,1), 1, 0xffff, "AS Number");
KEYWORD (show_ip_igrp2_timers, show_ip_igrp2_timers_as,
	 show_ip_igrp2_siaevent,
	 "timers", "IP-EIGRP Timers", PRIV_USER|PRIV_HIDDEN);

/******************************************************************
 * show ip eigrp traffic [<asystem>]
 *
 * OBJ(int,1) = <asystem>
 */
EOLS (show_ip_igrp2_traffic_eols, ipigrp2_show_traffic, 0);
NUMBER (show_ip_igrp2_traffic_as, show_ip_igrp2_traffic_eols,
	show_ip_igrp2_traffic_eols, OBJ(int,1), 1, 0xffff, "AS Number");
KEYWORD (show_ip_igrp2_traffic, show_ip_igrp2_traffic_as,
	 show_ip_igrp2_timers,
	 "traffic", "IP-EIGRP Traffic Statistics", PRIV_USER);

/******************************************************************
 * show ip eigrp topology [<asystem>]
 *                        [<IP address> [<IP mask>]
 *                         | active | zero-successors | all-links]
 *
 * OBJ(int,1) = <asystem>
 * OBJ(int,2) = TRUE if IP address is specified.
 * OBJ(paddr,1) = <IP address>
 * OBJ(paddr,2) = <IP mask>
 */
EOLS (show_ip_igrp2_top_eols, ipigrp2_show_topology, IGRP2_TOP_FS);
EOLS (show_ip_igrp2_top_all_eols, ipigrp2_show_topology, IGRP2_TOP_ALL);
KEYWORD(show_ip_igrp2_top_all, show_ip_igrp2_top_all_eols,
	show_ip_igrp2_top_eols,
	"all-links", "Show all links in topology table", PRIV_USER);
EOLS (show_ip_igrp2_top_zero_eols, ipigrp2_show_topology, IGRP2_TOP_ZERO);
KEYWORD (show_ip_igrp2_top_zero, show_ip_igrp2_top_zero_eols,
	 show_ip_igrp2_top_all,
	 "zero-successors", "Show only zero successor entries", PRIV_USER);
EOLS (show_ip_igrp2_top_pend_eols, ipigrp2_show_topology, IGRP2_TOP_PENDING);
KEYWORD (show_ip_igrp2_top_pend, show_ip_igrp2_top_pend_eols,
	 show_ip_igrp2_top_zero,
	 "pending", "Show only entries pending transmission", PRIV_USER);
EOLS (show_ip_igrp2_top_summ_eols, ipigrp2_show_topology, IGRP2_TOP_SUMMARY);
KEYWORD (show_ip_igrp2_top_summ, show_ip_igrp2_top_summ_eols,
	 show_ip_igrp2_top_pend,
	 "summary", "Show a summary of the topology table", PRIV_USER);
EOLS    (show_ip_igrp2_top_active_eol, ipigrp2_show_topology, 
	 IGRP2_TOP_ACTIVE);
KEYWORD (show_ip_igrp2_top_active, show_ip_igrp2_top_active_eol,
	 show_ip_igrp2_top_summ,
	 "active", "Show only active entries", PRIV_USER);
IPADDR (show_ip_igrp2_top_mask, show_ip_igrp2_top_eols, 
	show_ip_igrp2_top_eols, OBJ(paddr,2), "Network mask");
SET (show_ip_igrp2_top_set, show_ip_igrp2_top_mask, OBJ(int,2), TRUE);
IPADDR (show_ip_igrp2_top_ipaddr, show_ip_igrp2_top_set, 
	show_ip_igrp2_top_active, OBJ(paddr,1), 
	"Network to display information about");
NUMBER (show_ip_igrp2_top_as, show_ip_igrp2_top_ipaddr,
	show_ip_igrp2_top_ipaddr, OBJ(int,1), 1, 0xffff, "AS Number");
KEYWORD (show_ip_igrp2_top, show_ip_igrp2_top_as, show_ip_igrp2_traffic,
	 "topology", "IP-EIGRP Topology Table", PRIV_USER);

/******************************************************************
 * show ip eigrp {neighbors|interface} <asystem> [detail] [<interface><unit>]
 *
 * OBJ(int,1) = <asystem>
 * OBJ(idb,1) = <interface><unit>
 * OBJ(int,2) = TRUE for detailed information
 * OBJ(int,3) = command type (neighbors/interface)
 */
pdecl(show_ip_igrp2_nbrs_int);
EOLS (show_ip_igrp2_nbrs_eols, ipigrp2_show_neighbors, 0);
NUMBER (show_ip_igrp2_nbrs_as, show_ip_igrp2_nbrs_int, 
	show_ip_igrp2_nbrs_eols, OBJ(int,1), 1, 0xffff, "AS Number");
INTERFACE (show_ip_igrp2_nbrs_int, show_ip_igrp2_nbrs_as, 
	   show_ip_igrp2_nbrs_as, OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD_ID (show_ip_igrp2_nbrs_det, show_ip_igrp2_nbrs_int,
	    show_ip_igrp2_nbrs_int, OBJ(int,2), TRUE,
	    "detail", "Show detailed peer information", PRIV_USER);
KEYWORD_ID (show_ip_igrp2_int, show_ip_igrp2_nbrs_det, show_ip_igrp2_top,
 	    OBJ(int,3), EIGRP_SHOW_INTERFACE,
 	    "interfaces", "IP-EIGRP interfaces", PRIV_USER);
KEYWORD_ID (show_ip_igrp2_nbrs, show_ip_igrp2_nbrs_det, show_ip_igrp2_int,
 	    OBJ(int,3), EIGRP_SHOW_NEIGHBORS,
 	    "neighbors", "IP-EIGRP neighbors", PRIV_USER);
KEYWORD (show_ip_igrp2, show_ip_igrp2_nbrs, ALTERNATE,
	 "eigrp", "IP-EIGRP show commands", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_igrp2
