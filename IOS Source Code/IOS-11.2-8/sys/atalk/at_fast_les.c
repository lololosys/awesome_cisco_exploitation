/* $Id: at_fast_les.c,v 3.7.20.16 1996/08/26 20:34:51 kleung Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_fast_les.c,v $
 *------------------------------------------------------------------
 * Appletalk fastswitching support for the low-end platforms
 *
 * December 1992, Scott Mackie
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: at_fast_les.c,v $
 * Revision 3.7.20.16  1996/08/26  20:34:51  kleung
 * CSCdi66720:  AppleTalk route-cache disabled for subinterfaces.
 * Branch: California_branch
 *
 * Revision 3.7.20.15  1996/08/15  23:45:04  wilber
 * CSCdi66123:  BVI code continues to use a packet after disposal
 * (datagram_done)
 * Fix the missing/misused returns
 * Branch: California_branch
 *
 * Revision 3.7.20.14  1996/08/02  18:20:08  kleung
 * CSCdi64905:  spurious access in 11.2(0.19) on boot
 * Branch: California_branch
 *
 * Revision 3.7.20.13  1996/07/10  22:18:07  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.7.20.12  1996/07/09  21:17:41  amcrae
 * CSCdi59120:  H2TPA SW: not routed apple-talk in smds when fast
 * switching.
 * Branch: California_branch
 * When sending SMDS encap, trim any trailer off the packet.
 *
 * Revision 3.7.20.11  1996/05/19  15:13:12  wfried
 * CSCdi56148:  Need to update Atalk hop-count in correct particle
 * Branch: California_branch
 * Hop-count byte does not reside in new prepended particle. Needs to be
 * updated in original particle.
 *
 * Revision 3.7.20.10  1996/05/10  19:01:30  gstovall
 * CSCdi56930:  Appletalk Pings 50% (Small Packet Failures) over SMDS (FS)
 * Branch: California_branch
 * Trim the particle to reflect the actual size of the data.
 *
 * Revision 3.7.20.9  1996/05/04  00:33:52  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Atalk protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on IRB/CRB is enabled or not.
 *
 * Revision 3.7.20.8  1996/05/02  08:39:34  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.7.20.7  1996/04/19  22:12:04  dwong
 * CSCdi54733:  Appletalk doesnt fastswitch from an ATM interface
 * Branch: California_branch
 *
 * Revision 3.7.20.6  1996/04/17  13:35:13  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.7.20.5  1996/04/04  22:13:12  gstovall
 * CSCdi51211:  Appletalk fastswitching over serial with smds encaps
 * broken.
 * Branch: California_branch
 * Remember to deal with that stupid SMDS trailer.
 *
 * Revision 3.7.20.4  1996/03/30  02:17:38  slin
 * CSCdi53094:  Appletalk does not FastSwitch on the RSP
 * Branch: California_branch
 *
 * Revision 3.7.20.3  1996/03/23  01:26:18  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.7.20.2  1996/03/21  22:28:33  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.7.20.1  1996/03/18  18:52:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.5  1996/03/19  01:36:28  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.4.2.4  1996/03/09  22:14:23  fsunaval
 * CSCdi50760:  Particle-based Appletalk fast-switching broken on Fddi.
 * Branch: ELC_branch
 *
 * Revision 3.4.2.3  1996/02/14  02:30:21  wfried
 * CSCdi48945:  Implement particle-based Appletalk fastswitching
 * Branch: ELC_branch
 *
 * Revision 3.4.2.2  1996/02/08  06:45:04  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.4.2.1  1995/12/11  18:28:15  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.7  1996/02/05  06:35:15  dwong
 * CSCdi45581:  Appletalk: Wrong 802.3 length computed for small packets
 *              Incorporate code review comments.
 *
 * Revision 3.6  1996/02/03  21:21:22  dwong
 * CSCdi45581:  Appletalk: Wrong 802.3 length computed for small packets
 * Compute 802.3 length using DDP length instead of "datagramsize"
 *
 * Revision 3.5  1995/11/20  21:55:27  kleung
 * CSCdi43578:  Hop count not incremented on AppleTalk ATM fastswitch.
 *
 * Revision 3.4  1995/11/20  02:16:31  shj
 * CSCdi43575:  Appletalk fastswitching to LANE on 4500 screws up packet
 * length
 * Put correct "ethernet" length in type_or_len.
 *
 * Revision 3.3  1995/11/17  08:42:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:05:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:56:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  20:49:48  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/10/11  18:52:25  mohsen
 * CSCdi41315:  LS100 Switch crashed while testing Appletalk phase II on
 * 4500 ATM
 * Set the outgoing SVC when fast switching Appletalk packets. Packets
 * sent on a wrong VC result in a corruptred AAL5 PDU trailer length,
 * which causes the switch to crash.
 *
 * Revision 2.6  1995/08/21  21:32:29  wmay
 * CSCdi38861:  atalk switching failures with smds encaps. fails to ping
 * Fix rsp - 4500 versions.  Simplify code a little.
 *
 * Revision 2.5  1995/07/05  22:24:00  kleung
 * CSCdi35438:  AURP update event did not send through tunnel properly
 *
 * Revision 2.4  1995/06/22  22:46:15  wmay
 * CSCdi36246:  Appletalk doesnt fast-switch smds
 *
 * Revision 2.3  1995/06/21  02:50:40  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.2  1995/06/14  18:40:11  slin
 * CSCdi35153:  AC byte mis-set when f/s atalk onto Token Ring
 *
 * Revision 2.1  1995/06/07  20:08:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef GLOBAL_DEBUG
#ifdef AT_FAST_DEBUG
#define FAST_DEBUG		/* Provides msgs as things fast switch */
#define FAST1_DEBUG		/* Provides detailed analysis of fields */
#endif
#endif

#include "master.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "address.h"
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers_inline.h"
#include "ieee.h"
#include "../if/atm.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "../lane/lane.h"
#include "../wan/smds.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../atalk/atalk_private.h"
#include "../atalk/at_fast.h"
#include "../atalk/at_fast_les.h"
#include "at_domain.h"
#include "at_registry.h"

#ifdef FAST1_DEBUG
#include "ttysrv.h"
#endif

/*
 * These are offsets to get to the beginning of the AppleTalk DDP
 * frame from the beginning of the received datagram.
 */
#define AT_ISO2_OFFSET   ETHER_SNAP_ENCAPBYTES
#define AT_ETALK_OFFSET  ETALK_ENCAPBYTES
#define AT_FDDI_OFFSET	 FDDI_SNAP_ENCAPBYTES
#define AT_TRING_OFFSET	 (TRING_SNAP_ENCAPBYTES + (input->rxrifwordlen << 1))

/*
 * Traffic counters (in atalk_stat) maintained by this module are:
 * 
 * ATALK_INPUT
 *      This variable is incremented once for every TRUE return from the 
 *      fast switching packet handlers.  This represents the total packets 
 *      input for AppleTalk.
 * ATALK_HOPCOUNT
 *	This variable is incremented once for each packet discarded 
 *      due to excessive number of hops.  It is not incremented if 
 *      atalkpkt_debug is set since the packet is passed to process
 *      level (FALSE return).
 *
 * The following counters are maintained exclusively, sans 
 * initialization, by this module:
 *
 * ATALK_FASTFORWARD
 *	This is the total number of packets which were fast switched 
 *      within the controller. In the case of CBUS, this includes all
 *      packets switched within the CBUS.
 * ATALK_XFASTFORWARD
 *	This is the number of packets which were fast switched 
 *      across controllers.
 */

inline void
at_write_ether_header (ulong       *ptr,
		       atcachetype *cptr, 
		       ushort       len,
		       uchar        hb)
{
    ushort *sptr;

    PUTLONG(ptr++, cptr->m.maclong[0]);	  /* dd, dd, dd, dd */
    PUTLONG(ptr++, cptr->m.maclong[1]);	  /* dd, dd, ss, ss */
    PUTLONG(ptr++, cptr->m.maclong[2]);	  /* ss, ss, ss, ss */
    sptr = (ushort *)ptr;
    PUTSHORT(sptr++, (len - ETHER_ARPA_ENCAPBYTES)); /* ll, ll */
    ptr = (ulong *)sptr;
    PUTLONG(ptr++, CONST_AA_AA_03_08);	  /* aa, aa, 03, 08 */
    PUTLONG(ptr++, CONST_00_07_80_9B); 	  /* 00, 07, 80, 9b */
    *(uchar *)ptr = hb;                   /* hb */
}

inline void 
at_write_etalk_header (ulong       *ptr,
		       atcachetype *cptr,
		       uchar       hb)
{
    PUTLONG(ptr++, cptr->m.maclong[0]);	/* dd, dd, dd, dd */
    PUTLONG(ptr++, cptr->m.maclong[1]);	/* dd, dd, ss, ss */
    PUTLONG(ptr++, cptr->m.maclong[2]);	/* ss, ss, ss, ss */
    PUTLONG(ptr++, cptr->m.maclong[3]);	/* tt, tt, dn, sn */
    PUTSHORT(ptr, (cptr->m.macstring[ETALKLAPTYPE] << 8) | hb); /* pt, hp */
}

inline void 
at_write_fddi_header (ulong *ptr, atcachetype *cptr, uchar hb)
{
    uchar *bptr = (uchar *)ptr;

    *bptr++ = FDDI_48BIT_ADDRESS | FDDI_LLC_FRAME;   /* fc */
    ptr     = (ulong *)bptr;                    /* Increment by a byte */
    PUTLONG(ptr++, cptr->m.maclong[0]);		/* dd, dd, dd, dd */
    PUTLONG(ptr++, cptr->m.maclong[1]);		/* dd, dd, ss, ss */
    PUTLONG(ptr++, cptr->m.maclong[2]);		/* ss, ss, ss, ss */
    PUTLONG(ptr++, CONST_AA_AA_03_08);		/* aa, aa, 03, 08 */
    PUTLONG(ptr++, CONST_00_07_80_9B); 		/* 00, 07, 80, 9b */
    *(uchar *)ptr = hb;                         /* hb */
}

static inline void
at_write_fddi_rif_header (ulong *ptr, atcachetype *cptr, uchar hb)
{
    ulong *cache_macptr;
    ulong *macptr;
    int lwords;

    /*
     * Perform the rewrite
     */
    *(uchar *)(ptr) = FDDI_LLC_FC_BYTE;
    macptr = (ulong *)((uchar *)ptr + 1);
    cache_macptr = cptr->m.maclong;

    /*
     * Copy the cache entry, including RIF, to the packet
     */
    for (lwords = 0; lwords < cptr->byte_len>>2; lwords++) {
        PUTLONG(macptr++, *cache_macptr++);
    }

    /*
     * Check for one remaining short in the cache
     */
    if (cptr->byte_len & 0x02) {
	PUTSHORT(macptr, *(ushort *)cache_macptr);
        macptr = (ulong *) ((uchar *)macptr + 2);
    }

    /*
     * Write SNAP header & hop byte
     */
    PUTLONG(macptr++, CONST_AA_AA_03_08);   /* aa, aa, 03, 08 */
    PUTLONG(macptr++, CONST_00_07_80_9B);   /* 00, 07, 80, 9b */
    *(uchar *)macptr = hb;                  /* hb */
}

static inline void 
at_write_token_header (ulong *ptr, atcachetype *cptr, uchar hb)
{
    uchar 	*bptr = (uchar *)ptr;
    ushort	rifoffset;

    *bptr++ = AC_PRI0;			/* write the AC/FC bytes */
    *bptr++ = FC_TYPE_LLC;
    PUTLONG(bptr, cptr->m.maclong[0]);	/* dd, dd, dd, dd */
    bptr += sizeof(long);
    PUTLONG(bptr, cptr->m.maclong[1]);	/* dd, dd, ss, ss */
    bptr += sizeof(long);
    PUTLONG(bptr, cptr->m.maclong[2]); /* ss, ss, ss, ss */
    bptr += sizeof(long);

    if (cptr->w_riflen > 0) {
	for (rifoffset = 6; rifoffset < 6 + cptr->w_riflen; ++rifoffset) {
	    PUTSHORT(bptr, cptr->m.macshort[rifoffset]);
	    bptr += sizeof(short);
	}
    }
    PUTLONG(bptr, CONST_AA_AA_03_08);		/* aa, aa, 03, 08 */
    bptr += sizeof(long);
    PUTLONG(bptr, CONST_00_07_80_9B); 		/* 00, 07, 80, 9b */
    bptr += sizeof(long);
    *bptr = hb;			/* hb */
}

inline void
at_write_serial_header (ulong *ptr, atcachetype *cptr, uchar hb)
{
    ushort ix;
    ulong *cache_macptr;
    uchar *bptr;

    cache_macptr = (ulong *)&cptr->m.macstring[0];
    
    /*
     * Note that all serial encaps are even length
     */
    for (ix = cptr->byte_len; ix >= sizeof(ulong); ix -= sizeof(ulong)) {
	PUTLONG(ptr++,*cache_macptr++);
    }
    bptr = (uchar *)ptr;
    if (ix > 0) {
	PUTSHORT(bptr, *(ushort *)cache_macptr);
	bptr += sizeof(ushort);
    }
    *(uchar *)bptr = hb;			/* hb */
}

static inline void
lex_at_write_ether_header (ulong       *ptr,
			   atcachetype *cptr, 
			   ushort       len,
			   uchar        hb)
{
    ulong *cache_macptr = cptr->m.maclong;

    /*
     * Write Lex header and IEEE 802.3 header.
     */
    PUTLONG(ptr++, *cache_macptr++);
    PUTLONG(ptr++, *cache_macptr++);
    PUTLONG(ptr++, *cache_macptr++);
    PUTLONG(ptr++, *cache_macptr++);
    PUTLONG(ptr++, ((*cache_macptr++ & 0xFFFF0000)
		    | (len - ETHER_ARPA_ENCAPBYTES)));
    PUTLONG(ptr++, CONST_AA_AA_03_08);
    PUTLONG(ptr++, CONST_00_07_80_9B);
    *(uchar *)ptr = hb;
}

static inline void
lex_at_write_etalk_header (ulong       *ptr,
			   atcachetype *cptr,
			   uchar       hb)
{
    ulong *cache_macptr = cptr->m.maclong;

    /*
     * Write Lex and Etalk Headers.
     */
    PUTLONG(ptr++, *cache_macptr++);
    PUTLONG(ptr++, *cache_macptr++);
    PUTLONG(ptr++, *cache_macptr++);
    PUTLONG(ptr++, *cache_macptr++);
    PUTLONG(ptr++, *cache_macptr++);
    PUTLONG(ptr++, ((*cache_macptr++ & 0xFFFF0000) |
		    (cptr->m.macstring[cptr->byte_len-1] << 8) | hb));
}

static inline void
write_remapped_header (ushort *ptr, ushort dst, ushort src)
{
    PUTSHORT(ptr++, 0);
    PUTSHORT(ptr++, dst);
    PUTSHORT(ptr++, src);
}


/*
 * fs_at_ether_rewrite:
 * Perform Ethernet encaps rewrite for Appletalk
 */
static inline boolean
fs_at_ether_rewrite (paktype *pak,
		     hwidbtype *output,
		     atcachetype *cptr,
		     uchar *datagramstart,
		     ddplong *ddp,
		     ushort datagramsize,
		     ushort remap_dst,
		     ushort remap_src,
		     shortbyte hopword)
{
    /* The input device always guarantees that there will be room
     * to   re-write   either   an   ETALK_ENCAPBYTES    or    an
     * ETHER_SNAP_ENCAPBYTES encapsulation even for the  case  of
     * scattered packets in which case we do not need to  prepend
     * a new particle for the encaps.
     */
    if (cptr->phaseOne) {
        /*
         * Switch to ATp1 ethernet
         */
	if (pak_has_particles(pak)) {
	    particle_clip_inline(pak->particlequeue.qhead,
				 ETALK_ENCAPBYTES - pak->encsize);
	}
        datagramsize  = datagramsize + ETALK_ENCAPBYTES;
        datagramstart = pak->network_start - ETALK_ENCAPBYTES;
 
        at_write_etalk_header((ulong *)datagramstart,
                              cptr,
                              hopword.d.byte[0]);
    } else {
        /*
         * Switch to ATp2 ethernet
         */
	if (pak_has_particles(pak)) {
	    particle_clip_inline(pak->particlequeue.qhead,
				 ETHER_SNAP_ENCAPBYTES - pak->encsize);
	}
        datagramsize = datagramsize + ETHER_SNAP_ENCAPBYTES;
        datagramstart = pak->network_start - ETHER_SNAP_ENCAPBYTES;

        at_write_ether_header((ulong *)datagramstart,
                              cptr,
                              datagramsize,
                              hopword.d.byte[0]);
        if (atalk_DomainRouter_running)
            write_remapped_header((ushort *) ddp->cksum, remap_dst,
                                  remap_src);
    }

    if (datagramsize < MINETHERSIZE)
        datagramsize = MINETHERSIZE;
    pak->datagramsize  = datagramsize;
    pak->datagramstart = datagramstart;
    pak->if_output = cptr->idb;
    (*output->fastsend)(output, pak);
    return (TRUE);
}


/*
 * fs_at_fddi_rewrite:
 * Perform FDDI encaps rewrite for Appletalk
 */
static inline boolean
fs_at_fddi_rewrite (paktype *pak,
		    hwidbtype *output,
		    atcachetype *cptr,
		    uchar *datagramstart,
		    ddplong *ddp,
		    ushort datagramsize,
		    ushort remap_dst,
		    ushort remap_src,
		    shortbyte hopword)
{
    if (pak_has_particles(pak)) {
	particletype *orig_p, *rw_p;
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
	delta = (1 + cptr->byte_len + SNAP_HDRBYTES) - pak->encsize; 
	if (delta < PARTICLE_ENCAPBYTES) {
 
	    /*
	     * The rewrite can fit over the incoming encaps
	     */
	    particle_clip_inline(pak->particlequeue.qhead, delta);
	    datagramsize  += delta;
	    datagramstart -= delta;

	    /*
	     * Perform the rewrite
	     */
	    at_write_fddi_rif_header((ulong *)datagramstart,
	                             cptr,
	                             hopword.d.byte[0]);
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
	     * Save pointer to original particle
	     */
	    orig_p = pak->particlequeue.qhead;
 
	    /*
	     * Fixup packet pointers. Strip off the encapsulation
	     * from the first particle, setup the new encaps particle
	     * and append it onto the packet.
	     */
	    particle_clip_inline(orig_p, -pak->encsize);
	    rw_p->data_bytes = (1 + cptr->byte_len + SNAP_HDRBYTES);
	    datagramsize += delta;
	    datagramstart = rw_p->data_start;
	    requeue_inline(&pak->particlequeue, rw_p);
 
	    /*
	     * Perform the rewrite.
	     * The hop byte needs to be updated in the original particle.
	     */
	    at_write_fddi_rif_header((ulong *)datagramstart,
	                             cptr,
	                             hopword.d.byte[0]);
	    *orig_p->data_start = hopword.d.byte[0];
	}
    } else {
	/*
	 * Contiguous packet (SRB over FDDI not supported on LES platf)
	 */
	datagramsize  = datagramsize + FDDI_SNAP_ENCAPBYTES;
	datagramstart = pak->network_start - FDDI_SNAP_ENCAPBYTES;
 
	at_write_fddi_header((ulong *)datagramstart,
	                     cptr,
	                     hopword.d.byte[0]);
    }

    if (atalk_DomainRouter_running)
	write_remapped_header((ushort *) ddp->cksum, remap_dst, remap_src);

    pak->datagramsize  = datagramsize;
    pak->datagramstart = datagramstart;
    pak->if_output = cptr->idb;
    (*output->fastsend)(output, pak);
    return(TRUE);
}


/*
 * fs_at_serial_rewrite:
 * Perform Serial encaps rewrite for Appletalk
 */
static inline boolean
fs_at_serial_rewrite (paktype *pak,
		      hwidbtype *output,
		      atcachetype *cptr,
		      uchar *datagramstart,
		      ddplong *ddp,
		      ushort datagramsize,
		      ushort remap_dst,
		      ushort remap_src,
		      shortbyte hopword)
{
    /*
     * For serial, we are always guaranteed that the rewrite will
     * fit over the incoming encapsulation even for scattered packets,
     * so there is no need to allocate a new particle.
     */
    if (pak_has_particles(pak)) {
	particle_clip_inline(pak->particlequeue.qhead,
			     cptr->byte_len - pak->encsize);
    }
    datagramsize  = datagramsize + cptr->byte_len;
    datagramstart = pak->network_start - cptr->byte_len;
 
    at_write_serial_header((ulong *)datagramstart,
                           cptr,
                           hopword.d.byte[0]);
 
    if (atalk_DomainRouter_running)
        write_remapped_header((ushort *) ddp->cksum, remap_dst,
                              remap_src);
    pak->datagramsize  = datagramsize;
    pak->datagramstart = datagramstart;
    pak->if_output = cptr->idb;
    (*output->fastsend)(output, pak);
    return (TRUE);
}


/*
 * fs_at_fr_rewrite:
 * Perform Frame Relay encaps rewrite for Appletalk
 */
static inline boolean
fs_at_fr_rewrite (paktype *pak,
		  hwidbtype *output,
		  atcachetype *cptr,
		  uchar *datagramstart,
		  ddplong *ddp,
		  ushort datagramsize,
		  ushort remap_dst,
		  ushort remap_src,
		  shortbyte hopword)
{
    fr_pvc_info_type *fr_pvc;

    /*
     * For frame relay, we are always guaranteed that the rewrite will
     * fit over the incoming encapsulation even for scattered packets,
     * so there is no need to allocate a new particle.
     */
    if (pak_has_particles(pak)) {
        particle_clip_inline(pak->particlequeue.qhead,
                             cptr->byte_len - pak->encsize);
    }

    datagramsize  = datagramsize + cptr->byte_len;
    datagramstart = pak->network_start - cptr->byte_len;
 
    at_write_serial_header((ulong *)datagramstart,
                           cptr,
                           hopword.d.byte[0]);
 
    if (atalk_DomainRouter_running)
        write_remapped_header((ushort *) ddp->cksum, remap_dst,
                              remap_src);
    pak->datagramsize  = datagramsize;
    pak->datagramstart = datagramstart;
    pak->if_output = cptr->idb;
    (*output->fastsend)(output, pak);
    fr_pvc = HWIDB_TO_FR_PVC(output, cptr->subint_reference_num);
    INC_PVC_FAST_OUTPUT(fr_pvc, datagramsize);
    return (TRUE);
}


/*
 * fs_at_atmdxi_rewrite:
 * Perform ATM DXI encaps rewrite for Appletalk
 */
static inline boolean
fs_at_atmdxi_rewrite (paktype *pak,
		      hwidbtype *output,
		      atcachetype *cptr,
		      uchar *datagramstart,
		      ddplong *ddp,
		      ushort datagramsize,
		      ushort remap_dst,
		      ushort remap_src,
		      shortbyte hopword)
{
    /*
     * For ATM DXI, we are always guaranteed that the rewrite will
     * fit over the incoming encapsulation even for scattered packets,
     * so there is no need to allocate a new particle.
     */
    if (pak_has_particles(pak)) {
        particle_clip_inline(pak->particlequeue.qhead,
                             cptr->byte_len - pak->encsize);
    }

    datagramsize  = datagramsize + cptr->byte_len;
    datagramstart = pak->network_start - cptr->byte_len;

    at_write_serial_header((ulong *)datagramstart,
			   cptr,
			   hopword.d.byte[0]);

    if (atalk_DomainRouter_running)
	write_remapped_header((ushort *) ddp->cksum, remap_dst,
			      remap_src);
    pak->datagramsize  = datagramsize;
    pak->datagramstart = datagramstart;
    pak->if_output = cptr->idb;
    (*output->fastsend)(output, pak);
    INC_DXI_PVC_FAST_OUTPUT(HWIDB_TO_DXI_PVC(output,
					     cptr->subint_reference_num),
			    datagramsize);
    return (TRUE);
}


/*
 * fs_at_smds_rewrite:
 * Perform SMDS encaps rewrite for Appletalk
 */
static inline boolean
fs_at_smds_rewrite (paktype *pak,
		    hwidbtype *output,
		    atcachetype *cptr,
		    uchar *datagramstart,
		    ddplong *ddp,
		    ushort datagramsize,
		    ushort remap_dst,
		    ushort remap_src,
		    shortbyte hopword)
{
    smdstype	*sm;
    uint	modelen, pad;
    uchar	*trailptr = NULL;

    if (pak_has_particles(pak)) {

	/*
	 * Scattered packet
	 */
	particletype *rw_p, *tail_p;
	int	delta;

	/*
	 * Always allocate a new particle
	 */
	rw_p = allocate_fs_particle();
	if (!rw_p) {
	    return(FALSE);
	}
 
	/*
	 * Fixup packet pointers. Strip off the encapsulation
	 * from the first particle.
	 */
	particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);

	/*
	 * The hop byte needs to be updated in the original first particle.
	 */
        *((particletype *)pak->particlequeue.qhead)->data_start =
	    hopword.d.byte[0];

	/*
	 * Hack off any media padding (mostly ethernet).  This works because
	 * tiny packets (less than min ether size [60 bytes]) are in one
	 * particle.  If this ever changes, this code will break.  To do
	 * later, find the inventor of SMDS and shove a rabid weasel down
	 * their pants.
	 * This needs to be extended to clip the packet at
	 * datagramsize. Incoming SMDS encaps have a trailer which
	 * some (all?) drivers do not trim.  The game is to calculate
	 * the difference and then trim from the end, regardless of the
	 * number of particles.
	 */
	delta = pak->datagramsize - pak->encsize - datagramsize;
	while (delta > 0) {
	    tail_p = pak->particlequeue.qtail;
	    if (tail_p->data_bytes <= delta) {
		delta -= tail_p->data_bytes;
		unqueue_inline(&pak->particlequeue, tail_p);
		retparticle(tail_p);
	    } else {
		tail_p->data_bytes -= delta;
		break;
	    }
	} 

	/*
	 * Prepend new particle
	 */
	rw_p->data_bytes = cptr->byte_len;
	pak->datagramstart = rw_p->data_start;
	requeue_inline(&pak->particlequeue, rw_p);

    } else {

	/*
	 * Contiguous packet
	 */ 
	pak->datagramstart = pak->network_start - cptr->byte_len;
	trailptr = pak->network_start + datagramsize;
    }

    /*
     * Copy the header from the cache, then write the
     * miscellaneous smds stuff directly into the pak
     */
    modelen = output->smds_pdb->fast_output_modelen;
    sm = (smdstype *)(pak->datagramstart + modelen);
    at_write_serial_header((ulong *)pak->datagramstart, cptr,
			   hopword.d.byte[0]);
    if (atalk_DomainRouter_running)
	write_remapped_header((ushort *) ddp->cksum, remap_dst,
			      remap_src);

    pad = 3 - ((datagramsize + 3) % 4);
    datagramsize += cptr->byte_len;
    sm->smds_xhlpi &= ~3;
    sm->smds_xhlpi |= pad;
    sm->smds_betag = output->smds_begin_tag++;

    /*
     * Now write the trailer stuff (0 the pad bytes, then
     * write a long at the end)  Basize is also written here,
     * because the calculation is easier.
     *
     * We are assuming that there is always room at the end of
     * a particle for up to three bytes of pad and four bytes
     * of trailer.
     */
    if (pak_has_particles(pak)) {
	particletype *tail_p;

	tail_p = pak->particlequeue.qtail;
	trailptr = (uchar *)(tail_p->data_start + tail_p->data_bytes);
	tail_p->data_bytes = tail_p->data_bytes + sizeof(ulong) + pad;
    }

    PUTLONG(trailptr, 0);
    
    datagramsize += pad;
    trailptr += pad;
    PUTSHORT(&sm->smds_basize,
	     (datagramsize - (sizeof(ulong) + modelen)));
    pak->datagramsize = datagramsize + sizeof(ulong);
    PUTLONG(trailptr, GETLONG(&sm->smds_rsvd)); /* Trailer copy */
    pak->if_output = cptr->idb;
    (*output->fastsend)(output, pak);
    return(TRUE);
}


/*
 * fs_at_atm_channel_rewrite:
 * Perform ATM or Channel encaps rewrite for Appletalk
 */
static inline boolean
fs_at_atm_channel_rewrite (paktype *pak,
			   hwidbtype *output,
			   atcachetype *cptr,
			   uchar *datagramstart,
			   ddplong *ddp,
			   ushort datagramsize,
			   ushort remap_dst,
			   ushort remap_src,
			   shortbyte hopword)
{
    /*
     * For ATM or Channel, we are always guaranteed that the rewrite will
     * fit over the incoming encapsulation even for scattered packets,
     * so there is no need to allocate a new particle.
     *
     * We may need to revisit this as more encaps are added to ATM, but
     * at this point (Ether type LANE only) there is no problem.
     */
    if (pak_has_particles(pak)) {
        particle_clip_inline(pak->particlequeue.qhead,
                             cptr->byte_len - pak->encsize);
    }
    pak->atm_vcid = cptr->m.macshort[0];
    pak->atm_enctype = cptr->m.macshort[1];
    datagramstart = pak->network_start - cptr->byte_len;
    bcopy(cptr->m.maclong, datagramstart, cptr->byte_len);
    /* Update the hop count */
    *pak->network_start = hopword.d.byte[0];
    if (lec_enabled(cptr->idb)) {
        ether_hdr   *ehead = (ether_hdr *)(datagramstart +
                                           ATM_AAL5_LANE_ENCAPBYTES);
        PUTSHORT(&ehead->type_or_len, datagramsize + SNAP_HDRBYTES);
    }
    if (atalk_DomainRouter_running)
	write_remapped_header((ushort *) ddp->cksum, remap_dst,
			      remap_src);
    pak->datagramsize  = datagramsize + cptr->byte_len;
    pak->datagramstart = datagramstart;
    pak->if_output = cptr->idb;
    (*output->fastsend)(output, pak);
    return (TRUE);
}


/*
 * fs_at_token_rewrite:
 * Perform Token Ring encaps rewrite for Appletalk
 */
static inline boolean
fs_at_token_rewrite (paktype *pak,
		     hwidbtype *output,
		     atcachetype *cptr,
		     uchar *datagramstart,
		     ddplong *ddp,
		     ushort datagramsize,
		     ushort remap_dst,
		     ushort remap_src,
		     shortbyte hopword)
{
    if (pak_has_particles(pak)) {
	particletype *orig_p,  *rw_p;
	int delta;

	/*
	 * For Token Ring, the re-write can sometimes fit over the original
	 * re-write and sometimes it will need a new particle due to
	 * a large RIF. We test for this, as overwriting the incoming
	 * encapsulation is more efficient.
	 */
	delta = (TRING_SNAP_ENCAPBYTES + cptr->b_riflen) - pak->encsize;
	if (delta > PARTICLE_ENCAPBYTES) {
 
	    /*
	     * The rewrite can fit over the incoming encaps
	     */
	    particle_clip_inline(pak->particlequeue.qhead, delta);
	    datagramsize  += (TRING_SNAP_ENCAPBYTES + cptr->b_riflen);
	    datagramstart -= delta;
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
	     * Save pointer to original particle
	     */
	    orig_p = pak->particlequeue.qhead;
 
	    /*
	     * Fixup packet pointers. Strip off the encapsulation
	     * from the first particle, setup the new encaps particle
	     * and append it onto the packet.
	     */
	    particle_clip_inline(orig_p, -pak->encsize);
	    rw_p->data_bytes = (TRING_SNAP_ENCAPBYTES + cptr->b_riflen);
	    datagramsize += (TRING_SNAP_ENCAPBYTES + cptr->b_riflen);
	    datagramstart = rw_p->data_start;
	    requeue_inline(&pak->particlequeue, rw_p);

	    /*
	     * The hop byte needs to be updated in the original particle.
	     */
	    *orig_p->data_start = hopword.d.byte[0];
	}
    } else {
	/*
	 * Contiguous packet
	 */
	datagramsize  = datagramsize + TRING_SNAP_ENCAPBYTES + cptr->b_riflen;
	datagramstart =
	    pak->network_start - (TRING_SNAP_ENCAPBYTES + cptr->b_riflen);
    }

    /* 
     * Perform the rewrite
     */
    at_write_token_header((ulong *)datagramstart,
			  cptr,
			  hopword.d.byte[0]);

    if (atalk_DomainRouter_running)
	write_remapped_header((ushort *) ddp->cksum, remap_dst,
			      remap_src);
    pak->datagramsize  = datagramsize;
    pak->datagramstart = datagramstart;
    pak->if_output = cptr->idb;
    (*output->fastsend)(output, pak);
    return (TRUE);
}


/*
 * fs_at_lex_rewrite:
 * Perform Lex encaps rewrite for Appletalk
 */
static inline boolean
fs_at_lex_rewrite (paktype *pak,
		   hwidbtype *output,
		   atcachetype *cptr,
		   uchar *datagramstart,
		   ddplong *ddp,
		   ushort datagramsize,
		   ushort remap_dst,
		   ushort remap_src,
		   shortbyte hopword)
{
    /*
     * For Lex, we are always guaranteed that the rewrite will
     * fit over the incoming encapsulation even for scattered packets,
     * so there is no need to allocate a new particle.
     */
    if (pak_has_particles(pak)) {
        particle_clip_inline(pak->particlequeue.qhead,
                             cptr->byte_len - pak->encsize);
    }

    if (cptr->phaseOne) {
	/*
	 * Switch to ATp1 Lex
	 */
	datagramsize  = datagramsize + cptr->byte_len;
	datagramstart = pak->network_start - cptr->byte_len;

	lex_at_write_etalk_header((ulong *)datagramstart,
				  cptr,
				  hopword.d.byte[0]);
    } else {
  	/*
  	 * Switch to ATp2 Lex
  	 */
	datagramsize  = datagramsize + cptr->byte_len;
	datagramstart = pak->network_start - cptr->byte_len;

	lex_at_write_ether_header((ulong *)datagramstart,
				  cptr,
				  datagramsize,
				  hopword.d.byte[0]);
	if (atalk_DomainRouter_running)
	    write_remapped_header((ushort *) ddp->cksum, remap_dst,
				  remap_src);
    }

    pak->datagramsize  = datagramsize;
    pak->datagramstart = datagramstart;
    pak->if_output = cptr->idb;
    (*output->fastsend)(output, pak);
    return (TRUE);
}


/*
 * fs_at_bvi_rewrite:
 * Perform BVI encaps rewrite for Appletalk
 */
static inline boolean
fs_at_bvi_rewrite (paktype *pak,
		   hwidbtype *output,
		   atcachetype *cptr,
		   uchar *datagramstart,
		   ddplong *ddp,
		   ushort datagramsize,
		   ushort remap_dst,
		   ushort remap_src,
		   shortbyte hopword)
{
    ushort ddplen;
 
    /* The input device always guarantees that there will be room
     * to   re-write   either   an   ETALK_ENCAPBYTES    or    an
     * ETHER_SNAP_ENCAPBYTES encapsulation even for the  case  of
     * scattered packets in which case we do not need to  prepend
     * a new particle for the encaps.
     */
    if (cptr->phaseOne) {
        /*
         * Switch to ATp1 ethernet
         */
        if (pak_has_particles(pak)) {
            particle_clip_inline(pak->particlequeue.qhead,
                                 ETALK_ENCAPBYTES - pak->encsize);
        }
        datagramsize  = datagramsize + ETALK_ENCAPBYTES;
        datagramstart = pak->network_start - ETALK_ENCAPBYTES;
	
        at_write_etalk_header((ulong *)datagramstart,
                              cptr,
                              hopword.d.byte[0]);
    } else {
	
        /*
         * Switch to ATp2 ethernet
         */
        if (pak_has_particles(pak)) {
            particle_clip_inline(pak->particlequeue.qhead,
                                 ETHER_SNAP_ENCAPBYTES - pak->encsize);
        }
        ddplen = GETSHORT(ddp->length) & 0x3ff;
        datagramsize = ddplen + ETHER_SNAP_ENCAPBYTES;
        datagramstart = pak->network_start - ETHER_SNAP_ENCAPBYTES;
	
        at_write_ether_header((ulong *)datagramstart,
                              cptr,
                              datagramsize,
                              hopword.d.byte[0]);
        if (atalk_DomainRouter_running)
            write_remapped_header((ushort *) ddp->cksum, remap_dst,
                                  remap_src);
    }
    
    if (datagramsize < MINETHERSIZE)
        datagramsize = MINETHERSIZE;
    pak->datagramsize  = datagramsize;
    pak->datagramstart = datagramstart;
    
    /*
     * Overwrite the input interface to the BVI.
     * Reason is for bridging code, it could then figure
     * out in which bridge group to bridge the packet;
     * Otherwise, the input interface is a routed interface,
     * no bridging attributes associated with it.
     * Clear the output interface to let bridging code sort
     * it out.
     */
    if (!change_if_input(pak, cptr->idb)) {
	datagram_done(pak);
	return (TRUE);
    }
    
    pak->if_output = NULL;
    
    /*
     * Set mac_start and addr_start since bridging code
     * assumes it is the same as datagramstart for ether
     * case
     */
    pak->mac_start = pak->addr_start = pak->datagramstart;
    
    if (cptr->phaseOne)
	pak->info_start = NULL;
    else
	pak->info_start = pak->datagramstart + 
	    ETHER_8023_ENCAPBYTES;
     
    /* Charge bvi interface with bridge packet output
     * per transition 5 of CSCdi92583
     */
    output->counters.outputs_fast[ACCT_PROTO_BRIDGE]++;
    output->counters.tx_cumbytes_fast[ACCT_PROTO_BRIDGE] += 
    pak->datagramsize; 
    return (reg_invoke_tbridge_forward(pak));
}


static boolean
at_fastswitch (paktype *pak)
{
    ddplong* ddp;
    atcachetype *cptr, *primary;
    hwidbtype* output;
    hwidbtype* input;
    uchar *datagramstart = pak->datagramstart;
    shortbyte hopword;
    ushort dnet, datagramsize;
    at_Domain *domain;
    ushort remap_dst = 0;
    ushort remap_src = 0;
    boolean incrementhop = TRUE;

    /*
     * Is this a Phase I frame with wacky short DDP addresses? If so,
     * let the process-switching level handle it.
     */
    pak->linktype = LINK_APPLETALK;
    if (pak->enctype == ET_ETHERTALK) {
	if (datagramstart[ETALKLAPTYPE] != ALAP_DDP_LONG)
	    return(FALSE);
    }
    
    ddp = (ddplong *)pak->network_start;

    hopword.d.sword = GETSHORT(ddp->length);
    dnet            = GETSHORT(ddp->dnet);
    input           = pak->if_input->hwptr;

#ifdef FAST1_DEBUG
    if (atalkpkt_debug) {
	ttyprintf(CONTTY,"\nAT(fast): %s: packet = %08x %08x %08x %08x", 
		  input->hw_namestring,
		  GETLONG(datagramstart + 12),
		  GETLONG(datagramstart + 16),
		  GETLONG(datagramstart + 20),
		  GETLONG(datagramstart + 24));
	ttyprintf(CONTTY,
		  "\nAT(fast): hops = %d net = %d, node = %d hopbyte = %02x", 
		  hopword.d.byte[0]>>2, dnet,
		  ddp->dnode, hopword.d.byte[0]);
    }
#endif

    if (atalk_NBPFilters && ddp->ddppt == DDP_TYPE_NBP)
        return (FALSE);
	
    /* 
     * If SMRP packet, let SMRP fast switching routine handle it.
     * Note that only long DDP format is processed.
     */
    if (atalk_SMRP_running && (ddp->ddppt == DDP_TYPE_SMRP)) {
	pak->transport_start = ((uchar*)ddp) + DDP_OVERHEADBYTES;

	/* Do something pkt accounting */
	input->counters.inputs_fast[ACCT_PROTO_APPLE]++;
	input->counters.rx_cumbytes_fast[ACCT_PROTO_APPLE] += pak->datagramsize;
	pak->acct_proto = ACCT_PROTO_APPLE;
	atalk_stat[ATALK_INPUT]++;
	atalk_stat[ATALK_FASTFORWARD]++;
	
	return (reg_invoke_atalk_smrp_fastswitch(pak));
    }
    
    
    if (atalk_DomainRouter_running) {
	at_Domain *domain_in;
	atalkidbtype *atalkidb;
	
	if (ddp->ddppt == DDP_TYPE_NBP)
	    return (FALSE);

	atalkidb = atalk_getidb(pak->if_input);
	if (!atalkidb)
	  return (FALSE);
	
	ATFAST_REMAPNETS(atalkidb, dnet, ddp->dnode, GETSHORT(ddp->snet));

	if (cptr->phaseOne)
	    return (FALSE);

	domain_in = atalkidb->atalk_dminfo;
	ATFAST_ADJUSTHOPCOUNT(domain_in, domain, hopword.d.byte[0]);

    } else {

	cptr = at_lookup_cache_ptr(dnet, ddp->dnode);
	if (cptr == NULL)
	    return (FALSE);

	if (cptr->delaycount) {
 	    /* 
 	     * First parallel path # of packets will be
 	     * handled at process switch level.
 	     */
            return (FALSE);
        }
        else {
            /*
             * Switch to the next parallel cache entry
 	     */
 	    primary = cptr;
 	    cptr = primary->next_equalcache;
 	    primary->next_equalcache = cptr->equalcache;
  	}
    }
    
    if (incrementhop)
	if ((hopword.d.byte[0] += 0x04) > (DDP_MAX_HOPCOUNT << 2)) {
	    if (at_debug(atalkpkt_debug, ATALK_NULL_MASK))
		return (FALSE);
	    atalk_stat[ATALK_INPUT]++;
	    atalk_stat[ATALK_HOPCOUNT]++;
	    input->atalk_inputerrs++;
	    datagram_done(pak);	/* pitch invalid packet */
	    return (TRUE);	/* and tell system we handled it */
	}
	
    output = cptr->hwif;
    datagramsize = hopword.d.sword & DDPLENGTHMASK;

#ifdef FAST_DEBUG
    if (atalkpkt_debug) {
	ttyprintf(CONTTY,"\nAT(fast): Switching from %s to %s (%x), size %d",
		  input->hw_namestring,cptr->idb->namestring,output,datagramsize);
    }
#endif
	
    input->counters.inputs_fast[ACCT_PROTO_APPLE]++;
    input->counters.rx_cumbytes_fast[ACCT_PROTO_APPLE] += pak->datagramsize;
    pak->acct_proto = ACCT_PROTO_APPLE;
    atalk_stat[ATALK_INPUT]++;
    atalk_stat[ATALK_FASTFORWARD]++;
	
    switch (output->fast_switch_type) {

      case FS_ETHER:
	/*
	 * Switch to ethernet
	 */
	return (fs_at_ether_rewrite(pak, output, cptr, datagramstart, ddp,
				    datagramsize, remap_dst, remap_src,
				    hopword));

      case FS_FDDI:
	/*
	 * Switch to FDDI
	 */
	return (fs_at_fddi_rewrite(pak, output, cptr, datagramstart, ddp,
				   datagramsize, remap_dst, remap_src,
				   hopword));

      case FS_HDLC:
      case FS_PPP:
	/*
	 * Switch to serial, HDLC and PPP
	 */
	return (fs_at_serial_rewrite(pak, output, cptr, datagramstart, ddp,
				     datagramsize, remap_dst, remap_src,
				     hopword));

      case FS_FR:
	/*
	 * Switch to serial, Frame Relay
	 */
	return (fs_at_fr_rewrite(pak, output, cptr, datagramstart, ddp,
				 datagramsize, remap_dst, remap_src,
				 hopword));

      case FS_ATM_DXI:
	/*
	 * Switch to serial, ATM_DXI
	 */
	return (fs_at_atmdxi_rewrite(pak, output, cptr, datagramstart, ddp,
				     datagramsize, remap_dst, remap_src,
				     hopword));

      case FS_TUNNEL:
	/*
	 * GRE fastswitching.
	 */
	pak->if_output = cptr->idb;
	(*output->fastsend)(output, pak);
  	return (TRUE);

      case FS_SMDS:
	/*
	 * Switch to SMDS. We're going to do this all in-line because
	 * it will make writing the trailing encapsulation easier.
	 * SMDS is proof positive that Darwinism no longer works
	 * because someone who was stupid enough to design a protocol
	 * with not only a trailing length field, but a trailing length
	 * field which is padded to a longword boundary (speed considerations,
	 * don't you know) wasn't killed during lunchtime in a pedestrian
	 * crosswalk.
	 */
	return (fs_at_smds_rewrite(pak, output, cptr, datagramstart, ddp,
				   datagramsize, remap_dst, remap_src,
				   hopword));

      case FS_CHANNEL:
      case FS_AIP:
	/*
	 * Switch to ATM or channel interface:
	 */
	return (fs_at_atm_channel_rewrite(pak, output, cptr, datagramstart, ddp,
					  datagramsize, remap_dst, remap_src,
					  hopword));

      case FS_TOKEN:
	/*
	 * Switch to Token Ring
	 */
	return (fs_at_token_rewrite(pak, output, cptr, datagramstart, ddp,
				    datagramsize, remap_dst, remap_src,
 				    hopword));

      case FS_LEX:
	/*
	 * Switch to Lex
	 */
	return (fs_at_lex_rewrite(pak, output, cptr, datagramstart, ddp,
				  datagramsize, remap_dst, remap_src,
				  hopword));
	
      case FS_BVI:

	/*
	 * Switch to BVI(Bridge-group Virtual Interface).
	 * We need to pass this packet to tbridge
	 * fast-switching code
	 */
	return (fs_at_bvi_rewrite(pak, output, cptr, datagramstart, ddp,
				  datagramsize, remap_dst, remap_src,
				  hopword));

      default:
	return (FALSE);
    }
}

/*
 * atfast_CntrlSetupIF
 * Setup the interface fields for low-end fast switching.  Here we perform
 * all necessary validation of the hardware but we do not generate any
 * messages.  Messages are generated only after explicit initialization.
 */
static int
atfast_CntrlSetupIF (hwidbtype* idb)
{
    idbtype *swidb;
    atalkidbtype *atalkidb = NULL;

    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
        atalkidb = atalk_getidb(swidb);
        if (atalkidb)
            break;
    }

    if (!atalkidb)
	return (fsNotSupported);

    /* Don't go any further if the user has disabled route caching */
    if (idb->atalk_routecache == fsUserDisabled) {
	return (fsUserDisabled);
    }
    idb->atalk_fastoddoutadj = 1;

    /*
     * Serial encapsulation:
     *
     * - Supports only Extended encapsulations.
     * - Currently only HDLC, PPP and FR. (also ATM-DXI)
     */
    if (idb->status & IDB_SERIAL) {
	if (atalkidb->atalk_enctype == ET_ETHERTALK ||
	    !atalkif_net_extended(atalkidb)) {
	    return (fsNotSupported);
	} else {
	    if (!(   (idb->enctype == ET_HDLC)
		  || (idb->enctype == ET_FRAME_RELAY)
		  || (idb->enctype == ET_PPP)
		  || (idb->enctype == ET_SMDS)
		  || (is_atm_dxi(idb))))
		return (fsIncompLinkProtocol);
	}
	idb->fast_switch_flag |= FSF_APPLE;
	return (fsNotRouting);
    }

    if (idb->status & (IDB_ETHER|IDB_LEX|IDB_BVI)) {
	if (atalkidb->atalk_enctype == ET_ETHERTALK) {
	    if (idb->enctype != ET_ARPA) 
		return (fsIncompLinkProtocol);
	    idb->fast_switch_flag |= FSF_APPLE;
	}
	if (atalkidb->atalk_enctype == ET_SNAP) {
	    idb->fast_switch_flag |= FSF_SNAP_APPLE;
	}
	return (fsNotRouting);
    } 

    if (idb->status & IDB_FDDI) {
	idb->fast_switch_flag |= FSF_APPLE;
	return (fsNotRouting);
    }

    if (idb->status & IDB_TR) {
	idb->fast_switch_flag |= FSF_APPLE;
	return (fsNotRouting);
    }

    /*
     * Info. for ATM FS setup.
     */
    if (is_atm(idb)) {
        idb->fast_switch_flag |= FSF_SNAP_APPLE;
	return (fsNotRouting);
    }

    if (reg_invoke_fast_supported(LINK_APPLETALK, idb)) {
	idb->fast_switch_flag |= FSF_APPLE;
  	return (fsNotRouting);
    }

    return (fsNotSupported);
}

static void atalk_les_fs_register_drivers (ulong flags, hwidbtype* hwidb)
{
    /*
     * Check to see if we're configuring Appletalk
     */
    if (!(flags & (FSF_APPLE | FSF_SNAP_APPLE)))
	return;

    hwidb->atalk_routecache = atfast_CntrlSetupIF(hwidb);
    if (hwidb->fast_switch_flag & (FSF_APPLE | FSF_SNAP_APPLE)) {
	fs_table_fill((fastswitch_type) return_false, "return_false",
		      RXTYPE_SNAP_APPLE);
	fs_table_fill((fastswitch_type) return_false, "return_false",
		      RXTYPE_APPLE);
	reg_add_hdlc_fs(RXTYPE_APPLE, (service_hdlc_fs_type) at_fastswitch,
			"at_fastswitch");
	reg_add_framerelay_fs(RXTYPE_APPLE,
			      (service_framerelay_fs_type) at_fastswitch,
			      "at_fastswitch");
	reg_add_framerelay_fs(RXTYPE_SNAP_APPLE,
			      (service_framerelay_fs_type) at_fastswitch,
			      "at_fastswitch");
	reg_add_ppp_fs(RXTYPE_APPLE, (service_ppp_fs_type) at_fastswitch,
		       "at_fastswitch");
	reg_add_ether_fs(RXTYPE_APPLE,
			 (service_ether_fs_type)at_fastswitch,
			 "at_fastswitch");

	reg_add_ether_fs(RXTYPE_SNAP_APPLE,
			 (service_ether_fs_type) at_fastswitch,
			 "at_fastswitch");
	reg_add_fddi_fs(RXTYPE_APPLE,
			(service_fddi_fs_type) at_fastswitch,
			"at_fastswitch");
	reg_add_token_fs(RXTYPE_SNAP_APPLE,
			 (service_token_fs_type) at_fastswitch,
			 "at_fastswitch");
	reg_add_smds_fs(RXTYPE_APPLE,
			(service_smds_fs_type) at_fastswitch,
			"at_fastswitch");
	reg_add_atm_dxi_fs(RXTYPE_APPLE,
			   (service_atm_dxi_fs_type)at_fastswitch,
			   "at_fastswitch");

	reg_add_atm_dxi_fs(RXTYPE_SNAP_APPLE,
			   (service_atm_dxi_fs_type)at_fastswitch,
			   "at_fastswitch");
	reg_add_aip_fs(RXTYPE_APPLE,
		       (service_aip_fs_type) at_fastswitch,
		       "at_fastswitch");
	reg_add_aip_fs(RXTYPE_SNAP_APPLE,
		       (service_aip_fs_type) at_fastswitch,
		       "at_fastswitch");
	reg_add_channel_fs(RXTYPE_APPLE,
			   (service_channel_fs_type) at_fastswitch,
			   "at_fastswitch");
	
	reg_add_tunnel_fs(RXTYPE_APPLE,
			  (service_tunnel_fs_type)at_fastswitch,
			  "at_fastswitch");
	reg_add_lex_fs(RXTYPE_APPLE,
		       (service_lex_fs_type) at_fastswitch,
		       "at_fastswitch");
	reg_add_lex_fs(RXTYPE_SNAP_APPLE,
		       (service_lex_fs_type) at_fastswitch,
		       "at_fastswitch");
    }
}

static void atalk_les_fs_init (subsystype* subsys)
{
    reg_add_register_drivers(atalk_les_fs_register_drivers,
			     "atalk_les_fs_register_drivers");
}

/*
 * Appletalk LES FS subsystem header
 */

#define ATLESFS_MAJVERSION 1
#define ATLESFS_MINVERSION 0
#define ATLESFS_EDITVERSION  1

SUBSYS_HEADER(atalk_les_fs, ATLESFS_MAJVERSION, ATLESFS_MINVERSION, ATLESFS_EDITVERSION,
	      atalk_les_fs_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: atalk",
	      "req: fastswitch, atalk");
