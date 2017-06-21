/* $Id: vinesfast_les.c,v 3.4.12.10 1996/08/19 16:55:46 ehk Exp $
 * $Source: /release/112/cvs/Xsys/vines/vinesfast_les.c,v $
 *------------------------------------------------------------------
 * vinesfast_les.c -- VINES Fast Switching module providing low end
 *                    specific Support.
 *
 * July 1992, David R. Hampton
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vinesfast_les.c,v $
 * Revision 3.4.12.10  1996/08/19  16:55:46  ehk
 * CSCdi66379:  vines fast switching fails to route Ether SNAP pkt if >
 * 1478 bytes
 * Branch: California_branch
 * do not include MAC header length in the SNAP packet lengt
 *
 * Revision 3.4.12.9  1996/08/13  14:34:57  rbadri
 * CSCdi64089:  Multiprotocol support fixes required
 * Branch: California_branch
 * Vines support for LANE
 *
 * Revision 3.4.12.8  1996/07/24  01:21:46  ehk
 * CSCdi63737:  %ALIGN-3-CORRECT: Alignment correction made at 0x60427FC0
 * writing
 * Branch: California_branch
 * avoid alignment error with PUTLONG macro
 *
 * Revision 3.4.12.7  1996/07/17  00:02:59  ehk
 * CSCdi59059:  Out of order packets when using vines route-cache
 * Branch: California_branch
 * invalidate a fast cache entry before passing to process switching
 * a packet with metric notification bit set and relearn a new cache
 * to maintain original packet ordering.
 *
 * Revision 3.4.12.6  1996/05/05  06:56:31  ehk
 * CSCdi53535:  Misligned data during vines testing
 * Branch: California_branch
 * another misaligned memory access missed in the previous fix.
 *
 * Revision 3.4.12.5  1996/04/27  06:30:25  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.3.16.3  1996/04/25  23:55:41  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.3.16.2  1996/04/16  23:56:31  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.3.16.1  1996/02/23  20:59:55  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.4.12.4  1996/04/17  13:50:04  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.4.12.3  1996/04/03  04:26:12  fsunaval
 * CSCdi53159:  Vines fast switching is broken on new PAs.
 * Branch: California_branch
 *
 * Revision 3.4.12.2  1996/03/22  00:14:39  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.4.12.1  1996/03/18  22:32:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.5  1996/03/18  23:51:34  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.4  1996/03/06  19:16:08  gstovall
 * CSCdi50773:  SMDS fastswitching fails without smds dxi-mode.
 * Branch: ELC_branch
 * Handle messed up encapsize in a better way.
 *
 * Revision 3.2.2.3  1996/03/01  23:33:44  gstovall
 * CSCdi49899:  %Software-forced reload occurred while verifying smds over
 * serial.
 * Branch: ELC_branch
 * Deal with that pesky SMDS trailer correctly.
 *
 * Revision 3.2.2.2  1996/03/01  18:29:32  gstovall
 * CSCdi50508:  Particle based vines fastswitching not supported
 * Branch: ELC_branch
 * Bring the joy and splendor of particles to vines fastswtiching.
 *
 * Revision 3.2.2.1  1995/12/11  19:50:15  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.4  1996/02/13  08:17:11  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.3  1995/12/01  21:39:38  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.2  1995/11/17  17:59:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:45:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:34:22  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/18  04:16:27  shaker
 * CSCdi35990:  Need packet pool for fast switched packet structure
 * Use new pak_set_dirty() to only set PAK_BUF_DIRTY iff
 * PAK_BUF_USE_DIRTY is set. Set PAK_BUF_USE_DIRTY for RSP fs_pak
 *
 * Revision 2.1  1995/06/07  23:15:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Routines Exported from this module:
 *
 * vines_les_fastswitch: Fast switches vines packets (called via the
 * various fast switching registries.)
 */

#include "master.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "logger.h"

#include "../if/network.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../if/atm.h"
#include "../lane/lane.h"
#include "../wan/serial.h"
#include "../wan/dialer.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"
#include "../wan/ppp.h"
#include "vines_private.h"
#include "vinesfast.h"
#include "vines_ipc.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

/*
 * Globals
 */
static boolean (*vfs_fs_out[V_FS_MAX])(paktype *, vinesfastpathtype *);

/*
 * Layout of datagram sniff buffer
 */

/*
 *	     ARPA	    SAP		   SNAP		   HDLC
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
 *
 *
 *        Frame Relay   Frame Relay 
 *                         SNAP         
 *                  	           
 *                  	           
 *                  	           
 *                  	           
 *   0    DLCI  P  P	DLCI  DS SS
 *   1    x  x  x  x	CT SN SN SN
 *   2    x  x  x  x	P  P  x  x      
 *   3    x  x  x  x	x  x  x  x      
 *   4    x  x  x  x	x  x  x  x      
 *   5    x  x  x  x	x  x  x  x      
 *   6   		x  x  x  x 
 *   7   		x  x  x  x 
 *       
 */


/**********************************************************************
 *
 *		   LOW END HEADER REWRITE ROUTINES
 *
 **********************************************************************/

/*
 * vines_fs_out_ether_snap
 *
 * Write a SNAP ethernet header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.
 */
static boolean vines_fs_out_ether_snap (
    paktype *pak,
    vinesfastpathtype *path)
{
    ulong *macptr, *cache_macptr;

    if (pak_has_particles(pak)) {
	particle_clip_inline(pak->particlequeue.qhead,
			     E_SNAP_HDR_BYTES_OUT - pak->encsize);
    }

    pak->datagramstart = pak->network_start - E_SNAP_HDR_BYTES_OUT;
    pak->datagramsize = E_SNAP_HDR_BYTES_OUT + pak->length;
    if (pak->datagramsize < MINETHERSIZE)
	pak->datagramsize = MINETHERSIZE;

    macptr = (ulong *)pak->datagramstart;
    cache_macptr = path->vinesp_mh.mac_longs;
    PUTLONG(macptr++, *cache_macptr++);
    PUTLONG(macptr++, *cache_macptr++);
    PUTLONG(macptr++, *cache_macptr);
    PUTLONG(macptr++, (((pak->length+SNAP_HDRBYTES) << 16) | SNAPSNAP));
    PUTLONG(macptr, TYPE_VINES2);
    pak->if_output = path->idb;
    (*path->idb->hwptr->fastsend)(path->idb->hwptr, pak);
    return(TRUE);
}

/*
 * vines_fs_out_ether_arpa
 *
 * Write an ARPA ethernet header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.
 */
static boolean vines_fs_out_ether_arpa (
    paktype *pak,
    vinesfastpathtype *path)
{
    ulong *macptr, *cache_macptr;

    /*
     * Switch out an ethernet ARPA style
     */
    if (pak_has_particles(pak)) {
	particle_clip_inline(pak->particlequeue.qhead,
			     E_ARPA_HDR_BYTES_OUT - pak->encsize);
    }

    pak->datagramstart = pak->network_start - E_ARPA_HDR_BYTES_OUT;
    pak->datagramsize = E_ARPA_HDR_BYTES_OUT + pak->length;
    if (pak->datagramsize < MINETHERSIZE)
	pak->datagramsize = MINETHERSIZE;

    macptr = (ulong *)pak->datagramstart;
    cache_macptr = path->vinesp_mh.mac_longs;

    PUTLONG(macptr++, *cache_macptr++);
    PUTLONG(macptr++, *cache_macptr++);
    PUTLONG(macptr++, *cache_macptr);

    PUTSHORT((ushort *)macptr,TYPE_VINES);
    pak->if_output = path->idb;
    (*path->idb->hwptr->fastsend) (path->idb->hwptr, pak);
    return(TRUE);
}

/*
 * vines_fs_out_fddi
 *
 * Write a FDDI SNAP header onto the front of the supplied packet, and
 * then hand the packet off to the transmit code.
 */
static boolean vines_fs_out_fddi (
    paktype *pak,
    vinesfastpathtype *path)
{
    ulong *macptr, *cache_macptr;
    ushort *macptrs, *cache_macptrs;
    int i;

    if (pak_has_particles(pak)) {
	particletype *rw_p;
	int delta;

	/*
	 * For FDDI, the re-write can sometimes fit over the original
	 * re-write and sometimes it will need a new particle due to
	 * a large RIF. We test for this, as overwriting the incoming
	 * encapsulation is more efficient.
	 *
	 * delta is the route cache rewrite lenght, plus 1 byte for LLC
	 * FC.
	 */
	delta = F_SNAP_HDR_BYTES_OUT - pak->encsize;
	if (delta < PARTICLE_ENCAPBYTES) {

	    /*
	     * The rewrite can fit over the incoming encaps
	     */
	    particle_clip_inline(pak->particlequeue.qhead, delta);
	    pak->datagramsize += delta;
	    pak->datagramstart -=  delta;

	} else {
	    /*
	     * Cannot fit the rewrite over the existing encaps. Must
	     * allocate re-write particle. If we fail to get one, return
	     * FALSE, punting the packet to process level.
	     */
	    rw_p = allocate_fs_particle();
	    if (!rw_p) {
		return(FALSE);
	    }

	    /*
	     * Fixup packet pointers. Strip off the encapsulation
	     * from the first particle, setup the new encaps particle
	     * and append it onto the packet.
	     */
	    particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
	    rw_p->data_bytes = F_SNAP_HDR_BYTES_OUT;
	    pak->datagramstart = rw_p->data_start;
	    pak->datagramsize += delta;
	    requeue_inline(&pak->particlequeue, rw_p);
	}
    } else {
	pak->datagramstart = pak->network_start - F_SNAP_HDR_BYTES_OUT;
	pak->datagramsize = F_SNAP_HDR_BYTES_OUT + pak->length;
    }

    macptr = (ulong *)(pak->datagramstart + 1);
    cache_macptr = path->vinesp_mh.mac_longs;

    *(uchar *)pak->datagramstart = FDDI_LLC_FC_BYTE;
    PUTLONG(macptr++, *cache_macptr++);
    PUTLONG(macptr++, *cache_macptr++);
    PUTLONG(macptr++, *cache_macptr++);

    macptrs = (ushort *)macptr;
    cache_macptrs = (ushort *)cache_macptr;
    for (i = path->rifwordlength; i > 0; i--)
	PUTSHORT(macptrs++, *cache_macptrs++);

    macptr = (ulong *)macptrs;
    PUTLONG(macptr++, (SNAPSNAP << 16) | (LLC1_UI << 8));
    PUTLONG(macptr, TYPE_VINES2);

    pak->if_output = path->idb;
    (*path->idb->hwptr->fastsend)(path->idb->hwptr, pak);
    return(TRUE);
}

/*
 * vines_fs_out_token_snap
 *
 * Write a SNAP Token Ring header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.
 */
static boolean vines_fs_out_token_snap (
    paktype *pak,
    vinesfastpathtype *path)
{
    ulong *macptr, *cache_macptr;
    ushort *macptrs, *cache_macptrs;
    int i;

    if (pak_has_particles(pak)) {
	particletype *rw_p;
	int delta;

	/*
	 * For Token Ring, the re-write can sometimes fit over the original
	 * re-write and sometimes it will need a new particle due to
	 * a large RIF. We test for this, as overwriting the incoming
	 * encapsulation is more efficient.
	 */
	delta = T_SNAP_HDR_BYTES_OUT - pak->encsize;
	if (delta > PARTICLE_ENCAPBYTES) {

	    /*
	     * The rewrite can fit over the incoming encaps
	     */
	    particle_clip_inline(pak->particlequeue.qhead, delta);
	    pak->datagramsize = pak->datagramsize + T_SNAP_HDR_BYTES_OUT;
	    pak->datagramstart =  pak->datagramstart - delta;

	} else {
	    /*
	     * Cannot fit the rewrite over the existing encaps. Must
	     * allocate re-write particle. If we fail to get one, return
	     * FALSE, punting the packet to process level.
	     */
	    rw_p = allocate_fs_particle();
	    if (!rw_p) {
		return(FALSE);
	    }

	    /*
	     * Fixup packet pointers. Strip off the encapsulation
	     * from the first particle, setup the new encaps particle
	     * and append it onto the packet.
	     */
	    particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
	    rw_p->data_bytes = T_SNAP_HDR_BYTES_OUT;
	    pak->datagramsize = pak->datagramsize + T_SNAP_HDR_BYTES_OUT;
	    pak->datagramstart = rw_p->data_start;
	    requeue_inline(&pak->particlequeue, rw_p);
	}
    } else {
	pak->datagramstart = pak->network_start - T_SNAP_HDR_BYTES_OUT;
	pak->datagramsize = T_SNAP_HDR_BYTES_OUT + pak->length;
    }
    macptr = (ulong *)(pak->datagramstart + 2);
    cache_macptr = path->vinesp_mh.mac_longs;

    *(ushort *)pak->datagramstart = (AC_PRI0 << 8) | FC_TYPE_LLC;
    *macptr++ = *cache_macptr++;
    *macptr++ = *cache_macptr++;
    *macptr++ = *cache_macptr++;

    macptrs = (ushort *)macptr;
    cache_macptrs = (ushort *)cache_macptr;
    for (i = path->rifwordlength; i > 0; i--)
	*macptrs++ = *cache_macptrs++;

    macptr = (ulong *)macptrs;
    *macptr++ = (SNAPSNAP << 16) | (LLC1_UI << 8);
    *macptr   = TYPE_VINES2;

    pak->if_output = path->idb;
    (*path->idb->hwptr->fastsend)(path->idb->hwptr, pak);
    return(TRUE);
}

/*
 * vines_fs_out_token_vines
 *
 * Write a Banyan Token Ring header (SAP + demux byte) onto the front of
 * the supplied packet, and then hand the packet off to the transmit
 * code.
 */
static boolean vines_fs_out_token_vines (
    paktype *pak,
    vinesfastpathtype *path)
{
    ulong *macptr, *cache_macptr;
    ushort *macptrs, *cache_macptrs;
    int i;

    if (pak_has_particles(pak)) {
	particletype *rw_p;
	int delta;

	/*
	 * For Token Ring, the re-write can sometimes fit over the original
	 * re-write and sometimes it will need a new particle due to
	 * a large RIF. We test for this, as overwriting the incoming
	 * encapsulation is more efficient.
	 */
	delta = T_VINES_HDR_BYTES_OUT - pak->encsize;
	if (delta > PARTICLE_ENCAPBYTES) {

	    /*
	     * The rewrite can fit over the incoming encaps
	     */
	    particle_clip_inline(pak->particlequeue.qhead, delta);
	    pak->datagramsize = pak->datagramsize + T_VINES_HDR_BYTES_OUT;
	    pak->datagramstart =  pak->datagramstart - delta;

	} else {
	    /*
	     * Cannot fit the rewrite over the existing encaps. Must
	     * allocate re-write particle. If we fail to get one, return
	     * FALSE, punting the packet to process level.
	     */
	    rw_p = allocate_fs_particle();
	    if (!rw_p) {
		return(FALSE);
	    }

	    /*
	     * Fixup packet pointers. Strip off the encapsulation
	     * from the first particle, setup the new encaps particle
	     * and append it onto the packet.
	     */
	    particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
	    rw_p->data_bytes = T_VINES_HDR_BYTES_OUT;
	    pak->datagramsize = pak->datagramsize + T_VINES_HDR_BYTES_OUT;
	    pak->datagramstart = rw_p->data_start;
	    requeue_inline(&pak->particlequeue, rw_p);
	}
    } else {
	pak->datagramstart = pak->network_start - T_VINES_HDR_BYTES_OUT;
	pak->datagramsize = T_VINES_HDR_BYTES_OUT + pak->length;
    }

    macptr = (ulong *)(pak->datagramstart + 2);
    cache_macptr = path->vinesp_mh.mac_longs;

    PUTSHORT((ushort *)pak->datagramstart, (AC_PRI0 << 8) | FC_TYPE_LLC);
    PUTLONG(macptr++, *cache_macptr++);
    PUTLONG(macptr++, *cache_macptr++);
    PUTLONG(macptr++, *cache_macptr++);

    macptrs = (ushort *)macptr;
    cache_macptrs = (ushort *)cache_macptr;
    for (i = path->rifwordlength; i > 0; i--)
	PUTSHORT(macptrs++, *cache_macptrs++);

    macptr = (ulong *)macptrs;
    PUTLONG(macptr, (SAP_VINES << 24) | (SAP_VINES << 16) |
	    (LLC1_UI << 8) | VINES_DEMUX_IP);

    pak->if_output = path->idb;
    (*path->idb->hwptr->fastsend)(path->idb->hwptr, pak);
    return(TRUE);
}

/*
 * vines_fs_out_hdlc
 *
 * Write a HDLC header onto the front of the supplied packet, and then
 * hand the packet off to the transmit code.
 */
static boolean vines_fs_out_hdlc (
    paktype *pak,
    vinesfastpathtype *path)
{
    ushort *macptr;
    
    /*
     * For HDLC serial, we are always guaranteed that the rewrite will
     * fit over the incoming encapsulation even for scattered packets,
     * so there is no need to allocate a new particle.
     */
    if (pak_has_particles(pak)) {
	particle_clip_inline(pak->particlequeue.qhead,
			     path->reallength - pak->encsize);
    }

    pak->datagramsize = path->reallength + pak->length;
    pak->datagramstart = pak->network_start - path->reallength;
    macptr = (ushort *)pak->datagramstart;
    if (path->reallength > HDLC_ENCAPBYTES) {
	PUTSHORT(macptr++, path->vinesp_mh.mac_shorts[0]);
    }
    PUTLONG((ulong *)macptr, HDLC_VINESCODE);
	
    pak->if_output = path->idb;
    (*path->idb->hwptr->fastsend)(path->idb->hwptr, pak);
    return(TRUE);
}

/*
 * vines_fs_out_ppp
 *
 * Write a PPP header onto the front of the supplied packet, and then
 * hand the packet off to the transmit code.
 */
static boolean vines_fs_out_ppp (
    paktype *pak,
    vinesfastpathtype *path)
{
    ushort *macptr;

    /*
     * For PPP serial, we are always guaranteed that the rewrite will
     * fit over the incoming encapsulation even for scattered packets,
     * so there is no need to allocate a new particle.
     */
    if (pak_has_particles(pak)) {
	particle_clip_inline(pak->particlequeue.qhead,
			     path->reallength - pak->encsize);
    }

    pak->datagramsize = path->reallength + pak->length;
    pak->datagramstart = pak->network_start - path->reallength;
    macptr = (ushort *)pak->datagramstart;
    if (path->reallength > PPP_ENCAPBYTES) {
	PUTSHORT(macptr++, path->vinesp_mh.mac_shorts[0]);
    }
    PUTLONG((ulong *)macptr, ((PPP_STATION << 16) | TYPE_PPP_VINES));
    pak->if_output = path->idb;
    (*path->idb->hwptr->fastsend)(path->idb->hwptr, pak);
    return(TRUE);
}

/*
 * vines_fs_out_frame
 *
 * Write a Non-IETF Frame Relay header onto the front of the supplied
 * packet, and then hand the packet off to the transmit code.
 */
static boolean vines_fs_out_frame (
    paktype *pak,
    vinesfastpathtype *path)
{
    ulong *macptr, *cache_macptr;
    hwidbtype *hwidb;

    /*
     * For Frame Relay serial, we are always guaranteed that the rewrite will
     * fit over the incoming encapsulation even for scattered packets,
     * so there is no need to allocate a new particle.
     */
    if (pak_has_particles(pak)) {
	particle_clip_inline(pak->particlequeue.qhead,
			     path->reallength - pak->encsize);
    }

    pak->datagramstart = pak->network_start - path->reallength;
    pak->datagramsize = path->reallength + pak->length;
    cache_macptr = path->vinesp_mh.mac_longs;
    macptr = (ulong *)pak->datagramstart;

    PUTLONG(macptr++, *cache_macptr++);
    if (path->reallength > FRAME_RELAY_ENCAPBYTES) {
	PUTSHORT((ushort *)macptr, *(ushort *)cache_macptr);
    }
    hwidb = path->idb->hwptr;
    INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(hwidb, path->subint_reference_num),
			pak->datagramsize);
    pak->if_output = path->idb;
    (*hwidb->fastsend)(hwidb, pak);
    return(TRUE);
}

/*
 * vines_fs_out_frame_ietf
 *
 * Write a IETF Frame Relay header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.
 */
static boolean vines_fs_out_frame_ietf (
    paktype *pak,
    vinesfastpathtype *path)
{
    ulong *macptr, *cache_macptr;
    ushort *macptrs, *cache_macptrs, ix;
    hwidbtype *hwidb;

    /*
     * For Frame Relay serial, we are always guaranteed that the rewrite will
     * fit over the incoming encapsulation even for scattered packets,
     * so there is no need to allocate a new particle.
     */
    if (pak_has_particles(pak)) {
	particle_clip_inline(pak->particlequeue.qhead,
			     path->reallength - pak->encsize);
    }

    ix = path->reallength;
    pak->datagramstart = pak->network_start - ix;
    pak->datagramsize = ix + pak->length;
    cache_macptr = path->vinesp_mh.mac_longs;
    macptr = (ulong *)pak->datagramstart;

    do {
	PUTLONG(macptr++, *cache_macptr++);
	ix -= sizeof(ulong);
    } while (ix >= sizeof(ulong));

    if (ix) {
	macptrs = (ushort *)macptr;
	cache_macptrs = (ushort *)cache_macptr;
	PUTSHORT(macptrs++, *cache_macptrs++);
    }
    hwidb = path->idb->hwptr;
    INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(hwidb, path->subint_reference_num),
			pak->datagramsize);
    pak->if_output = path->idb;
    (*hwidb->fastsend)(hwidb, pak);
    return(TRUE);
}
/*
 * vines_fs_out_atm_dxi
 *
 * Write a atm-dxi header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.
 */
static boolean vines_fs_out_atm_dxi (
    paktype *pak,
    vinesfastpathtype *path)
{
    ulong *macptr, *cache_macptr;
    ulong encap_len;
    hwidbtype *hwidb;

    /*
     * For ATM-DXI serial, we are always guaranteed that the rewrite will
     * fit over the incoming encapsulation even for scattered packets,
     * so there is no need to allocate a new particle.
     */
    if (pak_has_particles(pak)) {
	particle_clip_inline(pak->particlequeue.qhead,
			     path->reallength - pak->encsize);
    }

    encap_len = path->reallength;
    pak->datagramstart = pak->network_start - encap_len;
    pak->datagramsize = encap_len + pak->length;
    cache_macptr = path->vinesp_mh.mac_longs;
    macptr = (ulong *)pak->datagramstart;

    for (;encap_len >= sizeof(ulong); encap_len -= sizeof(ulong)) {
	PUTLONG(macptr++, *cache_macptr++);
    }
    if (encap_len) {
	PUTSHORT(((ushort *)macptr), *(ushort *)cache_macptr);
    }
    hwidb = path->idb->hwptr;
    INC_DXI_PVC_FAST_OUTPUT(HWIDB_TO_DXI_PVC(hwidb,
					     path->subint_reference_num),
			    pak->datagramsize);
    pak->if_output = path->idb;
    (*hwidb->fastsend)(hwidb, pak);
    return(TRUE);
}

/*
 * vines_fs_out_smds
 *
 * Write the entire smds header onto the front of the supplied
 * packet.
 */
static boolean vines_fs_out_smds (
    paktype *pak,
    vinesfastpathtype *path)
{
    ulong *macptr, *cache_macptr;
    hwidbtype *output = path->idb->hwptr;
    smdstype *sm;
    smds_pdb_t *spd = output->smds_pdb;
    ushort len, pad, ix;
    ulong *tailptr;
    particletype *tail_p = NULL;
    
    if (pak_has_particles(pak)) {
	particletype *rw_p;

	/*
	 * Always allocate a new particle
	 */
	rw_p = allocate_fs_particle();
	if (!rw_p) {
	    return(FALSE);
	}

	/*
	 * Fixup packet pointers. Strip off the encapsulation
	 * from the first particle, setup the new encaps particle
	 * and append it onto the packet.
	 */
	particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
	rw_p->data_bytes = path->reallength;
	pak->datagramstart = rw_p->data_start;
	requeue_inline(&pak->particlequeue, rw_p);

    } else {
	pak->datagramstart = pak->network_start - path->reallength;
	tailptr = NULL;
    }
    len = pak->length;

    cache_macptr = path->vinesp_mh.mac_longs;
    macptr = (ulong *)pak->datagramstart;

    sm = (smdstype *)(pak->datagramstart + spd->fast_output_modelen);
    for (ix = path->reallength; ix >= sizeof(ulong); ix -= sizeof(ulong)) {
	PUTLONG(macptr++, *cache_macptr++);
    }
    if (ix) {
	PUTSHORT((ushort *)macptr, *(ushort *)cache_macptr);
    }

    pad = 3 - ((len + 3) % 4);
    len += path->reallength;
    sm->smds_xhlpi &= ~3;
    sm->smds_xhlpi |= pad;
    sm->smds_betag = output->smds_begin_tag++;
    PUTSHORT(&sm->smds_basize, (len - (sizeof(ulong) + 
			       spd->fast_output_modelen) + pad));
    /* add SMDS pad */
    macptr = (ulong *)(pak->datagramstart + len);
    PUTLONG(macptr, 0);
    /* add SMDS trailer */
    pak->datagramsize = len + sizeof(ulong) + pad;

    /*
     * Sigh... do the ugly dance to get the length adjusted for the stupid
     * SMDS trailer
     */
    if (pak_has_particles(pak)) {
	tail_p = pak->particlequeue.qtail;
	tailptr = (ulong *)(tail_p->data_start + tail_p->data_bytes + pad);
	tail_p->data_bytes = tail_p->data_bytes + sizeof(ulong) + pad;
    } else {
	tailptr = (ulong *)(pak->datagramstart + pak->datagramsize -
			    sizeof(ulong));
    }
    macptr = tailptr;
    PUTLONG(macptr, GETLONG((ulong *)&sm->smds_rsvd));
    pak->if_output = path->idb;
    (*output->fastsend)(output, pak);
    return(TRUE);
}

/*
 * vines_fs_out_atm
 *
 * Write an atm header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.
 */
static boolean vines_fs_out_atm (
    paktype *pak,
    vinesfastpathtype *path)
{
    ulong *macptr, *cache_macptr;
    ulong encap_len;
    hwidbtype *hwidb;

    encap_len = path->reallength;

    /*
     * For ATM we are always guaranteed that the rewrite will
     * fit over the incoming encapsulation even for scattered packets,
     * so there is no need to allocate a new particle.
     *
     * We may need to revisit this as more encaps are added to ATM, but
     * at this point (Ether type LANE only) there is no problem.
     */
    if (pak_has_particles(pak)) {
        particle_clip_inline(pak->particlequeue.qhead,
                             encap_len - pak->encsize);
    }

    pak->datagramstart = pak->network_start - encap_len;
    pak->datagramsize = encap_len + pak->length;
    cache_macptr = path->vinesp_mh.mac_longs;
    pak->atm_vcid = path->vinesp_mh.mac_shorts[0];
    if (is_atm_lane(path->idb)) {
        pak->atm_enctype = ATM_DM_PKT_AAL5;
    }
    macptr = (ulong *)pak->datagramstart;

    bcopy(cache_macptr, macptr, encap_len);

    hwidb = path->idb->hwptr;

    pak->if_output = path->idb;
    (*hwidb->fastsend)(hwidb, pak);
    return(TRUE);
}

/*
 * vines_fs_out_tunnel
 *
 * Just hand off the packet to the fastsend function.
 */
static boolean vines_fs_out_tunnel (
    paktype *pak,
    vinesfastpathtype *path)
{
    hwidbtype *hwidb;

    hwidb = path->idb->hwptr;

    pak->if_output = path->idb;
    (*hwidb->fastsend)(hwidb, pak);
    return(TRUE);
}

/**********************************************************************
 *
 *		      LOW END INPUT ROUTINES
 *
 **********************************************************************/

/*
 * vines_access_check
 * Determine if we are allowed to forward the passed datagram, based on the
 * VINES access pointer. Return FALSE if output access is denied.
 */
static boolean vines_access_check_les (
    paktype *pak,
    ulong list)
{
    vinesiptype *vip;
    vinesipctype *ipc;
    vinesacctype *item;
    ulong dstnet, srcnet;
    ushort dsthost, srchost;

    /*
     * Tell lower layers that we have groveled in the packet more
     * than is normal for the fast switch path and the packet may
     * need to be flushed out of the data cache for some platforms:
     */
    pak_set_dirty(pak);

    vip = (vinesiptype *)pak->network_start;
    item = (vinesacctype *)vines_access_lists[list];
    dstnet = GET2WD(vip->ddstnet);
    dsthost = vip->dsthost;
    srcnet = GET2WD(vip->ssrcnet);
    srchost = vip->srchost;
    do {
        if ((dsthost & item->dmaskhost) != item->dsthost)
            continue;
        if ((dstnet & item->dmasknet) != item->dstnet)
            continue;
        if ((srchost & item->smaskhost) != item->srchost)
            continue;
        if ((srcnet & item->smasknet) != item->srcnet)
            continue;
        if (item->protocol) {
            /* it is an extended access list */
            if (item->protocol != vip->protocol)
                continue;
	    if ((item->protocol == VINES_IPC) || (item->protocol == VINES_SPP)) {
		ipc = (vinesipctype *)vip->data;
		if ((ipc->srcport & item->smasksock) != item->srcsocket)
		    continue;
		if ((ipc->dstport & item->dmasksock) != item->dstsocket)
		    continue;
	    }
	}
	return(item->grant);
      } while ((item = item->next));
    return(FALSE);
}

/*
 * vines_les_fastswitch()
 *
 * The code below has been lovingly hand-crafted to provide a lifetime of
 * reliable use and happiness. ;-)
 *
 * Don't change this stuff unless you're prepared to spend time poring
 * over hot and sweaty assembler listings and HP analyzer timing output...
 */
static boolean vines_les_fastswitch (paktype *pak)
{
    vinescachetype *cache;
    vinesfastpathtype *path;
    vinesiptype* vinesip;
    idbtype *output;
    hwidbtype *hwinput;
    ushort indx, dsthost;
    ulong dstnet;
    
    pak->linktype = LINK_VINES;
    vinesip = (vinesiptype *)pak->network_start;
    dstnet = GETLONG(vinesip->ddstnet);
    dsthost = GETSHORT(&vinesip->dsthost);
    if (vinesip->tc & VINES_METRIC) {
	vines_invalidate_cache_entry(dstnet, dsthost);
        return(FALSE);
    }
    if ((vinesip->tc & VINES_HOPS) == 0)
        return(FALSE);

    indx = dstnet >> 16;
    indx ^= dstnet & 0xFFFF;
    indx ^= dsthost;
    indx ^= (indx >> 8);
    indx &= 0xFF;

    for (cache = vinescache[indx]; cache; cache = cache->next_entry) {
	if (!(cache->flags & V_FC_VALID) ||
	    (cache->host != dsthost) ||
	    (cache->net != dstnet))
            continue;
        path = cache->next_path;
        output = path->idb;
        if (output == pak->if_input)
	    if (!vines_getidb(path->idb)->vines_split_disabled)
		return(FALSE);

        /*
         * Check if access list are enabled on this interface.
         */
        if (!(ushort)vines_getidb(output)->vines_accesslist) {
	  vines_fs_yes_doit:
            cache->next_path = path->next_path;
            vinesip->tc--;	/* decrement hop count */
            hwinput = pak->if_input->hwptr;
	    hwinput->counters.inputs_fast[ACCT_PROTO_VINES]++;
	    hwinput->counters.rx_cumbytes_fast[ACCT_PROTO_VINES] +=
		pak->datagramsize;
	    pak->acct_proto = ACCT_PROTO_VINES;
	    /*
	     * Save the actual length.  It can't be computed because of
	     * the damn SMDS trailer. Remove any media padding
             * (mostly ethernet), by clipping the packet from the tail. 
	     */
	    if (pak_has_particles(pak)) {
		int delta;
		particletype *mp;
		ushort size;

		size = GETSHORT(&vinesip->len) + pak->encsize;

		/*
		 * Adjust the various sizes in the packet. Note, 
		 * adjust the packet from the end
		 */
		delta = pak->datagramsize - size;
		mp = pak->particlequeue.qtail;
		if (mp->data_bytes <= delta) {
		    delta -= mp->data_bytes;
		    particle_unqueue_inline(pak, mp);
		    retparticle(mp);
		    mp = pak->particlequeue.qtail;
		}
		mp->data_bytes -= delta;
		pak->datagramsize = size;
	    }
		
	    pak->length = GETSHORT(&vinesip->len);
	    return((*vfs_fs_out[path->encaptype])(pak, path));
        } else {

            if (vines_access_lists[vines_getidb(output)->vines_accesslist])
                if (!(vines_access_check_les(pak,
		      vines_getidb(output)->vines_accesslist)))
                    return(FALSE);
            goto vines_fs_yes_doit;
        }
    }
    return(FALSE);
}

/**********************************************************************
 *
 *		   MISCELLANEOUS LOW END ROUTINES
 *
 **********************************************************************/

/*
 * vines_clear_fs_tables
 *
 * Vines has been disabled.  Clear out the FS table entries.
 */
void vines_clear_fs_tables (void)
{
    fs_table_fill((fastswitch_type)return_false, "return_false",
		  RXTYPE_VINES);
    fs_table_fill((fastswitch_type)return_false, "return_false",
		  RXTYPE_SNAP_VINES);
    fs_table_fill((fastswitch_type)return_false, "return_false",
		  RXTYPE_SAP_VINES);
}

/*
 * vines_setup_fs_tables
 *
 * Vines has been enabled.  Fill in the FS table entries.
 */
void vines_setup_fs_tables (void)
{
    reg_add_ether_fs(RXTYPE_VINES, (service_ether_fs_type)vines_les_fastswitch,
		     "vines_les_fastswitch");
    reg_add_hdlc_fs(RXTYPE_VINES, (service_hdlc_fs_type)vines_les_fastswitch,
		    "vines_les_fastswitch");
    reg_add_token_fs(RXTYPE_SAP_VINES, (service_token_fs_type)vines_les_fastswitch,
		     "vines_les_fastswitch");
    reg_add_framerelay_fs(RXTYPE_VINES,
			  (service_framerelay_fs_type)vines_les_fastswitch,
			  "vines_les_fastswitch");
    reg_add_smds_fs(RXTYPE_VINES, (service_smds_fs_type)vines_les_fastswitch,
		    "vines_les_fastswitch");
    reg_add_ppp_fs(RXTYPE_VINES, (service_ppp_fs_type)vines_les_fastswitch,
		    "vines_les_fastswitch");
    reg_add_atm_dxi_fs(RXTYPE_VINES,
		       (service_atm_dxi_fs_type)vines_les_fastswitch,
		       "vines_les_fastswitch");
    reg_add_ether_fs(RXTYPE_SNAP_VINES,
		     (service_ether_fs_type)vines_les_fastswitch,
		     "vines_les_fastswitch");
    reg_add_token_fs(RXTYPE_SNAP_VINES,
		     (service_token_fs_type)vines_les_fastswitch,
		     "vines_les_fastswitch");
    reg_add_fddi_fs(RXTYPE_SNAP_VINES, (service_fddi_fs_type)vines_les_fastswitch,
		    "vines_les_fastswitch");
    reg_add_framerelay_fs(RXTYPE_SNAP_VINES,
			  (service_framerelay_fs_type)vines_les_fastswitch,
			  "vines_les_fastswitch");
    reg_add_atm_dxi_fs(RXTYPE_SNAP_VINES,
		       (service_atm_dxi_fs_type)vines_les_fastswitch,
		       "vines_les_fastswitch");

    reg_add_aip_fs(RXTYPE_VINES,
		   (service_aip_fs_type)vines_les_fastswitch,
			"vines_les_fastswitch");

    reg_add_aip_fs(RXTYPE_SNAP_VINES,
		   (service_aip_fs_type)vines_les_fastswitch,
			"vines_les_fastswitch");

    reg_add_tunnel_fs(RXTYPE_VINES,
		      (service_tunnel_fs_type)vines_les_fastswitch,
		      "vines_les_fastswitch");

    vfs_fs_out[V_FS_ETHER_ARPA]  = vines_fs_out_ether_arpa;
    vfs_fs_out[V_FS_ETHER_SNAP]  = vines_fs_out_ether_snap;
    vfs_fs_out[V_FS_TOKEN_SNAP]  = vines_fs_out_token_snap;
    vfs_fs_out[V_FS_TOKEN_VINES] = vines_fs_out_token_vines;
    vfs_fs_out[V_FS_FDDI]        = vines_fs_out_fddi;
    vfs_fs_out[V_FS_HDLC]        = vines_fs_out_hdlc;
    vfs_fs_out[V_FS_FRAME]       = vines_fs_out_frame;
    vfs_fs_out[V_FS_FRAME_IETF]  = vines_fs_out_frame_ietf;
    vfs_fs_out[V_FS_SMDS_CISCO]  = vines_fs_out_smds;
    vfs_fs_out[V_FS_SMDS_DXI]    = vines_fs_out_smds;
    vfs_fs_out[V_FS_PPP]         = vines_fs_out_ppp;
    vfs_fs_out[V_FS_ATM_DXI]	 = vines_fs_out_atm_dxi;
    vfs_fs_out[V_FS_ATM]	 = vines_fs_out_atm;
    vfs_fs_out[V_FS_TUNNEL]	 = vines_fs_out_tunnel;
}

/*
 * vines_fast_supported
 *
 * Has code been written to support this specific low end interface?  If
 * the interface is a serial interface, then we have to also check the
 * link layer protocol.  Some are supported, some aren't.
 */
static boolean vines_fast_supported (int linktype, hwidbtype *hwidb)
{

    if (!hwidb || (linktype != LINK_VINES)) {
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
     * ATM: all flavors
     */
    if (is_atm(hwidb)) {
	return(TRUE);
    }

    /*
     * Tokenring:
     */
    if (hwidb->status & IDB_TR) {
	return(TRUE);
    }

    if (hwidb->status & IDB_SERIAL) {
	if (!is_ddr(hwidb) &&
	    ((hwidb->enctype == ET_HDLC) ||
	     is_ppp(hwidb) ||
	     is_frame_relay(hwidb) ||
	     is_smds(hwidb) ||
	     is_atm_dxi(hwidb))) {
	    return(TRUE);
	}
    }

    /* Do LEX - XXX */

    return(FALSE);
}

/*
 * vines_les_fs_register_drivers
 *
 * Fast switching has been enabled or disabled on an interface.
 * Set/clear the fast switching registries appropriately.
 */
static void vines_les_fs_register_drivers (ulong flags, hwidbtype* idb)
{
    /*
     * Check to see if we're configuring Vines
     */
    if (!(flags & FSF_VINES))
	return;

    if (idb->fast_switch_flag & FSF_VINES)
	vines_setup_fs_tables();
    else 
	vines_clear_fs_tables();
}

/*
 * vines_les_fs_init
 *
 * Hook the VINES fast switching code into the rest of the router.
 */
static void vines_les_fs_init (subsystype* subsys)
{
    reg_add_register_drivers(vines_les_fs_register_drivers,
			     "vines_les_fs_register_drivers");
    reg_add_fast_supported(vines_fast_supported,
			   "vines_fast_supported");
}

/*
 * LES FS subsystem header
 */

#define VINESLESFS_MAJVERSION 1
#define VINESLESFS_MINVERSION 0
#define VINESLESFS_EDITVERSION  1

SUBSYS_HEADER(vines_les_fs, VINESLESFS_MAJVERSION, VINESLESFS_MINVERSION, VINESLESFS_EDITVERSION,
	      vines_les_fs_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: vines",
	      "req: fastswitch, vines");

