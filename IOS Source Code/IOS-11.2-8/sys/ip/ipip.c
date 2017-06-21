/* $Id: ipip.c,v 3.2.60.4 1996/07/06 05:53:48 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipip.c,v $
 *------------------------------------------------------------------
 * Raw IP over IP encapsulation
 *
 * Feburary 1995, Paul Traina
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ipip.c,v $
 * Revision 3.2.60.4  1996/07/06  05:53:48  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.2.60.3  1996/06/27  09:36:51  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.2.60.2  1996/06/18  01:45:43  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  20:17:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:46:37  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  14:26:00  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:35:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:26:49  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:59:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "packet.h"
#include "interface_private.h"
#include "subsys.h"

#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"

#include "../ip/tunnel.h"
#include "../ip/tunnel_debug.h"

/*
 * ipip_type2link
 *
 * Called by raw_enqueue to determine where to send the packet we just
 * decapsulated for us.  For us, it's always IP.
 */

static long ipip_type2link(paktype *pak)
{
    if (pak)
      return LINK_IP;
    else
      return LINK_ILLEGAL;
}

/*
 * ipip_encaps
 *
 * Called by the tunnel_oqueue routine.  This spoofs the output queue
 * on the tunnel interface.  Instead of putting packet on output hold
 * queue,  build a new packet from scratch and copy the existing packet
 * data into the new packet after the IP header.
 *
 * We are not responsible for releasing the old packet -- our caller
 * will do that (caller may need old packet for statistics).
 *
 */

static paktype *ipip_encaps (tunnel_info *tinfo, paktype *pak)
{
    paktype	 *new_pak;
    unsigned	  bytes;

    if (pak->linktype != LINK_IP)
      return NULL;

    bytes = tinfo->header->datagramsize + pak->datagramsize;
    new_pak = getbuffer(bytes);
    if (!new_pak)
      return NULL;

    (void) bcopy(pak->datagramstart,
		 (uchar *)ipheadstart(new_pak) + tinfo->header->datagramsize,
		 pak->datagramsize);

    /* sneak some information upstairs... */
    new_pak->datagramsize = bytes;

    return(new_pak);
}

/*
 * ipip_decaps
 *
 * Decapsulate an packet passed down from IP processing
 *
 * Determine which tunnel packet is destined for, if packet is good,
 * move data following IP header down to start of packet data area and
 * enqueue for native processing.
 */

static void ipip_decaps (paktype *pak)
{
    iphdrtype *ip;
    hwidbtype	*idb;
    tunnel_info	*tinfo;

    ip = (iphdrtype *)ipheadstart(pak);

    ip_traffic.ipip_inputs++;	/* ipip tunnel encaps */
    FOR_ALL_TUNNELS(tinfo)
	if (((tinfo->mode == TUN_MODE_IP_IP) ||
	     (tinfo->mode == TUN_MODE_NOS_IP)||
	     (tinfo->mode == TUN_MODE_DVMRP_IP)) &&
	    (ip->srcadr == tinfo->destination) &&
	    (tinfo->hwidb->state == IDBS_UP))
	    break;

    if (!tinfo || !tinfo->hwidb) {
	datagram_done(pak);
	return;
    }

    idb = tinfo->hwidb;

    if (tunnel_debug)
      buginf("\n%s: to decaps IP/IP packet %i->%i (len=%d, ttl=%d)",
	   idb->hw_namestring, ip->srcadr, ip->dstadr, ip->tl, ip->ttl);

    pak->enctype       = ET_TUNNEL;
    pak->network_start = ipdatastart(pak);
    ip = (iphdrtype *)ipheadstart(pak);

    pak->datagramstart = pak->network_start;
    pak->datagramsize  = ip->tl;

    if (tunnel_debug)
	buginf("\n%s: decapsulated IP/IP packet %i->%i (len=%d ttl=%d)",
	       idb->hw_namestring, ip->srcadr, ip->dstadr,
	       pak->datagramsize, ip->ttl);

    reg_invoke_tunnel_raw_enqueue(idb, pak);
}

/*
 * ipip_setmode
 *
 * Provide tunnel initialization with information for the
 * tinfo structure 
 *
 */

static void ipip_setmode (tunnel_info *tinfo,
			  linktype_t *getlink, int *enctype)
{
    tinfo->encaps = ipip_encaps;
    tinfo->resync = (void (*)(tunnel_info *))return_nothing;
    tinfo->cstate = NULL;

    tinfo->transport = TUN_TRANS_IP;

    switch (tinfo->mode) {
    case TUN_MODE_IP_IP:
	tinfo->protocol   = IPINIP_PROT;
	tinfo->modestring = "IP/IP";
	break;
    case TUN_MODE_NOS_IP:
	tinfo->protocol   = NOSIP_PROT;
	tinfo->modestring = "IP/IP (NOS)";
	break;
    case TUN_MODE_DVMRP_IP:			/* braindead abortion */
	tinfo->protocol   = IPINIP_PROT;
	tinfo->modestring = "IP/IP (DVMRP)";
        break;
    default:			/* this should never happen */
	tinfo->protocol   = IPINIP_PROT;
	tinfo->modestring = "IP/IP";
	buginf("\nIP: ipip_setmode (invalid tunnel mode %d selected)",
		tinfo->mode);
    }

    *getlink = ipip_type2link;
    *enctype = ET_TUNNEL;
}

/*
 * ipip_init
 *
 * Perform IPIP registration
 */

static void ipip_init (subsystype *subsys)
{
    reg_add_tunnel_setmode(TUN_MODE_NOS_IP,   ipip_setmode, "ip_ip_setmode");
    reg_add_tunnel_setmode(TUN_MODE_IP_IP,    ipip_setmode, "ip_ip_setmode");
    reg_add_tunnel_setmode(TUN_MODE_DVMRP_IP, ipip_setmode, "ip_ip_setmode");
    reg_add_ip_enqueue(NOSIP_PROT,  ipip_decaps, "ipip_decaps");
    reg_add_ip_enqueue(IPINIP_PROT, ipip_decaps, "ipip_decaps");
}


/*
 * IP IP subsystem header
 */

#define IPIP_MAJVERSION 1
#define IPIP_MINVERSION 0
#define IPIP_EDITVERSION  1

SUBSYS_HEADER(ipip, IPIP_MAJVERSION, IPIP_MINVERSION,
	      IPIP_EDITVERSION, ipip_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      "req: iphost, tunnel");
