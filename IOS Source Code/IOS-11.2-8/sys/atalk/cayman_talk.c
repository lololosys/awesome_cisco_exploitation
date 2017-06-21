/* $Id: cayman_talk.c,v 3.2.60.4 1996/07/10 22:19:26 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/cayman_talk.c,v $
 *------------------------------------------------------------------
 * Cayman TunnelTalk Encapsulation
 *
 * April 1992, Steven Lin
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cayman_talk.c,v $
 * Revision 3.2.60.4  1996/07/10  22:19:26  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/06/27  09:21:27  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.2.60.2  1996/03/23  01:28:19  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2.60.1  1996/03/18  18:53:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:29:52  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:34:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:44:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:50:56  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:10:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "packet.h"
#include "interface_private.h"
#include "../if/network.h"

#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../ip/tunnel.h"

#include "atalk_private.h"
#include "at_registry.h"

/*
 * cayman_cstate
 *
 * Called by tunnel_setmode to change state of tunnel.
 */
static void cayman_cstate (idbtype *swidb, boolean newstate)
{
    tunnel_info *tinfo = swidb->hwptr->tunnel;
    atalkidbtype *idb = atalk_getidb(swidb);
    
    if (!tinfo)
	return;

    if (idb == NULL) {
	atalk_init_idb(swidb);
	if ((idb = atalk_getidb(swidb)) == NULL) {
	    printf("\n%% Cannot create data structures for atalk interface "
		   "information.");
	    return;
	}
    }

    atalkif_shutdown(idb);

    if (newstate) {
	/* configure for AppleTalk */
	atalkif_setup(idb, ATALK_NET_UNNUMBERED, ATALK_NET_UNNUMBERED,
		      ATALK_NET_UNNUMBERED, 0, TRUE);

	/* Change checksum default */
	tinfo->checksum = tinfo->checksum_default = TRUE;

    } else {

	/* reset everything */
	atalkif_init(idb, TRUE, TRUE);

	/* Revert to non-cayman checksums */
	tinfo->checksum = tinfo->checksum_default = FALSE;
    }
}

/*
 * cayman_udp_type2link
 *
 * Called by raw_enqueue to determine where to send the packet we just
 * decapsulated for us. 
 */

static long cayman_udp_type2link(paktype *pak)
{
    if (pak)
	return LINK_APPLETALK;
    else
	return LINK_ILLEGAL;
}

/*
 * cayman_udp_encaps
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

static paktype *cayman_udp_encaps (tunnel_info *tinfo, paktype *pak)
{
    paktype	 *new_pak;
    udptype	 *udp;
    unsigned      bytes;
    uchar	 *hdr;

    if (pak->linktype != LINK_APPLETALK)
	return NULL;

    /* bytes = ipheader + udpheader + llap header + ddp pkt */
    bytes = tinfo->header->datagramsize + UDPHEADERBYTES + LLAP_ENCAPBYTES
	+ pak->datagramsize;
    new_pak = getbuffer(bytes);
    if (!new_pak)
        return NULL;

    udp = (udptype *) (ipheadstart(new_pak) + tinfo->header->datagramsize);
    udp->sport    = udp->dport = CAYMAN_PORT;
    udp->length   = UDPHEADERBYTES + pak->datagramsize + LLAP_ENCAPBYTES;
    udp->checksum = 0;

    /* copy old packet into place -- tunnel_encaps set DG start & size */
    (void) bcopy(pak->datagramstart, (uchar *) udp->udpdata + LLAP_ENCAPBYTES,
		 pak->datagramsize);

    /* Set LLAP header bytes. */
    hdr = (uchar *) udp->udpdata;
    hdr[LLAP_DSTNODE] = 0xFA;
    hdr[LLAP_SRCNODE] = 0xCE;
    hdr[LLAP_PAKTYPE] = 0x02;
    
    new_pak->datagramsize = bytes;

    return(new_pak);
}

/*
 * cayman_udp_decaps
 *
 * Decapsulate an packet passed down from IP processing
 *
 * Determine which tunnel packet is destined for.
 *
 * Finally, if packet is good, move the data following the IP, UDP and
 * LLAP headers up to the start of the packet data area and enqueue the 
 * packet for native processing.
 *
 * Returns TRUE if packet absorbed, FALSE otherwise.
 * Called by SERVICE_IP_UDP_INPUT.
 */

static boolean cayman_udp_decaps (paktype *pak, udptype *udp)
{
    iphdrtype   *ip;
    hwidbtype	*idb;
    tunnel_info	*tinfo;
    ddptype     *ddp;

    ip = (iphdrtype *)ipheadstart(pak);

    if (udp->dport != CAYMAN_PORT)
	return(FALSE);

    if (tunnel_debug)
      buginf("\n%s: Cayman/UDP to decaps %i->%i (len=%d ttl=%d)",
	     pak->if_input->namestring, ip->srcadr, ip->dstadr,
	     ip->tl, ip->ttl);

    FOR_ALL_TUNNELS(tinfo)
	if ((tinfo->mode == TUN_MODE_CAYMAN_UDP) &&
	    (ip->srcadr == tinfo->destination) &&
	    (tinfo->hwidb->state == IDBS_UP))
	    break;

    if (!tinfo || !tinfo->hwidb) {
	datagram_done(pak);
	return(TRUE);
    }

    idb = tinfo->hwidb;

    /* Strip off IP/UDP/LLAP header and move DDP pkt to datagramstart */
    ddp = (ddptype *) ((uchar *) udp + UDPHEADERBYTES + LLAP_ENCAPBYTES);

    pak->enctype       = ET_TUNNEL;
    pak->network_start = (uchar *)ddp;
    pak->datagramstart = pak->network_start;
    pak->datagramsize  = udp->length - UDPHEADERBYTES - LLAP_ENCAPBYTES;

    if (tunnel_debug)
	buginf("\n%s: CAYMAN/UDP decapsulated (len=%d)",
	       pak->if_input->namestring, pak->datagramsize);

    reg_invoke_tunnel_raw_enqueue(idb, pak);
    return(TRUE);
}

/*
 * cayman_udp_setmode
 *
 * Provide tunnel initialization with information for the
 * tinfo structure 
 *
 */
static void cayman_udp_setmode (tunnel_info *tinfo,
			        linktype_t *getlink, int *enctype)
{
    tinfo->encaps = cayman_udp_encaps;
    tinfo->resync = (void (*)(tunnel_info *))return_nothing;
    tinfo->cstate = cayman_cstate;

    tinfo->transport  = TUN_TRANS_IP;
    tinfo->protocol   = UDP_PROT;
    tinfo->modestring = "Cayman/UDP";

    *getlink = cayman_udp_type2link;
    *enctype = ET_TUNNEL;
}

static boolean
cayman_show_interface (atalkidbtype *idb)
{
    if (!atalkif_CaymanTunnel(idb))
	return (FALSE);

    printf("\n  AppleTalk port is a Cayman Tunnel");
    return (TRUE);
}

/*
 * cayman_init
 *
 * Perform cayman registration
 *
 */
void cayman_init (void)
{
    reg_add_atalk_show_interface_special(cayman_show_interface,
					 "cayman_show_interface");
    reg_add_ip_udp_input(cayman_udp_decaps, "cayman_udp_decaps");
    reg_add_tunnel_setmode(TUN_MODE_CAYMAN_UDP, cayman_udp_setmode,
			   "cayman_udp_setmode");
}
