/* $Id: tfast_les.c,v 3.3.44.9 1996/09/10 13:55:55 sbelair Exp $
 * $Source: /release/112/cvs/Xsys/ip/tfast_les.c,v $
 *------------------------------------------------------------------
 * Fastswitching support for tunnels
 *
 * September 1995, Scott Mackie and Greg Christy
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: tfast_les.c,v $
 * Revision 3.3.44.9  1996/09/10  13:55:55  sbelair
 * CSCdi67685:  Unable to fast-switch packets through encrypted tunnels
 * Branch: California_branch
 *
 * Queue up packets which are encrypted for process-level decryption
 * (since the gre header is encrypted), and then arrange for the packet
 * to start all over in tunnel_ip_les_fastswitch after decryption.
 *
 * Revision 3.3.44.8  1996/08/28  12:50:25  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.44.7  1996/08/26  19:07:23  ssangiah
 * CSCdi58114: Missing support for fastswitching of GRE for some protocols
 *             and on the c7200.
 * Branch: California_branch
 *
 * Revision 3.3.44.6  1996/08/09  23:51:00  sbelair
 * CSCdi62311:  Encrypted tunnels dont work
 * Branch: California_branch
 * For now at least, if on the fast tunneling path a packet needs
 * decryption befreo it can be processed further (decided by
 * crypto_map_check), then pass the packet to process-level.
 *
 * Revision 3.3.44.5  1996/08/07  21:00:39  mleelani
 * CSCdi65339:  Tunnel fastswitching support for RSP broken for multicast
 * Branch: California_branch
 * Support tunnel fastswitching for multicast packets.
 *
 * Revision 3.3.44.4  1996/07/06  05:54:06  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.3.44.3  1996/06/18  01:46:16  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.3.44.2  1996/04/29  05:23:23  pst
 * CSCdi55097:  Unaligned access in memd - possible IPX/tunneling
 * interaction
 * Branch: California_branch
 *
 * Revision 3.3.44.1  1996/03/18  20:17:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.2  1996/03/13  01:38:19  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.12.1  1996/02/20  14:26:32  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/22  22:26:02  gchristy
 * CSCdi45816:  Fastswitching GRE tunnels loses Type field for IPX
 * - Add RXTYPE_SNAP_NOV1 as one of the recognized IPX rxtypes.
 *
 * Revision 3.2  1995/11/26  08:31:21  gchristy
 * CSCdi42997:  Fast switching fails on Low End GRE tunnels
 * - Commit missing parts.
 * - Initialize fastsend to return_false() in case tfast_les subsystem
 *   is missing.
 *
 * Revision 3.1  1995/11/09  11:58:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/10  23:14:02  gchristy
 * Placeholder for Arkansas_branch.
 *
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
#include "../ip/ip_registry.h"
#include "packet.h"
#include "../../boot/cpu.h"
#include "../if/ether.h"
#include "logger.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "config.h"
#include "../os/pool.h"
#include "../os/particle.h"
#include "../ip/ip.h"
#include "../les/if_les.h"
#include "../if/network.h"
#include "tunnel.h"
#include "gre.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto_registry.h"

extern ushort fastiphdrchk(ushort *, ulong);

/*
 * gre_type2rxtype
 *
 * Given a GRE type, produce an RXTYPE f/s index.
 */

static inline ushort gre_type2rxtype (ulong type)
{
    switch (type) {
      case GRE_PTYPE_OSI:
	return(RXTYPE_CLNS);

      case GRE_PTYPE_XNS:
	return(RXTYPE_XNS);

      case GRE_PTYPE_IP:
	return(RXTYPE_DODIP);

      case GRE_PTYPE_VINES:
	return(RXTYPE_VINES);

      case GRE_PTYPE_DECNET:
	return(RXTYPE_DECNET);

      case GRE_PTYPE_BRIDGE:
	return(RXTYPE_TRANSIT);

      case GRE_PTYPE_APPLETALK:
	return(RXTYPE_APPLE);

      case GRE_PTYPE_NOVELL:
	return(RXTYPE_NOVELL1);

      default:
	return(RXTYPE_UNKNOWN);
    }
}

/*
 * gre_rxtype2type
 *
 * Given an RXTYPE f/s index, produce a GRE type.
 */

static inline ushort gre_rxtype2type (ushort rxtype)
{
    switch (rxtype) {
    case RXTYPE_CLNS:
    case RXTYPE_SAP_CLNS:
	return(GRE_PTYPE_OSI);

    case RXTYPE_XNS:
    case RXTYPE_SNAP_XNS:
	return(GRE_PTYPE_XNS);

    case RXTYPE_DODIP:
    case RXTYPE_SNAP_DODIP:
    case RXTYPE_SAP_IP:
	return(GRE_PTYPE_IP);
	  
    case RXTYPE_VINES:
    case RXTYPE_SNAP_VINES:
    case RXTYPE_SAP_VINES:
	return(GRE_PTYPE_VINES);

    case RXTYPE_DECNET:
    case RXTYPE_SNAP_DECNET:
	return(GRE_PTYPE_DECNET);

    case RXTYPE_TRANSIT:
	return(GRE_PTYPE_BRIDGE);

    case RXTYPE_APPLE:
    case RXTYPE_SNAP_APPLE:
	return(GRE_PTYPE_APPLETALK);

    case RXTYPE_NOVELL1:
    case RXTYPE_NOVELL2:
    case RXTYPE_SNAP_NOV1:
    case RXTYPE_SAP_NOVELL:
	return(GRE_PTYPE_NOVELL);

    default:
	return(GRE_PTYPE_UNKNOWN);
    }
}

/*
 * tunnel_ip_les_fastswitch
 *
 * Fastswitch IP tunnels with GRE and IPINIP encapsulation. This
 * checks for IP multicast packets and dispatches to the multicast 
 * fastswitching functions, otherwise the packets are passed-off to
 * the appropriate fast-switch function.
 */
boolean tunnel_ip_les_fastswitch (paktype *pak)
{

    iphdrtype   *outer_ip, *inner_ip;
    tunnel_info *tinfo;
    idbtype     *input;
    ipaddrtype  srcadr, dstadr;
    gretype     *gre;
    ushort       rxtype;
    ulong        encsize;
    ushort      *p;

    outer_ip = (iphdrtype *) ipheadstart(pak);

    /* we don't defragment yet */
    p = (ushort *)(outer_ip);
    if (GETSHORT(&p[IP_FRAG_INFO]) & (FOMASK | (IP_MF_BIT_MASK << 8))) {
	return FALSE;
    }

    /*
     * Check the incoming frame as fast as possible
     *
     * This call checks the version, header length and CRC blazingly
     * fast. It also, unfortunately, checks the TTL. So, if a frame
     * arrives with a low TTL, we have check it the slow way.
     */
    dstadr = GETLONG(&outer_ip->dstadr);
    if (fastiphdrchk((ushort *)outer_ip, dstadr)) {

      /*
       * We failed for some reason. Make sure it's the right one.
       *
       * Don't reference ip->fo directly, since this may be unaligned
       * for some encapsulations (e.g., tokenring with rif, ISL vLAN).
       */
      if ((*((char *)outer_ip) !=  IPV4_NOOPTION) ||
	  (ipcrc((ushort *)outer_ip, ipheadsize(outer_ip))))
	return(FALSE);
 
    } else {

	/*
	 * We dorked with the TTL. Time to own up.
	 */
	pak->flags |= PAK_MCICHECKED;
    }

    /*
     * Make sure the source address of the frame matches one of
     * our known remote tunnel endpoints
     */
    srcadr = GETLONG(&outer_ip->srcadr);
    dstadr = GETLONG(&outer_ip->dstadr);
    FOR_ALL_TUNNELS(tinfo) {
	if (tinfo->hwidb->state != IDBS_UP) continue;
	if (outer_ip->prot != tinfo->protocol) continue;
	if (srcadr != tinfo->destination) continue;
	if (dstadr == tinfo->source || 
	    (tinfo->source_idb && dstadr == tinfo->source_idb->ip_address)) {
	    break;
	}
    }

    /*
     * If we couldn't find a remote tunnel endpoint, or fast tunnelling
     * is disabled, bail.
     */
    if (!tinfo || !tinfo->fast_enabled) {
	return(FALSE);
    }
    /*
     * Find the GRE header start
     */
    if (tinfo->mode == TUN_MODE_GRE_IP) {

	/*
	 * If the incoming packet has to be decrypted, then
	 * the gre header is encrypted, so give the packet
	 * to the encryption process, then resume processing here.
	 */
	if (pak->if_input && 
	    pak->if_input->crypto_map &&
	    !is_vip(pak->if_input->hwptr) &&
	    !(pak->cryptoflags & CRYPTO_DECRYPT_DONE)) { 

	    hwidbtype *hwidb;

	    /*
	     * A return of FALSE from the crypto map checks
	     * implies that this packet is involved in 
	     * crypto connection setup and this should be
	     * done at process level.
             */
            
	    if (!reg_invoke_crypto_map_check(pak, 
					     pak->if_input->crypto_map,
		     			     CRYPTO_DECRYPT, 
					     TRUE)) {
	        return(FALSE);
            }

            if (crypto_deny_packet(pak)) {
                datagram_done(pak);
		return(TRUE);
	    } 

	    hwidb = pak->if_input->hwptr;
	    if (!IS_VIP(hwidb) && (pak->cryptoflags & CRYPTO_DECRYPT) &&
		  hwidb->crypto_vectors) {
		(*((crypto_vector_t *)
			hwidb->crypto_vectors)->tunnel_fs_vec)(hwidb, pak);
		return(TRUE);
	    }
	}

	/*
	 * Find the GRE header start
	 */
	gre = (gretype *)(ipdatastart(pak));

	/*
	 * We don't currently decapsulate tunnel options at interrupt level
	 * and only support the 4 byte header.
	 */
	if (GETSHORT(gre))		/* any flags? */
	    return(FALSE);

	rxtype   = gre_type2rxtype(GETSHORT(&gre->ptype));
	inner_ip = (iphdrtype *) (ipdatastart(pak) + MINGREHEADERBYTES);
	encsize  = ipheadsize(outer_ip) + MINGREHEADERBYTES;

    } else if (tinfo->mode == TUN_MODE_DVMRP_IP ||
	       tinfo->mode == TUN_MODE_IP_IP) {  

	rxtype   = RXTYPE_DODIP;
	inner_ip = (iphdrtype *) ipdatastart(pak);
	encsize  = ipheadsize(outer_ip);
    } else
	return(FALSE);

    /*
     * Make the packet look like it arrived from the tunnel. Set network_start
     * to the inner header so multicast fastswitching doesn't have to make
     * special checks for tunnels. If fastswitching fails, fix back
     * pak->network_start and pak->if_input to original values.
     */
    input = pak->if_input;
    clear_if_input_inline(pak);
    set_if_input_inline(pak, tinfo->hwidb->firstsw);

    pak->rxtype = rxtype;
    pak->network_start += encsize;
    pak->encsize += encsize;
   
    if (reg_invoke_tunnel_fs(rxtype, pak)) {
	/*
	 * We succeeded. Increment the fastswitching counters for
	 * the tunnel interface.
	 */
        input->hwptr->counters.inputs_fast[ACCT_PROTO_IP]++;
        input->hwptr->counters.rx_cumbytes_fast[ACCT_PROTO_IP] += pak->datagramsize;
	ip_traffic.gre_inputs++;
	return(TRUE);
    }

    /*
     * Clean up our messes...this should be unnecessary, but...
     */
    pak->rxtype         = RXTYPE_DODIP;
    pak->network_start -= encsize;
    pak->encsize       -= encsize;
    
    /*
     * Put the input queue back the way it was. The pointers in the packet 
     * will be reset by the idb->parse_packet call in the relevant driver
     */
    clear_if_input_inline(pak);
    set_if_input_inline(pak, input);
    return(FALSE);
}

/*
 * tunnel_prepare_packet()
 *
 * Prepare a particled packet to be injected into a tunnel. Because of
 * the GRE and IP headers that need to be prefixed to the packet, it
 * needs a new particle at the beginning of the packet for the headers
 * to be written into.
 */
static inline boolean tunnel_prepare_packet (paktype *pak, int headersize)
{
    particletype *head_p;
    
    head_p = allocate_fs_particle();
    if (!head_p)
        return (FALSE);
    /*
     * Adjust the packet pointers to make network_start and datagramstart
     * point to the start of the IP header with encsize set to zero. Clip
     * the current head of the particle queue to snip off the encapsulation
     * and enqueue the new particle to the head of the particle queue.
     */
    pak->network_start = head_p->data_start;
    pak->datagramstart = pak->network_start;
    pak->datagramsize += (headersize - pak->encsize);
    particle_clip_inline(pak->particlequeue.qhead, -pak->encsize);
    head_p->data_bytes = headersize;
    pak->encsize = 0;
    requeue_inline(&pak->particlequeue, head_p);
    return (TRUE);
}

/*
 * tunnel_fastsend
 *
 * Output fastsend routine for tunnels. This is called by fastswitching
 * and bridging in order to send a frame on to the media. In this
 * case, we'll tack on an IP header and send the frame via the
 * IP fastswitching code
 */

void gre_ipip_fastsend (hwidbtype *idb, paktype *pak)
{
    tunnel_info *tinfo = idb->tunnel;
    iphdrtype   *ip;
    paktype     *header;
    ulong        headersize;
    short        acct_proto;
    int		 alignment;
    ulong       *hdrptr, *ptr;
    paktype	*newpak;
    ushort      input_size;
    idbtype	*input;

    header = tinfo->header;

    /*
     * The header information isn't valid yet
     */
    if (!header->if_output) {
	datagram_done(pak);
	return;
    }

    /*
     * Work out how much network level header we'll add
     */
    if (tinfo->mode == TUN_MODE_GRE_IP) 
	headersize = header->datagramsize + MINGREHEADERBYTES;
    else
	headersize = header->datagramsize;

    if (pak_has_particles(pak)) {
        if (!tunnel_prepare_packet(pak, headersize))  {
            datagram_done(pak);
            return;
        }
    } else {
        /*
         * Adjust the packet pointers so that datagramstart and
         * network_start point at the start of the IP header with
         * encsize set to zero.
         */
        pak->network_start -= headersize;
        pak->datagramstart  = pak->network_start;
        pak->datagramsize  -= pak->encsize;
        pak->encsize        = 0;
        pak->datagramsize  += headersize;
    }

    pak->if_output = NULL;

    /*
     * Increment our counters
     */
    acct_proto = pak->acct_proto;
    idb->counters.tx_cumbytes_fast[acct_proto] += pak->datagramsize;
    idb->counters.outputs_fast[acct_proto]++;


    hdrptr = (ulong *)header->datagramstart;
    ptr    = (ulong *)pak->datagramstart;
    
    /*
     * Write the IP header
     */
    PUTLONG(ptr++, *hdrptr++);            /* version, ihl, tos, tl  */
    PUTLONG(ptr++, *hdrptr++);            /* id, flags, fo          */
    PUTLONG(ptr++, *hdrptr++);            /* ttl, pid, checksum     */
    PUTLONG(ptr++, *hdrptr++);            /* source address         */
    PUTLONG(ptr++, *hdrptr++);            /* destination address    */

    /*
     * Write the GRE header
     */
    if (tinfo->mode == TUN_MODE_GRE_IP) 
	PUTLONG(ptr++, (ulong)gre_rxtype2type(pak->rxtype));

    /*
     * Fixup the dynamic fields in the IP header
     */
    ip = (iphdrtype *)ipheadstart(pak);

    PUTSHORT(&ip->id, tinfo->ident++);
    PUTSHORT(&ip->tl, pak->datagramsize);
    PUTSHORT(&ip->checksum, 0);
    PUTSHORT(&ip->checksum, ipcrc((ushort *)ip, ipheadsize(ip)));

    /* 
     * Make a note of the current input idb and packet size.
     */
    input_size = pak->datagramsize;

    /*
     * Attempt to send the frame via the IP f/s path
     * Change the if_input to match the IDB for the
     * tunnel interface. We do this so that we
     * will avoid CSCdi87500 where access-lists
     * are checked against the new GRE src address
     * but the orignal hwidb.
     */
    if (pak_has_particles(pak)){
	/*
	 * This case is for the 7200 platform which 
	 * at this time does not handle the call
	 * to change_if_input properly. 
	 */
        pak->if_input = tinfo->hwidb->firstsw;
    } else {
        if (!change_if_input(pak, tinfo->hwidb->firstsw)){
            datagram_done(pak);
            return;
        }
    }

    /*
     * Save a pointer to pak->if_input in case the RSP
     * fastsend routine returns a DRAM pak to the buffer pool.
     * This can happen if the the packet was encrypted.
     */
    input = pak->if_input;

    if (!reg_invoke_tunnel_fs(RXTYPE_DODIP, pak)) {
        /*
         * Could not switch the packet via the IP fast-switching path. Send
         * the packet to process level. If it is a particled packet, coalesce
         * it before doing so.
         */
        if (pak_has_particles(pak)) {
            
            newpak = input_getbuffer(pak->datagramsize, pak->if_input->hwptr);
            if (!newpak) {
                /*
                 * drop it - input_getbuffer() will have done drop accting.
                 */
                datagram_done(pak);
                return;
            }

            if (!pak_coalesce(pak, newpak)) {
                /*
                 * Something went wrong. Clean up.
                 */
                newpak->flags |= PAK_INPUTQ; /* got cleared in pak_coalesce */
                pak->if_input->hwptr->counters.input_drops++;
                datagram_done(newpak);
                datagram_done(pak);
                return;
            }
            newpak->flags |= PAK_INPUTQ;    /* got cleared in pak_coalesce */

            /*
             * return original pak
             */
            datagram_done(pak);
            pak = newpak;

        } else if (pak->buffer_flags & PAK_BUF_DO_NOT_Q) {

            /*
             * input pak is not queueable, e.g. RSP's fs_pak.  dup it.
             */
            newpak = input_getbuffer(pak->datagramsize, pak->if_input->hwptr);
            if (!newpak) {
            /*
             * drop it - input_getbuffer() will have done drop accting.
             */
                datagram_done(pak);
                return;
            }
            /*
             * dup and realign the pak,
             * will also set dirty bits for RSP
             */
            pak_copy_and_recenter(pak, newpak, pak->datagramsize);
            newpak->flags |= PAK_INPUTQ;        /* got cleared in pak_copy */
            datagram_done(pak);
            pak = newpak;
        }
	/*
	 * If the packet isn't word aligned, we're screwed badly.  We need to
	 * move it so the IP header starts on a 32 bit word aligned boundry,
	 * otherwise the process path will barf.
	 *
	 * This is likely an insanely slow reverse single byte copy
	 * through shared memory.  Ouch. Ouch. Ouch.
	 *
	 * The alternative is to fix all of the IP code to handle packets on
	 * any alignment.
	 *
	 * Note: This destroys part of the old encaps area.
	 */
	alignment = (u_long)pak->network_start & 0x03L;
	if (alignment) {
	    memmove(pak->network_start - alignment, pak->network_start,
		    pak->datagramsize);
	    pak->network_start -= alignment;
	    pak->datagramstart = pak->network_start;
	}

	/*
	 * No IP fast-switch entry for the tunnel destination. Queue
	 * the frame for the IP process
	 */
	ip_simple_enqueue(pak);
    } else {

	/*
	 * The packet was fastswitched. Decrement the counters to undo the
	 * double increment in the fastswitch path.
	 */
        input->hwptr->counters.inputs_fast[ACCT_PROTO_IP]--;
        input->hwptr->counters.rx_cumbytes_fast[ACCT_PROTO_IP] -= input_size;
    }
}


/*
 * tunnel_fast_supported
 *
 * Do we support fast tunneling on this interface?
 */

static boolean tunnel_fast_supported (int linktype, hwidbtype *idb)
{
    tunnel_info *tinfo;

    if (!is_tunnel(idb) || !idb->tunnel)
	return(FALSE);

    tinfo = idb->tunnel;

    if (!tinfo->fast_enabled)
	return(FALSE);
    
    /*
     * Is the interface we're interested in running GRE?
     */
    switch (tinfo->mode) {
    case TUN_MODE_GRE_IP:
	if (tinfo->enable_key || tinfo->enable_seq ||
	    tinfo->carry_ipso || tinfo->srclen)
	    return(FALSE);

	/*
	 * Only return TRUE for the protocols we really support
	 */
	switch (linktype) {
	  case LINK_APPLETALK:
	  case LINK_CLNS:
	  case LINK_DECNET:
	  case LINK_IP:
	  case LINK_NOVELL:
	  case LINK_VINES:
	  case LINK_XNS:
	    return(TRUE);
	    
	  default:
	    return(FALSE);
	}
	break;

    case TUN_MODE_IP_IP:
    case TUN_MODE_DVMRP_IP:
	return (linktype == LINK_IP);

    default:
	return(FALSE);
    }

    return(FALSE);
}


/*
 * gre_ipip_les_fs_tunnel_fast_setup
 *
 * Configure the fast parameters for the GRE tunnel
 */

static boolean gre_ipip_les_fs_tunnel_fast_setup (tunnel_info *tinfo)
{
    hwidbtype *idb = tinfo->hwidb;

    idb->fast_switch_type = FS_TUNNEL;
    idb->fastsend         = gre_ipip_fastsend;

    return(TRUE);
}


/*
 * tunnel_les_fs_init
 *
 * Start the tunnel f/s support up.
 */

void tunnel_les_fs_init (subsystype* subsys)
{
    reg_add_tunnel_fast_setup(TUN_MODE_GRE_IP,
			      gre_ipip_les_fs_tunnel_fast_setup,
			     "gre_ipip_les_fs_tunnel_fast_setup");
    reg_add_tunnel_fast_setup(TUN_MODE_DVMRP_IP,
			      gre_ipip_les_fs_tunnel_fast_setup,
			     "gre_ipip_les_fs_tunnel_fast_setup");
    reg_add_tunnel_fast_setup(TUN_MODE_IP_IP,
			      gre_ipip_les_fs_tunnel_fast_setup,
			     "gre_ipip_les_fs_tunnel_fast_setup");
    reg_add_tunnel_ip_les_fs(tunnel_ip_les_fastswitch,
				 "tunnel_ip_les_fastswitch");
    reg_add_fast_supported(tunnel_fast_supported, "tunnel_fast_supported");
}

/*
 * tunnel LES FS subsystem header
 */

#define TUNNELLESFS_MAJVERSION 1
#define TUNNELLESFS_MINVERSION 0
#define TUNNELLESFS_EDITVERSION  1

SUBSYS_HEADER(tunnel_les_fs,
	      TUNNELLESFS_MAJVERSION, TUNNELLESFS_MINVERSION,
	      TUNNELLESFS_EDITVERSION, tunnel_les_fs_init,
	      SUBSYS_CLASS_PROTOCOL, subsys_seq_iphost, "req: fastswitch, iphost");
