/* $Id: ipigrp2.c,v 3.14.10.26 1996/09/10 18:09:46 sluong Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ipigrp2.c,v $
 *------------------------------------------------------------------
 * IGRP2 ip specific routines.
 *
 * April 1992, Bob Albrightson
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipigrp2.c,v $
 * Revision 3.14.10.26  1996/09/10  18:09:46  sluong
 * CSCdi65486:  IPX EIGRP Updates not Propogated with MTU < EIGRP packet
 * Size
 * Branch: California_branch
 *
 * Revision 3.14.10.25  1996/09/04  23:46:50  mikel
 * CSCdi65889:  offset-list doesnt work for EIGRP
 * Branch: California_branch
 * Re-enable offset-list command as well as adding offset-list out support,
 * and fix offset-list to work with access-lists
 *
 * Revision 3.14.10.24  1996/08/28  12:52:39  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.14.10.23  1996/08/27  20:23:47  sluong
 * CSCdi66169:  IPX-EIGRP can eventually crash router when there is a
 * flapping link.
 * Branch: California_branch
 *
 * Revision 3.14.10.22  1996/07/23  18:32:10  snyder
 * CSCdi63791:  debugging arrays should be const
 *              4112 out of data space, 556 out of image
 * Branch: California_branch
 *
 * Revision 3.14.10.21  1996/06/28  02:49:09  dkatz
 * CSCdi61545:  EIGRP doesnt limit hop count
 * Branch: California_branch
 * If the hop count exceeds the maximum, act as though an infinite metric
 * was received.
 *
 * Revision 3.14.10.20  1996/06/28  02:40:30  dkatz
 * CSCdi61491:  EIGRP may lose routes
 * Branch: California_branch
 * Only ignore the routes we are supposed to ignore.
 *
 * Revision 3.14.10.19  1996/06/28  01:20:56  snyder
 * CSCdi61601:  fix some char * that should have been char [] in iprouting
 * Branch: California_branch
 *
 *              You get space back for the pointer
 *              You get space back for each reference
 *              You get bytes out of data space
 *
 * Revision 3.14.10.18  1996/06/27  04:41:04  mikel
 * CSCdi37749:  <debug ip eigrp> help wrong
 * Branch: California_branch
 * clean up ip-eigrp AS debugging
 *
 * Revision 3.14.10.17  1996/06/18  16:46:37  dkatz
 * CSCdi60515:  EIGRP summaries stay in topo table
 * Branch: California_branch
 * Rework the decision process for doing topo table searches for
 * the min metric under a summary.  Get rid of the metric count
 * field.  Add a hook to suppress multiple enqueued requests for
 * a min metric search.
 *
 * Revision 3.14.10.16  1996/06/18  16:42:24  dkatz
 * CSCdi59446:  EIGRP doesnt advertise route matching unused summary
 * Branch: California_branch
 * Don't suppress the advertisement of a route on non-summarized
 * interfaces if the route matches the summary and there are no
 * component routes to the summary.  Add logging of summary table
 * changes.  While the hood is up, fix a problem where the peer
 * table would thrash when adjacencies are rejected.
 *
 * Revision 3.14.10.15  1996/06/18  01:47:29  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.14.10.14  1996/06/17  23:33:42  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.14.10.13  1996/05/28  01:21:26  dkatz
 * CSCdi21082:  IP-EIGRP: Auto-summary mishandled when configuring no ip
 * address
 * Branch: California_branch
 * Ensure that the lowest metric is selected.  Don't over-optimize.
 *
 * Revision 3.14.10.12  1996/05/27  22:45:22  dkatz
 * CSCdi21082:  IP-EIGRP: Auto-summary mishandled when configuring no ip
 * address
 * Branch: California_branch
 * Rewrite autosummary code.  Optimize a couple of unnecessary topo
 * database walks.  Fix potential multicast mishandling.
 *
 * Revision 3.14.10.11  1996/05/23  07:37:41  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.14.10.10  1996/05/17  17:22:42  albright
 * CSCdi57743:  EIGRP does not redistr. not direct connected static routes
 * Branch: California_branch
 *
 * Revision 3.14.10.9  1996/05/04  01:21:43  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.14.10.8  1996/04/15  21:18:04  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.14.10.7  1996/04/08  04:33:44  dkatz
 * CSCdi53903:  EIGRP stops receiving hellos after interface transition
 * Branch: California_branch
 * Don't disable multicasts when the interface goes down due to natural
 * causes.
 *
 * Revision 3.14.10.6  1996/04/03  20:15:02  mikel
 * CSCdi52078:  IP multicast goes away when two EIGRP AS are configured
 * Branch: California_branch
 * moved the eigrp multicast deconfigure routine to where we
 * actually process the interface down event
 *
 * Revision 3.14.10.5  1996/04/03  08:11:45  dkatz
 * CSCdi53499:  EIGRP summaries cant be parsed from NVRAM
 * Branch: California_branch
 *
 * Revision 3.14.10.4  1996/04/02  07:29:23  dkatz
 * CSCdi52277:  Redistribution metric problems with IGRP/EIGRP
 * Branch: California_branch
 * Always store canonical metrics scaled for IGRP, and make EIGRP do
 * its own scaling internally.  Get the scaling right in all three
 * places.
 *
 * Revision 3.14.10.3  1996/04/02  07:20:39  dkatz
 * CSCdi50441:  EIGRP autosummaries lost after manual summary config
 * Branch: California_branch
 * Make sure that we put back autosummarized interfaces when manual
 * summaries are deleted.
 *
 * Revision 3.14.10.2  1996/04/02  07:15:27  dkatz
 * CSCdi51920:  EIGRP packets miscounted
 * Branch: California_branch
 * Clean up protocol-specific packet counters.
 *
 * Revision 3.14.10.1  1996/04/02  05:37:10  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.14  1996/02/27  04:37:48  mikel
 * CSCdi49954:  EIGRP: modulatiry violation
 *
 * Revision 3.13  1996/02/23  20:41:46  mikel
 * CSCdi48057:  EIGRP: No discovery of neighb on fddi when bridge-gr
 * defined
 * High-end FDDI interfaces were not adding the eigrp multicast to
 * the software multicast filter during a non-reset change.  Insure
 * the smf adds and deletes the eigrp multicast properly.
 *
 * Revision 3.12  1996/02/01  06:05:31  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.11  1996/01/30  09:11:35  irfan
 * CSCdi47184:  EIGRP hellos go out on non-routing async interfaces
 * don't send hellos on or accept packets from static_routing interfaces.
 *
 * Revision 3.10  1996/01/24  20:16:38  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.9  1996/01/22  06:14:19  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.8  1996/01/12  17:55:04  albright
 * CSCdi44810:  eigrp wakeup timers ignored by new scheduler
 *
 * Revision 3.7  1996/01/11  02:29:11  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . merged registry functions ip_tuba_best_local_address and
 *     ip_choose_saddr to function ip_best_local_address().
 *
 * Revision 3.6  1996/01/09  01:31:47  mikel
 * CSCdi44677:  EIGRP static redistribute does not work
 * prevent day one bug where eigrp advertised a NULL routerid, allowing
 * extdata to be dropped
 *
 * Revision 3.5  1996/01/08  19:23:05  mikel
 * CSCdi46290:  EIGRP: topology table picks wrong FD
 * summary no longer tells dual about worst-metric summaries (regression)
 *
 * Revision 3.4  1995/12/07  22:29:44  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/22  18:10:02  albright
 * CSCdi27454:  IP-EIGRP: redistribute connected may be mishandled during
 * IF flap
 *
 * Revision 3.2  1995/11/17  17:34:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.20  1995/10/17  19:48:54  kleung
 * CSCdi42308:  Clean up code - use right comparisons.
 *
 * Revision 2.19  1995/10/17  07:45:47  fox
 * CSCdi42289:  ipigrp2.c fails to compile
 * Missed a brace, the first time....
 *
 * Revision 2.18  1995/10/17  07:20:03  fox
 * CSCdi42289:  ipigrp2.c fails to compile
 * Cleanup mistakes in commit of CSCdi42168.
 *
 * Revision 2.17  1995/10/17  00:48:04  kleung
 * CSCdi42168:  Router crash in dual_updatetopology() running AURP2 tests.
 *
 * Revision 2.16  1995/10/05  02:09:24  mikel
 * CSCdi40200:  EIGRP: incorrect redistributed routes in topology table
 * check to see an existing drdb already exists in the topology table.  If
 * so,  don't add overwrite the existing drdb.  This also eliminates
 * continuous updates seem when mutually redistributing
 * between different EIGRP processes
 *
 * Revision 2.15  1995/09/29  14:45:12  albright
 * CSCdi34629:  Variance and traffic share on eigrp not working
 *
 * Also included: Unequal cost paths are, in some cases, being
 * incorrectly promoted in the eigrp (dual) topology table.
 *
 * Revision 2.14  1995/09/25  08:31:01  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.13  1995/09/23  21:40:40  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Reference renamed fields.  Clean up various interfaces to the IP
 * forwarding table to support protocol-specific functions.
 *
 * Revision 2.12  1995/09/14  17:27:45  albright
 * CSCdi31964:  bus error at PC 0x27377e [dual_conn_rtchange]
 *
 * Revision 2.11  1995/09/07  01:45:44  mikel
 * CSCdi38859:  When serial encap changes, eigrp hold and hello times dont
 * change
 * Make sure to change the eigrp hold and hello timers to the proper NBMA
 * or non-NBMA defaults if we have a encap change.  This change only takes
 * place if have the default timers set
 *
 * Revision 2.10  1995/08/24  23:37:47  bcole
 * CSCdi23668:  IP-EIGRP: show ip proto output missing information
 *
 * Put back much of the output for 'show ip proto' that was lost way
 * back when this command was modularized.
 *
 * Revision 2.9  1995/08/21  20:16:04  mikel
 * CSCdi37985:  EIGRP: mishandled summary dependancies for secondaries
 * Make sure to enqueue necessary summary processing events.  Slight
 * cleanup of summary debug output
 *
 * Revision 2.8  1995/08/04  03:07:52  dstine
 * CSCdi37681:  Need event logging for EIGRP
 *
 * Revision 2.7  1995/08/03  07:42:26  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.6  1995/06/19 06:43:56  bcole
 * CSCdi36032: IP-EIGRP: Connected route for shutdown passive-if stays in
 * topo tbl
 *
 * Revert CSCdi34952 changes.  During an if flap, consider
 * passive-interfaces before deciding that there must be no topology table
 * entries which need to be removed.
 *
 * Revision 2.5  1995/06/19  01:07:42  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/13  18:05:12  mikel
 * CSCdi35696:  EIGRP: no ip address causes random corruption
 * insure iidb isn't prematuraly freed up
 *
 * Revision 2.3  1995/06/13  11:09:04  albright
 * CSCdi34952:  EIGRP passive interfacesubnet are not removed from
 * topology DB
 *
 * Revision 2.2  1995/06/12  22:04:01  mikel
 * CSCdi35363:  EIGRP: ifcost_enq leaves connected entries in topology
 * table
 * Connected routes aren't properly cleaned up from the topology table
 *
 * Revision 2.1  1995/06/07  21:07:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "media_registry.h"
#include "mgd_timers.h"
#include "packet.h"
#include "address.h"
#include "interface_private.h"
#include "subsys.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "config.h"
#include "../if/network.h"
#include "../os/chunk.h"
#include "dual.h"
#include "route.h"
#include "ipigrp2.h"
#include "route_inlines.h"
#include "iprouting_debug.h"
#include "igrp2-inlines.h"
#include "gdp.h"
#include "logger.h"
#include "route_map.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_router.h"
#include "parser_defs_ipigrp2.h"
#include "parser_defs_igrp2.h"
#include "../ip/ip_registry.h"
#include "eigrp_registry.h"
#include "sys_registry.h"
#include "msg_igrp2.c"			/* Not a typo, see logger.h */
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */

/*
 * Storage
 */
static char ipigrp2_abuffer[128];
static char ipigrp2_nbuffer[128];
static ipigrp2_traffic_t ipigrp2_traffic;

/*
 * Parse chains for IP EIGRP clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_ip_igrp2.h"
LINK_POINT(ipigrp2_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP EIGRP show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_ip_eigrp.h"
LINK_POINT(ipigrp2_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP EIGRP debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_ip_eigrp.h"
LINK_POINT(ipigrp2_debug_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for IP EIGRP test commands
 */
#define	ALTERNATE	NONE
#include "exec_test_ip_eigrp.h"
LINK_POINT(ipigrp2_test_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for IP EIGRP interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_ip_eigrp.h"
LINK_POINT(ipigrp2_interface_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for IP EIGRP router timer subcommands
 */
#define	ALTERNATE	NONE
#include "cfg_router_timers_ipigrp2.h"
LINK_POINT(ipigrp2_router_timer_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for IP EIGRP router distance subcommands
 */
#define	ALTERNATE	NONE
#include "cfg_router_distance_eigrp.h"
LINK_POINT(ipigrp2_router_distance_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for IP EIGRP split horizon commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_ip_eigrp_split_horizon.h"
LINK_POINT(ipigrp2_split_horizon_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for IP EIGRP specific router subcommands
 */
#define ALTERNATE NONE
#include "cfg_router_igrp2.h"
LINK_POINT(ipigrp2_router_commands, ALTERNATE);
#undef  ALTERNATE

static parser_extension_request ipigrp2_chain_init_table[] = {
    { PARSE_ADD_CLEAR_IP_CMD, &pname(ipigrp2_clear_commands) },
    { PARSE_ADD_SHOW_IP_CMD, &pname(ipigrp2_show_commands) },
    { PARSE_ADD_DEBUG_IP_CMD, &pname(ipigrp2_debug_commands) },
    { PARSE_ADD_TEST_IP_CMD, &pname(ipigrp2_test_commands) },
    { PARSE_ADD_CFG_INT_IP_CMD, &pname(ipigrp2_interface_commands) },
    { PARSE_ADD_CFG_ROUTER_TIMER_CMD, &pname(ipigrp2_router_timer_commands) },
    { PARSE_ADD_CFG_ROUTER_DISTANCE_CMD,
	  &pname(ipigrp2_router_distance_commands) },
    { PARSE_ADD_CFG_INT_IP_SPLIT_CMD, &pname(ipigrp2_split_horizon_commands) },
    { PARSE_ADD_CFG_ROUTER_CMD, &pname(ipigrp2_router_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * ipigrp2_gettag
 * return the admin tag entry for a given route descriptor block
 *
 */
static ulong ipigrp2_gettag (dual_rdbtype *drdb)
{
    ipigrp2_extdatatype *extdata;

    if (drdb && drdb->drdb_data) {
        extdata = drdb->drdb_data;
      return(extdata->tag);
    } else {
      return(0L);
    }
}

/*
 * ipigrp2_set_routerid
 *
 * Set the router ID if it's not yet set.  The vagaries of initialization
 * order require us to call this at each configuration path.
 */
static void ipigrp2_set_routerid (ddbtype *ddb)
{
    if (ddb->routerid.ip_address == IPADDR_ZERO) {
	ddb->routerid.ip_address = allocate_rtr_id(FALSE);
	if (ddb->routerid.ip_address == IPADDR_ZERO)
	    ddb->routerid.ip_address = ip_best_local_address(0L, TRUE);
    }
}

/*
 * ipigrp2_qevent

 * Enqueue work queue event.
 */
static void ipigrp2_qevent (pdbtype *pdb, ipigrp2_worktype *work,
			    igrp2_eventtype type)
{
    work->type = type;
    process_enqueue(pdb->workQ, work);
}

/*
 * ipigrp2_callback_sanity
 * Make sure we really want to process this callback event.
 */

static inline boolean ipigrp2_callback_sanity (pdbtype *pdb)
{
    if (!pdb->running)
	return(FALSE);
    /*
     * We are shutting down.  Ignore requests.
     */
    if (pdb->shutdown) {
	return(FALSE);
    }
    if (!pdb->ddb)
	return(FALSE);
    return(TRUE);
}

/*
 * ipigrp2_find_pdb
 * Given an IP packet, return the IP-EIGRP pdb or NULL.
 */

static pdbtype *ipigrp2_find_pdb (ulong asystem)
{
    pdbtype *pdb;

    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->asystem != asystem)
	    continue;
	if (pdb->proctype & PROC_IGRP2)
	    return(pdb);
    }
    return(NULL);
}

/*
 * find_igrpsocket
 * Given an IP packet, return the IGRP socket or NULL
 */

static ipsocktype *find_igrp2socket (int asystem)
{
    ipsocktype *soc;

    for (soc = IPsocketQ.qhead; soc; soc = soc->next) {
	if ((soc->protocol == IPIGRP2_PROT) && (asystem == soc->lport))
	    return(soc);
    }
    return(NULL);
}

/*
 * ipigrp2_process
 * Here we figure out which IGRP2 socket gets the datagram.
 */
static void ipigrp2_process (paktype *pak)
{
    ipsocktype *soc;
    igrp2type *igrp;
    ulong asystem;
    pdbtype *pdb;
    iphdrtype *ip;

    igrp = (igrp2type *)ipdatastart(pak);
    asystem = GETLONG(&igrp->asystem);
    soc = find_igrp2socket(asystem);
    if (soc && (soc->inallocated != soc->inallowed)) {
	ip = iphdrtype_start(pak);
	pdb = ipigrp2_find_pdb(asystem);
	if (pdb) {
	    soc->inallocated++;
 	    enqueue_ip_socket(soc, pak);
  	} else {
  	    retbuffer(pak);
  	}
    } else {
	reg_invoke_ip_gdpclient_enqueue(pak);
    }
}

/*
 * ipigrp2_mmetric_fudge
 * If offset-list is specified, fixup the delay factor.
 */
static ulong ipigrp2_mmetric_fudge (ddbtype *ddb, igrp2nettype *network,
 			     idbtype *idb, int direction)
{
    return(offset_metric(ddb->pdb, idb, network->ip_network.address, 
			 direction));
}

/*
 * ipigrp2_addrextract
 * Convert address/mask pair from packet into internal format.  Return
 * number of significant octets.
 */
static int ipigrp2_addrextract (ipigrp2_mpdecode *ipd, igrp2nettype *am)
{
    int length;
    
    length = (ipd->mask + 7) / 8;
    if (length > sizeof(ipaddrtype))
	return(-1);
    am->ip_network.address = 0L;
    if (ipd->mask) {
	bcopy(&ipd->number[0], &am->ip_network.address, length);
	am->ip_network.mask = 0xffffffff << (32 - ipd->mask);
    } else {
	am->ip_network.mask = 0;
	return(1);
    }
    return(length);
}

/*
 * ipigrp2_exteriors_out
 * Given an entry from the topology table flagged as a candidate default,
 * decide if we should advertise it as a candidate default or not.  Return
 * TRUE if route should be advertised as a candidate default, FALSE otherwise.
 */

static inline boolean ipigrp2_exteriors_out (pdbtype *pdb, ipaddrtype address)
{
    /*
     * Exteriors unconditionally disallowed
     */
    if (!pdb->exteriors_out)
	return(FALSE);
    /*
     * Exteriors unconditionally allowed
     */
    if (!pdb->exteriors_out_acl)
	return(TRUE);
    /*
     * Exteriors allowed if they match access list, otherwise disallowed
     */
    return(nfast_check(pdb->exteriors_out_acl, address));
}

/*
 * ipigrp2_exteriors_in
 * Given an route from a neighbor flagged as a candidate default,
 * decide if we should clear the candidate default flag or not.  Return
 * TRUE if flag should be honored, FALSE otherwise.
 */

static inline boolean ipigrp2_exteriors_in (pdbtype *pdb, ipaddrtype address)
{
    /*
     * Exteriors unconditionally disallowed
     */
    if (!pdb->exteriors_in)
	return(FALSE);
    /*
     * Exteriors unconditionally allowed
     */
    if (!pdb->exteriors_in_acl)
	return(TRUE);
    /*
     * Exteriors allowed if they match access list, otherwise disallowed
     */
    return(nfast_check(pdb->exteriors_in_acl, address));
}

/*
 * ipigrp2_printnet
 *
 * Return an IP address mask string in ASCII.
 */
static char *ipigrp2_printnet (igrp2nettype *addr)
{
    sprintf(ipigrp2_nbuffer, "%i%m", addr->ip_network.address, 
	    addr->ip_network.mask);
    return(ipigrp2_nbuffer);
}

/*
 * ipigrp2_process_routes
 * Grab metric, compute composite metric.  Then get destinations/masks
 * from packet and fire off call to dual.  Return FALSE if we run out of
 * memory, or the packet has bogus data.
 */
static boolean ipigrp2_process_routes (ipigrp2_mpdecode *ipd,
				       uchar *limit,
				       dual_newroute *route,
				       igrp2_mdecode *metric,
				       ddbtype *ddb,
				       ulong pkt_type,
				       boolean ignore_tlv,
				       ipaddrtype address,
				       dual_peertype *peer,
				       void *extd)
{
    pdbtype *pdb;
    int length;
    ipigrp2_extdatatype *data;
    ushort route_type;
    acl_headertype *acl;
    ipaddrtype source;
    idbtype *idb;

    idb = dual_idb(route->new_iidb);
    if (!idb) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return(FALSE);
    }

    pdb = ddb->pdb;
    source = peer->source.ip_address;
    igrp2_metric_decode(&route->new_vecmetric, metric);
    while (ipd < (ipigrp2_mpdecode *)limit) {
	route->new_ignore = ignore_tlv;
	route->new_nexthop.ip_address = (address == 0) ? source : address;
	length = ipigrp2_addrextract(ipd, &route->new_dest);
	if (length == -1)
	    return(FALSE);
	route->new_metric = igrp2_compute_metric(ddb, &route->new_vecmetric,
						 &route->new_dest,
						 route->new_iidb,
						 &route->new_successor_metric);

	route->new_opaque_flags = 0;

	/* Copy the external data for external destinations. */

	if (route->new_external) {
	    data = chunk_malloc(ddb->extdata_chunk);
	    if (!data)
		return(FALSE);
	    bcopy(extd, data, sizeof(ipigrp2_extdatatype));

	    /*
	     * If route is marked as a candidate default, consider
	     * performing input filtering on it.
	     */
	    if (data->flag & IGRP2_CD) {
		if (!ipigrp2_exteriors_in(pdb,
				  route->new_dest.ip_network.address)) {
		    data->flag &= ~IGRP2_CD;
		}
	    }
	    route->new_data = data;

	} else {			/* Not external */

	    route->new_data = NULL;

	    if (peer->peer_version.ios_majorver) {
		/*
		 * We can't trust the flag bit information from older routers.
		 * The flag bit information was carved out of a reserved
		 * field which older routers neglected to initialize to zero.
		 * So only trust the flag bits if we know that our neighbor
		 * is running new enough code that it can tell us what software
		 * version it is running.
		 */
		route->new_opaque_flags = IPIGRP2_METRICTYPEFLAGS(metric);
		/*
		 * If route is marked as a candidate default, consider
		 * performing input filtering on it.
		 */
		if (route->new_opaque_flags & IGRP2_CD) {
		    if (!ipigrp2_exteriors_in(pdb,
				     route->new_dest.ip_network.address)) {
			route->new_opaque_flags &= ~IGRP2_CD;
		    }
		}
	    }
	}

	/*
	 * At this point check to see if the ignore flag should be
	 * set in the topology table.  Only consider distance
	 * less than MAXDISTANCE and must pass input filtering.
	 */
	if (!route->new_ignore) {
	    route_type = (route->new_external) ?
		RTYPE_IGRP2_EXT : RTYPE_IGRP2_INT;
	    if (pdb_distance(pdb, route->new_infosource.ip_address,
			     route->new_dest.ip_network.address,
			     route->new_dest.ip_network.mask, 0,
			     route_type) >= MAXDISTANCE) {
		route->new_ignore = TRUE;
	    } else {
		acl = pdb->ifltr_all.acl;
		if (!acl) {
		    acl = idb->ip_iprotofltr[pdb->index];
		}
		if (acl) {
		    if (network_check(acl, route->new_infosource.ip_address,
				      route->new_dest.ip_network.address) ==
			FALSE) {
			route->new_ignore = TRUE;
		    }
		}
	    }
	}

	if (IPIGRP2_DTARGET(route->new_dest.ip_network.address, ddb)) {
	     /* workaround gcc.921225.68k compiler bug using tmp variable */
	    ulong bandwidth = GETLONG(&metric->bandwidth);
	    buginf("\nIP-EIGRP: %s %s M %u - %u %u SM %u - %u %u",
		   (route->new_data) ? "Ext" : "Int",
		   ipigrp2_printnet(&route->new_dest), route->new_metric,
		   route->new_vecmetric.bandwidth, route->new_vecmetric.delay,
		   route->new_successor_metric,
		   bandwidth, GETLONG(&metric->delay));
	}
	dual_rcvpkt(ddb, peer, route, pkt_type);
	ipd = (ipigrp2_mpdecode *) ((uchar *)ipd + length + 1);
    }
    return(TRUE);
}

/*
 * ipigrp2_receive
 * Transform update, query or reply packet into internal data structures.
 */
static void ipigrp2_receive (ddbtype *ddb, dual_peertype *peer,
			     paktype *pak, igrp2type *igrp2)
{
    iphdrtype *ip;
    pdbtype *pdb;
    igrp2_gentlvtype *tlv;
    int bytes;
    uchar *limit;
    ushort length;
    dual_newroute route;
    ipigrp2_externtype *ipe;
    ipigrp2_metrictype *ipm;
    boolean ignore_tlv;

    bzero(&route, sizeof(route));
    ip = (iphdrtype *)ipheadstart(pak);
    pdb = ddb->pdb;
    tlv = igrp2_packet_data(igrp2);
    bytes = ip->tl - ((ip->ihl << 2) + IGRP2HEADERBYTES);
    route.new_infosource = peer->source;
    route.new_iidb = peer->iidb;
    route.new_origin = IGRP2_ORG_IGRP2;

    if (ipigrp2_debug) {
	buginf("\nIP-EIGRP: Processing incoming %s packet", 
	       igrp2_otype(igrp2->opcode));
    }

    while (bytes > 0) {

	/*
	 * Let's inject a little sanity checking.  This is the same case
	 * as default in the switch statement.
	 */
	length = GETSHORT(&tlv->length);
	if (length < sizeof(igrp2_gentlvtype))
	    goto done;

	limit = (uchar *)tlv + length;
	switch (GETSHORT(&tlv->type)) {
	  case IPIGRP2_METRIC:
	    ipm = (ipigrp2_metrictype *) tlv;
	    route.new_external = FALSE;
	    route.new_data = NULL;
	    if (!ipigrp2_process_routes(ipm->int_dest, limit, &route,
					(igrp2_mdecode *)&ipm->delay, ddb,
					igrp2->opcode, FALSE,
					GETLONG(&ipm->nexthop),
					peer, NULL)) {
		goto done;
	    }
	    break;

	  case IPIGRP2_EXTERNAL:
	    route.new_external = TRUE;
	    ipe = (ipigrp2_externtype *) tlv;

	    /*
	     * If we are the source of this advertisement then reject it.
	     */
	    ignore_tlv = (GETLONG(&ipe->routerid) == ddb->routerid.ip_address);

	    if (!ipigrp2_process_routes(ipe->ext_dest, limit, &route,
					(igrp2_mdecode *)&ipe->delay, ddb,
					igrp2->opcode, ignore_tlv,
					GETLONG(&ipe->nexthop),
					peer, &ipe->routerid)) {
		goto done;
	    }
	    break;

	default:

	    /* Skip the unknown TLV. */

	    break;
	}
	bytes -= length;
	tlv = (igrp2_gentlvtype *) ((uchar *)tlv + length);
    }

  done:

    /* Finish up any Updates we felt it necessary to send. */

    dual_finish_update_send(ddb);
}

/*
 * ipigrp2_write
 * Send a datagram.
 *
 * This routine does not free the supplied packet.
 */
static void ipigrp2_write (igrp2type *igrp2, int bytes, dual_peertype *peer,
			   igrp2_idbtype *iidb, boolean priority)
{
    ipaddrtype dest;
    ipaddrtype source;
    paktype *pak;
    idbtype *interface;

    interface = iidb->idb;

    pak = igrp2_getpak(IPHEADERBYTES(NOPT) + bytes);
    if (!pak) {
	return;
    }
    bcopy(igrp2, ipheadstart(pak) + IPHEADERBYTES(NOPT), bytes);
    dest = (peer) ? peer->source.ip_address : ALLIGRPROUTERS;
    source = interface->ip_address;
    if (!source && interface->ip_unnumbered)
	source = interface->ip_unnumbered->ip_address;
    pak->if_output = interface;
    if (priority)
	pak->flags |= PAK_PRIORITY;

    /*
     * Need to rewrite desthost because this might be a unicast transmission
     * and the previous was a multicast transmission. Don't want NBMA nets
     * to replicate packet on the idb output queue.
     */
    pak->desthost = dest;

    ipwrite(pak, bytes + IPHEADERBYTES(NOPT), IPIGRP2_PROT, source, dest, 
	    interface, NULL, &iprouteparams);
}

/*
 * ipigrp2_cleanup
 * Here to cleanup after an IGRP router has been shutdown.
 * Close socket and release pdb storage.
 */

static void ipigrp2_cleanup (pdbtype *pdb)
{
    ddbtype *ddb;

    ddb = pdb->ddb;
    while (!process_is_queue_empty(pdb->workQ))
	chunk_free(ddb->workq_chunk, process_dequeue(pdb->workQ));
    igrp2_free_ddb(ddb);
    age_all_routes(pdb);		/* Final bits of cleanup */
    reg_invoke_ip_close_socket((ipsocktype *)pdb->socket);
    delete_watched_queue(&pdb->workQ);
    unqueue(&protoQ, pdb);
    free_pdbindex(pdb);
    free(pdb);
}

/*
 * ipigrp2_receive_probe
 *
 * Process an incoming probe packet
 */
static void ipigrp2_receive_probe (ddbtype *ddb, dual_peertype *peer,
				   paktype *pak, igrp2type *igrp2)
{
    igrp2_probe_type *probe;
    ulong probe_seq;
    ulong expected_seq;

    /* Pull it apart. */

    probe = igrp2_packet_data(igrp2);
    probe_seq = GETLONG(&probe->probe_sequence);
    expected_seq = peer->rcv_probe_sequence + 1;

    /* If the incoming packet doesn't match the expected seq #, complain. */

    if (probe_seq != expected_seq) {
	errmsg(&msgsym(PROBESEQ, DUAL), probe_seq,
	       (*ddb->printaddr)(&peer->source),
	       expected_seq);
    }
    peer->rcv_probe_sequence = probe_seq;
}

/*
 * ipigrp2_timers_command
 * Sets that maximum allowable time a route may remain in active state before
 * clearing
 */
void ipigrp2_timers_command (parseinfo *csb)
{
    pdbtype *pdb = csb->protocol;
    ddbtype *ddb;

    if (!router_enable)
 	return;
 
    ddb = pdb->ddb;
    if (csb->nvgen) {
	if (ddb->active_time == IGRP2_DEF_ACTIVE_TIME)
	    return;

	nv_write(TRUE, "%s", csb->nv_command);
	if (ddb->active_time != 0)
	    nv_add(TRUE, " %d", ddb->active_time / ONEMIN);
	else
	    nv_add(TRUE, " disabled");
	return;
    }

    if (csb->sense) {
	ddb->active_time = GETOBJ(int,1) * ONEMIN;
	if (GETOBJ(int,1) == 0) {
	    mgd_timer_stop(&ddb->ddb_active_timer);
	} else {
	    mgd_timer_start(&ddb->ddb_active_timer,
			    IGRP2_ACTIVE_SCAN_FREQUENCY);
	}
    } else {
	ddb->active_time = IGRP2_DEF_ACTIVE_TIME;
	mgd_timer_start(&ddb->ddb_active_timer, IGRP2_ACTIVE_SCAN_FREQUENCY);
    }
}	

/*
 * ipigrp2_ager
 * Clear out any entries that have no rdb's.
 */
static boolean ipigrp2_ager (pdbtype *pdb, ndbtype *ndb)
{
    if (ip_own_ndb(ndb, pdb))
	if (ndb->routecount == 0)
	    return(TRUE);
    return(FALSE);
}

/*
 * ipigrp2_rdbupdate
 * Given a routing block, update the IGRP specific data
 */
static void ipigrp2_rdbupdate (rdbtype *rdb, idbtype *interface,
			       dual_rdbtype *drdb)
{
    igrp2metric *metric;

    if (!rdb)
	return;
    metric = &drdb->drdb_vecmetric;
    rdb->rdb_delay = metric->delay;
    rdb->rdb_bandwidth = metric->bandwidth;
    rdb->load = metric->load;
    rdb->reliability = metric->reliability ? metric->reliability :
	interface->hwptr->reliability;
    rdb->rdb_mtu = metric->mtu ? metric->mtu : interface->hwptr->maxmtu;
    rdb->rdb_nexthop_metric = drdb->drdb_successor_metric;
    rdb->hopcount = metric->hopcount;
}

/*
 * ipigrp2_proto_table_add
 * Interface between IP-EIGRP and IP routing table
 */

static rdbtype *ipigrp2_proto_table_add (ddbtype *ddb, dual_ndbtype *dndb,
					 dual_rdbtype *drdb, long flags)
{
    ulong tag;
    ipigrp2_extdatatype *extdata;
    ushort route_type;
    rdbtype *rdb;
    pdbtype *pdb;

    pdb = ddb->pdb;
    extdata = drdb->drdb_data;

    if (extdata) {
	if (extdata->flag & IGRP2_CD) {
	    flags |= NET_DEXTERIOR;
	}
	tag = GETLONG(&extdata->tag);

    } else {
	tag = 0;
    }
    route_type = (drdb->drdb_data) ? RTYPE_IGRP2_EXT : RTYPE_IGRP2_INT;
    pdb->arg1 = drdb->drdb_successor_metric;
    /*
     * Network update may suspend on us, so preserve ddb metric information
     */
    rdb = network_update(pdb, dndb->dndb_dest.ip_network.address,
			 dndb->dndb_dest.ip_network.mask,
			 drdb->drdb_nexthop.ip_address,
			 dual_idb(drdb->drdb_iidb), drdb->drdb_metric, flags,
			 drdb->drdb_infosource.ip_address, tag, NULL,
			 route_type);
    if (rdb) {
	ipigrp2_rdbupdate(rdb, dual_idb(drdb->drdb_iidb), drdb);
    }
    return(rdb);
}

/*
 * ipigrp2_summary_revise
 *
 * Attempt to update a summary metric.  Returns instructions as to
 * whether updating the summary in the topo table is necessary, and
 * if so whether the metric should be found by searching the topo
 * table.
 */

static summary_revise_result ipigrp2_summary_revise (ddbtype *ddb,
						     summarytype *sum,
						     boolean added,
						     ulong metric)
{
    summary_revise_result result;

    if (ipigrp2_debug_sum) {
	buginf("\nIP-EIGRP: summary_revise: %i%m %d %u",
	       sum->address, sum->mask, added, metric);
    }
    dual_log_all(ddb, DUALEV_SUMREV, &sum->address, &sum->mask);
    dual_log_all(ddb, DUALEV_SUMREV2, &added, &metric);

    if (added) {			/* Added the route. */

	/*
	 * Route is being added.  If the new route has a better metric than the
	 * old, update the summary with the new metric.  If the new route has
	 * a worse metric than the old, update the summary by searching for
	 * the route with the best metric (since this worse route might have
	 * been the best one just a moment ago).  If the metrics are equal,
	 * we're safe and do nothing.
	 */
	if (metric < sum->min_metric) {	/* We're the best */
	    result = SUMMARY_UPDATE_METRIC;
	    sum->min_metric = metric;
	} else if (metric > sum->min_metric) { /* Something else was better */
	    result = SUMMARY_FIND_METRIC;
	} else {			/* Metric is the same */
	    result = SUMMARY_NO_CHANGE;
	}

    } else {				/* Deleted the route */

	/*
	 * Deleting the route.  If the metric of the route being deleted
	 * is worse than the best metric, we're all set.  Otherwise (the
	 * metrics are equal) search for a new metric.
	 */
	if (metric > sum->min_metric) {
	    result = SUMMARY_NO_CHANGE;
	} else {
	    result = SUMMARY_FIND_METRIC;
	}
    }
    if (ipigrp2_debug_sum)
	buginf("\nIP-EIGRP: summary_revise: Metric now %u, result %u",
	       metric, result);
    dual_log_all(ddb, DUALEV_SUMREV3, &sum->min_metric, &result);
    return(result);
}

/*
 * ipigrp2_process_summary
 *
 * Add or delete summary address from topology table.
 *
 * The supplied metric is used if adding the summary.  If no metric is
 * supplied (a NULL pointer) an added summary will be assigned the
 * appropriate minima.
 *
 * add is FALSE if the summary is being deleted, TRUE if it is being added
 * or updated.
 */
static void ipigrp2_process_summary (pdbtype *pdb, summarytype *summary,
				     igrp2metric *metric, boolean add)
{
    ddbtype *ddb;
    ipigrp2_worktype *work;

    if (ipigrp2_debug_sum) {
	buginf("\nIP-EIGRP: process_summary: %i%m %d",
	       summary->address, summary->mask, add);
    }
    if (!ipigrp2_callback_sanity(pdb))
	return;

    ddb = pdb->ddb;
    work = chunk_malloc(ddb->workq_chunk);
    if (work == NULL) {
	return;
    }

    work->c.sum.dest.address = summary->address;
    work->c.sum.dest.mask = summary->mask;
    work->c.sum.add = add;
    if (metric) {
	work->c.sum.metric = malloc(sizeof(igrp2metric));
	if (work->c.sum.metric == NULL) {
	    chunk_free(ddb->workq_chunk, work);
	    return;
	}
	*work->c.sum.metric = *metric;
    } else {
	work->c.sum.metric = NULL;
    }
    ipigrp2_qevent(pdb, work, IGRP2_WORK_SUMMARY);
}

/*
 * ipigrp2_summary_depend
 * 
 * Process summary based on a more specific route coming or going.
 *
 * added = TRUE if the route itself is going up, or FALSE if it is going down.
 */
static void ipigrp2_summary_depend (pdbtype *pdb,
				    ipaddrtype address,
				    ipaddrtype mask,
				    igrp2metric *vecmetric,
				    ulong metric,
				    boolean added)
{
    summarytype *sum;
    ddbtype *ddb;
    summary_revise_result result;

    ddb = pdb->ddb;

    /*
     * Update all summaries which depend upon this route.
     */
    for (sum = pdb->summaryQ.qhead; sum; sum = sum->next) {

	/*
	 * The summary itself changed. Only deal with non-summary route
	 * change.
	 */
	if ((sum->address == (address & mask)) && (sum->mask == mask))
	    continue;
	if (sum->address != (address & sum->mask))
	    continue;
	if (ipigrp2_debug_sum)
	    buginf("\nIP-EIGRP: summary_depend: %i%m %d %u",
		   address, mask, added, metric);
	result = ipigrp2_summary_revise(ddb, sum, added, metric);
	switch (result) {

	  case SUMMARY_UPDATE_METRIC:
	    ipigrp2_process_summary(pdb, sum, vecmetric, TRUE);
	    break;

	  case SUMMARY_FIND_METRIC:
	    if (!sum->being_recalculated) {
		dual_log_all(ddb, DUALEV_SUMDEP, NULL, NULL);
		ipigrp2_process_summary(pdb, sum, NULL, TRUE);
		sum->being_recalculated = TRUE;
	    }
	    break;

	  case SUMMARY_NO_CHANGE:
	    break;
	}
    }
}

/*
 * ipigrp2_rtupdate
 * Attempt to install a route specified by dual.
 */
static boolean ipigrp2_rtupdate (ddbtype *ddb, dual_ndbtype *dndb,
				 dual_rdbtype *drdb, boolean *promote)
{
    pdbtype *pdb;
    ndbtype *ndb;
    rdbtype *rdb;
    rdbtype *temp_rdb;
    ipaddrtype addr;
    ipaddrtype mask;
    ulong metric;
    int count, i;
    long flags;

    *promote = FALSE;
    pdb = ddb->pdb;
    addr = dndb->dndb_dest.ip_network.address;
    mask = dndb->dndb_dest.ip_network.mask;
    if (drdb->drdb_origin == IGRP2_ORG_SUMMARY) {
	flags = NET_SUMMARY;
    } else {
	flags = 0;
    }

    ndb = ip_rtlookup(addr, mask);
    if (ndb == NULL) {
	metric = METRIC_INACCESSIBLE;
    } else {
	metric = ndb->metric;
    }
	    
    rdb = ipigrp2_proto_table_add(ddb, dndb, drdb, flags);
    if (rdb == NULL) {
	return(FALSE);
    }

    /*
     * Successful update to the routing table.  Set the successors
     * field in the ddb correctly.
     */
    if ((ndb == NULL) || (ndb->metric < metric)) {
	
	/*
	 * Only one best cost successor (so far...)
	 */
	dndb->dndb_successors = 1;
	*promote = TRUE;
    } else {

	/*
	 * No easy way to glean it in this case.  Use the brute force
	 * method looking for number of best cost successors learned
	 * from this instance if igrp2.
	 */
	count = 0;
	for (i = 0; i < ndb->routecount; i++) {
	    temp_rdb = ndb->rdb[i];
	    if ((temp_rdb->metric == ndb->metric) &&
		(temp_rdb->pdbindex == pdb->index)) {
		count++;
	    }
	}
	dndb->dndb_successors = count;
	*promote = (ndb->metric == rdb->metric);
    }

    /*
     * Check if this route affects a summary.
     */
    if (*promote && (drdb->drdb_origin != IGRP2_ORG_SUMMARY)) {
	ipigrp2_summary_depend(pdb, addr, mask, &drdb->drdb_vecmetric, 
			       drdb->drdb_metric, TRUE);
    }
    return(TRUE);
}

/*
 * ipigrp2_addrmatch
 * Perform IP address match.
 */
static boolean ipigrp2_addrmatch (igrp2addrtype *addr1, igrp2addrtype *addr2)
{
    return(addr1->ip_address == addr2->ip_address);
}

/*
 * ipigrp2_printaddr
 *
 * Return an IP address in ASCII.
 */
static char *ipigrp2_printaddr (igrp2addrtype *addr)
{
    sprintf(ipigrp2_abuffer, "%i", addr->ip_address);
    return(ipigrp2_abuffer);
}

/*
 * ipigrp2_netmatch
 * Perform IP network address and mask match.
 */
static boolean ipigrp2_netmatch (igrp2nettype *net1, igrp2nettype *net2)
{
    return((net1->ip_network.address == net2->ip_network.address) && 
	   (net1->ip_network.mask == net2->ip_network.mask));
}

/*
 * ipigrp2_addrcopy
 *
 * Copy an IP address from source to dest. First byte specifies length
 * of address. Length returned is (length of address + 1).
 */
static ulong ipigrp2_addrcopy (igrp2addrtype *source, uchar *dest)
{
    
    *dest++ = sizeof(ipaddrtype);
    PUTLONG(dest, source->ip_address);
    return(sizeof(ipaddrtype) + 1);
}

/*
 * ipigrp2_localaddr
 *
 * Check to see if supplied address is one of the system's addresses.
 */
static boolean ipigrp2_localaddr (ddbtype *ddb, igrp2addrtype *address,
				  idbtype *idb)
{

    return(ip_ifaddress(idb, GETLONG(&address->ip_address)));
}

/*
 * ipigrp2_rtdelete
 * Delete an entry from the routing table only if it belongs to us.
 */
static void ipigrp2_rtdelete (ddbtype *ddb, dual_ndbtype *dndb,
			      dual_rdbtype *drdb)
{
    pdbtype *pdb;
    ndbtype *ndb;
    rdbtype *rdb;
    int i;
    ulong metric_was;

    pdb = ddb->pdb;

    /*
     * First lookup entry in routing table.
     */
    ndb = ip_rtlookup(dndb->dndb_dest.ip_network.address,
		      dndb->dndb_dest.ip_network.mask);
    if (!ndb || ndb->attrflags & NET_ATTACHED || !ip_own_ndb(ndb, pdb)) {
	return;
    }

    for (i = 0; i < ndb->routecount; i++) {
	rdb = ndb->rdb[i];
	if ((rdb->gateway == drdb->drdb_nexthop.ip_address) &&
	    (rdb->idb == dual_idb(drdb->drdb_iidb) || rdb->idb == NULL)) {
	    metric_was = rdb->metric;
	    rdb_delete(pdb, ndb, i);

	    /*
	     * If the last rdb has been deleted, we need to remove
	     * the ndb.  Perhaps calling netarray_ager() is the right
	     * thing to do.  Perhaps calling ip_route_delete()?
	     * Nope.  Age after a delay.
	     */
	    if (ndb->routecount == 0) {
		if (!mgd_timer_running(&ddb->ddb_pdm_timer)) {
		    mgd_timer_start(&ddb->ddb_pdm_timer, IPIGRP2_AGE_DELAY);
		}
		if (drdb->drdb_origin != IGRP2_ORG_SUMMARY)
		    ipigrp2_summary_depend(pdb, ndb->netnumber, ndb->netsmask,
					   NULL, metric_was, FALSE);
	    }
	    break;
	}
    }
}

/*
 * ipigrp2_holddown
 * Let the caller know if this rdb should be deleted.
 */
static boolean ipigrp2_holddown (pdbtype *pdb, ndbtype *ndb, rdbtype *rdb, 
				 ulong newmetric)
{
    /*
     * If we are updating an existing path with a worse metric, then it
     * doesn't matter if the route is considered feasible or not.  Accept
     * it regardless.  This allows EIGRP to cause a metric to get
     * arbitrarily worse without making the route go into holddown.
     */
    if (ndb->routecount == 1 && rdb && newmetric != METRIC_INACCESSIBLE &&
	(newmetric > ndb->metric))
	return(FALSE);
    /*
     * If we have new information, or this information is about a route
     * that is not optimal, then perform a feasible successor check.
     * If it's a feasible successor, return FALSE.  It's feasible if:
     * - our neighbor's metric is better than our best metric, and
     * - our new metric is less than the variance times the best metric
     */ 
    if (!rdb || rdb->metric != ndb->metric) {
	if ((pdb->arg1 < ndb->metric)
	    && (newmetric < pdb->igrp_variance * ndb->metric)) {
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * ipigrp2_rtgetmetric
 * Get best metric from routing table for this destination.  If the
 * destination doesn't exist or the metrics aren't comparable then
 * return inaccessible.
 */
static ulong ipigrp2_rtgetmetric (igrp2nettype *dest, ddbtype *ddb)
{
    ndbtype *ndb;

    ndb = ip_rtlookup(dest->ip_network.address, dest->ip_network.mask);
    if (ndb && (ndb->distance == ((pdbtype *)ddb->pdb)->distance))
	return(ndb->metric);
    return(METRIC_INACCESSIBLE);
}

/*
 * ipigrp2_peerbucket
 * Return slot based on IP address.
 */
static int ipigrp2_peerbucket (igrp2addrtype *address)
{
    return(nethash(address->ip_address));
}

/*
 * ipigrp2_ndbbucket
 * Return slot based on IP address.
 */
static int ipigrp2_ndbbucket (igrp2nettype *address)
{
    return(nethash(address->ip_network.address));
}

/*
 * ipigrp2_headerptr
 *
 * Return pointer to beginning of IGRP2 fixed length header.
 */
static igrp2type *ipigrp2_headerptr (paktype *pak)
{
    iphdrtype *ip;
    uchar *datastart;

    ip = (iphdrtype *)ipheadstart(pak);
 
    if (ip->ihl) {
	/*
	 * Packet length is already known.  Packet must either be incoming,
	 * or has already been through an ipwrite attempt.
	 */
	datastart = ipdatastart(pak);
    } else {
	datastart = ipheadstart(pak) + IPHEADERBYTES(NOPT);
    }
    return((igrp2type *)datastart);
}

/*
 * ipigrp2_paklen
 *
 * Return length of IGRP2 packet minus the IP header length.
 */
static ulong ipigrp2_paklen (paktype *pak)
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    return(ip->tl - (ip->ihl << 2));
}

/*
 * ipigrp2_conn_rtchange
 *
 * Enqueue a connected route change event.
 */
static void ipigrp2_conn_rtchange (pdbtype *pdb, idbtype *interface,
				  ipaddrtype address, ipaddrtype mask,
				  boolean sense, boolean config)
{
    ipigrp2_worktype *work;

    if (!ipigrp2_callback_sanity(pdb))
	return;

    work = chunk_malloc(pdb->ddb->workq_chunk);
    if (work == NULL) {
	return;
    }

    work->c.con.interface = interface;
    work->c.con.dest.address = address & mask;
    work->c.con.dest.mask = mask;
    work->c.con.up = sense;
    work->c.con.config = config;
    ipigrp2_qevent(pdb, work, IGRP2_WORK_CONNSTATE);
}

/*
 * ipigrp2_is_exterior
 * Is IP routing table entry a candidate default?
 * Candidate default bit may be set either in a dummy ndb or in a majornet
 * route (or both).  Check everywhere.
 */
static boolean ipigrp2_is_exterior (ipaddrtype addr, ipaddrtype mask)
{
    ndbtype *ndb;
    boolean exterior;

    if (get_majormask(addr) == mask) {
	/*
	 * Consider a dummy ndb
	 */
	ndb = net_lookup(addr, FALSE);
	if (ndb) {
	    exterior = (ndb->attrflags & NET_EXTERIOR);
	    if ((ndb->attrflags & NET_SUBNETTED) == 0)
		return(exterior);
	    if (exterior)
		return(exterior);
	    /*
	     * Real majornet route may also exist and need to be considered.
	     */
	}
    }
    ndb = ip_rtlookup(addr, mask);
    if (!ndb)
	return(FALSE);
    exterior = (ndb->attrflags & NET_EXTERIOR);
    return(exterior);
}

static const char ipigrp2_nei_banner[] = {"\nIP-EIGRP %s for process %d"};

/*
 * ipigrp2_show_neighbors
 *
 * Process "show ip eigrp {neighbors|interface} <asystem> [<interface><unit>]
 */
void ipigrp2_show_neighbors (parseinfo *csb)
{
    ddbtype *ddb;
    pdbtype *pdb;
    int     asystem;

    asystem = GETOBJ(int,1);
    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (!(ddb = pdb->ddb))
	    continue;
	if (!pdb->running)
	    continue;
	if (pdb->asystem == GETOBJ(int,1) || !asystem) {
	    switch (GETOBJ(int,3)) {
	      case EIGRP_SHOW_NEIGHBORS:
		printf(ipigrp2_nei_banner, "neighbors", pdb->asystem);
		igrp2_show_neighbors(ddb, GETOBJ(idb,1), FALSE, GETOBJ(int,2));
		break;

	      case EIGRP_SHOW_INTERFACE:
		printf(ipigrp2_nei_banner, "interfaces", pdb->asystem);
		igrp2_show_interface(ddb, GETOBJ(idb,1), GETOBJ(int,2));
		break;
	    }

	    if (asystem)
		break;
	}
    }
}

/*
 * ipigrp2_protid_string
 *
 * Look at IP-IGRP2 external protocol id and return string for
 * printing.
 */
static char *ipigrp2_protid_string (int prot_id)
{

    char *str;

    switch (prot_id) {
    case IGRP1_PROTID:
	str = "IGRP";
	break;
    case IGRP2_PROTID:
	str = "EIGRP";
	break;
    case STATIC_PROTID:
	str = "Static";
	break;
    case CONNECTED_PROTID:
	str = "Connected";
	break;
    case RIP_PROTID:
	str = "RIP";
	break;
    case HELLO_PROTID:
	str = "Hello";
	break;
    case OSPF_PROTID:
	str = "OSPF";
	break;
    case ISIS_PROTID:
	str = "IS-IS";
	break; 
    case EGP_PROTID:
	str = "EGP";
	break;
    case BGP_PROTID:
	str = "BGP";
	break;
    case IDRP_PROTID:
	str = "IDRP";
	break;
    default:
    case NULL_PROTID:
	str = "Unknown protocol";
	break;
    }
    return(str);
}

/*
 * ipigrp2_show_topology_route
 */
static void ipigrp2_show_topology_route (ddbtype *ddb, ipaddrtype address, 
					 ipaddrtype mask)
{
    dual_ndbtype *dndb = NULL;
    dual_rdbtype *drdb = NULL;
    dual_rdbtype *drdb_next = NULL;
    ipigrp2_extdatatype *ext_info;
    igrp2nettype dest;
    boolean active;
    boolean connected, redistributed;

    if (ddb == NULL)
	return;
    dest.ip_network.address = address;
    dest.ip_network.mask = (mask) ? mask : get_majormask(address);

    dndb = dual_ndblookup(ddb, &dest);
    if (dndb == NULL) {
	printf("\n% Route not in topology table");
	return;
    }
    /*
     * Lock both the ddb and the neighbor descriptor block for the rest
     * of this function. This will guarantee that we can make the 
     * through the printing of the route chain without someone
     * freeing the root of the route chain out from beneath us.
     */
    mem_lock(ddb);
    mem_lock(dndb);
    active = dual_dndb_active(dndb);
    printf("\nIP-EIGRP topology entry for %s",
	   ipigrp2_printnet(&dest));
    printf("\n  State is %s, Query origin flag is %d, %d Successor(s)",
	   active ? "Active" : "Passive", dndb->query_origin,
	   dndb->dndb_successors);
    printf(", FD is %u", dndb->dndb_old_metric);
    if (active)
	printf("\nWaiting for %d replies",
	       dndb->dndb_reply_status.handles_used);
    printf("\n  Routing Descriptor Blocks:");

    drdb = dndb->dndb_rdb;
    while (drdb != NULL) {
	mem_lock(drdb);
	redistributed = (drdb->drdb_origin == IGRP2_ORG_REDISTRIBUTED);
	connected = (drdb->drdb_origin == IGRP2_ORG_CONNECTED);
	printf("\n  %s", ipigrp2_printaddr(&drdb->drdb_nexthop));
	if (drdb->drdb_iidb) {
	    printf(" (%s)", dual_idb(drdb->drdb_iidb)->namestring);
	}
	printf(", from %s, Send flag is 0x%x", connected ? "Connected" :
	       redistributed ? "Redistributed" :
	       ipigrp2_printaddr(&drdb->drdb_infosource),
	       drdb->drdb_send_flag);
	if (active && (drdb->drdb_handle != NO_PEER_HANDLE) &&
	    igrp2_test_handle(ddb, &dndb->dndb_reply_status,
			      drdb->drdb_handle)) {
	    printf(", outstanding reply");
	}
	printf("\n      Composite metric is (%u/%u), Route is %s", 
	       drdb->drdb_metric, drdb->drdb_successor_metric,
	       (drdb->drdb_external) ? "External" : "Internal");
	printf("\n      Vector metric:");
	printf("\n        Minimum bandwidth is %d Kbit",
	       (drdb->drdb_vecmetric.bandwidth) ?
	       IGRP2_BANDWIDTH_SCALE/drdb->drdb_vecmetric.bandwidth : 0);
	printf("\n        Total delay is %u microseconds",
	       igrp2_cdelay(drdb->drdb_vecmetric.delay));
	printf("\n        Reliability is %d/255",
	       drdb->drdb_vecmetric.reliability);
	printf("\n        Load is %d/255", drdb->drdb_vecmetric.load);
	printf("\n        Minimum MTU is %d", drdb->drdb_vecmetric.mtu);
	printf("\n        Hop count is %d", drdb->drdb_vecmetric.hopcount);
	if (drdb->drdb_data) {
	    ext_info = chunk_malloc(ddb->extdata_chunk);
	    if (ext_info) {
		bcopy(drdb->drdb_data, ext_info, sizeof(*ext_info));
		printf("\n      External data:");
		printf("\n        Originating router is %i %s",
		       ext_info->routerid,
		       (ext_info->routerid == ddb->routerid.ip_address) ? 
		       "(this system)" : " ");
		printf("\n        AS number of route is %d",
		       ext_info->asystem);
		printf("\n        External protocol is %s, external metric "
		       "is %u", ipigrp2_protid_string(ext_info->protocol),
		       ext_info->metric);
		printf("\n        Administrator tag is %d (0x%08x)",
		       ext_info->tag, ext_info->tag);
		if (ext_info->flag & IGRP2_CD)
		    printf("\n        Exterior flag is set");
		chunk_free(ddb->extdata_chunk, ext_info);
	    }
	} else {
	    if (drdb->drdb_opaque_flags & IGRP2_CD)
		printf("\n      Exterior flag is set");
	}
	drdb_next = drdb->drdb_next;
	free(drdb);
	drdb = drdb_next;
    }
    free(dndb);
    free(ddb);
}

static const char ipigrp2_top_banner[] = {"\nIP-EIGRP Topology Table for process %d"};

/*
 * ipigrp2_show_topology
 *
 * Process "show ip eigrp topology <asystem>
 *
 * OBJ(int,1) = AS number ala "show ip igrp2 topology 109"
 * OBJ(int,2) = TRUE, if IP address/mask pair present ala
 *                    "show ip igrp2 topology 160.89.0.0 255.255.0.0"
 * OBJ(paddr,1) = IP destination
 * OBJ(paddr,2) = IP mask
 */
void ipigrp2_show_topology (parseinfo *csb)
{
    ddbtype    *ddb;
    pdbtype    *pdb;
    int        asystem;
    boolean    detail;
    ipaddrtype ipaddr;
    ipaddrtype mask;

    asystem = GETOBJ(int,1);
    detail = GETOBJ(int,2);
    ipaddr = GETOBJ(paddr,1)->ip_addr;
    mask = GETOBJ(paddr,2)->ip_addr;

    automore_enable(NULL);
    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (!(ddb = pdb->ddb))
	    continue;

	if (!pdb->running)
	    continue;
	if (asystem && pdb->asystem != asystem)
	    continue;

	if (detail) {
	    ipigrp2_show_topology_route(ddb, ipaddr, mask);
	} else {
	    printf(ipigrp2_top_banner, pdb->asystem);
	    igrp2_show_topology(ddb, csb->which);
	    if (asystem)
		break;
	}
    }
    automore_disable();
}

/*
 * ipigrp2_auto_summary_needed
 *
 * An auto-summary is needed for the net/mask on this interface if the
 * interface is speaking EIGRP and none of its addresses fall under
 * the net/mask.
 *
 * Returns TRUE if a summary is needed.
 */
static boolean ipigrp2_auto_summary_needed (pdbtype *pdb, idbtype *idb,
					    ipaddrtype sum_addr,
					    ipaddrtype sum_mask)
{
    ipaddtype *ptr;
    ipaddrtype if_address;

    /* No summary needed if the interface is passive. */

    if (idb->ip_passive_router & pdb->mask)
	return(FALSE);			/* Interface is passive */

    /* No summary needed if the interface has no addresses in our process. */

    if (!get_nets_per_idb(pdb, idb))
	return(FALSE);

    /* No summary needed if the address is not in our process. */

    if (!get_nets_per_address(pdb, sum_addr))
	return(FALSE);

    if (idb->ip_unnumbered) {
	if_address = idb->ip_unnumbered->ip_address;
    } else {
	if_address = idb->ip_address;
    }

    /* No summary if main address is covered by summary. */

    if ((if_address & sum_mask) == sum_addr) {
	return(FALSE);
    }


    /* No summary if secondary is covered by summary. */

    for (ptr = idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
	if ((ptr->address & sum_mask) == sum_addr) {
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * ipigrp2_find_sum_idb
 *
 * Find a given idb for a summary address.
 *
 * Returns NULL if no summary is found.
 */
static sum_idbtype *ipigrp2_find_sum_idb (summarytype *summary, idbtype *idb)
{

    sum_idbtype *sum_idb;

    for (sum_idb = summary->idbQ.qhead; sum_idb; 
	 sum_idb = sum_idb->next) {
	if (sum_idb->idb == idb) break;
    }
    return(sum_idb);
}

/*
 * ipigrp2_delete_summary_interface
 *
 * Delete a single summary entry.  Triggers the appropriate DUAL response.
 * May delete the entire summary entry.
 */
static void ipigrp2_delete_summary_interface (pdbtype *pdb,
					      summarytype *summary,
					      sum_idbtype *sum_idb)
{
    igrp2_take_nbrs_down(pdb->ddb, sum_idb->idb, FALSE, "summary deleted");
    unqueue(&summary->idbQ, sum_idb);
    free(sum_idb);

    if (summary->idbQ.count == 0) {
	ipigrp2_process_summary(pdb, summary, NULL, FALSE);
	unqueue(&pdb->summaryQ, summary);
	free(summary);
    }
}

/*
 * ipigrp2_configure_summary
 *
 * Add/delete summary address from pdb.
 * In this function, we deliberately take neighbors down over every
 * interface the summary is configured.  This allows an easy way to, in 
 * the adding case to delete the more specific destinations for the summary
 * in the neighboring topology tables, and in the deleting case to announce
 * more specific destination to neighbors.
 */

static void ipigrp2_configure_summary (pdbtype *pdb,
				       ipaddrtype address,
				       ipaddrtype mask,
				       idbtype *idb,
				       boolean sense,
				       boolean nvgen)
{
    summarytype *summary;
    sum_idbtype *interface;

    if (ipigrp2_debug_sum) {
	buginf("\nIP-EIGRP: configure_summary: %s %i%m %d", idb->namestring,
	       address, mask, sense);
    }
    interface = NULL;

    /* Normalize address. */

    address = address & mask;

    /*
     * Find summary, if any.
     */
    for (summary = pdb->summaryQ.qhead; summary; 
	 summary = summary->next) {
	if (summary->address == address && summary->mask == mask) {
	    interface = ipigrp2_find_sum_idb(summary, idb);
	    break;
	}
    }

    /*	
     * Adding a summary address.
     */
    if (sense) {
	if (!summary) {
	    summary = malloc(sizeof(summarytype));
	    if (!summary) {
		return;
	    }
	    summary->address = address;
	    summary->mask = mask;
	    summary->min_metric = METRIC_INACCESSIBLE;
	    enqueue(&pdb->summaryQ, summary);
	}
	if (!interface) {
	    interface = malloc(sizeof(sum_idbtype));
	    if (!interface) {
		return;
	    }
	    interface->idb = idb;
	    interface->configured = nvgen;
	    enqueue(&summary->idbQ, interface);
	    if (summary->idbQ.count == 1) {
		ipigrp2_process_summary(pdb, summary, NULL, TRUE);
	    }
	    igrp2_take_nbrs_down(pdb->ddb, idb, FALSE, "summary added");
	}

	/* If we're configuring this one, override any existing autosummary. */

	if (nvgen) {
	    interface->configured = nvgen;
	}
    } else {

	/*
	 * Deleting a summary address.
	 */
	if (nvgen && (!summary || !interface || !interface->configured)) {
	    printf("\nIP-EIGRP: Summary address %i%m not found",
		   address, mask);
	} else {
	    if (summary && interface && (interface->configured == nvgen)) {
		ipigrp2_delete_summary_interface(pdb, summary, interface);
	    }
	}
    }
}

/*
 * ipigrp2_build_auto_summaries
 *
 * Updates the set of autosummaries on an interface to match the current
 * configuration using brute force.  Does nothing if autosummaries are
 * disabled.
 */
static void ipigrp2_build_auto_summaries (pdbtype *pdb, idbtype *idb)
{
    summarytype *summary, *next_sum;
    sum_idbtype *sum_idb;
    int net_ix;
    ipaddrtype net, mask;

    if (!pdb->auto_summary)
	return;

    /*
     * First walk all summaries looking for autosummary entries that
     * are no longer valid.
     */
    for (summary = pdb->summaryQ.qhead; summary; summary = next_sum) {
	next_sum = summary->next;	/* Entry may get deleted! */
	sum_idb = ipigrp2_find_sum_idb(summary, idb);
	if (sum_idb && !sum_idb->configured) { /* Autosummary there */
	    if (!ipigrp2_auto_summary_needed(pdb, idb, summary->address,
					     summary->mask)) {
		ipigrp2_delete_summary_interface(pdb, summary, sum_idb);
	    }
	}
    }

    /*
     * Many networks may be configured with many interfaces exist.
     * Release CPU to avoid CPUHOG or WATCHDOG timeout.
     */
    process_may_suspend();

    /*
     * Now walk all networks and interfaces and create any necessary
     * autosummaries.  ipigrp2_configure_summary will ignore attempts
     * to add duplicates.
     */
    for (net_ix = 0; net_ix < pdb->netcount; net_ix++) {
	net = pdb->networks[net_ix];
	mask = pdb->network_mask[net_ix];
	if (ipigrp2_auto_summary_needed(pdb, idb, net, mask)) {
	    ipigrp2_configure_summary(pdb, net, mask, idb, TRUE, FALSE);
	}
    }
}

/*
 * ipigrp2_interface_up
 * Returns true if this is interface is up and allowed to
 * participate in routing protocol
 */
static boolean ipigrp2_interface_up (idbtype *idb)
{
    return ((idb) && (!idb->static_routing) && interface_up(idb));
}

/*
 * ipigrp2_adjust_connected
 *
 * Adjust all connected routes for an interface.
 */
static void ipigrp2_adjust_connected (pdbtype *pdb, idbtype *idb,
				      boolean sense, boolean config)
{
    ipaddtype *ptr;

    if (idb->ip_unnumbered)		/* Unnumbered i/fs don't count. */
	return;

    /* Twiddle all routes on this interface that fall under our networks. */

    if (get_nets_per_address(pdb, idb->ip_address)) {
	ipigrp2_conn_rtchange(pdb, idb, idb->ip_address, idb->ip_nets_mask,
			      sense, config);
    }

    for (ptr = idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
	if (get_nets_per_address(pdb, ptr->address)) {
	    ipigrp2_conn_rtchange(pdb, idb, ptr->address, ptr->mask, sense,
				  config);
	}
    }
}

/*
 * ipigrp2_ifdown
 *
 * Called to take down an IIDB.
 *
 * "delete" is TRUE if the IIDB should be deleted
 *
 * This routine is safe to call from other threads;  it enqueues work queue
 * entries.
 */
static void ipigrp2_ifdown (pdbtype *pdb, idbtype *idb, boolean delete)
{
    ipigrp2_worktype *work;
    igrp2_idbtype *iidb;
    ddbtype *ddb;

    /* Delete the connected routes (enqueues a bunch of work queue entries). */

    ipigrp2_adjust_connected(pdb, idb, FALSE, delete);

    ddb = pdb->ddb;
    iidb = igrp2_find_iidb(ddb, idb);
    if (!iidb)
	return;
    
    /* Take the interface down. */

    work = chunk_malloc(ddb->workq_chunk);
    if (work == NULL) {
	return;
    }

    iidb->iidb_going_down = delete;
    work->c.ifd.interface = idb;
    ipigrp2_qevent(pdb, work, IGRP2_WORK_IFDOWN);
}

/*
 * ipigrp2_setup_multicast
 *
 * Turn multicast processing on or off on the interface as necessary.
 */
static void ipigrp2_setup_multicast (ddbtype *ddb, idbtype *idb, boolean sense)
{
    ulong other_processes;
    pdbtype *pdb;
    ddbtype *other_ddb;
    igrp2_idbtype *iidb, *other_iidb;

    iidb = igrp2_find_iidb(ddb, idb);
    if (!iidb)
	return;

    /* Bail if the interface is already in the right state. */

    if (sense == iidb->multicast_enabled)
	return;

    /*
     * If we're enabling EIGRP on this interface and this is the first
     * EIGRP process on the interface, enable the multicast.  If we're
     * disabling EIGRP and we're the last EIGRP process on the interface,
     * disable the multicast.
     */
    other_processes = 0;
    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype != PROC_IGRP2)
	    continue;
	other_ddb = pdb->ddb;
	if (!other_ddb)
	    continue;
	if (other_ddb == ddb)		/* Skip our process */
	    continue;
	for (other_iidb = other_ddb->iidbQ.qhead; other_iidb;
	     other_iidb = other_iidb->next) {
	    if (other_iidb->idb == idb)
		other_processes++;
	}
    }
    if (!other_processes) {
	ip_multicast_ctl(idb, ALLIGRPROUTERS, sense);
	iidb->multicast_enabled = sense;
    }
}

/*
 * ipigrp2_set_mtu
 */
static void ipigrp2_set_mtu (igrp2_idbtype *iidb)
{
    idbtype *idb;

    if (iidb) {
	idb = iidb->idb;
	iidb->max_packet_size = min(max_proto_mtu(idb, LINK_IP),
				    MAXIGRP2BYTES) - IPHEADERBYTES(NOPT);
    }
}

/*
 * ipigrp2_mtu_change
 */
static void ipigrp2_mtu_change (hwidbtype *hwidb, idbtype *swidb, int delta)
{
    idbtype *idb;
    igrp2_idbtype *iidb;
    pdbtype *pdb;
    ddbtype *ddb;

    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	    if (pdb->proctype != PROC_IGRP2)
		continue;
	    ddb = pdb->ddb;
	    if (!ddb)
		continue;
	    iidb = igrp2_find_iidb(pdb->ddb, idb);
	    if (ddb->set_mtu)
		(*ddb->set_mtu)(iidb);
	    igrp2_take_nbrs_down(pdb->ddb, idb, FALSE, "mtu change");
	}
    }
}

/*
 * ipigrp2_onoff_idb
 *
 * Turn an interface on or off for EIGRP.  This may cause work queue entries
 * to be enqueued, since we can't necessarily do it all on this thread.
 *
 * If "passive" is true, the IIDB is considered a passive interface
 * (significant only when "sense" is TRUE).
 *
 * Tinkers with the multicast addresses appropriately.
 */
static void ipigrp2_onoff_idb (ddbtype *ddb, idbtype *idb,
			       boolean sense, boolean passive)
{
    igrp2_idbtype *iidb;

    iidb = igrp2_find_iidb(ddb, idb);
    if (sense) {
	if (iidb) {			/* It's already there */
	    iidb->iidb_going_down = FALSE; /* Stop any enqueued deletion */
	} else {
	    igrp2_allocate_iidb(ddb, idb, passive);
	}

	/*
	 * only enable multicast for the on case.  the off case
         * is taken care of when we process the interface down event
         */

	ipigrp2_setup_multicast(ddb, idb, sense);
    } else {
	ipigrp2_ifdown(ddb->pdb, idb, TRUE);
    }

}

/*
 * ipigrp2_enqueue_redist
 * Enqueue a redist event on the work queue.
 */
static void ipigrp2_enqueue_redist (pdbtype *pdb, ndbtype *ndb, int event)
{
    ipigrp2_worktype *work;

    if (!ipigrp2_callback_sanity(pdb))
	return;

    /*
     * Queue redistribution request and process later.
     */
    if (!(work = chunk_malloc(pdb->ddb->workq_chunk))) {
	return;
    }

    work->c.red.dest.address = ndb->netnumber;
    work->c.red.dest.mask = ndb->netsmask;
    work->c.red.pdbindex = ndb->pdbindex;
    work->c.red.event = event;
    ipigrp2_qevent(pdb, work, IGRP2_WORK_REDIST);
}

/*
 * ipigrp2_redist_conn_state
 *
 * Cause a connected route to be redistributed or not when the "network"
 * configuration changes.
 *
 * Note that the PDB reflects the addition or deletion of a network before
 * we are called, so ip_pdb_on_ndb will correctly reflect the new state of
 * the network command.
 */
static void ipigrp2_redist_conn_state (pdbtype *pdb, idbtype *idb,
				       ipaddrtype address, ipaddrtype mask)
{
    ndbtype *ndb;

    if (idb->ip_unnumbered)
	return;
    ndb = ip_rtlookup((address & mask), mask);
    if (ndb == NULL)
	return;
    if (!(ndb->attrflags & NET_ATTACHED))
	return;
    /*
     * Are we redistributing this route either explicitly or implicitly?
     * Bail if not.
     */
    if (!ip_redist_flag(ndb, pdb))
	return;

    /*
     * Redistribute in the opposite sense of the setting of the network
     * command (but if the interface is down, don't redistribute the
     * route if we would otherwise).
     */
    if (ip_pdb_on_ndb(ndb, pdb)) {	/* We own the route */
	ipigrp2_enqueue_redist(pdb, ndb, ROUTE_DOWN);
    } else if (interface_up(idb)) {	/* Don't own;  is it up? */
	ipigrp2_enqueue_redist(pdb, ndb, ROUTE_UP);
    }
}

/*
 * ipigrp2_conn_summary_depend
 * Check summary route dependencies as a connected route comes/goes.
 */

static void ipigrp2_conn_summary_depend (ddbtype *ddb, idbtype *idb,
					 ipaddrtype address,
					 ipaddrtype mask, boolean sense)
{
    ulong metric;
    boolean up;

    if (ipigrp2_debug_sum) {
	buginf("\nIP-EIGRP: conn_summary_depend: %s %i%m %d",
	       idb->namestring, address, mask, sense);
    }
    if (idb->ip_unnumbered != NULL)
	return;
    up = (interface_up(idb) && sense);
    /*
     * Check if interface address was part of summary.
     */
    metric = igrp2_computemetric(
		igrp_metric_to_eigrp(SCALED_BANDWIDTH(idb->visible_bandwidth)),
				 idb->hwptr->load,
				 igrp_metric_to_eigrp(idb->delay),
				 idb->hwptr->reliability,
				 ddb->k1, ddb->k2, ddb->k3, ddb->k4, ddb->k5);
    ipigrp2_summary_depend(ddb->pdb, address, mask, NULL, metric, up);
}

/*
 * ipigrp2_redistribute
 *
 * A redistribute command was entered for the IP-IGRP2 process.
 */
static void ipigrp2_redistribute (pdbtype *pdb, pdbtype *dist, boolean sense)
{
    ipigrp2_worktype *work;

    if (!ipigrp2_callback_sanity(pdb))
	return;
    if (ipigrp2_debug_not) {
	buginf("\nIP-EIGRP: Callback: redistribute %s. Sense: %d",
	       dist->name ? dist->name : "", sense);
    }
    /*
     * Do not redistribute if there is no default-metric set, unless it is
     * igrp, connected or static.
     */
    if (sense) {
	if (((dist->proctype &
	      (PROC_IGRP|PROC_IGRP2|PROC_STATIC|PROC_CONNECTED)) == 0) &&
	    !ip_def_metric_configured(pdb, dist->index)) {
	    return;
	}
    }

    work = chunk_malloc(pdb->ddb->workq_chunk);
    if (work == NULL) {
	return;
    }

    work->c.rea.pdbindex = dist->index;
    work->c.rea.sense = sense;
    ipigrp2_qevent(pdb, work, IGRP2_WORK_REDISTALL);
}

/*
 * ipigrp2_network_address
 *
 * Handle a single address on an interface when a network command is
 * added or deleted.
 *
 * We tinker with the redistributed connecteds, the topology table, and
 * summaries.
 */
static void ipigrp2_network_address (pdbtype *pdb, ipaddrtype address,
				     ipaddrtype mask, idbtype *idb,
				     boolean sense)
{
    ddbtype *ddb;

    ddb = pdb->ddb;
    ipigrp2_redist_conn_state(pdb, idb, address, mask);
    ipigrp2_conn_rtchange(pdb, idb, address, mask, sense, TRUE);
}

/*
 * ipigrp2_network_command
 *
 * Process "network" command for an IGRP2 routing process. This function
 * will obtain the idbs associated with the network number configured.
 * Each idb will be stored in an igrp2_idbtype data structure that resides
 * in the ddb.
 *
 * We will always be called for every network, even when a "no router eigrp"
 * command is entered (each network is turned off first).  When we
 * are called, the change has already been made to the pdb (the network
 * has been added or deleted).
 */
static void ipigrp2_network_command (pdbtype *pdb, ipaddrtype net,
				     ipaddrtype mask,
				     uchar flags, ulong area,
				     int area_id_format, boolean sense)
{
    idbtype *idb;
    ddbtype    *ddb;
    ipaddtype  *ptr;
    ipaddrtype address;
    igrp2_idbtype *iidb;
    pdbtype    *dist;
    int interface_count;
    int count;
    boolean passive_intf;

    ddb = pdb->ddb;
    ipigrp2_set_routerid(ddb);		/* Just in case. */
    net = get_majornet(net);
    mask = get_majormask(net);

    interface_count = ddb->iidbQ.count;

    /* Walk all IDBs trolling for networks that fall under this config. */

    FOR_ALL_SWIDBS(idb) {
	if (!idb->ip_enabled)
	    continue;

	if (idb->ip_unnumbered) {
	    address = idb->ip_unnumbered->ip_address;
	} else {
	    address = idb->ip_address;
	}

	passive_intf = ((idb->ip_passive_router & pdb->mask) != 0);
	count = get_nets_per_idb(pdb, idb);
	iidb = igrp2_find_iidb(ddb, idb);

	/* If we're needing to create an IIDB, do it now. */

	if (sense && count) {
	    ipigrp2_onoff_idb(ddb, idb, TRUE, passive_intf);
	}

	/*
	 * Check primary address.
	 */
	if ((address & mask) == net) {
	    ipigrp2_network_address(pdb, address, idb->ip_nets_mask, idb,
				    sense);
	}

	/*
	 * Process same thing for secondary addresses.
	 */
	for (ptr = idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
	    if ((ptr->address & mask) == net) {
		ipigrp2_network_address(pdb, ptr->address, ptr->mask, idb,
					sense);
	    }
	}

	/* Update any autosummaries for this interface. */

	ipigrp2_build_auto_summaries(pdb, idb);

	/* If we're needing to delete an IIDB, do it now. */

	if (!sense && !count && iidb)
	    ipigrp2_onoff_idb(ddb, idb, FALSE, passive_intf);

    }

    /*
     * Return if we are not adding the first idb or deleting the last idb.
     */
    if (interface_count && ddb->iidbQ.count)
	return;

    /*
     * Add/delete redistribution entries in topology table if process just
     * started or shall be deleted.
     */
    for (dist = protoQ.qhead; dist; dist = dist->next) {
	if (pdb == dist) continue;
	if (!(pdb->distflag & dist->mask))
	    continue;
	ipigrp2_redistribute(pdb, dist, sense);
    }
}

/*
 * ipigrp2_passive_interface_command
 * Process passive-interface router command for this process.
 */
static void ipigrp2_passive_interface_command (pdbtype *pdb, idbtype *idb,
					       boolean sense)
{
    ipigrp2_worktype *work;

    if (ipigrp2_debug_not)
	buginf("\nIP-EIGRP: Callback: passive_interface_command %s sense %d",
	       idb ? idb->namestring : "", sense);

    /*
     * Queue table reload request and process later.
     */
    work = chunk_malloc(pdb->ddb->workq_chunk);
    if (!work) {
	return;
    }
    work->c.pasv.interface = idb;
    work->c.pasv.sense = sense;
    ipigrp2_qevent(pdb, work, IGRP2_WORK_PASSIVEINTF);
}

/*
 * ipigrp2_ip_address_command
 *
 * Called when an IP address is added or deleted from an interface.
 *
 * The IDB will always reflect the state of the change;  if added, the
 * address will already be present, and vice versa.  The one exception to
 * this is with unnumbered interfaces;  the callback for deletion is done
 * before the unnumbered pointer is NULLed out.
 */
static void ipigrp2_ip_address_command (idbtype *idb, ipaddrtype address,
					ipaddrtype mask, boolean secondary,
					boolean adding)
{

    pdbtype       *pdb;
    int           addr_count;
    boolean	  passive_intf;
    ddbtype	  *ddb;

    if (ipigrp2_debug_not) {
	buginf("\nIP-EIGRP: Callback: address_command %s %i%m sense %d",
	       idb ? idb->namestring : "", address, mask, adding);
    }

    /*
     * Inform all IGRP2 speakers that are configured on this interface
     * there is an address change.
     */
    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype != PROC_IGRP2)
	    continue;
	if (!pdb->running)
	    continue;

	ddb = pdb->ddb;
	if (!ddb)
	    continue;
	ipigrp2_set_routerid(ddb);	/* Just in case */
	passive_intf = ((idb->ip_passive_router & pdb->mask) != 0);

        /*
         * When a secondary IP address is added/deleted,
         * the parameter, address, is not 0L.
         * Its value should be preserved.
         */
	if (address == 0L && idb->ip_unnumbered) {
	    address = idb->ip_unnumbered->ip_address;
            mask = idb->ip_nets_mask;
        }
	addr_count = get_nets_per_address(pdb, address);

	if (adding) {
	    if (addr_count) {		/* This one counts */
		ipigrp2_onoff_idb(ddb, idb, TRUE, passive_intf);
		ipigrp2_conn_rtchange(pdb, idb, address, mask, adding, TRUE);
	    }
	} else {			/* Deleting */
	    if (addr_count) {		/* It's ours */
		ipigrp2_conn_rtchange(pdb, idb, address, mask, adding, TRUE);
	    }
            if (igrp2_find_iidb(ddb, idb) != NULL) {
                if (idb->ip_unnumbered) {
                    /*
                     * The unnumbered link goes down when either
                     * 'no ip unnumbered' or deleting the primary IP.
                     * The unnumbered link still uses the primary IP
                     * and stays up when a secondary IP is deleted
                     * by 'no ip <address> <mask> secondary'.
                     */
                    if (!secondary) {
                        ipigrp2_onoff_idb(ddb, idb, FALSE, passive_intf);
                    }
                } else if (get_nets_per_idb(pdb, idb) == 0) {

                    /* They're all gone now. */

                    ipigrp2_onoff_idb(ddb, idb, FALSE, passive_intf);
                }
            }
	}

	/* Clear all neighbors on the interface. */

	igrp2_take_nbrs_down(ddb, idb, FALSE, "address changed");
    }
}

/*
 * ipigrp2_process_reload_iptable
 * IP routing table has been reloaded.  Need to re-populate it with
 * EIGRP routes from the topology table.
 */
static void ipigrp2_process_reload_iptable (pdbtype *pdb)
{
    ddbtype *ddb;

    ddb = pdb->ddb;
    if (ddb == NULL)
	return;
    dual_reload_proto_table(ddb);
}

/*
 * ipigrp2_reload_iptable
 * IP routing table has been reloaded.  Need to re-populate it with
 * EIGRP routes from the topology table.
 */
static void ipigrp2_reload_iptable (pdbtype *pdb, idbtype *idb)
{
    ipigrp2_worktype *work;

    if (!ipigrp2_callback_sanity(pdb))
	return;
    if (ipigrp2_debug_not) {
	buginf("\nIP-EIGRP: Callback: reload_iptable %s",
	       idb ? idb->namestring : "");
    }

    if (idb) {
	/*
	 * This is just an interface down event.  We will get
	 * redist callbacks, or backup route callbacks, or lost route
	 * callbacks for such an event.  No need to attempt to reload the
	 * entire topology table.
	 */
	return;
    }
    /*
     * Queue table reload request and process later.
     */
    work = chunk_malloc(pdb->ddb->workq_chunk);
    if (!work) {
	return;
    }
    ipigrp2_qevent(pdb, work, IGRP2_WORK_TABLERELOAD);
}

/*
 * ipigrp2_process_exterior_change
 * A route's interior/exterior disposition has changed.  Attempt to mirror
 * that change in the topology table.  Also, notify our neighbors of this
 * event.
 */

static void ipigrp2_process_exterior_change (pdbtype *pdb,
					     ipigrp2_worktype *work)
{
    ddbtype *ddb;
    dual_ndbtype *dndb;
    dual_rdbtype *drdb;
    igrp2nettype dest;
    boolean iptable_entry_exterior, rdb_is_exterior, changed;
    ipigrp2_extdatatype *extdata;
    ipaddrtype addr, mask;
    uchar *flag_ptr;
    
    addr = work->c.d.dest.address;
    mask = work->c.d.dest.mask;
    ddb = pdb->ddb;
    dest.ip_network.address = addr;
    dest.ip_network.mask = mask;
    dndb = dual_ndblookup(ddb, &dest);
    if (dndb == NULL)
	return;
    iptable_entry_exterior = ipigrp2_is_exterior(addr, mask);
    /*
     * IP routing table has indicated a change in exterior/interior state
     * for this route.  Update the drdbs of our feasible successors to
     * reflect the change.
     */
    if (IPIGRP2_DTARGET(addr, ddb))
	buginf("\nIP-EIGRP: Route %i%m now %sterior", addr, mask, 
	       iptable_entry_exterior ? "ex" : "in");
    extdata = NULL;
    changed = FALSE;
    for (drdb = dndb->dndb_rdb; drdb; drdb = drdb->drdb_next) {
	if (drdb->drdb_successor_metric >= dndb->dndb_old_metric)
	    continue;
	if (drdb->drdb_external) {
	    extdata = drdb->drdb_data;
	    if (extdata == NULL)
		continue;
	    flag_ptr = &extdata->flag;
	} else {
	    flag_ptr = &drdb->drdb_opaque_flags;
	}
	rdb_is_exterior = (*flag_ptr & IGRP2_CD);
	if (iptable_entry_exterior == rdb_is_exterior)
	    continue;
	if (iptable_entry_exterior) {
	    *flag_ptr |= IGRP2_CD;
	} else {
	    *flag_ptr &= ~IGRP2_CD;
	}
	changed = TRUE;
    }
    if (changed) {
	dual_sendupdate(ddb, dndb, "Change in CD bit");
    }
}

/*
 * ipigrp2_exterior_change
 * A route's interior/exterior disposition has changed.  Queue the event.
 */

static void ipigrp2_exterior_change (pdbtype *pdb, ndbtype *ndb, boolean sense)
{
    ipigrp2_worktype *work;

    if (!ipigrp2_callback_sanity(pdb))
	return;
    if (ipigrp2_debug_not) {
	buginf("\nIP-EIGRP: Callback: exterior_change %i%m. Sense: %d",
	       ndb->netnumber, ndb->netsmask, sense);
    }
    /*
     * Queue table reload request and process later.
     */
    work = chunk_malloc(pdb->ddb->workq_chunk);
    if (!work)
	return;

    work->c.d.dest.address = ndb->netnumber;
    work->c.d.dest.mask = ndb->netsmask;
    ipigrp2_qevent(pdb, work, IGRP2_WORK_INTEXTSTATE);
}

/*
 * ipigrp2_get_prot_id
 *
 * Map an ndb->proctype into an IP-IGRP2 external protocol id.
 */
static int ipigrp2_get_prot_id (ulong proctype)
{
    enum IPIGRP2_PROT_ID prot_id;

    switch (proctype) {
    case PROC_CONNECTED:
	prot_id = CONNECTED_PROTID;
	break;
    case PROC_STATIC:
	prot_id = STATIC_PROTID;
	break;
    case PROC_BGP:
	prot_id = BGP_PROTID;
	break;
    case PROC_RIP:
	prot_id = RIP_PROTID;
	break;
    case PROC_IGRP:
	prot_id = IGRP1_PROTID;
	break;
    case PROC_EGP:
	prot_id = EGP_PROTID;
	break;
    case PROC_OSPF:
	prot_id = OSPF_PROTID;
	break;
    case PROC_ISIS:
	prot_id = ISIS_PROTID;
	break;
    case PROC_IGRP2:
	prot_id = IGRP2_PROTID;
	break;
    default:
	prot_id = NULL_PROTID;
	break;
    }
    return(prot_id);
}

/*
 * ipigrp2_build_external_info
 *
 * Build external data for external destinations. Get information from either
 * the routing table entry or policy configuration.
 *
 * Returns a pointer to the external info block, or NULL if we're out of
 * memory.
 */
static ipigrp2_extdatatype *ipigrp2_build_external_info (pdbtype *pdb,
							 ddbtype *ddb, 
							 pdbtype *dist,
							 ndbtype *ndb)
{

    ipigrp2_extdatatype *ext_info;
    route_maptype       *map;
    ulong               tag;

    ext_info = chunk_malloc(ddb->extdata_chunk);
    if (ext_info) {
	
	/*
	 * Check and see if route-map has a "set tag" value.
	 */
	tag = ndb->tag;
	map = pdb->route_map;
	if (map) {
	    if (map->set_list.set_flags & ROUTE_MAP_TAG_SET) {
		tag = map->set_list.tag;
	    }
	}

	ext_info->routerid = ddb->routerid.ip_address;
	ext_info->asystem = dist->asystem;
	ext_info->tag = tag;
	ext_info->metric = ndb->metric;
	ext_info->protocol = ipigrp2_get_prot_id(dist->proctype);

	/*
	 * Set candidate default bit if routing table says so.  This is
	 * defined as a series of flags, but since only one bit (CD) is
	 * currently supported, we simply equate it.
	 */
	ext_info->flag = (ndb->attrflags & NET_EXTERIOR) ? IGRP2_CD : 0;
    }
    return(ext_info);
}

/*
 * ipigrp2_find_ndb_idb
 * For connected/static routes where no default metric is configured, we
 * derive the metric from the route's next hop interface.  Find one.
 */
static idbtype *ipigrp2_find_ndb_idb (ndbtype *ndb)
{
    ipaddrtype dummy;
    idbtype *idb;
    rdbtype *rdb;
    int i;

    idb = NULL;
    for (i = 0; i < ndb->routecount; i++) {
	rdb = ndb->rdb[i];
	if (rdb == NULL)
	    continue;
	idb = rdb->idb;
	if (idb)
	    return(idb);
	if (rdb->gateway) {
	    dummy = rdb->gateway;
	    idb = reg_invoke_ip_first_hop(&dummy, dummy, IP_PATH_DEFFLAGS);
	    if (idb)
		return(idb);
	    /*
	     * Play by the same rules as ip_static_update().  If
	     * samecable returns non NULL, that means the interface is
	     * up, but the connected route has not made it into the
	     * routing table yet.  However, the static route is
	     * obviously in the routing table already.
	     */
	    idb = samecable(rdb->gateway);
	    if (idb)
		return(idb);
	}
    }
    dummy = ndb->netnumber;
    idb = reg_invoke_ip_first_hop(&dummy, dummy, IP_PATH_DEFFLAGS);
    return(idb);
}

/*
 * ipigrp2_pick_redist_metric
 *
 * Choose what metric to use for this redistributed route.
 *
 * Returns FALSE if a metric could not be decided upon.
 */

static boolean ipigrp2_pick_redist_metric (ddbtype *ddb, pdbtype *pdb,
					   ndbtype *ndb, rdbtype *rdb,
					   pdbtype *src_pdb, ulong *metric,
					   igrp2metric *vecmetric)
{
    route_maptype *map;
    struct vector def_metric;
    boolean default_metric;
    idbtype *idb;

    bzero(vecmetric, sizeof(*vecmetric));
    if (rdb) {
	/*
	 * This is a hack.  Redistribution is based on the index set
	 * in the ndb.  However, we may still have a different
	 * process which wrote this ndb.  We must determine if it
	 * is igrp1 or igrp2 regardless of what the index in the
	 * ndb is set to in order to avoid confusing the metrics.
	 */
	if (INDEX2PDB(rdb->pdbindex)->proctype & (PROC_IGRP)) {
	    vecmetric->delay = igrp_metric_to_eigrp(rdb->rdb_delay);
	    vecmetric->bandwidth = igrp_metric_to_eigrp(rdb->rdb_bandwidth);
	} else {
	    vecmetric->delay = rdb->rdb_delay;
	    vecmetric->bandwidth = rdb->rdb_bandwidth;
	}
	vecmetric->reliability = rdb->reliability;
	vecmetric->load = rdb->load;
	vecmetric->mtu = rdb->rdb_mtu;
	vecmetric->hopcount = rdb->hopcount;
    } else {
	if ((map = pdb->route_map) &&
	    (map->set_list.set_flags & ROUTE_MAP_METRIC_SET) &&
	    map->set_list.metric.igrp_metric) {
	    vecmetric->delay = 
		igrp_metric_to_eigrp(map->set_list.metric.value.vector.delay);
	    vecmetric->bandwidth =
		igrp_metric_to_eigrp(map->set_list.metric.value.vector.bandwidth);
	    vecmetric->reliability =
		map->set_list.metric.value.vector.reliability;
	    vecmetric->load =
		map->set_list.metric.value.vector.loading;
	    vecmetric->mtu = map->set_list.metric.value.vector.mtu;
	} else {
	    default_metric = ip_def_metric_configured(pdb, src_pdb->index);
	    if (default_metric) {
		ip_get_def_metric_vector(pdb, src_pdb->index, &def_metric);
		vecmetric->delay = igrp_metric_to_eigrp(def_metric.delay);
		vecmetric->bandwidth =
		    igrp_metric_to_eigrp(def_metric.bandwidth);
		vecmetric->reliability = def_metric.reliability;
		vecmetric->load = def_metric.loading;
		vecmetric->mtu = def_metric.mtu;
	    } else {
		/*
		 * This stuff is used for static routes, and redistributed
		 * connected routes.  It supports the use
		 * of the metric stored in the next hop interface iff
		 * a default metric is not configured.
		 */
		if ((src_pdb->proctype != PROC_STATIC) &&
		    (src_pdb->proctype != PROC_CONNECTED))
		    return(FALSE);
		idb = ipigrp2_find_ndb_idb(ndb);
		if (idb == NULL)
		    return(FALSE);
		vecmetric->delay = igrp_metric_to_eigrp(idb->delay);
		vecmetric->bandwidth =
		    igrp_metric_to_eigrp(
				 SCALED_BANDWIDTH(idb->visible_bandwidth));
		vecmetric->reliability = idb->hwptr->reliability;
		vecmetric->load = idb->hwptr->load;
		vecmetric->mtu = idb->ip_mtu;
	    }
	    vecmetric->hopcount = 0;
	}
    }
    *metric = igrp2_computemetric(vecmetric->bandwidth,
				  vecmetric->load,
				  vecmetric->delay,
				  vecmetric->reliability,
				  ddb->k1, ddb->k2, ddb->k3, ddb->k4, ddb->k5);
    return(TRUE);
}

/*
 * ipigrp2_rtchange
 *
 * Inform DUAL about redistributed route. This inserts/deletes the
 * route from the topology table.
 *
 * This function is also called when an IGRP2 route gets overwritten by
 * another process because of administrative distance.
 */
static void ipigrp2_rtchange (pdbtype *pdb, ndbtype *ndb, int event)
{
    ddbtype *ddb;
    rdbtype *rdb;
    boolean reachable, apply_filters;
    pdbtype *src_pdb;
    dual_ndbtype *dndb;
    dual_rdbtype *drdb;
    dual_newroute newroute;
    dual_rtev_type dual_event_type;

    bzero(&newroute, sizeof(newroute));

    reachable = (event == ROUTE_UP || event == ROUTE_MODIF);
    src_pdb = NULL;
    /*
     * If we are implicitly redistributing a connected/interface static
     * route, and one of the protocols which is causing us to do so caused
     * us to get a DOWN event, then consider other protocols which may
     * also be causing us to implicitly redistribute this route.
     */
    if ((ndb->attrflags & NET_ATTACHED) && (event == ROUTE_DOWN)) {
	src_pdb = ip_get_src_pdb(pdb, ndb, FALSE);
	if (src_pdb && src_pdb != pdb) {
	    event = ROUTE_MODIF;
	    reachable = TRUE;
	}
    } else if (reachable) {
	if (INDEX2PDB(ndb->pdbindex) != NULL) {
	    src_pdb = ip_get_src_pdb(pdb, ndb, FALSE);
	}
    }

    if (src_pdb == NULL) {
	/*
	 * The ndb's owning pdb may no longer exist, in which case src_pdb
	 * is NULL.  Make sure we don't dereference src_pdb.
	 */
	reachable = FALSE;
    }
    rdb = NULL;
    if (reachable) {
	apply_filters = TRUE;
	/*
	 * Determine real owner
	 */
	if (ndb->attrflags & NET_ATTACHED) {
	    newroute.new_keep = TRUE;
	    if (src_pdb == pdb) {
		src_pdb = INDEX2PDB(ndb->pdbindex);
	    }
	}
	/*
	 * If the route is owned by either IGRP1 or IGRP2, use routing
	 * table metrics and not configured (default-metric) metrics.
	 */
	if (src_pdb->proctype & (PROC_IGRP | PROC_IGRP2)) {
	    rdb = ndb->rdb[0];
	    apply_filters = (pdb->asystem != src_pdb->asystem);
	}

	/*
	 * Perform configuration filters only if redistributing
	 * between different routing processes. IGRP1 109 and IGRP2
	 * 109 is not subject to filters.
	 */
	if (apply_filters) {
	    if (!pdb_outfilter(pdb, NULL, src_pdb, ndb->netnumber)) {
		reachable = FALSE;
	    }
	    pdb->route_map = NULL;
	    if (!pdb_routemap(pdb, src_pdb, ndb)) {
		reachable = FALSE;
	    }
	}
    }
    /*
     * Inform DUAL that route has changed.
     */
    ddb = pdb->ddb;
    newroute.new_dest.ip_network.address = ndb->netnumber;
    newroute.new_dest.ip_network.mask = ndb->netsmask;

    /*
     * Overwritten case.
     */
    if (pdb == src_pdb)
	dual_lostroute(ddb, &newroute.new_dest);

    /*
     * Set up metric for DUAL call.
     */
    if (reachable) {
	if (!ipigrp2_pick_redist_metric(ddb, pdb, ndb, rdb, src_pdb,
					&newroute.new_metric,
					&newroute.new_vecmetric))
	    return;
	dual_event_type = (event == ROUTE_UP) ? DUAL_RTUP : DUAL_RTCHANGE; 
	ndb->advertisingmask |= pdb->mask;
    } else {
	newroute.new_metric = METRIC_INACCESSIBLE;
	newroute.new_vecmetric.delay = METRIC_INACCESSIBLE;
	dual_event_type = DUAL_RTDOWN;
	ndb->advertisingmask &= ~pdb->mask;
    }

    /*
     * Build the external information for a specific redistributed
     * route. IGRP1 routes are considered external. IGRP2 routes from
     * another process are considered external.  IGRP2 routes from
     * this process (routes going down - overwritten by another process)
     * are left in whatever state in which they were found.
     */
    dndb = dual_ndblookup(ddb, &newroute.new_dest);
    if (reachable && !ip_pdb_on_ndb(ndb, pdb)) {
	newroute.new_data =
	    ipigrp2_build_external_info(pdb, ddb, src_pdb, ndb);
	newroute.new_external = TRUE;
    } else if (!reachable && !ip_pdb_on_ndb(ndb, pdb) &&
	       dndb && igrp2_route_is_external(dndb)) {
	drdb = dndb->dndb_rdb;
	newroute.new_data = chunk_malloc(ddb->extdata_chunk);
	newroute.new_external = TRUE;
	newroute.new_opaque_flags = drdb->drdb_opaque_flags;
	if (newroute.new_data) {
	    bcopy(drdb->drdb_data, newroute.new_data,
		  sizeof(ipigrp2_extdatatype));
	}
    } else {
	newroute.new_data = NULL;
	newroute.new_external = FALSE;
	if (ndb->attrflags & NET_EXTERIOR) {
	    newroute.new_opaque_flags = IGRP2_CD;
	} else {
	    newroute.new_opaque_flags = 0;
	}
    }

    /*
     * Inform DUAL.
     */
    newroute.new_origin = IGRP2_ORG_REDISTRIBUTED;
    /*
     * If we are telling DUAL to delete a redistributed route, and it is
     * a feasible succesor, then update our possible summary dependencies.
     */
    if (dual_event_type == DUAL_RTDOWN) {
	drdb = dual_routelookup(ddb, &newroute.new_dest, newroute.new_iidb,
				&newroute.new_nexthop);
	if (drdb) {
	    ipigrp2_summary_depend(pdb, ndb->netnumber, ndb->netsmask,
				   NULL, drdb->drdb_metric, FALSE);
	}
    }
    dual_rtchange(ddb, &newroute, dual_event_type);

    /*
     * If DUAL installed a redistributed route as a feasible successor,
     * then update our possible summary dependencies.
     */
    if (dual_event_type != DUAL_RTDOWN) {
	drdb = dual_routelookup(ddb, &newroute.new_dest, newroute.new_iidb,
				&newroute.new_nexthop);
	if (drdb) {
	    ipigrp2_summary_depend(pdb, ndb->netnumber, ndb->netsmask,
				   &newroute.new_vecmetric, drdb->drdb_metric,
				   TRUE);
	}
    }
}

/*
 * ipigrp2_process_redistribute
 * Now, finally process the redistribute command.
 */
static void ipigrp2_process_redistribute (pdbtype *pdb, ipigrp2_worktype *work)
{
    ndbtype *ndb, *sdb, *db;
    int     ndb_bucket, sdb_bucket;
    pdbtype *dist;

    dist = INDEX2PDB(work->c.rea.pdbindex);
    if (dist == NULL)
	return;

    /*
     * For each network created by routing process dist, add to topology
     * table.
     */
    ndb_bucket = 0;
    sdb_bucket = 0;
    ndb = NULL;
    sdb = NULL;
    while ((db = geteachroute(&ndb, &ndb_bucket, &sdb, &sdb_bucket, FALSE))) {
	if (!ip_pdb_on_ndb(db, dist)) {
	    continue;
	}
	if ((db->attrflags & NET_ATTACHED) && ip_pdb_on_ndb(db, pdb)) {
	    /*
	     * Oh wait, we already consider this route to be connected, or
	     * an implicitly redistributed static route.
	     * Until another event causes that to no longer be true, ignore
	     * that this route is also being redistributed.
	     */
	    continue;
	}
	if (db->attrflags & NET_NOREDIST)
	    continue;
	if (db->metric == METRIC_INACCESSIBLE)
	    continue;
	ipigrp2_rtchange(pdb, db, (work->c.rea.sense) ? ROUTE_UP : ROUTE_DOWN);
    }
}

/*
 * ipigrp2_redist_conn_sanity
 * We are configured to run over this network via the "network" command, and
 * we've received a redist callback regarding this connected, or interface
 * static route.  Decide if we should consider this route as connected or
 * redistributed.
 * 
 * If this is an interface static route, then it is being implicitly
 * redistributed, so consider it as redistributed.
 * If this is a connected route created dynamically for a P2P async interface,
 * consider it as redistributed.
 * If it is any other kind of connected route, consider it to be connected.
 * These other connected routes are added and deleted from the topology table
 * via interface state changes, so they should be ignored here.
 * Return TRUE to indicate that the route should be handled as a redistributed
 * route.
 */
static inline boolean ipigrp2_redist_conn_sanity (ndbtype *ndb)
{
    ulong proctype;

    proctype = NDB2PROCTYPE(ndb);
    if (proctype == PROC_STATIC)
	return(TRUE);
    if (proctype != PROC_CONNECTED)
	return(FALSE);
    /*
     * Route is connected.  Is it a dynamic route for a P2P async interface,
     * or can we ignore it?
     */
    if (ndb->netsmask != MASK_HOST)
	return(FALSE);
    if (ip_up_ouraddress(ndb->netnumber))
	return(FALSE);
    return(TRUE);
}

/*
 * ipigrp2_redist_callback
 *
 * A route that IP-IGRP2 is redistributing has changed. Queue request
 * to process later while running under IP-IGRP2's process.
 */
static void ipigrp2_redist_callback (pdbtype *pdb, ndbtype *ndb, int event,
				     int default_route)
{
    pdbtype *src_pdb;
    ulong proctype, pdbindex;

    if (!ipigrp2_callback_sanity(pdb))
	return;
    if (ipigrp2_debug_not) {
	buginf("\nIP-EIGRP: Callback: redist %s %i%m%s.  Event: %d",
	       INDEX2PDB(ndb->pdbindex)->name ?
	       INDEX2PDB(ndb->pdbindex)->name : "",
	       ndb->netnumber, ndb->netsmask,
	       default_route ? " (default)" : "", event);
    }
    if (ndb->attrflags & NET_SUBNETTED) {
	/*
	 * This is a "dummy" ndb.  Ignore the callback.
	 */
	return;
    }
    if (ip_own_ndb(ndb, pdb)) {
	/*
	 * Shouldn't happen, but...
	 */
	return;
    }
    src_pdb = ip_get_src_pdb(pdb, ndb, FALSE);
    if (src_pdb == pdb) {
	if (!ipigrp2_redist_conn_sanity(ndb))
	    return;
	src_pdb = INDEX2PDB(ndb->pdbindex);
    }
    /*
     * Otherwise, we must figure out the owner in order to perform
     * our default metric check.  Either
     *    a routing protocol we are redistributing owns or is configured to 
     *    run over this network via the "network" command,
     * or
     *    we are redistributing connected.
     * If src_pdb is NULL, don't necessarily ignore this callback event, as
     * we may still have a redistributed topo table entry to clean up.
     */
    if (event == ROUTE_UP) {
	if (src_pdb == NULL) {
	    /*
	     * Shouldn't happen, but...
	     */
	    return;
	}
	proctype = src_pdb->proctype;
	pdbindex = src_pdb->index;
	/*
	 * Do not allow redistributed networks when a default metric isn't 
	 * configured, except for igrp, igrp2, static or connected.
	 */
	if (((proctype &
	      (PROC_IGRP|PROC_IGRP2|PROC_STATIC|PROC_CONNECTED)) == 0) &&
	    !ip_def_metric_configured(pdb, pdbindex)) {
	    return;
	}
    }
    ipigrp2_enqueue_redist(pdb, ndb, event);
}

/*
 * ipigrp2_candidate_update
 * Some feasible successors are marked candidate defaults, some
 * are not.  So the entry in the routing may not, but probably should be.
 * Make sure the routing table entry is a candidate default.
 */
static void ipigrp2_candidate_update (ddbtype *ddb, dual_ndbtype *dndb)
{
    ipaddrtype addr, mask;
    ndbtype *ndb;
    pdbtype *pdb;

    pdb = ddb->pdb;
    addr = dndb->dndb_dest.ip_network.address;
    mask = dndb->dndb_dest.ip_network.mask;
    if (ipigrp2_debug_not) {
	buginf("\nIP-EIGRP: Callback: candidate_update %i%m", addr, mask);
    }
    ndb = ip_rtlookup(addr, mask);
    if (!ndb)
	return;
    if (!ip_own_ndb(ndb, pdb))
	return;
    if (ndb->attrflags & NET_EXTERIOR)
	return;
    if (ipigrp2_debug) {
	buginf("\nIP-EIGRP: %i%m now marked exterior", addr, mask);
    }
    ndb->attrflags |= NET_DEXTERIOR;
}

/*
 * ipigrp2_process_redist
 *
 * Process each element on the redistribution queue. As each element
 * is inserted in the topology table, it is then unqueued.
 */
static void ipigrp2_process_redist (pdbtype *pdb, ipigrp2_worktype *work)
{
    ndbtype *ndb, temp_ndb;
    int event;

    event = work->c.red.event;
    ndb = ip_rtlookup(work->c.red.dest.address, work->c.red.dest.mask);
    if (ndb) {

	/*
	 * The owner of the route has changed during the queuing period.
	 */
	if (ndb->pdbindex != work->c.red.pdbindex) {
	    ndb = NULL;
	}
    }

    /*
     * Route has been, or is in the process of being deleted.
     * Process ROUTE_DOWN event with use of temporary db on stack
     * if needed.  temp_ndb->pdbindex = 0 down below does not get
     * used in ipigrp2_rtchange().
     * Route may still exist, but it might be time to stop redistributing
     * this particular route.  For example, we are redistributing 
     * RIP, but RIP is no longer configured over network X.  (Ie. someone
     * typed 'no network X').
     */
    if ((ndb == NULL) || (ndb->metric == METRIC_INACCESSIBLE) ||
	(!ip_redist_flag(ndb, pdb) && !ip_pdb_on_ndb(ndb, pdb))) {
	if (ndb == NULL) {
	    ndb = &temp_ndb;
	    ndb->netnumber = work->c.red.dest.address;
	    ndb->netsmask = work->c.red.dest.mask;
	    ndb->rdb[0] = NULL;
	    ndb->routecount = 1;
	    ndb->attrflags = 0;
	    
	    /*
	     * pdb may be gone. Use index for redistributing pdb.
	     */
	    ndb->pdbindex = work->c.red.pdbindex;
	    ndb->pdb_overmask = 0;
	}
	event = ROUTE_DOWN;
    }
    ipigrp2_rtchange(pdb, ndb, event);
}

/*
 * ipigrp2_process_backup
 * Process each element on the backup queue.
 */
static void ipigrp2_process_backup (pdbtype *pdb, ipigrp2_worktype *work)
{
    ddbtype *ddb;
    igrp2nettype dest;

    ddb = pdb->ddb;

    dest.ip_network.address = work->c.d.dest.address;
    dest.ip_network.mask = work->c.d.dest.mask;

    /*
     * Ask DUAL if route is still active and validate if available.
     */
    dual_validate_route(ddb, &dest);
}

/*
 * ipigrp2_process_lost
 * Process each element on the lost route queue.
 */
static void ipigrp2_process_lost (pdbtype *pdb, ipigrp2_worktype *work)
{
    ddbtype *ddb;
    igrp2nettype dest;

    /*
     * Inform DUAL that route has changed.
     */
    ddb = pdb->ddb;

    dest.ip_network.address = work->c.d.dest.address;
    dest.ip_network.mask = work->c.d.dest.mask;
    dual_lostroute(ddb, &dest);
}

/*
 * ipigrp2_get_summary_metric
 *
 * Examine all more specific routes in topology table for summary. Get
 * minimum metric. Returns TRUE if a more specific route was found.
 */
static boolean ipigrp2_get_summary_metric (ddbtype *ddb, summarytype *summary,
					   dual_newroute *route)
{
    dual_ndbtype *dndb, *save_dndb;
    int          index;
    ipaddrtype   addr;
    ulong        metric;

    dual_log_all(ddb, DUALEV_GETSUM, &summary->address, &summary->mask);
    summary->being_recalculated = FALSE;

    /*
     * Find minimum metric by traversing all more specific
     * routes for this summary address.
     */
    dndb = NULL;
    save_dndb = NULL;
    index = 0;

    /*
     * A min metric of infinity means there are no specific routes for
     * the summary in the topology table.
     */
    summary->min_metric = METRIC_INACCESSIBLE;

    /*
     * Scan topology table.
     */
    while ((dndb = dual_table_walk(ddb, dndb, &index))) {
	if (!dndb->dndb_successors)
	    continue;

	/*
	 * Don't consider the metric of other summaries when choosing a metric
	 * for this summary.
	 */
	if (dndb->dndb_rdb &&
	    (dndb->dndb_rdb->drdb_origin == IGRP2_ORG_SUMMARY)) {
	    continue;
	}
	/*
	 * Ignore the summary itself, regardless of who owns it.
	 */
	if (dndb->dndb_dest.ip_network.address == summary->address &&
	    dndb->dndb_dest.ip_network.mask == summary->mask) {
	    continue;
	}

	addr = dndb->dndb_dest.ip_network.address & summary->mask;
	if (addr == summary->address) {
	    if (dndb->dndb_metric == METRIC_INACCESSIBLE) {
		/*
		 * dndb must be in the process of being deleted.  Ignore.
		 */
		continue;
	    }
	    metric = igrp2_computemetric(dndb->dndb_vecmetric.bandwidth,
					 dndb->dndb_vecmetric.load,
					 dndb->dndb_vecmetric.delay,
					 dndb->dndb_vecmetric.reliability,
					 ddb->k1, ddb->k2, ddb->k3, ddb->k4,
					 ddb->k5);
	    if (metric < summary->min_metric) {
		summary->min_metric = metric;
		save_dndb = dndb;
	    }
	    if (ipigrp2_debug_sum) {
		buginf("\nIP-EIGRP: get_summary_metric: %i%m",
		       summary->address, summary->mask);
	    }
	}
    }

    dual_log_all(ddb, DUALEV_GETSUM2, &summary->min_metric, NULL);

    if (!save_dndb)
	return(FALSE);

    /*
     * Save vector metric in ddb for caller.
     */
    route->new_vecmetric = save_dndb->dndb_vecmetric;
    route->new_metric = summary->min_metric;
    return(TRUE);
}

/*
 * ipigrp2_process_ifdown
 *
 * Process an ifdown event.  Tear down all neighbors, and delete the IIDB
 * if so directed.
 */
static void ipigrp2_process_ifdown (pdbtype *pdb, ipigrp2_worktype *work)
{
    ddbtype *ddb;
    idbtype *idb;
    igrp2_idbtype *iidb;

    ddb = pdb->ddb;
    if (!ddb)
	return;
    idb = work->c.ifd.interface;
    if (!idb)
	return;
    iidb = igrp2_find_iidb(ddb, idb);
    if (!iidb)
	return;

    /* Tear down all peers. */

    igrp2_take_nbrs_down(ddb, idb, TRUE, "interface down");

    /*
     * If we're deleting the IIDB, turn off multicast receive and clean
     * up any autosummaries.
     */
    if (iidb->iidb_going_down) {
	ipigrp2_setup_multicast(ddb, idb, FALSE);
	igrp2_deallocate_iidb(ddb, iidb);
	ipigrp2_build_auto_summaries(pdb, idb);
    }
}

/*
 * ipigrp2_process_summary_entry
 * Process a summary from the work queue.
 */
static void ipigrp2_process_summary_entry (pdbtype *pdb,
					   ipigrp2_worktype *work)
{
    ddbtype      *ddb;
    dual_ndbtype *dndb;
    boolean add;
    igrp2metric *metric;
    summarytype *summary;
    ipaddrtype address, mask;
    dual_newroute route;
    dual_rtev_type event;

    bzero(&route, sizeof(route));

    address = work->c.sum.dest.address;
    mask = work->c.sum.dest.mask;

    ddb = pdb->ddb;
    route.new_dest.ip_network.address = address;
    route.new_dest.ip_network.mask = mask;
    add = work->c.sum.add;
    route.new_iidb = ddb->nulliidb;
    metric = work->c.sum.metric;

    dual_log_all(ddb, DUALEV_PROCSUM, &address, &mask);
    dual_log_all(ddb, DUALEV_PROCSUM2, &add, &metric);

    /*
     * Deleting an entry that is not present.
     */
    dndb = dual_ndblookup(ddb, &route.new_dest);
    if (!add && !dndb) {
	if (metric) {
	    free(metric);
	}
	return;
    }

    event = (add) ? DUAL_RTUP : DUAL_RTDOWN;
    route.new_origin = IGRP2_ORG_SUMMARY;

    /*
     * Use metric in workq entry, if caller supplied them.  Otherwise, scan
     * topology table to find minimum metric.
     */
    if (metric == NULL) {

	/*
	 * Find summary, if it exists.
	 */
	for (summary = pdb->summaryQ.qhead; summary; 
	     summary = summary->next) {
	    if (summary->address == address && summary->mask == mask) {
		break;
	    }
	}

	if (!summary || !ipigrp2_get_summary_metric(ddb, summary, &route)) {

	    /*
	     * Don't delete an entry that doesn't exist or is not our
	     * summary.
	     */
	    if (!dndb ||
		(dndb->dndb_rdb &&
		 dndb->dndb_rdb->drdb_origin != IGRP2_ORG_SUMMARY)) {
		return;
	    }

	    /*
	     * No more specific routes exist, or the summary is being deleted.
	     * If summary is in topology table, we must get rid of it.
	     */
	    route.new_metric = METRIC_INACCESSIBLE;
	    route.new_vecmetric.delay = METRIC_INACCESSIBLE;
	    event = DUAL_RTDOWN;
	}
    } else {
	route.new_vecmetric = *metric;
	free(metric);
	route.new_metric = igrp2_computemetric(route.new_vecmetric.bandwidth,
					       route.new_vecmetric.load,
					       route.new_vecmetric.delay,
					       route.new_vecmetric.reliability,
					       ddb->k1, ddb->k2,
					       ddb->k3, ddb->k4, ddb->k5);
    }

    /*
     * Inform DUAL about summary route.
     */
    route.new_opaque_flags = 0;
    route.new_external = FALSE;
    if (ipigrp2_is_exterior(address, mask)) {
	route.new_opaque_flags = IGRP2_CD;
    }
    dual_rtchange(ddb, &route, event);
}

/*
 * ipigrp2_process_passive_intf
 *
 * Process a "passive-interface" command.
 *
 * If we are switching states, we need to tear everything down and then
 * rebuild it in the other state.
 */
static void ipigrp2_process_passive_intf (pdbtype *pdb, ipigrp2_worktype *work)
{
    ddbtype *ddb;
    idbtype *idb;
    igrp2_idbtype *iidb;

    ddb = pdb->ddb;
    if (!ddb)
	return;
    idb = work->c.pasv.interface;
    if (!idb)
	return;
    iidb = igrp2_find_iidb(ddb, idb);

    /*
     * If there's no IIDB, it means that we haven't configured anything on
     * this interface yet.  We'll wait, and when the appropriate commands
     * are issued, the passive interface will be configured.
     */
    if (!iidb)
	return;

    /* See if we're in the right state.  If so, bail. */

    if (iidb->passive_interface == work->c.pasv.sense)
	return;

    /* OK, we're switching states.  Go do it. */

    igrp2_passive_interface(ddb, iidb, work->c.pasv.sense);

    /* Update any autosummaries for this interface. */

    ipigrp2_build_auto_summaries(pdb, idb);
}

/*
 * ipigrp2_process_conn_change
 * Process an interface or connected route change event.
 * If an address was supplied, only change that particular address for
 * the interface.
 */
static void ipigrp2_process_conn_change (pdbtype *pdb, ipigrp2_worktype *work)
{
    ddbtype *ddb;
    igrp2_idbtype *iidb;
    idbtype *interface;
    dual_rtev_type event;
    dual_newroute route;
    ndbtype *ndb;
    ipaddrtype address, mask;
    boolean adding;
    boolean config;

    address = work->c.con.dest.address;
    mask = work->c.con.dest.mask;
    adding = work->c.con.up;
    config = work->c.con.config;
    
    bzero(&route, sizeof(route));

    /*
     * Setup protocol specific parameters for call.
     */
    ddb = pdb->ddb;
    interface = work->c.con.interface;
    iidb = igrp2_find_iidb(ddb, interface);
    if (!iidb)
	return;

    /* If this change is due to configuration, rebuild autosummaries. */

    if (pdb->auto_summary && config) {
	ipigrp2_build_auto_summaries(pdb, interface);
    }

    route.new_iidb = iidb;
    route.new_vecmetric.mtu = interface->ip_mtu;
    route.new_dest.ip_network.address = address;
    route.new_dest.ip_network.mask = mask;
    route.new_opaque_flags = 0;
    if (adding) {
	event = DUAL_RTUP;
	ndb = ip_rtlookup(route.new_dest.ip_network.address,
			  route.new_dest.ip_network.mask);
	if (!ndb)
	    return;
	if (!ip_pdb_on_ndb(ndb, pdb) || ip_own_ndb(ndb, pdb))
	    return;
	if (ipigrp2_is_exterior(route.new_dest.ip_network.address,
				route.new_dest.ip_network.mask)) {
	    route.new_opaque_flags = IGRP2_CD;
	}
    } else {
	event = DUAL_RTDOWN;
    }

    dual_conn_rtchange(ddb, &route, event);

    /* Adjust the corresponding summaries. */

    if (adding && config) {

	/* Make sure route already exists so that we don't add it twice. */

	ndb = ip_rtlookup(address & mask, mask);
	if (!ndb)
	    return;
	if (!ip_pdb_on_ndb(ndb, pdb))
	    return;
    }
    ipigrp2_conn_summary_depend(pdb->ddb, interface, address, mask, adding);
}

/*
 * ipigrp2_process_workq
 *
 * Process the work queue.
 *
 * Returns TRUE if we bailed because we hit our limiter.
 */
#define WORKQ_LIMITER 20		/* Max 20 per pass */
static boolean ipigrp2_process_workq (pdbtype *pdb)
{
    ipigrp2_worktype *work;
    ulong workq_limiter;

    workq_limiter = WORKQ_LIMITER;
    while (!process_is_queue_empty(pdb->workQ)) {

	/* Bail if we should go do something else. */

	workq_limiter--;
	if (!workq_limiter)
	    return(TRUE);

	process_may_suspend();

	work = process_dequeue(pdb->workQ);

	switch (work->type) {
	  case IGRP2_WORK_REDIST:
	  case IGRP2_WORK_UNREDIST:
	    ipigrp2_process_redist(pdb, work);
	    break;
	  case IGRP2_WORK_IFDOWN:
	    ipigrp2_process_ifdown(pdb, work);
	    break;
	  case IGRP2_WORK_LOST:
	    ipigrp2_process_lost(pdb, work);
	    break;
	  case IGRP2_WORK_BACKUP:
	    ipigrp2_process_backup(pdb, work);
	    break;
	  case IGRP2_WORK_CONNSTATE:
	    ipigrp2_process_conn_change(pdb, work);
	    break;
	  case IGRP2_WORK_REDISTALL:
	    ipigrp2_process_redistribute(pdb, work);
	    break;
	  case IGRP2_WORK_SUMMARY:
	    ipigrp2_process_summary_entry(pdb, work);
	    break;
	  case IGRP2_WORK_PASSIVEINTF:
	    ipigrp2_process_passive_intf(pdb, work);
	    break;
	  case IGRP2_WORK_TABLERELOAD:
	    ipigrp2_process_reload_iptable(pdb);
	    break;
	  case IGRP2_WORK_INTEXTSTATE:
	    ipigrp2_process_exterior_change(pdb, work);
	    break;
	  case IGRP2_WORK_SENDSAP:	/* Not used in IP */
	    break;			/* do nothing */
	}
	chunk_free(pdb->ddb->workq_chunk, work);
	dual_finish_update_send(pdb->ddb);
    }
    return(FALSE);
}

/*
 * ipigrp2_defaultmetric_command
 *
 * The "default-metric" command was entered for an IP-IGRP2 process.
 * Go through all protocols that are redistributed into IP-IGRP2, add/delete
 * their routes in the topology table.
 */
static void ipigrp2_defaultmetric_command (pdbtype *pdb, boolean sense)
{

    pdbtype *dist;

    for (dist = protoQ.qhead; dist; dist = dist->next) {

	/*
	 * Don't redistribute ourselves.  And, only redistribute that 
	 * which the user requested.
	 */
	if (pdb == dist)
	    continue;
	if (!(pdb->distflag & dist->mask))
	    continue;

	if (!sense) {

	    /*
	     * Don't turn off redistribution for igrp simply because
	     * the user typed 'no default-metric'.  Also, if it is
	     * static simply call redistribute with sense set to TRUE
	     * to change the metric.
	     */
	    if (dist->proctype & (PROC_IGRP|PROC_IGRP2))
		continue;
	    if (dist->proctype == PROC_STATIC) {
		ipigrp2_redistribute(pdb, dist, TRUE);
		continue;
	    }
	}

	ipigrp2_redistribute(pdb, dist, sense);
    }
}

/*
 * ipigrp2_set_distflags
 *
 * This function is called when either the IGRP1 or IGRP2 pdb is created.
 * It sets the distflags so IGRP1 and IGRP2 from the same AS can support
 * auto-redistribution.
 */
static void ipigrp2_set_distflags (pdbtype *pdb)
{

    pdbtype *pdb2;

    for (pdb2 = protoQ.qhead; pdb2; pdb2 = pdb2->next) {
	if (pdb->asystem != pdb2->asystem)
	    continue;
	if (pdb2->proctype & (PROC_IGRP | PROC_IGRP2)) {
	    pdb->distflag |= pdb2->mask;
	    pdb2->distflag |= pdb->mask;
	    break;
	}
    }
}

/*
 * Process IP-IGRP2 router subcommands. Right now it only supports:
 *
 * [no] eigrp logging
 * [no] eigrp event-log-size <nn>
 */
void ipigrp2_commands (parseinfo *csb)
{
    ddbtype       *ddb;
    pdbtype       *pdb;

    pdb = (pdbtype *) csb->protocol;
    if (!pdb)
	return;
    ddb = pdb->ddb;
    if (!ddb)
	return;

    /*
     * NV write all interface subcommands for IGRP2 related pdbs.
     */
    if (csb->nvgen) {
	switch (csb->which) {
	  case IPIGRP2_EVENT_LOGGING:
	    nv_write(ddb->log_events, "%s", csb->nv_command);
	    break;

	  case IPIGRP2_EVENT_LOG_SIZE:
	    nv_write(ddb->max_events != DUAL_DEFAULT_MAX_EVENTS, "%s %d",
		     csb->nv_command, ddb->max_events);
	    break;

	  case IPIGRP2_LOG:
	    dual_configure_logging(csb, ddb, FALSE, FALSE, FALSE);
	    break;

	  case IPIGRP2_KILL_EVERYONE:
	    nv_write(ddb->kill_everyone, csb->nv_command);
	    break;

	  case IPIGRP2_NBR_LOGGING:
	    nv_write(ddb->log_adj_change != IGRP2_DEFAULT_LOG_ADJ_CHANGE,
		     "%s%s", ddb->log_adj_change ? "" : "no ",
		     csb->nv_command);
	    break;

 	default:
	    break;
	}
	return;
    }

    switch (csb->which) {
      case IPIGRP2_EVENT_LOGGING:
	ddb->log_events = csb->sense;
	break;

      case IPIGRP2_EVENT_LOG_SIZE:
	if (!dual_init_events(ddb, GETOBJ(int,1), csb->sense))
	    printf(nomemory);
	break;

      case IPIGRP2_LOG:
	dual_configure_logging(csb, ddb, GETOBJ(int,1), GETOBJ(int,2),
			       GETOBJ(int,3));
	break;

      case IPIGRP2_KILL_EVERYONE:
	ddb->kill_everyone = csb->sense;
	break;

      case IPIGRP2_NBR_LOGGING:
	if (csb->set_to_default)
	    ddb->log_adj_change = IGRP2_DEFAULT_LOG_ADJ_CHANGE;
	else
	    ddb->log_adj_change = csb->sense;
	break;

    default:
	break;
    }
}

/*
 * ipigrp2_summary_address_command
 *
 * Process the following interface subcommand:
 *
 * [no] ip summary-address eigrp <as-number> <address> <mask>
 */
static void ipigrp2_summary_address_command (pdbtype *pdb, idbtype *idb,
					     parseinfo *csb)
{

    ipaddrtype  address;
    ipaddrtype  mask;
    ddbtype *ddb;

    if (GETOBJ(paddr,1)->type != ADDR_IP ||
	GETOBJ(paddr,2)->type != ADDR_IP) {
	printf("\nSummary address and mask must be in IP dotted "
	       "decimal format");
	return;
    }

    ddb = pdb->ddb;
    if (!ddb)
	return;

    address = GETOBJ(paddr,1)->ip_addr;
    mask = GETOBJ(paddr,2)->ip_addr;

    ipigrp2_configure_summary(pdb, address, mask, idb, csb->sense, TRUE);

    /*
     * If we're deleting a summary and autosummarization is in effect, try
     * adding back the autosummary.
     */
    if (csb->sense == FALSE && pdb->auto_summary) {
	ipigrp2_build_auto_summaries(pdb, idb);
    }

    if (idb) {
	igrp2_take_nbrs_down(ddb, idb, FALSE, "summary configured");
    } else {
	igrp2_take_all_nbrs_down(ddb, "summary configured");
    }
}

/*
 * ipigrp2_if_commands
 *
 * Process IP-IGRP2 interface subcommands. Right now it only supports:
 *
 * [no] ip hello-interval eigrp <as-number> <value>
 * [no] ip hold-time eigrp <as-number> <value>
 * [no] ip split-horizon eigrp <as-number> <value>
 * [no] ip summary-address eigrp <as-number> <address> <mask>
 */
void ipigrp2_if_commands (parseinfo *csb)
{

    ddbtype       *ddb;
    pdbtype       *pdb;
    idbtype       *idb;
    igrp2_idbtype *iidb = NULL;
    int           pdb_type;
    summarytype   *summary;
    sum_idbtype	  *sum_idb;
    ulong interval;

    if (!(idb = csb->interface)) return;

    /*
     * NV write all interface subcommands for IGRP2 related pdbs.
     */
    if (csb->nvgen) {
	for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	    if (pdb->proctype != PROC_IGRP2)
		continue;
	    ddb = pdb->ddb;
	    if ((csb->which != IPIGRP2_SUMMARY) &&
		!(iidb = igrp2_find_iidb(ddb, idb))) {
		if (!(iidb = igrp2_find_temp_idb(ddb, idb))) {
		    continue;
		}
	    }

	    switch (csb->which) {
	      case IPIGRP2_HELLO:
		nv_write((iidb->hello_interval !=
			  igrp2_def_helloint(idb, ddb->linktype)),
			 "%s %s %d %d", csb->nv_command, pdb->name, 
			 pdb->asystem, iidb->hello_interval/ONESEC);
		break;
	      case IPIGRP2_BW_PCT:
		nv_write((iidb->igrp2_bandwidth_percentage !=
			  IGRP2_DEFAULT_BANDWIDTH_PERCENT), "%s %s %d %d",
			 csb->nv_command, pdb->name, pdb->asystem,
			 iidb->igrp2_bandwidth_percentage);
		break;
	      case IPIGRP2_HOLDTIME:
		nv_write((iidb->holdtime !=
			  igrp2_def_holdint(idb, ddb->linktype)),
			 "%s %s %d %d", csb->nv_command, pdb->name, 
			 pdb->asystem, iidb->holdtime/ONESEC);
		break;
	      case IPIGRP2_SPLIT:
		if (ddb->def_split_horizon)
		    nv_write(!iidb->split_horizon, "no %s %s %d",
			     csb->nv_command, pdb->name, pdb->asystem);
		else
		    nv_write(iidb->split_horizon, "%s %s %d", csb->nv_command,
			     pdb->name, pdb->asystem);
		break;
	      case IPIGRP2_SUMMARY:
		for (summary = pdb->summaryQ.qhead; summary; 
		     summary = summary->next) {
		    sum_idb = ipigrp2_find_sum_idb(summary, idb);
		    if (!sum_idb)
			continue;
		    nv_write(sum_idb->configured, "%s %s %d %i %i", 
			     csb->nv_command, pdb->name, pdb->asystem,
			     summary->address, summary->mask);
		}
		break;
	    }
	}
	return;
    }

    /*
     * This command only supported by IGRP2.
     */
    if (!partialmatch("eigrp", (char *) csb->router_name.myname)) {
	printf("\nInvalid routing process");
	return;
    }

    /*
     * Get pdb from parse block.
     */
    pdb_type = PDB_IP;
    pdb = router_parse(csb, FALSE, &pdb_type);
    if (!pdb)
	return;

    /*
     * Get DUAL descriptor block.
     */
    if (!(ddb = pdb->ddb)) return;

    switch (csb->which) {
      case IPIGRP2_HELLO:
	if (csb->sense) {
	    interval = GETOBJ(int,1)*ONESEC;
	} else {
	    interval = igrp2_def_helloint(idb, ddb->linktype);
	}
	igrp2_set_hello_interval(ddb, idb, interval);
	break;
      case IPIGRP2_BW_PCT:
	igrp2_set_bandwidth_percent(ddb, idb, GETOBJ(int,1));
	break;
      case IPIGRP2_HOLDTIME:
	if (csb->sense) {
	    interval = GETOBJ(int,1)*ONESEC;
	} else {
	    interval = igrp2_def_holdint(idb, ddb->linktype);
	}
	igrp2_set_holdtime(ddb, idb, interval);
	break;
      case IPIGRP2_SPLIT:
	if (csb->set_to_default)
	    csb->sense = ddb->def_split_horizon;
	igrp2_set_split_horizon(ddb, idb, csb->sense);
	break;
      case IPIGRP2_SUMMARY:
	ipigrp2_summary_address_command(pdb, idb, csb);
	break;
    }
}

/*
 * ipigrp2_callbackup_routes
 *
 * Called when a backup route is candidate for promotion to the primary
 * route. This occurs when better administratively distance route has
 * gone unreachable.
 */
static void ipigrp2_callbackup_routes (pdbtype *pdb, ipaddrtype address, 
				       ipaddrtype mask, backuptype *backup)
{
    ipigrp2_worktype *work;

    if (!ipigrp2_callback_sanity(pdb))
	return;
    if (ipigrp2_debug_not) {
	buginf("\nIP-EIGRP: Callback: callbackup_routes %i%m",
	       address, mask);
    }

    work = chunk_malloc(pdb->ddb->workq_chunk);
    if (work == NULL) {
	return;
    }

    work->c.d.dest.address = address;
    work->c.d.dest.mask = mask;
    ipigrp2_qevent(pdb, work, IGRP2_WORK_BACKUP);
}

/*
 * ipigrp2_lostroute
 *
 * The IP routing table has informed us that an IGRP2 route has been
 * overwritten by another routing protocol. We need to delete it from
 * the topology table so we are synced up.
 */
static void ipigrp2_lostroute (pdbtype *pdb, ndbtype *ndb)
{
    ipigrp2_worktype *work;

    if (!ipigrp2_callback_sanity(pdb))
	return;
    if (ipigrp2_debug_not) {
	buginf("\nIP-EIGRP: Callback: lostroute %i%m", ndb->netnumber,
	       ndb->netsmask);
    }

    work = chunk_malloc(pdb->ddb->workq_chunk);
    if (work == NULL) {
	return;
    }

    work->c.d.dest.address = ndb->netnumber;
    work->c.d.dest.mask = ndb->netsmask;
    ipigrp2_qevent(pdb, work, IGRP2_WORK_LOST);
}

/*
 * ipigrp2_show_event
 *
 * show ip eigrp event
 */
void ipigrp2_show_event (parseinfo *csb)
{
    
    pdbtype *pdb;
    ddbtype *ddb;
    ulong   asystem;

    asystem = GETOBJ(int,1);

    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (!(ddb = pdb->ddb)) 
	    continue;
	if (!pdb->running) 
	    continue;
	if (asystem && pdb->asystem != asystem) 
	    continue;
	printf("\nEvent information for AS %d:", pdb->asystem);
	dual_show_events(ddb, csb->which, GETOBJ(int,2), GETOBJ(int,3));
    }
}

/*
 * ipigrp2_show_traffic
 *
 * Show traffic for all or given process.
 */
void ipigrp2_show_traffic (parseinfo *csb)
{
    
    pdbtype *pdb;
    ddbtype *ddb;
    ulong   asystem;

    asystem = GETOBJ(int,1);

    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (!(ddb = pdb->ddb))
	    continue;
	if (!pdb->running)
	    continue;
	if (asystem && pdb->asystem != asystem)
	    continue;

	printf("\nIP-EIGRP Traffic Statistics for process %d", pdb->asystem);
	igrp2_show_traffic(ddb);
	if (asystem)
	    break;
    }
}

static const char ipigrp2_timers_banner[] = 
    {"\nIP-EIGRP timers for process %d"};

/*
 * ipigrp2_show_timers
 *
 * Show timers for all or given process.
 */
void ipigrp2_show_timers (parseinfo *csb)
{
    
    pdbtype *pdb;
    ddbtype *ddb;
    ulong   asystem;

    asystem = GETOBJ(int,1);

    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (!(ddb = pdb->ddb))
	    continue;
	if (!pdb->running)
	    continue;
	if (asystem && pdb->asystem != asystem)
	    continue;

	printf(ipigrp2_timers_banner, pdb->asystem);
	igrp2_show_timers(ddb);
	if (asystem)
	    break;
    }
}

/*
 * ipigrp2_metric_command
 *
 * Store K-values in ddb.
 */
static void ipigrp2_metric_command (pdbtype *pdb)
{

    ddbtype *ddb;

    ddb = pdb->ddb;
    ddb->k1 = pdb->igrp_k1;
    ddb->k2 = pdb->igrp_k2;
    ddb->k3 = pdb->igrp_k3;
    ddb->k4 = pdb->igrp_k4;
    ddb->k5 = pdb->igrp_k5;
}

/*
 * ipigrp2_accept_peer
 *
 * Don't process hello from neighbor if not on the same subnet as received
 * interface. 
 * Return an indication of whether or not the peer was accepted.  FALSE
 * means peer was not accepted.
 */
static boolean ipigrp2_accept_peer (ddbtype *ddb, igrp2addrtype *source,
				    igrp2_idbtype *iidb)
{
    idbtype    *idb;

    idb = iidb->idb;

    if (ip_onifsubnet(idb, source->ip_address))
	return(TRUE);

    if (ipigrp2_debug) {
	buginf("\nIP-EIGRP: Neighbor %i not on common subnet for %s",
	       source->ip_address, idb->namestring);
    }
    return(FALSE);
}

/*
 * ipigrp2_advertise_request
 *
 * Called by DUAL to ask if a destination should be advertised out
 * the supplied interface. IP-IGRP2 will check filter and summary (aggregation)
 * data structures to determine if the destination is eligible for interface.
 */
static advertise_request_type ipigrp2_advertise_request (ddbtype *ddb,
							 igrp2nettype *dest,
							 idbtype *interface)
{

    pdbtype     *pdb;
    summarytype *summary;
    ipaddrtype  addr, mask;
    advertise_request_type advertise;
    boolean     debug;
    char	*debug_string;

    pdb = ddb->pdb;
    addr = dest->ip_network.address;
    mask = dest->ip_network.mask;
    debug = IPIGRP2_DTARGET(addr,ddb);

    if (debug)
	buginf("\nIP-EIGRP: %i%m, ", addr, mask);

    /*
     * Check outbound interface filters.
     */
    if (!pdb_outfilter(pdb, interface, NULL, addr)) {
	return(SUPPRESS_ROUTE);
    }

    /*
     * Check summary addresses.
     */
    advertise = ADVERTISE_ROUTE;
    for (summary = pdb->summaryQ.qhead; summary; 
	 summary = summary->next) {

	/*
	 * This is a summary address destination.  Advertise it on all
	 * interfaces if there are no component subnets to the summary;
	 * otherwise, advertise it on the specified interfaces and
	 * poison it on the others (since we may have advertised it
	 * previously).
	 */
	if (summary->address == addr && summary->mask == mask) {
	    if (summary->min_metric == METRIC_INACCESSIBLE) { /* Unused */
		advertise = ADVERTISE_ROUTE;
	    } else {
		if (ipigrp2_find_sum_idb(summary, interface) != NULL) {
		    advertise = ADVERTISE_ROUTE;
		} else {
		    advertise = POISON_ROUTE;
		}
	    }
	    break;
	}

	/*
	 * This address is a more specific address of a configured summary. 
	 * Don't advertise it if we are advertising the summary out this
	 * interface.  Keep looping, there might be a more specific
	 * summary but blow out of here as soon as a summary is found for this
	 * address.
	 */
	if (summary->address == (addr & summary->mask)) {
	    if (ipigrp2_find_sum_idb(summary, interface) != NULL) {
		advertise = SUPPRESS_ROUTE;
		break;
	    }
	}
    }

    if (debug) {
	debug_string = "do";		/* Satisfy the compiler. */
	switch (advertise) {
	  case ADVERTISE_ROUTE:
	    debug_string = "do";
	    break;
	  case SUPPRESS_ROUTE:
	    debug_string = "don't";
	    break;
	  case POISON_ROUTE:
	    debug_string = "poison";
	    break;
	}
	buginf("- %s advertise out %s", debug_string, interface->namestring);
    }
    return(advertise);
}

/*
 * ipigrp2_show_summary_entry
 * Show summarization information for a particular summary table entry.
 */
static void ipigrp2_show_summary_entry (summarytype *summary, boolean *banner,
					boolean configured, char *banner_msg)
{
    sum_idbtype *sum_idb, *sum_next;
    int count;

    count = 0;
    mem_lock(summary);
    for (sum_idb = summary->idbQ.qhead; sum_idb; sum_idb = sum_next) {
	mem_lock(sum_idb);
	sum_next = sum_idb->next;
	if (sum_idb->configured == configured) { /* Found one! */
	    if (!*banner) {
		*banner = TRUE;
		printf(banner_msg);
	    }
	    if (count) {
		if ((count % 3) == 0) {
		    printf("\n     ");
		} else {
		    printf(",");
		}
	    } else {
		printf("\n    %i%m for", summary->address, summary->mask);
	    }
	    count++;
	    printf(" %s", sum_idb->idb->namestring);
	}
	free(sum_idb);
    }
    if (count && summary->min_metric != METRIC_INACCESSIBLE) {
	printf("\n      Summarizing with metric %u", summary->min_metric);
    }
    free(summary);
}

/*
 * ipigrp2_show_auto_summary
 * Show IP-IGRP2 auto summary information.
 */
static void ipigrp2_show_auto_summary (pdbtype *pdb)
{
    summarytype *summary;
    boolean banner;

    banner = FALSE;
    for (summary = pdb->summaryQ.qhead; summary;
	summary = summary->next) {
	ipigrp2_show_summary_entry(summary, &banner, FALSE,
				   "\n  Automatic address summarization:");
    }
}

/*
 * ipigrp2_show_summary
 *
 * Show IP-IGRP2 summaries in "show ip protocols" display.
 */
static void ipigrp2_show_summary (pdbtype *pdb)
{

    summarytype *summary;
    boolean     banner;

    printf("\n  Automatic network summarization %s in effect",
	   (pdb->auto_summary) ? "is" : "is not");

    if (pdb->auto_summary)
	ipigrp2_show_auto_summary(pdb);
    banner = FALSE;
    for (summary = pdb->summaryQ.qhead; summary;
	summary = summary->next) {
	ipigrp2_show_summary_entry(summary, &banner, TRUE,
				   "\n  Address Summarization:");
    }
}

/*
 * ipigrp2_auto_summary_command
 *
 * Process the configuration of the auto-summary command.  This is called
 * after the setting of pdb->auto_summary is changed.
 */
static void ipigrp2_auto_summary_command (pdbtype *pdb)
{
    ddbtype *ddb;
    igrp2_idbtype *iidb;
    summarytype *summary, *next_sum;
    sum_idbtype *sum_idb, *next_sum_idb;

    ddb = pdb->ddb;
    if (!ddb)
	return;

    if (pdb->auto_summary) {
	
	 /* Turning on autosummaries.  Build them for each interface. */

	for (iidb = ddb->iidbQ.qhead; iidb; iidb = iidb->next) {
	    ipigrp2_build_auto_summaries(pdb, iidb->idb);
	}

    } else {

	/* Turning off autosummaries.  Delete them all. */

	for (summary = pdb->summaryQ.qhead; summary; summary = next_sum) {
	    next_sum = summary->next;
	    for (sum_idb = summary->idbQ.qhead; sum_idb;
		 sum_idb = next_sum_idb) {
		next_sum_idb = sum_idb->next;
		if (!sum_idb->configured) {
		    ipigrp2_delete_summary_interface(pdb, summary, sum_idb);
		}
	    }
	}
    }
    igrp2_take_all_nbrs_down(ddb, "summary configured");
}

/*
 * ipigrp2_encap_fixup
 * set igrp2 timers to proper values after an encapsulation change
 */ 
static void ipigrp2_encap_fixup (idbtype *swidb, int old_encap)
{
    pdbtype *pdb;
    ddbtype *ddb;
    igrp2_idbtype *iidb;

    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype != PROC_IGRP2)
	    continue;
	if (!pdb->running)
	    continue;
	ddb = pdb->ddb;
	iidb = igrp2_find_iidb(ddb, swidb);
	if (iidb == NULL)
	    continue;
	if (!igrp2_default_timers (iidb))
	    continue;
	iidb->hello_interval = igrp2_def_helloint(swidb, ddb->linktype);
	iidb->holdtime = igrp2_def_holdint(swidb, ddb->linktype);
    }
}

/*
 * ipigrp2_clear_event_logging
 *
 * Stop event logging.
 */
void ipigrp2_clear_event_logging (parseinfo *csb)
{
    ulong as_number;
    pdbtype *pdb;
    ddbtype *ddb;

    as_number = GETOBJ(int,1);

    /*
     * Scan through all IGRP2 pdbs.
     */
    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype != PROC_IGRP2)
	    continue;
	ddb = pdb->ddb;
	if (!ddb)
	    continue;
	if (as_number && (ddb->asystem != as_number))
	    continue;
	dual_clear_event_logging(ddb);
    }
}

/*
 * ipigrp2_clear_log
 *
 * Clear out the EIGRP event log (and restart logging if it has been
 * stopped).
 */
void ipigrp2_clear_log (parseinfo *csb)
{
    ulong as_number;
    pdbtype *pdb;
    ddbtype *ddb;

    as_number = GETOBJ(int,1);

    /*
     * Scan through all IGRP2 pdbs.
     */
    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype != PROC_IGRP2)
	    continue;
	ddb = pdb->ddb;
	if (!ddb)
	    continue;
	if (as_number && (ddb->asystem != as_number))
	    continue;
	dual_clear_log(ddb);
    }
}

/*
 * ipigrp2_clear_neighbors
 *
 * Process exec command:
 * 	clear ip eigrp [<as #>] neighbors [<ip-address> | <interface>]
 */
void ipigrp2_clear_neighbors (parseinfo *csb)
{

    idbtype       *idb;
    pdbtype       *pdb;
    ddbtype       *ddb;
    igrp2_idbtype *iidb;
    dual_peertype *peer;
    ipaddrtype    addr;
    igrp2addrtype address;
    ulong	  as_number;

    addr = GETOBJ(paddr,1)->ip_addr;
    idb = GETOBJ(idb,1);
    as_number = GETOBJ(int,1);

    /*
     * Scan through all IGRP2 pdbs.
     */
    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype != PROC_IGRP2)
	    continue;
	ddb = pdb->ddb;
	if (!ddb)
	    continue;
	if (as_number && (ddb->asystem != as_number))
	    continue;

	if (!idb && !addr) {

	    /*
	     * Take all neigbhors down.	
	     */
	    for (iidb = ddb->iidbQ.qhead; iidb; 
		 iidb = iidb->next) {
		igrp2_take_nbrs_down(ddb, iidb->idb, FALSE,
				     "manually cleared");
	    }
	} else if (idb) {

	    /*
	     * Take neighbors down on a specific interface.
	     */
	    igrp2_take_nbrs_down(ddb, idb, FALSE, "manually cleared");
	} else {

	    /*
	     * Take a single neighbor down.
	     */
	    idb = samecable(addr);
	    if (!idb)
		continue;
	    address.ip_address = addr;
	    peer = igrp2_find_peer(ddb, &address, idb);
	    if (!peer)
		continue;
	    igrp2_take_peer_down(ddb, peer, FALSE, "manually cleared");
	}
    }
}
/*
 * ipigrp2_find_ddb
 *
 * Find the DDB given the AS number
 */
static ddbtype *ipigrp2_find_ddb (int asnum)
{
    ddbtype *ddb;
    pdbtype *pdb;

    ddb = NULL;
    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if ((pdb->proctype == PROC_IGRP2) && (pdb->asystem == asnum)) {
	    ddb = pdb->ddb;
	    break;
	}
    }
    return(ddb);
}

/*
 * ipigrp2_test_command
 * This isn't really a command, it is a helper routine to find the ddb
 * and fill in the ip address for the generic test command.
 * OBJ(int,1) has the type
 * OBJ(int,2) has the as number.
 * OBJ(paddr,1) has the ipaddress.
 * OBJ(paddr,2) has the optional mask.
 */
void ipigrp2_test_command (parseinfo *csb)
{
    int as;
    ipaddrtype address, mask;
    ddbtype *ddb;

    as = GETOBJ(int,2);
    if (as == 0) {
	printf("\nNo as number");
	return;
    }

    ddb = ipigrp2_find_ddb(as);
    if (ddb == NULL) {
	printf("\nCouldn't find process");
	return;
    }

    address = GETOBJ(paddr,1)->ip_addr;
    mask = GETOBJ(paddr,2)->ip_addr;
    if (mask == 0L)
	mask = get_majormask(address);
#ifdef notdef
    dual_test_command(ddb, GETOBJ(int,1));
#endif
}

/*
 * ipigrp2_debug_command
 * Parse an ip address and mask for the fsm debug command
 * OBJ(int,1) AS number
 * OBJ(paddr,1) ip address
 * OBJ(paddr,2) optional mask
 */
void ipigrp2_debug_command (parseinfo *csb)
{
    int as;
    ipaddrtype address, mask;
    ddbtype *ddb;
    pdbtype *pdb;

    (const) SETOBJ(pdb,1) = iprouting_debug_arr;
    as = GETOBJ(int,1);
    ddb = NULL;
    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if ((pdb->proctype == PROC_IGRP2) && (pdb->asystem == as)) {
	    ddb = pdb->ddb;
	    break;
	}
    }
    if (ddb == NULL) {
	printf("\nCouldn't find process");
	return;
    }
    if (!(ipigrp2_debug || igrp2_packet_debug)) {
	printf("\nFirst enable IP-EIGRP Route Events or EIGRP packet debug");
	return;
    }
    if (csb->sense) {
	address = GETOBJ(paddr,1)->ip_addr;
	if (address == 0L) {
	    printf("\nNo IP address specified");
	    return;
	}
	igrp2_set_debug_ddb(ddb);
	switch (csb->which) {
	case DEBUG_IPIGRP2_AS_TARGET:
	    mask = GETOBJ(paddr,2)->ip_addr;
	    if (mask == 0L)
		mask = get_majormask(address);
	    ddb->debug_target.ip_network.address = address;
	    ddb->debug_target.ip_network.mask = mask;
	    ddb->debug_target_set = TRUE;
	    printf("\nIP Target enabled on AS %d for %i%m", as,
		   address, mask);
	    break;
	case DEBUG_IPIGRP2_NEI_TARGET:
	    ddb->debug_peer.ip_address = address;
	    ddb->debug_peer_set = TRUE;
	    printf("\nIP Neighbor target enabled on AS %d for %i", as,
		   ddb->debug_peer.ip_address);
	    break;
	}
    } else {
	switch (csb->which) {
	case DEBUG_IPIGRP2_AS_TARGET:
	    ddb->debug_target_set = FALSE;
	    printf("\nIP Target disabled on AS %d", as);
	    break;
	case DEBUG_IPIGRP2_NEI_TARGET:
	    ddb->debug_peer_set = FALSE;
	    printf("\nIP Neighbor target disabled on AS %d", as);
	    break;
	}
	igrp2_set_debug_ddb(NULL);
    }
    debug_command(csb);
}

static void ipigrp2_debug_all (boolean sense)
{
    ddbtype *ddb;
    pdbtype *pdb;

    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (!(pdb->proctype & PROC_IGRP2)) 
	    continue;
	if (pdb->ddb == NULL)
	    continue;
	ddb = pdb->ddb;
	if (!sense) {
	    ddb->debug_target_set = FALSE;
	    ddb->debug_peer_set = FALSE;
	}
    }
}	
/*
 * ipigrp2_distance
 * IP-IGRP2 uses better distance for internal routes and
 * worse distance for external routes.  This helps avoid redistribution loops.
 */

static int ipigrp2_distance (pdbtype *pdb, ipaddrtype target,
			     ushort route_type, int flags, int *pdb_peerindex)
{
    int distance;
    
    if (route_type == RTYPE_IGRP2_INT) {
	distance = pdb->distance;
    } else {
	distance = pdb->distance2;
	*pdb_peerindex = 1;
    }
    if (flags & NET_SUMMARY) 
	distance = IGRP2_SUMMARY_DISTANCE;
    return(distance);
}

/*
 * ipigrp2_distcmp
 * Compare administrative distances.
 * Return an indication of whether or not 'our' distance is better than
 * 'their' distance.  Normally, this is the traditional distance check.
 * But, if IGRP<->EIGRP auto-redistribution is in effect and the EIGRP
 * route is external, consider route metrics instead.
 */

static enum DISTTYPE ipigrp2_distcmp (pdbtype *pdb, uchar our_dist,
				      ndbtype *db, ulong metric,
				      ushort route_type)
{
    pdbtype *their_pdb;

    their_pdb = INDEX2PDB(db->pdbindex);
    if (their_pdb && (their_pdb->proctype & PROC_IGRP) &&
	their_pdb->asystem == pdb->asystem) {
	/*
	 * Same AS as us, so automatic redistribution is occuring.
	 * If comparing IGRP route against external EIGRP route,
	 * compare metrics and ignore distance.
	 * Don't dork with IGRP routes as long as they are in holddown,
	 * let holddown do its thing.
	 */
	if ((route_type == RTYPE_IGRP2_EXT) &&
	    !TIMER_RUNNING(db->blindtimer)) {
	    /*
	     * Shift EIGRP metric for comparison purposes
	     */
	    metric >>= 8;
	    if (metric <= db->metric)
		return(IP_DIST_BETTER);
	    return(IP_DIST_WORSE);
	}
    }
    return(iprouting_distcmp_inline(our_dist, db->distance));
}

/*
 * ipigrp2_exterior_bit
 * Check the exterior bit in a drdb, which may be in one of 2 different
 * places, depending upon the route type.
 */

static inline uchar ipigrp2_exterior_bit (dual_rdbtype *drdb)
{
    ipigrp2_extdatatype *ext_info;

    if (drdb->drdb_data) {
	ext_info = drdb->drdb_data;
	return(ext_info->flag & IGRP2_CD);
    } else {
	return(drdb->drdb_opaque_flags);
    }
}

/*
 * ipigrp2_exterior_flag
 * Scan the feasible succesors, looking for an exterior bit which is set.
 * Return the bit if found, or 0 if no flag values are set.
 */
static uchar ipigrp2_exterior_flag (dual_ndbtype *dndb)
{
    dual_rdbtype *drdb;
    int successors;

    successors = dndb->dndb_successors;
    for (drdb = dndb->dndb_rdb; drdb && (successors-- > 0);
	 drdb = drdb->drdb_next) {
	if (ipigrp2_exterior_bit(drdb)) {
	    return(IGRP2_CD);
	}
    }
    return(0);
}

/*
 * ipigrp2_exteriors_differ
 * Compare exterior bit of the feasible successors.  Return TRUE if there
 * is disagreement between the feasible successors, FALSE otherwise.
 * Return the bit in 'flag', if found.  Return zero in 'flag' if
 * drdb == NULL, or there are no feasible successors.
 */

static boolean ipigrp2_exteriors_differ (dual_ndbtype *dndb, uchar *flag)
{
    dual_rdbtype *drdb;
    int successors;
    uchar new_flag;

    drdb = dndb->dndb_rdb;
    successors = dndb->dndb_successors;
    if ((drdb == NULL) || (successors == 0)) {
	*flag = 0;
	return(FALSE);
    }
    *flag = ipigrp2_exterior_bit(drdb);
    if (successors-- < 2) {
	return(FALSE);
    }
    for (drdb = drdb->drdb_next; drdb && (successors-- > 0);
	 drdb = drdb->drdb_next) {
	new_flag = ipigrp2_exterior_bit(drdb);
	if (*flag == new_flag)
	    continue;
	if (!(*flag))
	    *flag = new_flag;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * ipigrp2_exterior_check
 * See if the exterior bit has changed state, and also update the routing
 * table if necessary.
 */

static void ipigrp2_exterior_check (ddbtype *ddb, dual_ndbtype *dndb,
				    uchar *flag)
{
    if (!ipigrp2_exteriors_differ(dndb, flag))
	return;
    /*
     * Feasible successors in topology table disagree on interior/exterior
     * state.  Make sure that the entry in the routing table reflects the
     * intended sense for the candidate default bit.
     */
    ipigrp2_candidate_update(ddb, dndb);
}

/*
 * ipigrp2_compare_external
 *
 * Protocol-dependent check to compare EXTERIOR_TYPE data structure
 *
 * Returns TRUE if identical, FALSE if not.
 */
static boolean ipigrp2_compare_external (void *data1, void *data2)
{
    if (data1 == NULL && data2 == NULL)
	return(TRUE);
    if (data1 == NULL || data2 == NULL)
	return(FALSE);

    return(!bcmp(data1, data2, sizeof(ipigrp2_extdatatype)));
}

/*
 * ipigrp2_transport_ready
 *
 * Called when the transport is ready.
 *
 * peer is non-null if a peer has finished with the packet, or null if an
 * interface has finished with the packet.
 *
 * The if the refcount in the packet descriptor is zero, the packet is
 * completely acknowledged.
 *
 * We don't actually care about any of this;  we just try to send packets
 * if things are flow-ready.
 */
static void ipigrp2_transport_ready (ddbtype *ddb,
				     igrp2_idbtype *iidb,
				     igrp2_pakdesc *pakdesc,
				     dual_peertype *peer,
				     boolean others_pending)
{
    /* Bail if someone else wants to send;  we defer until it's quiet. */

    if (others_pending)
	return;

    /* If we're sending peer probes and this peer is flow-ready, send one. */

    if (peer && peer->send_probe_sequence && igrp2_peer_is_flow_ready(peer)) {
	pakdesc = igrp2_enqueue(ddb, peer, iidb, TRUE);
	if (pakdesc) {
	    pakdesc->opcode = IGRP_OPC_PROBE;
	    pakdesc->start_serno = peer->send_probe_sequence;
	    pakdesc->end_serno = peer->send_probe_sequence;
	    peer->send_probe_sequence++;
	    igrp2_debug_enqueued_pkt(ddb, peer, iidb, pakdesc);
	}
    }

    /*
     * If we're sending interface probes and this interface is flow-ready,
     * send one.
     */
    if (iidb->send_probe_sequence && igrp2_interface_is_flow_ready(iidb)) {
	pakdesc = igrp2_enqueue(ddb, NULL, iidb, TRUE);
	if (pakdesc) {
	    pakdesc->opcode = IGRP_OPC_PROBE;
	    pakdesc->start_serno = iidb->send_probe_sequence;
	    pakdesc->end_serno = iidb->send_probe_sequence;
	    iidb->send_probe_sequence++;
	    igrp2_debug_enqueued_pkt(ddb, peer, iidb, pakdesc);
	}
    }
}

/*
 * ipigrp2_build_packet
 *
 * Build a packet, given a pakdesc.
 *
 * Returns a pointer to the packet header, or NULL.
 *
 * This routine is called by the transport when it comes across a packet
 * that was enqueued earlier by igrp2_enqueue.  It is now time to transmit
 * that packet, so we are being called to build the packet with up-to-date
 * data.
 */
static igrp2type *ipigrp2_build_packet (ddbtype *ddb, dual_peertype *peer,
					igrp2_pakdesc *pakdesc,
					boolean *suppressed_packet)
{
    igrp2_probe_type *probe;
    igrp2type *igrp2;

    /* Select based on opcode. */

    switch (pakdesc->opcode) {
      case IGRP_OPC_PROBE:
	igrp2 = igrp2_getbuf(IGRP2_PROBE_SIZE);
	if (igrp2) {
	    probe = igrp2_packet_data(igrp2);
	    PUTLONG(&probe->probe_sequence, pakdesc->start_serno);
	}
	pakdesc->length = IGRP2_PROBE_SIZE;
	break;
      default:
	igrp2 = NULL;
	break;
    }
    return(igrp2);
}

/*
 * ipigrp2_item_size
 *
 * Return the size of the packet item that would be necessary to carry
 * the supplied route.  We return the worst-case value, since the
 * item may change from internal to external between packetization
 * and transmission.
 */
static ulong ipigrp2_item_size (dual_ndbtype *dndb)
{
    ulong dest_size, item_size;

    dest_size = 1 + ip_bytesinmask(dndb->dndb_dest.ip_network.mask);
    item_size = max(sizeof(ipigrp2_externtype), sizeof(ipigrp2_metrictype))
	+ dest_size;
    return(item_size);
}

/*
 * ipigrp2_add_item
 *
 * Build an item from the supplied DNDB and add it to the packet.
 *
 * Returns the length of the item, or 0 if there wasn't enough room to
 * add the item.
 *
 * Note that we do not put multiple destinations into a TLV any more.
 * This could still be done as an optimization to make the packets smaller.
 * Maybe Mikel will do this.
 */
static ulong ipigrp2_add_item (ddbtype *ddb, igrp2_idbtype *iidb,
			       dual_ndbtype *dndb, void *packet_ptr,
			       ulong bytes_left, boolean unreachable)
{
    ulong item_length;
    dual_rdbtype *drdb;
    ulong dest_size;
    boolean external;
    ipigrp2_metrictype *internal_route;
    ipigrp2_externtype *external_route;
    ipigrp2_mpdecode *dest;
    ulong origin;
    ulong flags;

    dest_size = 1 + ip_bytesinmask(dndb->dndb_dest.ip_network.mask);
    flags = 0;
    external = igrp2_route_is_external(dndb);

    /* Bail if there's no room for the item.  (There better be!) */

    if (external) {
	item_length = sizeof(ipigrp2_externtype) + dest_size;
    } else {
	item_length = sizeof(ipigrp2_metrictype) + dest_size;
    }
    if (item_length > bytes_left)
	return(0);			/* Uhoh! */

    drdb = dndb->dndb_rdb;
    if (IPIGRP2_DTARGET(dndb->dndb_dest.ip_network.address,ddb)) {
	buginf("\nIP-EIGRP: %s %s metric %u - %u %u",
	       (external ? "Ext" : "Int"),
	       ipigrp2_printnet(&dndb->dndb_dest), dndb->dndb_metric,
	       dndb->dndb_vecmetric.bandwidth,
	       dndb->dndb_vecmetric.delay);
    }
    
    /* Build the item depending on the type. */

    if (external) {			/* External route */
	external_route = packet_ptr;
	dest = external_route->ext_dest;
	PUTSHORT(&external_route->type, IPIGRP2_EXTERNAL);
	PUTSHORT(&external_route->length, item_length);
	PUTLONG(&external_route->nexthop, 0);
	if (drdb) {			/* It has to be there, but we check */
	    bcopy(drdb->drdb_data, &external_route->routerid,
		  sizeof(ipigrp2_extdatatype));
	} else {			/* Uhoh */
	    errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	    return(0);
	}

	/*
	 * If route is marked as a candidate default, consider
	 * performing output filtering on it.
	 */
	if (external_route->flag & IGRP2_CD) {
	    if (!ipigrp2_exteriors_out(ddb->pdb,
				       dndb->dndb_dest.ip_network.address)) {
		external_route->flag &= ~IGRP2_CD;
	    }
	}
	igrp2_metric_encode(ddb, dndb, iidb, 
			    (igrp2_mdecode *)&external_route->delay, 
			    unreachable, FALSE);

    } else {				/* Internal route */

	/*
	 * If route is marked as a candidate default, consider
	 * performing output filtering on it.
	 */
	internal_route = packet_ptr;
	dest = internal_route->int_dest;
	if (drdb) {
	    flags = drdb->drdb_opaque_flags;
	    origin = drdb->drdb_origin;
	} else {
	    flags = 0;
	    origin = IGRP2_ORG_IGRP2;
	}
	if (flags & IGRP2_CD) {
	    if (!ipigrp2_exteriors_out(ddb->pdb,
				       dndb->dndb_dest.ip_network.address))
		flags &= ~IGRP2_CD;
	}
	PUTSHORT(&internal_route->type, IPIGRP2_METRIC);
	PUTSHORT(&internal_route->length, item_length);
	PUTLONG(&internal_route->nexthop, 0);
	igrp2_metric_encode(ddb, dndb, iidb, 
			    (igrp2_mdecode *)&internal_route->delay, 
			    unreachable,(origin == IGRP2_ORG_CONNECTED));
	IPIGRP2_METRICTYPEFLAGS(&internal_route->delay) = flags;
    }
    
    dest->mask = ip_bitsinmask(dndb->dndb_dest.ip_network.mask);
    bcopy(&dndb->dndb_dest.ip_network.address, dest->number, dest_size - 1);
    return(item_length);
}

/*
 * ipigrp2_error_cmd
 *
 * test ip eigrp <as> error-percent <n>
 *
 * OBJ(int,2) = AS number
 * OBJ(int,3) = error percentage
 * OBJ(idb,1) = interface, or NULL
 */
void ipigrp2_error_cmd (parseinfo *csb)
{
#ifdef IGRP2_INDUCE_ERRORS		/* Lots of code if we allow it. */
    ddbtype *ddb;
    igrp2_idbtype *iidb;
    ulong error_rate;

    error_rate = GETOBJ(int,3) * 128 / 100;

    /* Get the DDB. */

    ddb = ipigrp2_find_ddb(GETOBJ(int,2));
    if (ddb == NULL) {
	printf("\nCouldn't find process");
	return;
    }

    /* If there's an IDB specified, use that one. */

    if (GETOBJ(idb,1)) {
	iidb = igrp2_find_iidb(ddb, GETOBJ(idb,1));
	if (iidb) {
	    iidb->iidb_error_rate = error_rate;
	} else {
	    printf("\nEIGRP not running on this interface.");
	}
    } else {

	/* No IDB specified;  set it on all of them. */

	for (iidb = ddb->iidbQ.qhead; iidb; iidb = iidb->next) {
	    iidb->iidb_error_rate = error_rate;
	}
    }

#else					/* IGRP2_INDUCE_ERRORS */

    printf("\nCommand not supported.");

#endif					/* IGRP2_INDUCE_ERRORS */
}

/*
 * ipigrp2_probe_command
 *
 * Send/stop sending probes
 *
 * test ip eigrp <as> probe {<interface> | <IP address>} [stop]
 *
 * OBJ(int,2) = AS number
 * OBJ(idb,1) = interface, if specified
 * OBJ(paddr,1) = address, if specified
 * OBJ(int,3) = TRUE if "stop" selected
 */
void ipigrp2_probe_command (parseinfo *csb)
{
    ddbtype *ddb;
    igrp2_idbtype *iidb;
    idbtype *idb;
    ipaddrtype ip_address;
    igrp2addrtype address;
    dual_peertype *peer;

    /* Get the DDB. */

    ddb = ipigrp2_find_ddb(GETOBJ(int,2));
    if (ddb == NULL) {
	printf("\nCouldn't find process");
	return;
    }

    /* Find the IIDB. */

    idb = GETOBJ(idb,1);
    if (idb) {				/* It's the interface form */
	iidb = igrp2_find_iidb(ddb, idb);
	if (!iidb) {
	    printf("\nCouldn't find interface");
	    return;
	}

	/* Ask the mainline code to do it. */

	igrp2_probe_interface(ddb, iidb, GETOBJ(int,3));

    } else {				/* Must be the IP address form */

	ip_address = GETOBJ(paddr,1)->ip_addr;
	idb = samecable(ip_address);
	peer = NULL;
	if (idb) {
	    address.ip_address = ip_address;
	    peer = igrp2_find_peer(ddb, &address, idb);
	}
	if (!peer) {
	    printf("\nCouldn't find peer");
	    return;
	}

	/* Ask the mainline code to do it. */

	igrp2_probe_peer(ddb, peer, GETOBJ(int,3));
    }
}

/*
 * ipigrp2_enable_process_wait
 *
 * Enable process blocking predicate for Hello process
 *
 * The process is set up to wait on the IP socket.
 */
static void ipigrp2_enable_process_wait (ddbtype *ddb)
{
    pdbtype *pdb;

    pdb = ddb->pdb;
    if (!pdb)
	return;
    process_watch_queue(pdb->socket->inq, ENABLE, RECURRING);
}

/*
 * ipigrp2_input_packet
 *
 * Returns the next input packet, or NULL if it is not there.
 * Returns the source of the packet as well.
 */
static paktype *ipigrp2_input_packet (ddbtype *ddb, igrp2addrtype *source)
{
    pdbtype *pdb;
    paktype *pak;
    iphdrtype *ip;
    ulong dummy;
    ipsocktype *soc;

    pdb = ddb->pdb;
    if (!pdb)
	return(NULL);
    soc = pdb->socket;

    pak = read_ipsocket(soc, FALSE, 0, &dummy, &dummy);
    if (pak) {
	ip = (iphdrtype *)ipheadstart(pak);
	source->ip_address = ip->srcadr;
	ipigrp2_traffic.inputs++;
    }
    return(pak);
}

/*
 * ipigrp2_router
 * Main IGRP2 routing process.  One per major network.
 */

#define PACKET_LIMITER 20		/* Max packets per pass */
static process ipigrp2_router (void)
{
    ipsocktype *soc;
    paktype *pak;
    igrp2_input_packet *inpak;
    iphdrtype *ip;
    igrp2type *igrp2;
    ddbtype *ddb;
    dual_peertype *peer;
    ulong packet_limiter;
    pdbtype *pdb;
    ulong major, minor;
    boolean once_around;

    /* Initialization gorp. */

    if (!process_get_arg_ptr((void **) &pdb))
	process_kill(THIS_PROCESS);
    memory_new_owner(pdb);

    process_wait_on_system_init();
    soc = find_igrp2socket(pdb->asystem);
    if (soc)				/* if socket, already routing AS */
	pdb_killp(pdb, -1);
    soc = open_ipsocket(IPIGRP2_PROT, NULL, NULL, pdb->asystem, 0, 0);
    if (!soc) {
	errmsg(&msgsym(NOSOCKET, DUAL), pdb->asystem);
	pdb_killp(pdb, -1);
    }
    pdb->socket = soc;
    ddb = pdb->ddb;

    /* Wake up the hello process. */

    process_set_boolean(ddb->run_hello_process, TRUE);

    /* Set up the event predicates. */

    process_watch_queue(ddb->ddb_inputQ, ENABLE, RECURRING);
    process_watch_queue(pdb->workQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&ddb->ddb_master_timer, ENABLE);
    while (TRUE) {
	if (ddb->ddb_change_queue) {	/* Better be empty now! */
	    errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	}
	process_wait_for_event();

	/* Empty the event queue.  We process events in our own order. */

	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case TIMER_EVENT:
	      case QUEUE_EVENT:
	      case DIRECT_EVENT:
		break;
	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}

	/*
	 * Top of inner loop.  We need to bleed off all of the event-causing
	 * stuff before we go to sleep, even if we hit our limiters.  We
	 * exit this loop if we actually exhausted everything.
	 */
	do {
	    once_around = FALSE;

	    /*
	     * First work on the packet queue.
	     */
	    packet_limiter = PACKET_LIMITER;
	    while ((inpak = process_dequeue(ddb->ddb_inputQ))) {
		process_may_suspend();
		pak = inpak->packet;
		if (interface_up(inpak->if_input)) {
		    igrp2 = (igrp2type *) ipdatastart(pak);
		    ip = (iphdrtype *)ipheadstart(pak);
		    /*
		     * Process fixed header.
		     */
		    peer = igrp2_receive(ddb, pak, 
					 (igrp2addrtype *) &ip->srcadr, 
					 inpak->if_input);
		    
		    /*
		     * Switch on packet type.  Routines must check version
		     * number.
		     */
		    if (peer && !inpak->if_input->static_routing) {
			switch (igrp2->opcode) {
			  case IGRP_OPC_UPDATE:
			  case IGRP_OPC_QUERY:
			  case IGRP_OPC_REPLY:
			    ipigrp2_receive(ddb, peer, pak, igrp2);
			    break;
			    
			  case IGRP_OPC_PROBE:
			    ipigrp2_receive_probe(ddb, peer, pak, igrp2);
			    break;
			    
			  default:
			    break;
			}
		    }
		}
		
		/*
		 * Dispose of processed datagram.
		 */
		datagram_done(pak);
		chunk_free(igrp2_inpak_chunks, inpak);
		
		/* Do something else if we've done lots of packets. */
		
		packet_limiter--;
		if (!packet_limiter) {
		    once_around = TRUE;
		    break;
		}
	    }

	    /* Process the work queue. */
	    
	    if (!process_is_queue_empty(pdb->workQ))
		once_around = once_around || ipigrp2_process_workq(pdb);
	    
	    /* Process any timer expirations. */
	    
	    if (mgd_timer_expired(&ddb->ddb_master_timer))
		once_around = once_around || igrp2_process_managed_timers(ddb);
	    
	} while (once_around);
    }
}

/*
 * ipigrp2_pdm_timer_expiry
 *
 * Handle the expiration of the PDM timer.  For us, this means that it's
 * time to run the ager.
 */
static void ipigrp2_pdm_timer_expiry (ddbtype *ddb, mgd_timer *timer)
{
    pdbtype *pdb;
    mgd_timer_stop(timer);
    pdb = ddb->pdb;
    age_all_routes(pdb);
}

/*
 * ipigrp2_route_adjust
 *
 * Called when an interface has changed state. Inform the DUAL finite state
 * machine about the change.
 */
static void ipigrp2_route_adjust (pdbtype *pdb, idbtype *idb)
{
    ipaddrtype address, mask;
    boolean unnumbered;

    if (ipigrp2_debug_not) {
	buginf("\nIP-EIGRP: Callback: route_adjust %s",
	       idb ? idb->namestring : "");
    }
    if (pdb->proctype != PROC_IGRP2)
	return;
    if (!igrp2_find_iidb(pdb->ddb, idb) &&
	!(idb->ip_passive_router & pdb->mask)) {
	return;
    }
    unnumbered = (idb->ip_unnumbered != NULL);
    if (unnumbered) {
	address = idb->ip_unnumbered->ip_address;
	mask = idb->ip_unnumbered->ip_nets_mask;
    } else {
	address = idb->ip_address;
	mask = idb->ip_nets_mask;
    }

    if (interface_up(idb)) {		/* Coming up */
	ipigrp2_adjust_connected(pdb, idb, TRUE, FALSE);
    } else {

	/* Going down.  Serialize the event;  we'll take care of it. */

	ipigrp2_ifdown(pdb, idb, FALSE);
    }
}

/*
 * ipigrp2_interface_change
 *
 * Process a change in metric on an interface.
 */
static void ipigrp2_interface_change (ddbtype *ddb, igrp2_idbtype *iidb)
{
    pdbtype *pdb;

    pdb = ddb->pdb;
    if (!pdb)
	return;
    ipigrp2_route_adjust(pdb, iidb->idb);
}

/*
 * ipigrp2_ddbinit
 * Initialize the ddb data structure.
 */
static boolean ipigrp2_ddbinit (pdbtype *pdb)
{
    ddbtype *ddb;
    
    ddb = malloc_named(sizeof(ddbtype), "IP DDB");
    if (ddb == NULL) {
	return(FALSE);
    }

    if (!dual_init_ddb(ddb, "IP-EIGRP Hello", sizeof(ipigrp2_extdatatype),
		       sizeof(ipigrp2_worktype))) {
	free(ddb);
	return(FALSE);
    }
    dual_allocate_free_lists();

    ddb->pdb = pdb;
    pdb->ddb = ddb;

    ddb->name = "IP-EIGRP";
    ddb->linktype = LINK_IP;
    ddb->k1 = pdb->igrp_k1;
    ddb->k2 = pdb->igrp_k2;
    ddb->k3 = pdb->igrp_k3;
    ddb->k4 = pdb->igrp_k4;
    ddb->k5 = pdb->igrp_k5;
    ddb->max_hopcount = IGRP_MAXHOPS;
    ddb->asystem = pdb->asystem;
    ddb->nulliidb = igrp2_create_iidb(pdb->ddb, nullidb);

    /*
     * Turn on split horizon by default
     */
    ddb->def_split_horizon = TRUE;
    ddb->active_time = IGRP2_DEF_ACTIVE_TIME;

    /* Set up the transfer vectors. */

    ddb->accept_peer	= ipigrp2_accept_peer;
    ddb->add_item	= ipigrp2_add_item;
    ddb->addrcopy	= ipigrp2_addrcopy;
    ddb->addrmatch	= ipigrp2_addrmatch;
    ddb->advertise_request = ipigrp2_advertise_request;
    ddb->buildpacket	= ipigrp2_build_packet;
    ddb->compare_external = ipigrp2_compare_external;
    ddb->enable_process_wait = ipigrp2_enable_process_wait;
    ddb->exterior_check	= ipigrp2_exterior_check;
    ddb->exterior_flag	= ipigrp2_exterior_flag;
    ddb->headerptr	= ipigrp2_headerptr;
    ddb->hellohook	= NULL;
    ddb->iidb_cleanup	= NULL;
    ddb->iidb_show_detail = NULL;
    ddb->input_packet	= ipigrp2_input_packet;
    ddb->interface_change = ipigrp2_interface_change;
    ddb->interface_up	= ipigrp2_interface_up;
    ddb->item_size	= ipigrp2_item_size;
    ddb->localaddr	= ipigrp2_localaddr;
    ddb->mmetric_fudge	= ipigrp2_mmetric_fudge;
    ddb->ndbbucket	= ipigrp2_ndbbucket;
    ddb->ndb_done	= NULL;
    ddb->ndb_new	= NULL;
    ddb->netmatch	= ipigrp2_netmatch;
    ddb->paklen		= ipigrp2_paklen;
    ddb->pdm_timer_expiry = ipigrp2_pdm_timer_expiry;
    ddb->peerbucket	= ipigrp2_peerbucket;
    ddb->peershowhook	= NULL;
    ddb->printaddr	= ipigrp2_printaddr;
    ddb->printnet	= ipigrp2_printnet;
    ddb->rtdelete	= ipigrp2_rtdelete;
    ddb->rtgetmetric	= ipigrp2_rtgetmetric;
    ddb->rtmodify       = NULL;
    ddb->rtupdate	= ipigrp2_rtupdate;
    ddb->sendpacket	= ipigrp2_write;
    ddb->transport_ready = ipigrp2_transport_ready;
    ddb->set_mtu        = ipigrp2_set_mtu;
    ddb->extract_tag    = ipigrp2_gettag;


    /* Other random gorp. */

    ipigrp2_set_routerid(ddb);
    ddb->log_events = FALSE;
    ddb->packet_overhead = IPHEADERBYTES(NOPT);

    /* Fire up the active timer. */

    mgd_timer_start(&ddb->ddb_active_timer, IGRP2_ACTIVE_SCAN_FREQUENCY);

    /* Enqueue and we're off. */

    enqueue(&ddbQ, ddb);
    return(TRUE);
}

/*
 * ipigrp2_showipproto
 * Show igprp2 specific information
 */

static void ipigrp2_showipproto (pdbtype *pdb)
{
    printf("\n  Default networks %sflagged in outgoing updates",
 	   pdb->exteriors_out ? "": "not ");
    if (pdb->exteriors_out_acl)
 	printf(", if matching access list %s",
	       pdb->exteriors_out_acl->name);
    printf("\n  Default networks %saccepted from incoming updates",
 	   pdb->exteriors_in ? "": "not ");
    if (pdb->exteriors_in_acl)
 	printf(", if matching access list %s",
	       pdb->exteriors_in_acl->name);
    printf("\n  EIGRP metric weight K1=%u, K2=%u, K3=%u, K4=%u, K5=%u",
 	   pdb->igrp_k1, pdb->igrp_k2, pdb->igrp_k3, pdb->igrp_k4,
 	   pdb->igrp_k5);
    printf("\n  EIGRP maximum hopcount %d", pdb->igrp_maxhops);
    printf("\n  EIGRP maximum metric variance %d", pdb->igrp_variance);
    if (pdb->defaultmetricflag) {
 	printf("\n  Default redistribution metric is ");
 	printf("%u %u %d %d %d", pdb->user_bandwidth,
 	       pdb->def_delay, pdb->def_reliability,
 	       pdb->def_loading, pdb->def_mtu);
     }
    showipproto_redistribute(pdb);
}

/*
 * ipigrp2_show_global_traffic
 * Show igprp2 traffic statistics
 */

static void ipigrp2_show_global_traffic (void)
{
    printf("\n\nIP-IGRP2 statistics:");
    printf("\n  Rcvd: %lu total", ipigrp2_traffic.inputs);
    printf("\n  Sent: %lu total", ipigrp2_traffic.outputs);
}

/*
 * ipigrp2_start_acl_timer
 *
 * (re)start the ACL change timer.  If the timer is already running, it is
 * restarted.  This means that as long as the user enters at least one
 * access list change every 10 seconds, the neighbors won't be cleared
 * until they're all done.
 */
static void ipigrp2_start_acl_timer (pdbtype *pdb, idbtype *idb)
{
    ddbtype *ddb;

    ddb = pdb->ddb;
    if (!ddb)
	return;

    igrp2_start_acl_timer(ddb, idb);
}

/*
 * ipigrp2_acl_changed
 *
 * Callback for when a route filter changed
 */
static void ipigrp2_acl_changed (pdbtype *pdb, idbtype *idb, boolean out,
				 boolean sense)
{
    /*
     * Simply kick the ACL change timer.  It will eventually clear the
     * neighbors on the appropriate interface(s).
     */
    ipigrp2_start_acl_timer(pdb, idb);
}

/*
 * ipigrp2_access_list_callback
 *
 * Callback whenever an access list is changed.
 *
 * If the access list applies to our route filters, we start the appropriate
 * timer to trigger a neighbor teardown in order to implement the filters.
 */
static void ipigrp2_access_list_callback (acl_headertype *acl)
{
    pdbtype *pdb;
    ddbtype *ddb;
    idbtype *idb;
    igrp2_idbtype *iidb;

    /* Look for all EIGRP PDBs. */

    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype == PROC_IGRP2) {

	    ddb = pdb->ddb;
	    if (ddb) {

		/* Check to see if this is one of our protocol-wide filters. */
		
		if (pdb->ofltr_all.acl == acl ||
		    pdb->ifltr_all.acl == acl) { 
		    ipigrp2_start_acl_timer(pdb, NULL);
		} else {
		    
		    for (iidb = ddb->iidbQ.qhead; iidb; iidb = iidb->next) {
			idb = iidb->idb;

			/* Try one of the interface filters. */

			if (idb->ip_iprotofltr[pdb->index] == acl ||
			    idb->ip_oprotofltr[pdb->index] == acl) {
			    ipigrp2_start_acl_timer(pdb, idb);
			}
		    }
		}
	    }
	}
    }
}

/*
 * ipigrp2_hopcount_changed
 *
 * Handle change in configured max hopcount.
 *
 * Update the DDB field.  Blast all of the adjacencies;  a brute force
 * way of ridding ourselves of now-unreachable destinations.  It happens
 * so seldom that we need not get fancy.
 */
static void ipigrp2_hopcount_changed (pdbtype *pdb, uchar hopcount)
{
    ddbtype *ddb;
    ddb = pdb->ddb;
    if (ddb) {
	ddb->max_hopcount = hopcount;
	igrp2_take_all_nbrs_down(ddb, "Max hopcount changed");
    }
}

/*
 * ipigrp2_pdb_init
 * Initialize the pdb data structure.
 */
static pdbtype *ipigrp2_pdb_init (ushort asystem)
{
    pdbtype *pdb;

    /*
     * Create the pdb.
     */
    pdb = malloc_named(sizeof(pdbtype), ip_pdb_name);
    if (!pdb) {
	return(NULL);
    }
    /*
     * Allocate an unused router process flag
     */
    if (allocate_pdbindex(pdb) == -1) {
	free(pdb);
	return(NULL);
    }	
    pdb->name = "eigrp";
    pdb->family = PDB_IP;
    pdb->pname = "IP-EIGRP Router";
    pdb->process = (processcbtype) ipigrp2_router;
    pdb->cleanup = ipigrp2_cleanup;
    pdb->majorupdate = NULL;
    pdb->is_feasible = simple_variance;
    pdb->holddown = ipigrp2_holddown;
    pdb->holdokay = FALSE;

    /*
     * This should probably be set to PROC_IGRP2 once the metrics are no 
     * longer directly comparable.  But for now, this is fine.
     */
    pdb->proctype = PROC_IGRP2;
    pdb->distflag = pdb->mask;
    pdb->redistallowed = PROC_REDISTMASK;
    pdb->redistribute = ipigrp2_redistribute;
    pdb->redist_callback = ipigrp2_redist_callback;
    pdb->add_route = ipigrp2_callbackup_routes;
    pdb->lost_route = ipigrp2_lostroute;
    pdb->query = ipigrp2_reload_iptable;
    pdb->showipproto = ipigrp2_showipproto;
    pdb->exterior_cb = ipigrp2_exterior_change;
    pdb->distance_cb = ipigrp2_distance;
    pdb->network_cb = ipigrp2_network_command;
    pdb->acl_changed = ipigrp2_acl_changed;
    pdb->hopcount_change = ipigrp2_hopcount_changed;
    pdb->metric_cmd = ipigrp2_metric_command;
    pdb->defmetric_cmd = ipigrp2_defaultmetric_command;
    pdb->route_adjust = ipigrp2_route_adjust;
    pdb->show_summary = ipigrp2_show_summary;
    pdb->auto_sum_cmd = ipigrp2_auto_summary_command;
    pdb->malloc_backup = FALSE;
    pdb->atomic_update = TRUE;
    pdb->route_ager = ipigrp2_ager;
    pdb->exteriors_out = TRUE;
    pdb->exteriors_in = TRUE;
    pdb->broadcasttime = pdb->def_broadcasttime = 0;
    pdb->invalidtime = pdb->def_invalidtime = 0;
    pdb->holdtime = pdb->def_holdtime = 0;
    pdb->flushtime = pdb->def_flushtime = 0;
    pdb->multipath = pdb->def_multipath = DEFMAXROUTES;
    pdb->distance = pdb->def_distance = IGRP2_DISTANCE;
    pdb->distance2 = pdb->def_distance2 = IGRP2_EX_DISTANCE;
    pdb->distance_cmp = ipigrp2_distcmp;
    pdb->igrp_variance = IGRP_DEFVARIANCE;
    pdb->asystem = asystem;
    pdb->igrp_k1 = pdb->igrp_k1_def = IGRP_K1_DEFAULT;
    pdb->igrp_k2 = pdb->igrp_k2_def = IGRP_K2_DEFAULT;
    pdb->igrp_k3 = pdb->igrp_k3_def = IGRP_K3_DEFAULT;
    pdb->igrp_k4 = pdb->igrp_k4_def = IGRP_K4_DEFAULT;
    pdb->igrp_k5 = pdb->igrp_k5_def = IGRP_K5_DEFAULT;
    pdb->igrp_maxhops = pdb->igrp_def_maxhops = IGRP_MAXHOPS;
    pdb->auto_summary = TRUE;
    TIMER_STOP(pdb->sleep_timer);

    if (!ipigrp2_ddbinit(pdb)) {
	free_pdbindex(pdb);
	free(pdb);
	return(NULL);
    }

    pdb->workQ = create_watched_queue("IP-EIGRP work queue", 0, 1);
    enqueue(&protoQ, pdb);
    return(pdb);
}

/*
 * ipigrp2_init
 */
static void ipigrp2_init (subsystype *subsys)
{
    memset(&ipigrp2_traffic, 0, sizeof(ipigrp2_traffic));

    /*
     * Register some functions
     */
    ip_set_proto_outcounter(IPIGRP2_PROT, &ipigrp2_traffic.outputs);
    reg_add_media_change_mtu(ipigrp2_mtu_change, "ipigrp2_mtu_change");
    reg_add_ip_enqueue(IPIGRP2_PROT, ipigrp2_process, "ipigrp2_process");
    reg_add_ip_show_traffic(ipigrp2_show_global_traffic,
			    "ipigrp2_show_global_traffic");
    reg_add_ip_address_change(ipigrp2_ip_address_command,
			      "ipigrp2_ip_address_command");
    reg_add_ip_passive_cmd_route_adjust(PROC_IGRP2, 
					ipigrp2_passive_interface_command,
					"ipigrp2_passive_interface_command");
    reg_add_iprouting_pdb_init(PROC_IGRP2, ipigrp2_pdb_init,
			       "ipigrp2_pdb_init");
    reg_add_encap_fixup(ipigrp2_encap_fixup, "ipigrp2_encap_fixup");
    reg_add_ip_access_list(ipigrp2_access_list_callback,
			   "ipigrp2_access_list_callback");
    reg_add_debug_all(ipigrp2_debug_all, "ipigrp2_debug_all");
    reg_add_ipigrp2_set_distflags(ipigrp2_set_distflags,
				  "ipigrp2_set_distflags");
    parser_add_command_list(ipigrp2_chain_init_table, "ipigrp2");
}

/*
 * IP EIGRP Routing subsystem header
 */

#define IPIGRP2_MAJVERSION 1
#define IPIGRP2_MINVERSION 0
#define IPIGRP2_EDITVERSION  1

SUBSYS_HEADER(ipigrp2, IPIGRP2_MAJVERSION, IPIGRP2_MINVERSION,
	      IPIGRP2_EDITVERSION, ipigrp2_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iprouting, eigrp", "req: iprouting");

