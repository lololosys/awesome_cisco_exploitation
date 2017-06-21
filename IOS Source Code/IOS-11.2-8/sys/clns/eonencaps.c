/* $Id: eonencaps.c,v 3.2.60.3 1996/06/27 09:24:41 fox Exp $
 * $Source: /release/112/cvs/Xsys/clns/eonencaps.c,v $
 *------------------------------------------------------------------
 * EON compatible CLNS over IP tunneling
 *
 * November 1992, Paul Traina
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: eonencaps.c,v $
 * Revision 3.2.60.3  1996/06/27  09:24:41  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.2.60.2  1996/06/18  01:43:14  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:03:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:40:31  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:40:32  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:53:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:08:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:22:56  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:18:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "subsys.h"
#include "packet.h"
#include "interface_private.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../ip/tunnel.h"

#include "../clns/clns.h"
#include "../clns/clns_externs.h"


				/* vers 1, unicast, fletcher checksum */
#define	EON_SNACP_LEN		4
#define	EON_SNACP_COOKIE	0x0100fc02

typedef struct eon_header_ {
    ulong snacp;
    uchar data[0];
} eon_header;


/*
 * eon_ip_type2link
 *
 * Called by raw_enqueue to determine where to send the packet we just
 * decapsulated for us.  For us, it's always CLNP.
 */

static long eon_ip_type2link(paktype *pak)
{
    if (pak)
      return LINK_CLNS;
    else
      return LINK_ILLEGAL;
}

/*
 * eon_ip_encaps
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

static paktype *eon_ip_encaps (tunnel_info *tinfo, paktype *pak)
{
    paktype	 *new_pak;
    unsigned	  bytes;
    eon_header	 *eon;

    if (pak->linktype != LINK_CLNS)
	return NULL;

    /*
     * Due to the fudge byte, pak->datagramsize was incremented
     * by one when clns_encaps() called (idb->encaps)(), so now we
     * need to move things back so the first byte going out is the pid
     */

    bytes = tinfo->header->datagramsize + pak->datagramsize +
	sizeof(eon_header);
    new_pak = getbuffer(bytes);
    if (!new_pak)
      return NULL;

    eon = (eon_header *) ((uchar *)ipheadstart(new_pak) +
			  tinfo->header->datagramsize);

    eon->snacp = EON_SNACP_COOKIE;

    (void) bcopy(pak->datagramstart, eon->data, pak->datagramsize);

    /* sneak some information upstairs... */
    new_pak->datagramsize = bytes;

    return(new_pak);
}

/*
 * eon_ip_decaps
 *
 * Decapsulate an packet passed down from IP processing
 *
 * Determine which tunnel packet is destined for, if packet is good,
 * move data following IP header down to start of packet data area and
 * enqueue for native processing.
 */

static void eon_ip_decaps (paktype *pak)
{
    iphdrtype   *ip;
    clnshdrtype *clns;
    hwidbtype	*idb;
    tunnel_info	*tinfo;
    int		 spot;

    ip = (iphdrtype *)ipheadstart(pak);

    ip_traffic.eonip_inputs++;	/* EON CLNS over IP tunnel encaps */
    FOR_ALL_TUNNELS(tinfo)
	if ((tinfo->mode == TUN_MODE_EON_IP) &&
	    (ip->srcadr == tinfo->destination) &&
	    (tinfo->hwidb->state == IDBS_UP))
	    break;

    if (!tinfo || !tinfo->hwidb){
	datagram_done(pak);
	return;
    }

    idb = tinfo->hwidb;

    if (tunnel_debug)
      buginf("\n%s: to decaps EON/IP packet %i->%i (len=%d, ttl=%d)",
	   idb->hw_namestring, ip->srcadr, ip->dstadr, ip->tl, ip->ttl);

    pak->enctype       = ET_TUNNEL;
    pak->network_start = ipdatastart(pak) + sizeof(eon_header);

    pak->datagramstart = pak->network_start;
    pak->datagramsize  = ip->tl - (ip->ihl << 2) - sizeof(eon_header);

    clns = (clnshdrtype *)clnsheadstart(pak);

    if (tunnel_debug) 
	if (clns->pid == NLP_ISO8473) {
	    spot = clns_set_pointers(pak);
	    if (spot) {
		buginf("\n%s: decapsulated EON/IP ISO8473 packet"
		       " with bad syntax at %d", idb->hw_namestring, spot);
	    } else {
		buginf("\n%s: decapsulated EON/IP packet %n->%n",
		       idb->hw_namestring,
		       pak->clns_srcaddr,  pak->clns_dstaddr);
	    }
	} else {
	    buginf("\n%s: decapsulating EON/IP packet (pid=0x%x)",
		   idb->hw_namestring, clns->pid);
	}

    reg_invoke_tunnel_raw_enqueue(idb, pak);
    return;
}

/*
 * eon_ip_setmode
 *
 * Provide tunnel initialization with information for the
 * tinfo structure 
 *
 */
static void eon_ip_setmode (tunnel_info *tinfo,
		            linktype_t *getlink, int *enctype)
{
    tinfo->encaps = eon_ip_encaps;
    tinfo->resync = (void (*)(tunnel_info *))return_nothing;
    tinfo->cstate = NULL;

    tinfo->transport  = TUN_TRANS_IP;
    tinfo->protocol   = EONIP_PROT;
    tinfo->modestring = "EON";

    *getlink = eon_ip_type2link;
    *enctype  = ET_TUNNEL;
}

/*
 * eon_init
 *
 * Perform EON registration
 */

static void eon_init (subsystype *subsys)
{
    reg_add_tunnel_setmode(TUN_MODE_EON_IP, eon_ip_setmode, "eon_ip_setmode");
    reg_add_ip_enqueue(EONIP_PROT, eon_ip_decaps, "eon_ip_decaps");
}

/*
 * EON subsystem header
 */
#define EON_MAJVERSION 1
#define EON_MINVERSION 0
#define EON_EDITVERSION 1

SUBSYS_HEADER(eon, EON_MAJVERSION, EON_MINVERSION,
	      EON_EDITVERSION, eon_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iphost, clns",
	      "req: iphost, clns, tunnel");
