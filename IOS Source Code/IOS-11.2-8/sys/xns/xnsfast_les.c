/* $Id: xnsfast_les.c,v 3.4.12.6 1996/08/14 02:38:08 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/xnsfast_les.c,v $
 *------------------------------------------------------------------
 * xnsfast_les.c -- XNS Fast Switching module providing low end specific
 *                  Support.
 *
 * 26-June-1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xnsfast_les.c,v $
 * Revision 3.4.12.6  1996/08/14  02:38:08  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.4.12.5  1996/08/13  14:41:20  rbadri
 * CSCdi64089:  Multiprotocol support fixes required
 * Branch: California_branch
 * XNS support for LANE
 *
 * Revision 3.4.12.4  1996/07/26  23:54:33  ssangiah
 * CSCdi63705:  Need to add particle-based XNS fast-switching support.
 * Branch: California_branch
 *
 * Revision 3.4.12.3  1996/04/27  06:48:09  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.2.18.3  1996/04/26  00:13:43  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.2.18.2  1996/04/17  00:11:18  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.2.18.1  1996/02/23  21:33:46  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.4.12.2  1996/04/17  13:52:37  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.4.12.1  1996/03/18  22:54:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/13  02:15:25  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.24.1  1996/02/20  22:02:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/13  19:44:41  hampton
 * Reorder routines to remove platform specific extern statements in
 * fastswitching code.  [CSCdi48844]
 *
 * Revision 3.3  1996/02/13  08:18:07  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.2  1995/11/17  19:25:48  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:40:10  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/18  04:16:33  shaker
 * CSCdi35990:  Need packet pool for fast switched packet structure
 * Use new pak_set_dirty() to only set PAK_BUF_DIRTY iff
 * PAK_BUF_USE_DIRTY is set. Set PAK_BUF_USE_DIRTY for RSP fs_pak
 *
 * Revision 2.1  1995/06/07  23:31:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Routines Exported from this module
 * xns_fastswitch:	Fast switches xns packests
 *
 * Both the above routines are called through a switch table "fast_switch"
 * using the RXTYPE_ codes as the index. ( The RXTYPE_ contains packet type
 * code.)
 */

#include "master.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "xns.h"
#include "novell.h"
#include "xnsfast.h"
#include "logger.h"
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "access.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "config.h"
#include "../wan/serial.h"
#include "../wan/dialer.h"
#include "../if/atm.h"
#include "../lane/lane.h"


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

extern void xns_fqc_header(char *);


/*
 * xns_access_check
 * Determine if we are allowed to forward the passed datagram, based on the
 * XNS access pointer. Return FALSE if output access is denied.
 */
boolean xns_access_check_pan (
    paktype	 *pak_ptr,
    idbtype	 *if_ptr)
{
    xnspephdrtype *xns;
    ulong flags;
    xnseacctype *item;
    ulong src_net, dst_net, dst_host_1long, src_host_1long;
    ushort dst_host_2short, src_host_2short, dst_socket, src_socket;
    xnssb	*xsb;

    /*
     * Tell lower layers that we have groveled in the
     * packet more than normal for the fast switch path
     * and the packet may need to be flushed out of the
     * data cache for some platforms:
     */
    pak_set_dirty(pak_ptr);

    xns = (xnspephdrtype *)xnsheadstart(pak_ptr);
    xsb = idb_get_swsb_inline(if_ptr, SWIDB_SB_XNS);
    if (!(item = xns_get_ext_acl(xsb->xns_accesslist)))
	return(TRUE);
    dst_net = GETLONG(xns->ddnet);
    dst_host_1long = *((ulong *)xns->dhost);
    dst_host_2short = * ((ushort *)&xns->dhost[4]); 
    dst_socket = xns->dsock; 

    src_net = GETLONG(xns->ssnet);
    src_host_1long = *((ulong *)xns->shost);
    src_host_2short = *((ushort *)&xns->shost[4]); 
    src_socket = xns->ssock; 

    do {
	flags = item->flags;
       	/*
	 * A network of -1 indicates all XNS networks.
	 * A socket of 0 indicates all XNS sockets.
	 */
	if (flags & XNS_SRCNET) { 
	    if ((item->srcnet != 0xFFFFFFFF) && 
		(item->srcnet != (src_net & ~item->srcnetmask)))
		continue;
	}
	if (flags & XNS_DSTNET) {
	  if ((item->dstnet != 0xFFFFFFFF) &&
	      (item->dstnet != (dst_net & ~item->dstnetmask)))
	    continue;
	}        
	if (flags & XNS_SRCHOST) {
	    /*
	     * Check for match on source host address
	     */
	    if ((src_host_2short & ~item->srchostmask.u.sword[2])
		!= item->srchost.u.sword[2])
		continue;
	    if ((src_host_1long & ~item->srchostmask.u.lword[0])
		!= (item->srchost.u.lword[0]))
		continue;
	}
	if (flags & XNS_DSTHOST) {
	    /*
	     * Check for match on destination host address
	     */
	    if ((dst_host_2short & ~item->dsthostmask.u.sword[2]) 
		!= item->dsthost.u.sword[2])
		continue;
	    if ((dst_host_1long & ~item->dsthostmask.u.lword[0])
		                            != (item->dsthost.u.lword[0]))
		continue;
	}
	/* check if it is extended access list */
	if (xsb->xns_accesslist > MAXXNSACC ) {
	    if (flags & XNS_PROTOCOL) {
		if (item->protocol != xns->pt)
		  continue;
		if (flags & XNS_PEPCLIENT) {
		    if (xns->pt == XNS_TYPE_PEP) {
			if (item->pep_clienttype != xns->pep_client_type)
			  continue;
		    }
		}
	    }
	    if (flags & XNS_SRCSOCKET) {
		if (item->srcsocket && (item->srcsocket != src_socket))
		  continue;
	    }
	    if (flags & XNS_DSTSOCKET) {
		if(item->dstsocket && (item->dstsocket != dst_socket))
		  continue;
	    }
	}
	return(item->grant);
      } while ((item = item->next));
    return(FALSE);
}


/*
 * xns_fast_supported
 *
 * Determine whether fastswitch support present for given hw/config
 *
 */
static boolean xns_fast_supported (int linktype, hwidbtype *hwidb)
{
    if (!hwidb || (linktype != LINK_XNS)) {
	return(FALSE);
    }

    /*
     * Ethernet:
     */
    if ((hwidb->status & IDB_ETHER) && (hwidb->enctype == ET_ARPA)) {
	return(TRUE);
    }

    /*
     * Fddi:
     */
    if ((hwidb->status & IDB_FDDI) && (hwidb->enctype == ET_SNAP)) {
	return(TRUE);
    }

    /*
     * Tokenring:
     */
    if ((hwidb->status & IDB_TR) && (hwidb->enctype == ET_SNAP)) {
	return(TRUE);
    }

    /*
     * Serial:
     */
    if (hwidb->status & IDB_SERIAL) {
	if ((hwidb->enctype == ET_HDLC) && !is_ddr(hwidb)) {
	    return(TRUE);
	}
    }

    /*
     * ATM
     */
    if (hwidb->status & IDB_ATM) {
	return (TRUE);
    }
    return(FALSE);
}

/*
 * ether_encap_rewrite()
 *      Routine to re-write the ethernet encapsulation on a xns packet
 * being fast-switched.
 */
static inline void ether_encap_rewrite (paktype *pak, xnspathtype *path)
{
    ulong  *cache_macptr;
    ulong *macptr;
 
    macptr = (ulong *)pak->datagramstart;
    cache_macptr = path->xnsp_mh.mac_longs;

    PUTLONG(macptr++, *cache_macptr++);
    PUTLONG(macptr++, *cache_macptr++);
    PUTLONG(macptr++, *cache_macptr++);
    PUTSHORT(macptr, *(ushort *)cache_macptr);
}

/*
 * fddi_encap_rewrite()
 *      Routine to re-write the FDDI encapsulation on a xns packet
 * being fast-switched.
 */
static inline void fddi_encap_rewrite (paktype *pak, xnspathtype *path)
{
    ulong  *cache_macptr;
    ulong *macptr;
 
    *pak->datagramstart = FDDI_LLC_FC_BYTE;
    macptr = (ulong *)((fddi_hdr *)pak->datagramstart)->daddr;
    cache_macptr = path->xnsp_mh.mac_longs;

    PUTLONG(macptr++, *cache_macptr++);
    PUTLONG(macptr++, *cache_macptr++);
    PUTLONG(macptr++, *cache_macptr++);
    PUTLONG(macptr++, *cache_macptr++);
    PUTLONG(macptr, *cache_macptr);
}

/*
 * tring_encap_rewrite()
 *	Routine to re-write the token-ring encapsulation on a xns packet
 * being fast-switched.
 */
static inline void tring_encap_rewrite (paktype *pak, xnspathtype *path)
{
    ulong  *cache_macptr;
    ushort *macsptr;
    ulong  *macptr;

    macsptr = (ushort *)pak->datagramstart;
    cache_macptr = path->xnsp_mh.mac_longs;

    /*
     * Build AC-FC (not in the cache)
     */
    PUTSHORT(macsptr++, (AC_PRI0 << 8) | FC_TYPE_LLC);
    macptr = (ulong *)macsptr;

    /*
     * Ugly but functional code to copy the cache entry including a RIF 
     * if required, into the header.
     */
    {
        int longwords;
        for (longwords = 0; longwords < path->length>>2; longwords++)
	    PUTLONG(macptr++, *cache_macptr++);
    }
 
    if (path->length & 0x02)
        PUTSHORT(macptr, *(ushort *)cache_macptr);
}

/*
 * fs_xns_prepare_packet()
 *      Prepare a packet prior to re-writing the outbound encapsulation.
 */
static inline boolean fs_xns_prepare_packet (paktype *pak, int delta)
{
    particletype *rw_p;
 
    if (delta < (PARTICLE_ENCAPBYTES - pak->encsize)) {
        /*
         * Rewrite can fit over existing encapsulation
         */
        particle_clip_inline(pak->particlequeue.qhead, delta);
        pak->datagramsize += delta;
        pak->datagramstart -= delta;
    } else {
        /*
         * Allocate a rewrite particle
         */
        rw_p = allocate_fs_particle();
        if (!rw_p) {
            return(FALSE);
        }
        pak->datagramsize += delta;
        pak->datagramstart = rw_p->data_start;
        particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
        rw_p->data_bytes = delta + pak->encsize;
        requeue_inline(&pak->particlequeue, rw_p);
    }
    return(TRUE);
}

/*
 * fs_xns_ether_dispatch()
 *      Routine to dispatch a xns packet that is headed out of an
 * ethernet interface.
 */
static inline boolean fs_xns_ether_dispatch (hwidbtype *output,
                                             paktype *pak, xnspathtype *path)
{
    int delta, size;
 
    if (pak_has_particles(pak)) {
        delta = ETHER_ARPA_ENCAPBYTES - pak->encsize;
        if (!fs_xns_prepare_packet(pak, delta))
            return (FALSE);
    } else {
        size = pak->datagramsize - pak->encsize + ETHER_ARPA_ENCAPBYTES;
        if (size < MINETHERSIZE)
            size = MINETHERSIZE;
        pak->datagramsize = size;
        pak->datagramstart = xnsheadstart(pak) - ETHER_ARPA_ENCAPBYTES;
    }

    /*
     * Now, re-write the encapsulation and send the packet on its way.
     */
    ether_encap_rewrite(pak, path);
    pak->if_output = path->xnsp_idb_ptr;
    (*output->fastsend)(output, pak);
 
    return (TRUE);
}

/*
 * fs_xns_fddi_dispatch()
 *      Routine to dispatch a xns packet that is headed out of an
 * FDDI interface.
 */
static inline boolean fs_xns_fddi_dispatch (hwidbtype *output,
                                            paktype *pak, xnspathtype *path)
{
    int delta;
 
    if (pak_has_particles(pak)) {
        delta = FDDI_SNAP_ENCAPBYTES - pak->encsize;
        if (!fs_xns_prepare_packet(pak, delta))
            return (FALSE);
    } else {
        pak->datagramsize = pak->datagramsize - pak->encsize
                                              + FDDI_SNAP_ENCAPBYTES;
        pak->datagramstart = xnsheadstart(pak) - FDDI_SNAP_ENCAPBYTES;
    }
 
    /*
     * Now, re-write the encapsulation and send the packet on its way.
     */
    fddi_encap_rewrite(pak, path);
    pak->if_output = path->xnsp_idb_ptr;
    (*output->fastsend)(output, pak);
 
    return (TRUE);
}

/*
 * fs_xns_tring_dispatch()
 *	Routine to dispatch a xns packet that is headed out of an
 * token-ring interface.
 */
static inline boolean fs_xns_tring_dispatch (hwidbtype *output,
                                             paktype *pak, xnspathtype *path)
{
    int delta;
 
    if (pak_has_particles(pak)) {
        delta = (TR_ACFC_SIZE + path->length) - pak->encsize;
        if (!fs_xns_prepare_packet(pak, delta))
            return (FALSE);
    } else {
        pak->datagramsize = pak->datagramsize - pak->encsize 
                                              + (TR_ACFC_SIZE + path->length);
        pak->datagramstart = xnsheadstart(pak) - (TR_ACFC_SIZE + path->length);
    }

    /*
     * Now, re-write the encapsulation and send the packet on its way.
     */
    tring_encap_rewrite(pak, path);
    pak->if_output = path->xnsp_idb_ptr;
    (*output->fastsend)(output, pak);

    return(TRUE);
}

/*
 * fs_xns_hdlc_dispatch()
 *      Routine to dispatch a xns packet that is headed out of an
 * HDLC encapsulated serial interface.
 */
static inline boolean fs_xns_hdlc_dispatch (hwidbtype *output,
                                            paktype *pak, xnspathtype *path)
{
    int delta;
    ushort *macsptr;
 
    if (pak_has_particles(pak)) {
        delta = path->length - pak->encsize;
        if (!fs_xns_prepare_packet(pak, delta))
            return (FALSE);
    } else {
        pak->datagramsize = pak->datagramsize - pak->encsize + path->length;
        pak->datagramstart = xnsheadstart(pak) - path->length;
    }
 
    /*
     * Now, re-write the encapsulation and send the packet on its way.
     */
    macsptr = (ushort *)pak->datagramstart;
    if (path->length > HDLC_ENCAPBYTES) {
        PUTSHORT(macsptr++, path->xnsp_mh.mac_shorts[0]);
    }
    PUTLONG((ulong *)macsptr, HDLC_XNSCODE);
    pak->if_output = path->xnsp_idb_ptr;
    (*output->fastsend)(output, pak);
 
    return (TRUE);
}

/*
 * xns_fastswitch()
 *
 * The code below has been lovingly hand-crafted to provide a lifetime of
 * reliable use and happiness. ;-)
 *
 * Don't change this stuff unless you're prepared to spend time poring
 * over hot and sweaty assembler listings and HP analyzer timing output...
 */

boolean xns_fastswitch (paktype *pak)
{
    xnshdrtype *xns;
    xnscachetype *cache;
    xnspathtype *path;
    hwidbtype *input, *output;
    idbtype *swoutput;
    ushort datagramsize, addrlow;
    ulong bucket;
    shortlong net, addrhigh;
    xnssb	*xsb;

    xns = (xnshdrtype *)xnsheadstart(pak);
    pak->linktype = LINK_XNS;

    if (GETLONG(xns->ssnet) == 0)
	return(FALSE);
    /* Check if hop count valid (including reserved tc bits) */
    if (xns->tc < XNS_MAX_HOPS) {
      xns_hop_count_ok:
	net.d.lword = GETLONG(xns->ddnet);
        addrhigh.d.lword = GETLONG(xns->dhost);
        addrlow = GETSHORT(xns->dhost + 4);
	bucket =  net.d.lword;
	bucket ^= addrhigh.d.lword;
	bucket ^= addrlow;
	bucket ^= (bucket >> 16);
	bucket ^= (bucket >> 8);
	bucket &=0xFF;
	for (cache = xnscache[bucket]; cache; cache = cache->xnsc_next_ptr){
            if ((*(ushort *)&cache->xnsc_dest_address[4] == addrlow) &&
                (*(ulong *)cache->xnsc_dest_address == addrhigh.d.lword) &&
                (cache->xnsc_dest_net == net.d.lword)) {         
		if (cache->xnsc_version == xnscache_version) {
		    path = cache->xnsc_path_next_ptr;
		    swoutput = path->xnsp_idb_ptr;
		    output = swoutput->hwptr;
		    if (swoutput == pak->if_input)
			return(FALSE);
		    xsb = idb_get_swsb_inline(swoutput, SWIDB_SB_XNS);
		    /*
		     * Check if this cache entry is for directly connected net.
		     */
		    if (!cache->xnsc_connected) {
			cache->xnsc_path_next_ptr = path->xnsp_next_ptr;
		      resume_flow:
			
			/*
			 * Check if access list is enabled on this interface.
			 */
			if (!(ushort)xsb->xns_accesslist) {
			  yes_doit:
			    /*
			     * Do some accounting of packets and bytes
			     * in and out.
			     */
			    datagramsize = pak->datagramsize;
			    xns_stat[XNS_INPUT]++;
			    xns_stat[XNS_FORWARDED]++;
			    
			    input = pak->if_input->hwptr;
			    input->counters.inputs_fast[ACCT_PROTO_XNS]++;
			    input->counters.rx_cumbytes_fast[ACCT_PROTO_XNS] += datagramsize;
			    pak->acct_proto = ACCT_PROTO_XNS;
			    xns->tc += 1;  /* adding 1 to hop count */
			    if (xns->cksum != NULL_CHECKSUM)
				xns_fqc_header((char *)&xns->cksum);
			    output = path->xnsp_idb_ptr->hwptr;

			    switch (output->fast_switch_type) {
                            /*
                             * Fast-switch to an ethernet interface.
                             */
			    case FS_ETHER:
                                return(fs_xns_ether_dispatch(output,
                                                              pak, path));
                            /*
                             * Fast-switch to a FDDI interface.
                             */
			    case FS_FDDI:
                                return(fs_xns_fddi_dispatch(output,
                                                            pak, path));
                            /*
                             * Fast-switch to a token-ring interface.
                             */
			    case FS_TOKEN:
                                return(fs_xns_tring_dispatch(output,
                                                             pak, path));
                            /*
                             * Fast-switch to a HDLC serial interface.
                             */
			    case FS_HDLC:
                                return(fs_xns_hdlc_dispatch(output,
                                                            pak, path));
                            /*
                             * Fast-switch to a GRE tunnel.
                             */
			    case FS_TUNNEL:
                                pak->if_output = path->xnsp_idb_ptr;
                                (*output->fastsend)(output, pak);
                                return(TRUE);
 
 
 			      case FS_AIP:
 				if (is_atm_lane(path->xnsp_idb_ptr)) {
                                     ulong *macptr;
    				     ulong *cache_macptr;

 				    /*
 				     * Switch to ATM-LANE
 				     */
 				    datagramsize = (datagramsize - 
 						    pak->encsize +
 						    ATM_LANE_DATA_ENCAPBYTES);
 				    if (datagramsize < MINETHERSIZE)
 					datagramsize = MINETHERSIZE;
 				    pak->datagramsize = datagramsize;
 				    pak->datagramstart = (xnsheadstart(pak) -
 							  ATM_LANE_DATA_ENCAPBYTES);
 				
 				    macptr = (ulong *)pak->datagramstart;
 				    cache_macptr = path->xnsp_mh.mac_longs; 
 				    /*
 				     * LANE looks like ether ARPA but for
 				     * 3 more short words of ATM header
 				     */
 				    PUTLONG(macptr++, *cache_macptr++);
 				    PUTLONG(macptr++, *cache_macptr++);
 				    PUTLONG(macptr++, *cache_macptr++);
 				    PUTLONG(macptr++, *cache_macptr++);
 				    PUTLONG(macptr,   *cache_macptr);
 				    pak->if_output = path->xnsp_idb_ptr;
 				    pak->atm_enctype = ATM_DM_PKT_AAL5;
 				    pak->atm_vcid =path->xnsp_mh.mac_shorts[0];
 				    (*output->fastsend)(output, pak);
 				    return(TRUE);
 				} else {
 				    return (FALSE);
 				}
			    default:
                                /*
                                 * This really shouldn't happen, but we'll
                                 * let process level handle it rather than
                                 * just trash it.
                                 */
				return(FALSE);
			    }
			} else {
			    if (xns_access_check_pan(pak, swoutput))
				goto yes_doit;
			}
		    } else {

			/*
			 * The destination is on directly connected net.
			 * read the destnation address from received
			 * packet and check if it is targeted at us or a
			 * a broadcast packet and return FALSE if that is
			 * the case.
			 */
			idbtype *swinput;
	                net.d.lword = GETLONG((ulong *)xns->dhost);
			swinput = pak->if_input;
			xsb = idb_get_swsb_inline(swinput, SWIDB_SB_XNS);
	                datagramsize = *((ushort *)&xns->dhost[4]);
			if (((datagramsize == *((ushort *)&xsb->xns_node[4]))
			    && (net.d.lword == *((ulong *)xsb->xns_node)))
			    || ((datagramsize ==
				*((ushort *) &xsb->xns_node[4])) &&
				(net.d.lword  ==*((ulong *)xsb->xns_node))))
			    return(FALSE);
			goto resume_flow;
		    }
		} else {
		    goto deny_access;
		}
	    }
	}
	/* The max hop count has apparently been exceeded. However, now do 
         * the "correct" check for Novell hop count (ignoring reserved bits).
         */
    } else if ((xns->tc & XNS_TC_HOP_MASK) < XNS_MAX_HOPS) {
	goto xns_hop_count_ok;
    }
  deny_access:
    return(FALSE); 
}

static void xns_les_fs_register_drivers (ulong flags, hwidbtype* idb)
{
    /*
     * Check to see if we're configuring XNS
     */
    if (!(flags & FSF_XNS))
	return;

    if (idb->fast_switch_flag & FSF_XNS) {
        reg_add_ether_fs(RXTYPE_XNS,
                         (service_ether_fs_type)xns_fastswitch,
                         "xns_fastswitch");
        reg_add_hdlc_fs(RXTYPE_XNS,
                        (service_hdlc_fs_type)xns_fastswitch,
                        "xns_fastswitch");
        reg_add_fddi_fs(RXTYPE_XNS,
                        (service_fddi_fs_type)xns_fastswitch,
                        "xns_fastswitch");
        reg_add_smds_fs(RXTYPE_XNS, (service_smds_fs_type)return_false,
                        "return_false");
        reg_add_token_fs(RXTYPE_SNAP_XNS,
                         (service_token_fs_type)xns_fastswitch,
                         "xns_fastswitch");
	reg_add_tunnel_fs(RXTYPE_XNS,
			  (service_tunnel_fs_type)xns_fastswitch,
			  "xns_fastswitch");
	reg_add_aip_fs(RXTYPE_XNS,
			  (service_aip_fs_type)xns_fastswitch,
			  "xns_fastswitch");
	
    } else {
        fs_table_fill((fastswitch_type)return_false, "return_false",
                      RXTYPE_XNS);
        fs_table_fill((fastswitch_type)return_false, "return_false",
                      RXTYPE_SNAP_XNS);
    }
}

static void xns_les_fs_init (subsystype* subsys)
{
    reg_add_register_drivers(xns_les_fs_register_drivers,
                             "xns_les_fs_register_drivers");
    reg_add_fast_supported(xns_fast_supported, "xns_fast_supported");
}

/*
 * XNS LES FS subsystem header
 */

#define XNSLESFS_MAJVERSION 1
#define XNSLESFS_MINVERSION 0
#define XNSLESFS_EDITVERSION  1

SUBSYS_HEADER(xns_les_fs,
	      XNSLESFS_MAJVERSION, XNSLESFS_MINVERSION, XNSLESFS_EDITVERSION,
              xns_les_fs_init, SUBSYS_CLASS_PROTOCOL,
              "seq: xns",
              "req: fastswitch, xns");

/* end of xnsfast_les.c */
