/* $Id: ipfast_pak.h,v 3.2.30.2 1996/07/30 01:08:20 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_pak.h,v $
 *------------------------------------------------------------------
 * ipfast_pak.h -- IP Fast Switching Definitions
 *
 * September 1, 1995, Bill May
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_pak.h,v $
 * Revision 3.2.30.2  1996/07/30  01:08:20  mbeesley
 * CSCdi55762:  Need to support fast IP multicasting on platforms
 * capable of scatter/gather IE c7200
 * Branch: California_branch
 *
 * Revision 3.2.30.1  1996/04/27  05:51:52  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.2.2.1  1996/02/23  20:23:46  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.2  1996/01/10  02:17:04  mleelani
 * CSCdi46648:  Spurious access in ip_pak_write_mac_inline()
 * Wrong fall-through in the ATM fast path.
 *
 * Revision 3.1  1995/11/09  11:57:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/07  23:10:02  wmay
 * CSCdi39353:  MIP to XXX fast switching failures - use common inline
 * for congestion path on high end and ip multicast, and make it go a
 * little more efficiently.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IPFAST_PAK_H__
#define __IPFAST_PAK_H__

/*
 * ip_pak_write_tunnel_header
 *
 * Write GRE or DVMRP encapsulated headers to a pak buffer.
 */
static inline void ip_pak_write_tunnel_header (paktype *pak,
					       tunnel_info *tunnel)
{

    iphdrtype *ip;
    gretype   *gre;
    ushort     size;

    ip = (iphdrtype *) ipheadstart(tunnel->header);

    if (tunnel->mode == TUN_MODE_GRE_IP) {
	size = sizeof(iphdrtype) + GREHEADERBYTES(tunnel);
    } else {
	size = sizeof(iphdrtype);
    }
    pak->datagramstart -= size;
    pak->datagramsize += size;
    /*
     * Copy cached header into buffer.
     */
    bcopy(ip, pak->datagramstart, size);
    ip = (iphdrtype *) pak->datagramstart;
    ip->tl = pak->datagramsize;
    ip->id = tunnel->ident++;
    ip->checksum = 0;
    ip->checksum = ipcrc((ushort *) ip, sizeof(iphdrtype));

    /*
     * Write only an IP header for DVMRP tunnels. For GRE tunnels, write
     * an IP header and a GRE header.
     */
    if (tunnel->mode == TUN_MODE_GRE_IP) {
	gre = (gretype *) ipdatastart_iph(ip);
	PUTLONG(gre, GRE_PTYPE_IP);
    }
}

/*
 * ip_pak_write_mac_inline :
 * Rewrite an encapsulation onto an outbound IP packet
 */
static inline void ip_pak_write_mac_inline (paktype *pak,
					    hwidbtype *hwidb,
					    ulong *macptr,
					    ulong mac_length,
					    ushort subint_ref_num)
{
    ulong *pktptr;
    ushort size;
    enum FSTYPE fast_switch_type;

    fast_switch_type = hwidb->fast_switch_type;

    pak->datagramstart = pak->network_start;
    size = pak->datagramsize;

    /*
     * Adjust packet size and starting point for each encapsulation type.
     * Write MAC header into new copy of packet. This switch only loops
     * for packets sent on tunnels.
     */
 switch_again:
    switch (fast_switch_type) {
    case FS_TUNNEL:
	ip_pak_write_tunnel_header(pak, hwidb->tunnel);
	size = pak->datagramsize;
	hwidb = hwidb->tunnel->header->if_output->hwptr;
	fast_switch_type = hwidb->fast_switch_type;
	goto switch_again;

    /*
     * Switch to the BVI(Bridge-group Virtual Interface).
     * Since it's a virtual ethernet i/f, we just treat
     * it as the FS_ETHER case
     */
    case FS_BVI:
    case FS_ETHER:
	size += ETHER_ARPA_ENCAPBYTES;
	if (size < MINETHERSIZE) size = MINETHERSIZE;
	pak->datagramstart -= ETHER_ARPA_ENCAPBYTES;
	pktptr = (ulong *)pak->datagramstart;

	PUTLONG(pktptr++, GETLONG(macptr++));
	PUTLONG(pktptr++, GETLONG(macptr++));
	PUTLONG(pktptr++, GETLONG(macptr++));
	PUTSHORT(pktptr, GETSHORT(macptr));
	break;

    case FS_FDDI: {
	uchar *tmpptr;
	
	mac_length++;
	size += mac_length;

	tmpptr = pak->datagramstart -= mac_length;
	*tmpptr++ = (FDDI_ASYNCHRONOUS_FRAME | FDDI_48BIT_ADDRESS |
		     FDDI_LLC_FRAME | FDDI_PRIORITY_ZERO);
	mac_length--;
	bcopy(macptr, tmpptr, mac_length);
    }
	break;

    case FS_HDLC:
	size += mac_length;
	pak->datagramstart -= mac_length;
	pktptr = (ulong *) pak->datagramstart;
	PUTLONG(pktptr++, GETLONG(macptr++));
	if (mac_length > sizeof(ulong)) {
	    PUTSHORT(pktptr, GETSHORT(macptr));
	}
	break;

    case FS_PPP: {
	ushort *macsptr, *cache_macsptr;
	ushort len, compare;
	
	size += mac_length;
	pak->datagramstart -= mac_length;
	macsptr = (ushort *)pak->datagramstart;
	cache_macsptr = (ushort *)macptr;
	
	for (len = mac_length, compare = sizeof(ushort);
	     len >= sizeof(ushort);
	     len -= sizeof(ushort)) {
	    PUTSHORT(macsptr++, *cache_macsptr++);
	}
	if (len) {
	    *(uchar *)macsptr = *(uchar *)cache_macsptr;
	}
    }
	break;

    case FS_FR:
	size += mac_length;
	pak->datagramstart -= mac_length;

	/*
	 * potential optimization point - could probably use the
	 * same format as hdlc.  However, I'm not sure if other
	 * combinations could leave us more than 4 (or 6 for the mip)
	 * bytes of header
	 */
	cache_copy_even_inline((ulong *)pak->datagramstart,
			       macptr,
			       mac_length);
	INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(hwidb, subint_ref_num),
			    size);
	break;

    case FS_SLIP:

	/*
	 * No MAC header.
	 */
	break;

    case FS_TOKEN: {
	ushort *sptr;
	
	mac_length += sizeof(ushort);
	size += mac_length;
	pak->datagramstart -= mac_length;
	sptr = (ushort *)pak->datagramstart;
	/*
	 * Build AC-FC (not in the cache) and copy dstaddr and srcaddr
	 * from the cache.
	 */
	PUTSHORT(sptr++, (AC_PRI0 << 8) | FC_TYPE_LLC);

	/*
	 * Copy the cache entry, including RIF, to the packet.
	 */
	bcopy(macptr, sptr, mac_length - sizeof(ushort));
    }
	break;

    case FS_LEX:
	size += mac_length;
	if (size < MINETHERSIZE) size = MINETHERSIZE;
	pak->datagramstart -= mac_length;
	pktptr = (ulong *)pak->datagramstart;

	PUTLONG(pktptr++, GETLONG(macptr++));
	PUTLONG(pktptr++, GETLONG(macptr++));
	PUTLONG(pktptr++, GETLONG(macptr++));
	PUTLONG(pktptr++, GETLONG(macptr++));
	PUTLONG(pktptr, GETLONG(macptr));
	break;

    case FS_ATM_DXI:
	size += mac_length;
	pak->datagramstart -= mac_length;

	cache_copy_even_inline((ulong *)pak->datagramstart,
			       macptr,
			       mac_length);
	INC_DXI_PVC_FAST_OUTPUT(HWIDB_TO_DXI_PVC(hwidb,
						 subint_ref_num),
				size);
	break;

    case FS_SMDS:
	/*
	 * Switch to SMDS.
	 */
	{
	    smdstype *sm;
	    smds_pdb_t *spd = hwidb->smds_pdb;
	    uint modelen, pad;
	    uchar *padptr;
	    modelen = spd->fast_output_modelen;

	    padptr = pak->datagramstart + size;
	    pak->datagramstart -= mac_length;
	    cache_copy_even_inline((ulong *)pak->datagramstart,
				   macptr,
				   mac_length);
	    pad = 3 - ((size + 3) % 4);
	    if (pad) {
	        PUTLONG(padptr, 0);
	        padptr += pad;
		size += pad;
	    }
	    sm = (smdstype *) (pak->datagramstart + modelen);
	    sm->smds_xhlpi &= ~3;
	    sm->smds_xhlpi |= pad;
	    sm->smds_betag = hwidb->smds_begin_tag++;
	    size += mac_length;
 	    sm->smds_basize = size - (sizeof(ulong) + modelen);

	    /*
	     * SMDS trailer
	     */
	    size += sizeof(ulong);
	    PUTLONG(padptr, GETLONG(&sm->smds_rsvd));
	}
	break;

      case FS_CHANNEL:
	pak->datagramstart -= mac_length;
	size += mac_length;
	bcopy(macptr, pak->datagramstart, mac_length);
	break;

      case FS_AIP:
	pak->atm_vcid = *(vcd_t *)macptr;
	pak->atm_enctype = *(ushort *)((uchar *)macptr + sizeof(vcd_t));
	size += mac_length;
	pak->datagramstart -= mac_length;
	cache_copy_even_inline((ulong *)pak->datagramstart,
			       macptr,
			       mac_length);
        break;

      case FS_DIALER:
	/*
	 * Get real output fastswitch type, and run through the switch again
	 * to get the work done by the appropriate encapsulation.
	 */
	fast_switch_type = get_dialerfstype(hwidb);
	goto switch_again;

    default:

	/*
	 * This really shouldn't happen, but we'll let process level
	 * handle it rather than just trash it
	 */
	break;
    }
    pak->datagramsize = size;
}

/*
 * ip_pak_write_tunnel_scattered_header:
 * Write GRE or DVMRP encapsulated headers to a pak buffer.
 */
static inline void ip_pak_write_tunnel_scattered_header (paktype *pak,
							 tunnel_info *tunnel,
							 particletype *rw_p)
{
    iphdrtype *ip;
    gretype   *gre;
    ushort     size;

    ip = (iphdrtype *) ipheadstart(tunnel->header);
    if (tunnel->mode == TUN_MODE_GRE_IP) {
	size = sizeof(iphdrtype) + GREHEADERBYTES(tunnel);
    } else {
	size = sizeof(iphdrtype);
    }

    /*
     * Adjust the packet
     */
    rw_p->data_bytes = size;
    pak->datagramstart = rw_p->data_start;
    pak->datagramsize += size;
    requeue_inline(&pak->particlequeue, rw_p);

    /*
     * Copy cached header into buffer.
     */
    bcopy(ip, rw_p->data_start, size);
    ip = (iphdrtype *) pak->datagramstart;
    ip->tl = pak->datagramsize;
    ip->id = tunnel->ident++;
    ip->checksum = 0;
    ip->checksum = ipcrc((ushort *) ip, sizeof(iphdrtype));

    /*
     * Write only an IP header for DVMRP tunnels. For GRE tunnels, write
     * an IP header and a GRE header.
     */
    if (tunnel->mode == TUN_MODE_GRE_IP) {
	gre = (gretype *) ipdatastart_iph(ip);
	PUTLONG(gre, GRE_PTYPE_IP);
    }
}

/*
 * ip_pak_write_mac_scattered_inline:
 * Rewrite an encapsulation onto an outbound IP packet (scattered)
 */
static inline boolean ip_pak_write_mac_scattered_inline (paktype *pak,
							 hwidbtype *hwidb,
							 ulong *macptr,
							 ulong mac_length,
							 ushort subint_ref_num)
{
    ushort size, *pktsptr;
    uchar *pktcptr;
    particletype *rw_p, *rw_ptemp;
    enum FSTYPE fast_switch_type;

    /*
     * Get a fastswitching particle
     */
    rw_p = allocate_fs_particle();
    if (!rw_p) {
	return(FALSE);
    }
    fast_switch_type = hwidb->fast_switch_type;
    pak->datagramstart = pak->network_start;
    size = pak->datagramsize;

    /*
     * Adjust packet size and starting point for each encapsulation type.
     * Write MAC header into new copy of packet. This switch only loops
     * for packets sent on tunnels.
     */
    switch_again:

    switch (fast_switch_type) {

    case FS_TUNNEL:
	rw_ptemp = allocate_fs_particle();
	if (!rw_ptemp) {
	    return(FALSE);
	}
	ip_pak_write_tunnel_scattered_header(pak, hwidb->tunnel, rw_ptemp);
	size = pak->datagramsize;
	hwidb = hwidb->tunnel->header->if_output->hwptr;
	fast_switch_type = hwidb->fast_switch_type;
	goto switch_again;

    case FS_FDDI:
	/*
	 * Put on an FDDI encapsulation
	 */
	mac_length += 1;
	pak->datagramstart = rw_p->data_start;
	size += mac_length;
	rw_p->data_bytes = mac_length;
	requeue_inline(&pak->particlequeue, rw_p);
	pktcptr = rw_p->data_start;

	/*
	 * Write in frame control byte
	 */
	*pktcptr++ = (FDDI_ASYNCHRONOUS_FRAME | FDDI_48BIT_ADDRESS |
		      FDDI_LLC_FRAME | FDDI_PRIORITY_ZERO);
	mac_length -= 1;

	/*
	 * Copy in the encapsulation
	 */
	bcopy(macptr, pktcptr, mac_length);
	break;

    case FS_FR:
	/*
	 * Put on a Frame Relay encapsulation
	 */
	pak->datagramstart = rw_p->data_start;
	size += mac_length;
	rw_p->data_bytes = mac_length;
	requeue_inline(&pak->particlequeue, rw_p);
	
	/*
	 * Rewrite the encapsulation
	 */
	bcopy(macptr, rw_p->data_start, mac_length);

	/*
	 * Do accounting
	 */
	INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(hwidb, subint_ref_num),
			    size);
	break;

    case FS_SLIP:
	/*
	 * No MAC header for SLIP.
	 */
	break;

    case FS_TOKEN:
	/*
	 * Put on a Token Ring encapsulation
	 */
	mac_length += sizeof(ushort);
	pak->datagramstart = rw_p->data_start;
	size += mac_length;
	rw_p->data_bytes = mac_length;
	requeue_inline(&pak->particlequeue, rw_p);
	pktsptr = (ushort *)rw_p->data_start;

	/*
	 * Build AC_FC (not in cache) and rewrite encapsulation
	 */
	*pktsptr++ = ((AC_PRI0 << 8) | FC_TYPE_LLC);
	bcopy(macptr, pktsptr, mac_length - sizeof(ushort));
	break;

    case FS_ATM_DXI:
	/*
	 * Put on an ATM-DXI encapsulation
	 */
	pak->datagramstart = rw_p->data_start;
	size += mac_length;
	rw_p->data_bytes = mac_length;
	requeue_inline(&pak->particlequeue, rw_p);

	/*
	 * Rewrite the encapsulation
	 */
	bcopy(macptr, rw_p->data_start, mac_length);

	/*
	 * Do accounting
	 */
	INC_DXI_PVC_FAST_OUTPUT(HWIDB_TO_DXI_PVC(hwidb,
						 subint_ref_num),
				size);
	break;

    case FS_SMDS:
    	{
	    smdstype *sm;
	    smds_pdb_t *spd = hwidb->smds_pdb;
	    uint pad;
	    uchar *padptr;
	    particletype *tail_p;
	    
	    /*
	     * Put on an SMDS encapsulation
	     */
	    pak->datagramstart = rw_p->data_start;
	    rw_p->data_bytes = mac_length;
	    requeue_inline(&pak->particlequeue, rw_p);
	    
	    /*
	     * Rewrite the encapsulation
	     */
	    bcopy(macptr, rw_p->data_start, mac_length);
	    
	    /*
	     * SMDS pad
	     */
	    tail_p = pak->particlequeue.qtail;
	    padptr = tail_p->data_start + tail_p->data_bytes;
	    pad = 3 - ((size + 3) % 4);
	    if (pad) {
		PUTLONG(padptr, 0);
		padptr += pad;
		size += pad;
		tail_p->data_bytes += pad;
	    }
	    
	    /*
	     * SMDS trailer
	     */
	    sm = (smdstype *)(pak->datagramstart + spd->fast_output_modelen);
	    sm->smds_xhlpi &= ~3;
	    sm->smds_xhlpi |= pad;
	    sm->smds_betag = hwidb->smds_begin_tag++;
	    size += mac_length;
	    sm->smds_basize = size - (sizeof(ulong) + 
				      spd->fast_output_modelen);
	    
	    /*
	     * SMDS trailer
	     */
	    size += sizeof(ulong);
	    PUTLONG(padptr, GETLONG(&sm->smds_rsvd));
	    tail_p->data_bytes += sizeof(ulong);
	}
	break;
	    
    case FS_AIP:
	/*
	 * Put on an ATM encapsulation
	 */
	pak->atm_vcid = *(vcd_t *)macptr;
	pak->atm_enctype = *(ushort *)((uchar *)macptr + sizeof(vcd_t));
	pak->datagramstart = rw_p->data_start;
	size += mac_length;
	rw_p->data_bytes = mac_length;
	requeue_inline(&pak->particlequeue, rw_p);

	/*
	 * Rewrite encapsulation
	 */
	bcopy(macptr, rw_p->data_start, mac_length);
        break;

    case FS_DIALER:
	/*
	 * Get real output fastswitch type, and run through the switch again
	 * to get the work done by the appropriate encapsulation.
	 */
	fast_switch_type = get_dialerfstype(hwidb);
	goto switch_again;

    case FS_ETHER:				/* Ether ARPA */
    case FS_PPP:				/* PPP */
    case FS_LEX:				/* LEX */
    case FS_HDLC:				/* HDLC */
    case FS_CHANNEL:				/* Channel */

	pak->datagramstart = rw_p->data_start;
	size += mac_length;
	rw_p->data_bytes = mac_length;
	requeue_inline(&pak->particlequeue, rw_p);

	/*
	 * Rewrite the encapsulation
	 */
	bcopy(macptr, rw_p->data_start, mac_length);
	break;

    default:
	/*
	 * This really shouldn't happen, but we'll let process level
	 * handle it rather than just trash it
	 */
	break;
    }

    /*
     * Adjust the size of the packet and return success
     */
    pak->datagramsize = size;
    return(TRUE);
}

#endif /* __IPFAST_PAK_H__ */
