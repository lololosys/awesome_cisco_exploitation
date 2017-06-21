/* $Id: at_iptalk.c,v 3.3.60.4 1996/07/10 22:18:23 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_iptalk.c,v $
 *------------------------------------------------------------------
 * atalk_iptalk.c	- IP encapsulation for Appletalk
 *
 * March 1991, Charles Hedrick
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_iptalk.c,v $
 * Revision 3.3.60.4  1996/07/10  22:18:23  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.3.60.3  1996/06/27  09:21:19  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.3.60.2  1996/03/23  01:26:52  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.3.60.1  1996/03/18  18:52:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/13  01:06:51  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.2  1996/03/07  08:28:58  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/01/24  21:09:36  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  08:43:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:05:37  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:57:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:50:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/09  00:50:49  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.2  1995/06/21  02:50:47  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  20:08:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 **************************************************************************
 *   IPTalk								  *
 *   NB: This is not a full KIP implementation.  KIP has its own routing, *
 *   that depends upon static tables of all KIP hosts.  We do not approve *
 *   of that strategy.  The purpose of this IPTalk implementation is not  *
 *   to let us play in the KIP world, but rather to let us talk to Unix   *
 *   systems that have the public-domain CAP software.  Thus we treat     *
 *   IPTalk as a local encapsulation, but do routing is usual.  The       *
 *   current implementation has some limits:				  *
 *    1) Interface status of the actual interface is probably not         *
 *       available to Atalk.						  *
 *    2) Routing is not done via IPtalk, since the dummy interfaces are   *
 *       not on the idbq.						  *
 **************************************************************************
*/


#include "master.h"
#include "interface_private.h"
#include "../ip/ip_registry.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../atalk/atalk_private.h"
#include "at_registry.h"
#include "../ip/tunnel.h"
#include "../ip/ipaddress.h"

#define ATALK_PORT(p) \
        (((unsigned)((p) - 16512) < 128) || \
         ((unsigned)((p) - 200) < 128) || \
         ((unsigned)((p) - 768) < 128))

long	 atalk_iptalk_baseport;	/* base UDP port number */

/*
 * iptalk_cstate
 *
 * Called by tunnel_setmode to change state of tunnel.
 */
static void
iptalk_cstate (idbtype *swidb, boolean newstate)
{
    if (newstate)
	swidb->subif_link_type = SUBIF_TYPE_MULTI;
}

/*
 * iptalk_udp_type2link
 *
 * Called by raw_enqueue to determine where to send the packet we just
 * decapsulated.
 */
static long iptalk_udp_type2link (paktype *pak)
{
    if (pak)
	return LINK_APPLETALK;
    else
	return LINK_ILLEGAL;
}

/*
 * iptalk_udp_encaps
 *
 * Called by the tunnel_oqueue routine.  This spoofs the output queue
 * on the tunnel interface.  Instead of putting the packet on output hold
 * queue, build a new packet from scratch, create a UDP header, and copy
 * the existing packet data into the new packet after the UDP header.
 *
 * We are not responsible for releasing the old packet -- our caller
 * will do that (caller may need old packet for statistics).
 *
 */
static paktype *
iptalk_udp_encaps (tunnel_info *tinfo, paktype *pak)
{
    paktype *new_pak;
    udptype *udp;
    unsigned bytes;
    int dsock = atalk_pak_dsock(pak);
    int ssock = atalk_pak_ssock(pak);
    
    if (pak->linktype != LINK_APPLETALK)
	return NULL;

    /* bytes = ipheader + udpheader + ddp pkt */
    bytes = IPHEADERBYTES(NOPT) + UDPHEADERBYTES + pak->datagramsize;
    new_pak = getbuffer(bytes);
    if (!new_pak)
	return NULL;

    udp = (udptype *) (ipheadstart(new_pak) + IPHEADERBYTES(NOPT));
    if (ssock < 128)
	udp->sport = atalk_iptalk_baseport + ssock;
    else
	udp->sport = 16384 + ssock;
    if (dsock < 128)
	udp->dport = atalk_iptalk_baseport + dsock;
    else
	udp->dport = 16384 + dsock;

    udp->length = UDPHEADERBYTES + pak->datagramsize;
    udp->checksum = 0;

    /* copy old packet into place -- tunnel_encaps sets DG start & size */
    memmove((uchar *) udp->udpdata, pak->datagramstart, pak->datagramsize);

    new_pak->datagramsize = bytes;
    return new_pak;
    
}

/*
 * iptalk_udp_decaps
 *
 * Decapsulate a packet passed down from IP processing
 *
 * Determine which tunnel packet is destined for.
 *
 * Finally, if packet is good, move the data following the IP and UDP
 * headers up to the start of the packet data area and enqueue the
 * packet for native processing.
 *
 * Returns TRUE if packet is absorbed, FALSE otherwise.
 * Called by SERVICE_IP_UDP_INPUT.
 */
static boolean
iptalk_udp_decaps (paktype *pak, udptype *udp)
{
    iphdrtype *ip;
    hwidbtype *idb;
    tunnel_info *tinfo;
    ddptype *ddp;
    atalkidbtype *atalkidb;
    char const * idb_name;

    idb_name = idb_get_namestring(pak->if_input);
    
    ip = iphdrtype_start(pak);

    if ((!ATALK_PORT(udp->sport)) && (!ATALK_PORT(udp->dport)))
	return FALSE;

    FOR_ALL_TUNNELS(tinfo)
	if (tinfo->mode == TUN_MODE_IPTALK_UDP &&
	    ((tinfo->source_idb && tinfo->source_idb == pak->if_input) ||
	     (tinfo->source &&
	      ipaddr_findidb(tinfo->source) == pak->if_input)) &&
	    tinfo->hwidb->state == IDBS_UP)
	    break;

    if (!tinfo || !tinfo->hwidb) {
        /*
	 * Allow other a chance at it
	 */
        return FALSE;
    }

    if (tunnel_debug)
	buginf("\n%s: IPTalk to decaps %i->%i (len=%d ttl=%d)",
	       idb_name, ip->srcadr, ip->dstadr, ip->tl, ip->ttl);

    idb = tinfo->hwidb;

    /* Strip off IP/UDP header */
    ddp = (ddptype *) ((uchar *) udp + UDPHEADERBYTES + LLAP_ENCAPBYTES);

    pak->enctype = ET_TUNNEL;
    pak->network_start = (uchar *) ddp;
    pak->datagramstart = pak->network_start;
    pak->datagramsize = udp->length - UDPHEADERBYTES;

    atalkidb = atalk_getidb(idb->firstsw);
    *(pak->network_start - LLAP_ENCAPBYTES + LLAP_DSTNODE) =
	atalkidb->atalknode;

    if (tunnel_debug)
	buginf("\n%s: IPTalk decapsulated (len=%d)",
	       idb_name, pak->datagramsize);
    reg_invoke_tunnel_raw_enqueue(idb, pak);
    return TRUE;
}

/*
 * iptalk_udp_setmode
 *
 * Provide tunnel initialization with information for the tinfo structure
 *
 */
static void
iptalk_udp_setmode (tunnel_info *tinfo, linktype_t *getlink, int *enctype)
{
    tinfo->encaps = iptalk_udp_encaps;
    tinfo->resync = (void (*)(tunnel_info *)) return_nothing;
    tinfo->cstate = iptalk_cstate;

    tinfo->transport = TUN_TRANS_IP_MULTI;
    tinfo->protocol = UDP_PROT;
    tinfo->modestring = "IPTalk";

    *getlink = iptalk_udp_type2link;
    *enctype = ET_TUNNEL;
}

static void
show_iptalk_globals (void)
{
    if (atalk_iptalk_baseport != 0) {
	printf("\n  IPTalk uses the udp base port of %d",
	       atalk_iptalk_baseport);
	if (atalk_iptalk_baseport == ATALK_IPTALK_BASEPORT_DEFAULT)
	    printf(" (Default).");
	else printf(".");
    }
}

static void
show_iptalk_interface_private (atalkidbtype *idb)
{
    if (atalkif_IPTalkTunnel(idb))
	printf("\n  IPTalk information for %s, IP %i",
	       idb_get_namestring(idb->phys_idb),
	       idb_sw_get_ipaddress(idb->phys_idb));
}

static boolean
show_iptalk_interface_special (atalkidbtype *idb)
{
    show_iptalk_interface_private(idb);
    return FALSE;
}

/*
 * iptalk_init
 *
 * Perform iptalk registration
 *
 */
void
iptalk_init (void)
{
    atalk_iptalk_baseport = ATALK_IPTALK_BASEPORT_DEFAULT;

    reg_add_atalk_show_globals(show_iptalk_globals, "show_iptalk_globals");
    
    reg_add_ip_udp_input(iptalk_udp_decaps, "iptalk_udp_decaps");
    reg_add_tunnel_setmode(TUN_MODE_IPTALK_UDP, iptalk_udp_setmode,
			   "iptalk_udp_setmode");
    reg_add_atalk_show_private_interface(show_iptalk_interface_private, "");
    reg_add_atalk_show_interface_special(show_iptalk_interface_special, "");
}


