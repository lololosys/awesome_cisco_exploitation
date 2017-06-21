/*--------------------------------------------------------------------------
 *
 * $Id: vlan_private.h,v 3.5.10.2 1996/06/01 01:32:53 hampton Exp $
 *
 * vlan/vlan_private.h
 *
 * April 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1997 by Cisco Systems, Inc.
 *
 * All rights reserved.
 *
 * Private functions for Virtual LANs subsystem (i.e. their scope is only
 * this subsystem), shared between ISL and IEEE 802.10 based vLANS across the
 * packet memory based (4x00 and 75xx) and MCI/CxBus (70x0) based platforms.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: vlan_private.h,v $
 * Revision 3.5.10.2  1996/06/01  01:32:53  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.5.10.1  1996/03/23  01:34:30  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.5  1996/02/25  03:52:49  mmcneali
 * CSCdi49686:  IPX non-functional in gs7 images on ISL encapsulating
 *              subinterfaces.
 *
 * Revision 3.4  1996/01/23  08:55:39  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 *
 * $Endlog$
 */


#ifndef __VLAN_PRIVATE_H__
#define __VLAN_PRIVATE_H__


/*
 * Exported functions and external declarations.
 */

extern  void    vlan_debug_init (void);
extern  void    vlan_parser_init (void);
extern  void    vlan_register_fastswitching (void);
extern  void    vlan_packet_counters (paktype * vlan_packet,
				      uchar     vlan_encapsulation);

/*
 * dot10_colour_2_swidb ()
 *
 * Resolve an 802.10 SAID (Virtual LAN Identifier) received on any interface
 * type to a subinterface.
 */

static inline idbtype *dot10_colour_2_swidb (ulong dot10_colour,
					     hwidbtype *input_hwidb)
{

register said_descriptor *dot10_said_ptr;

    dot10_said_ptr = said_table[nethash(dot10_colour)];

    /*
     * Find out if a configured entry matches the SAID of the received packet.
     */

    while (dot10_said_ptr) {

        if (dot10_said_ptr->sdb_said == dot10_colour)
            break;
        dot10_said_ptr = dot10_said_ptr->next_said_db;
    }

    /*
     * If no SAID (vLAN colour ID) associated with
     * the colour of the received packet then bail.
     */

    if (dot10_said_ptr == NULL)
        return (NULL);

    return(dot10_said_ptr->sdb_sub_interface[input_hwidb->hw_if_index]);
}


/*
 * Check if we are configured to route the given protocol on this software
 * interface. The assumption with some of the protocols is that if the
 * interface has a (non-zero!) protocol network number associated with it
 * then it's routing that protocol.
 *
 * If so, then increment the dot10 vLAN packet counters.
 */

static inline boolean routing_dot10_vlan_on_subif (idbtype     *vlan_subint,
					           enum RXTYPE interior_rxtype)
{
    /*
     * When CRB is enabled, we must have the rxtype_flags
     * bit set to corresponding flag to decide a 
     * particular protocol on a interface is routable
     */
    if (ROUTING_RXTYPE(vlan_subint, interior_rxtype)) {
	/*
	 * The following is UGLY, but it is more efficient 
	 * (and predictable) than a GCC switch statement parsing 
	 * for sparse values since we can order based upon perceived 
	 * frequency of traffic i.e. IP before APOLLO!! ;-)
	 */

	if (interior_rxtype == RXTYPE_DODIP) {

	    if (RUNNING_IF(LINK_IP, vlan_subint)) {

		vlan_subint->sde_said_db->
			ingressing_dot10_packets[VLAN_ROUTE_IP]++;
		return (TRUE);

	    } else
		return (FALSE);

	} else if ((interior_rxtype == RXTYPE_NOVELL1) || 
		   (interior_rxtype == RXTYPE_NOVELL2) ||
		   (interior_rxtype == RXTYPE_SAP_NOVELL)) {

	    if (RUNNING_IF(LINK_NOVELL, vlan_subint)) {

		vlan_subint->sde_said_db->
			ingressing_dot10_packets[VLAN_ROUTE_NOVELL]++;
		return (TRUE);

	    } 
	    else
		return (FALSE);

	} else if (interior_rxtype == RXTYPE_APPLE) {
	    if (RUNNING_IF(LINK_APPLETALK, vlan_subint)) {

		vlan_subint->sde_said_db->
			ingressing_dot10_packets[VLAN_ROUTE_APPLE]++;
		return (TRUE);

	    } 
	    else
		return (FALSE);

	} else if (interior_rxtype == RXTYPE_DECNET) {

	    if (RUNNING_IF(LINK_DECNET, vlan_subint)) {

		vlan_subint->sde_said_db->
			ingressing_dot10_packets[VLAN_ROUTE_DECNET]++;
		return (TRUE);

	    } 
	    else
		return (FALSE);

	} else if (interior_rxtype == RXTYPE_CLNS) {

	    if (RUNNING_IF(LINK_CLNS, vlan_subint)) {

		vlan_subint->sde_said_db->
			ingressing_dot10_packets[VLAN_ROUTE_CLNS]++;
		return (TRUE);

	    } 
	    else
		return (FALSE);

	} else if (interior_rxtype == RXTYPE_XNS) {

	    if (RUNNING_IF(LINK_XNS, vlan_subint)) {

		vlan_subint->sde_said_db->
			ingressing_dot10_packets[VLAN_ROUTE_XNS]++;
		return (TRUE);

	    } 
	    else
		return (FALSE);

	} else if (interior_rxtype == RXTYPE_VINES) {

	    if (RUNNING_IF(LINK_VINES, vlan_subint)) {

		vlan_subint->sde_said_db->
			ingressing_dot10_packets[VLAN_ROUTE_VINES]++;
		return (TRUE);

	    } 
	    else
		return (FALSE);

	} else if (interior_rxtype == RXTYPE_APOLLO) {

	    if (RUNNING_IF(LINK_APOLLO, vlan_subint)) {

		vlan_subint->sde_said_db->
			ingressing_dot10_packets[VLAN_ROUTE_APOLLO]++;
		return (TRUE);

	    } 
	    else
		return (FALSE);
	}
    } else if (((interior_rxtype == RXTYPE_RFC826_ARP) || 
	       (interior_rxtype == RXTYPE_REVERSE_ARP)) &&
               RUNNING_IF(LINK_IP, vlan_subint)) {
	/*
	 * Deal with ARP and RARP packet here. Reason Being 
	 * ROUTING_RXTYPE is not working for oddball ARP nor 
	 * RARP packet. Tho all non-routable and non-bridgable
	 * dot10 packets will go to process path instead of being
	 * dropped as ISL packets, we still put check here to 
	 * increase the vLAN counter correctly.
	 */
	vlan_subint->sde_said_db->
		ingressing_dot10_packets[VLAN_ROUTE_IP]++;
	return (TRUE);
    }

    return (FALSE);
}


/*
 * Check for ISL vLAN encapsulation if we are configured to route the given
 * protocol on this software interface. The assumption with some of the
 * protocols is that if the interface has a (non-zero!) protocol network
 * number associated with it then it's routing that protocol.
 *
 * If so, then increment the ISL vLAN packet counters.
 */

static inline boolean routing_isl_vlan_on_subif (idbtype     *vlan_subint,
					         enum RXTYPE interior_rxtype)
{
    /*
     * When CRB is enabled, we must have the rxtype_flags
     * bit set to corresponding flag to decide a 
     * particular protocol on a interface is routable
     */
    if (ROUTING_RXTYPE(vlan_subint, interior_rxtype)) {
	/*
	 * The following is UGLY, but it is more efficient 
	 * (and predictable) than a GCC switch statement parsing 
	 * for sparse values since we can order based upon perceived 
	 * frequency of traffic i.e. IP before APOLLO!! ;-)
	 */

	if (interior_rxtype == RXTYPE_DODIP) {

	    if (RUNNING_IF(LINK_IP, vlan_subint)) {

		vlan_subint->isl_vlan->
			ingressing_isl_packets[VLAN_ROUTE_IP]++;
		return (TRUE);

	    } 
	    else
		return (FALSE);

	} else if ((interior_rxtype == RXTYPE_NOVELL1) || 
		   (interior_rxtype == RXTYPE_NOVELL2) ||
		   (interior_rxtype == RXTYPE_SAP_NOVELL)) {

	    if (RUNNING_IF(LINK_NOVELL, vlan_subint)) {

		vlan_subint->isl_vlan->
			ingressing_isl_packets[VLAN_ROUTE_NOVELL]++;
		return (TRUE);

	    } 
	    else
		return (FALSE);

	} else if (interior_rxtype == RXTYPE_APPLE) {

	    if (RUNNING_IF(LINK_APPLETALK, vlan_subint)) {

		vlan_subint->isl_vlan->
			ingressing_isl_packets[VLAN_ROUTE_APPLE]++;
		return (TRUE);

	    } 
	    else
		return (FALSE);

	} else if (interior_rxtype == RXTYPE_DECNET) {

	    if (RUNNING_IF(LINK_DECNET, vlan_subint)) {

		vlan_subint->isl_vlan->
			ingressing_isl_packets[VLAN_ROUTE_DECNET]++;
		return (TRUE);

	    } 
	    else
		return (FALSE);

	} else if (interior_rxtype == RXTYPE_CLNS) {

	    if (RUNNING_IF(LINK_CLNS, vlan_subint)) {

		vlan_subint->isl_vlan->
			ingressing_isl_packets[VLAN_ROUTE_CLNS]++;
		return (TRUE);

	    } 
	    else
		return (FALSE);

	} else if (interior_rxtype == RXTYPE_XNS) {

	    if (RUNNING_IF(LINK_XNS, vlan_subint)) {

		vlan_subint->isl_vlan->
			ingressing_isl_packets[VLAN_ROUTE_XNS]++;
		return (TRUE);

	    } else
		return (FALSE);

	} else if (interior_rxtype == RXTYPE_VINES) {

	    if (RUNNING_IF(LINK_VINES, vlan_subint)) {

		vlan_subint->isl_vlan->
			ingressing_isl_packets[VLAN_ROUTE_VINES]++;
		return (TRUE);

	    } 
	    else
		return (FALSE);

	} else if (interior_rxtype == RXTYPE_APOLLO) {

	    if (RUNNING_IF(LINK_APOLLO, vlan_subint)) {

		vlan_subint->isl_vlan->
			ingressing_isl_packets[VLAN_ROUTE_APOLLO]++;
		return (TRUE);

	    } 
	    else
		return (FALSE);
	}
    }	

    return (FALSE);
}


/*
 * Return the subinterface software IDB associated with the given ISL colour.
 * Uses a two byte hash to index into a table of ISL vLAN Descriptors and
 * locate the correct swidb.
 */

static inline idbtype *isl_colour_2_swidb (hwidbtype *input_hwidb,
					   ushort isl_colour)
{

register isl_vlan_descriptor *isl_vlan_ptr;
register uchar               bucket;

    bucket = (isl_colour >> 8);
    bucket ^= isl_colour; 

    isl_vlan_ptr = isl_vlan_table[bucket];

    /* Locate the correct ISL Descriptor Block */

    while (isl_vlan_ptr != NULL) {

        if (isl_vlan_ptr->colour == isl_colour)
            break;
        isl_vlan_ptr = isl_vlan_ptr->next_isl_vlan_db;
    }

     /*
      * Where we do not find a vLAN structure associated with the received
      * colour then drop the packet.
      *
      * The assumption here is that when we deconfigure a software IDB from
      * routing a particular vLAN ID (i.e. "colour") we blow away the
      * structure.
      */

    if (isl_vlan_ptr == NULL)
        return (NULL);

    return(isl_vlan_ptr->isl_vlan_subinterface[input_hwidb->hw_if_index]);
}


#endif /* __VLAN_PRIVATE_H__ */
