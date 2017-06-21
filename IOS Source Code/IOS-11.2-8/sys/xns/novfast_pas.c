/* $Id: novfast_pas.c,v 3.1.60.8 1996/09/09 20:49:07 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/xns/novfast_pas.c,v $
 *------------------------------------------------------------------
 * novfast_pas.c - Particle based IPX fastswitching
 *
 * December 1995, David Carroll
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novfast_pas.c,v $
 * Revision 3.1.60.8  1996/09/09  20:49:07  mbeesley
 * CSCdi68071:  IPX fast switching performance awful with many
 * Spurious memory acces. Setup pak->if_output.
 * Branch: California_branch
 *
 * Revision 3.1.60.7  1996/08/14  14:12:21  dblair
 * CSCdi64968:  Implment outbound half-fastswitching for multilink PPP
 * Branch: California_branch
 * add multilink half-fastswitching support
 *
 * Revision 3.1.60.6  1996/08/14  02:37:52  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.1.60.5  1996/08/13  02:27:50  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.1.60.4  1996/06/23  15:05:56  boyle
 * CSCdi61041:  Input acl not logged with the action permit
 * Branch: California_branch
 *
 * Revision 3.1.60.3  1996/05/03  02:18:58  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.1.60.2  1996/04/27  06:47:58  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1.18.1  1996/04/26  00:11:56  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.1.60.1  1996/03/22  00:17:03  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.4  1996/03/19  00:02:18  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.2.3  1996/03/06  19:09:55  gstovall
 * CSCdi50773:  SMDS fastswitching fails without smds dxi-mode.
 * Branch: ELC_branch
 * Deal with the messed up encapsize in a better way.
 *
 * Revision 3.1.2.2  1996/02/27  23:10:33  dcarroll
 * CSCdi48856:  IPX fastswitching for serial SMDS broken
 * Branch: ELC_branch
 * Fix particle math to get to smds trailer.
 *
 * Revision 3.1.2.1  1995/12/22  20:57:26  dcarroll
 * Branch: ELC_branch
 * Implement particle based IPX fastswitching for Predator.
 *
 * Revision 3.1  1995/12/22  16:14:47  dcarroll
 * Add placeholder for new developement.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "address.h"
#include "xns.h"
#include "novell.h"
#include "novfast.h"
#include "access.h"
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "../wan/serial.h"
#include "../wan/ppp.h"
#include "../wan/dialer.h"
#include "../wan/dialer_private.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"
#include "../if/tring.h"
#include "../les/if_les.h"
#include "../if/atm.h"

#include "../tbridge/tbridge_sde.h"
#include "../vlan/vlan.h"

#include "../if/atm.h"
#include "../lane/lane.h"

/*
 * Forward declarations
 */
static boolean nov_acct_and_access_check_pas (paktype *, ipxhdrtype *,
					      idbtype *, boolean);

 
/*
 * Layout of datagram sniff buffer
 */

/*
 *	     ARPA	   ISO1		  ISO2		  HDLC
 *
 *	  D  D  D  D	D  D  D  D	D  D  D  D
 *   	  D  D  S  S	D  D  S  S	D  D  S  S
 *	  S  S  S  S	S  S  S  S	S  S  S  S
 *   0	  P  P  x  x	L  L  DS SS	L  L  DS SS	B  B  P  P
 *   1	  x  x  x  x	CT x  x  x	CT SN SN SN	x  x  x  x
 *   2	  x  x  x  x	x  x  x  x	SN SN P  P	x  x  x  x
 *   3	  x  x  x  x	x  x  x  x	x  x  x  x	x  x  x  x
 *   4    x  x  x  x	x  x  x  x	x  x  x  x	x  x  x  x
 *   5    x  x 		x  x  x  x 	x  x  x  x	x  x  x  x
 *   6    		x		x  x  x  x
 *   7    				x  x  x  x
 */

/* 
 * definitions
 */
typedef struct shortlong_ {
    union {
	ulong lword;
	ushort sword[2];
    } d;

} shortlong;

/*
 * Macros to handle DDR fastswitching.
 */
#define DDR_SETNEXT(OUT, DDB)						\
    output_fast_switch_type = output->dialerdb->fs_type;                \
    if (output->cache_board_encap != NULL) {                            \
      /*                                                                \
       * Set proper MIP channel.                                        \
       */                                                               \
      (*output->cache_board_encap)(output, path->novp_mh.mac_bytes,     \
                                   sizeof(ushort));                     \
    }

#define DDR_CLASSIFY(OFFSET, RESUME)					\
    /*									\
     * Tell lower layers that we have groveled in the packet more than	\
     * normal for the fast switch path and the packet may need to be 	\
     * flushed out of the data cache for some platforms:		\
     */									\
    pak_set_dirty(pak);							\
    if (!nov_accesscheck(pak, dialerlist))				\
	goto RESUME;


#ifdef DEBUG
void
display_trace (hwidbtype *input,
	       hwidbtype *output,
	       novpathtype *path)
{
    if (!novell_debug)
	return;

    if (input->status & IDB_ETHER)
	buginf(" (e");
    else
	buginf(" (s");
    buginf("->");
    if (output->status & IDB_ETHER)
	buginf("e) ");
    else
	buginf("s) ");
    switch (input->rxtype) {
      case RXTYPE_NOVELL1:
	buginf("(nov1");
	break;
      case RXTYPE_NOVELL2:
	if (input->status & IDB_FDDI) {
	    buginf(" raw");
	} else {
	    buginf("(nov2");
	}
	break;
      case RXTYPE_SNAP_NOV1:
	buginf("(snap");
	break;
      case RXTYPE_SAP_NOVELL:
	buginf("(iso1");
	break;
      default:
	if (input->status & IDB_SERIAL) {
	    switch (input->enctype) {
	    case ET_HDLC:
		buginf("(hdlc");
		break;
	    case ET_PPP:
		buginf("( ppp");
		break;
	    case ET_FRAME_RELAY:
		buginf("(  fr");
		break;
	    case ET_ATM_DXI:
		buginf("(atm-");
		break;
	    case ET_SMDS:
		buginf("(smds");
		break;
	    default:
		buginf("(????");
		break;
	    }
	}
	break;
    }
    buginf("->%s)", encstring(path->novp_nidb_ptr->enctype));
}
#endif


/*
 * ipx_fast_supported
 *
 * Determine whether fastswitch support present for given hw/config
 *
 */
static boolean ipx_pas_fast_supported (int linktype, hwidbtype *hwidb)
{
    if (!hwidb || (linktype != LINK_NOVELL)) {
	return(FALSE);
    }
    /*
     * Ethernet:
     */
    if (hwidb->status & IDB_ETHER) {
	return(TRUE);
    }
    /*
     * Fddi:
     */
    if (hwidb->status & IDB_FDDI) {
	return(TRUE);
    }
    /*
     * Tokenring:
     */
    if (hwidb->status & IDB_TR) {
	return(TRUE);
    }
    /*
     * Serial: MK5025, HD64570, M32 and BRI supported if encap is hdlc/ppp.
     *         fr only if ddr is *not* configured
     */
    if (hwidb->status & IDB_SERIAL) {
	if ((hwidb->enctype == ET_HDLC) ||
	     (is_ppp(hwidb) && !is_multilink(hwidb)) ||
	     (is_frame_relay(hwidb) && !is_ddr(hwidb)) ||
	     is_frame_relay(hwidb) ||
	     is_atm_dxi(hwidb) ||
	     is_smds(hwidb)) {
	    return(TRUE);
	}
    }
    /*
     * ATM: all flavors
     */
    if (is_atm(hwidb)) {
	return(TRUE);
    }

    if (hwidb->status & IDB_LEX) {
	return(TRUE);
    }

    return(FALSE);
}

static inline void copy_cache (ushort cache_len, ulong *mac, ulong *cache)
{
    register ushort compare;

    compare = sizeof(ulong);

    while (cache_len >= compare) {
	PUTLONG(mac++, GETLONG(cache++));
	cache_len -= compare;
    }
    if (cache_len) {
	PUTSHORT(mac, GETSHORT(cache));
    }
}

static inline void copy_cache_maybe_odd (ushort cache_len, ulong *mac, ulong *cache)
{
    register ushort compare;

    compare = sizeof(ulong);

    while (cache_len >= compare) {
	PUTLONG(mac++, GETLONG(cache++));
	cache_len -= compare;
    }
    if (cache_len) {
	ushort *macs, *caches;
	macs = (ushort *)mac;
	caches = (ushort *)cache;
	if (cache_len & 0x2)
	    PUTSHORT(macs++, GETSHORT(caches++));
	if (cache_len & 0x01)
	    *(uchar *)macs = *(uchar *)caches;
    }
}

/*
 * fs_nov_ether_rewrite:
 * Perform ether encaps rewrite for novell
 */
static inline boolean fs_nov_ether_rewrite(paktype *pak,
					   novpathtype *path,
					   ushort temp_length,
					   idbtype *swoutput,
					   hwidbtype *output)
{
    int enctype;
    ulong *macptr, *cache_macptr;
    ushort *macsptr, *cache_macsptr;
    int delta;
    particletype *rwp;
    
    enctype = path->novp_nidb_ptr->enctype;
    
    if (enctype == ET_SAP) {
	/*
	 * Encapsulation guaranteed to fit in first particle,
	 * no need to get a new one.
	 */
	delta = pak->datagramstart - ipxheadstart(pak) + ETHER_SAP_ENCAPBYTES;
	particle_clip_inline(pak->particlequeue.qhead, delta);
	pak->datagramstart -= delta;
	
	macptr = (ulong *)pak->datagramstart;
	cache_macptr = path->novp_mh.mac_longs;
	
	/*
	 * Write the DA and SA
	 */
	PUTLONG(macptr++, *cache_macptr++);
	PUTLONG(macptr++, *cache_macptr++);
	PUTLONG(macptr++, *cache_macptr++);
	
	/*
	 * Write the IEEE802.3 length
	 */
	macsptr = (ushort *)macptr;
	PUTSHORT(macsptr++, ROUND(temp_length + SAP_HDRBYTES));
	
	/*
	 * Write the LLC header
	 */
	PUTSHORT(macsptr++, SAPSAP_NOVELL);
	*(uchar *)macsptr = LLC1_CLASS_U;
	
	temp_length += ETHER_SAP_ENCAPBYTES;
	
    } else if (enctype == ET_SNAP) {
	/*
	 * Encapsulation guaranteed to fit in first particle,
	 * no need to get a new one.
	 */
	delta = pak->datagramstart - ipxheadstart(pak) + ETHER_SNAP_ENCAPBYTES;
	particle_clip_inline(pak->particlequeue.qhead, delta);
	pak->datagramstart -= delta;
	
	macptr = (ulong *)pak->datagramstart;
	cache_macptr = path->novp_mh.mac_longs;
	
	/*
	 * Write the DA and SA
	 */
	PUTLONG(macptr++, *cache_macptr++);
	PUTLONG(macptr++, *cache_macptr++);
	PUTLONG(macptr++, *cache_macptr++);
	
	/*
	 * Write the IEEE802.3 length
	 */
	macsptr = (ushort *)macptr;
	PUTSHORT(macsptr++, ROUND(temp_length + SNAP_HDRBYTES));
	
	/*
	 * Write the SNAP header
	 */
	PUTSHORT(macsptr++, SNAPSNAP);
	macptr = (ulong *)macsptr;
	PUTLONG(macptr++, OLD_ETHEROUI);
	macsptr = (ushort *)macptr;
	PUTSHORT(macsptr++, TYPE_NOVELL1);
	
	temp_length += ETHER_SNAP_ENCAPBYTES;
	
    } else if (enctype == ET_ISL) {
	
	/* ISL vLAN encapsulation */
	
	/*
	 * Get re-write particle. If we fail to allocate
	 * it, then punt to process level.
	 */
	rwp = allocate_fs_particle();
	if(!rwp) {
	    return (FALSE);
	}
	
	/*
	 * Fixup pointers. Strip off the encapsulation from
	 * the first particle, and point to the rewrite
	 * particle for datagram start.
	 */
	particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
	rwp->data_bytes = ISL_HEADER_LENGTH + ETHER_8023_ENCAPBYTES;
	pak->datagramstart = rwp->data_start;
	requeue_inline(&pak->particlequeue, rwp);
	
	macptr = (ulong *)pak->datagramstart;
	cache_macptr = path->novp_mh.mac_longs;
	
	/*
	 * Write the exterior ISL header, followed
	 * by the Novell-Ethernet format, for now.
	 *
	 * Mark the packet as requiring ISL Inner CRC
	 * generation; done by the FEIP ucode until the
	 * MICE hardware assist is available at which
	 * time the "PAK_DO_ISL_CRC" becomes redundant.
	 */
        PUTLONG(macptr++, *cache_macptr++);
        PUTLONG(macptr++, *cache_macptr++);
        PUTLONG(macptr++, *cache_macptr++);
        PUTLONG(macptr++, *cache_macptr++);
        PUTLONG(macptr++, *cache_macptr++);
        PUTLONG(macptr++, *cache_macptr++);
        PUTLONG(macptr++, *cache_macptr++);
        PUTLONG(macptr++, *cache_macptr++);
        PUTLONG(macptr++, *cache_macptr++);

        cache_macsptr = (ushort *) cache_macptr;
        macsptr       = (ushort *) macptr;

        PUTSHORT(macsptr++, *cache_macsptr);
        PUTSHORT(macsptr, ROUND(temp_length));

        /*
         * Now write the checksum field of the IPX header. Note that the
         * IPX header is not in the head particle of the packet.
         */
        macsptr = (ushort *)ipxheadstart(pak);
        PUTSHORT(macsptr, NOVELL_NO_CHECKSUM);
	
	temp_length += (ETHER_8023_ENCAPBYTES + ISL_HEADER_LENGTH);
	
	pak->flags |= PAK_DO_ISL_CRC;
	
	swoutput->isl_vlan->
	    egressing_isl_packets[VLAN_ROUTE_NOVELL]++;
	
    } else if (enctype == ET_SDE) {
	
	/* 802.10 vLAN encapsulation, maybe with SNAP */
	/*
	 * Get re-write particle. If we fail to allocate
	 * it, then punt to process level.
	 */
	rwp = allocate_fs_particle();
	if(!rwp) {
	    return (FALSE);
	}
	
	/*
	 * Fixup pointers. Strip off the encapsulation from
	 * the first particle, and point to the rewrite
	 * particle for datagram start.
	 */
	particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
	
	if (path->total_encap_bytes == SDE_ENCAPBYTES +
	    ETHER_8023_ENCAPBYTES) {
	    
	    rwp->data_bytes = SDE_ENCAPBYTES + ETHER_8023_ENCAPBYTES;
	    
	    temp_length += (ETHER_8023_ENCAPBYTES +
			    SDE_ENCAPBYTES);
	} else {
	    
	    rwp->data_bytes = SDE_ENCAPBYTES + ETHER_SNAP_ENCAPBYTES;
	    
	    temp_length += (ETHER_SNAP_ENCAPBYTES +
			    SDE_ENCAPBYTES);
	}
	
	pak->datagramstart = rwp->data_start;
	requeue_inline(&pak->particlequeue, rwp);
	
	macptr = (ulong *)pak->datagramstart;
	
	cache_macptr = path->novp_mh.mac_longs;
	
	write_ether_dot10_cache_macheader(
					  macptr, cache_macptr,
					  (temp_length -
					   ETHER_8023_ENCAPBYTES),
					  path->total_encap_bytes);
	
	swoutput->sde_said_db->
	    egressing_dot10_packets[VLAN_ROUTE_NOVELL]++;
	
    } else {
	delta = pak->datagramstart - ipxheadstart(pak) + ETHER_ARPA_ENCAPBYTES;
	particle_clip_inline(pak->particlequeue.qhead, delta);
	pak->datagramstart -= delta;
	
	macptr = (ulong *)pak->datagramstart;
	cache_macptr = path->novp_mh.mac_longs;
	
	/*
	 * Write the DA and SA
	 */
	PUTLONG(macptr++, *cache_macptr++);
	PUTLONG(macptr++, *cache_macptr++);
	PUTLONG(macptr++, *cache_macptr++);
	
	/*
	 * Write either the Novell Type or IEEE802.3 length
	 * (the latter is the 0xFFFF bogon Novell encap).
	 */
	macsptr = (ushort *)macptr;
	if (enctype == ET_ARPA) {
	    PUTSHORT(macsptr, TYPE_NOVELL1);
	} else {
	    PUTSHORT(macsptr++, ROUND(temp_length));
	    PUTSHORT(macsptr, NOVELL_NO_CHECKSUM);
	}
	
	temp_length += ETHER_ARPA_ENCAPBYTES;	
    }
    
    /*
     * Set datagramsize. If this is an odd length datagram, round it off.
     */
    pak->datagramsize = temp_length;
    if (temp_length & 1) {
        rwp = pak->particlequeue.qtail;
        rwp->data_bytes++;
        pak->datagramsize++;
    }

    pak->if_output = swoutput;
    (*output->fastsend)(output, pak);
    return(TRUE);
}

/*
 * fs_nov_fddi_rewrite:
 * Perform fddi encaps rewrite for novell
 */
static inline boolean fs_nov_fddi_rewrite(paktype *pak,
					   novpathtype *path,
					   ushort temp_length,
					   idbtype *swoutput,
					   hwidbtype *output)
{
    int enctype;
    ulong *macptr, *cache_macptr;
    ushort *macsptr, *cache_macsptr;
    int delta, i;
    particletype *rwp;
    
    /*
     * For FDDI, the rewrite can sometimes fit over the original
     * encapsulation area. Other times it will not fit for a 
     * large RIF. Test the delta of the encapsulation and if it
     * is too large, then allocate an additional particle to 
     * add to the beginning of the particle queue.
     */
    enctype = path->novp_nidb_ptr->enctype;
    delta = path->total_encap_bytes + pak->datagramstart - 
	ipxheadstart(pak);
    
    if (enctype == ET_SAP) {
	if (delta > PARTICLE_ENCAPBYTES) {
	    rwp = allocate_fs_particle();
	    if(!rwp) {
		return (FALSE);
	    }
	    particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
	    rwp->data_bytes = path->total_encap_bytes;
	    pak->datagramstart = rwp->data_start;
	    requeue_inline(&pak->particlequeue, rwp);
	} else {
	    particle_clip_inline(pak->particlequeue.qhead, delta);
	    pak->datagramstart -= delta;
	}
	
	/*
	 * Write the FC control byte
	 */
	*pak->datagramstart = FDDI_ASYNCHRONOUS_FRAME |
	    FDDI_48BIT_ADDRESS |
		FDDI_LLC_FRAME |
		    FDDI_PRIORITY_ZERO;
	macptr = (ulong *)((fddi_hdr *)pak->datagramstart)->daddr;
	cache_macptr = path->novp_mh.mac_longs;
	
	/*
	 * Write the DA and SA
	 */
	PUTLONG(macptr++, *cache_macptr++);
	PUTLONG(macptr++, *cache_macptr++);
	PUTLONG(macptr++, *cache_macptr++);
	
	/*
	 * Write the RIF, if required
	 */
	macsptr = (ushort *)macptr;
	if (path->rifwords > 0) {
	    cache_macsptr = (ushort *)cache_macptr;
	    for (i = 0; i < path->rifwords; i++) {
		PUTSHORT(macsptr++, *cache_macsptr++);
	    }
	}
	
	/*
	 * Write the LLC header
	 */
	PUTSHORT(macsptr++, SAPSAP_NOVELL);
	*(uchar *)macsptr = LLC1_CLASS_U;
	
	pak->datagramsize = temp_length + path->total_encap_bytes;
    } else if (enctype == ET_SNAP) {
	if (delta > PARTICLE_ENCAPBYTES) {
	    rwp = allocate_fs_particle();
	    if(!rwp) {
		return (FALSE);
	    }
	    particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
	    rwp->data_bytes = path->total_encap_bytes;
	    pak->datagramstart = rwp->data_start;
	    requeue_inline(&pak->particlequeue, rwp);
	} else {
	    particle_clip_inline(pak->particlequeue.qhead, delta);
	    pak->datagramstart -= delta;
	}
	
	/*
	 * Write the FC control byte
	 */
	*pak->datagramstart = FDDI_ASYNCHRONOUS_FRAME |
	    FDDI_48BIT_ADDRESS |
		FDDI_LLC_FRAME |
		    FDDI_PRIORITY_ZERO;
	macptr = (ulong *)((fddi_hdr *)pak->datagramstart)->daddr;
	cache_macptr = path->novp_mh.mac_longs;
	
	/*
	 * Write the DA and SA
	 */
	PUTLONG(macptr++, *cache_macptr++);
	PUTLONG(macptr++, *cache_macptr++);
	PUTLONG(macptr++, *cache_macptr++);
	
	/*
	 * Write the RIF, if required
	 */
	macsptr = (ushort *)macptr;
	if (path->rifwords > 0) {
	    cache_macsptr = (ushort *)cache_macptr;
	    for (i = 0; i < path->rifwords; i++) {
		PUTSHORT(macsptr++, *cache_macsptr++);
	    }
	    cache_macptr = (ulong *)cache_macsptr;
	}
	/*
	 * Write the SNAP header
	 */
	macptr = (ulong *)macsptr;
	
	PUTLONG(macptr++, *cache_macptr++);
	PUTLONG(macptr++, *cache_macptr++);
	pak->datagramsize = temp_length +
	    path->total_encap_bytes;
	
    } else if (enctype == ET_NOVELL_FDDI) {
	if (delta > PARTICLE_ENCAPBYTES) {
	    rwp = allocate_fs_particle();
	    if(!rwp) {
		return (FALSE);
	    }
	    particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
	    rwp->data_bytes = path->total_encap_bytes;
	    pak->datagramstart = rwp->data_start;
	    requeue_inline(&pak->particlequeue, rwp);
	} else {
	    particle_clip_inline(pak->particlequeue.qhead, delta);
	    pak->datagramstart -= delta;
	}
	
	/*
	 * Write the FC control byte
	 */
	*pak->datagramstart = FDDI_ASYNCHRONOUS_FRAME |
	    FDDI_48BIT_ADDRESS |
		FDDI_LLC_FRAME |
		    FDDI_PRIORITY_ZERO;
	macptr = (ulong *)((fddi_hdr *)pak->datagramstart)->daddr;
	cache_macptr = path->novp_mh.mac_longs;
	
	/*
	 * Write the DA and SA
	 */
	PUTLONG(macptr++, *cache_macptr++);
	PUTLONG(macptr++, *cache_macptr++);
	PUTLONG(macptr++, *cache_macptr++);
	
	/*
	 * Write the RIF, if required
	 */
	macsptr = (ushort *)macptr;
	if (path->rifwords > 0) {
	    cache_macsptr = (ushort *)cache_macptr;
	    for (i = 0; i < path->rifwords; i++) {
		PUTSHORT(macsptr++, *cache_macsptr++);
	    }
	    cache_macptr = (ulong *)cache_macsptr;
	}
	/*
	 * Write no header whatsoever:
	 */
	pak->datagramsize = temp_length + path->total_encap_bytes;
	
    } else if (enctype == ET_SDE) {
	
	/* IEEE 802.10 vLAN on FDDI, maybe with SNAP */
	
	rwp = allocate_fs_particle();
	if(!rwp) {
	    return (FALSE);
	}
	particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
	rwp->data_bytes = path->total_encap_bytes;
	pak->datagramstart = rwp->data_start;
	requeue_inline(&pak->particlequeue, rwp);
	
	/*
	 * Write the FC control byte.
	 */
	
	*pak->datagramstart = FDDI_ASYNCHRONOUS_FRAME |
	    FDDI_48BIT_ADDRESS |
		FDDI_LLC_FRAME |
		    FDDI_PRIORITY_ZERO;
	macptr = (ulong *)
	    ((fddi_hdr *)pak->datagramstart)->daddr;
	
	cache_macptr = path->novp_mh.mac_longs;
	
	if (path->total_encap_bytes == (FDDI_SNAP_ENCAPBYTES
	    + SDE_ENCAPBYTES - SNAP_HDRBYTES)) {
            /* write DA + SA + SDE */
            PUTLONG(macptr++, *cache_macptr++);
            PUTLONG(macptr++, *cache_macptr++);
            PUTLONG(macptr++, *cache_macptr++);
            PUTLONG(macptr++, *cache_macptr++);
            PUTLONG(macptr++, *cache_macptr++);
            PUTLONG(macptr++, *cache_macptr++);
            PUTLONG(macptr++, *cache_macptr++);
        } else if (path->total_encap_bytes == (FDDI_SNAP_ENCAPBYTES + 
                                               SDE_ENCAPBYTES)) {
            write_dot10_cache_macheader(macptr, cache_macptr, TRUE);
        } else {
            write_dot10_cache_macheader(macptr, cache_macptr, FALSE);
        }
	
	pak->datagramsize = temp_length +
	    path->total_encap_bytes;
	
	swoutput->sde_said_db->
	    egressing_dot10_packets[VLAN_ROUTE_NOVELL]++;
    }

    pak->if_output = swoutput;
    (*output->fastsend)(output, pak);
    
    return(TRUE);
}    

/*
 * fs_nov_channel_rewrite:
 * Perform channel encaps rewrite for novell
 */
static inline boolean fs_nov_channel_rewrite(paktype *pak,
					   novpathtype *path,
					   ushort temp_length,
					   idbtype *swoutput,
					   hwidbtype *output)
{
    particletype *rwp;

    /*
     * Fast Switch to Channel output interface:
     * Encapsulation not guaranteed to fit in first particle.
     * Get a new particle and prepend to the front of the 
     * particle queue. Adjust pointers accordingly.
     */
    rwp = allocate_fs_particle();
    if(!rwp) {
	return (FALSE);
    }
    particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
    rwp->data_bytes = path->total_encap_bytes;
    pak->datagramstart = rwp->data_start;
    requeue_inline(&pak->particlequeue, rwp);
    
    pak->datagramsize =
	temp_length + path->total_encap_bytes;
    bcopy(path->novp_mh.mac_longs, pak->datagramstart,
	  path->total_encap_bytes);
    
    pak->if_output = swoutput;
    (*output->fastsend)(output, pak);
    return(TRUE);
}
    
/*
 * fs_nov_token_rewrite:
 * Perform token encaps rewrite for novell
 */
static inline boolean fs_nov_token_rewrite(paktype *pak,
					   novpathtype *path,
					   ushort temp_length,
					   idbtype *swoutput,
					   hwidbtype *output)
{
    int enctype;
    ulong *macptr, *cache_macptr;
    ushort *macsptr, *cache_macsptr;
    int i;
    particletype *rwp;

    enctype = path->novp_nidb_ptr->enctype;
    
    if (enctype == ET_SAP) {
	/* 
	 * Get a particle to hold the encapsulation data.
	 * Adjust the pointers and prepend the particle
	 * to the front of the queue.
	 */
	rwp = allocate_fs_particle();
	if(!rwp) {
	    return (FALSE);
	}
	particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
	rwp->data_bytes = path->total_encap_bytes;
	pak->datagramstart = rwp->data_start;
	requeue_inline(&pak->particlequeue, rwp);
	
	/*
	 * Write the AC and FC control bytes
	 */
	PUTSHORT(pak->datagramstart,
		 ((AC_PRI0 << 8) | FC_TYPE_LLC));
	macptr = (ulong *)((tring_hdr *)pak->datagramstart)->daddr;
	cache_macptr = path->novp_mh.mac_longs;
	
	/*
	 * Write the DA and SA
	 */
	PUTLONG(macptr++, *cache_macptr++);
	PUTLONG(macptr++, *cache_macptr++);
	PUTLONG(macptr++, *cache_macptr++);
	
	/*
	 * Write the RIF, if required
	 */
	macsptr = (ushort *)macptr;
	if (path->rifwords > 0) {
	    cache_macsptr = (ushort *)cache_macptr;
	    for (i = 0; i < path->rifwords; i++) {
		PUTSHORT(macsptr++, *cache_macsptr++);
	    }
	}
	
	/*
	 * Write the LLC header
	 */
	PUTSHORT(macsptr++, SAPSAP_NOVELL);
	*(uchar *)macsptr = LLC1_CLASS_U;
	
	pak->datagramsize = temp_length + path->total_encap_bytes;
    } else if (enctype == ET_SNAP) {
	/* 
	 * Get a particle to hold the encapsulation data.
	 * Adjust the pointers and prepend the particle
	 * to the front of the queue.
	 */
	rwp = allocate_fs_particle();
	if(!rwp) {
	    return (FALSE);
	}
	particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
	rwp->data_bytes = path->total_encap_bytes;
	pak->datagramstart = rwp->data_start;
	requeue_inline(&pak->particlequeue, rwp);
	
	/*
	 * Write the AC and FC control bytes
	 */
	PUTSHORT(pak->datagramstart,
		 ((AC_PRI0 << 8) | FC_TYPE_LLC));
	macptr = (ulong *)((tring_hdr *)pak->datagramstart)->daddr;
	cache_macptr = path->novp_mh.mac_longs;
	
	/*
	 * Write the DA and SA
	 */
	PUTLONG(macptr++, *cache_macptr++);
	PUTLONG(macptr++, *cache_macptr++);
	PUTLONG(macptr++, *cache_macptr++);
	
	/*
	 * Write the RIF, if required
	 */
	if (path->rifwords > 0) {
	    macsptr = (ushort *)macptr;
	    cache_macsptr = (ushort *)cache_macptr;
	    for (i = 0; i < path->rifwords; i++) {
		PUTSHORT(macsptr++, *cache_macsptr++);
	    }
	    macptr = (ulong *)macsptr;
	    cache_macptr = (ulong *)cache_macsptr;
	}
	
	/*
	 * Write the SNAP header
	 */
	PUTLONG(macptr++, GETLONG(cache_macptr++));
	PUTLONG(macptr++, GETLONG(cache_macptr++));
	
	pak->datagramsize = temp_length +
	    path->total_encap_bytes;
	
    } else if (enctype == ET_SDE) {
	
	/* IEEE 802.10 vLAN encapsulation maybe with SNAP
	 * Get a particle to hold the encapsulation data.
	 * Adjust the pointers and prepend the particle
	 * to the front of the queue.
	 */
	rwp = allocate_fs_particle();
	if(!rwp) {
	    return (FALSE);
	}
	particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
	rwp->data_bytes = path->total_encap_bytes;
	pak->datagramstart = rwp->data_start;
	requeue_inline(&pak->particlequeue, rwp);
	
	/*
	 * Write the AC and FC control bytes.
	 */
	
	PUTSHORT(pak->datagramstart,
		 ((AC_PRI0 << 8) | FC_TYPE_LLC));
	
	macptr = (ulong *)
	    ((tring_hdr *)pak->datagramstart)->daddr;
	
	cache_macptr = path->novp_mh.mac_longs;
	
	
	if (path->total_encap_bytes == FDDI_SAP_ENCAPBYTES +
	    SDE_ENCAPBYTES -
	    TR_ACFC_SIZE)
	    
	    write_dot10_cache_macheader(macptr,
					cache_macptr,
					FALSE);
	else
	    
	    write_dot10_cache_macheader(macptr,
					cache_macptr,
					TRUE);
	
	pak->datagramsize = temp_length +
	    path->total_encap_bytes;
	
	swoutput->sde_said_db->
	    egressing_dot10_packets[VLAN_ROUTE_NOVELL]++;
    }
    
    pak->if_output = swoutput;
    (*output->fastsend)(output, pak);
    
    return(TRUE);
}
    
/*
 * fs_nov_hdlc_ppp_rewrite:
 * Perform hdlc_ppp encaps rewrite for novell
 */
static inline boolean fs_nov_hdlc_ppp_rewrite(paktype *pak,
					   novpathtype *path,
					   ushort temp_length,
					   idbtype *swoutput,
					   hwidbtype *output)
{
    ushort *macsptr, *cache_macsptr;
    int i;
    int delta;

    /*
     * Encapsulation guaranteed to fit in first particle,
     * no need to get a new one. 
     */
    delta = path->total_encap_bytes + pak->datagramstart - 
	ipxheadstart(pak);
    particle_clip_inline(pak->particlequeue.qhead, delta);
    pak->datagramstart -= delta;

    pak->datagramsize =
	temp_length + path->total_encap_bytes;
    /*
     * Write the header ... assume minimum 4 bytes:
     */
    PUTLONG(pak->datagramstart, path->novp_mh.mac_longs[0]);
    if (path->total_encap_bytes > HDLC_ENCAPBYTES) {
	i = path->total_encap_words -
	    btow(HDLC_ENCAPBYTES);
	macsptr =
	    (ushort*)pak->datagramstart+
		btow(HDLC_ENCAPBYTES);
	cache_macsptr =
	    (ushort*)&path->novp_mh.mac_longs[1];
	while (i-- > 0) {
	    PUTSHORT(macsptr++, *cache_macsptr++);
	}
    }
    pak->if_output = swoutput;
    (*output->fastsend)(output, pak);
    return(TRUE);
}
    
/*
 * fs_nov_fr_rewrite:
 * Perform fr encaps rewrite for novell
 */
static inline boolean fs_nov_fr_rewrite(paktype *pak,
					   novpathtype *path,
					   ushort temp_length,
					   idbtype *swoutput,
					   hwidbtype *output)
{
    int delta;
    
    /*
     * Encapsulation guaranteed to fit in first particle,
     * no need to get a new one. 
     */
    delta = path->total_encap_bytes + pak->datagramstart - 
	ipxheadstart(pak);
    particle_clip_inline(pak->particlequeue.qhead, delta);
    pak->datagramstart -= delta;

    pak->datagramsize =
	temp_length + path->total_encap_bytes;
    /*
     * Write the header ... assume minimum 4 bytes:
     */
    copy_cache(path->total_encap_bytes,
	       (ulong *)pak->datagramstart,
	       path->novp_mh.mac_longs);
    INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(output,
					path->subint_reference_num),
			pak->datagramsize);
    pak->if_output = swoutput;
    (*output->fastsend)(output, pak);
    return(TRUE);
}    

/*
 * fs_nov_atm_dxi_rewrite:
 * Perform atm_dxi encaps rewrite for novell
 */
static inline boolean fs_nov_dxi_rewrite(paktype *pak,
					   novpathtype *path,
					   ushort temp_length,
					   idbtype *swoutput,
					   hwidbtype *output)
{
    int delta;
    particletype *rwp;

    /*
     * Test whether a new particle is needed for the encaps.
     */
    delta = path->length + pak->datagramstart - ipxheadstart(pak);
    if (delta > PARTICLE_ENCAPBYTES) {
	rwp = allocate_fs_particle();
	if(!rwp) {
	    return (FALSE);
	}
	particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
	rwp->data_bytes = path->length;
	pak->datagramstart = rwp->data_start;
	requeue_inline(&pak->particlequeue, rwp);
    } else {
	particle_clip_inline(pak->particlequeue.qhead, delta);
	pak->datagramstart -= delta;
    }
    
    pak->datagramsize =
	temp_length + path->length;
    pak->datagramstart =
	ipxheadstart(pak) - path->length;
    copy_cache(path->total_encap_bytes,
	       (ulong *)pak->datagramstart,
	       path->novp_mh.mac_longs);
    INC_DXI_PVC_FAST_OUTPUT(HWIDB_TO_DXI_PVC(output,
					     path->subint_reference_num),
			    pak->datagramsize);
    pak->if_output = swoutput;
    (*output->fastsend)(output, pak);
    return(TRUE);
}    

/*
 * fs_nov_smds_rewrite:
 * Perform smds encaps rewrite for novell
 */
static inline boolean fs_nov_smds_rewrite(paktype *pak,
					   novpathtype *path,
					   ushort temp_length,
					   idbtype *swoutput,
					   hwidbtype *output)
{
    smdstype *sm;
    smds_pdb_t *spd;
    uint len,pad;
    uchar *trailptr;
    particletype *rwp, *tail_p;
    
    /*
     * Fixup packet pointers. Strip off the encapsulation 
     * from the first particle, setup the new encaps particle
     * and append it onto the packet.
     */
    rwp = allocate_fs_particle();
    if(!rwp) {
	return (FALSE);
    }
    particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
    rwp->data_bytes = path->total_encap_bytes;
    pak->datagramstart = rwp->data_start;
    requeue_inline(&pak->particlequeue, rwp);

    spd = output->smds_pdb;
    /*
     * write the encap header
     */
    copy_cache(path->total_encap_bytes,
	       (ulong *)pak->datagramstart,
	       path->novp_mh.mac_longs);
    /*
     * Calculate the trailer pointer.  Calculate
     * the number of pad bytes and the other
     * smds per packet fields
     */
    len = temp_length;
    tail_p = pak->particlequeue.qhead;
    tail_p = tail_p->next;
    while (tail_p != pak->particlequeue.qtail) {
	temp_length -= tail_p->data_bytes;
	tail_p = tail_p->next;
    }
    trailptr = (uchar *)(tail_p->data_start + temp_length);

    pad = 3 - ((len + 3) % 4);
    if (pad) {
	PUTLONG(trailptr, 0);
	trailptr += pad;
	len	 += pad;
	tail_p->data_bytes += pad;
    }
    sm = (smdstype *)(pak->datagramstart +
		      spd->fast_output_modelen);
    sm->smds_xhlpi &= ~3;
    sm->smds_xhlpi |= pad;
    sm->smds_betag = output->smds_begin_tag++;
    len += path->total_encap_bytes;
    PUTSHORT(&sm->smds_basize,
	     (len - (sizeof(ulong) +
		     spd->fast_output_modelen)));
    pak->datagramsize = len + sizeof(ulong);
    tail_p->data_bytes += sizeof(ulong);
    /* trailer copy */
    PUTLONG(trailptr, GETLONG(&sm->smds_rsvd));

    pak->if_output = swoutput;
    (*output->fastsend)(output,pak);
    return(TRUE);
}

/*
 * fs_nov_aip_rewrite:
 * Perform aip encaps rewrite for novell
 */
static inline boolean fs_nov_aip_rewrite(paktype *pak,
					   novpathtype *path,
					   ushort temp_length,
					   idbtype *swoutput,
					   hwidbtype *output)
{
    int enctype;
    int delta;
    particletype *rwp = NULL;
    ushort *ptr;

    /*
     * Fast Switch to ATM
     */
    pak->atm_vcid = path->novp_mh.mac_shorts[0];
    pak->atm_enctype = path->novp_mh.mac_shorts[1];
    /*
     * Test whether a new particle is needed for the encaps.
     */
    delta = path->total_encap_bytes + pak->datagramstart - ipxheadstart(pak);
    if (delta > PARTICLE_ENCAPBYTES) {
	rwp = allocate_fs_particle();
	if(!rwp) {
	    return (FALSE);
	}
	particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
	rwp->data_bytes = path->total_encap_bytes;
	pak->datagramstart = rwp->data_start;
	requeue_inline(&pak->particlequeue, rwp);
    } else {
	particle_clip_inline(pak->particlequeue.qhead, delta);
	pak->datagramstart -= delta;
    }
    copy_cache_maybe_odd(path->total_encap_bytes,
			 (ulong *)pak->datagramstart,
			 path->novp_mh.mac_longs);
    /*
     * for LANE, must insert correct "ethernet" length
     * for all encaps except ARPA, and force cksum to
     * FFFF for NOVELL_ETHER encap.
     */
    enctype = path->novp_nidb_ptr->enctype;
    if (lec_enabled(swoutput) && (enctype != ET_ARPA)) {
	ether_hdr   *ehead =
	    (ether_hdr *)(pak->datagramstart +
			  ATM_AAL5_LANE_ENCAPBYTES);
	
	PUTSHORT(&ehead->type_or_len,
		 ROUND(pak->datagramsize -
		       ETHER_ARPA_ENCAPBYTES - 
		       ATM_AAL5_LANE_ENCAPBYTES));
	if (enctype == ET_NOVELL_ETHER) {
            /*
             * Now write the checksum field of the IPX header. Note that the
             * IPX header may not be in the head particle of the packet.
             */
            ptr = (ushort *)ipxheadstart(pak);
            PUTSHORT(ptr, NOVELL_NO_CHECKSUM);
	}
    }
    pak->if_output = swoutput;
    (*output->fastsend)(output, pak);
    return(TRUE);
}    

/*
 * fs_nov_lex_rewrite:
 * Perform lex encaps rewrite for novell
 */
static inline boolean fs_nov_lex_rewrite(paktype *pak,
					   novpathtype *path,
					   ushort temp_length,
					   idbtype *swoutput,
					   hwidbtype *output)
{
    int enctype;
    ulong *macptr;
    ushort *macsptr, *cache_macsptr;
    int delta;
    particletype *particle;
    
    enctype = path->novp_nidb_ptr->enctype;
    
    if (enctype == ET_SAP) {
	/*
	 * Encapsulation guaranteed to fit in first particle,
	 * no need to get a new one. 
	 */
	delta = path->total_encap_bytes + pak->datagramstart - 
	    ipxheadstart(pak);
	particle_clip_inline(pak->particlequeue.qhead, delta);
	pak->datagramstart -= delta;

	macsptr = (ushort *)pak->datagramstart;
	cache_macsptr = path->novp_mh.mac_shorts;
	
	/*
	 * Write Lex Header.
	 */
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	
	/*
	 * Write the DA and SA
	 */
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	
	/*
	 * Write the IEEE802.3 length
	 */
	PUTSHORT(macsptr++, ROUND(temp_length + SAP_HDRBYTES));
	
	/*
	 * Write the LLC header
	 */
	PUTSHORT(macsptr++, SAPSAP_NOVELL);
	*(uchar *)macsptr = LLC1_CLASS_U;
	
	temp_length += path->length;
    } else if (enctype == ET_SNAP) {
	/*
	 * Encapsulation guaranteed to fit in first particle,
	 * no need to get a new one. 
	 */
	delta = path->total_encap_bytes + pak->datagramstart - 
	    ipxheadstart(pak);
	particle_clip_inline(pak->particlequeue.qhead, delta);
	pak->datagramstart -= delta;

	macsptr = (ushort *)pak->datagramstart;
	cache_macsptr = path->novp_mh.mac_shorts;
	
	/*
	 * Write Lex Header.
	 */
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	
	/*
	 * Write the DA and SA
	 */
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	
	/*
	 * Write the IEEE802.3 length
	 */
	PUTSHORT(macsptr++, ROUND(temp_length + SNAP_HDRBYTES));
	
	/*
	 * Write the SNAP header
	 */
	PUTSHORT(macsptr++, SNAPSNAP);
	macptr = (ulong *)macsptr;
	PUTLONG(macptr++, OLD_ETHEROUI);
	macsptr = (ushort *)macptr;
	PUTSHORT(macsptr++, TYPE_NOVELL1);
	
	temp_length += path->length;
    } else {
	/*
	 * Encapsulation guaranteed to fit in first particle,
	 * no need to get a new one. 
	 */
	delta = path->total_encap_bytes + pak->datagramstart - 
	    ipxheadstart(pak);
	particle_clip_inline(pak->particlequeue.qhead, delta);
	pak->datagramstart -= delta;

	macsptr = (ushort *)pak->datagramstart;
	cache_macsptr = path->novp_mh.mac_shorts;
	
	/*
	 * Write Lex Header.
	 */
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	
	/*
	 * Write the DA and SA
	 */
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	PUTSHORT(macsptr++, *cache_macsptr++);
	
	/*
	 * Write either the Novell Type or IEE802.3 length
	 * (the latter is the 0xFFFF bogon Novell encap).
	 */
	if (enctype == ET_ARPA) {
	    PUTSHORT(macsptr, TYPE_NOVELL1);
	} else {
	    PUTSHORT(macsptr++, ROUND(temp_length));
	    PUTSHORT(macsptr, NOVELL_NO_CHECKSUM);
	}
	
	temp_length += path->length;
    }
    
    /*
     * Set datagramsize. If this is an odd length datagram, round it off.
     */
    pak->datagramsize = temp_length;
    if (temp_length & 1) {
        particle = pak->particlequeue.qtail;
        particle->data_bytes++;
        pak->datagramsize++;
    }

    pak->if_output = swoutput;
    (*output->fastsend)(output, pak);
    return(TRUE);
} 

/*
 * nov_pas_sanitize_pak:
 * Sanitize the IPX frame to make sure that the various length
 * indicators in the frame all agree with each other. PCs acting as
 * Novell clients and servers tend to transmit some trailing bytes at
 * the end that should be ignored. On exit from this routine, the packet
 * would have been adjusted such that length indicated in the IPX header
 * will be equal to the datagramsize indicated in the paktype header less
 * the size of the layer 2 encapsulation.
 */
static inline void nov_pas_sanitize_pak (paktype *pak, ipxhdrtype *ipx,
                                         ushort ipxlen)
{
    particletype *particle;
    ushort delta, encsize;
 
    /*
     * Compute the size of the layer 2 encapsulation.
     */
    encsize = (ulong)ipx - (ulong)pak->datagramstart;
 
    /*
     * If the size indicated in the length field of the IPX header is less
     * than the actual number of bytes received as part of the IPX frame,
     * then there are some garbage bytes at the end. Snip these excess
     * bytes off from the particle(s) at the tail of the particle queue.
     */
    if (ipxlen < (pak->datagramsize - encsize)) {
        delta = pak->datagramsize - encsize - ipxlen;
        pak->datagramsize -= delta;
        particle = pak->particlequeue.qtail;
        while (particle && (particle->data_bytes <= delta)) {
            delta -= particle->data_bytes;
            unqueue_inline(&pak->particlequeue, particle);
            retparticle(particle);
            particle = pak->particlequeue.qtail;
        }
        particle->data_bytes -= delta;
    }
}


/*
 * nov_fastswitch()
 *
 * The code below has been lovingly hand-crafted to provide a lifetime of
 * reliable use and happiness. ;-)
 *
 * Don't change this stuff unless you're prepared to spend time poring
 * over hot and sweaty assembler listings and HP analyzer timing output...
 */
boolean nov_fastswitch (paktype *pak)
{
    novcachetype *cache;
    novpathtype *path;
    hwidbtype *input, *output;
    idbtype *swoutput;
    idbtype *swinput, *master_idb;
    shortlong net, addrhigh;
    ushort addrlow, temp_length;
    ipxhdrtype *ipx;
    ulong indx;
    enum FSTYPE output_fast_switch_type;

    /*
     * Don't let non-particled paks through. This can happen if
     * a particled pak is coalesced before it comes here, as in the 
     * case of tunneling. This file only supports particled paks.
     */
    if (!pak_has_particles(pak)) {
	return (FALSE);
    }

    /*
     * Get pointer to IPX header based on input encapsulation:
     */
    input = pak->if_input->hwptr;
    pak->linktype = LINK_NOVELL;
    ipx = (ipxhdrtype *)ipxheadstart(pak);
    temp_length = GETSHORT(&ipx->len);

    /* Sanity check the ipx length in the ipx header */
    if (temp_length > ((ulong) pak->datagramsize -
                       ((ulong) ipx - (ulong) pak->datagramstart)))
      return(FALSE);

    /*
     *  multilink ppp is not yet ready for particle fastswitching
     */
    if (is_multilink(input))
	return(FALSE);

    /* If spx spoofing is enabled, force processing in the slow path */
    if (is_ddr(input)) {
      /* get the dialer hw interface */
      master_idb = get_netcfg_idb(input->firstsw);
      if (master_idb && master_idb->hwptr->novell_spxspoof && 
	  (ipx->pt == NOVELL_TYPE_SPX))
	return(FALSE);
    } else {
      if (input->novell_spxspoof && (ipx->pt == NOVELL_TYPE_SPX))
	return(FALSE);
    }
    /* Check if hop count valid (including reserved tc bits) */
    if (ipx->tc < novell_max_hops) {

      nov_hop_count_ok:
	net.d.lword      =  GETLONG(ipx->ddnet);
	addrhigh.d.lword =  GETLONG(ipx->dhost);
	addrlow          = GETSHORT(ipx->dhost+4);

	indx = net.d.lword;
	indx ^= addrhigh.d.lword;
	indx ^= addrlow;
	indx ^= (indx >> 16);
	indx ^= (indx >> 8);
	indx &=0xFF;

	for (cache = novcache[indx]; cache; cache = cache->novc_next_ptr) {
	    if ((*(ushort *)&cache->novc_dest_address[4] == addrlow) &&
		(*(ulong *)cache->novc_dest_address == addrhigh.d.lword) &&
		(cache->novc_dest_net == (ulong) net.d.lword)) {
		if (cache->novc_version == novcache_version) {
		    path = cache->novc_path_next_ptr;

		    swoutput = path->novp_idb_ptr;
		    output = swoutput->hwptr;

		    /* Check total size; dump to slow switch if too big */
		    if ((temp_length + path->total_encap_bytes) >
			output->max_pak_size) {
			return(FALSE);
		    }

		    /*
		     * Remote routes round-robin if multiple cache
		     * entries exist:
		     */
		    cache->novc_path_next_ptr = path->novp_next_ptr;

		    /*
		     * Check if an access list is enabled on *any* interface.
		     * If so, check if *this* interface. If so, filter.
		     */
 		    if (novell_global_acl_checks || cache->novc_bcast) {
 			if (swoutput->novell_output_acl_checks ||
 			    input->novell_input_acl_checks ||
			    cache->novc_bcast) {
			    /*
			     * Tell lower layers that we have groveled in the
			     * packet more than normal for the fast switch path
			     * and the packet may need to be flushed out of the
			     * data cache for some platforms:
			     */
			    pak_set_dirty(pak);
			    
 			    /*
 			     * Run input swidb match iff input-access-group:
 			     */
 			    if (input->novell_input_acl_checks) {
 				if (input->status & IDB_IEEEMASK) {
 				    FOR_ALL_SWIDBS_ON_HW(input, swinput) {
 					if (swinput->novellnets &&
 					    (swinput->primary_nidb->enctype ==
 					     pak->enctype)) {
 					    pak->if_input = swinput;
 					    break;
 					}
 				    }
 				}
 			    }
			    if (!nov_acct_and_access_check_pas
				(pak, ipx, swoutput, cache->novc_bcast))
				return(FALSE);
			}
		    }

                    /*
                     * Sanitize the packet so that all the length fields in
                     * the packet match. Any extra garbage bytes at the end
                     * of the frame will be clipped off. In particular, the 
                     * SMDS trailer will get snipped off here.
                     */
                    nov_pas_sanitize_pak(pak, ipx, temp_length);

		    /*
		     * Do some accounting of packets and bytes
		     * in and out.
		     */
		    novell_stat[NOVELL_INPUT]++;
		    novell_stat[NOVELL_FORWARDED]++;
		    
		    /*
		     * Bail if source net is 0:
		     */
		    if (GETLONG(ipx->ssnet) == 0)
			return(FALSE);
		    
		    input->counters.inputs_fast[ACCT_PROTO_NOVELL]++;
		    input->counters.rx_cumbytes_fast[ACCT_PROTO_NOVELL] +=
			pak->datagramsize;
		    pak->acct_proto = ACCT_PROTO_NOVELL;
		    /*
		     * Update the hop count:
		     */
		    if (novell_igrp_running == 0 ||
			!NOVELL_IGRP_HOPCOUNT_ALGORITHM_ALLOWED) {
			/* Always do 4 -> 4 if IGRP not running on this box */
			ipx->tc += 1;
		    } else if (ipx->tc <= XNS_MAX_HOPS) {
			if (path->igrp_route) {
			    /* 4 -> 8 */
			    if (ipx->tc == 0)
				ipx->tc = 1;
			    ipx->tc += 1;
			    if (ipx->tc <= XNS_MAX_HOPS)
				ipx->tc = (ipx->tc * 16);
			} else {
			    /* 4 -> 4 */
			    ipx->tc += 1;
			}
		    } else {
			if (path->igrp_route) {
			    /* 8 -> 8 */
			    ipx->tc += 1;
			} else {
			    /* 8 -> 4 */
			    ipx->tc = (ipx->tc / 16);
			}
		    }

		    output_fast_switch_type = output->fast_switch_type;

		    /*
		     * Re entry point for DDR fastswitching.  output now points
		     * to the physical output interface.
		     */
		  resume_dialer:
		    
		    switch (output_fast_switch_type) {

		      case FS_ETHER:
			return(fs_nov_ether_rewrite(pak, path, temp_length, swoutput, output));
			
		      case FS_FDDI:
			return(fs_nov_fddi_rewrite(pak, path, temp_length, swoutput, output));
			
		      case FS_CHANNEL:
			return(fs_nov_channel_rewrite(pak, path, temp_length, swoutput, output));
			
		      case FS_TOKEN:
			return(fs_nov_token_rewrite(pak, path, temp_length, swoutput, output));

		      case FS_HDLC:
		      case FS_PPP:
			return(fs_nov_hdlc_ppp_rewrite(pak, path, temp_length, swoutput, output));

		      case FS_FR:
			return(fs_nov_fr_rewrite(pak, path, temp_length, swoutput, output));

		    case FS_ATM_DXI:
			return(fs_nov_dxi_rewrite(pak, path, temp_length, swoutput, output));

		    case FS_SMDS:
			return(fs_nov_smds_rewrite(pak, path, temp_length, swoutput, output));

		    case FS_AIP:
			return(fs_nov_aip_rewrite(pak, path, temp_length, swoutput, output));

		    case FS_LEX:
			return(fs_nov_lex_rewrite(pak, path, temp_length, swoutput, output));
			
		      case FS_DIALER:
			/*
			 * Switch to a DDR interface (serial, dialer, PRI, BRI)
			 */
			{
			    DDR_FASTSWITCH(input, output, cache, DDR_SETNEXT,
					   ipx_dialerlist, DDR_CLASSIFY, 0,
					   path->dialergroup,
					   resume_dialer);
			}

		      case FS_TUNNEL:
			/*
			 * GRE fastswitching.
			 */
			pak->if_output = swoutput;
			(*output->fastsend)(output, pak);
			return(TRUE);

		      default:
			return(FALSE);

		    }    /* end of switch of FS_TYPE */
		}
	    }
	}
	/*
	 * The max hop count may have been reached. However, now do a
	 * comprehensive check to determine if the hop count is excessive.
	 */
    } else {
	if (NOVELL_IGRP_HOPCOUNT_ALGORITHM_ALLOWED) {
	    if (((ipx->tc >= XNS_IGRP_MIN_HOPS) &&
		 (ipx->tc <  XNS_IGRP_MAX_HOPS)) ||
		(ipx->tc  < (XNS_MAX_HOPS+1))) {
		goto nov_hop_count_ok;
	    }
	}
    }
    return(FALSE);
}

inline static boolean inline_item_denied (xnseacctype *item,
 					  int list,
 					  uchar proto,
 					  ulong dst_net,
 					  ulong dst_host_1long,
 					  ushort dst_host_2short,
 					  ushort dst_socket,
 					  ulong src_net,
 					  ulong src_host_1long,
 					  ushort src_host_2short,
 					  ushort src_socket)
{
    ulong flags;

    do {
	flags = item->flags;
       	/*
	 * A network of -1 indicates all XNS networks.
	 * A socket of 0 indicates all XNS sockets.
	 */
	if (flags & XNS_SRCNET) { 
	    if (item->srcnet != NOVELL_TYPE_ANY_NET) {
		if (flags & XNS_SRCNETMASK) {
		    if (item->srcnet != (src_net & item->srcnetmask))
			continue;
		} else {
		    if (item->srcnet != src_net)
			continue;
		}
	    }
	}
	if (flags & XNS_DSTNET) { 
	    if (item->dstnet != NOVELL_TYPE_ANY_NET) {
	      if (flags & XNS_DSTNETMASK) {
		if (item->dstnet != (dst_net & item->dstnetmask))
		  continue;
	      } else {
		if (item->dstnet != dst_net)
		  continue;
	      }
	    }
	}
	if (flags & XNS_SRCHOST) {
	    /*
	     * Check for match on source host address
	     */
	    if (flags & XNS_SRCHOSTMASK) {
		if ((src_host_2short & item->srchostmask.u.sword[2])
		    != item->srchost.u.sword[2])
		    continue;
		if ((src_host_1long & item->srchostmask.u.lword[0])
		    != (item->srchost.u.lword[0]))
		    continue;
	    } else {
		if (src_host_2short != item->srchost.u.sword[2])
		    continue;
		if (src_host_1long != item->srchost.u.lword[0])
		    continue;
	    }
	}
	if (flags & XNS_DSTHOST) {
	    /*
	     * Check for match on source host address
	     */
	  if (flags & XNS_DSTHOSTMASK) {
	    if ((dst_host_2short & item->dsthostmask.u.sword[2])
		!= item->dsthost.u.sword[2])
		continue;
	    if ((dst_host_1long & item->dsthostmask.u.lword[0])
		!= (item->dsthost.u.lword[0]))
		continue;
	  } else {
	    if (dst_host_2short != item->dsthost.u.sword[2])
	      continue;
	    if (dst_host_1long != item->dsthost.u.lword[0])
	      continue;
	  }
	}

	/* check if it is extended access list */
	if (list > MAXNOVACC ) {
	    if (flags & XNS_PROTOCOL) {
		if ((item->protocol != (ushort) -1) &&
		    (item->protocol != proto))
		  continue;
	    }
	    if (flags & XNS_SRCSOCKET) {
		if (item->srcsocket && (item->srcsocket != src_socket))
		  continue;
	    }
	    if (flags & XNS_DSTSOCKET) {
		if(item->dstsocket && (item->dstsocket != dst_socket))
		    continue;
	    }
	    if (item->log)
		return(TRUE);
	}

#ifdef DEBUG
if ((!item->grant) && novell_debug) buginf("*deny*");
#endif

	return(!item->grant);
	
    } while ((item = item->next));

#ifdef DEBUG
if (novell_debug) buginf("**deny**");
#endif

    /* Got here -- access list says implied deny */
    return(TRUE); /* Return "item_denied" */
}

static boolean item_denied (xnseacctype *item,
			    int list,
			    uchar proto,
			    ulong dst_net,
			    ulong dst_host_1long,
			    ushort dst_host_2short,
			    ushort dst_socket,
			    ulong src_net,
			    ulong src_host_1long,
			    ushort src_host_2short,
			    ushort src_socket)
{
    return(inline_item_denied(item,
			      list,
			      proto,
			      dst_net,
			      dst_host_1long,
			      dst_host_2short,
			      dst_socket,
			      src_net,
			      src_host_1long,
			      src_host_2short,
			      src_socket));
}

/*
 * nov_acct_and_access_check_pas:
 * Determine if we are allowed to forward the passed datagram, based on the
 * NOVELL access pointer. Return FALSE if output access is denied.
 *
 * Also do accounting work.
 */
static boolean nov_acct_and_access_check_pas (paktype    *pak,
					      ipxhdrtype *ipx,
					      idbtype	 *swoutput,
					      boolean    bcast)
{
    xnseacctype *item = NULL;
    int list = 0;
    uchar proto;
    ulong src_net, src_host_1long, dst_net, dst_host_1long;
    ushort src_host_2short, dst_host_2short, dst_socket, src_socket;

    proto = ipx->pt;
    dst_net = GETLONG(ipx->ddnet);
    dst_host_1long = GETLONG(ipx->dhost);
    dst_host_2short = GETSHORT(&ipx->dhost[4]); 
    dst_socket = GETSHORT(&ipx->dsock); 

    src_net = GETLONG(ipx->ssnet);
    src_host_1long = GETLONG(ipx->shost);
    src_host_2short = GETSHORT(&ipx->shost[4]); 
    src_socket = GETSHORT(&ipx->ssock); 


    /*
     * Validate ALL packets against input filter:
     */
    list = pak->if_input->novell_inaccesslist; /* %%% */
    item = ipx_get_ext_acl(list);

#ifdef DEBUG
    if (novell_debug) if (item) buginf("(in)");
#endif

    if (item &&
	item_denied(item, list, proto,
		    dst_net, dst_host_1long, dst_host_2short, dst_socket,
		    src_net, src_host_1long, src_host_2short, src_socket)) {
	return(FALSE);
    }

#ifdef DEBUG
    if (novell_debug) if (item) buginf("(ok)");
#endif

    if (bcast) {
	
#ifdef DEBUG
 	if (novell_debug) buginf(" (bc)");
#endif
  	/*
  	 * Screen out ineligible entries:
  	 */
  	if ((dst_net == 0) ||
  	    (dst_net == NOVELL_TYPE_ANY_NET) ||
  	    ((ipx->pt == NOVELL_TYPE_NETBIOS) && !novell_type20_helpered) ||
  	    (dst_socket == NOVELL_SOCK_RIP) ||
  	    (dst_socket == NOVELL_SOCK_SAP) ||
  	    (dst_socket == NOVELL_SOCK_NLSP) ||
 	    (dst_socket == NOVELL_SOCK_IGRP)) {
	    
#ifdef DEBUG
  	    if (novell_debug) buginf("*bc deny*");
#endif
  	    return(FALSE);
  	}
    }
    
    /*
     * Validate traditional data packets against access-group filter:
     */
    list = swoutput->novell_accesslist;
    item = ipx_get_ext_acl(list);
    
#ifdef DEBUG
    if (novell_debug) if (item) buginf("(out)");
#endif

    if (item &&
	item_denied(item, list, proto, dst_net,
		    dst_host_1long, dst_host_2short,
		    dst_socket, src_net, src_host_1long,
		    src_host_2short, src_socket)) {
	return(FALSE);
    }

#ifdef DEBUG
    if (novell_debug) if (item) buginf("(ok)");
#endif
    
    if (swoutput->ipx_accounting)
	return(nov_fast_acct_do_work(src_net, dst_net, src_host_1long,
				     src_host_2short, dst_host_1long,
				     dst_host_2short, GETSHORT(&ipx->len)));
    return(TRUE);
}

static void ipx_pas_register_drivers (ulong flags, hwidbtype* idb)
{
    /*
     * Check to see if we're configuring IPX
     */
    if (!(flags & (FSF_NOVELL1 | FSF_NOVELL2)))
	return;

    if (idb->novell_fastokay) {
        reg_add_ether_fs(RXTYPE_NOVELL1,
                         (service_ether_fs_type)nov_fastswitch,
                         "nov_fastswitch");
        reg_add_fddi_fs(RXTYPE_NOVELL1,
                        (service_fddi_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_framerelay_fs(RXTYPE_NOVELL1,
                        (service_framerelay_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_hdlc_fs(RXTYPE_NOVELL1,
                        (service_hdlc_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_ppp_fs(RXTYPE_NOVELL1,
                       (service_ppp_fs_type)nov_fastswitch,
                       "nov_fastswitch");
	reg_add_atm_dxi_fs(RXTYPE_NOVELL1,
			   (service_atm_dxi_fs_type)nov_fastswitch,
			   "nov_fastswitch");
	reg_add_smds_fs(RXTYPE_NOVELL1,
			(service_smds_fs_type)nov_fastswitch,
			"nov_fastswitch");
        reg_add_aip_fs(RXTYPE_NOVELL1,
			(service_aip_fs_type)nov_fastswitch,
			"nov_fastswitch");
	reg_add_tunnel_fs(RXTYPE_NOVELL1,
			  (service_tunnel_fs_type)nov_fastswitch,
			  "nov_fastswitch");

#ifdef FUTURE
	reg_add_channel_fs(RXTYPE_NOVELL1,
			   (service_channel_fs_type)nov_fastswitch,
			   "nov_fastswitch");
#endif

        reg_add_ether_fs(RXTYPE_NOVELL2,
                         (service_ether_fs_type)nov_fastswitch,
                         "nov_fastswitch");
        reg_add_framerelay_fs(RXTYPE_NOVELL2,
                        (service_framerelay_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_hdlc_fs(RXTYPE_NOVELL2,
                        (service_hdlc_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_ppp_fs(RXTYPE_NOVELL2,
                       (service_ppp_fs_type)nov_fastswitch,
                       "nov_fastswitch");
	reg_add_atm_dxi_fs(RXTYPE_NOVELL2,
			   (service_atm_dxi_fs_type)nov_fastswitch,
			   "nov_fastswitch");
	reg_add_smds_fs(RXTYPE_NOVELL2,
			(service_smds_fs_type)nov_fastswitch,
			"nov_fastswitch");
        reg_add_aip_fs(RXTYPE_NOVELL2,
                        (service_aip_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_ether_fs(RXTYPE_SNAP_NOV1,
                         (service_ether_fs_type)nov_fastswitch,
                         "nov_fastswitch");
        reg_add_fddi_fs(RXTYPE_SNAP_NOV1,
                        (service_fddi_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_fddi_fs(RXTYPE_NOVELL2,
                        (service_fddi_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_framerelay_fs(RXTYPE_SNAP_NOV1,
                        (service_framerelay_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_atm_dxi_fs(RXTYPE_SNAP_NOV1,
			   (service_atm_dxi_fs_type)nov_fastswitch,
			   "nov_fastswitch");
	reg_add_smds_fs(RXTYPE_SNAP_NOV1,
			(service_smds_fs_type)nov_fastswitch,
			"nov_fastswitch");
        reg_add_aip_fs(RXTYPE_SNAP_NOV1,
			   (service_aip_fs_type)nov_fastswitch,
			   "nov_fastswitch");
        reg_add_token_fs(RXTYPE_SNAP_NOV1,
                         (service_token_fs_type)nov_fastswitch,
                         "nov_fastswitch");

        reg_add_ether_fs(RXTYPE_SAP_NOVELL,
                         (service_ether_fs_type)nov_fastswitch,
                         "nov_fastswitch");
        reg_add_fddi_fs(RXTYPE_SAP_NOVELL,
                        (service_fddi_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_token_fs(RXTYPE_SAP_NOVELL,
                         (service_token_fs_type)nov_fastswitch,
                         "nov_fastswitch");
        reg_add_aip_fs(RXTYPE_SAP_NOVELL,
			   (service_aip_fs_type)nov_fastswitch,
			   "nov_fastswitch");
    } else {
            fs_table_fill((fastswitch_type)return_false, "return_false",
                          RXTYPE_NOVELL1);
            fs_table_fill((fastswitch_type)return_false, "return_false",
                          RXTYPE_NOVELL2);
            fs_table_fill((fastswitch_type)return_false, "return_false",
                          RXTYPE_SNAP_NOV1);
            fs_table_fill((fastswitch_type)return_false, "return_false",
                          RXTYPE_SAP_NOVELL);
    }
}

static void ipx_pas_fs_init (subsystype *subsys)
{
    reg_add_register_drivers(ipx_pas_register_drivers,
                             "ipx_pas_registers_drivers");
    reg_add_fast_supported(ipx_pas_fast_supported,
			   "ipx_pas_fast_supported");
}

/*
 * Novell PAS FS subsystem header
 */

#define IPXPASFS_MAJVERSION 1
#define IPXPASFS_MINVERSION 0
#define IPXPASFS_EDITVERSION  1

SUBSYS_HEADER(ipx_pas_fs,
	      IPXPASFS_MAJVERSION, IPXPASFS_MINVERSION, IPXPASFS_EDITVERSION,
              ipx_pas_fs_init, SUBSYS_CLASS_PROTOCOL,
              "seq: ipx",
              "req: fastswitch, ipx");
