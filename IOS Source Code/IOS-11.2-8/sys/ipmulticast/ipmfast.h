/* $Id: ipmfast.h,v 3.2.60.2 1996/07/23 23:38:15 mleelani Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/ipmfast.h,v $
 *------------------------------------------------------------------
 * ipmfast.h - IP multicast fastswitching inline functions.
 *
 * December, 1994.
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ipmfast.h,v $
 * Revision 3.2.60.2  1996/07/23  23:38:15  mleelani
 * CSCdi61494:  Multicast fastswitching with incoming Dialer (ISDN) is
 * broken
 * Branch: California_branch
 * Support multicast fastswitching on incoming dialers.
 *
 * Revision 3.2.60.1  1996/05/09  14:31:14  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.92.1  1996/04/27  07:05:05  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.82.1  1996/04/15  07:26:31  shj
 * CSCdi52538:  IP multicast packets do not fastswitch to LANE
 * Branch: LE_Cal_V112_0_2_branch
 * Allow for variable length encapsulations on ATM.  LANE encaps is
 * different length from other ATM encaps.
 *
 * Revision 3.2  1995/11/17  17:29:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:01:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:03:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IPMFAST_H__
#define __IPMFAST_H__


/*
 * ipmulticast_hes_rxselect
 *
 * Read part of the IP header into the sniff buffer.
 */
static inline ulong ipmulticast_hes_rxselect (hwidbtype *hwidb, 
					      mciregtype *regptr)
{

    vc_info_t *vc;
    ulong     ipoffset;

    switch (hwidb->fast_switch_type) {
    case FS_FDDI:
	ipoffset = FDDI_RECEIVE_OFFSET + FDDI_SNAP_ENCAPWORDS;
	break;
    case FS_TOKEN:
	ipoffset = CTR_RECEIVE_OFFSET + TRING_SNAP_ENCAPWORDS;
	break;
    case FS_ETHER:
    case FS_HDLC:
    case FS_PPP:
    case FS_DIALER:
    case FS_FR:
    case FS_ATM_DXI:
    case FS_CHANNEL:
	ipoffset = MCI_OFFSETSIZE;
	break;
    case FS_AIP:
	regptr->argreg = hwidb->rxoffset;
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	vc = atm_getvc(hwidb->atm_db, regptr->readshort);
	if (!vc) return(FALSE);
	ipoffset = hwidb->rxoffset + btow(vc->encapsize);
	break;
    default:
	return(0);
    }

    /*
     * Read in IP header into sniff buffer.
     */
    regptr->argreg = ipoffset;
    regptr->cmdreg = MCI_CMD_RX_SELECT;

    /*
     * If tunnel encapsulated, offset is after MAC header and outer IP
     * header. Don't read IP header again. It was done in 
     * tunnel_ip_fastswitch() to determine if multicast fastswitching should
     * be called.
     */
    if (hwidb->rxtype == RXTYPE_IP_TUNNEL) {
	ipoffset += btow(sizeof(iphdrtype));
	if (hwidb->tunnel_swidb->hwptr->tunnel->mode == TUN_MODE_GRE_IP) {
	    ipoffset += 
		btow(GREHEADERBYTES(hwidb->tunnel_swidb->hwptr->tunnel));
	}
    } else {

	/*
	 * Version, ihl, tos, total length.
	 */
	hwidb->sniff[SNIFFBASE+3].d.lword = regptr->readlong;

	/*
	 * Ident, flags, fragment offset.
	 */
	hwidb->sniff[SNIFFBASE+4].d.lword = regptr->readlong;

	/*
	 * Ttl, prot, and checksum
	 */
	hwidb->sniff[SNIFFBASE+5].d.lword = regptr->readlong;

	/*
	 * Source and destination addresses.
	 */
	hwidb->sniff[SNIFFBASE+6].d.lword = regptr->readlong;
	hwidb->sniff[SNIFFBASE+7].d.lword = regptr->readlong;
    }

    /*
     * Need to get port numbers for extended access-list checking, sigh.
     */
    hwidb->sniff[SNIFFBASE+8].d.lword = regptr->readlong;
    return(ipoffset);
}

/*
 * ipmulticast_hes_write_tunnel_header
 *
 * Write GRE or DVMRP encapsulated headers to Cbus.
 */
static inline void ipmulticast_hes_write_tunnel_header (mciregtype *regptr, 
					tunnel_info *tunnel, ulong bytes)
{

    ulong     *ptr;
    iphdrtype *ip;

    ip = (iphdrtype *) ipheadstart(tunnel->header);
    ptr = (ulong *) ip;
    ip->tl = bytes + sizeof(iphdrtype);
    ip->id = tunnel->ident++;
    ip->checksum = 0;

    /*
     * Write an IP header only for DVMRP tunnels. For GRE tunnels, write
     * an IP header and a GRE header.
     */
    if (tunnel->mode == TUN_MODE_DVMRP_IP) {
	ip->checksum = ipcrc((ushort *) ip, sizeof(iphdrtype));
	regptr->write1long = *ptr++;
	regptr->write1long = *ptr++;
	regptr->write1long = *ptr++;
	regptr->write1long = *ptr++;
	regptr->write1long = *ptr;
    } else if (tunnel->mode == TUN_MODE_GRE_IP) {
	ip->tl += GREHEADERBYTES(tunnel);
	ip->checksum = ipcrc((ushort *) ip, sizeof(iphdrtype));
	regptr->write1long = *ptr++;
	regptr->write1long = *ptr++;
	regptr->write1long = *ptr++;
	regptr->write1long = *ptr++;
	regptr->write1long = *ptr;
	regptr->write1long = GRE_PTYPE_IP;
    }
}

/*
 * ipmulticast_hes_mac_rewrite_inline
 *
 * Write MAC header to buffer. 
 */
static inline ulong ipmulticast_hes_mac_rewrite_inline (hwidbtype *hwidb,
	mciregtype *regptr, ulong *mac_header, ulong mac_length, ulong offset,
	ulong bytes)
{

    tunnel_info *tunnel;
    ulong       length;

    /*
     * Check if this is a tunnel interface.
     */
    tunnel = (hwidb->tunnel && hwidb->tunnel->header) ? hwidb->tunnel : NULL;
    if (tunnel) {
	mac_length += sizeof(iphdrtype);
	if (tunnel->mode == TUN_MODE_GRE_IP) {
	    mac_length += GREHEADERBYTES(tunnel);
	}
	hwidb = tunnel->header->if_output->hwptr;
    }

    length = mac_length;

    switch (hwidb->fast_switch_type) {
    case FS_ETHER:
	regptr->argreg = offset - (length >> 1);
	regptr->cmdreg = MCI_CMD_TX1_SELECT;
	regptr->write1long = *mac_header++;
	regptr->write1long = *mac_header++;
	regptr->write1long = *mac_header++;
	regptr->write1short = *((ushort *) mac_header);
	break;
    case FS_FDDI:
	length += 1 + MCI_TX_ODDALIGN;
	regptr->argreg = offset - (mac_length >> 1) - 1;
	regptr->cmdreg = MCI_CMD_TX1_SELECT;
	regptr->write1short = FDDI_LLC_FC_BYTE;
	regptr->write1long = *mac_header++;
	regptr->write1long = *mac_header++;
	regptr->write1long = *mac_header++;
	regptr->write1long = *mac_header++;
	regptr->write1long = *mac_header;
	break;
    case FS_TOKEN:
	length += 2;
	regptr->argreg = offset - (length >> 1);
	regptr->cmdreg = MCI_CMD_TX1_SELECT;
	regptr->write1short = (AC_PRI0 << 8) | FC_TYPE_LLC;
	regptr->write1long = *mac_header++;
	regptr->write1long = *mac_header++;
	regptr->write1long = *mac_header++;
	regptr->write1long = *mac_header++;
	regptr->write1long = *mac_header;
	break;
    case FS_HDLC:
    case FS_PPP:
    case FS_FR:
	regptr->argreg = offset - btow(length);
	regptr->cmdreg = MCI_CMD_TX1_SELECT;
	regptr->write1long = *mac_header;
	break;
    case FS_SMDS:

	/*
	 * Alex says this can wait for 10.5 because Stu said this can wait.
	 * Turn fastswitching off. This only happens for the high-end.
         */
	hwidb->firstsw->ip_multicast_fs = FALSE;
	ip_mroute_fs_change = TRUE;
	break;
    case FS_AIP:
    {
        int     n;
        ushort *mac = (ushort *)mac_header;

	regptr->argreg = offset - btow(length);
	regptr->cmdreg = MCI_CMD_TX1_SELECT;
        for (n = btow(length); n; n--)
            regptr->write1short = *mac++;
	break;
    }
    case FS_ATM_DXI:
	regptr->argreg = offset - btow(length);
	regptr->cmdreg = MCI_CMD_TX1_SELECT;
	regptr->write1short = *((ushort *) mac_header);
	break;
    case FS_CHANNEL:
	regptr->argreg = offset - btow(length);
	regptr->cmdreg = MCI_CMD_TX1_SELECT;
	regptr->write1long = *mac_header;
	break;
    case FS_LEX:
	regptr->argreg = offset - btow(length);
	regptr->cmdreg = MCI_CMD_TX1_SELECT;
	regptr->write1long = *mac_header++;
	regptr->write1long = *mac_header++;
	regptr->write1long = *mac_header++;
	regptr->write1short = *((ushort *) mac_header);
	break;
    default:
	return(0);
    }

    /*
     * The underlying MAC header has already been written. If we are an
     * IP encapsulating tunnel, write the IP header now. Fill in IP header
     * fields that are not cached in tinfo->header.
     */
    if (tunnel) {
	ipmulticast_hes_write_tunnel_header(regptr, tunnel, bytes);
    }
    return(length);
}

#endif __IPMFAST_H__
