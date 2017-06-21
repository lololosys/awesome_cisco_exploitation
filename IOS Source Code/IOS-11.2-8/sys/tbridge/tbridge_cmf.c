/* $Id: tbridge_cmf.c,v 3.1.8.7 1996/09/10 22:35:09 lwei Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_cmf.c,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_cmf.c
 *
 * April 1996, Tony Speakman
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Constrained Multicast Flooding (CMF)
 *------------------------------------------------------------------
 * $Log: tbridge_cmf.c,v $
 * Revision 3.1.8.7  1996/09/10  22:35:09  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 * routers
 * Branch: California_branch
 *
 * Revision 3.1.8.6  1996/06/18  01:49:00  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.1.8.5  1996/05/14  04:30:15  speakman
 * CSCdi57584:  CGMP packets not seen by RSP
 * Branch: California_branch
 * Calculate network_start when copying RSP packets for flooding.
 *
 * Revision 3.1.8.4  1996/05/11  09:33:36  speakman
 * CSCdi57444:  CMF UI needs appropriate keyword
 * Branch: California_branch
 * Record the successor before freeing the predecessor.
 *
 * Revision 3.1.8.3  1996/05/11  07:34:03  speakman
 * CSCdi57444:  CMF UI needs appropriate keyword
 * Branch: California_branch
 * Change the show/clear keyword from 'cmf' to 'multicast'.
 * Extend 'clear' flexibility.
 *
 * Revision 3.1.8.2  1996/05/08  00:23:53  speakman
 * CSCdi56968:  CMF accounting for flooded packets is incorrect
 * Branch: California_branch
 *  - account for IP multicast rx packets in the SA's bte
 *  - show CMF rx and tx packet counts per-interface
 *  - constrain the initial IGMP report on a given interface
 *    to router ports only to prevent suppression and sequential
 *    joining behaviour
 *  - show IGMP timers only when CMF debugging is enabled
 *  - clear the multicast-router-port state when the multicast-group
 *    state is cleared
 *  - flood (unconstrained) IP multicast data packets in the absence of
 *    any multicast-router ports
 *
 * Revision 3.1.8.1  1996/05/04  05:08:51  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - New file(tbridge_bvi.c) for Bridge-group Virtual Interface
 * functions.
 * - Obsolete tbridge_crb_inline.h with tbridge_irb_inline.h.
 * - Bridge/route decision(tbridge_irb_inline.h).
 * - Learn routable packets(DLLI_IRBPKT) in bridge table.
 * - Move locate_bte_inline() and tbridge_get_arpa_type_inline() to
 *   tbridge_common_inline.h.
 * - Rip out MTR transparent bridging support.
 *
 * Constrained Multicast Flooding
 * - Add the tbridge monitor.
 * - Add the tbridge registry.
 * - Add the entire CMF subsystem.
 * - Convert tbridge flood array to a linked list of tbifd's.
 * - Add the call outs to CMF from the LES, RSP, and PAS forwarding
 *   functions.
 *
 * Revision 3.1  1996/04/21  23:06:46  speakman
 * Placeholders for the CMF subsystem and eventual partition of the
 * bridging registries.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "mgd_timers.h"
#include "access.h"
#include "../ip/ipaccess.h"

#include "subsys.h"

#include "ieee.h"
#include "../if/network.h"

#include "../ui/debug.h"

#include "../ipmulticast/mroute.h"
#include "../ipmulticast/igmp.h"
#include "../ipmulticast/cgmp.h"
#include "../ipmulticast/dvmrp.h"
#include "../ipmulticast/pim.h"
#include "../iprouting/route.h"
#include "../iprouting/ospf_pak.h"

#include "../srt/span.h"

#include "../tbridge/tbridge_debug.h"
#include "../tbridge/tbridge_public.h"
#include "../tbridge/tbridge_monitor.h"
#include "../tbridge/tbridge_cmf.h"
#include "../tbridge/parser_defs_tbridge_cmf.h"

#include "sys_registry.h"
#include "../h/fastswitch_registry.regh"
#include "../tbridge/tbridge_registry.regh"

extern void tbridge_cmf_parser_init(void);

static
boolean	tbridge_cmf_enable;

static
boolean	tbridge_cmf_cgmp_enable;



/******************************************************************************
 * Primitives to MANAGE interface descriptors and multicast group descriptors
 *****************************************************************************/

/*
 * DATA STRUCTURE ANNOTATION:
 *
 * TB interface descriptors (tbifd's) point at a swidb and contain
 * timers and attributes related to tbridging functions for that swidb.
 *
 * tbifd's live on an outgoing-interface queue (oifQ) off
 * TB mulitcast-group descriptors (tbmgd's).
 * 
 * tbmgd's in turn live on tbmgd queues pointed to by elements of
 * the bridge group's tbmgd queue hash table (span->mcast_groups[]).
 */

/*
 * Create a tbifd and enqueue it to the given queue.
 * The only element that is not simply zeroed is the
 * swidb to which the tbifd points.
 */
static tbifd_type *
tbridge_create_tbifd_in_q (idbtype *swidb, queuetype *tbifdQ)
{
    tbifd_type *tbifd;

    tbifd = malloc_named((sizeof(tbifd_type)), "TB IFD");
    if (tbifd != NULL) {
	/*
	 * Initialize and enqueue the IFD.
	 */
	tbifd->tbifd_swidb = swidb;
	mgd_timer_init_leaf(&tbifd->tbifd_igmp_qry_tmr, &tbm_igmp_qry_tmr,
			    TBM_IGMP_QRY_TIMER, tbifd, FALSE);
	mgd_timer_init_leaf(&tbifd->tbifd_igmp_rpt_tmr, &tbm_igmp_rpt_tmr,
			    TBM_IGMP_RPT_TIMER, tbifd, FALSE);
	mgd_timer_init_leaf(&tbifd->tbifd_mrouter_tmr, &tbm_mrouter_tmr,
			    TBM_MROUTER_TIMER, tbifd, FALSE);
	tbifd->tbifd_igmp_qry_expired_count = 0;
	tbifd->tbifd_igmp_rpt_expired_count = 0;
	tbifd->tbifd_mrouter_expired_count = 0;
	tbifd->tbifd_attributes = 0x0;
	tbifd->tbifd_rx_flood_packets = 0;
	tbifd->tbifd_tx_flood_packets = 0;
	enqueue(tbifdQ, tbifd);
    } else {
	return(NULL);
    }
    return(tbifd);
}

/*
 * Locate a tbifd in a queue by swidb.
 */
static tbifd_type *
tbridge_locate_tbifd_in_q (idbtype *swidb, queuetype *tbifdQ)
{
    tbifd_type *tbifd;

    for (tbifd = tbifdQ->qhead; tbifd != NULL; tbifd = tbifd->tbifd_next)
	if (tbifd->tbifd_swidb == swidb)
	    break;

    return(tbifd);
}

/*
 * Remove and delete a tbifd from a queue.
 */
static void
tbridge_delete_tbifd_in_q (queuetype *tbifdQ, tbifd_type *tbifd)
{
    p_unqueue(tbifdQ, tbifd);
    mgd_timer_stop(&tbifd->tbifd_igmp_qry_tmr);
    mgd_timer_stop(&tbifd->tbifd_igmp_rpt_tmr);
    mgd_timer_stop(&tbifd->tbifd_mrouter_tmr);
    free(tbifd);
}

/*
 * Clear a queue of tbifd's, stopping any timers
 * and freeing the tbifd's.
 */
static void
tbridge_clear_tbifdQ (queuetype *tbifdQ)
{
    while (tbifdQ->qhead != NULL)
	tbridge_delete_tbifd_in_q(tbifdQ, tbifdQ->qhead);
}

/*
 * Create a tbmgd and enqueue it to the given queue.
 * Record the IP address for this multicast group
 * in the descriptor.
 */
static tbmgd_type *
tbridge_create_tbmgd_in_q (ipaddrtype ipdstaddr, queuetype *tbmgdQ)
{
    tbmgd_type *tbmgd;

    tbmgd = malloc_named((sizeof(tbmgd_type)), "TB MGD");
    if (tbmgd != NULL) {
	/*
	 * Initialize and enqueue the MGD.
	 */
	queue_init(&tbmgd->tbmgd_oifQ, 0L);
	tbmgd->tbmgd_ipaddr = ipdstaddr;
	enqueue(tbmgdQ, tbmgd);
    } else {
	return(NULL);
    }
    return(tbmgd);
}

/*
 * Locate a tbmgd in a queue by IP address.
 */
static tbmgd_type *
tbridge_locate_tbmgd_in_q (ipaddrtype ipdstaddr, queuetype *tbmgdQ)
{
    tbmgd_type *tbmgd;

    for (tbmgd = tbmgdQ->qhead; tbmgd != NULL; tbmgd = tbmgd->tbmgd_next)
	if (tbmgd->tbmgd_ipaddr == ipdstaddr)
	    break;

    return(tbmgd);
}

/*
 * Remove and delete a tbmgd from a queue.
 */
static void
tbridge_delete_tbmgd_in_q (queuetype *tbmgdQ, tbmgd_type *tbmgd)
{
    p_unqueue(tbmgdQ, tbmgd);
    tbridge_clear_tbifdQ(&tbmgd->tbmgd_oifQ);
    free(tbmgd);
}

/*
 * Create a tbmgd queue and record its address in
 * the multicast group hash table.
 */
static queuetype *
tbridge_create_tbmgdQ_in_tbl (ipaddrtype ipdstaddr, queuetype **tbmgdQ_tbl)
{
    queuetype	*tbmgdQ;

    tbmgdQ = malloc_named((sizeof(queuetype)), "TB MGDQ");
    if (tbmgdQ != NULL) {
	/*
	 * Initialize the MGD queue and insert its
	 * address in the bridge group's hash table.
	 */
	queue_init(tbmgdQ, 0L);
	tbmgdQ_tbl[nethash(ipdstaddr)] = tbmgdQ;
    } else {
	return(NULL);
    }
    return(tbmgdQ);
}

/*
 * Clear a queue of tbmgd's, first clearing any OIF lists,
 * and then freeing the tbmgd's.
 */
static void
tbridge_clear_tbmgdQ (queuetype *tbmgdQ)
{
    tbmgd_type	*tbmgd;

    tbmgd = p_dequeue(tbmgdQ);
    while (tbmgd != NULL) {
	tbridge_clear_tbifdQ(&tbmgd->tbmgd_oifQ);
	free(tbmgd);
	tbmgd = p_dequeue(tbmgdQ);
    }
}


/******************************************************************************
 * Functions to DISPLAY multicast group information
 *****************************************************************************/

/*
 * Translate a port's attributes to ASCII characters.
 */
static void
tbridge_tbifd_attribute_string (char *string, ulong tbifd_attributes)
{
    int		ix;
    ulong	sw_ix;

    for (ix = 0; ix < TBIFD_ATTRIBUTE_SIZE; ix++) {
	sw_ix = (1 << ix);
	if (tbifd_attributes & sw_ix) {
	    switch (sw_ix) {
	    case TBIFD_FLOOD:
		sprintf(string++, "F");
		break;
	    case TBIFD_AUTO_FLOOD:
		sprintf(string++, "A");
		break;
	    case TBIFD_IPMCAST_GROUP:
		sprintf(string++, "G");
		break;
	    case TBIFD_IPMCAST_ROUTER:
		sprintf(string++, "R");
		break;
	    default:
		sprintf(string++, "?");
		break;
	    }
	}
    }
    *string = '\0';
}

/*
 * Translate a group's attributes to ASCII characters.
 */
static void
tbridge_tbmgd_attribute_string (char *string, ulong tbmgd_attributes)
{
    int		ix;
    ulong	sw_ix;

    for (ix = 0; ix < TBMGD_ATTRIBUTE_SIZE; ix++) {
	sw_ix = (1 << ix);
	if (tbmgd_attributes & sw_ix) {
	    switch (sw_ix) {
	    case TBMGD_IGMP_RPT_SUPPRESS:
		sprintf(string++, "S");
		break;
	    case TBMGD_IGMP_QRY_CURRENT:
		sprintf(string++, "Q");
		break;
	    default:
		sprintf(string++, "?");
		break;
	    }
	}
    }
    *string = '\0';
}

/*
 * Display a given group's ports' statistics, attributes, and timer statuses.
 */
static void
tbridge_show_group_minfo (tbmgd_type *tbmgd)
{
    tbifd_type	*tbifd;
    char	attribute_string[TBIFD_ATTRIBUTE_SIZE + 1];

    printf("\n\n  %16i %-20s %-11s",
	   tbmgd->tbmgd_ipaddr, "RX count", "TX count");

    if (tbridge_debug_cmf) {
	tbridge_tbmgd_attribute_string(attribute_string,
				       tbmgd->tbmgd_attributes);
	printf("   %s", attribute_string);
    }

    for (tbifd = tbmgd->tbmgd_oifQ.qhead; tbifd;
	 tbifd = tbifd->tbifd_next) {

	tbridge_tbifd_attribute_string(attribute_string,
				       tbifd->tbifd_attributes);
	attribute_string[TBIFD_ATTRIBUTE_SIZE] = '\0';
	printf("\n   %15s %8s %-11d %-11d",
	       tbifd->tbifd_swidb->namestring, attribute_string,
	       tbifd->tbifd_rx_flood_packets, tbifd->tbifd_tx_flood_packets);

	if (tbridge_debug_cmf) {
	    if (mgd_timer_running(&tbifd->tbifd_igmp_qry_tmr))
		printf("   Q tmr %d",
		       mgd_timer_left_sleeping(&tbifd->tbifd_igmp_qry_tmr)/
		       1000);
	    if (mgd_timer_running(&tbifd->tbifd_igmp_rpt_tmr))
		printf("   R tmr %d",
		       mgd_timer_left_sleeping(&tbifd->tbifd_igmp_rpt_tmr)/
		       1000);
	    if (mgd_timer_running(&tbifd->tbifd_mrouter_tmr))
		printf("   A tmr %d (!!!)",
		       mgd_timer_left_sleeping(&tbifd->tbifd_mrouter_tmr)/
		       1000);

	    if (tbifd->tbifd_igmp_qry_expired_count)
		printf("   Q exp %d", tbifd->tbifd_igmp_qry_expired_count);
	    if (tbifd->tbifd_igmp_rpt_expired_count)
		printf("   R exp %d", tbifd->tbifd_igmp_rpt_expired_count);
	    if (tbifd->tbifd_mrouter_expired_count)
		printf("   A exp %d (!!!)",
		       tbifd->tbifd_mrouter_expired_count);
	}
    }
}

/*
 * The guts of the 'show bridge multicast' command.
 */
void
tbridge_show_bridge_multicast_command (parseinfo *csb)
{
    spantype	*span;
    int		bridge_group, ix0, ix1, group_hash, jx0, jx1;
    boolean	show_groups, show_routers;
    ipaddrtype	group_addr;

    tbifd_type	*tbifd;
    tbmgd_type	*tbmgd;
    char	attribute_string[TBIFD_ATTRIBUTE_SIZE + 1];

    if (!tbridge_cmf_enable) {
	printf("\n\nIP multicasts are currently being flooded on all "
	       "forwarding ports.\n");
	return;
    }

    ix0 = ix1 = GETOBJ(int, 1);
    show_groups = GETOBJ(int, 2);
    show_routers = GETOBJ(int, 3);
    group_addr = GETOBJ(paddr,1)->ip_addr;

    /*
     * If no keywords are specified, show all state.
     */
    if (!show_groups && !show_routers)
	show_groups = show_routers = TRUE;

    /*
     * Derive the initial and final values for the range of bridge
     * groups to display: all of them or just a given one.
     */
    if (0 == ix0) {
	ix0 = 1;
	ix1 = SPAN_MAXINDEX - 1;
    }

    /*
     * Derive the initial and final values for the range of multicast
     * groups to display: all of them or just a given one.
     */
    if (0 == group_addr) {
	jx0 = 0;
	jx1 = NETHASHLEN - 1;
    } else {
	jx0 = jx1 = nethash(group_addr);
    }

    if (tbridge_cmf_cgmp_enable)
	printf("\n\n CGMP is enabled.");

    for (bridge_group = ix0; bridge_group <= ix1; bridge_group++) {

	if ((NULL == spanarray[bridge_group]))
	    continue;
	else
	    span = spanarray[bridge_group];

	/*
	 * Display the multicast router port summary.
	 */
	if (show_routers) {
	    if (span->mcast_router_ports.count > 0) {
		printf("\n\n Multicast router ports "
		       "for bridge group %d:", bridge_group);
		printf("\n\n  %d multicast router ports",
		       span->mcast_router_ports.count);
	    }

	    for (tbifd = span->mcast_router_ports.qhead; tbifd;
		 tbifd = tbifd->tbifd_next) {
		tbridge_tbifd_attribute_string(attribute_string,
					       tbifd->tbifd_attributes);
		attribute_string[TBIFD_ATTRIBUTE_SIZE] = '\0';
		printf("\n   %15s %8s", tbifd->tbifd_swidb->namestring,
		       attribute_string);

		if (tbridge_debug_cmf) {
		    if (mgd_timer_running(&tbifd->tbifd_igmp_qry_tmr))
			printf(" Q tmr %d (!!!)",
			mgd_timer_left_sleeping(&tbifd->tbifd_igmp_qry_tmr)/
			       1000);
		    if (mgd_timer_running(&tbifd->tbifd_igmp_rpt_tmr))
			printf(" R tmr %d (!!!)",
			mgd_timer_left_sleeping(&tbifd->tbifd_igmp_rpt_tmr)/
			       1000);
		    if (mgd_timer_running(&tbifd->tbifd_mrouter_tmr))
			printf(" A tmr %d",
			mgd_timer_left_sleeping(&tbifd->tbifd_mrouter_tmr)/
			       1000);

		    if (tbifd->tbifd_igmp_qry_expired_count)
			printf(" Q exp %d (!!!)",
			       tbifd->tbifd_igmp_qry_expired_count);
		    if (tbifd->tbifd_igmp_rpt_expired_count)
			printf(" R exp %d (!!!)",
			       tbifd->tbifd_igmp_rpt_expired_count);
		    if (tbifd->tbifd_mrouter_expired_count)
			printf(" A exp %d",
			       tbifd->tbifd_mrouter_expired_count);
		}
	    }
	}

	/*
	 * Display individual group port summaries.
	 */
	if (show_groups) {
	    printf("\n\n Multicast groups "
		   "for bridge group %d:", bridge_group);
	    for (group_hash = jx0; group_hash <= jx1; group_hash++) {

		if (NULL == span->mcast_groups[group_hash])
		    continue;

		if (group_addr) {

		    tbmgd = tbridge_locate_tbmgd_in_q(group_addr,
					span->mcast_groups[group_hash]);
		    if (NULL == tbmgd)
			continue;
		    tbridge_show_group_minfo(tbmgd);

		} else {

		    for (tbmgd = span->mcast_groups[group_hash]->qhead; tbmgd;
			 tbmgd = tbmgd->tbmgd_next)
			tbridge_show_group_minfo(tbmgd);
		}
	    }
	}
    }
}


/******************************************************************************
 * Functions to MANAGE multicast group information
 *****************************************************************************/

/*
 * The guts of the 'clear bridge multicast' command.
 */
void
tbridge_clear_bridge_multicast_command (parseinfo *csb)
{
    spantype	*span;
    int		bridge_group, ix0, ix1, group_hash, jx0, jx1;
    boolean	clear_groups, clear_routers, clear_counts;
    ipaddrtype	group_addr;
    idbtype	*swidb;

    queuetype	*tbmgdQ;
    tbifd_type	*tbifd;
    tbmgd_type	*tbmgd, *tbmgd_next;

    ix0 = ix1 = GETOBJ(int, 1);
    clear_groups = GETOBJ(int,2);
    clear_routers = GETOBJ(int,3);
    clear_counts = GETOBJ(int,4);
    group_addr = GETOBJ(paddr,1)->ip_addr;
    swidb = GETOBJ(idb, 1);

    /*
     * If no keywords are specified, clear all state.
     */
    if (!clear_groups && !clear_routers)
	clear_groups = clear_routers = TRUE;

    /*
     * Derive the initial and final values for the range of bridge
     * groups to clear: all of them or just a given one.
     */
    if (0 == ix0) {
	ix0 = 1;
	ix1 = SPAN_MAXINDEX - 1;
    }

    /*
     * Derive the initial and final values for the range of multicast
     * groups to clear: all of them or just a given one.
     */

    if (0 == group_addr) {
	jx0 = 0;
	jx1 = NETHASHLEN - 1;
    } else {
	jx0 = jx1 = nethash(group_addr);
    }

    for (bridge_group = ix0; bridge_group <= ix1; bridge_group++) {

	span = spanarray[bridge_group];
	if (NULL == span)
	    continue;

	if (clear_routers) {

	    tbifd = NULL;
	    if (swidb) {
		tbifd = tbridge_locate_tbifd_in_q(swidb,
						  &span->mcast_router_ports);
		if (NULL == tbifd)
		    continue;
	    }

	    if (clear_counts) {
		if (tbifd) {
		    tbifd->tbifd_rx_flood_packets = 0;
		    tbifd->tbifd_tx_flood_packets = 0;
		} else {
		    for (tbifd = span->mcast_router_ports.qhead; tbifd != NULL;
			 tbifd = tbifd->tbifd_next) {
			tbifd->tbifd_rx_flood_packets = 0;
			tbifd->tbifd_tx_flood_packets = 0;
		    }
		}
	    } else {
		if (tbifd) {
		    tbridge_delete_tbifd_in_q(&span->mcast_router_ports,
					      tbifd);
		} else {
		    tbridge_clear_tbifdQ(&span->mcast_router_ports);
		}
	    }
	}

	if (clear_groups) {
	    for (group_hash = jx0; group_hash <= jx1; group_hash++) {

		tbmgdQ = span->mcast_groups[group_hash];
		if (NULL == tbmgdQ)
		    continue;

		tbmgd_next = tbmgdQ->qhead;
		while (tbmgd_next != NULL) {

		    tbmgd = tbmgd_next;
		    tbmgd_next = tbmgd->tbmgd_next;

		    if (group_addr && (tbmgd->tbmgd_ipaddr != group_addr))
			continue;

		    tbifd = NULL;
		    if (swidb) {
			tbifd = tbridge_locate_tbifd_in_q(swidb,
							  &tbmgd->tbmgd_oifQ);
			if (NULL == tbifd)
			    continue;
		    }

		    if (clear_counts) {
			if (tbifd) {
			    tbifd->tbifd_rx_flood_packets = 0;
			    tbifd->tbifd_tx_flood_packets = 0;
			} else {
			    for (tbifd = tbmgd->tbmgd_oifQ.qhead;
				 tbifd != NULL;
				 tbifd = tbifd->tbifd_next) {
				tbifd->tbifd_rx_flood_packets = 0;
				tbifd->tbifd_tx_flood_packets = 0;
			    }
			}
		    } else {
			if (tbifd)
			    tbridge_delete_tbifd_in_q(&tbmgd->tbmgd_oifQ,
						      tbifd);
			else
			    tbridge_delete_tbmgd_in_q(tbmgdQ, tbmgd);
		    }
		}
	    }
	}
    }
}


/*
 * The callback for other subsystems (STA specifically) to
 * clear CMF data structures.
 */
static
void tbridge_cmf_clear (spantype *span)
{
    queuetype	*tbmgdQ;
    int		ix;

    if (NULL == span)
	return;

    /*
     * Free all multicast group data structures.
     */
    tbridge_clear_tbifdQ(&span->mcast_router_ports);

    for (ix = 0; ix < NETHASHLEN; ix++) {
	tbmgdQ = span->mcast_groups[ix];
	if (NULL == tbmgdQ)
	    continue;
	tbridge_clear_tbmgdQ(tbmgdQ);
	span->mcast_groups[ix] = NULL;
	free(tbmgdQ);
    }
}

/*
 * The configuration command to enable or disable CGMP.
 */
void tbridge_cmf_command (parseinfo *csb)
{
    int		ix;

    if (csb->nvgen) {
	switch (csb->which) {

	case TBRIDGE_CFG_CMF:
	    nv_write(tbridge_cmf_enable &&
		     !tbridge_cmf_cgmp_enable, "%s", csb->nv_command);
	    break;

	case TBRIDGE_CFG_CMF_CGMP:
	    nv_write(tbridge_cmf_enable &&
		     tbridge_cmf_cgmp_enable, "%s", csb->nv_command);
	    break;

	default:
	    break;
	}
	return;
    }

    switch (csb->which) {

    case TBRIDGE_CFG_CMF:
	/*
	 * If CMF is currently enabled and it's being disabled,
	 * tear down the current multicast-group state and disable.
	 */
	if (tbridge_cmf_enable && !csb->sense) {
	    tbridge_cmf_enable = FALSE;
	    tbridge_cmf_cgmp_enable = FALSE;
	    /*
	     * Free all multicast-group state.
	     */
	    for (ix = 1; ix < SPAN_MAXINDEX; ix++)
		tbridge_cmf_clear(spanarray[ix]);
	/*
	 * Otherwise, just set the sense.
	 */
	} else {
	    tbridge_cmf_enable = csb->sense;
	    tbridge_cmf_cgmp_enable = FALSE;
	}
	break;

    case TBRIDGE_CFG_CMF_CGMP:
	/*
	 * If CMF CGMP is being enabled or disabled, tear down
	 * the current multicast-group state, and flip the sense.
	 */
	if (tbridge_cmf_enable &&
	    tbridge_cmf_cgmp_enable != csb->sense) {
	    tbridge_cmf_cgmp_enable = csb->sense;
	    /*
	     * Free all multicast group descriptors.
	     */
	    for (ix = 1; ix < SPAN_MAXINDEX; ix++)
		tbridge_cmf_clear(spanarray[ix]);
	} else {
	    tbridge_cmf_enable = csb->sense;
	    tbridge_cmf_cgmp_enable = csb->sense;
	}
	break;
    }

    return;
}


/******************************************************************************
 * Not-so-primitives to MODIFY interface descriptors in multicast groups
 *****************************************************************************/

/*
 * Process a multicast router announcment of some kind.  Mostly this just
 * restarts the router-announcement timer for known multicast router ports
 * thereby maintaining them in the multicast-router port list.
 */
static void
tbridge_add_mroute_port (idbtype *swidb,
			 queuetype *mcast_router_ports,
			 queuetype **tbmgdQ_tbl)
{
    int		ix;
    tbifd_type	*tbifd;
    tbmgd_type	*tbmgd;
    boolean	found;

    /*
     * See if this port is already known to be a multicast
     * router port.  If it's not already known, create a new
     * descriptor for it in the multicast router port list.
     */
    found = FALSE;
    for (tbifd = mcast_router_ports->qhead; tbifd != NULL;
	 tbifd = tbifd->tbifd_next) {
	if (tbifd->tbifd_swidb == swidb) {
	    found = TRUE;
	    break;
	}
    }

    /*
     * New multicast-router port recognized.
     */
    if (!found) {
	tbifd = tbridge_create_tbifd_in_q(swidb, mcast_router_ports);
	if (NULL == tbifd)
	    return;
	tbifd->tbifd_attributes |= TBIFD_IPMCAST_ROUTER;
	if (tbridge_debug_cmf)
	    buginf("\nTBCMF: adding router port %s", swidb->namestring);
    }

    /*
     * Re-start the router port timer associated with the
     * port on which a router last spoke.
     * Its expiry is used to maintain the notion of which
     * ports are multicast router ports.
     */
    mgd_timer_stop(&tbifd->tbifd_mrouter_tmr);
    tbifd->tbifd_mrouter_expired_count = 0;
    mgd_timer_start(&tbifd->tbifd_mrouter_tmr, ONEMIN + (10 * ONESEC));

    if (found)
	return;

    /*
     * This is a new multicast router port.  Add it to every
     * multicast group OIF either by marking an existing descriptor
     * for this port with the mcast router attribute, or creating
     * a new descriptor when one does not already exist in the OIF.
     */
    for (ix = 0; ix < NETHASHLEN; ix++) {
	if (NULL == tbmgdQ_tbl[ix])
	    continue;
	for (tbmgd = tbmgdQ_tbl[ix]->qhead; tbmgd != NULL;
	     tbmgd = tbmgd->tbmgd_next) {
	    tbifd  = tbridge_locate_tbifd_in_q(swidb, &tbmgd->tbmgd_oifQ);
	    if (NULL == tbifd) {
		tbifd = tbridge_create_tbifd_in_q(swidb, &tbmgd->tbmgd_oifQ);
		if (NULL == tbifd)
		    return;
		tbifd->tbifd_tbmgd = tbmgd;
	    }
	    tbifd->tbifd_attributes |= TBIFD_IPMCAST_ROUTER;
	}
    }
}

/*
 * Process a join of some kind, typcially IGMP reports.  Mostly this just
 * cancels the IGMP report timer for known multicast-group ports thereby
 * maintaining them in the multicast-group port list.
 */
static void
tbridge_add_mgroup_port (idbtype *swidb, ipaddrtype ipdstaddr)
{
    queuetype	*mcast_router_ports;
    queuetype	*tbmgdQ;
    tbmgd_type	*tbmgd;
    tbifd_type	*router_port, *tbifd;

    mcast_router_ports = &swidb->span_ptr->mcast_router_ports;

    /*
     * Multicast groups in the absence of known multicast-router ports
     * don't make any sense.
     */

    if (mcast_router_ports->count <= 0 && 
	!(global_irb_enable && swidb->span_ptr->irb_bg_vidb))
	return;

    /*
     * Locate the given port in the given group.  If not found,
     * do all the work to create a descriptor for it.
     */
    tbmgdQ = swidb->span_ptr->mcast_groups[nethash(ipdstaddr)];
    if (NULL == tbmgdQ) {
	tbmgdQ = tbridge_create_tbmgdQ_in_tbl(ipdstaddr,
					      swidb->span_ptr->mcast_groups);
	if (NULL == tbmgdQ)
	    return;
    }

    tbmgd = tbridge_locate_tbmgd_in_q(ipdstaddr, tbmgdQ);
    if (NULL == tbmgd) {
	tbmgd = tbridge_create_tbmgd_in_q(ipdstaddr, tbmgdQ);

	if (NULL == tbmgd)
	    return;

	/*
	 * Initialize the outgoing-interface list with all the
	 * known multicast router ports.
	 */
	for (router_port = mcast_router_ports->qhead; router_port != NULL;
	     router_port = router_port->tbifd_next) {
	    tbifd = tbridge_create_tbifd_in_q(router_port->tbifd_swidb,
					      &tbmgd->tbmgd_oifQ);
	    if (NULL == tbifd)
		return;
	    tbifd->tbifd_tbmgd = tbmgd;
	    tbifd->tbifd_attributes |= TBIFD_IPMCAST_ROUTER;
	}
    }

    tbifd = tbridge_locate_tbifd_in_q(swidb, &tbmgd->tbmgd_oifQ);
    if (NULL == tbifd) {
	tbifd = tbridge_create_tbifd_in_q(swidb,
					  &tbmgd->tbmgd_oifQ);
	if (NULL == tbifd)
	    return;

	tbifd->tbifd_tbmgd = tbmgd;
    }

    /*
     * Clear the expired and heard report counts.
     */
    mgd_timer_stop(&tbifd->tbifd_igmp_rpt_tmr);
    tbifd->tbifd_igmp_rpt_expired_count = 0;
    tbmgd->tbmgd_no_rpt_count = 0;

    if (!(tbifd->tbifd_attributes & TBIFD_IPMCAST_GROUP)) {
	tbifd->tbifd_attributes |= TBIFD_IPMCAST_GROUP;
	tbmgd->tbmgd_member_count++;
	if (tbridge_debug_cmf)
	    buginf("\nTBCMF: adding group port %s to group %i",
		   swidb->namestring, tbmgd->tbmgd_ipaddr);
    }

    return;
}

/*
 * Process IGMP queries.  Start the IGMP query timer for the querying port
 * and start the IGMP report timer for each group port in each multicast
 * group in the bridge group.
 */
static void
tbridge_start_query_timers (idbtype *swidb,
			    queuetype **tbmgdQ_tbl,
			    ulong response_time)
{
    int		ix;
    tbmgd_type	*tbmgd;
    tbifd_type	*tbifd;

    for (ix = 0; ix < NETHASHLEN; ix++) {
	if (NULL == tbmgdQ_tbl[ix])
	    continue;
	for (tbmgd = tbmgdQ_tbl[ix]->qhead; tbmgd != NULL;
	     tbmgd = tbmgd->tbmgd_next) {

	    tbifd  = tbridge_locate_tbifd_in_q(swidb, &tbmgd->tbmgd_oifQ);
	    if (NULL == tbifd) {
		buginf("\nTBCMF: router port %s missing from OIF in %i",
		       swidb->namestring, tbmgd->tbmgd_ipaddr);
		return;
	    }

	    /*
	     * For each multicast group, disable report suppression
	     * and time the duration of the query response time.
	     * This timer is associated with the port on which
	     * the query was received.
	     * Its expiry is used simply to disable report
	     * suppression again at the end of the query response time.
	     */
	    mgd_timer_stop(&tbifd->tbifd_igmp_qry_tmr);
	    mgd_timer_start(&tbifd->tbifd_igmp_qry_tmr,
			    (response_time * ONESEC) + ONESEC);
	    /*
	     * The order of setting these bits is VERY important
	     * since TBMGD_IGMP_RPT_SUPPRESS gets set in the
	     * receive interrupt.  Should probably mask interrupts
	     * around clearing it here.
	     */
	    tbmgd->tbmgd_attributes &= ~TBMGD_IGMP_RPT_SUPPRESS;
	    tbmgd->tbmgd_attributes |= TBMGD_IGMP_QRY_CURRENT;
	    tbmgd->tbmgd_no_rpt_count++;
	    /*
	     * For each group port in each multicast group, time
	     * the reception of an IGMP report in response to
	     * an IGMP query.
	     * The expiry of this timer is used to maintain the
	     * notion of which ports are multicast group ports,
	     * and of which groups they are members.
	     * Don't time responses on the port on which the query
	     * was received.
	     */
	    for (tbifd = tbmgd->tbmgd_oifQ.qhead; tbifd != NULL;
		 tbifd = tbifd->tbifd_next) {
		if ((tbifd->tbifd_attributes & TBIFD_IPMCAST_GROUP) &&
		    (tbifd->tbifd_swidb != swidb)) {
		    mgd_timer_stop(&tbifd->tbifd_igmp_rpt_tmr);
		    mgd_timer_start(&tbifd->tbifd_igmp_rpt_tmr,
				    (response_time * ONESEC) + ONESEC);
		}
	    }
	}
    }
}


/******************************************************************************
 * Functions to HANDLE tbridge monitor events:
 *****************************************************************************/

/************************
 * Expired Timer Handlers
 ************************/

/*
 * Expired IGMP query timer handler.
 */
static void
tbridge_igmp_qry_timer (tbifd_type *tbifd)
{
    tbmgd_type	*tbmgd;

    mgd_timer_stop(&tbifd->tbifd_igmp_qry_tmr);

    tbmgd = tbifd->tbifd_tbmgd;

    /*
     * Upon expiry of a query epoch, just disable suppression
     * so that reports received between queries will be forwarded.
     */
    tbmgd->tbmgd_attributes &= ~TBMGD_IGMP_QRY_CURRENT;
    tbmgd->tbmgd_attributes &= ~TBMGD_IGMP_RPT_SUPPRESS;

    /*
     * If we haven't received any reports for this group
     * after more than two queries, delete the group.
     */
    if (tbmgd->tbmgd_no_rpt_count > 2) {

	spantype	*span;
	uchar		hash;
	queuetype	*tbmgdQ;

	span = tbifd->tbifd_swidb->span_ptr;
	hash = nethash(tbmgd->tbmgd_ipaddr);
	tbmgdQ = span->mcast_groups[hash];

	if (tbridge_debug_cmf)
	    buginf("\nTBCMF: deletion of group %i (no reports)",
		   tbmgd->tbmgd_ipaddr);

	tbridge_delete_tbmgd_in_q(tbmgdQ, tbmgd);

	if (tbmgdQ->count <= 0) {
	    span->mcast_groups[hash] = NULL;
	    free(tbmgdQ);
	}
    }
}

/*
 * Expired IGMP report timer handler.
 */
static void
tbridge_igmp_rpt_timer (tbifd_type *tbifd)
{
    mgd_timer_stop(&tbifd->tbifd_igmp_rpt_tmr);

    if (tbridge_debug_cmf)
	buginf("\nTBCMF: IGMP report expiry on %s in group %i",
	       tbifd->tbifd_swidb->namestring,
	       tbifd->tbifd_tbmgd->tbmgd_ipaddr);

    /*
     * If we haven't received a report on the given port
     * after more than two queries, delete the port from the group.
     */
    if (++tbifd->tbifd_igmp_rpt_expired_count > 2) {

	tbmgd_type	*tbmgd;
	spantype	*span;
	uchar		hash;
	queuetype	*tbmgdQ;

	tbmgd = tbifd->tbifd_tbmgd;
	span = tbifd->tbifd_swidb->span_ptr;
	hash = nethash(tbmgd->tbmgd_ipaddr);
	tbmgdQ = span->mcast_groups[hash];

	if (tbridge_debug_cmf)
	    buginf("\nTBCMF: departure of group port %s from group %i "
		   "(no reports)",
		   tbifd->tbifd_swidb->namestring,
		   tbmgd->tbmgd_ipaddr);

	/*
	 * This port is no longer a multicast-group port.
	 */
	tbifd->tbifd_attributes &= ~TBIFD_IPMCAST_GROUP;
	tbmgd->tbmgd_member_count--;

	/*
	 * Delete only ports which are not also multicast-router ports.
	 */
	if (!tbifd->tbifd_attributes) {

	    if (tbridge_debug_cmf)
		buginf("\nTBCMF: deletion of %s from group %i",
		       tbifd->tbifd_swidb->namestring,
		       tbmgd->tbmgd_ipaddr);

	    tbridge_delete_tbifd_in_q(&tbmgd->tbmgd_oifQ, tbifd);
	}

	/*
	 * The most recent deletion has resulted in a group with
	 * no multicast-group ports.
	 */
	if (tbmgd->tbmgd_member_count <= 0) {

	    if (tbridge_debug_cmf)
		buginf("\nTBCMF: deletion of group %i (no members)",
		       tbmgd->tbmgd_ipaddr);

	    tbridge_delete_tbmgd_in_q(tbmgdQ, tbmgd);

	    if (tbmgdQ->count <= 0) {
		span->mcast_groups[hash] = NULL;
		free(tbmgdQ);
	    }
	}
    }
}

/*
 * Expired router-announcement timer handler.
 */
static void
tbridge_mrouter_timer (tbifd_type *tbifd)
{
    mgd_timer_stop(&tbifd->tbifd_mrouter_tmr);

    if (tbridge_debug_cmf)
	buginf("\nTBCMF: router announcement expiry on %s",
	       tbifd->tbifd_swidb->namestring);

    /*
     * If we haven't received an announcement on the given port
     * after more than two time-out intervals, delete the port from
     * the multicast-router port list and from all multicast groups.
     */
    if (++tbifd->tbifd_mrouter_expired_count > 2) {

	int		ix;
	idbtype		*swidb;
	spantype	*span;
	queuetype	**tbmgdQ_tbl;
	tbmgd_type	*tbmgd;

	swidb = tbifd->tbifd_swidb;
	span = swidb->span_ptr;
	tbmgdQ_tbl = span->mcast_groups;

	tbridge_delete_tbifd_in_q(&span->mcast_router_ports, tbifd);

	for (ix = 0; ix < NETHASHLEN; ix++) {

	    if (NULL == tbmgdQ_tbl[ix])
		continue;

	    tbmgd = tbmgdQ_tbl[ix]->qhead;
	    while (tbmgd != NULL) {

		tbifd  = tbridge_locate_tbifd_in_q(swidb, &tbmgd->tbmgd_oifQ);
		if (NULL == tbifd) {
		    buginf("\nTBCMF: router port %s missing from OIF in %i",
			   swidb->namestring, tbmgd->tbmgd_ipaddr);
		    continue;
		}

		if (tbridge_debug_cmf)
		    buginf("\nTBCMF: departure of router port %s from group %i"
			   " (no announcements)",
			   tbifd->tbifd_swidb->namestring,
			   tbifd->tbifd_tbmgd->tbmgd_ipaddr);

		/*
		 * This port is no longer a multicast-router port.
		 */
		tbifd->tbifd_attributes &= ~TBIFD_IPMCAST_ROUTER;


		/*
		 * Delete only ports which are not also multicast-group ports.
		 */
		if (!tbifd->tbifd_attributes) {

		    if (tbridge_debug_cmf)
			buginf("\nTBCMF: deletion of %s from group %i",
			       tbifd->tbifd_swidb->namestring,
			       tbifd->tbifd_tbmgd->tbmgd_ipaddr);

		    tbridge_delete_tbifd_in_q(&tbifd->tbifd_tbmgd->tbmgd_oifQ,
					      tbifd);
		}


		/*
		 * The most recent deletion has resulted in a group with
		 * no multicast-router ports.
		 */
		if (span->mcast_router_ports.count <= 0 && 
		    !(global_irb_enable && span->irb_bg_vidb)) {

		    tbmgd_type	*tbmgd_next;

		    if (tbridge_debug_cmf)
			buginf("\nTBCMF: deletion of group %i (no routers)",
			       tbmgd->tbmgd_ipaddr);

		    tbmgd_next = tbmgd->tbmgd_next;
		    tbridge_delete_tbmgd_in_q(tbmgdQ_tbl[ix], tbmgd);
		    tbmgd = tbmgd_next;

		    if (tbmgdQ_tbl[ix]->count <= 0) {
			free(tbmgdQ_tbl[ix]);
			tbmgdQ_tbl[ix] = NULL;
		    }
		} else {
		    tbmgd = tbmgd->tbmgd_next;
		}
	    }
	}

    } else {

	/*
	 * Haven't timed out more than twice.  Restart the
	 * router-announcement timer.
	 */
	mgd_timer_start(&tbifd->tbifd_mrouter_tmr, ONEMIN);
    }
}

/****************************
 * Interesing Packet Handlers
 ****************************/

/*
 * Interesting IGMP packet handler.
 */
static void
tbridge_igmp_pkt (paktype *pak)
{
    idbtype	*swidb;

    iphdrtype	*ip;
    igmptype	*igmp;
    int		length;

    swidb = pak->if_input;
    if (NULL == swidb) {
	retbuffer(pak);
	return;
    }

    ip = (iphdrtype *)ipheadstart(pak);
    length = ip->tl - (ip->ihl << 2);

    /*
     * Validate length of packet.
     */
    if (length < IGMPHEADERBYTES) {
	igmp_traffic.formaterr++;
	retbuffer(pak);
	return;
    }

    igmp = (igmptype *)ipdatastart(pak);

    /*
     * Checksum packet.
     */
    if (ipcrc((ushort *)igmp, length)) {
	igmp_traffic.checksumerr++;
	retbuffer(pak);
	return;
    }

    /*
     * Validate version number.
     */
    if ((igmp->type & 0xF0) != IGMP_VERSION1_OR_2_TYPE) {
	igmp_traffic.formaterr++;
	retbuffer(pak);
	return;
    }

    /*
     * Detect and process interesting packets.
     */

    switch (igmp->type) {

    case IGMP_QUERY_TYPE: {

	ulong	response_time;

	response_time = (igmp->code) ? (igmp->code / 10) :
	    IGMP_REPORT_DELAY_MAX;
	tbridge_add_mroute_port(swidb,
				&swidb->span_ptr->mcast_router_ports,
				swidb->span_ptr->mcast_groups);
	tbridge_start_query_timers(swidb,
				   swidb->span_ptr->mcast_groups,
				   response_time);
    }
	break;

    case IGMP_NEW_REPORT_TYPE:
    case IGMP_REPORT_TYPE:
	tbridge_add_mgroup_port(swidb, (ipaddrtype)(GETLONG(&igmp->address)));
	break;

    case IGMP_LEAVE_TYPE:
	break;

    case IGMP_PIM_TYPE:
	if (PIM_QUERY_CODE == igmp->code) {
	    tbridge_add_mroute_port(swidb,
				    &swidb->span_ptr->mcast_router_ports,
				    swidb->span_ptr->mcast_groups);
	}
	break;

    case IGMP_DVMRP_TYPE:
	if (DVMRP_PROBE_CODE == igmp->code) {
	    tbridge_add_mroute_port(swidb,
				    &swidb->span_ptr->mcast_router_ports,
				    swidb->span_ptr->mcast_groups);
	}
	break;

    default:
	/*
	 * False alarm.  This is an IGMP packet that should not
	 * have passed through the browser!
	 */
	if (tbridge_debug_cmf)
	    buginf("\nTBCMF: unexpected IGMP packet encountered: %i %02x %02x",
		   ip->dstadr, igmp->type, igmp->code);
	break;
    }

    retbuffer(pak);
    return;
}


/*
 * Interesting MOSPF packet handler.
 */
static void
tbridge_ospf_pkt (paktype *pak)
{
    idbtype	*swidb;

    iphdrtype	*ip;
    ospftype	*ospf;

    swidb = pak->if_input;
    if (NULL == swidb) {
	retbuffer(pak);
	return;
    }

    ip = (iphdrtype *)ipheadstart(pak);
    ospf = (ospftype *)ipdatastart(pak);

    switch (ospf->ospf_type) {

    case OSPF_HELLO: {

	hellotype	*hello;
	hello = (hellotype *)(ospf->ospf_data);

	if (GET_MC_BIT(hello->h_rtr_options)) {
	    tbridge_add_mroute_port(swidb,
				    &swidb->span_ptr->mcast_router_ports,
				    swidb->span_ptr->mcast_groups);
	}
    }
	break;

    default:
	/*
	 * False alarm.  This is an OSPF packet that should not
	 * have passed through the browser!
	 */
	if (tbridge_debug_cmf)
	    buginf("\nTBCMF: unexpected OSPF packet encountered: %i %02x %02x",
		   ip->dstadr, ospf->ospf_type,
		   ((hellotype *)(ospf->ospf_data))->h_rtr_options);
	break;
    }

    retbuffer(pak);
    return;
}

/*
 * Interesting CGMP packet handler.
 */
static void
tbridge_cgmp_pkt (paktype *pak)
{
    idbtype	*swidb;
    cgmptype	*cgmp;
    btetype	*bte;

    ipaddrtype	group;

    swidb = pak->if_input;
    if (NULL == swidb) {
	retbuffer(pak);
	return;
    }

    cgmp = (cgmptype *)pak->network_start;
    group = (ipaddrtype)(GETLONG(&cgmp->cgmp_mac_addrs[0].daddr[2]));

    /*
     * Detect and process interesting packets.
     */
    switch (cgmp->cgmp_type) {

    case CGMP_JOIN_TYPE:

	if (tbridge_debug_cmf)
	    buginf("\nTBCMF: CGMP join %e %e",
		   cgmp->cgmp_mac_addrs[0].daddr,
		   cgmp->cgmp_mac_addrs[0].saddr);

	if (is_ieee_zero(cgmp->cgmp_mac_addrs[0].daddr)) {
	    tbridge_add_mroute_port(swidb,
				    &swidb->span_ptr->mcast_router_ports,
				    swidb->span_ptr->mcast_groups);
	} else {
	    /* Look up the USA in the bridge table. */
	    bte = tbridge_locate_bte(swidb->span_ptr,
				     cgmp->cgmp_mac_addrs[0].saddr);
	    if (bte) {
		tbridge_add_mgroup_port(bte->bte_interface, group);
	    }
	}
	break;

    case CGMP_LEAVE_TYPE: {

	tbifd_type	*tbifd;
	queuetype	*tbmgdQ;
	tbmgd_type	*tbmgd;

	if (tbridge_debug_cmf)
	    buginf("\nTBCMF: CGMP leave %e %e",
		   cgmp->cgmp_mac_addrs[0].daddr,
		   cgmp->cgmp_mac_addrs[0].saddr);

	if (is_ieee_zero(cgmp->cgmp_mac_addrs[0].daddr)) {
	    tbifd = tbridge_locate_tbifd_in_q(swidb,
				      &swidb->span_ptr->mcast_router_ports);
	    if (tbifd != NULL) {
		mgd_timer_stop(&tbifd->tbifd_mrouter_tmr);
		tbifd->tbifd_mrouter_expired_count = 2;
		mgd_timer_start(&tbifd->tbifd_mrouter_tmr, 0x0);
	    }

	} else {

	    if (is_ieee_zero(cgmp->cgmp_mac_addrs[0].saddr)) {

		tbmgdQ = swidb->span_ptr->mcast_groups[nethash(group)];
		if (NULL == tbmgdQ)
		    break;
		tbmgd = tbridge_locate_tbmgd_in_q(group, tbmgdQ);
		if (NULL == tbmgd)
		    break;
		tbifd = tbridge_locate_tbifd_in_q(swidb,
						  &tbmgd->tbmgd_oifQ);
		if (NULL == tbifd)
		    break;

		mgd_timer_stop(&tbifd->tbifd_igmp_qry_tmr);
		tbmgd->tbmgd_no_rpt_count = 3;
		mgd_timer_start(&tbifd->tbifd_igmp_qry_tmr, 0x0);

	    } else {

		/* Look up the USA in the bridge table. */
		bte = tbridge_locate_bte(swidb->span_ptr,
					 cgmp->cgmp_mac_addrs[0].saddr);
		if (bte) {
		    tbmgdQ = swidb->span_ptr->mcast_groups[nethash(group)];
		    if (NULL == tbmgdQ)
			break;
		    tbmgd = tbridge_locate_tbmgd_in_q(group, tbmgdQ);
		    if (NULL == tbmgd)
			break;
		    tbifd = tbridge_locate_tbifd_in_q(bte->bte_interface,
						      &tbmgd->tbmgd_oifQ);
		    if (NULL == tbifd)
			break;

		    mgd_timer_stop(&tbifd->tbifd_igmp_rpt_tmr);
		    tbifd->tbifd_igmp_rpt_expired_count = 2;
		    mgd_timer_start(&tbifd->tbifd_igmp_rpt_tmr, 0x0);
		}
	    }
	}
    }
	break;

    default:
	/*
	 * False alarm.  This is a CGMP packet that should not
	 * have passed through the browser!
	 */
	if (tbridge_debug_cmf)
	    buginf("\nTBCMF: unexpected CGMP packet encountered: %s %02x %02x",
		   swidb->namestring, cgmp->cgmp_version, cgmp->cgmp_type);
	break;
    }

    retbuffer(pak);
    return;
}


/******************************************************************************
 * The callback function from the fastpath to derive constrained flood lists.
 *****************************************************************************/

#define STATIC static
#define INLINE inline

#include "../tbridge/tbridge_cmf_inline.h"

/*
 * Identify and browse "interesting" IP multicast packets to maintain
 * multicast-group outgoing interface lists and multicast-router port
 * lists.  For "uninteresting" IP multicast packets, see if we have
 * group information for them, and identify the outgoing interface list.
 *
 * flood_list will only be written if a constrained flood list is
 * derived.  The caller should default to the spanning tree flood list.
 * 
 * flood_scope is a mask that describes what combination of multicast-
 * router and multicast-group ports the flood should include.
 */

static ulong
tbridge_cmf (paktype *pak, tbifd_type **flood_list)
{
    ulong	flood_scope;
    tbifd_type	*constrained_flood_list;
    ieee_addrs	*mac;
    ipaddrtype	ip_dstaddr;

    if (!tbridge_cmf_enable)
	return(TBIFD_FLOOD);

    mac = (ieee_addrs *)pak->addr_start;

    /*
     * Default is to assume that the packet will be conventionally flooded.
     */
    flood_scope = TBIFD_FLOOD;
    constrained_flood_list = NULL;

    if (tbridge_cmf_cgmp_enable) {

	/*
	 * Make like the cat5000.  Use the low-order four bytes of
	 * the destination MAC address as the effective IP multicast
	 * group address.
	 */
	ip_dstaddr = (ipaddrtype)(GETLONG(((ulong *)&mac->daddr[2])));

	/*
	 * Identify CGMP packets by destination MAC address.
	 */
	if (ieee_equal(mac->daddr, cgmp_address))
	    flood_scope = tbridge_browse_cgmp_inline(pak,
						     &constrained_flood_list);

    } else {

	uchar		ip_protocol;

	ip_dstaddr = GETLONG((ipaddrtype *)
			     (pak->network_start + TBRIDGE_IP_DSTADDR_OFFSET));
	ip_protocol = *(pak->network_start + TBRIDGE_IP_PROTOCOL_OFFSET);

	if (TBRIDGE_IP_PROTOCOL_IGMP == ip_protocol) {

	    /*
	     * Amongst unreserved IP multicast addresses, we are
	     * interested in IGMP reports and leaves.
	     */
	    flood_scope = tbridge_browse_igmp_inline(pak,
						     &constrained_flood_list);

	} else if (TBRIDGE_IP_MCAST_RSVD(ip_dstaddr)) {

	    /*
	     * Amongst the reserved IP multicast addresse, we are
	     * interested in IGMP queries, PIM queries, DVMRP probes,
	     * and MOSPF hellos.  We could snag these by casing on
	     * the destination IP address looking for
	     *
	     *  - TBRIDGE_ALLSYSTEMS (IGMP queries)
	     *  - TBRIDGE_ALLROUTERS (PIM queries)
	     *  - TBRIDGE_DVMRPROUTERS (DVMRP probes)
	     *  - TBRIDGE_ALLSPFROUTERS (MOSFP hellos)
	     *
	     * but it's far more expedient just to snag them by
	     * protocol type since the first three are all buried
	     * in IGMP.
	     *
	     * If and when PIM and/or DVMRP are broken out of IGMP
	     * and given their own reserved IP multicast addresses,
	     * then it will be time to convert this to case on the
	     * destination IP multicast address instead.
	     */

	    if (TBRIDGE_IP_PROTOCOL_IGMP == ip_protocol) {
		flood_scope = tbridge_browse_igmp_inline(pak,
						  &constrained_flood_list);
	    } else if (TBRIDGE_IP_PROTOCOL_OSPF == ip_protocol) {
		flood_scope = tbridge_browse_ospf_inline(pak,
						  &constrained_flood_list);
	    }
	}
    }

    /*
     * A browsed packet, such as an IGMP report, may be suppressed.
     */
    if (TBIFD_NO_FLOOD == flood_scope)
	return(flood_scope);

    /*
     * We enter this conditional with all IP multicast data and
     * any uninteresting reserved IP multicast packets.
     */
    if (NULL == constrained_flood_list) {

	spantype	*span;
	tbmgd_type	*tbmgd;

	/*
	 * See if we have group information for this multicast IP
	 * address and, if so, use the group's outgoing-interface
	 * list to constrain the flood.
	 */
	tbmgd = NULL;
	span = pak->if_input->span_ptr;
	if (span->mcast_groups[nethash(ip_dstaddr)] != NULL)
	    tbmgd = tbridge_locate_tbmgd_in_q(ip_dstaddr,
			      (span->mcast_groups[nethash(ip_dstaddr)]));

	if (tbmgd && tbmgd->tbmgd_oifQ.qhead) {

	    constrained_flood_list = tbmgd->tbmgd_oifQ.qhead;
	    flood_scope = TBIFD_IPMCAST_GROUP | TBIFD_IPMCAST_ROUTER;

	    if (tbridge_debug_ipm) {
		buginf("\nTB2: constrained flood (%04x) from %s to %e %s",
		       flood_scope,
		       pak->if_input->namestring,
		       mac->daddr,
		       rxtype_string(pak->rxtype));
	    }

	} else {

	    /*
	     * Make like the cat5000.  When it misses on an IP multicast
	     * group address lookup, the packet is handled by the processor
	     * which DOES have access to the real IP address (as opposed
	     * to just the low-order four bytes of the destination MAC
	     * address).  So re-compute the destination IP address in this
	     * case.
	     */
	    if (tbridge_cmf_cgmp_enable) {
		ip_dstaddr = GETLONG((ipaddrtype *)
				       (pak->network_start +
					TBRIDGE_IP_DSTADDR_OFFSET));
	    }

	    /*
	     * Constrain IP multicast data packets for which we have
	     * no multicast group state info to multicast routers ports
	     * only when there is at least one multicast router port present.
	     */
	    if ((span->mcast_router_ports.count > 0) &&
		!TBRIDGE_IP_MCAST_RSVD(ip_dstaddr)) {
		constrained_flood_list = span->mcast_router_ports.qhead;
		flood_scope = TBIFD_IPMCAST_ROUTER;
	    }

	}
    }

    if (constrained_flood_list != NULL)
	*flood_list = constrained_flood_list;
    return(flood_scope);
}


/******************************************************************************
 * The CMF subsystem.
 *****************************************************************************/

static void
tbridge_cmf_init (subsystype* subsys)

{
    /*
     * Open for business ...
     */
    tbridge_cmf_parser_init();

    reg_add_tbridge_monitor_queue(TBM_IGMP_QUEUE, tbridge_igmp_pkt,
				  "tbridge_igmp_pkt");
    reg_add_tbridge_monitor_queue(TBM_OSPF_QUEUE, tbridge_ospf_pkt,
				  "tbridge_osp_pkt");
    reg_add_tbridge_monitor_queue(TBM_CGMP_QUEUE, tbridge_cgmp_pkt,
				  "tbridge_cgmp_pkt");

    reg_add_tbridge_monitor_timer(TBM_IGMP_QRY_TIMER, tbridge_igmp_qry_timer,
				  "tbridge_igmp_qry_timer");
    reg_add_tbridge_monitor_timer(TBM_IGMP_RPT_TIMER, tbridge_igmp_rpt_timer,
				  "tbridge_igmp_rpt_timer");
    reg_add_tbridge_monitor_timer(TBM_MROUTER_TIMER, tbridge_mrouter_timer,
				  "tbridge_mrouter_timer");

    reg_add_tbridge_cmf_clear(tbridge_cmf_clear, "tbridge_cmf_clear");

    reg_add_tbridge_cmf(tbridge_cmf, "tbridge_cmf");

    tbridge_cmf_enable = FALSE;
    tbridge_cmf_cgmp_enable = FALSE;
}

#define TBRCMF_MAJVERSION 1
#define TBRCMF_MINVERSION 0
#define TBRCMF_EDITVERSION 1

SUBSYS_HEADER(tbridge_cmf, TBRCMF_MAJVERSION, TBRCMF_MINVERSION,
	      TBRCMF_EDITVERSION,
	      tbridge_cmf_init, SUBSYS_CLASS_PROTOCOL, NULL,
	      "req: fastswitch");
