/* $Id: clnsfast_les.c,v 3.3.60.4 1996/07/25 19:05:13 ssangiah Exp $
 * $Source: /release/112/cvs/Xsys/clns/clnsfast_les.c,v $
 *------------------------------------------------------------------
 * clnsfast_les.c -- CLNS Fast Switching module providing Lance/mk5025 specific
 * Support.
 *
 * 7-Aug-1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clnsfast_les.c,v $
 * Revision 3.3.60.4  1996/07/25  19:05:13  ssangiah
 * CSCdi63706:  Need to add particle-based CLNS fast-switching support.
 * Fix inappropriately named function in previous commit.
 * Branch: California_branch
 *
 * Revision 3.3.60.3  1996/07/24  20:46:09  ssangiah
 * CSCdi63706:  Need to add particle-based CLNS fast-switching support.
 * Branch: California_branch
 *
 * Revision 3.3.60.2  1996/04/17  13:36:01  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.3.60.1  1996/03/18  19:03:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/13  01:13:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.2  1996/03/07  08:40:28  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  13:40:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  08:52:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:12:44  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:08:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  20:52:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/07/18  04:15:49  shaker
 * CSCdi35990:  Need packet pool for fast switched packet structure
 * Use new pak_set_dirty() to only set PAK_BUF_DIRTY iff
 * PAK_BUF_USE_DIRTY is set. Set PAK_BUF_USE_DIRTY for RSP fs_pak
 *
 * Revision 2.3  1995/06/29  11:40:34  shaker
 * CSCdi36094:  Clns fast-switching is broken on EIP-AIP
 * Ethernet case cannot assume that the DSAP/LSAP is already in place in
 * the input packet when constructing the new output packet.  This is
 * basically the same problem as was fixed for serial interfaces in
 * CSCdi35864.
 *
 * Revision 2.2  1995/06/20  03:46:58  shaker
 * CSCdi35864:  Clns fast switching broken on serial interfaces
 *  (HIP/FSIP) w/AIP
 * Write entire cached encaps string into the outgoing packet, instead
 * of making assumptions about the input interface type
 *
 * Revision 2.1  1995/06/07  20:17:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Routines Exported from this module
 * clns_fastswitch - Fast switches clns packets
 *
 * Called through a switch table "fast_switch" using the RXTYPE code
 * as the index (RXTYPE is the same packet type code that the MCI
 * microcode uses to classify packets).
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "packet.h"
#include "access.h"
#include "logger.h"
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "address.h"
#include "config.h"
#include "../if/ether.h"
#include "../wan/serial.h"
#include "../if/fddi.h"
#include "../if/network.h"
#include "../les/if_les.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/clns_externs.h"
#include "../if/atm.h"
#include "../lane/lane.h"


/*
 * Due to the fact that we always have the Control Byte
 * from the ISO frame stuffed in between the HDLC encapsulation
 * and the start of the Fixed CLNS header. We consider the
 * Control Byte to be part of the encapsulation in the
 * low-end fastswitching code
 */
#define CLNS_HDLC_ENCAPBYTES (HDLC_ENCAPBYTES + 1)

/*
 * The SAP header for a CLNS frame. To make it up to 4 bytes, we always
 * write back the CLNS PID (0x81) too.
 */
#define CONST_FE_FE_03_81 0xFEFE0381

/*
 * ether_encap_rewrite()
 *	Routine to re-write the ethernet encapsulation on a CLNS packet
 * being fast-switched.
 */
static inline void ether_encap_rewrite (hwidbtype *idb, paktype *pak,
                                        clns_cache_adjtype *cache_adj)
{
    PUTLONG(pak->datagramstart,       GETLONG(cache_adj->dst_snpa));
    PUTSHORT(pak->datagramstart + 4,  GETSHORT(cache_adj->dst_snpa + 4));
    PUTLONG(pak->datagramstart  + 6,  GETLONG(idb->hardware));
    PUTSHORT(pak->datagramstart + 10, GETSHORT(idb->hardware + 4));

    /*
     * Write in the ISO length and control byte (just to be sure)
     */
    PUTSHORT(&pak->datagramstart[ISO1LEN], 
             (pak->datagramsize - ETHER_ARPA_ENCAPBYTES));
    pak->datagramstart[ISO1CTL] = LLC1_UI;

    /*
     * Write the DSAP and LSAP:
     */
    PUTSHORT(&pak->datagramstart[ISO1DSAP], 0xFEFE);
}

/*
 * fddi_encap_rewrite()
 *      Routine to re-write the FDDI encapsulation on a CLNS packet
 * being fast-switched.
 */
static inline void fddi_encap_rewrite (hwidbtype *idb, paktype *pak,
                                       clns_cache_adjtype *cache_adj)
{
    /*
     * This isn't optimal at all but does the job. And it's
     * faster than punting the frame to process switching.
     * Needs some alignment TLC to make it run faster
     */
    *(uchar *)(pak->datagramstart) = FDDI_LLC_FC_BYTE;

    PUTLONG(pak->datagramstart + 1, GETLONG(cache_adj->dst_snpa));
    PUTSHORT(pak->datagramstart + 5,
             GETSHORT(cache_adj->dst_snpa + 4));

    if (idb->type == IDBTYPE_FDDIT) {
        /*
         * FIP (FDDIT) on RSP requires canonical MAC:
         */
        PUTLONG(pak->datagramstart + 7, GETLONG(idb->hardware));
        PUTSHORT(pak->datagramstart + 11, GETSHORT(idb->hardware + 4));
    } else {
        /*
         * For Midrange boxes:
         */
        PUTLONG(pak->datagramstart + 7,
                GETLONG(idb->bit_swapped_hardware));
        PUTSHORT(pak->datagramstart + 11,
                 GETSHORT(idb->bit_swapped_hardware + 4));
    }

    PUTLONG(pak->datagramstart + 13, CONST_FE_FE_03_81);
}

/*
 * fs_clns_prepare_packet()
 *      Prepare a packet prior to re-writing the outbound encapsulation.
 */
static inline boolean fs_clns_prepare_packet (paktype *pak, int delta)
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
 * fs_clns_ether_dispatch()
 *      Routine to dispatch a clns packet that is headed out of an
 * ethernet interface.
 */
static inline boolean fs_clns_ether_dispatch (hwidbtype *idb, 
                                              idbtype *swidb, paktype *pak,
                                              clns_cache_adjtype *cache_adj)
{
    int delta, size;
    if (pak_has_particles(pak)) {
        delta = ETHER_SAP_ENCAPBYTES - pak->encsize;
        if (!fs_clns_prepare_packet(pak, delta))
            return (FALSE);
    } else {
        size = pak->datagramsize - pak->encsize + ETHER_SAP_ENCAPBYTES;
        if (size < MINETHERSIZE)
            size = MINETHERSIZE;
        pak->datagramsize = size;
        pak->datagramstart = pak->network_start - ETHER_SAP_ENCAPBYTES;
    }

    /*
     * Now, re-write the encapsulation and send the packet on its way.
     */
    ether_encap_rewrite(idb, pak, cache_adj);
    pak->if_output = swidb;
    (*idb->fastsend)(idb, pak);

    return(TRUE);
}

/*
 * fs_clns_fddi_dispatch()
 *      Routine to dispatch a clns packet that is headed out of an
 * FDDI interface.
 */
static inline boolean fs_clns_fddi_dispatch (hwidbtype *idb, 
                                             idbtype *swidb, paktype *pak,
                                             clns_cache_adjtype *cache_adj)
{
    int delta;

    if (pak_has_particles(pak)) {
        delta = FDDI_SAP_ENCAPBYTES - pak->encsize;
        if (!fs_clns_prepare_packet(pak, delta))
            return (FALSE);
    } else {
        pak->datagramsize = pak->datagramsize - pak->encsize + 
                                                FDDI_SAP_ENCAPBYTES;
        pak->datagramstart = pak->network_start - FDDI_SAP_ENCAPBYTES;
    }

    /*
     * Now, re-write the encapsulation and send the packet on its way.
     */
    fddi_encap_rewrite(idb, pak, cache_adj);
    pak->if_output = swidb;
    (*idb->fastsend)(idb, pak);

    return (TRUE);
}

/*
 * fs_clns_hdlc_dispatch()
 *      Routine to dispatch a clns packet that is headed out of an
 * HDLC encapsulated serial interface.
 */
static inline boolean fs_clns_hdlc_dispatch (hwidbtype *idb, 
                                             idbtype *swidb, paktype *pak,
                                             clns_cache_adjtype *cache_adj)
{
    int delta;

    /*
     * Switch to Serial.  Note, since we need to use the rewrite part of 
     * the cache, make sure it's valid. If it's not, send this packet up 
     * to process level and that will get it propagated for next time.
     */
    if (!cache_adj->length)
        return(FALSE);

    if (pak_has_particles(pak)) {
        delta = cache_adj->length - pak->encsize;
        if (!fs_clns_prepare_packet(pak, delta))
            return (FALSE);
    } else {
            pak->datagramstart = pak->network_start - cache_adj->length;
            pak->datagramsize = pak->datagramsize - pak->encsize
                                                  + cache_adj->length;
    }

    /*
     * Now, re-write the encapsulation and send the packet on its way.
     */
    bcopy(cache_adj->rewrite, pak->datagramstart, cache_adj->length);
    pak->if_output = swidb;
    (*idb->fastsend)(idb, pak);

    return(TRUE);
}

/*
 * fs_clns_atm_dispatch()
 *      Routine to dispatch a clns packet that is headed out of an
 * ATM interface.
 */
static inline boolean fs_clns_atm_dispatch (hwidbtype *idb, 
                                            idbtype *swidb, paktype *pak,
                                            clns_cache_adjtype *cache_adj)
{
    int delta;

    /*
     * Switch to ATM.  Note, since we need to use the rewrite part of
     * the cache, make sure it's valid. If it's not, send this packet up
     * to process level and that will get it propagated for next time.
     */
    if (!cache_adj->length)
        return(FALSE);

    if (pak_has_particles(pak)) {
        delta = cache_adj->length - pak->encsize;
        if (!fs_clns_prepare_packet(pak, delta))
            return (FALSE);
    } else {
            pak->datagramstart = pak->network_start - cache_adj->length;
            pak->datagramsize = pak->datagramsize - pak->encsize
                                                  + cache_adj->length;
    }

    /*
     * Now, re-write the encapsulation and send the packet on its way.
     */
    pak->atm_vcid = GETSHORT(cache_adj->rewrite);
    /*
     * This is specific to LANE.
     */
    if (is_atm_lane(swidb)) {
	int length;
        /*
	 * Write down the correct length of the datagram, rather than
	 * using the previous packet's length.
	 */
        length = pak->datagramsize - ATM_LANE_DATA_ENCAPBYTES;
        PUTSHORT(&cache_adj->rewrite[ATM_LANE_DATA_ENCAPBYTES - 2], length);
        pak->atm_enctype = GETSHORT(&cache_adj->rewrite[2]);
    }
    bcopy(cache_adj->rewrite, pak->datagramstart, cache_adj->length);
    pak->if_output = swidb;
    (*idb->fastsend)(idb, pak);

    return(TRUE);
}

/*
 * clns_fastswitch
 *
 * Normally there'd be a warning here about how you'd die if changes
 * you committed to this routine made it run slower.
 *
 * With CLNS, you'd have to work hard. But still be careful in here.
 */
boolean clns_fastswitch (paktype *pak)
{
    hwidbtype *idb;
    idbtype   *swidb;
    clns_cache_adjtype *cache_adj;
    clns_adjtype *adj;
    ulong src_addr_len, dest_addr_len, min_hdr_len, header_len; 
    ulong temp, c1, length, index;
    uchar *data;
    clnshdrtype *clns;

    clns = (clnshdrtype *)clnsheadstart(pak);
 
    if (clns->pid != NLP_ISO8473)
	return FALSE;
    if (clns->version != CLNS_VERSION)
	return(FALSE);
    if (clns->lifetime <= 1)
	return(FALSE);

    pak->linktype = LINK_CLNS;
    data = (uchar *)clns + CLNS_FIXEDHDR_SIZE;

    dest_addr_len = data[0];
    if (dest_addr_len >= CLNS_MAX_NSAP_LENGTH)
	return (FALSE);
    src_addr_len = *(data + dest_addr_len + 1);
    min_hdr_len = CLNS_FIXEDHDR_SIZE + 1 + dest_addr_len + 1 + src_addr_len + 
	(clns->sp ? sizeof(clns_segpart_t) : 0);
    header_len = clns->hlen;

    if (header_len > min_hdr_len) {
        uchar *ptr = (uchar *) &clns->pid;
	/*
	 * options are present
	 * in the packet. We handle only
 	 * the global QoS option and the padding 
	 * option for fast switching.
	 */
	if ((GETLONG(&ptr[min_hdr_len]) & CLNS_QOS_OPT_MASK) == 
	      CLNS_QOS_OPT_VALUE) {
	    min_hdr_len += CLNS_MIN_OPTION_SIZE;
	    if (header_len != min_hdr_len) { 
	        if (header_len < min_hdr_len)
	            return(FALSE);
	        if (ptr[min_hdr_len] != CLNS_OPT_PADDING)
	            return(FALSE);
	        if (header_len != min_hdr_len + ptr[min_hdr_len+1] + 2)
	            return(FALSE);	
	    }
	} else if (ptr[min_hdr_len] == CLNS_OPT_PADDING) {
	    min_hdr_len += ptr[min_hdr_len+1] + 2;
	    if (header_len != min_hdr_len) {
	        if (header_len < min_hdr_len)
	            return(FALSE);
	        if ((GETLONG(&ptr[min_hdr_len]) & CLNS_QOS_OPT_MASK) != 
		     CLNS_QOS_OPT_VALUE)
	            return(FALSE);
                if (header_len != min_hdr_len + CLNS_MIN_OPTION_SIZE)
	            return(FALSE);
	    }
	} else
	    return(FALSE);
    } else if (header_len != min_hdr_len) {
	return(FALSE);
    }

    if (!((clns->checksum[0] == 0) && (clns->checksum[1] == 0))) {
	/*
	 * verify the checksum 
	 */
	uchar *chkptr = (uchar *)clns;

	temp = c1 = 0;
	for (length = clns->hlen, index = 0; index < length; index++){
	    /* sum accumulation loop */
	    temp += chkptr[index];
	    c1   += temp;
	}
	/* adjust accumulated sums to mod 255 */
	temp = temp%255;
	c1 = c1%255;
	if (!((temp == 0) && (c1 == 0))) {
	    return FALSE;
	}
    }

    swidb = pak->if_input;
    idb   = swidb->hwptr;
 
    cache_adj = nsap_cache_search(data);
    if (cache_adj) {

	/*
	 * Filter the packet if appropriate.
	 */
	if (swidb->clns_filter_in) {

	    /*
	     * Tell lower layers that we have groveled in the packet
	     * more than normal for the fast switch path and the packet
	     * may need to be flushed out of the data cache for some
	     * platforms:
	     */
	    pak_set_dirty(pak);

	    if (!clns_filter_addr(swidb->clns_filter_in, 
				  (nsap_addr *) data,
				  (nsap_addr *) (data + 
		 		  ((nsap_addr *) data)->length + 1))) {
		return FALSE;
	    }
	}

	adj = cache_adj->adj;
	if ((adj->idb == swidb) || !(adj->idb->hwptr->clns_fastokay))
	    return(FALSE);
	if (adj->idb->clns_mtu < GETSHORT(clns->clns_seglen))
	    return(FALSE);
	/* 
	 * Do some accounting of packets and bytes
	 * in and out.
	 *
	 * This counting should really be done after we do output
	 * filtering, or any other path that might reject this packet.
	 * If you decide to fix this, beware, because the variable that
	 * what was the input idb becomes the output idb below. What a
	 * pain in the rear this code is! XXX
	 */ 
	clns_traffic[CLNS_INPUT]++;
	clns_traffic[CLNS_FORWARD]++;
	idb->counters.inputs_fast[ACCT_PROTO_CLNS]++; 
	idb->counters.rx_cumbytes_fast[ACCT_PROTO_CLNS] += pak->datagramsize;
	swidb = cache_adj->adj->idb;
	idb = swidb->hwptr;

	/* Filter the packet if appropriate. */

	if (swidb->clns_filter_out) {

	    /*
	     * Tell lower layers that we have groveled in the packet
	     * more than normal for the fast switch path and the packet
	     * may need to be flushed out of the data cache for some
	     * platforms:
	     */
	    pak_set_dirty(pak);

	    if (!clns_filter_addr(swidb->clns_filter_out, 
				  (nsap_addr *) data,
				  (nsap_addr *) (data + 
		 		  ((nsap_addr *) data)->length + 1))) {
		return FALSE;
	    }
	}

	pak->acct_proto = ACCT_PROTO_CLNS;

	if (!((clns->checksum[0] == 0) && (clns->checksum[1] == 0))) {
	    /*
	     * update checksum
	     */
	    clns->checksum[0] = (clns->checksum[0] + 5) % 255;
	    if (clns->checksum[1] < 4)
		clns->checksum[1] = (255 + clns->checksum[1] - 4);
	    else
		clns->checksum[1] = (clns->checksum[1] - 4) % 255;

	    if (clns->checksum[0] == 0)
		clns->checksum[0] = 255;
	    if (clns->checksum[1] == 0)
		clns->checksum[1] = 255;
	}
	--clns->lifetime;

	/*
	 * Note, if you're adding an outgoing encaps that requires
	 * use of the rewrite[] part of the cache, make sure it's
	 * valid first (see the HDLC example below).
	 */
	switch (idb->fast_switch_type) {

        /*
         * Fast-switch to a FDDI interface.
         */
	case FS_FDDI:
            return(fs_clns_fddi_dispatch(idb, swidb, pak, cache_adj));

        /*
         * Fast-switch to an ethernet interface.
         */
	case FS_ETHER:
            return(fs_clns_ether_dispatch(idb, swidb, pak, cache_adj));
 
        /*
         * Fast-switch to a HDLC encapsulated serial interface.
         */
	case FS_HDLC:
            return(fs_clns_hdlc_dispatch(idb, swidb, pak, cache_adj));
 
        /*
         * Fast-switch to an ATM interface.
         */
	case FS_AIP:
            return(fs_clns_atm_dispatch(idb, swidb, pak, cache_adj));
 
        /*
         * Fast-switch to a GRE Tunnel.
         */
	case FS_TUNNEL:
	    pak->if_output = swidb;
	    (*idb->fastsend)(idb, pak);
	    return(TRUE);

	default:
	    /*
	     * This really shouldn't happen, but we'll let process level
	     * handle it rather than just trash it
	     */
	    return(FALSE);
	}
    }
    return(FALSE);             
} /* end of clns_fastswitch */


static void clns_les_fs_register_drivers (ulong flags, hwidbtype* idb)
{
    /*
     * Check to see if we're configuring CLNS
     */
    if (!(flags & FSF_CLNS))
	return;

    if (idb->fast_switch_flag & FSF_CLNS) {
	reg_add_ether_fs(RXTYPE_CLNS,
			 (service_ether_fs_type)clns_fastswitch,
			 "clns_fastswitch");
	reg_add_fddi_fs(RXTYPE_CLNS,
			(service_fddi_fs_type)clns_fastswitch,
			"clns_fastswitch");
	reg_add_hdlc_fs(RXTYPE_CLNS,
			(service_hdlc_fs_type)clns_fastswitch,
			"clns_fastswitch");
	reg_add_aip_fs(RXTYPE_CLNS,
			(service_aip_fs_type)clns_fastswitch,
			"clns_fastswitch");
	reg_add_tunnel_fs(RXTYPE_CLNS,
			  (service_tunnel_fs_type)clns_fastswitch,
			  "clns_fastswitch");
    } else {
	fs_table_fill((fastswitch_type)return_false, "return_false",
		      RXTYPE_CLNS);
    }
}

static void clns_les_fs_init (subsystype* subsys)
{
    reg_add_register_drivers(clns_les_fs_register_drivers,
			     "clns_les_fs_register_drivers");
}

/*
 * CLNS LES FS subsystem header
 */
#define CLNSLESFS_MAJVERSION 1
#define CLNSLESFS_MINVERSION 0
#define CLNSLESFS_EDITVERSION  1

SUBSYS_HEADER(clns_les_fs, CLNSLESFS_MAJVERSION, CLNSLESFS_MINVERSION, 
              CLNSLESFS_EDITVERSION,
	      clns_les_fs_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: clns",
	      "req: fastswitch, clns");

/* end of clnsfast_les.c */
