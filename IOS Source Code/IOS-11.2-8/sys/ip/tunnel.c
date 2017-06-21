/* $Id: tunnel.c,v 3.9.8.17 1996/09/04 00:54:42 tylin Exp $
 * $Source: /release/112/cvs/Xsys/ip/tunnel.c,v $
 *------------------------------------------------------------------
 * Tunnel Interface
 *
 * April, 1992 - Paul Traina
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Provide a virtual interface that acts like a point to point link
 * over IP.  Encapsulate packets using one of serveral methods
 *------------------------------------------------------------------
 * $Log: tunnel.c,v $
 * Revision 3.9.8.17  1996/09/04  00:54:42  tylin
 * CSCdi67974:  PIM-MIB returns wrong ifIndex for tunnel intf intro. by
 * CSCdi61338
 * Branch: California_branch
 *
 * Revision 3.9.8.16  1996/08/28  12:50:30  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.8.15  1996/08/09  23:44:40  sbelair
 * CSCdi62311:  Encrypted tunnels dont work
 * Branch: California_branch
 * In tunnel_oqueue, check to see if the gre packet needs to be encrypted
 * before being sent down the tunnel.
 *
 * Revision 3.9.8.14  1996/07/23  13:24:33  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.9.8.13  1996/07/06  05:54:11  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.9.8.12  1996/06/27  09:36:55  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.9.8.11  1996/06/17  23:32:41  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.9.8.10  1996/06/07  20:42:44  fred
 * CSCdi58655:  turning RED off on an interface turns WFQ off on that
 * interface
 *         save default setting and restore it when folks remove RED,
 *         Custom, or Priority Queuing
 * Branch: California_branch
 *
 * Revision 3.9.8.9  1996/05/21  09:51:42  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.9.8.8  1996/05/21  06:32:26  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.9.8.7  1996/05/08  20:55:58  lwei
 * CSCdi57099:  rsp-boot-m image can not be built due to the fix to
 * CSCdi56122
 * Branch: California_branch
 *
 * Revision 3.9.8.6  1996/05/08  01:47:02  lwei
 * CSCdi56122:  Tunnel interface default MTU should not show up in nvgen
 * output
 * Branch: California_branch
 *
 * Revision 3.9.8.5  1996/04/29  21:19:26  lwei
 * CSCdi55600:  GRE tunnel interface has wrong IP MTU
 * Branch: California_branch
 *
 * Revision 3.9.8.4  1996/04/29  05:23:25  pst
 * CSCdi55097:  Unaligned access in memd - possible IPX/tunneling
 * interaction
 * Branch: California_branch
 *
 * Revision 3.9.8.3  1996/04/10  04:23:00  bcole
 * CSCdi54192:  IPX pings over a multipoint GRE tunnel fail intermittently
 * Branch: California_branch
 * Fixup IPX encapsulation over multipoint tunnels.
 *
 * Revision 3.9.8.2  1996/03/23  01:33:26  slin
 * Branch: California_branch
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.9.8.1  1996/03/18  20:17:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.20.4  1996/03/16  06:59:49  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.20.3  1996/03/13  01:38:21  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.20.2  1996/03/07  09:47:16  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.20.1  1996/02/20  01:00:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/29  08:28:12  mleelani
 * CSCdi50179:  IP fastswitching of multicast over tunnel caused a reload
 * Tunnel state change causes race condition.
 *
 * Revision 3.8  1996/02/07  16:13:03  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.7  1996/02/01  06:04:34  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1995/11/26  08:31:23  gchristy
 * CSCdi42997:  Fast switching fails on Low End GRE tunnels
 * - Commit missing parts.
 * - Initialize fastsend to return_false() in case tfast_les subsystem
 *   is missing.
 *
 * Revision 3.5  1995/11/21  23:11:37  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.4  1995/11/20  22:48:31  snyder
 * CSCdi44202:  spellink errors
 *              Tunnelling -> Tunneling
 *
 * Revision 3.3  1995/11/17  09:37:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:41:07  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:59:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  21:05:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/10/18  17:20:11  wmay
 * CSCdi39845:  show interface displays serials in a non-intuitive manner
 * Sort high end idb's by slot/slotunit/vc, low end by unit/vc.
 *
 * Default slot and slotunit are changed to MAXUINT.  Virtual interfaces
 * (tunnel, loop, async) shouldn't change them, as parser is now smart
 * enough to use unit on high end.
 *
 * Revision 2.8  1995/10/13  20:24:08  dino
 * CSCdi42136:  Crash in ipmulticast_les_fastswitch() when tunnels go down
 *
 * Revision 2.7  1995/09/09  00:53:27  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.6  1995/09/02  10:05:10  fred
 * CSCdi39438:  no fair does not disable WFQ
 *              make sure that tunnels do not use fair queuing
 *
 * Revision 2.5  1995/08/25  03:24:27  myeung
 * CSCdi38715:  OSPF route change brings down tunnel line protocol
 * - Add blind timer to tolerate transient failure
 *
 * Revision 2.4  1995/06/28  09:27:03  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/21  03:29:01  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.2  1995/06/09  08:28:04  smackie
 * Move a NULL pointer check to prevent the router from tossing it's
 * cookies when a tunnel is configured. (CSCdi35576)
 *
 * Revision 2.1  1995/06/07  21:01:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "subsys.h"
#include "interface_private.h"
#include "snmp_interface.h"
#include "../snmp/ifmib_registry.h"
#include "packet.h"
#include "logger.h"
#include "../if/msg_tunnel.c"		/* Not a typo, see logger.h */
#include "../ui/debug.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_tunnel.h"
#include "../parser/parser_defs_parser.h"
#include "../ip/ip.h"
#include "../ip/ip_debug.h"
#include "../ip/udp_debug.h"
#include "../ip/gre.h"
#include "../if/if_vidb.h"
#include "../if/network.h"
#include "../parser/parser_defs_priority.h"
#include "../iprouting/route.h"

#include "../ip/tunnel.h"
#include "../ip/tunnel_debug.h"

#include "sys_registry.h"
#include "media_registry.h"
#include "../ip/ip_registry.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_registry.h"


static queuetype tunnel_pQ;		/* actual queue of tunnels */
queuetype *tunnelQ = &tunnel_pQ;	/* pointer to queue */


/*
 * tunnel_vencap
 *
 * Called by the IDB dispatch routines
 */

boolean tunnel_vencap (paktype *pak, long address)
{
    addrtype proto_addr;
    boolean broadcast;
    tunnel_info *tinfo;

    pak->enctype = ET_NULL;
    broadcast = FALSE;
    switch (pak->linktype) {
      case LINK_CLNS_ALL_ES:
      case LINK_CLNS_ALL_IS:
      case LINK_CLNS_BCAST:
      case LINK_ISIS_ALL_L1_IS:
      case LINK_ISIS_ALL_L2_IS:
	pak->linktype = LINK_CLNS;
	break;
    }

    pak->encsize    = 0;
    pak->mac_start  = NULL;
    pak->addr_start = NULL;
    pak->rif_start  = NULL;
    pak->riflen     = 0;
    pak->info_start = NULL;
    if (pak->if_output->subif_link_type == SUBIF_TYPE_MULTI) {
	/*
	 * Multipoint tunnel.  Try to resolve transport layer destination
	 */
	if (!reg_invoke_media_tunnel_macaddr(pak->linktype, pak, address,
					     &proto_addr, &broadcast)) {
	    tinfo = pak->if_output->hwptr->tunnel;
	    if (!tinfo->destination) {
		/*
		 * If packet is broadcast, may need to replicate GRE packet
		 * and send to other destinations.
		 */
		if (broadcast) {
		    if (!reg_invoke_media_tunnel_bcast(pak->linktype, pak)) {
			/*
			 * No mapping found
			 */
			return(FALSE);
		    }
		    return(TRUE);
		}
		return(FALSE);
	    }
	    pak->desthost = tinfo->destination;
	} else {
	    /*
	     * Hide transport layer tunnel destination address in payload
	     * packet so that tunnel can be multipoint.
	     */
	    pak->desthost = proto_addr.ip_addr;
	}
    }

    reg_invoke_fs_cache_update(pak->linktype, pak, 0L);

    return(TRUE);
}

/*
 * tunnel_oqueue
 *
 * Called by the IDB dispatch routines -- if this were a normal interface
 * we would put the packet on the appropriate output hold queue.  However,
 * since we don't need an output hold queue, this is the most convenient
 * place to steal the packet away from the normal code path.  We will take
 * the packet, pass it to the appropriate encapsulation routine, which will
 * create a new packet based upon the old (we don't muck with the old one
 * in case this packet is being flooded (udp flooding)) and then release
 * our interest in the old packet.
 *
 */

static boolean tunnel_oqueue (hwidbtype *idb, paktype *pak, enum HEADTAIL which)
{
    tunnel_info *tinfo = idb->tunnel;
    paktype	*new_pak, *header;
    iphdrtype   *new_ip;
    udptype	*udp;

    /*
     * output_if is reset every time we have a routing update so that
     * (a) we don't need to route each packet
     * (b) we can determine if a route to a particular destination is up
     * (c) we can punt recursive routing loops
     */
    new_pak = ((*tinfo->encaps)(tinfo, pak));
    if (new_pak) {
	if (pak->flags & PAK_HEARSELF)
	    new_pak->flags |= PAK_HEARSELF;

	switch (tinfo->transport) {
	case TUN_TRANS_IP_MULTI:
	    /*
	     * Use transport layer address resolved at encapsulation time.
	     */
	    ipwrite(new_pak, new_pak->datagramsize, tinfo->protocol,
		    TUNNEL_MYADDR(tinfo), pak->desthost, NULL, NOPT, NULL);
	    break;
	case TUN_TRANS_IP:
	    new_ip = (iphdrtype *)ipheadstart(new_pak);
	    header = tinfo->header;

	    if (!header->if_output)
		goto punt;

	    bcopy(header->datagramstart,
		  (uchar *)new_ip,
		  header->datagramsize);
	    new_pak->flags     |= (pak->flags & PAK_MULTICAST);
	    new_pak->desthost  = header->desthost;
	    new_pak->if_output = header->if_output;
	    new_ip->id	      = tinfo->ident++;
	    new_ip->tl        = new_pak->datagramsize;
	    new_ip->checksum  = 0;
	    new_ip->checksum  = ipcrc((ushort *)new_ip, ltob(new_ip->ihl));
	    if (pak->if_output && 
		pak->if_output->crypto_map &&
		!(pak->cryptoflags & CRYPTO_CM)) {
		if (!reg_invoke_crypto_map_check(new_pak, 
			pak->if_output->crypto_map, CRYPTO_ENCRYPT, FALSE) ||
                     crypto_deny_packet(pak))
		    goto punt;
      
	    }

	    if (new_ip->prot == UDP_PROT) {
		udp = (udptype *) ipdatastart(new_pak);
		if (tinfo->checksum)
		    udp->checksum = CheckSum((uchar *) udp, udp->length,
					     new_ip->srcadr, new_ip->dstadr,
					     UDP_PROT);

		if (udp_debug)
		    buginf("\nUDP: sent src=%i(%d), dst=%i(%d)",
			   new_ip->srcadr, udp->sport,
			   new_ip->dstadr, udp->dport);
	    }

	    if (tunnel_debug)
		buginf("\n%s: %s encapsulated %i->%i (linktype=%d, len=%d)",
		       idb->hw_namestring, tinfo->modestring,
		       new_ip->srcadr, new_ip->dstadr,
		       pak->linktype, new_pak->datagramsize);

	    if (ippkt_debug)
		ipbug(new_pak, "sending");

	    if (!ipsendnet(new_pak, FALSE, FALSE))
		goto punt;
	    break;
	}

	idb->counters.tx_cumbytes += pak->datagramsize;
	GET_TIMESTAMP(idb->lastoutput);

	datagram_done(pak);
	return TRUE;
    }

    /*
     * Failure
     */
  punt:
    if (new_pak)	/* if we didn't dispose of it, do so now */
      datagram_done(new_pak);

    idb->counters.output_total_drops++;
    datagram_done(pak);
    return FALSE;
}

/*
 * tunnel_null_encaps
 *
 * stub routine in case there is no protocol specific encapsulation routine
 * -- this should never actually be called
 */

static paktype *tunnel_null_encaps (tunnel_info *tinfo, paktype *pak)
{
    return NULL;
}

/*
 * tunnel_raw_enqueue
 *
 * Called by the protocol-specific decapsulation routine to do some
 * generic tunnel cleanup.  The decapsulation routine is responsible
 * for fixing up the data portion of the packet,  but some things
 * need to be cleaned up in the pak structure itself before calling
 * raw_enqueue (also, accounting needs to be done).  This should be
 * generic to all tunnel code.
 *
 */

static void tunnel_raw_enqueue (hwidbtype *idb, paktype *pak)
{
    /* stripped packet now needs to be checked (ttl et al) */
    pak->flags &= ~PAK_MCICHECKED;

    if (change_if_input(pak, idb->firstsw)) {
	idb->counters.inputs++;
	idb->counters.rx_cumbytes += pak->datagramsize;
	raw_enqueue(idb, pak);
    } else
	retbuffer(pak);
}

/*
 * tunnel_reset
 *
 * Something happened, restart sequencing if enabled and/or clear
 * packets if necessary.
 */

static void tunnel_reset (hwidbtype *idb)
{
    tunnel_info *tinfo = idb->tunnel;

    /* clear packets outbound on tunnel interface here */
    /* but nothing of this sort for now */

    (*tinfo->resync)(tinfo);
}

/*
 * tunnel_build_header
 *
 * When tunnel is running in IP or UDP datagram mode, we pre-compute
 * an IP header and store it in tinfo->header.  That way, we can just
 * bcopy the header onto the front of the packet and let it fly, rather
 * than incurring the overhead of ipwrite().
 */

static boolean tunnel_build_header (tunnel_info *tinfo)
{
    ipaddrtype my_address;
    paktype *header;
    iphdrtype *ip;
    idbtype *dest_idb, *swidb;

    header = tinfo->header;
    ip     = (iphdrtype *)ipheadstart(header);
    header->desthost = tinfo->destination;

    dest_idb = reg_invoke_ip_first_hop(&header->desthost, header->desthost,
				       IP_PATH_DEFFLAGS);

    /*
     * If we had a route, and then lost it,  this could be just
     * a transient failure.
     *
     * Set a blind timer during which time we'll do nothing.
     * If the timer has expired, and we're still hosed, down the interface.
     *
     * This is more complex that it should be because of timer aliasing
     * concerns.
     */
    if (!dest_idb && header->if_output) {
	if (TIMER_RUNNING(tinfo->blind_timer)) {
	    if (AWAKE(tinfo->blind_timer)) {
		TIMER_STOP(tinfo->blind_timer);
		reg_invoke_ip_adjust_tunnel_mroutes(tinfo->hwidb->firstsw);
		header->if_output = NULL;
	    }
	} else
	    TIMER_START(tinfo->blind_timer, TUNNEL_BLINDTIME);
	return TRUE;
    }
    TIMER_STOP(tinfo->blind_timer);

    /*
     * If we have a recursive fault, return false to alert upper layers
     */
    if (hwidb_or_null(dest_idb) == tinfo->hwidb) {
	reg_invoke_ip_adjust_tunnel_mroutes(tinfo->hwidb->firstsw);
	header->if_output = NULL;
	return FALSE;
    }

    /*
     * The real interface this tunnel goes out on may have changed.
     * Recalculate the tunnel idb's IP MTU.
     */
    if (dest_idb) {
	swidb = tinfo->hwidb->firstsw;
	swidb->ip_mtu = def_proto_mtu(dest_idb, LINK_IP) - IPHEADERBYTES(NOPT)
			- (ISGRE(tinfo->hwidb) ? GREHEADERBYTES(tinfo) : 0);
    }

    /*
     * Adjust mutlicast routing table if necessary
     */
    if (header->if_output != dest_idb)
	reg_invoke_ip_adjust_tunnel_mroutes(tinfo->hwidb->firstsw);

    header->if_output = dest_idb;

    if (!dest_idb)
	return TRUE;

    /* XXX this shit does NOT belong in this file */
    tinfo->hwidb->mci_index   = dest_idb->hwptr->mci_index;
    tinfo->hwidb->buffer_pool = dest_idb->hwptr->buffer_pool;
    tinfo->hwidb->mci_regptr  = dest_idb->hwptr->mci_regptr;

    my_address = TUNNEL_MYADDR(tinfo);

    ipbuild_head(header, 1, tinfo->protocol, my_address,
		 tinfo->destination, NULL, &ipdefaultparams);

    header->datagramstart = (char *)ip;
    header->datagramsize  = ltob(ip->ihl);

    return TRUE;
}

/*
 * tunnel_periodic
 *
 * Something happened, restart sequencing if enabled and/or clear
 * packets if necessary.  Also, check to see where we should be
 * routing packets and fixup the precomputed route and header.
 */

static void tunnel_periodic (hwidbtype *idb)
{
    tunnel_info *tinfo = idb->tunnel;

    if (!tinfo->destination || !systeminit_flag) {
	reg_invoke_ip_adjust_tunnel_mroutes(tinfo->hwidb->firstsw);
	tinfo->header->if_output = NULL;
	return;
    }

    if (!TIMER_RUNNING_AND_SLEEPING(tinfo->out_timer)) {
	if (!tunnel_build_header(tinfo)) {
	    TIMER_START(tinfo->out_timer, TUNNEL_STOMPTIME);
	    errmsg(&msgsym(RECURDOWN,TUN), idb->hw_namestring);
	} else
	    TIMER_START(tinfo->out_timer, TUNNEL_HEADCALCTIME);
    }
}

/*
 * tunnel_enable_fast_switching
 * Check fast switching, and if available, set it up.
 */
static void tunnel_enable_fast_switching (tunnel_info *tinfo)
{
    boolean old_enabled = tinfo->fast_enabled;

    tinfo->fast_capable = reg_used_tunnel_fast_setup(tinfo->mode);

    tinfo->fast_enabled = 
	 tinfo->fast_capable && !tinfo->enable_key &&
	 !tinfo->enable_seq  && !tinfo->carry_ipso &&
	 reg_invoke_tunnel_fast_setup(tinfo->mode, tinfo);

    if (old_enabled != tinfo->fast_enabled)
	reg_invoke_fast_setup(tinfo->hwidb);
}

/*
 * tunnel_setmode
 *
 * We set the enctype here, even though it's always ET_TUNNEL so far
 * because GRE is the only protocol to use the enctype (we fake out
 * get10mblink with it) so far, but others may later.
 *
 */

static void tunnel_setmode (idbtype *idb, int mode)
{
    hwidbtype   *hwidb = idb->hwptr;
    tunnel_info *tinfo = hwidb->tunnel;
    int		 enctype;
    linktype_t	 getlink;

    if (tinfo->mode == mode)
	return;

    if (tinfo->cstate)
	(*tinfo->cstate)(idb, FALSE);	 /* change state */

    tinfo->mode = mode;

    reg_invoke_tunnel_setmode(mode, tinfo, &getlink, &enctype);

    if (tinfo->cstate)
	(*tinfo->cstate)(idb, TRUE); /* change state */

    hwidb->getlink = getlink;
    hwidb->enctype = hwidb->enctype_def = idb->ip_enctype =
	               hwidb->enctype_cfg = enctype;
    reg_invoke_media_enctype_setup(idb, enctype);

    tunnel_enable_fast_switching(tinfo);

    tunnel_build_header(tinfo);

    (*tinfo->resync)(tinfo);
}

/*
 * tunnel_default_setmode
 *
 * Provide tunnel initializion with information for the
 * tinfo structure for unregistered routines
 *
 */
static void tunnel_default_setmode (tunnel_info *tinfo,
				    linktype_t *getlink, int *enctype)
{
    tinfo->encaps = tunnel_null_encaps;
    tinfo->resync = (void (*)(tunnel_info *))return_nothing;
    tinfo->cstate = NULL;

    tinfo->transport  = TUN_TRANS_IP;
    tinfo->protocol   = IPINIP_PROT;
    tinfo->modestring = "uninitialized";

    *getlink = (linktype_t) return_zero;
    *enctype = ET_TUNNEL;
}

/*
 * tunnel_default_fast_setup
 *
 * Set the default f/s parameters for tunneling. Which is to say, Off.
 */

static boolean tunnel_default_fast_setup (tunnel_info *tinfo)
{
    return(FALSE);
}

/*
 * tunnel_linestate
 * Return TRUE/FALSE for "interface_up" and friends
 * If we're a virtual tunnel, we can't be up if we don't know
 * how to get to our destination (or if we have no destination).
 * For a multipoint tunnel, we also can't be up until a key is defined.
 */

static boolean tunnel_linestate (hwidbtype *hwidb)
{
    tunnel_info *tinfo;

    if (!systeminit_flag)
	return(FALSE);

    if (!is_tunnel(hwidb))
	return(FALSE);

    tinfo = hwidb->tunnel;

    if (!tinfo || !(tinfo->source || tinfo->source_idb))
	return(FALSE);

    switch (tinfo->transport) {
    case TUN_TRANS_IP_MULTI:
	if (!tinfo->enable_key && tinfo->mode != TUN_MODE_IPTALK_UDP)
	    return(FALSE);
	break;
    case TUN_TRANS_IP:
	if (!tinfo->destination || !tinfo->header->if_output)
	    return(FALSE);
	break;
    }

    /*
     * If we're using someone else's interface as the source address
     * for our packets, then we can't receive anything unless they're
     * up, so we might as well go down.
     */
    if (tinfo->source_idb && !interface_up_simple(tinfo->source_idb))
	return(FALSE);

    return(TRUE);
}

/*
 * Display information specific to tunnel interfaces.
 * (call from show_interface)
 */

static void tunnel_show (hwidbtype *idb)
{
    tunnel_info *tinfo;

    if (!is_tunnel(idb) || !idb->tunnel)
      return;

    tinfo = idb->tunnel;

    if (tinfo->source_idb)
      printf("\n  Tunnel source %i (%s)", tinfo->source_idb->ip_address,
	     tinfo->source_idb->namestring);
    else
      printf("\n  Tunnel source %i", tinfo->source);

    printf(", destination %i", tinfo->destination);
    printf("\n  Tunnel protocol/transport %s, key ", tinfo->modestring);
    if (tinfo->enable_key)
      printf("0x%x", tinfo->key);
    else
      printf("disabled");

    if (!tinfo->enable_seq)
      printf(", sequencing disabled");
    else
      printf("\n  Order sequence numbers %u/%u (tx/rx)",
	     tinfo->t_sequence, tinfo->r_sequence);

    printf("\n  Checksumming of packets %sabled",
	   tinfo->checksum ? "en" : "dis");

    if (tinfo->fast_capable)
	printf(",  fast tunneling %sabled",
	       tinfo->fast_enabled ? "en" : "dis");

    if (tinfo->carry_ipso)
      printf("\n  IP security options are inherited from source packet.");
}

/*
 * tunnel_extract_addr
 *
 * Given a TUNNEL packet, extract the requested 'MAC' level address
 * into a hwaddrtype structure.
 */
static boolean tunnel_extract_addr (paktype *pak, hwaddrtype *address,
				    enum SRCDST which)
{
    address->type = STATION_CISCO_TUNNEL;
    address->length = STATIONLEN_CISCO_TUNNEL;
    address->target_type = TARGET_UNICAST;
    address->addr[0] = 0;
    return(TRUE);
}

/*
 * tunnel_createidb
 * Create an IDB on the fly when it is first referenced by an
 * "interface tunnel N" command.
 *
 */

static idbtype *tunnel_createidb (uint unit)
{
    idbtype     *swidb;
    hwidbtype   *idb;
    tunnel_info *tinfo;
    int		 bytes;

    /*
     * If they insist on actually using the tunnel, give them some
     * debugging flags
     */
    tunnel_debug_init();

    /*
     * don't put tunnel on normal in-use queue, but rather use special
     * vidb_inuseQ so we can clean up later.
     */
    swidb = vidb_malloc();
    if (!swidb) {
	printf(nomemory);
	return(NULL);
    }
    idb = swidb->hwptr;

    tinfo = malloc(sizeof(tunnel_info));
    if (!tinfo) {
	printf(nomemory);
	return(NULL);
    }

    tinfo->hwidb = idb;

    bytes = IPHEADERBYTES(NOPT);
    tinfo->header = getbuffer(bytes);
    if (!tinfo->header) {
	free(tinfo);
	return(NULL);
    }
    tinfo->header->datagramsize = bytes;

    idb->tunnel   = tinfo;
    idb->status  |= IDB_TUNNEL | IDB_BRIDGE;
    idb->type     = IDBTYPE_UNKNOWN;	/* fully defined by status bits */
    idb->typestring = "Tunnel";
    idb->state	  = IDBS_UP;
    idb->name     = IFNAME_TUNNEL;
    idb->unit     = unit;
    idb->fast_switch_type = FS_TUNNEL;
    idb_init_names(swidb, TRUE);

    /* some lies to stave off routing loops -- we don't know what our
       bandwith really is, so say we have a bad link */

    set_default_interface_bandwidth(swidb, VISIBLE_TUNNEL_BANDWIDTH);
    set_default_interface_delay(swidb, TUNNEL_DELAY);
    idb->reliability = 255;			/* good reliability */
    idb->load = 1;				/* poor loading */
    idb->fastsend = (fastsend_t) return_false;

    tunnel_setmode(swidb, TUN_MODE_DEFAULT);
    idb->iqueue	  = raw_enqueue;
    idb->vencap   = tunnel_vencap;
    idb->deencap  = NULL;
    idb->oqueue   = tunnel_oqueue;
    idb->oqueue_dequeue = (oqueue_dequeue_t)return_null;
    idb->soutput  = (soutput_t)return_nothing;
    idb->extract_hwaddr = invalidate_hwaddr;
    idb->extract_snpa = tunnel_extract_addr;
    idb->broadcast= (broadcast_t)return_fuz_false;/*no link level broadcasts */
    idb->reset    = tunnel_reset;
    idb->periodic = tunnel_periodic;
    idb->linestate = tunnel_linestate;
    idb->show_support = tunnel_show;
    idb->hw_if_index = idb_get_next_hw_index();
    idb->snmp_if_index = idb->firstsw->snmp_if_index = reg_invoke_ifmib_register_hwidb(idb);
    idb->old_queue_algorithm = idb->queue_algorithm = QUEUE_FCFS;

    /* interface now exists, put it on the idb queue */
    /* do this last so idb->tunnel is always valid */

    idb_enqueue(idb);
    p_enqueue(tunnelQ, tinfo);

    nnets++;				/* increment total # of interfaces */

    return(swidb);
}

static boolean tunnel_parser_createidb (uint64 *ifcode, parseinfo *csb)
{
    if (*ifcode == IFTYPE_TUNNEL) {
	csb->createidb = tunnel_createidb;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * Parse tunnel interface commands
 *
 *	     interface tunnel 0
 *	     ip address n.n.n.n
 *	[no] tunnel destination <n.n.n.n|hostname>
 *	[no] tunnel carry-security
 *	[no] tunnel key <key>
 *	[no] tunnel mode <proto> <transport>
 *	[no] tunnel sequencing
 *	[no] tunnel source <n.n.n.n|interface>
 */

void tunnel_cmd (parseinfo *csb)
{
    hwidbtype   *idb;
    idbtype     *swidb, *sidb;
    tunnel_info *tinfo;
    boolean	sense;
    long	addr;

    swidb = csb->interface;
    sense = csb->sense;

    if (!swidb)
	return;

    idb = swidb->hwptr;
    if (!is_tunnel(idb)) {
	if (!csb->nvgen)
	    printf("\nonly valid on tunnel interfaces");
	return;
    }

    tinfo = idb->tunnel;

    if (csb->nvgen) {
	if (!is_tunnel(idb) || !idb->tunnel)
	  return;
	switch (csb->which) {
	  case TUNNEL_CMD_CHECKSUM:
	    if (tinfo->checksum_default) {
		nv_write(!tinfo->checksum, "no %s", csb->nv_command);
	    } else {
		nv_write(tinfo->checksum, "%s", csb->nv_command);
	    }
	    break;
	  case TUNNEL_CMD_CARRY_SEC:
	    nv_write(tinfo->carry_ipso, "%s", csb->nv_command);
	    break;
	  case TUNNEL_CMD_DESTINATION:
	    nv_write(tinfo->destination, "%s %i",
		     csb->nv_command, tinfo->destination);
	    break;
	  case TUNNEL_CMD_KEY:
	    nv_write(tinfo->enable_key, "%s %u", csb->nv_command, tinfo->key);
	    break;
	  case TUNNEL_CMD_MODE:
	    if (tinfo->mode == GETOBJ(int,1))
		nv_write(tinfo->mode != TUN_MODE_DEFAULT, "%s",
			 csb->nv_command);
	    break;
	  case TUNNEL_CMD_SEQUENCING:
	    nv_write(tinfo->enable_seq, "%s", csb->nv_command);
	    break;
	  case TUNNEL_CMD_SOURCE:
	    nv_write(tinfo->source, "%s %i", csb->nv_command, tinfo->source);
	    if (tinfo->source_idb)
		nv_write(TRUE, "%s %s", csb->nv_command,
			 tinfo->source_idb->namestring); 
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }
	
    switch (csb->which) {
      case TUNNEL_CMD_CARRY_SEC:
	tinfo->carry_ipso = sense;
	break;

      case TUNNEL_CMD_CHECKSUM:
	if (csb->set_to_default)
	    tinfo->checksum = tinfo->checksum_default;
	else
	    tinfo->checksum = sense;
	break;

      case TUNNEL_CMD_DESTINATION:
	if (!sense) {
	    tinfo->destination = 0L;	 /* will stop all routing to dest */
	    break;
	}
	addr = GETOBJ(paddr,1)->ip_addr;
	if (martian(addr, TRUE)) {
	    printf("Not a valid host address - %i", addr);
	    break;
	}
	if (sense && tinfo->destination != addr) {
	    tinfo->destination = addr;
	    tunnel_reset(idb);
	}
	break;

      case TUNNEL_CMD_KEY:
	tinfo->key = GETOBJ(int,1);
	if (tinfo->enable_key != sense)
	    tinfo->enable_key = sense;
	break;

      case TUNNEL_CMD_MODE:
	if (sense) {
	    if (tinfo->mode != GETOBJ(int,1))
		tunnel_setmode(swidb, GETOBJ(int,1));
	} else
	    tunnel_setmode(swidb, TUN_MODE_DEFAULT);
	break;

      case TUNNEL_CMD_SEQUENCING:
	if (tinfo->enable_seq != sense)
	    tinfo->enable_seq = sense;
	(*tinfo->resync)(tinfo);
	break;

      case TUNNEL_CMD_SOURCE:
	if (!sense) {
	    tinfo->source     = 0L;
	    tinfo->source_idb = NULL;
	} else {
	    sidb = GETOBJ(idb,1);
	    if (!sidb) {
		addr = GETOBJ(paddr,1)->ip_addr;
		if (martian(addr, TRUE)) {
		    printf("Not a valid host address - %i", addr);
		    break;
		}
		if (tinfo->source != addr) {
		    tinfo->source = addr;
		    tinfo->source_idb = NULL;	/* static source address now */
		    tunnel_reset(idb);
		}
	    } else {
		if (swidb == sidb) {
		    printf("Cannot use self");
		    break;
		}
		if (tinfo->source_idb != sidb) {
		    tinfo->source = 0L;
		    tinfo->source_idb = sidb;
		    tunnel_reset(idb);
		}
	    }
	}
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;

    } /* end switch */

    tunnel_enable_fast_switching(tinfo);
    tunnel_build_header(tinfo);
}

/*
 * ip_get_tunnel_mtu
 *
 * Returns tunnel MTU. It is the size of the tunnel output interface
 * MTU minus extra IP header and possibly another encapsulation header.
 *
 * But if the tunnel output interface is not available at this point,
 * just return the MTU of the tunnel hwidb.
 */
static ulong ip_get_tunnel_mtu (idbtype *idb)
{
    tunnel_info *tinfo;
    idbtype *oidb;
    ulong def_mtu, adjust;

    adjust = 0;
    oidb = idb;

    if (idb->hwptr && (idb->hwptr->status & IDB_TUNNEL) &&
	(idb->hwptr->tunnel) && (idb->hwptr->tunnel->header->if_output)) {
	tinfo = idb->hwptr->tunnel;

	/*
	 * Tunnel interface MTU really depends on the output interface
	 */
	oidb = tinfo->header->if_output;
	
	if (tinfo->mode == TUN_MODE_GRE_IP) {
	    adjust = GREHEADERBYTES(tinfo);
	}
	adjust += IPHEADERBYTES(NOPT);
    }

    def_mtu = oidb->hwptr->max_proto_mtu ?
	      (*oidb->hwptr->max_proto_mtu)(oidb, LINK_IP, TRUE) :
	      oidb->hwptr->maxmtu;
    def_mtu -= adjust;

    return(def_mtu);
}

/*
 * tunnel_init
 * Initialize the tunnel interface handling
 */

static void tunnel_init (subsystype *subsys)
{
    queue_init(tunnelQ, 0);

    reg_add_create_idb(tunnel_parser_createidb, "tunnel_parser_createidb");
    reg_add_tunnel_raw_enqueue(tunnel_raw_enqueue, "tunnel_raw_enqueue");
    reg_add_default_tunnel_setmode(tunnel_default_setmode,
				   "tunnel_default_setmode");
    reg_add_default_tunnel_fast_setup(tunnel_default_fast_setup,
				      "tunnel_default_fast_setup");
    reg_add_media_tunnel_mtu(LINK_IP, ip_get_tunnel_mtu, "ip_get_tunnel_mtu");

    tunnel_parser_init();
}

/*
 * Tunnel subsystem header
 */

#define TUNNEL_MAJVERSION 1
#define TUNNEL_MINVERSION 0
#define TUNNEL_EDITVERSION 1

SUBSYS_HEADER(tunnel, TUNNEL_MAJVERSION, TUNNEL_MINVERSION,
	      TUNNEL_EDITVERSION, tunnel_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      subsys_req_iphost);
