/* $Id: ipfast_les.c,v 3.8.18.21 1996/09/13 16:33:18 ahh Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_les.c,v $
 *------------------------------------------------------------------
 * ipfast_les.c -- Pancake Fast Switching Support
 *
 * 22-May-1990, Chris Shaker
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_les.c,v $
 * Revision 3.8.18.21  1996/09/13  16:33:18  ahh
 * CSCdi69060:  NAT: input acl checks not done
 * Conditional was wrong, programmer is an idiot.
 * Branch: California_branch
 *
 * Revision 3.8.18.20  1996/09/04  17:16:06  ahh
 * CSCdi67885:  NAT: sub-interfaces not supported properly
 * Allow interface commands to work in sub-interface configuration
 * mode, allow translation to be enabled on some sub-interfaces
 * but not others and still have the fast-path work.
 * Branch: California_branch
 *
 * Revision 3.8.18.19  1996/08/30  20:22:52  wilber
 * CSCdi67725:  ip ping failed when increasing packet size when irb is
 * enabled
 * Recalculate and overwrite the length for the fast-switched SNAP packet
 * (Bridge-group Virtual Interface specific)
 * Branch: California_branch
 *
 * Revision 3.8.18.18  1996/08/23  18:39:44  abaindur
 * CSCdi66370:  C7200 IP fastswitch code is a loner
 * Branch: California_branch
 *
 * Revision 3.8.18.17  1996/08/15  23:45:25  wilber
 * CSCdi66123:  BVI code continues to use a packet after disposal
 * (datagram_done)
 * Fix the missing/misused returns
 * Branch: California_branch
 *
 * Revision 3.8.18.16  1996/07/15  10:19:29  xliu
 * CSCdi62909:  Need to check crypto map assigned to an interface in les
 * fastswitch
 * Branch: California_branch
 *
 * Revision 3.8.18.15  1996/07/11  14:19:53  fbordona
 * CSCdi62521:  DLSw circuit does not connect using FST over ATM peer
 *              Remove DLSw FST hooks from platform/media specific drivers.
 * Branch: California_branch
 *
 * Revision 3.8.18.14  1996/07/06  05:53:33  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.8.18.13  1996/06/21  07:44:26  gchristy
 * CSCdi60987:  RSP: support fastswitching of GRE/IPIP tunnels
 * Branch: California_branch
 *
 * Revision 3.8.18.12  1996/06/18  01:45:07  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.8.18.11  1996/06/05  23:18:47  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Add crypto fastsend vector instead of using media fastsend vector.
 *
 * Revision 3.8.18.10  1996/06/01  00:24:36  ahh
 * CSCdi59212:  NAT: unsupported switching paths can be disabled more se...
 * Branch: California_branch
 *
 * Revision 3.8.18.9  1996/05/13  23:20:38  ahh
 * CSCdi56426:  NAT: EFT bugs and feedback
 * Fix hash algorithm, fix address block coalescing, reduce up-front
 * memory allocation, drop untranslatable packets, clean up debugs.
 * Branch: California_branch
 *
 * Revision 3.8.18.8  1996/05/12  23:09:34  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.8.18.7  1996/05/04  01:03:39  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.8.18.6  1996/04/27  05:51:39  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.8.18.5  1996/04/24  00:05:56  ahh
 * CSCdi55483:  NAT: code review comments, various bug fixes
 * Fast-path fixup, cruft removal.
 * Branch: California_branch
 *
 * Revision 3.4.16.3  1996/04/26  01:22:59  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.4.16.2  1996/04/17  02:05:58  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.4.16.1  1996/02/23  20:23:39  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.8.18.4  1996/04/19  15:41:51  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.21  1996/04/11  23:14:56  carrel
 * Branch: IosSec_branch
 * IP code review comments #1
 *
 * Revision 2.1.6.20  1996/04/09  20:37:29  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.19  1996/03/30  05:39:15  xliu
 * Branch: IosSec_branch
 * Commit before CA split.
 *
 * Revision 2.1.6.18  1996/03/01  01:37:33  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 2.1.6.17  1996/02/21  03:29:57  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 3.8.18.3  1996/04/17  13:41:47  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.8.18.2  1996/04/16  18:53:12  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.8.18.1  1996/03/18  20:16:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.2  1996/03/13  01:37:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.2.1  1996/02/20  00:59:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/10  00:17:26  raj
 * CSCdi35584:  IP Alignment error
 * Retrieve halfword values in header using GETSHORT when in
 * fastswitch path.
 *
 * Revision 3.7  1996/02/07  04:04:46  mmcneali
 * CSCdi45328:  VLAN Translator/Routers FAIL to decrement IP TTL.
 *
 * Revision 3.6  1996/01/23  08:49:51  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.5  1996/01/20  06:53:19  lbustini
 * CSCdi39980:  BOD not working on PRI when fastswitching is enabled
 * Use correct MIP vc number when selecting output interface.
 *
 * Revision 3.4  1995/11/26  08:15:51  mmcneali
 * CSCdi43630:  ISL VLAN Multicast changed--IOS gs7 images are DEAF
 *
 *         "no ip address" should not be generated on subinterfaces,
 *         creates backwards compatibility problems with earlier releases.
 *
 *         Explicitly indicate cache entries that are associated with a
 *         dot10 vLAN subinterface.
 *
 * Revision 3.3  1995/11/17  09:34:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:40:44  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:56:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:04:48  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/09/25  08:32:01  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.5  1995/08/11  15:51:33  dino
 * CSCdi38636:  Shave off a few cycles processing tunnels at interrupt
 * level
 *
 * Revision 2.4  1995/08/10  23:31:00  wfried
 * CSCdi37608:  oam f5 cells generated by 4500 are not recognized by
 * analyzer
 * Changed ATM code to pass VC pointer for OAM cells and other related
 * changes
 *
 * Revision 2.3  1995/07/29  01:55:55  dino
 * CSCdi36878:  Fast multicast bypasses access lists, accounting, policy
 *              routing
 *
 * Revision 2.2  1995/07/18  04:16:03  shaker
 * CSCdi35990:  Need packet pool for fast switched packet structure
 * Use new pak_set_dirty() to only set PAK_BUF_DIRTY iff
 * PAK_BUF_USE_DIRTY is set. Set PAK_BUF_USE_DIRTY for RSP fs_pak
 *
 * Revision 2.1  1995/06/07  20:59:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "access.h"
#include "../parser/parser_defs_access.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../if/network.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../if/atm.h"
#include "../wan/serial.h"

#include "../ip/ip.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccess_private.h"
#include "../ip/ipaccount.h"
#include "../ip/ipfast.h"
#include "../ip/ip_registry.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"
#include "../wan/dialer.h"
#include "../wan/dialer_private.h"
#include "../ip/tunnel.h"
#include "../ip/gre.h"
#include "../crypto/crypto_registry.h"
#include "../crypto/crypto_public.h"
#include "../ipmulticast/igmp.h"
#include "../ip/ipaccess_inline.h"
#include "../iprouting/route.h"

#include "../tbridge/tbridge_sde.h"
#include "../vlan/vlan.h"

/*
 * Externs
 */
extern ushort fastiphdrchk(ushort *, ulong);

/*
 * Be very cautious about stepping back too far when overwriting
 * an outbound encapsulation on top of the inbound (since we use
 * some optimized routines to do the rewrite which can overwrite
 * a few bytes). 
 */
#define IP_REWRITE_SLOP_BYTES	8

#define DDR_SETNEXT(OUT, DDB)						\
    output_fast_switch_type = DDB->fs_type;		\
    if (output->cache_board_encap != NULL) {				\
	/*								\
	 * Set proper MIP channel.					\
	 */								\
	(*output->cache_board_encap)(output, cptr->m.macstring,		\
				     sizeof(ushort));			\
    }

#define DDR_CLASSIFY(OFFSET, RESUME)					\
    acl_headertype *acl;						\
    /*									\
     * Tell lower layers that we have groveled in the packet more than	\
     * normal for the fast switch path and the packet may need to be	\
     * flushed out of the data cache for some platforms:		\
     */									\
    pak_set_dirty(pak);							\
									\
    acl = acl_array[dialerlist];					\
    if (!ip_accesscheck_inline(pak, acl, NULL, TRUE)) 			\
	goto RESUME;
    

/*
 * ip_fs_input_acl_inline
 *
 */

static inline boolean ip_fs_input_acl_inline (hwidbtype *input, paktype *pak)
{
    acl_headertype *acl;

    /*
     * For DDR interfaces, get the access list from the
     * rotary group lead.
     */
    if (is_ddr(input)) {
	hwidbtype *netidb;

	netidb = get_netidb(input);
	acl = netidb->firstsw->ip_input_accesslist;
    } else {
	acl = pak->if_input->ip_input_accesslist;       
    }
    if (acl && !ip_accesscheck_inline(pak, acl, NULL, TRUE))  
	return(FALSE);                                  
    return(TRUE);
}

#if defined(OBJECT_4K)

/*
 * ether_encap_rewrite:
 * Very special inline asm routine to perform ethernet encap
 * rewrite. Instead of rewriting exactly 14 bytes from the
 * cache into the packet, we rewrite 16 bytes, clobbering 
 * the first 2 bytes. This is more efficient. It is not
 * a problem, as the first 2 bytes of the frame should never be
 * needed and are therefore undefined as it is.
 */
static inline void ether_encap_rewrite (uchar *cache_ptr, uchar *pak_ptr)
{
    ulong data0, data1;

    /*
     * Go into mips3 instruction set
     */
    asm volatile (".set mips3");
    asm volatile (".set noreorder");

    /*
     * Read the first two longlong's from the cache
     */
    asm volatile ("ldl %0, (0-2)(%1); ldr %0, (7-2)(%1);" 
		  : "=&r" (data0) : "r" (cache_ptr) );
    asm volatile ("ldl %0, (8-2)(%1); ldr %0, (15-2)(%1);" 
		  : "=&r" (data1) : "r" (cache_ptr) );

    /*
     * Write the two longlong's into the packet
     */
    asm volatile ("sdl %0, (0-2)(%1); sdr %0,(7-2)(%1);" 
		  :  : "r" (data0), "r" (pak_ptr) );
    asm volatile ("sdl %0, (8-2)(%1); sdr %0, (15-2)(%1);" 
		  :  : "r" (data1), "r" (pak_ptr) );

    /*
     * Go back to mips2
     */
    asm volatile (".set reorder");
    asm volatile (".set mips2");
}

/*
 * fddi_encap_rewrite:
 * Very special inline asm routine to perform fddi encap
 * rewrite. This routine can possibly write 7 extra bytes of
 * rewrite. It also writes the frame control byte.
 */
static inline void fddi_encap_rewrite (uchar *cache_ptr, uchar *pak_ptr, 
				       int bytes)
{
    uchar *src, *dst;
    ulong data;

    src = cache_ptr + bytes;
    dst = pak_ptr + bytes + 1;	/* FC byte not in cache */

    do {
	src -= 8;
	dst -= 8;
	bytes -= 8;

	asm volatile (".set mips3");
	asm volatile (".set noreorder");

	/*
	 * read 8 bytes of data from possibly unaligned source
	 * address into a 64-bit register.
	 */
	asm volatile ("ldl %0, 0(%1); ldr %0, 7(%1);"  
		      : "=&r" (data) : "r" (src) );

	/*
	 * write out 8 bytes of data to a potentially unaligned
	 * destination.
	 */
	asm volatile ("sdl %0, 0(%1); sdr %0, 7(%1);" 
		      : "=&r" (data) : "r" (dst) );

	asm volatile (".set reorder");
	asm volatile (".set mips2");

    } while (bytes > 0);

    /*
     * Fill in the frame control byte
     */
    *pak_ptr = FDDI_LLC_FC_BYTE;
}

/*
 * bvi_encap_rewrite:
 * Very special inline asm routine to perform bvi encap
 * rewrite. This routine can possibly write 7 extra bytes of
 * rewrite.
 */
static inline void bvi_encap_rewrite (uchar *cache_ptr, uchar *pak_ptr, 
				      int bytes)
{
    uchar *src, *dst;
    ulong data;

    src = cache_ptr + bytes;
    dst = pak_ptr + bytes;

    do {
	src -= 8;
	dst -= 8;
	bytes -= 8;

	asm volatile (".set mips3");
	asm volatile (".set noreorder");

	/*
	 * read 8 bytes of data from possibly unaligned source
	 * address into a 64-bit register.
	 */
	asm volatile ("ldl %0, 0(%1); ldr %0, 7(%1);"  
		      : "=&r" (data) : "r" (src) );

	/*
	 * write out 8 bytes of data to a potentially unaligned
	 * destination.
	 */
	asm volatile ("sdl %0, 0(%1); sdr %0, 7(%1);" 
		      : "=&r" (data) : "r" (dst) );

	asm volatile (".set reorder");
	asm volatile (".set mips2");

    } while (bytes > 0);
}

#else /* !OBJECT_4K */

/*
 * ether_encap_rewrite:
 * Normal C code to rewrite an ethernet encapsulation. Note, we write it
 * backwards as this is faster, and we write 16 bytes instead of 14 (again
 * because this is faster).
 */
static inline void ether_encap_rewrite (uchar *cache_ptr, uchar *pak_ptr)
{
    ulong *src, *dst;

    src = (ulong *)(cache_ptr + ETHER_ARPA_ENCAPBYTES);
    dst = (ulong *)(pak_ptr + ETHER_ARPA_ENCAPBYTES);
    PUTLONG(--dst, GETLONG(--src));
    PUTLONG(--dst, GETLONG(--src));
    PUTLONG(--dst, GETLONG(--src));
    PUTLONG(--dst, GETLONG(--src));
}

/*
 * fddi_encap_rewrite:
 * Normal C code to rewrite an fddi encapsulation plus frame control
 */
static inline void fddi_encap_rewrite (uchar *cache_ptr, uchar *pak_ptr, 
				       int bytes)
{
    int lwords;
    ulong *src, *dst;

    /*
     * Put on the frame control byte
     */
    *pak_ptr++ = FDDI_LLC_FC_BYTE;

    /*
     * Copy the cache entry, including RIF, to the packet
     */
    src = (ulong *)cache_ptr;
    dst = (ulong *)pak_ptr;
    for (lwords = 0; lwords < (bytes>>2); lwords++) {
	PUTLONG(dst++, *src++);
    }

    /*
     * Check for one remaining short in the cache
     */
    if (bytes & 0x02) {
	PUTSHORT(dst, *(ushort *)src);
    }
}

/*
 * bvi_encap_rewrite:
 * Normal C code to rewrite a BVI encapsulation
 */
static inline void bvi_encap_rewrite (uchar *cache_ptr, uchar *pak_ptr, 
				      int bytes)
{
    ulong *src, *dst;

    /*
     * Copy the cache entry to the packet
     */
    src = (ulong *)(cache_ptr + bytes);
    dst = (ulong *)(pak_ptr + bytes);
    while (bytes > 0) {
	dst--;
	src--;
	PUTLONG(dst, *src);
	bytes -= 4;
    }
}

#endif /* OBJECT_4K */

/*
 * fs_ip_prepare_packet:
 * Prepare a packet for a rewrite string
 */
static inline boolean fs_ip_prepare_packet (paktype *pak, int delta)
{
    particletype *rw_p;

    if (delta < (PARTICLE_ENCAPBYTES-IP_REWRITE_SLOP_BYTES)) {
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
 * fs_ip_ether_rewrite:
 * Rewrite the encapsulation for an IP packet being fast switched
 * to an ethernet interface
 */
static inline boolean fs_ip_ether_rewrite (paktype *pak,
					   ushort size,
					   idbtype *swoutput,
					   hwidbtype *output,
					   ipcache_rntype *cptr,
					   crypto_vector_t *crypto_vec)
{
    int delta;

    /*
     * In addition to Ethernet v2 encapsulated IP datagrams
     * also Fast Switch IEEE 802.10 and ISL vLAN packets.
     */
    if (!swoutput->isl_vlan && !swoutput->sde_said_db) {

	/*
	 * Handle normal ethernet frames IE no 802.10/ISL configured
	 */
	if (pak_has_particles(pak)) {
	    /*
	     * Note, all ethernet port adaptors (10mb and 100mb) do
	     * automatic frame padding for small frames, so no need to check.
	     */
	    delta = ETHER_ARPA_ENCAPBYTES - pak->encsize;
	    fs_ip_prepare_packet(pak, delta);
	} else {
	    /*
	     * Handle the contigous packet case
	     */
	    size = size - pak->encsize + ETHER_ARPA_ENCAPBYTES;
	    if (size < MINETHERSIZE) {
		size = MINETHERSIZE;
	    }
	    pak->datagramsize = size;
	    pak->datagramstart = pak->network_start - ETHER_ARPA_ENCAPBYTES;
	}

	/*
	 * Perform the re-write
	 */
	ether_encap_rewrite(cptr->m.macstring, pak->datagramstart);
	
    } else if (IS_ISL_VLAN_SUBINTERFACE(swoutput)) {

	/*
	 * ISL Routing
	 * This involves prepending a ETHER_8023_ENCAPBYTES header
	 * along with a ISL_HEADER_LENGTH byte ISL header onto the packet.
	 */
	if (pak_has_particles(pak)) {
	    delta = ETHER_8023_ENCAPBYTES + ISL_HEADER_LENGTH - pak->encsize;
	    if (!fs_ip_prepare_packet(pak, delta)) {
		return(FALSE);
	    }
	} else {
	    /*
	     * Handle the contigous packet case
	     * Skip the minimum length check since the IP
	     * header(20), ISL encapsulation(26) and interior
	     * Ethernet MAC(14) total 60 bytes themselves.
	     */
	    pak->datagramsize = (size - pak->encsize) +
		(ETHER_8023_ENCAPBYTES + ISL_HEADER_LENGTH);
	    pak->datagramstart = pak->network_start -
		(ETHER_8023_ENCAPBYTES + ISL_HEADER_LENGTH);
	}

	/*
	 * Write the exterior ISL header, followed
	 * by the standard interior Ethernet v2 format.
	 *
	 * Mark the packet as requiring ISL Inner CRC
	 * generation; done by the FEIP ucode until the
	 * MICE hardware assist is available at which
	 * time the "PAK_DO_ISL_CRC" becomes redundant.
	 */
	write_isl_cache_macheader((ulong *)pak->datagramstart, 
				  cptr->m.maclong);
	pak->flags |= PAK_DO_ISL_CRC;
	swoutput->isl_vlan->egressing_isl_packets[VLAN_ROUTE_IP]++;

    } else if (IS_DOT10_VLAN_SUBINTERFACE(swoutput)) {
	ushort dot3_length;

	/*
	 * 802.10 Routing
	 * This involves prepending a SDE_ENCAPBYTES header
	 * along with an ETHER_SNAP_ENCAPBYTES header onto the
	 * packet.
	 */
	if (pak_has_particles(pak)) {
	    delta = ETHER_SNAP_ENCAPBYTES + SDE_ENCAPBYTES - pak->encsize;
	    if (!fs_ip_prepare_packet(pak, delta)) {
		return(FALSE);
	    }
	} else {
	    /*
	     * Handle the contigous case
	     */
	    size = (size - pak->encsize) + SDE_ENCAPBYTES +
		ETHER_SNAP_ENCAPBYTES;
	    if (size < MINETHERSIZE)
		size = MINETHERSIZE;
	    pak->datagramsize = size;
	    pak->datagramstart = pak->network_start -
		(ETHER_SNAP_ENCAPBYTES + SDE_ENCAPBYTES);
	}

	/*
	 * Write the 802.10 header (need to pass in the length)
	 */
	dot3_length = pak->datagramsize - ETHER_8023_ENCAPBYTES;
	write_ether_dot10_cache_macheader((ulong *)pak->datagramstart, 
					  cptr->m.maclong, dot3_length,
					  (ETHER_SNAP_ENCAPBYTES + 
					   SDE_ENCAPBYTES));

	swoutput->sde_said_db->egressing_dot10_packets[VLAN_ROUTE_IP]++;
    } else {
	/*
	 * Failed. This should never happen. Punt to process level
	 */
	return (FALSE);
    }

    pak->if_output = swoutput;

    if (crypto_vec) {
	(*crypto_vec->fs_vec)(output, pak);
	return (TRUE);
    }
		
    /*
     * Send the packet on its way
     */
    (*output->fastsend)(output, pak);
    return(TRUE);
}

/*
 * fs_ip_fddi_rewrite:
 * Rewrite FDDI encapsulation for a packet going to an FDDI interface
 */
static inline boolean fs_ip_fddi_rewrite (paktype *pak,
					  ushort size,
					  idbtype *swoutput,
					  hwidbtype *output,
					  ipcache_rntype *cptr,
					  crypto_vector_t *crypto_vec)
{
    int delta;

    if (pak_has_particles(pak)) {
	delta = (1 + cptr->length) - pak->encsize;
	if (!fs_ip_prepare_packet(pak, delta)) {
	    return(FALSE);
	}
    } else {
	/*
	 * Handle the contigous packet case
	 */
	pak->datagramsize = size - pak->encsize + (1 + cptr->length);
	pak->datagramstart = pak->network_start - (1 + cptr->length);
    }

    /*
     * Perform the rewrite, do vLAN accounting and send the packet.
     * Note, this will add the frame control byte as well.
     */
    fddi_encap_rewrite(cptr->m.macstring, pak->datagramstart, cptr->length);
    if (IS_DOT10_VLAN_SUBINTERFACE(swoutput)) {
	swoutput->sde_said_db->egressing_dot10_packets[VLAN_ROUTE_IP]++;
    }
    pak->if_output = swoutput;

    if (crypto_vec) {
	(*crypto_vec->fs_vec)(output, pak);
	return (TRUE);
    }
		
    (*output->fastsend)(output, pak);
    return(TRUE);
}

/*
 * fs_ip_hdlc_rewrite:
 * Rewrite HDLC encapsulation for a packet going to a Serial interface
 */
static inline boolean fs_ip_hdlc_rewrite (paktype *pak,
					  ushort size,
					  idbtype *swoutput,
					  hwidbtype *output,
					  ipcache_rntype *cptr,
					  crypto_vector_t *crypto_vec)
{
    int delta;

    if (pak_has_particles(pak)) {
	delta = cptr->length - pak->encsize;
	fs_ip_prepare_packet(pak, delta);
    } else {
	/*
	 * Handle the contigous case
	 */
	pak->datagramsize = size - pak->encsize + cptr->length;
	pak->datagramstart = pak->network_start - cptr->length;
    }

    /*
     * Perform rewrite and send packet on its way
     */
    PUTLONG(pak->datagramstart, *cptr->m.maclong);
    if (cptr->length > HDLC_ENCAPBYTES) {
	/* 
	 * Ugly, but needed for RSP and MIP. Possibly also PRI PA.
	 */
	PUTSHORT(pak->datagramstart + sizeof(ulong), cptr->m.macshort[2]);
    }
    pak->if_output = swoutput;

    if (crypto_vec) {
	(*crypto_vec->fs_vec)(output, pak);
	return (TRUE);
    }
		
    (*output->fastsend)(output, pak);
    return(TRUE);
}

/*
 * fs_ip_channel_rewrite:
 * Rewrite Channel encapsulation for a packet going to an IBM channel
 */
static inline boolean fs_ip_channel_rewrite (paktype *pak,
					     ushort size,
					     idbtype *swoutput,
					     hwidbtype *output,
					     ipcache_rntype *cptr,
					     crypto_vector_t *crypto_vec)
{
    int delta;

    if (pak_has_particles(pak)) {
	delta = cptr->length - pak->encsize;
	if (!fs_ip_prepare_packet(pak, delta)) {
	    return(FALSE);
	}
    } else {
	/*
	 * Handle the contigous case
	 */
	pak->datagramsize = size - pak->encsize + cptr->length;
	pak->datagramstart = pak->network_start - cptr->length;
    }

    /*
     * Perform rewrite and send packet on its way
     */
    bcopy(cptr->m.maclong, pak->datagramstart, cptr->length);
    pak->if_output = swoutput;

    if (crypto_vec) {
	(*crypto_vec->fs_vec)(output, pak);
	return (TRUE);
    }
		
    (*output->fastsend)(output, pak);
    return(TRUE);
}

/*
 * fs_ip_fr_rewrite:
 * Rewrite Frame Relay encapsulation for a packet going to a Serial interface
 */
static inline boolean fs_ip_fr_rewrite (paktype *pak,
					ushort size,
					idbtype *swoutput,
					hwidbtype *output,
					ipcache_rntype *cptr,
					crypto_vector_t *crypto_vec)
{
    fr_pvc_info_type *fr_pvc;
    int delta;

    if (pak_has_particles(pak)) {
	delta = cptr->length - pak->encsize;
	fs_ip_prepare_packet(pak, delta);
    } else {
	/*
	 * Handle the contigous case
	 */
	pak->datagramsize = size - pak->encsize + cptr->length;
	pak->datagramstart = pak->network_start - cptr->length;
    }

    /*
     * Perform rewrite and send packet on its way
     */
    PUTLONG(pak->datagramstart, *cptr->m.maclong);
    if (cptr->length > FRAME_RELAY_ENCAPBYTES) {
	/* 
	 * Ugly, but needed for RSP and MIP. Possibly PRI PA.
	 */
	PUTSHORT(pak->datagramstart + sizeof(ulong), cptr->m.macshort[2]);
    }
    fr_pvc = HWIDB_TO_FR_PVC(output, cptr->subint_reference_num);
    INC_PVC_FAST_OUTPUT(fr_pvc, pak->datagramsize);
    pak->if_output = swoutput;

    if (crypto_vec) {
	(*crypto_vec->fs_vec)(output, pak);
	return (TRUE);
    }
		
    (*output->fastsend)(output, pak);
    return(TRUE);
}

/*
 * fs_ip_ppp_rewrite:
 * Perform PPP encaps rewrite for IP
 */
static inline boolean fs_ip_ppp_rewrite (paktype *pak,
					 ushort size,
					 idbtype *swoutput,
					 hwidbtype *output,
					 ipcache_rntype *cptr,
					 crypto_vector_t *crypto_vec)
{
    ushort *macsptr, *cache_macsptr;
    iphdrtype *ip;
    int delta;

    if (pak_has_particles(pak)) {
	delta = cptr->length - pak->encsize;
	fs_ip_prepare_packet(pak, delta);
    } else {
	/*
	 * Handle the contigous case
	 */
	ip = (iphdrtype *)ipheadstart(pak);
	delta = cptr->length;
	pak->datagramsize = GETSHORT(&ip->tl) + delta;
	pak->datagramstart = pak->network_start - delta;
    }

    /*
     * Perform rewrite and let packet on its way. The encapsulation
     * can be between one and four bytes
     */
    macsptr = (ushort *)pak->datagramstart;
    cache_macsptr = cptr->m.macshort;
    for (delta=cptr->length; delta >= (int)sizeof(ushort); delta-=sizeof(ushort)) {
	PUTSHORT(macsptr++, *cache_macsptr++);
    }
    if (delta) {
	*(uchar *)macsptr = *(uchar *)cache_macsptr;
    }
    pak->if_output = swoutput;

    if (crypto_vec) {
	(*crypto_vec->fs_vec)(output, pak);
	return (TRUE);
    }
		
    (*output->fastsend)(output, pak);
    return(TRUE);
}

/*
 * fs_ip_slip_rewrite:
 * Perform SLIP encaps rewrite for IP
 */
static inline boolean fs_ip_slip_rewrite (paktype *pak,
					  ushort size,
					  idbtype *swoutput,
					  hwidbtype *output,
					  ipcache_rntype *cptr,
					  crypto_vector_t *crypto_vec)
{
    iphdrtype *ip;
    int delta;
    
    /*
     * There is no encapsulation for SLIP
     */
    if (cptr->length != 0) {
	return(FALSE);
    }
    if (pak_has_particles(pak)) {
	delta = -pak->encsize;
	fs_ip_prepare_packet(pak, delta);
    } else {
	/*
	 * Handle the contigous case
	 */
	ip = (iphdrtype *)ipheadstart(pak);
	pak->datagramsize = GETSHORT(&ip->tl);
	pak->datagramstart = pak->network_start;
    }

    pak->if_output = swoutput;

    if (crypto_vec) {
	(*crypto_vec->fs_vec)(output, pak);
	return (TRUE);
    }
		
    /*
     * Send the packet on its way
     */
    (*output->fastsend)(output, pak);
    return(TRUE);
}

/*
 * fs_ip_tr_rewrite:
 * Perform Token Ring encaps rewrite for IP
 */
static inline boolean fs_ip_tr_rewrite (paktype *pak,
					ushort size,
					idbtype *swoutput,
					hwidbtype *output,
					ipcache_rntype *cptr,
					crypto_vector_t *crypto_vec)
{
    ulong *macptr, *cache_macptr;
    ushort *macsptr;
    int delta, lwords;

    if (pak_has_particles(pak)) {
	delta = (cptr->length + 2) - pak->encsize;
	if (!fs_ip_prepare_packet(pak, delta)) {
	    return(FALSE);
	}
    } else {
	/*
	 * Handle the contigous case
	 */
	pak->datagramstart = pak->network_start - (2 + cptr->length);
	pak->datagramsize = size - pak->encsize + (2 + cptr->length); 
    }
    
    /*
     * Build AC-FC (not in the cache)
     */
    macsptr = (ushort *)pak->datagramstart;
    PUTSHORT(macsptr++, (AC_PRI0 << 8) | FC_TYPE_LLC);
    macptr = (ulong *)macsptr;
    cache_macptr = cptr->m.maclong;
    
    /*
     * Copy the cache entry, including RIF, to the packet
     */
    for (lwords = 0; lwords < cptr->length>>2; lwords++) {
	PUTLONG(macptr++, *cache_macptr++);
    }

    /*
     * Check for one remaining short in the cache
     */
    if (cptr->length & 0x02) {
	PUTSHORT(macptr, *(ushort *)cache_macptr);
    }

    /* 
     * vLAN Packet accounting 
     */
    if (IS_DOT10_VLAN_SUBINTERFACE(swoutput)) {
	swoutput->sde_said_db->egressing_dot10_packets[VLAN_ROUTE_IP]++;
    }

    pak->if_output = swoutput;

    if (crypto_vec) {
	(*crypto_vec->fs_vec)(output, pak);
	return (TRUE);
    }
		
    /*
     * Send the packet on its way
     */
    (*output->fastsend)(output, pak);
    return(TRUE);
}

/*
 * fs_ip_lex_rewrite:
 * Perform LEX encaps rewrite for IP
 */
static inline boolean fs_ip_lex_rewrite (paktype *pak,
					 ushort size,
					 idbtype *swoutput,
					 hwidbtype *output,
					 ipcache_rntype *cptr,
					 crypto_vector_t *crypto_vec)
{
    int delta;
    ulong *macptr, *cache_macptr;

    if (pak_has_particles(pak)) {
	delta = cptr->length - pak->encsize;
	fs_ip_prepare_packet(pak, delta);
    } else {
	/*
	 * Handle the contigous case
	 */
	size = size - pak->encsize + cptr->length;
	if (size < MINETHERSIZE) {
	    size = MINETHERSIZE;
	}
	pak->datagramsize = size;
	pak->datagramstart = pak->network_start - cptr->length;
    }

    /*
     * Perform rewrite and let the packet on its way
     */
    macptr = (ulong *)pak->datagramstart;
    cache_macptr = cptr->m.maclong;
    PUTLONG(macptr++, GETLONG(cache_macptr++));
    PUTLONG(macptr++, GETLONG(cache_macptr++));
    PUTLONG(macptr++, GETLONG(cache_macptr++));
    PUTLONG(macptr++, GETLONG(cache_macptr++));
    PUTLONG(macptr, GETLONG(cache_macptr));
    pak->if_output = swoutput;

    if (crypto_vec) {
	(*crypto_vec->fs_vec)(output, pak);
	return (TRUE);
    }
		
    (*output->fastsend)(output, pak);
    return(TRUE);
}

/*
 * fs_ip_atm_dxi_rewrite:
 * Perform ATM/DXI encaps rewrite for IP
 */
static inline boolean fs_ip_atm_dxi_rewrite (paktype *pak,
					     ushort size,
					     idbtype *swoutput,
					     hwidbtype *output,
					     ipcache_rntype *cptr,
					     crypto_vector_t *crypto_vec)
{
    ulong *macptr, *cache_macptr;
    ushort *macsptr, *cache_macsptr;
    int delta, bytes;

    if (pak_has_particles(pak)) {
	delta = cptr->length - pak->encsize;
	if (!fs_ip_prepare_packet(pak, delta)) {
	    return(FALSE);
	}
    } else {
	/*
	 * Handle the contigous case
	 */
	size = size - pak->encsize + cptr->length;
	pak->datagramsize = size;
	pak->datagramstart = pak->network_start - cptr->length;
    }

    /*
     * Perform rewrite
     */
    macptr = (ulong *)pak->datagramstart;
    cache_macptr = cptr->m.maclong;
    bytes = cptr->length;
    while (bytes >= 4) {
	PUTLONG(macptr++, *cache_macptr++);
	bytes -= 4;
    }
    if (bytes & 0x02) {
	macsptr = (ushort *)macptr;
	cache_macsptr = (ushort *)cache_macptr;
	PUTSHORT(macsptr, *cache_macsptr);
    }

    /*
     * Account for it and send it on its way
     */
    INC_DXI_PVC_FAST_OUTPUT(
        HWIDB_TO_DXI_PVC(output, cptr->subint_reference_num), 
        pak->datagramsize);
    pak->if_output = swoutput;

    if (crypto_vec) {
	(*crypto_vec->fs_vec)(output, pak);
	return (TRUE);
    }
		
    (*output->fastsend)(output, pak);
    return(TRUE);
}

/*
 * fs_ip_atm_rewrite:
 * Perform ATM encaps rewrite for IP
 */
static inline boolean fs_ip_atm_rewrite (paktype *pak,
					 ushort size,
					 idbtype *swoutput,
					 hwidbtype *output,
					 ipcache_rntype *cptr,
					 crypto_vector_t *crypto_vec)
{
    ulong *macptr, *cache_macptr;
    ushort *macsptr, *cache_macsptr;
    int delta, bytes;

    pak->atm_vcid = cptr->m.macshort[0];
    pak->atm_enctype = cptr->m.macshort[1];

    if (pak_has_particles(pak)) {
	delta = cptr->length - pak->encsize;
	if (!fs_ip_prepare_packet(pak, delta)) {
	    return(FALSE);
	}
    } else {
	/*
	 * Handle the contigous case
	 */
	pak->datagramsize = size - pak->encsize + cptr->length;
	pak->datagramstart = pak->network_start - cptr->length;
    }

    /*
     * Perform rewrite and send the packet on its way
     */
    macptr = (ulong *)pak->datagramstart;
    cache_macptr = (ulong *)cptr->m.maclong;
    bytes = cptr->length;
    while (bytes >= 4) {
	PUTLONG(macptr++, *cache_macptr++);
	bytes -= 4;
    }
    if (bytes > 0) {
	macsptr = (ushort *)macptr;
	cache_macsptr = (ushort *)cache_macptr;
	if (bytes & 0x02) {
	    PUTSHORT(macsptr++, *cache_macsptr++);
	}
	if (bytes & 0x01) {
	    *(char *)macsptr = *(char *)cache_macsptr;
	}
    }
    pak->if_output = swoutput;

    if (crypto_vec) {
	(*crypto_vec->fs_vec)(output, pak);
	return (TRUE);
    }
		
    (*output->fastsend)(output, pak);
    return(TRUE);
}

/*
 * fs_ip_smds_rewrite:
 * Perform SMDS encaps rewrite for IP
 */
static inline boolean fs_ip_smds_rewrite (paktype *pak,
					  ushort size,
					  idbtype *swoutput,
					  hwidbtype *output,
					  ipcache_rntype *cptr,
					  crypto_vector_t *crypto_vec)
{
    ulong *macptr, *cache_macptr, *padptr;
    smds_pdb_t *spd = output->smds_pdb;
    smdstype *sm;
    ushort len, pad, ix;
    int delta;

    if (pak_has_particles(pak)) {
	len = pak->datagramsize - pak->encsize;
	delta = cptr->length - pak->encsize;
	if (!fs_ip_prepare_packet(pak, delta)) {
	    return(FALSE);
	}
    } else {
	/*
	 * Handle the contigous case
	 */
	len = size - pak->encsize;
	pak->datagramstart = pak->network_start - cptr->length;
    }

    cache_macptr = cptr->m.maclong;
    macptr = (ulong *)pak->datagramstart;
    for (ix = cptr->length; ix >= sizeof(ulong); ix -= sizeof(ulong)) {
	PUTLONG(macptr++, *cache_macptr++);
    }
    if (ix) {
	ushort *spt = (ushort *)macptr;
	ushort *cpt = (ushort *)cache_macptr;
	PUTSHORT(spt, *cpt);
    }
    pad = 3 - ((len + 3) % 4);
    len += cptr->length;
    sm = (smdstype *)(pak->datagramstart + spd->fast_output_modelen);
    sm->smds_xhlpi &= ~3;
    sm->smds_xhlpi |= pad;
    sm->smds_betag = output->smds_begin_tag++;
    sm->smds_basize = len - (4 + spd->fast_output_modelen) + pad;

    if (pak_has_particles(pak)) {
	particletype *tail_p;

	/*
	 * SMDS pad
	 */
	tail_p = pak->particlequeue.qtail;
	padptr = (ulong *)(tail_p->data_start + tail_p->data_bytes);
	PUTLONG(padptr, 0);
	
	/*
	 * SMDS trailer
	 */
	pak->datagramsize += (4 + pad);
	cache_macptr = (ulong *)&sm->smds_rsvd;
	macptr = (ulong *)(tail_p->data_start + tail_p->data_bytes + pad);
	
	/*
	 * Copy the trailer and fix up the tail particle. We are assuming that
	 * there is always room at the end of a particle for up to three bytes
	 * of pad and four bytes of trailer.
	 */
	PUTLONG(macptr, GETLONG(cache_macptr));
	tail_p->data_bytes += (pad + 4);

    } else {

	/*
	 * Handle the contigous case. SMDS pad
	 */
	padptr = (ulong *)((uchar *)pak->datagramstart + len);
	PUTLONG (padptr, 0);

	/*
	 * SMDS trailer
	 */
	pak->datagramsize = len + 4 + pad;
	cache_macptr = (ulong *)&sm->smds_rsvd;
	macptr = (ulong *)((uchar *)pak->datagramstart + 
			   pak->datagramsize - 4);
	
	/*
	 * Copy the trailer.
	 */
	PUTLONG(macptr, GETLONG(cache_macptr));
    }

    pak->if_output = swoutput;

    if (crypto_vec) {
	(*crypto_vec->fs_vec)(output, pak);
	return (TRUE);
    }
		
    /*
     * Send the packet on its way
     */
    (*output->fastsend)(output, pak);
    return(TRUE);
}

/*
 * fs_ip_bvi_rewrite:
 * Perform ether type encaps rewrite for IP packets on a BVI
 */
static inline boolean fs_ip_bvi_rewrite (paktype *pak,
					 ushort size,
					 idbtype *swoutput,
					 ipcache_rntype *cptr)
{
    int delta;

    /*
     * Though we are building ether-like packet because
     * the BVI is virtual-ether, we may have SNAP-encap
     * because the ARP cache tells us that this packet
     * should be SNAP-encaped, which means the original
     * bridged interface we learn the ARP from is a token
     * or fddi like medias.
     */

    delta = cptr->length - pak->encsize; /* Encaps len diff */

    if (pak_has_particles(pak)) {
	/*
	 * Note, all ethernet port adaptors (10mb and 100mb) do
	 * automatic frame padding for small frames, so no need to check.
	 */
	if (!fs_ip_prepare_packet(pak, delta)) {
	    return (FALSE);
	}
    } else {
	/*
	 * Handle the contigous packet case
	 */
	size += delta;

	/* Virtual ether */
	if (size < MINETHERSIZE)
	    size = MINETHERSIZE;

	pak->datagramsize = size;
	pak->datagramstart = pak->network_start - cptr->length;
    }
    
    bvi_encap_rewrite(cptr->m.macstring, pak->datagramstart, cptr->length);

    /*
     * Overwrite the input interface to the BVI.
     * Clear the output interface to let bridging code 
     * sort it out
     */
    if (!change_if_input(pak, swoutput)) {
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
    if (cptr->length == ETHER_ARPA_ENCAPBYTES) {
	pak->info_start = NULL;
    } else {
	pak->info_start = pak->datagramstart + ETHER_8023_ENCAPBYTES;
	/*
	 * SNAP-encaped. Adjust the 802.3 length
	 */
	PUTSHORT(&pak->datagramstart[2 * IEEEBYTES], 
		 (pak->datagramsize - ETHER_8023_ENCAPBYTES));
    }

    /* Do output packet accounting for bvi here and charge to bridging.
     * This is transition (5) for CSCdi92583.
     * Note it is assumed if_input has been overwritten to bvi at this
     * point. 
     */
    swoutput->hwptr->counters.outputs_fast[ACCT_PROTO_BRIDGE]++;
    swoutput->hwptr->counters.tx_cumbytes_fast[ACCT_PROTO_BRIDGE] += 
    pak->datagramsize;
	
    return (reg_invoke_tbridge_forward(pak));
}

/*
 * ip_fixup_smds:
 * Handle SMDS input packets
 */
static inline ushort ip_fixup_smds (paktype *pak, iphdrtype *ip)
{
    ushort size;

    /*
     * Account for the trailer
     */
    size = GETSHORT(&ip->tl) + pak->encsize;
    if (pak->datagramsize < SMDS_ENCAPBYTES2) {
	return(0);
    }

    if (pak_has_particles(pak)) {
	int delta;
	particletype *mp;

	/*
	 * Adjust the various sizes in the packet. Note, 
	 * adjust the packet from the end
	 */
	delta = pak->datagramsize - size;
	mp = pak->particlequeue.qtail;
	if (mp->data_bytes <= delta) {
	    delta -= mp->data_bytes;
	    unqueue_inline(&pak->particlequeue, mp);
	    retparticle(mp);
	    mp = pak->particlequeue.qtail;
	}
	mp->data_bytes -= delta;
	pak->datagramsize = size;
    }
    return(size);
}

/*
 * ip_fs_accum_stats_inline:
 * Accumulate ip traffic stats
 */
static inline void ip_fs_accum_stats_inline (hwidbtype* input, ushort size)
{
    /*
     * Do input accounting on packets and bytes in. The
     * fastsend routine will account for output:
     */
    input->counters.inputs_fast[ACCT_PROTO_IP]++;
    input->counters.rx_cumbytes_fast[ACCT_PROTO_IP] += size;
    ip_traffic.inputs++;
    ip_traffic.forwarded++;
}

/*
 * ip_fastswitch_common
 * Very high speed switching of IP datagrams.
 * This routine has been very heavily optimized for speed, since it is in
 * the inner loop of the code that handles back-to-back datagrams.  Do not
 * add permanent debugging code, subroutines, or much of anything else.
 * The GNU CC compiler will generate the best code.
 * Speedups *must* be debugged with an oscilloscope.
 */

static inline boolean ip_fastswitch_common (paktype *pak, boolean nat)
{
    charlong destaddr;
    ipcache_rntype *cptr;
    hwidbtype *output, *input;
    idbtype *swoutput, *swinput;
    ushort size;
    iphdrtype *ip;
    enum FSTYPE output_fast_switch_type;
    crypto_vector_t *crypto_vec = NULL;
    boolean fast_switched;

    pak->cryptoflags = 0;
    pak->linktype = LINK_IP;
    ip = (iphdrtype *)ipheadstart(pak);

    if (nat) {
	idbtype *idb;

	/*
	 * Check for outside->inside.  Needs to be done before routing.
	 * -- actually want to do decryption, then input access-list checks,
	 * then translation, then routing, then output access-lists
	 */
	input = pak->if_input->hwptr;

	if (is_ddr(input))
	    idb = get_netidb(input)->firstsw;
	else
	    idb = pak->if_input;
	if (idb->ip_info->nat_outside) {
	    /*
	     * Check inbound ACL's if enabled ANYWHERE on box.
	     */
	    if (input->ip_fast_flags & IP_FAST_INPUTACCESSLISTS) {
		if (!ip_fs_input_acl_inline(input, pak))
		    return(FALSE);
	    }
	    if (reg_invoke_fast_translate_before_routing(RXTYPE_DODIP, pak))
		return(FALSE);
	}
    }

    /*
     * The ip fastswitch cache is a chain bucket hash table.
     * Compute the hash table index based on the IP destination
     * address. Then search the chain bucket hash table entry
     * for this destination address. If you find this destination
     * address, check to make sure that this cache entry is valid:
     */

    destaddr.d.lword = GETLONG(&ip->dstadr);
    cptr = ipcache_lookup_fast(destaddr.d.lword);
    if (cptr) {
	/*
	 * Verify that this is a proper IP packet by checking
	 * for a correct IP protocol version, a valid IP header
	 * length, a valid total packet length, and a valid
	 * checksum.  If any of these are invalid, return FALSE.
	 * Else, decrement the TTL and recompute the IP header
	 * checksum. Then, set the MCICHECKED bit so that
	 * process level code does not check the CRC and
	 * decrement the TTL again.
	 */
	if (fastiphdrchk((ushort *)ip, destaddr.d.lword))
	    return(FALSE);
	else
	    pak->flags |= PAK_MCICHECKED;
    
	swinput = pak->if_input;
	input = swinput->hwptr;

	/*
	 * Handle inbound SMDS frames
	 */
	if (input->fast_switch_type == FS_SMDS) {
	    size = ip_fixup_smds(pak, ip);
	    if (size == 0) {
		return(FALSE);
	    }
	} else {
	    size = pak->datagramsize;
	}
	    
	/*
	 * Check the size of the input IP packet against
	 * the output IP MTU size. This works because
	 * the incoming MTU size is on both sides of the
	 * comparison, and drops out, leaving a comparison
	 * between the input IP packet size and the output
	 * IP MTU size. This could be more clearly written
	 * as 'if ( (size - pak->encsize) > swoutput->ip_mtu)':
	 */
	swoutput = cptr->idb;
	if (size > (swoutput->ip_mtu + pak->encsize))
	    return (FALSE);					

	output = swoutput->hwptr;
	/*
	 * Though we don't populate the cache when the input
	 * and output interfaces are the same and
	 * idb->ip_routecache_same isn't set, without this check,
	 * there is nothing to stop us from fast-switching out
	 * the same interface since cache can get populated from
	 * other flow.
	 */
	if ((swoutput == swinput) && !swoutput->ip_routecache_same)
	    return (FALSE);
	
	/*
	 * If neither accounting nor access lists nor policy routing
	 * are enabled, attempt to fast switch this packet now:
	 */
	if (!output->ip_fast_flags) {
	    /*
	     * Re entry point, if we pass output access checks
	     * and/or perform IP accounting for this output
	     * interface:
	     */
	    resume_flow:
		
	    pak->acct_proto = ACCT_PROTO_IP;
	    output_fast_switch_type = output->fast_switch_type;

	    /*
	     * Re entry point for DDR fastswitching. Output now points
	     * to the physical output interface.
	     */
	    resume_dialer: 
    
	    switch (output_fast_switch_type) {

	    /*
	     * Switch to Ethernet, ISL or 802.10
	     */
	    case FS_ETHER:
		fast_switched = fs_ip_ether_rewrite(pak, size, swoutput, output,
					   cptr, crypto_vec);
                break;
	    /*
	     * Switch to FDDI
	     */
	    case FS_FDDI:
		fast_switched = fs_ip_fddi_rewrite(pak, size, swoutput, output,
					  cptr, crypto_vec);
                break;
	    /*
	     * Switch to HDLC
	     */
	    case FS_HDLC:
		fast_switched = fs_ip_hdlc_rewrite(pak, size, swoutput, output,
					  cptr, crypto_vec);
                break;
	    /*
	     * Switch to an IBM channel
	     */
	    case FS_CHANNEL:
		fast_switched = fs_ip_channel_rewrite(pak, size, swoutput, output, 
					     cptr, crypto_vec);
                break;
	    /*
	     * Switch to Frame Relay
	     */
	    case FS_FR:
		fast_switched = fs_ip_fr_rewrite(pak, size, swoutput, output,
					cptr, crypto_vec);
                break;
	    /*
	     * Switch to PPP
	     */
	    case FS_PPP:
		fast_switched = fs_ip_ppp_rewrite(pak, size, swoutput, output,
					 cptr, crypto_vec);
                break;
	    /*
	     * Switch to SLIP
	     */
	    case FS_SLIP:
		fast_switched = fs_ip_slip_rewrite(pak, size, swoutput, output,
					  cptr, crypto_vec);
                break;
	    /*
	     * Switch to SMDS
	     */
	    case FS_SMDS:
		fast_switched = fs_ip_smds_rewrite(pak, size, swoutput, output,
					  cptr, crypto_vec);
                break;
	    /*
	     * Switch to Token Ring
	     */
	    case FS_TOKEN:
		fast_switched = fs_ip_tr_rewrite(pak, size, swoutput, output,
					cptr, crypto_vec);
                break;
	    /*
	     * Switch to LEX
	     */
	    case FS_LEX:
		fast_switched = fs_ip_lex_rewrite(pak, size, swoutput, output,
					 cptr, crypto_vec);
                break;
	    /*
	     * Switch to a GRE Tunnel
	     */
	    case FS_TUNNEL:
		pak->if_output = swoutput;

		if (crypto_vec) {
		    (*crypto_vec->fs_vec)(output, pak);
		}
                else {
                  (*output->fastsend)(output, pak);
                }
		fast_switched = TRUE;
                break;
	    /*
	     * Switch to ATM/DXI
	     */
	    case FS_ATM_DXI:
		fast_switched = fs_ip_atm_dxi_rewrite(pak, size, swoutput, output, 
					     cptr, crypto_vec);
                break;
	    /*
	     * Switch to ATM
	     */
	    case FS_AIP:
		fast_switched = fs_ip_atm_rewrite(pak, size, swoutput, output, 
					 cptr, crypto_vec);
                break;
	    /*
	     * Switch to NULL (bit bucket)
	     */
	    case FS_NULL:
		datagram_done(pak);
		fast_switched = TRUE;
		break;
	    /*
	     * Switch to a Dialer interface
	     */
	    case FS_DIALER:
		/*
		 * Switch to a DDR interface (serial, dialer, PRI, BRI)
		 */
                fast_switched = FALSE;
	        {
		    DDR_FASTSWITCH(input, output, cptr, DDR_SETNEXT, 
				   ip_dialerlist, DDR_CLASSIFY, 0, 
				   cptr->dialergroup,
				   resume_dialer);
		}
                break;

	    /*
	     * Switch to the BVI(Bridge-group Virtual Interface).
	     * We need to pass this packet to tbridge
	     * fast-switching code
	     */
	    case FS_BVI:
		fast_switched = fs_ip_bvi_rewrite(pak, size, swoutput, cptr);
                break;

	    default:
		/*
		 * This really shouldn't happen, but we'll let process level
		 * handle it rather than just trash it
		 */
		fast_switched = FALSE;
	    }

            /*
             * Done with the fast switching path. If we did fast switch
             * accumulate ip traffic statistics and return
             */
            if (fast_switched) {
               ip_fs_accum_stats_inline(input, size);
            }
            return(fast_switched);

	} else {

	    /*
	     * Tell lower layers that we have groveled in the
	     * packet more than normal for the fast switch path
	     * and the packet may need to be flushed out of the
	     * data cache for some platforms:
	     */
	    pak_set_dirty(pak);

	    /*
	     * Check input crypto map and set decrypt flag if needed.
	     */
	    if ((output->ip_fast_flags & IP_FAST_INPUT_CRYPTO_MAP) &&
		swinput->crypto_map) { 
		if (!reg_invoke_crypto_map_check(pak,
						 swinput->crypto_map,
						 CRYPTO_DECRYPT, TRUE) ||
                    crypto_deny_packet(pak)) {
		    return(FALSE);
		}
		if (is_vip(input)) {
		    pak->cryptoflags &= ~CRYPTO_DECRYPT;
		}

		if (pak->cryptoflags & CRYPTO_DECRYPT)
		    crypto_vec = (crypto_vector_t *)input->crypto_vectors;
	    }
	    /*
	     * Now, check for encryption on output, but don't 
	     * encrypt yet. If the output interface is vip interface, by
             * pass the crypto engine on RP/RSP.
	     */
	    if ((output->ip_fast_flags & IP_FAST_CRYPTO_MAP) &&
		swoutput->crypto_map)  {
		pak->if_output = swoutput;	/* needed for crypto EPA */
		if (!reg_invoke_crypto_map_check(pak, swoutput->crypto_map,
						 CRYPTO_ENCRYPT, TRUE) ||
                    crypto_deny_packet(pak)) {
		    return(FALSE);
		}
		if (is_vip(output)) {
		    pak->cryptoflags &= ~CRYPTO_ENCRYPT;
		}
		if (pak->cryptoflags & CRYPTO_ENCRYPT) {
		    crypto_vec =
			(crypto_vector_t *)output->crypto_vectors;
		}
	    }

	    /*
	     * Is IP policy routing enabled on some interface?
	     */
	    if ((output->ip_fast_flags & IP_FAST_POLICY) &&
		(swinput->ip_info && swinput->ip_info->policy_enabled))
		return(FALSE);

	    if (!nat || !swinput->ip_info->nat_outside) {
		/*
		 * Check inbound ACL's if enabled ANYWHERE on box.
		 * This has been done already above if NAT is enabled
		 * and the packet came in an outside interface.
		 */
		if (output->ip_fast_flags & IP_FAST_INPUTACCESSLISTS) {
		    if (!ip_fs_input_acl_inline(input, pak))
			return(FALSE);
		}
	    }

	    if (nat) {
		/*
		 * Is inside-to-outside translation needed on the packet?
		 * This needs to be after routing, but before output
		 * access list checking.
		 */
		if ((output->ip_fast_flags & IP_FAST_NAT) &&
		    !(swinput->ip_info->nat_outside) &&
		    (swoutput->ip_info->nat_outside) &&
		    reg_invoke_fast_translate_after_routing(RXTYPE_DODIP, pak))
		    return(FALSE);
	    }

	    /*
	     * Are access lists enabled on this interface?
	     */
	    if (output->ip_fast_flags & IP_FAST_ACCESSLISTS) {
		if (!ip_accesscheck_inline(pak,
					   swoutput->ip_output_accesslist,
					   NULL, TRUE))
		    return(FALSE);
	    }

	    /*
	     * Is IP accounting enabled on this interface?
	     */
	    if ((output->ip_fast_flags & IP_FAST_ACCOUNTING) &&
		!ip_fast_accumulate_acctg(GETLONG(&ip->srcadr),
					  destaddr.d.lword,
					  GETSHORT(&ip->tl), 1))
		return(FALSE);

	    goto resume_flow;
	}
	
    } else if (IPMULTICAST(destaddr.d.lword)) {
	return(reg_invoke_ipmulticast_les_fs(pak));
    } else if (ip->prot == FST_DLSW_PROT) {
        return(reg_invoke_nim_dlsw_fst_forward(pak->if_input->hwptr, pak));
    }

    input = pak->if_input->hwptr;
    input->counters.cache_miss[ACCT_PROTO_IP]++;
    return(FALSE);        /* Not in IP fastswitch cache */
}

static boolean ip_fastswitch (paktype *pak)
{
    return(ip_fastswitch_common(pak, FALSE));
}

static boolean ip_nat_fastswitch (paktype *pak)
{
    return(ip_fastswitch_common(pak, TRUE));
}

/*
 * ip_les_fs_register_drivers
 * Note that the frame relay support handles the installation
 * of IP and other switching drivers
 */

void ip_les_fs_register_drivers (ulong flags, hwidbtype* idb)
{
    boolean (*fs)(paktype *);

    /*
     * Check to see if we're configuring IP
     */
    if (!(flags & FSF_DODIP))
	return;

    if (idb->fast_switch_flag & FSF_DODIP) {
	/*
	 * We want to replace the fast-switch routine
	 * for all interfaces if NAT is enabled anywhere.
	 */
	if (ip_nat_anywhere)
	    fs = ip_nat_fastswitch;
	else
	    fs = ip_fastswitch;
	reg_add_ether_fs(RXTYPE_DODIP, (service_ether_fs_type)fs,
			 "ip_fastswitch");
	reg_add_token_fs(RXTYPE_SNAP_DODIP, (service_token_fs_type)fs,
			 "ip_fastswitch");
	reg_add_fddi_fs(RXTYPE_DODIP, (service_fddi_fs_type)fs,
			"ip_fastswitch");
	reg_add_hdlc_fs(RXTYPE_DODIP, (service_hdlc_fs_type)fs,
			"ip_fastswitch");
	reg_add_ppp_fs(RXTYPE_DODIP, (service_ppp_fs_type)fs,
		       "ip_fastswitch");
	reg_add_ppp_from_process_fs(RXTYPE_DODIP,
				    (service_ppp_from_process_fs_type)fs,
				    "ip_fastswitch");
	reg_add_slip_fs(RXTYPE_DODIP, (service_slip_fs_type)fs,
			"ip_fastswitch");
	reg_add_smds_fs(RXTYPE_DODIP, (service_smds_fs_type)fs,
			"ip_fastswitch");
	reg_add_smds_fs(RXTYPE_UNKNOWN,
			(service_smds_fs_type)return_false, "return_false");
	reg_add_framerelay_fs(RXTYPE_DODIP, (service_framerelay_fs_type)fs,
			      "ip_fastswitch");
	reg_add_atm_dxi_fs(RXTYPE_DODIP, (service_atm_dxi_fs_type)fs,
			   "ip_fastswitch");
	reg_add_atm_dxi_fs(RXTYPE_SNAP_DODIP, (service_atm_dxi_fs_type)fs,
			   "ip_fastswitch");
	reg_add_aip_fs(RXTYPE_DODIP, (service_aip_fs_type)fs,
			    "ip_fastswitch");
	reg_add_tunnel_fs(RXTYPE_DODIP, (service_tunnel_fs_type)fs,
			  "ip_fastswitch");
	reg_add_channel_fs(RXTYPE_DODIP, (service_channel_fs_type)fs,
			   "ip_fastswitch");
    } else {
	fs_table_fill((fastswitch_type)return_false, "return_false",
		      RXTYPE_DODIP);
	fs_table_fill((fastswitch_type)return_false, "return_false",
		      RXTYPE_SNAP_DODIP);
	reg_add_hdlc_fs(RXTYPE_UNKNOWN,
			(service_hdlc_fs_type)return_false, "return_false");
	reg_add_smds_fs(RXTYPE_UNKNOWN,
			(service_smds_fs_type)return_false, "return_false");
    }
}

static boolean ip_les_fast_supported (int linktype, hwidbtype *hwidb)
{
    if (!hwidb || linktype != LINK_IP || is_tunnel(hwidb))
	return(FALSE);

    return(TRUE);
}

void ip_les_fs_query (void ** ip_fast_p)
{
    *ip_fast_p = ip_fastswitch;
}

void ip_les_fs_init (subsystype* subsys)
{
    reg_add_register_drivers(ip_les_fs_register_drivers,
			     "ip_les_fs_register_drivers");

    reg_add_fast_supported(ip_les_fast_supported, "ip_les_fast_supported");

    reg_add_ip_les_fs_query(ip_les_fs_query, "ip_les_fs_query");
}


/*
 * IP LES FS subsystem header
 */

#define IPLESFS_MAJVERSION 1
#define IPLESFS_MINVERSION 0
#define IPLESFS_EDITVERSION  1

SUBSYS_HEADER(ip_les_fs, IPLESFS_MAJVERSION, IPLESFS_MINVERSION, IPLESFS_EDITVERSION,
	      ip_les_fs_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      "req: fastswitch, iphost");

