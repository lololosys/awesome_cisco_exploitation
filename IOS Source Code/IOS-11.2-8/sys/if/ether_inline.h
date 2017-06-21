/* $Id: ether_inline.h,v 3.3.22.1 1996/05/21 20:43:50 wilber Exp $
 * $Source: /release/112/cvs/Xsys/if/ether_inline.h,v $
 *------------------------------------------------------------------
 * ether_inline.h - Inline functions for Ethernet frame classification
 *
 * May 1993, Scott Mackie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ether_inline.h,v $
 * Revision 3.3.22.1  1996/05/21  20:43:50  wilber
 * CSCdi53363:  Bridging of ipx raw between eth/fddi broken on 4500
 * Don't clear pak->info_start for Novell-ether packet otherwise bridging
 * will treat it as an ARPA packet
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/23  08:48:53  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.2  1995/11/17  09:27:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:02:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/12  21:57:51  pst
 * CSCdi39958:  ip ethernet 802.3 packets bridged when they should be
 * routed
 *
 * Revision 2.1  1995/06/07  20:51:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * ether_decode_encapsulation_inline
 *
 * Pull an incoming Ethernet/802.3 encapsulated frame into its various
 * elements, saving the majority of the context in the paktype header
 */

static inline ushort ether_decode_encapsulation_inline (paktype   *pak,
							ether_hdr *ether)
{
    ushort type = GETSHORT(&ether->type_or_len);

    pak->mac_start  = (uchar *)ether;
    pak->addr_start = (uchar *)ether;

    /*
     * Check to see if the frame has an LLC or ARPA encapsulation
     */
    if (type > MAXETHERSIZE) {
	/*
	 * It's an ARPA frame
	 */
	pak->network_start = ether->data;
	pak->encsize = ETHER_ARPA_ENCAPBYTES;
	pak->enctype = ET_ARPA;
	pak->info_start = NULL;
	/*
	 * Ugly, but more predictable than GCC switch statement
	 * parsing for sparse values
	 */
	if (type == TYPE_IP10MB) {
	    type = RXTYPE_DODIP;
	} else if (type == TYPE_NOVELL1) {
	    type = RXTYPE_NOVELL1;
	} else if (type == TYPE_ETHERTALK) {
	    type = RXTYPE_APPLE;
	    pak->enctype        = ET_ETHERTALK;
	    pak->network_start  = ether->data + ATALK_LAP_EXTRABYTES;
	    pak->encsize       += ATALK_LAP_EXTRABYTES;
	} else if (type == TYPE_VINES) {
	    type = RXTYPE_VINES;
	} else if (type == TYPE_DECNET) {
	    type = RXTYPE_DECNET;
	} else if (type == TYPE_XNS) {
	    type = RXTYPE_XNS;
	} else if (type == TYPE_APOLLO) {
	    type = RXTYPE_UNKNOWN;
	    pak->enctype        = ET_APOLLO;
	    pak->network_start  = ether->data + APOLLO_EXTRABYTES;
	    pak->encsize       += APOLLO_EXTRABYTES;
	} else if (type == TYPE_RFC826_ARP) {
	    type = RXTYPE_RFC826_ARP;
	} else if (type == TYPE_REVERSE_ARP) {
	    type = RXTYPE_REVERSE_ARP;

	/*
	 * Note, to allow for received datagrams enlarged by a vLAN
	 * header, we now must check for maximum MTU packets which are
	 * 802.3 format *but* with a length greater than MAXETHERSIZE.
	 */

        } else if (ieee_equal(isl_vlan_multicast, pak->datagramstart)) {

            type = RXTYPE_ISL;

        } else if (GETSHORT(pak->datagramstart + ETHER_8023_ENCAPBYTES)
		   == SAPSAP_SDE) {

	    type = RXTYPE_IEEE_802_10;

	} else {
	    type = RXTYPE_UNKNOWN;
	}

    } else {

	/*
	 * It's an LLC frame
	 */
	snap_hdr *snap = (snap_hdr *)ether->data;
	ushort lsap = GETSHORT(&snap->dsap);
	
	pak->info_start = (uchar *)snap;
	
	/*
	 * Is the frame SNAP encapsulated
	 */
	if (lsap == SNAPSNAP) {
	    pak->network_start = snap->data;
	    pak->encsize       = ETHER_SNAP_ENCAPBYTES;
	    pak->enctype       = ET_SNAP;

	    /*
	     * Make a check here for Inter Switch Link (ISL) vLAN encapsulation
	     * coming in on Fast Ethernet. This is identified by it using the
	     * Cisco multicast OUI for the Destination Address. We need to set
	     * up info_start, network_start ... here, these pointers will be
	     * used in routing/bridging code. We might not be able to set them
	     * in vLAN code because the image doesn't have vLAN subsystem;
	     * tbridge is called before fs when Input interface is Ethernet 
	     * not FastEthernet; or ISL paket's vLAN ID doesn't match the ID
	     * configured on the input interface.
	     */		  
	    if (ieee_equal(isl_vlan_multicast, pak->datagramstart))
		type = RXTYPE_ISL;
	    else {
		switch (GETSHORT(&snap->type)) {

		case TYPE_IP10MB:
		    type = RXTYPE_SNAP_DODIP;
		    break;
		case SAPSAP_SDE:
		    type = RXTYPE_IEEE_802_10;
		    break;
		case TYPE_VINES:
		case TYPE_VINES2:
		    type = RXTYPE_SNAP_VINES;
		    break;
		case TYPE_NOVELL1:
		    type = RXTYPE_SNAP_NOV1;
		    break;
		case TYPE_ETHERTALK:
		    type = RXTYPE_SNAP_APPLE;
		    break;
		case TYPE_RFC826_ARP:
		    type = RXTYPE_RFC826_ARP;
		    break;
		case TYPE_REVERSE_ARP:
		    type = RXTYPE_REVERSE_ARP;
		    break;
		case TYPE_RSRB:
		    type = RXTYPE_SRB;
		    break;
		default:
		    type = RXTYPE_SNAP_UNK;
		    break;
		}
	    }
	} else {
	    pak->network_start = snap->oui;
	    pak->encsize       = ETHER_SAP_ENCAPBYTES;
	    pak->enctype       = ET_SAP;
	    
	    switch (lsap) {

	      case SAPSAP_NOVELL_ETHER:
		/*
		 * Handle AFU Novell case
		 */
		pak->enctype       = ET_NOVELL_ETHER;
		pak->network_start = ether->data;
		pak->encsize       = ETHER_8023_ENCAPBYTES;
		type = RXTYPE_NOVELL2;
		break;

	      case SAPSAP_NOVELL:
		type = RXTYPE_SAP_NOVELL;
		break;

	      case SAPSAP_CLNS:
	        type = RXTYPE_CLNS;
		break;

	      case SAPSAP_IP:
	        type = RXTYPE_SAP_IP;
		break;

	      default:
		type = RXTYPE_UNKNOWN;
		if ((snap->dsap == SAP_IBMNM) ||
		    (snap->dsap == SAP_CONS)) {
		    int info_size = SAP_HDRBYTES;
		      
		    pak->llc2_sapoffset = ETHER_8023_ENCAPBYTES;
		    if ((snap->control & LLC1_CLASS) == LLC1_CLASS_U) {
			pak->enctype = ET_SAP;
			info_size = SAP_HDRBYTES;
		    } else {
			pak->enctype = ET_LLC2;
			info_size = SAP_LONGHDRBYTES;
			if ((snap->control & ~LLC1_P) == LLC1_XID)
			    info_size += 3;
		    }
		    pak->network_start = ether->data + info_size;
		    pak->encsize       = ETHER_8023_ENCAPBYTES + info_size;
		}
	    }
	}
    }
    return(type);
}
