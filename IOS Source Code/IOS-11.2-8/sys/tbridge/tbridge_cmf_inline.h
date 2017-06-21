/* $Id: tbridge_cmf_inline.h,v 3.1.18.3 1996/05/14 04:30:16 speakman Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_cmf_inline.h,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_cmf_inline.h
 *
 * January 1996, Tony Speakman
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Generic inline functions for Constrained IP Multicasting Flooding (CMF)
 *------------------------------------------------------------------
 * $Log: tbridge_cmf_inline.h,v $
 * Revision 3.1.18.3  1996/05/14  04:30:16  speakman
 * CSCdi57584:  CGMP packets not seen by RSP
 * Branch: California_branch
 * Calculate network_start when copying RSP packets for flooding.
 *
 * Revision 3.1.18.2  1996/05/08  00:23:54  speakman
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
 * Revision 3.1.18.1  1996/05/04  05:08:53  wilber
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
 * Revision 3.1  1996/02/20  22:43:49  speakman
 * Placeholders for constrained IP multicast flooding.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TBRIDGE_CMF_INLINE_H__
#define __TBRIDGE_CMF_INLINE_H__


/******************************************************************************
 * Functions to GENERATE tbridge monitor events:
 *****************************************************************************/

/*
 * Browse, copy, and enqueue selected IGMP packets to the tbridge monitor.
 * Return a flood list and a mask to constrain the scope in which these
 * selected packets will subsequently be flooded.
 */

STATIC INLINE
ulong
tbridge_browse_igmp_inline (paktype *pak, tbifd_type **constrained_flood_list)

{
    ulong	flood_scope;
    igmptype	*igmp;
    boolean	copy;
    spantype	*span;

    /*
     * Since bvi's span ptr is undefined, we have to 
     * get the right one from the spannarray[] by using
     * the bridge group index into the spannarray
     */
    if (is_bvi(pak->if_input->hwptr)) {
	span = spanarray[pak->if_input->hwptr->unit];
    }
    else {
	span = pak->if_input->span_ptr;
    }

    igmp = (igmptype *)ipdatastart(pak);
    copy = FALSE;

    *constrained_flood_list = NULL;

    /*
     * The default is to return an indication to flood throughout
     * the bridge group.
     */
    flood_scope = TBIFD_FLOOD;

    switch (igmp->type) {

    case IGMP_QUERY_TYPE:
	/*
	 * It would be best, but time consuming, to disable
	 * report suppression on all groups at this point.
	 */
	copy = TRUE;
	break;

    case IGMP_NEW_REPORT_TYPE:
    case IGMP_REPORT_TYPE: {

	ipaddrtype	group;
	tbmgd_type	*tbmgd;
	queuetype	*tbmgdQ;

	/*
	 * See if we have multicast group info for the reported group.
	 */
	group = (ipaddrtype)(GETLONG(&igmp->address));
	tbmgd = NULL;
	tbmgdQ = span->mcast_groups[nethash(group)];
	if (tbmgdQ != NULL)
	    tbmgd = tbridge_locate_tbmgd_in_q(group, tbmgdQ);
	if (tbmgd != NULL) {

	    /*
	     * We have multicast group info for the reported group.
	     */
	    if (tbmgd->tbmgd_attributes & TBMGD_IGMP_RPT_SUPPRESS) {
		/*
		 * If we are suppressing reports (because one has
		 * already been sent in the current query epoch),
		 * return an indication NOT to flood this report.
		 */
		flood_scope = TBIFD_NO_FLOOD;
	    } else {

		/*
		 * Suppress further reports from this group only
		 * if we are currently in a query epoch.
		 */
		if (tbmgd->tbmgd_attributes & TBMGD_IGMP_QRY_CURRENT)
		    tbmgd->tbmgd_attributes |= TBMGD_IGMP_RPT_SUPPRESS;

		/*
		 * Return an indication to flood this report only
		 * on known multicast router ports.
		 */
		flood_scope = TBIFD_IPMCAST_ROUTER;
		*constrained_flood_list = tbmgd->tbmgd_oifQ.qhead;
	    }
	} else {
	    /*
	     * Constrain reports for unknown multicast groups to
	     * multicast router ports only.
	     */
	    flood_scope = TBIFD_IPMCAST_ROUTER;
	    *constrained_flood_list = span->
		mcast_router_ports.qhead;
	}

	copy = TRUE;
    }
	break;

    case IGMP_LEAVE_TYPE: {

	ipaddrtype	group;
	tbmgd_type	*tbmgd;
	queuetype	*tbmgdQ;

	flood_scope = TBIFD_IPMCAST_ROUTER;

	/*
	 * See if we have multicast group info for the reported group.
	 */
	group = (ipaddrtype)(GETLONG(&igmp->address));
	tbmgd = NULL;
	tbmgdQ = span->mcast_groups[nethash(group)];
	if (tbmgdQ != NULL)
	    tbmgd = tbridge_locate_tbmgd_in_q(group, tbmgdQ);
	if (tbmgd != NULL) {

	    /*
	     * Return an indication to flood this report only
	     * on known multicast router ports.
	     */
	    flood_scope = TBIFD_IPMCAST_ROUTER;
	    *constrained_flood_list = tbmgd->tbmgd_oifQ.qhead;

	}
	copy = TRUE;
    }
	break;

    case IGMP_PIM_TYPE:
	switch (igmp->code) {
	case PIM_QUERY_CODE:
	    copy = TRUE;
	    break;
	default:
	    break;
	}
	break;

    case IGMP_DVMRP_TYPE:
	switch (igmp->code) {
	case DVMRP_PROBE_CODE:
	    copy = TRUE;
	    break;
	default:
	    break;
	}
	break;

    default:
	break;
    }

    /*
     * If this packet is from BVI, which means the actual i/f
     * is the routing i/f. Then we don't have to create 
     * a timer for this i/f
     */
    if (is_bvi(pak->if_input->hwptr)) {
    	copy = FALSE;
    }

    if (copy) {

	/*
	 * Make a copy for process-level processing.  The original
	 * gets fast switched out from under us.
	 */

	paktype	*newpak;

	newpak = pak_clip(pak, pak->datagramsize);
	if (newpak) {
	    newpak->if_input = pak->if_input;
	    process_enqueue(tbm_igmpQ, newpak);
	}

	/*
	 * In the absence of a constrained list, the default is to
	 * flood interesting packets throughout the bridge group.
	 */
	if (NULL == *constrained_flood_list)
	    *constrained_flood_list = pak->if_input->span_ptr->floodQ.qhead;

    }

    return(flood_scope);
}


/*
 * Browse, copy, and enqueue selected OSPF packets to the tbridge monitor.
 * Return a flood list and a mask to constrain the scope in which these
 * selected packets will subsequently be flooded.
 */

STATIC INLINE
ulong
tbridge_browse_ospf_inline (paktype *pak, tbifd_type **constrained_flood_list)

{
    ulong	flood_scope;
    ospftype	*ospf;
    boolean	copy;

    ospf = (ospftype *)ipdatastart(pak);
    copy = FALSE;

    *constrained_flood_list = NULL;

    /*
     * The default is to return an indication to flood throughout
     * the bridge group.
     */
    flood_scope = TBIFD_FLOOD;

    if (ospf->ospf_type == OSPF_HELLO) {

	hellotype	*hello;
	hello = (hellotype *)(ospf->ospf_data);

	if (GET_MC_BIT(hello->h_rtr_options)) {
	    copy = TRUE;
	}
    }

    if (copy) {

	/*
	 * Make a copy for process-level processing.  The original
	 * gets fast switched out from under us.
	 */

	paktype	*newpak;

	newpak = pak_clip(pak, pak->datagramsize);
	if (newpak) {
	    newpak->if_input = pak->if_input;
	    process_enqueue(tbm_ospfQ, newpak);
	}

	*constrained_flood_list = pak->if_input->span_ptr->floodQ.qhead;

    }

    return(flood_scope);
}


/*
 * Browse, copy, and enqueue selected CGMP packets to the tbridge monitor.
 * Return a flood list and a mask to constrain the scope in which these
 * selected packets will subsequently be flooded.
 */

STATIC INLINE
ulong
tbridge_browse_cgmp_inline (paktype *pak, tbifd_type **constrained_flood_list)

{
    ulong	flood_scope;
    cgmptype	*cgmp;
    boolean	copy;

    cgmp = (cgmptype *)pak->network_start;
    copy = FALSE;

    *constrained_flood_list = NULL;

    /*
     * The default is to return an indication to flood throughout
     * the bridge group.
     */
    flood_scope = TBIFD_FLOOD;

    switch (cgmp->cgmp_type) {

    case CGMP_JOIN_TYPE:
	copy = TRUE;
	break;

    case CGMP_LEAVE_TYPE:
	copy = TRUE;
	break;

    default:
	break;
    }

    if (copy) {

	/*
	 * Make a copy for process-level processing.  The original
	 * gets fast switched out from under us.
	 */

	paktype	*newpak;

	newpak = pak_clip(pak, pak->datagramsize);
	if (newpak) {
	    newpak->if_input = pak->if_input;
	    process_enqueue(tbm_cgmpQ, newpak);
	}

	*constrained_flood_list = pak->if_input->span_ptr->floodQ.qhead;

    }

    return(flood_scope);
}

#endif /* __TBRIDGE_CMF_INLINE_H__ */
