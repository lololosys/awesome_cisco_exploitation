/* $Id: fddi_inline.h,v 3.2 1995/11/17 09:27:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/fddi_inline.h,v $
 *------------------------------------------------------------------
 * fddi_inline.h - Inline functions for FDDI frame classification
 *
 * June 1993, Scott Mackie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: fddi_inline.h,v $
 * Revision 3.2  1995/11/17  09:27:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:02:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/07  23:39:03  wfried
 * CSCdi37635:  FDDI frames w/ RIFs are not recognized prior to fast-sw
 * Prevent SRB packets with the RIF header from being mistakenly classified
 *
 * Revision 2.1  1995/06/07  20:52:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * fddi_decode_encapsulation_inline
 *
 * Pull apart and classify an incoming FDDI frame into it's various segments
 */
static inline ushort fddi_decode_encapsulation_inline (paktype *pak,
						       fddi_hdr *fddi,
						       uchar rii_mask)
{
    snap_hdr *snap;
    ushort lsap;
    ushort type;

    pak->mac_start   = (uchar *)&fddi->fc;
    pak->addr_start  = fddi->daddr;
    pak->riflen = get_fddi_riflen(fddi, rii_mask);

    pak->info_start = &fddi->data[pak->riflen];

    snap = (snap_hdr *)pak->info_start;
    lsap = GETSHORT(&snap->dsap);

    /*
     * Check to see if we've got a SNAP code
     * in the right place.
     */
    if (lsap == SNAPSNAP) {

	/*
	 * Okay. We've got an ISO2 packet so work
	 * out what type it is from the SNAP header
	 */
	pak->network_start = snap->data;
	pak->encsize       = FDDI_SNAP_ENCAPBYTES;

	switch (GETSHORT(&snap->type)) {
	  case TYPE_IP10MB:
	    type = RXTYPE_DODIP;
	    break;
	  case TYPE_DECNET:
	    type = RXTYPE_DECNET;
	    break;
	  case TYPE_XNS:
	    type = RXTYPE_XNS;
	    break;
	  case TYPE_ETHERTALK:
	    type = RXTYPE_APPLE;
	    break;
	  case TYPE_NOVELL1:
	    type = RXTYPE_SNAP_NOV1;
	    break;
	  case TYPE_VINES2:
	  case TYPE_VINES_ECHO2:
	  case TYPE_VINES:	/* optional */
	  case TYPE_VINES_ECHO:	/* optional */
	    type = RXTYPE_SNAP_VINES;
	    break;
	  case TYPE_DEC_SPANNING:
	    type = RXTYPE_UNKNOWN;
	    break;
	  case TYPE_RSRB:
	  case TYPE_DLSW:
	    type = RXTYPE_SRB;
	    break;
	  default:
	    type = RXTYPE_UNKNOWN;
	    break;
	}
    } else {
	/*
	 * It's a vanilla LLC encapsulation
	 */
	pak->network_start = snap->oui;
	pak->encsize       = FDDI_SAP_ENCAPBYTES;

	/* Check firstly for routing IEEE 802.10 vLAN encapsulation */

        if (lsap == SAPSAP_SDE) {
	    type = RXTYPE_IEEE_802_10;
	} else if (lsap == SAPSAP_CLNS) {
	    type = RXTYPE_CLNS;
	} else if (lsap == SAPSAP_NOVELL) {
	    type = RXTYPE_SAP_NOVELL;
	} else if (lsap == SAPSAP_NOVELL_ETHER) {
	    type = RXTYPE_NOVELL2;
	    pak->network_start = (uchar *)snap;
	    pak->encsize       = FDDI_ENCAPBYTES;
	} else
	    type = RXTYPE_SAP_UNKN;
    }

    return(type);
}


