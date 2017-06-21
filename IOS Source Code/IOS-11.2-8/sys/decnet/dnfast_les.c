/* $Id: dnfast_les.c,v 3.3.60.8 1996/08/14 02:36:12 hampton Exp $
 * $Source: /release/112/cvs/Xsys/decnet/dnfast_les.c,v $
 *------------------------------------------------------------------
 * dnfast_les.c -- DECnet fast switching support for Pancake
 *
 * 24-July-1990, Chris Shaker, derived from dnfast_mci.c, by Kirk Lougheed
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dnfast_les.c,v $
 * Revision 3.3.60.8  1996/08/14  02:36:12  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.3.60.7  1996/07/22  23:07:07  asastry
 * CSCdi62165:  DECnet fast-switching should be able to use DECnet default
 * route
 * Branch: California_branch
 *
 * Revision 3.3.60.6  1996/07/03  17:40:06  rbadri
 * CSCdi62015:  DECnet fast switching over LANE doesnt work
 * Branch: California_branch
 * Move the LANE specific checks to the case under AIP_FS, to
 * minimize impact to general code.
 *
 * Revision 3.3.60.5  1996/07/03  00:11:13  ssangiah
 * CSCdi58629: Need to add particle-based decnet fast-switching support.
 * Branch: California_branch
 *
 * Revision 3.3.60.4  1996/07/01  18:53:07  rbadri
 * CSCdi60085:  Spurious memory access made at atmzr_fastsend
 * Branch: California_branch
 * Set pak->if_output before calling the fast send routine
 * for ATM interfaces in the fast switch path.
 *
 * Revision 3.3.60.3  1996/05/09  14:12:27  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.60.2.4.1  1996/04/27  06:34:39  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.60.2  1996/04/17  13:36:36  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.3.60.1.8.1  1996/04/08  01:45:15  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.60.1  1996/03/18  19:11:46  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.64.1  1996/03/22  09:35:26  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.52.1  1996/03/05  05:49:19  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.40.1  1996/02/27  20:41:00  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.14.2  1996/02/14  05:35:12  rbadri
 * Fix compilation error.
 *
 * Revision 3.3.14.1  1996/02/13  00:17:23  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * DECnet routing for LANE
 *
 * Revision 3.3  1995/11/17  08:59:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:15:02  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:16:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  20:53:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/08/24  21:20:55  asastry
 * CSCdi38977:  decnet static routes breaks decnet fast switching
 *
 * Revision 2.3  1995/08/23  18:53:57  asastry
 * CSCdi38977:  DECnet static routes breaks decnet fast switching
 *
 * Revision 2.2  1995/07/18  04:15:59  shaker
 * CSCdi35990:  Need packet pool for fast switched packet structure
 * Use new pak_set_dirty() to only set PAK_BUF_DIRTY iff
 * PAK_BUF_USE_DIRTY is set. Set PAK_BUF_USE_DIRTY for RSP fs_pak
 *
 * Revision 2.1  1995/06/07  20:23:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "packet.h"
#include "logger.h"
#include "access.h"
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "../if/ether.h"
#include "config.h"
#include "../wan/serial.h"
#include "../if/fddi.h"
#include "../if/atm.h"
#include "../lane/lane.h"
#include "../srt/span.h"
#include "../les/if_les.h"
#include "../if/network.h"
#include "access.h"
#include "decnet.h"
#include "decnet_externs.h"
#include "dnconversion.h"

/*
 * "Get Out of Jail Free" Macro for DECnet fastswitch exceptions
 */

#define DNFAST_ABORT							\
    {									\
	/*								\
	 * re-write source and destination to recover from atg		\
	 */								\
	PUTSHORT(&p.u.dnptr->source, *(ushort *)source);		\
	PUTSHORT(&p.u.dnptr->dest, *(ushort *)dest);			\
									\
	return(FALSE);							\
    }

/*
 * "Cheat" defines for DECnet fastswitching codes
 */

#define CONST_AA_AA_03_00 0xAAAA0300  /* SNAP DSAP and SSAP, CTL and 1st byte of OUI */
#define CONST_00_00_60_03 0x00006003  /* Last 2 bytes of OUI, DECnet ARPA type code  */
#define CONST_0F_00_60_03 0x0F006003  /* HDLC station type, DECnet ARPA type code  */

/*
 * This structure will fool the compiler and force it to keep *data* and
 * *dnptr* in the same register, a performance consideration for high speed
 * switching.
 */
typedef struct reg_ {
    union {
	uchar *data;
	dndatapacket *dnptr;
    } u;
} reg;
 
/*
 * ether_encap_rewrite()
 *	Routine to re-write the ethernet encapsulation on a decnet packet
 * being fast-switched.
 */
static inline void ether_encap_rewrite (hwidbtype *output,
                                        paktype *pak, ushort gateway)
{
    ulong *macptr;
    
    macptr = (ulong *)pak->datagramstart;

    PUTLONG(macptr++, DN_MAC_CANONICAL);
    PUTLONG(macptr++, ((gateway << 16) | (0xAA00)));
    PUTLONG(macptr++, GETLONG(&output->hardware[2]));
    PUTSHORT(macptr, TYPE_DECNET);
}

/*
 * fddi_encap_rewrite()
 *	Routine to re-write the FDDI encapsulation on a decnet packet
 * being fast-switched.
 */
static inline void fddi_encap_rewrite (hwidbtype *output,
                                       paktype *pak, ushort gateway)
{
    ulong *macptr;

    *pak->datagramstart = FDDI_LLC_FC_BYTE;
    macptr = (ulong *)(pak->datagramstart + 1);
    if (output->type == IDBTYPE_FDDIT) {
        /*
         * FIP (FDDIT) on RSP requires cononical data:
         */
        PUTLONG(macptr++, DN_MAC_CANONICAL);
        PUTLONG(macptr++, ((gateway << 16) | (0xAA00)));
        PUTLONG(macptr++, GETLONG(&output->hardware[2]));
    } else {
        /*
         * Midrange FDDI requires bitswapped data:
         */
        PUTLONG(macptr++, DN_MAC_SWAPPED);
        PUTLONG(macptr++, (( ((bit_swaps[gateway >> 8] << 8) |
                              (bit_swaps[gateway & 0xff])) << 16) |
                           (0x5500)));
        PUTLONG(macptr++, GETLONG(&output->bit_swapped_hardware[2]));
    }
    PUTLONG(macptr++, CONST_AA_AA_03_00);
    PUTLONG(macptr, CONST_00_00_60_03);
}

/*
 * atm_encap_rewrite()
 *	Routine to re-write the ATM encapsulation on a decnet packet
 * being fast-switched.
 */
static inline void atm_encap_rewrite (hwidbtype *output, paktype *pak, 
                                      ushort lecid, ushort gateway)
{
    ushort *encptr;
    ulong *macptr;

    /*
     * Write down the ATM Encap header followed by LEC_ID
     */
    encptr = (ushort *)pak->datagramstart;

    PUTSHORT(encptr++, pak->atm_vcid);
    PUTSHORT(encptr++, pak->atm_enctype);
    PUTSHORT(encptr++, lecid);

    /*
     * Now write down the Ethernet encapsulation
     */
    macptr = (ulong *)(pak->datagramstart + ATM_AAL5_LANE_ENCAPBYTES);

    PUTLONG(macptr++, DN_MAC_CANONICAL);
    PUTLONG(macptr++, ((gateway << 16) | (0xAA00)));
    PUTLONG(macptr++, GETLONG(&output->hardware[2]));
    PUTSHORT(macptr, TYPE_DECNET);
}

/*
 * fs_decnet_prepare_packet()
 *	Prepare a packet prior to re-writing the outbound encapsulation.
 */
static inline boolean fs_decnet_prepare_packet (paktype *pak, int delta)
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
 * fs_decnet_ether_dispatch()
 *	Routine to dispatch a decnet packet that is headed out of an
 * ethernet interface.
 */
static inline boolean fs_decnet_ether_dispatch (hwidbtype *output,
                                                idbtype *swoutput,
                                                paktype *pak, ushort gateway)
{
    int delta, size;

    if (pak_has_particles(pak)) {
        /*
         * Note that all ethernet port adaptors (10Mb and 100Mb) do
         * automatic frame padding for small frames. So, there is
         * no need to check for minimum ethernet size frame.
         */
        delta = ETHER_ARPA_ENCAPBYTES - pak->encsize;
        if (!fs_decnet_prepare_packet(pak, delta))
            return (FALSE);
    }
    else {
        size = pak->datagramsize - pak->encsize + ETHER_ARPA_ENCAPBYTES;
        if (size < MINETHERSIZE)
            size = MINETHERSIZE;
        pak->datagramsize = size;
        pak->datagramstart = pak->network_start - ETHER_ARPA_ENCAPBYTES;
    }

    /*
     * Now, re-write the encapsulation and send the packet on its way.
     */
    ether_encap_rewrite(output, pak, gateway);
    pak->if_output = swoutput;
    (*output->fastsend)(output, pak);

    return (TRUE);
}

/*
 * fs_decnet_fddi_dispatch()
 *	Routine to dispatch a decnet packet that is headed out of an
 * FDDI interface.
 */
static inline boolean fs_decnet_fddi_dispatch (hwidbtype *output,
                                               idbtype *swoutput,
                                               paktype *pak, ushort gateway)
{
    int delta;

    if (pak_has_particles(pak)) {
        delta = FDDI_SNAP_ENCAPBYTES - pak->encsize;
        if (!fs_decnet_prepare_packet(pak, delta))
            return (FALSE);
    }
    else {
        pak->datagramsize = pak->datagramsize - pak->encsize 
                            + FDDI_SNAP_ENCAPBYTES;
        pak->datagramstart = pak->network_start - FDDI_SNAP_ENCAPBYTES;
    }

    /*
     * Now, re-write the encapsulation and send the packet on its way.
     */
    fddi_encap_rewrite(output, pak, gateway);
    pak->if_output = swoutput;
    (*output->fastsend)(output, pak);
 
    return (TRUE);
}

/*
 * fs_decnet_hdlc_dispatch()
 *	Routine to dispatch a decnet packet that is headed out of an
 * HDLC encapsulated serial interface.
 */
static inline boolean fs_decnet_hdlc_dispatch (hwidbtype *output,
                                               idbtype *swoutput,
                                               paktype *pak, ushort gateway)
{
    int delta;

    if (pak_has_particles(pak)) {
        delta = HDLC_ENCAPBYTES - pak->encsize;
        if (!fs_decnet_prepare_packet(pak, delta))
            return (FALSE);
    }
    else {
        pak->datagramsize = pak->datagramsize - pak->encsize + HDLC_ENCAPBYTES;
        pak->datagramstart = pak->network_start - HDLC_ENCAPBYTES;
    }

    /*
     * Now, re-write the encapsulation and send the packet on its way.
     */
    PUTLONG(pak->datagramstart, CONST_0F_00_60_03);
    if (output->board_encap)
        if (((*output->board_encap)(pak, output)) == FALSE)
            return(FALSE);
 
    pak->if_output = swoutput;
    (*output->fastsend)(output, pak);

    return (TRUE);
} 

/*
 * fs_decnet_atm_dispatch()
 *      Routine to dispatch a decnet packet that is headed out of an
 * ATM interface.
 */
static inline boolean fs_decnet_atm_dispatch (hwidbtype *output,
                                              idbtype *swoutput,
                                              paktype *pak, ushort gateway,
                                              dnroutetype *thisroute)
{
    ushort *encap_ptr; /* Temp ptr to play with "ushort" fields */
    ushort lecid;      /* For storing the LANE client id */
    int delta;

    if (pak_has_particles(pak)) {
        delta = ATM_AAL5_LANE_ENCAPBYTES + ETHER_ARPA_ENCAPBYTES 
                                         - pak->encsize;
        if (!fs_decnet_prepare_packet(pak, delta))
            return (FALSE);
    }
    else {
        pak->datagramsize = (pak->datagramsize + ATM_AAL5_LANE_ENCAPBYTES +
                             ETHER_ARPA_ENCAPBYTES - pak->encsize);
        if (pak->datagramsize < MINETHERSIZE)
            pak->datagramsize = MINETHERSIZE;
        pak->datagramstart = (pak->network_start - ETHER_ARPA_ENCAPBYTES -
                              ATM_AAL5_LANE_ENCAPBYTES);
    }

    encap_ptr = (ushort *)thisroute->encap_info;

    pak->atm_vcid    = *encap_ptr++;
    pak->atm_enctype = *encap_ptr++;
    lecid            = *encap_ptr;

    /*
     * Now, re-write the encapsulation and send the packet on its way.
     */
    atm_encap_rewrite(output, pak, lecid, gateway);

    pak->if_output = swoutput;
    (*output->fastsend)(output, pak);
    return (TRUE);
}

/*
 * dn_fastswitch
 * Highspeed switching of Ethernet, FDDI or HDLC encapsulated DECnet datagrams.
 * This routine has been very heavily optimized for speed, since it is in
 * the inner loop of the code that handles back-to-back datagrams.  Do not
 * add permanent debugging code, subroutines, or much of anything else.
 * The GNU CC compiler will generate the best code.
 * Speedups *must* be debugged with an oscilloscope.
 */

boolean dn_fastswitch (paktype *pak)	
{
    dnroutetype *thisroute, **routeptr;
    hwidbtype *output, *input;
    idbtype *swoutput, *swinput;
    uchar firstbyte;
    dnacctype *acl;
    dnaccitemtype *list;
    ulong wrkreg, ultemp;
    dnnet *net;
    uchar uctemp;
    reg p;
    ushort gateway;
    uchar source[2], dest[2];

    swinput = pak->if_input;
    input = swinput->hwptr;
    net = dn_nets[swinput->dn_network];
    if (net == NULL)
	return(FALSE);

    pak->linktype = LINK_DECNET;
    /*
     * Set up pointer to the base of the DECnet datagram in the packet
     * buffer in IO RAM. I'm assuming that it is faster just to access
     * the packet buffer where it lays, instead of copying parts it to
     * the IDB sniff buffer which is allocated in faster CPU RAM.
     */
    p.u.data = ((dnhdrtype *)dnheadstart(pak))->pakstart;
    firstbyte = *p.u.data;

    /*
     * Ensure that we have zero or one byte of DECnet padding:
     */
    if (firstbyte & DN_PAD) {
        if (firstbyte != (DN_PAD + 1))
	    return(FALSE);
	firstbyte = *(++p.u.data);
    }

    /*
     * Don't fast switch control or not-long packets:
     */
    firstbyte ^= DN_LONG;
    if (firstbyte & (DN_CTL+DN_LONG))
	return(FALSE);

    /*
     * Copy source and destination into local RAM for speed so we can
     * recover from later failures after ATG
     */
    *(ushort *)source = GETSHORT((ushort *)p.u.dnptr->source);
    *(ushort *)dest   = GETSHORT((ushort *)p.u.dnptr->dest);

    /*
     * Determine destination route and output interface.
     * Don't fast switch if no route or if we are routing out
     * the input interface. Output interface must be Lance
     * Ethernet or MK5025 Serial using HDLC encapsulation.
     */
    wrkreg = dn_area(dest);
    if (wrkreg == net->my_dn_area)
	routeptr = &net->dnroutes[dn_node(dest)];
    else
	if (net->doareas)
	    routeptr = &net->dnareas[wrkreg];
	else
	    routeptr = &net->dnroutes[0];

    if (!(thisroute = *routeptr)) {
        input->counters.inputs_fast[ACCT_PROTO_DECNET]++;
	input->counters.rx_cumbytes_fast[ACCT_PROTO_DECNET] += pak->datagramsize;
	goto dn_check_retreq;
    }

    /*
     * If the next hop type is "PhaseV", send to process level so that it
     * can be sent to CLNS.
     */
    if (thisroute->type == PHASEV)
	goto dn_check_retreq;

    /*
     * If routing to another network/area -- find the correct network entry.
     */
    uctemp = thisroute->flags;
    if (uctemp & DN_RT_NET)
	goto dn_check_atg;

 return_dn_check_atg:
    if (uctemp & DN_RT_ME)	/* if addressed to us */
	DNFAST_ABORT;

    /*
     * Select possible multipath route
     */
    if (thisroute->nextsplit)
	goto dn_nextsplit;

 return_dn_nextsplit:
    if (!(swoutput = thisroute->interface)) {
	/*
	 * If we have a static route that is not an interface static
	 * route (i.e. of the form 'dec route a.b x.y'), send packet
	 * to process level for processing.
	 */
	if (thisroute->type == PHASEIV_STATIC)
	    DNFAST_ABORT;

	goto dn_check_retreq;
    }
    output = swoutput->hwptr;

    /*
     * If HELLO for router, drop the datagram if return
     * not requested, else send to process level.
     */
    uctemp = thisroute->flags;
    if (uctemp  & (DN_RT_ROUTER | DN_RT_NET))
	goto dn_check_retreq;
    /*
     * If switching disabled -- send to process level.
     */
    if (!output->dn_fastokay )
	DNFAST_ABORT;

    /*
     * If access list(s) are present, run datagram by them.
     */
    if (swoutput->dn_accesslist)
	goto dn_access_check;

 return_dn_access_check:

    /*
     * Check for congestion encountered.  For DECnet this means
     * we set the congestion encountered bit if there are any packets
     * in the queue ahead of us.
     */
    if (output->output_qcount) {
        p.u.dnptr->next_level_2 |= DN_RTFL2_CON;
        dn_stat[DNST_CONGEST]++;
    }

    /*
     * Always clear the local bit unless packet is going back out
     * the same interface.
     */
    if ((swinput != swoutput) && (p.u.dnptr->typecode & DN_LOCAL))
	p.u.dnptr->typecode &= ~DN_LOCAL;

    /*
     * Do some accounting of packets and bytes input. Fastsend will
     * account for output packets:
     */
    input->counters.inputs_fast[ACCT_PROTO_DECNET]++;
    input->counters.rx_cumbytes_fast[ACCT_PROTO_DECNET] += pak->datagramsize;
    pak->acct_proto = ACCT_PROTO_DECNET;
    dn_stat[DNST_RECEIVED]++;
    dn_stat[DNST_DATAS]++;
    dn_stat[DNST_FORWARD]++;

    /*
     * Check hop count.
     */
    uctemp = p.u.dnptr->visits;
    if (uctemp >= (uchar)net->dn_max_visits)
	goto dn_check_retreq;
    uctemp++;
    p.u.dnptr->visits = uctemp;

    gateway = *(ushort *)thisroute->gateway;

    switch (output->fast_switch_type) {

     /*
      * Fast-switch to an ethernet interface.
      */
      case FS_ETHER:
          return(fs_decnet_ether_dispatch(output, swoutput, pak, gateway));

     /*
      * Fast-switch to a FDDI interface.
      */
      case FS_FDDI:
          return(fs_decnet_fddi_dispatch(output, swoutput, pak, gateway));

     /*
      * Fast-switch to a HDLC encapsulated serial interface.
      */
      case FS_HDLC:
          return(fs_decnet_hdlc_dispatch(output, swoutput, pak, gateway));
 
      /*
       * Fast-switch to a GRE Tunnel.
       */
      case FS_TUNNEL:
        pak->if_output = swoutput;
        (*output->fastsend)(output, pak);
        return(TRUE);

      /*
       * Fast-switch to an ATM interface. Only LANE fast switching is
       * supported right now.
       */
      case FS_AIP:
	if (is_atm_lane(swoutput)) {
        /*
	 * Move this check up, once everyone starts using cache_version
	 */
        if (thisroute->cache_version != swoutput->cache_version) {
            return (FALSE);
	}

	return(fs_decnet_atm_dispatch(output, swoutput, pak, 
				      gateway, thisroute));
	} else {
	    return (FALSE);
	}
      default:
	/*
	 * This really shouldn't happen, but we'll let process level
	 * handle it rather than just trash it
	 */
	return(FALSE);
    }

  dn_access_check:
    /*
     * Access lists present -- evaluate them.  Pass NSP (re-)connect
     * initiate packets to process level.
     */

    /*
     * Tell lower layers that we have groveled in the packet more than
     * normal for the fast switch path and the packet may need to be
     * flushed out of the data cache for some platforms:
     */
    pak_set_dirty(pak);

    if ((p.u.dnptr->databyte == NSP_CONNECT) ||
	(p.u.dnptr->databyte == NSP_RECONNECT))
	DNFAST_ABORT;

    ultemp = swoutput->dn_accesslist;
    if ((acl = dn_get_acl(ultemp)) != NULL) {
	list = acl->normal;
    } else {
	list = NULL;
    }
    if (list && !dn_accesscheck(p.u.dnptr, list))
	goto dn_check_retreq;

    goto return_dn_access_check;

 dn_nextsplit:
    if (!(thisroute = (dnroutetype *) dn_multipath(p.u.dnptr, net, thisroute)))
	DNFAST_ABORT;
    goto return_dn_nextsplit;

 dn_check_atg:
    if (!(net = dn_address_translation(p.u.dnptr, net)))
	goto dn_check_retreq;
    else {

	/*
	 * Network/area entry found -- try to determine route.
	 */
	routeptr = dn_get_route(p.u.dnptr, net);
	thisroute = *routeptr;
	if (!thisroute)
	    goto dn_check_retreq;
    }
    uctemp = thisroute->flags;
    goto return_dn_check_atg;

 dn_check_retreq:
    if ((p.u.dnptr->typecode & DN_RETREQ) || 
	reg_invoke_dnconversion_report() || decnet_default_route) {
	DNFAST_ABORT;
    } else {
	retbuffer(pak);
	return(TRUE);
    }
}

static void decnet_les_fs_register_drivers (ulong flags, hwidbtype* idb)
{
    /*
     * Check to see if we're configuring DECnet
     */
    if (!(flags & FSF_DECNET))
	return;

    if (idb->fast_switch_flag & FSF_DECNET) {
	reg_add_ether_fs(RXTYPE_DECNET,
			 (service_ether_fs_type)dn_fastswitch,
			 "dn_fastswitch");
	reg_add_fddi_fs(RXTYPE_DECNET,
			(service_fddi_fs_type)dn_fastswitch,
			"dn_fastswitch");
	reg_add_aip_fs(RXTYPE_DECNET,
			(service_aip_fs_type)dn_fastswitch,
			"dn_fastswitch");
	reg_add_hdlc_fs(RXTYPE_DECNET,
			(service_hdlc_fs_type)dn_fastswitch,
			"dn_fastswitch");
	reg_add_tunnel_fs(RXTYPE_DECNET,
			  (service_tunnel_fs_type)dn_fastswitch,
			  "dn_fastswitch");
    } else {
	fs_table_fill((fastswitch_type)return_false, "return_false",
		      RXTYPE_DECNET);
    }
}

static void decnet_les_fs_init (subsystype* subsys)
{
    reg_add_register_drivers(decnet_les_fs_register_drivers,
			     "decnet_les_fs_register_drivers");
}

/*
 * DECNET LES FS subsystem header
 */

#define DECNETLESFS_MAJVERSION 1
#define DECNETLESFS_MINVERSION 0
#define DECNETLESFS_EDITVERSION  1

SUBSYS_HEADER(decnet_les_fs, DECNETLESFS_MAJVERSION, DECNETLESFS_MINVERSION, DECNETLESFS_EDITVERSION,
	      decnet_les_fs_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: decnet",
	      "req: fastswitch, decnet");
