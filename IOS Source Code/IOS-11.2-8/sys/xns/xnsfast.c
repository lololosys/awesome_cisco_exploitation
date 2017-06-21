/* $Id: xnsfast.c,v 3.5.20.9 1996/09/11 20:09:44 snyder Exp $
 * $Source: /release/112/cvs/Xsys/xns/xnsfast.c,v $
 *------------------------------------------------------------------
 * xnsfast.c -- Device independent XNS and Novell Fast Switching
 *
 * 29-April-1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xnsfast.c,v $
 * Revision 3.5.20.9  1996/09/11  20:09:44  snyder
 * CSCdi68837:  more things const
 *              156 out of data, 64 image
 * Branch: California_branch
 *
 * Revision 3.5.20.8  1996/08/28  00:52:56  hampton
 * Migrate Desktop Protocols away from the one minute and one second
 * registries.  [CSCdi67383]
 * Branch: California_branch
 *
 * Revision 3.5.20.7  1996/08/13  14:41:18  rbadri
 * CSCdi64089:  Multiprotocol support fixes required
 * Branch: California_branch
 * XNS support for LANE
 *
 * Revision 3.5.20.6  1996/07/23  18:50:28  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.5.20.5  1996/07/09  06:06:50  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.5.20.4  1996/06/28  23:34:03  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.5.20.3  1996/05/17  12:18:24  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.6.2  1996/04/26  15:24:51  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.6.1  1996/04/03  22:22:59  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.20.2  1996/05/09  14:50:04  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.5.20.1.18.1  1996/04/27  07:39:56  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.20.1.8.1  1996/04/08  02:44:09  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.5.20.1  1996/03/18  22:54:24  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.24.1  1996/03/22  22:56:56  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.10.1  1996/03/05  05:38:33  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.5.4.1  1996/02/27  21:21:29  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5  1996/02/01  06:13:31  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/22  07:53:15  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3.12.1  1996/02/15  19:27:44  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.3  1995/11/17  19:25:42  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:54:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  14:00:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/11  23:59:02  hampton
 * Convert XNS and Apollo to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi39966]
 *
 * Revision 2.3  1995/08/07 05:45:11  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *
 * Revision 2.2  1995/07/10  05:33:13  fred
 * CSCdi36850:  commit fast path queuing for appletalk, decnet, etc.
 *
 * Revision 2.1  1995/06/07  23:30:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Description of Routines Exported from this module. 
 *
 * xns_cache_init:	  Initializes XNS cache hash list.
 * xns_cache_update:      Updates XNS cache.
 * xns_fscache_age:	  Ages XNS routing cache.
 * xns_fscache_flush:	  Flushes XNS routing cache.
 * show_xns_cache:	  Displays XNS cache entries.
 *
 */

#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "access.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/rif_inline.h"
#include "config.h"
#include "../wan/serial.h"
#include "../if/fddi.h"
#include "../if/network.h"
#include "../if/parser_defs_rif.h"
#include "xns.h"
#include "../xns/xnstypes.h"
#include "xnsfast.h"
#include "novfast.h"
#include "novell.h"
#include "../srt/srb_multiring.h"
#include "../srt/srt_registry.h"
#include "../if/atm.h"
#include "../lane/lane.h"



/*
 * macros
 */

#define TABLE(hash) (xnscache[hash])
#define T_ADDR(hash) (&xnscache[hash])
/*
 * Local Storage
 */
xnscachetype *xnscache[XNS_HASHLEN];


/*
 * xnscache_init
 * Initializes XNS cache hashlist.
 */

void xns_cache_init (void)
{
    int i;

    for (i=0; i < XNS_HASHLEN; i++)
        xnscache[i] = NULL;
    xnscache_version = 0L;
    /*
     * Register some functions
     */
    reg_add_fs_cache_update(LINK_XNS, xns_cache_update, "xns_cache_update");
}

/*
 * xns_free_entry
 * Release an entry
 */

void xns_free_entry (
    xnscachetype *entry_ptr,
    xnscachetype *prev_ptr)
{
    int status;

    status = raise_interrupt_level(NETS_DISABLE);
    if (prev_ptr != NULL) {
	prev_ptr->xnsc_next_ptr = entry_ptr->xnsc_next_ptr;
    }
    if (entry_ptr->xnsc_path_list_ptr != NULL) {
	free(entry_ptr->xnsc_path_list_ptr);
	entry_ptr->xnsc_path_list_ptr = NULL;
    }
    free(entry_ptr);
    reset_interrupt_level(status);
}

/*
 * xnscache_increment_version
 * Routine to invalidate the XNS cache version number
 */

void xnscache_increment_version (hwidbtype* idb, idbtype *swidb,
				 boolean immediately)
{
    if ((idb == NULL) || (idb->xns_routecache))
	xnscache_version++;
}

/*
 * xns_cache_update
 * Updates the XNS cache with the current XNS datagram.
 * Caller checks for fast switching okay on output interface,
 * for XNS protocol suite, and for correct output encapsulation.
 * Promote entries to top of list if they've drifted down aways.
 */

void xns_cache_update (paktype *pak_ptr, ulong address_tmp) 
                                       /* pointer to outgoing Datagram */
{
    xnshdrtype	*xns;
    xnscachetype *cache_ptr, *prev_ptr;
    idbtype	*idb_ptr;
    hwidbtype	*hwidb;
    boolean	newflag;
    int		len, status, depth;
    ulong	bucket;
    xns_ripentrytype *rip_rt;
    xnspathtype	*cache_pptr, *prev_pptr;
    pathentrytype *path_ptr;
    ushort	pathcount;
    charlong	dnet;
    uchar	*daddr, dsap;
    int		delta, extra, riflen;
    trrif_t	*trh;
    riftype	*rif;
    ulong	*ptr;
    leveltype	level;
    xnssb	*xsb;

    xns = (xnshdrtype *)xnsheadstart(pak_ptr);

    /*
     * Conditions for acceptance:
     * - must be transit (not sourced by us).
     * - output interface must be different that input interface.
     * - output must be via an MCI interface.
     * - no multicast destinations.
     */
    idb_ptr = pak_ptr->if_output;
    hwidb = idb_ptr->hwptr;
    if ((pak_ptr->if_input == NULL) || (pak_ptr->if_input == idb_ptr))
	/* 
	 * Either a loopback or not a transit packet.  Ignore it.
	 */
	return;
    /*
     * Is fast switching permitted?
     */
    if (!hwidb->xns_fastokay)
	return;
    if (pak_ptr->flags & PAK_ENCAPS_ONLY) {
	/*
	 * TRUE for LANE unicast packets which are sent on BUS VCs.
	 * Populate the cache only when it is sent on the Data Direct VC
	 */
	return;
    }
    /*
     * Forbid multicast.  Check for valid encapsulation type as well.
     * ATM supports fastswitching only on LANE.
     */
    if (hwidb->status & IDB_ETHER) {
	if (pak_ptr->datagramstart[0] & XNS_MULTICAST_ETHER)
	    return;
	if (pak_ptr->enctype != ET_ARPA)
	    return;
    } else if (hwidb->status & IDB_TR) {
	if (pak_ptr->datagramstart[2] & XNS_MULTICAST_TR)
	    return;
	if (pak_ptr->enctype != ET_SNAP)
	    return;
    } else if (hwidb->status & IDB_FDDI) {
	if (hwidb->type == IDBTYPE_FDDIT) {
	    if (pak_ptr->datagramstart[1] & XNS_MULTICAST_ETHER)
		return;
	} else if (pak_ptr->datagramstart[1] & XNS_MULTICAST_TR) {
	    return;
	}
    } else if (hwidb->status & IDB_ATM) {
	if (!is_atm_lane(idb_ptr)) {
	    return;
	}
    }

    /*
     * Look for interfaces that do not work correctly when an odd byte
     * start is used.
     */
    if ((hwidb->status & IDB_ETHER) && ether_odd_byte_start_bug(hwidb))
	extra = 1;
    else
        extra = 0;

    /*
     * Find or create a cache pointer
     */
    newflag = FALSE;
    depth = 0;
    dnet.d.lword = GET2WD(xns->ddnet);
    bucket =  dnet.d.lword;
    bucket ^= GET2WD(xns->dhost);
    bucket ^= GETSHORT(xns->dhost+4);
    bucket ^= (bucket >> 16);
    bucket ^= (bucket >> 8);
    bucket &=0xFF;
    prev_ptr = (xnscachetype *)T_ADDR(bucket);
    for (cache_ptr = TABLE(bucket); cache_ptr;
	 cache_ptr = cache_ptr->xnsc_next_ptr, depth++){
	if (ieee_equal(xns->dhost, cache_ptr->xnsc_dest_address) &&
	    (cache_ptr->xnsc_dest_net == dnet.d.lword))
	    break;
	prev_ptr = cache_ptr;
    }
    if (cache_ptr == NULL) {
	cache_ptr = (xnscachetype *)malloc(sizeof(xnscachetype));
	if (cache_ptr == NULL)
	    return;
	newflag = TRUE;
	cache_ptr->xnsc_path_count = 0;
	cache_ptr->xnsc_path_list_ptr = NULL;
	prev_ptr = NULL;
	depth = 0;
    } else
	/* check if this entry is already in sink with new info  */
        if (cache_ptr->xnsc_version == xnscache_version)
	    return;

    xsb = idb_get_swsb_inline(idb_ptr, SWIDB_SB_XNS);
    /*
     * Update cache, link a new one into hash table
     */
    cache_ptr->xnsc_dest_net = dnet.d.lword;
    ieee_copy(xns->dhost, cache_ptr->xnsc_dest_address);
    GET_TIMESTAMP(cache_ptr->xnsc_lastupdate);
    cache_ptr->xnsc_connected = (dnet.d.lword == xsb->xnsnet);
     
    /*
     * check if we are dealing with directly connected net.
     */
    if (cache_ptr->xnsc_connected) {
	if (cache_ptr->xnsc_path_count != 1) {
	    if (cache_ptr->xnsc_path_list_ptr != NULL) {
		level = raise_interrupt_level(NETS_DISABLE);
		free(cache_ptr->xnsc_path_list_ptr);
		cache_ptr->xnsc_path_list_ptr = NULL;
		reset_interrupt_level(level);
	    }
	    if (!(cache_ptr->xnsc_path_list_ptr =  
	                  (xnspathtype *)malloc(sizeof( xnspathtype)))){
		xns_free_entry(cache_ptr, prev_ptr);
		return;
	    }
	}
	dsap = 0;
	if (hwidb->status & IDB_ETHER) {
	    len = ETHER_ARPA_ENCAPBYTES;
	    delta = 0;
	} else if (hwidb->status & IDB_FDDI) {
	    len = FDDI_SNAP_ENCAPBYTES - 1;
	    delta = 1;
	} else if (hwidb->status & IDB_TR) {
	    /*
	     * Compute RIF.
	     */
	    trh = (trrif_t *)pak_ptr->datagramstart;
	    if (trh->saddr[0] & TR_RII) {
		len = TRING_ENCAPBYTES - 2 + (trh->rc_blth & RC_LTHMASK);
		dsap = *((uchar *)&trh->rc_blth + (trh->rc_blth & RC_LTHMASK));
	    } else {
		len = TRING_ENCAPBYTES - 2;
		dsap = *((uchar *)&trh->rc_blth);
	    }
	    /*
	     * Now look for SAP vs SNAP. Gah.
	     */
	    len += ((dsap == SAP_SNAP) ? SNAP_HDRBYTES : SAP_HDRBYTES);
	    delta = 2;
	} else {
	    len = pak_ptr->encsize;	/* This includes any board encap */
	    delta = 0;
	}
	cache_ptr->xnsc_path_next_ptr = cache_ptr->xnsc_path_list_ptr;
	cache_ptr->xnsc_path_next_ptr->xnsp_idb_ptr = idb_ptr;
	cache_ptr->xnsc_path_next_ptr->xnsp_next_ptr = 
	                                  cache_ptr->xnsc_path_list_ptr;
	bcopy(pak_ptr->datagramstart + delta, 
	      cache_ptr->xnsc_path_list_ptr->xnsp_mh.mac_bytes, len);
	bcopy(pak_ptr->datagramstart + delta, 
	      cache_ptr->xnsc_path_list_ptr->xnsp_mh_oa.mac_bytes+1, len);
	cache_ptr->xnsc_path_list_ptr->length = len;
	cache_ptr->xnsc_path_list_ptr->extra_bytes = extra;
	cache_ptr->xnsc_path_count = 1;
    } else {
        /*
         * Here we are dealing with a remote net.
         * Look for routes to this network.
         */
        rip_rt = grip_find_rt(dnet.d.lword, xns_table); 

	if (rip_rt) {
	    if (!(cache_ptr->xnsc_path_count >= rip_rt->paths)) {
		if (cache_ptr->xnsc_path_list_ptr != NULL) {
		    level = raise_interrupt_level(NETS_DISABLE);
		    free(cache_ptr->xnsc_path_list_ptr);
		    cache_ptr->xnsc_path_list_ptr = NULL;
		    reset_interrupt_level(level);
		}
		if (!(cache_ptr->xnsc_path_list_ptr =
		 (xnspathtype *)malloc((sizeof(xnspathtype)*rip_rt->paths)))) {
		    xns_free_entry(cache_ptr, prev_ptr);
		    return;
		}
	    }
	    pathcount = rip_rt->paths;
	    cache_ptr->xnsc_path_count = 0;
            cache_ptr->xnsc_path_next_ptr = NULL;
	    path_ptr = rip_rt->path_list;
	    prev_pptr = cache_pptr = cache_ptr->xnsc_path_list_ptr;
	    while (pathcount) {
		idb_ptr = cache_pptr->xnsp_idb_ptr = path_ptr->idb;
		hwidb = idb_ptr->hwptr;
		if (!(interface_up(idb_ptr))) {
		    path_ptr = path_ptr->next;
		    pathcount--;
		    continue;
		} else if (!hwidb->xns_fastokay) { 
		    path_ptr = path_ptr->next;
		    pathcount--;
		    continue;
		}
		if (hwidb->status & IDB_ETHER) { 
		    ieee_copy(path_ptr->srchost, cache_pptr->xnsp_mh.mac_bytes);
		    ieee_copy(hwidb->hardware,
			      &cache_pptr->xnsp_mh.mac_bytes[IEEEBYTES]);
		    cache_pptr->xnsp_mh.mac_shorts[6] = TYPE_XNS;
		    len = ETHER_ARPA_ENCAPBYTES;
		} else if (hwidb->status & IDB_FDDI){
		    daddr = path_ptr->srchost;
		    if (hwidb->type == IDBTYPE_FDDIT) {
			ieee_copy(path_ptr->srchost, cache_pptr->xnsp_mh.mac_bytes);
			ieee_copy(hwidb->hardware, 
			      &cache_pptr->xnsp_mh.mac_bytes[IEEEBYTES]);
		    } else {
			ieee_swap(daddr, cache_pptr->xnsp_mh.mac_bytes);
			ieee_copy(hwidb->bit_swapped_hardware, 
			      &cache_pptr->xnsp_mh.mac_bytes[IEEEBYTES]);
		    }
		    cache_pptr->xnsp_mh.mac_longs[3] = 0xaaaa0300; /*aaaa0300*/
		    cache_pptr->xnsp_mh.mac_shorts[8] = 0x0000;     /* 00 00 */
		    cache_pptr->xnsp_mh.mac_shorts[9] = TYPE_XNS;
		    len = FDDI_SNAP_ENCAPBYTES - 1;
		} else if (hwidb->status & IDB_TR) {
		    level = raise_interrupt_level(NETS_DISABLE);
		    /*
		     * If multiring is enabled on this interface for the
		     * protocol in question, look up a RIF. If it is not,
		     * then don't look up the RIF.
		     */
		    if (srb_multiring_xns_enabled(idb_ptr)) {
			rif = rif_lookup(path_ptr->srchost, hwidb, NULL, 0);
			if (!rif) {
			    reset_interrupt_level(level);
			    path_ptr = path_ptr->next;
			    pathcount--;
			    continue;
			}
			riflen = rif->length;
		    } else {
			rif = NULL;
			riflen = 0;
		    }
		    xsb = idb_get_swsb_inline(idb_ptr, SWIDB_SB_XNS);
		    if (xsb->xns_enctype == ET_3COM_TR) {
			/*
			 * XNS, 3Com style.  802.2 with 3COM's SAP code.
			 */
			ieee_copy(path_ptr->srchost,
				  cache_pptr->xnsp_mh.mac_bytes);
			ieee_copy(hwidb->hardware,
				  cache_pptr->xnsp_mh.mac_bytes+IEEEBYTES);
			if (riflen)
			  bcopy(rif->rif, &cache_pptr->xnsp_mh.mac_bytes[12],
				riflen);
			ptr = (ulong *)(&cache_pptr->xnsp_mh.mac_bytes[12]
					+ riflen);
			*ptr = (SAP_3COM << 24) | (SAP_3COM << 16)  |
			       (LLC1_UI << 8);
			len = TRING_SAP_ENCAPBYTES + riflen - 2;
		    } else if (xsb->xns_enctype == ET_UB_TR) {
			/*
			 * XNS, UB style.  SNAP with UB's OUI code.
			 */
			ieee_copy(path_ptr->srchost,
				  cache_pptr->xnsp_mh.mac_bytes);
			ieee_copy(hwidb->hardware,
				  &cache_pptr->xnsp_mh.mac_bytes[IEEEBYTES]);
			if (riflen)
			  bcopy(rif->rif, &cache_pptr->xnsp_mh.mac_bytes[12],
				riflen);
			ptr = (ulong *)(&cache_pptr->xnsp_mh.mac_bytes[12]
					+ riflen);
			*ptr++ = 0xaaaa0300;
			*ptr++ = (0xdd0f << 16) | TYPE_XNS;
			len = TRING_SNAP_ENCAPBYTES + riflen - 2;
		    } else {
			/*
			 * Generic XNS.  SNAP with zero OUI code.
			 */
			ieee_copy(path_ptr->srchost,
				  cache_pptr->xnsp_mh.mac_bytes);
			ieee_copy(hwidb->hardware,
				  &cache_pptr->xnsp_mh.mac_bytes[IEEEBYTES]);
			if (riflen)
			  bcopy(rif->rif, &cache_pptr->xnsp_mh.mac_bytes[12],
				riflen);
			ptr = (ulong *)(&cache_pptr->xnsp_mh.mac_bytes[12] + riflen);
			*ptr++ = 0xaaaa0300;
			*ptr++ = TYPE_XNS;
			len = TRING_SNAP_ENCAPBYTES + riflen - 2;
		    }
		    reset_interrupt_level(level);
		    if (riflen)
			cache_pptr->xnsp_mh.mac_bytes[IEEEBYTES] |= TR_RII;
		} else if (hwidb->status & IDB_ATM) {
		    if (is_atm_lane(idb_ptr)) {
			len = pak_ptr->encsize;
		    	bcopy(pak_ptr->datagramstart, 
			      cache_pptr->xnsp_mh.mac_bytes,
			      len);

		    } else {
			/*
			 * Not supporting non LANE interfaces to fastswitch
			 * free the cache entry we malloced above.
			 */
			xns_free_entry(cache_ptr, prev_ptr);
			return;
		    }
		} else {
		    if (hwidb->cache_board_encap) {
			/* get the board encap for the beg. of the cache */
			delta = (*hwidb->cache_board_encap)(
				      hwidb,
				      cache_pptr->xnsp_mh.mac_bytes,
				      MAXMAC_BYTES - HDLC_ENCAPBYTES);
#if DEBUG
			if (delta == -1 ) {
			    buginf("illegal encap size");
			    delta = 0;
			    len = 0;
			}
#endif
			/* Save off real encap after the board encap */
			*(ulong *)&cache_pptr->xnsp_mh.mac_bytes[delta] =
			    HDLC_XNSCODE;
			len = delta + HDLC_ENCAPBYTES;
		    } else {
			cache_pptr->xnsp_mh.mac_longs[0] = HDLC_XNSCODE;
			len = HDLC_ENCAPBYTES;
		    }
		}
		bcopy(cache_pptr->xnsp_mh.mac_bytes,
		      cache_pptr->xnsp_mh_oa.mac_bytes+1, len);
		cache_pptr->length = len;
		cache_pptr->extra_bytes = extra;
		if (rip_rt->nextpath == path_ptr)
		    cache_ptr->xnsc_path_next_ptr = cache_pptr;
		cache_ptr->xnsc_path_count++;
		pathcount--;
		prev_pptr->xnsp_next_ptr = cache_pptr;
		prev_pptr = cache_pptr;
		cache_pptr++;
		path_ptr = path_ptr->next;
	    }
	    prev_pptr->xnsp_next_ptr = cache_ptr->xnsc_path_list_ptr;
            if (cache_ptr->xnsc_path_next_ptr == NULL)
	        cache_ptr->xnsc_path_next_ptr = cache_ptr->xnsc_path_list_ptr;
	}
    }
    if (!(cache_ptr->xnsc_path_count)) {
	xns_free_entry(cache_ptr, prev_ptr);
	return;
    }
    if (newflag) {
	status = raise_interrupt_level(NETS_DISABLE);
	cache_ptr->xnsc_next_ptr = xnscache[bucket];
	xnscache[bucket] = cache_ptr;
	reset_interrupt_level(status);
    } else if (depth > XNSRCACHE_DEPTH) {
	status = raise_interrupt_level(NETS_DISABLE);
	prev_ptr->xnsc_next_ptr = cache_ptr->xnsc_next_ptr;
	cache_ptr->xnsc_next_ptr = xnscache[bucket];
	xnscache[bucket] = cache_ptr;
	reset_interrupt_level(status);
    }
    cache_ptr->xnsc_version = xnscache_version;
}

/*
 * xnscache_ager
 * Age XNS routing cache, or flush it entirely.
 */

static void xnscache_ager (boolean flushflag)
{
    xnscachetype *cache_ptr, *pred;
    int i, status;
    
    if (!xns_running)
	return;
    if (flushflag)
	xnscache_increment_version(NULL, NULL, FALSE);
    status = raise_interrupt_level(NETS_DISABLE);
    for (i = 0; i < XNS_HASHLEN; i++) {
	pred = (xnscachetype *) &xnscache[i];
	cache_ptr = pred->xnsc_next_ptr;
	while (cache_ptr) {
	    if (flushflag ||
		((xnscache_version != cache_ptr->xnsc_version) &&
		 (CLOCK_OUTSIDE_INTERVAL(cache_ptr->xnsc_lastupdate,
					 XNSRCACHE_TIMEOUT)))) {
		xns_free_entry(cache_ptr, pred);
		cache_ptr = pred;
	    }
	    pred = cache_ptr;
	    cache_ptr = cache_ptr->xnsc_next_ptr;
	}
    }
    reset_interrupt_level(status);
}

/*
 * xns_fscache_age
 * Age XNS routing cache
 */

void xns_fscache_age (void)
{
    xnscache_ager(FALSE);
}

/*
 * xns_fscache_flush
 * Flush XNS routing cache
 */

void xns_fscache_flush (void)
{
    xnscache_ager(TRUE);
}

/*
 * show_xns_cache
 * Display contents of XNS routing cache
 */

static const char xstring[] =
"               Destination   Interface    MAC Header\n";

void show_xns_cache (ushort type)
{
    xnscachetype *cache_ptr;
    int i, j, depth, length;
    ushort count;
    xnspathtype *path_ptr, *tmp_ptr, *first_ptr;

    automore_enable(NULL);
    printf("XNS routing cache version is %lu\n", xnscache_version);
    automore_header(xstring);
    for (i = 0; i < XNS_HASHLEN; i++) {
	for (cache_ptr = xnscache[i], depth = 0; cache_ptr; depth++, 
	     cache_ptr = cache_ptr->xnsc_next_ptr) {
	     if (!validmem(cache_ptr)) {
		 goto done;
	     }
	     mem_lock(cache_ptr);
	     printf("%c%-10q.%e  ",
	       ((cache_ptr->xnsc_version == xnscache_version) ? '*' : ' '),
               type, cache_ptr->xnsc_dest_net,
	       cache_ptr->xnsc_dest_address);
	    first_ptr = path_ptr = cache_ptr->xnsc_path_list_ptr;
	    if (!validmem(first_ptr)) {
		free(cache_ptr);
		goto done;
	    }
	    mem_lock(first_ptr);
	    tmp_ptr = cache_ptr->xnsc_path_next_ptr;
	    for (count = cache_ptr->xnsc_path_count; count;
		 count--, path_ptr++) {
		if (cache_ptr->xnsc_path_count != 1)
		    printf("%c%11s  ", ((path_ptr == tmp_ptr) ? '@' : ' '),
		                         path_ptr->xnsp_idb_ptr->namestring);
		else
		    printf(" %11s  ",path_ptr->xnsp_idb_ptr->namestring);
		length = path_ptr->length;
		for (j = 0; j < length; j++) {
		    printf("%-02x", path_ptr->xnsp_mh.mac_bytes[j]);
		    if (j == 17)
			printf("\n%43c",' ');
		}
		printf("\n");
		if (count - 1)
		    printf("%28c",' ');
		if (automore_quit()) {
		    free(first_ptr);
		    free(cache_ptr);
		    goto done;
		}
	    }
	    free(first_ptr);
	    free(cache_ptr);
	}
    }
  done:
    automore_disable();
}

/* xns_fastsetup
 * Turn on use of fast switching.  Logically AND configuration option
 * "idb->dn_routecache" with whether or not this interface can support
 * fast switching. If fastswitching is allowed, set  "idb->xns_fastokay" to
 * TRUE and the appropriate bit in the "fast_switch_flag".
 */

void xns_fastsetup (hwidbtype *idb)
{
    hwidbtype *tmpidb;
    boolean fastflag;
    
    fastflag = reg_invoke_fast_supported(LINK_XNS, idb);

    if (!router_enable || idb->comp_db || idb->ppp_compression)
	fastflag = FALSE;

    idb->xns_fastokay = (fastflag && idb->xns_routecache && xns_running);
    if (idb->xns_fastokay) {
	idb->fast_switch_flag |= FSF_XNS;
	reg_invoke_register_drivers(FSF_XNS, idb);
    } else {
	idb->fast_switch_flag &= ~FSF_XNS;
	FOR_ALL_HWIDBS(tmpidb)
	    if (tmpidb->xns_fastokay)
		break;
	if (!tmpidb) {
	    reg_invoke_register_drivers(FSF_XNS, idb);
	}
    }
    xnscache_increment_version(NULL, NULL, FALSE);
}
