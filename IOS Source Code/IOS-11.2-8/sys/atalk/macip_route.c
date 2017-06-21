/* $Id: macip_route.c,v 3.2.58.3 1996/07/10 22:19:33 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/macip_route.c,v $
 *------------------------------------------------------------------
 * macip_route.c	- routing routines for MacIP.
 *
 * September 1991, Russ Tremain, Tremain Laboratories, under contract
 * 	for cisco Systems, Inc.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: macip_route.c,v $
 * Revision 3.2.58.3  1996/07/10  22:19:33  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.2.58.2  1996/03/23  01:28:33  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2.58.1  1996/03/18  18:53:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  08:29:57  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  13:34:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:45:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:59:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:51:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:10:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <ciscolib.h>
#include <interface.h>
#include "packet.h"
#include "address.h"
#include "../ip/ip.h"
#include "atalk_private.h"
#include "macip_util.h"
#include "macip_db.h"
#include "macip_route.h"


/*
 * macip_to_IP -
 *	DDP in, IP out.  true if pkt handled.
 *
 *	NOTE:   called from macip_process() whenever there are
 *	macip packets to route from DDP.
 *
 */
void
macip_to_IP(paktype * pak)
{
    ipaddrtype cip;
    server_rec		*ss;
    ataddress		src;
    client_rec		*cptr;
    iphdrtype		*ip;

    /* count pkts in: */
    ++atalk_stat[MACIP_DDP_IN];

#ifdef	notdef
    /* this is a client error, but let it pass silently,
     * since MacTCP v1.1 incorrectly directs MacIP pkts
     * to the IPGATEWAY ATP socket instead of the MACIP
     * DDP static socket.
     */
    if (atalk_pak_dsock(pak) != ATALK_SOCK_MACIP)
        ++atalk_stat[MACIP_DDP_WRONG_SOCKET];
#endif

    /* save client source DDP address: */
    src.spec = atalk_pak_ssock(pak);
    src.net  = atalk_pak_snet(pak);
    src.node = atalk_pak_snode(pak);

    /*
     * Set network_start to point at encapsulated IP header
     */
    pak->datagramstart = pak->transport_start;
    pak->network_start = pak->datagramstart;
    pak->datagramsize = pak->atalk_datalen;
    pak->encsize = 0;

    /*
     * Realign this puppy if it's necessary
     */
    ALIGN_DATAGRAM_CMD(pak_realign(pak, NULL));

    ip = (iphdrtype *)ipheadstart(pak);

    /*
     * IF not a client, and in one of our servers address space, then try to
     * make a client.  This will automatically restore "off-zone" clients after
     * a reload.
     */
    cip = GETLONG(&ip->srcadr);
    /* if not a client... */
    if ((cptr = mdb_clientbyIP(cip)) == NULL) {
	/* if no server for this IP address... */
	if (!mdb_serverbyCIP(&ss, cip)) {
	    ++atalk_stat[MACIP_DDP_NO_CLIENT_SERVICE];
	    datagram_done(pak);
	    return;
	}
	/* otherwise, we have a server, add new client: */
	if ((mdb_app_client(ss, cip, &src) != MDB_OK) || 
	    (cptr = mdb_clientbyIP(cip)) == NULL )  {
	    datagram_done(pak);
	    ++atalk_stat[MACIP_DDP_CLIENT_CREATE_ERROR];
	    return;
	}

					/* note - <ss> set above */
    } else {				/* established client */
	ss = cptr->srec;
    }

    /* force route lookup: */
    pak->if_output = NULL;
    pak->enctype = 0;
    pak->length = 0;
    pak->linktype = LINK_IP;
    pak->desthost = 0;
    /* prevent redirects if DDP & IP interface happen to be the same: */
    pak->flags |= PAK_NOREDIRECT;

    /* take care of input queue accounting, since we're reusing pkt: */
    clear_if_input(pak);
    pak->if_input = ss->ipidb;

    /* client is still alive - update timer: */
    GET_TIMESTAMP(cptr->timer);

    /* forward via ip: */
    if (ip_forward((paktype *)pak)) {
	++atalk_stat[MACIP_DDP_IP_OUT];
    } else {
	/* if forward fails, pkt is for us: */
	ip_enqueue((paktype *)pak);
    }
}

/*
 * macip_to_DDP -
 *	IP in, DDP out.
 *
 *	NOTE:   called from macip_process() whenever there are
 *	macip packets to route from IP.
 *
 */
void
macip_to_DDP (paktype *inpak)
{
    ipaddrtype	dst;
    client_rec	*cptr;
    paktype	*pak;
    iphdrtype	*inip;

    inip = (iphdrtype *)ipheadstart(inpak);

    dst = GETLONG(&inip->dstadr);	/* get ip dest. */
    if ((cptr = mdb_clientbyIP(dst)) == NULL) {
	datagram_done(inpak);
	return;
    }

    /* we only accept pkts for our active clients: */
    ++atalk_stat[MACIP_IP_IN];

    /* add DDP layer: */
    pak = atalk_pak_create(cptr->srec->ddpidb,
			   atalk_address(cptr->ddpaddr.net,
					 cptr->ddpaddr.node),
			   ATALK_SOCK_MACIP, /* src socket */
			   ATALK_SOCK_MACIP, /* dst socket */
			   DDP_TYPE_MACIP,
			   GETSHORT(&inip->tl)); /* ddp data length == IP data length */
    
    if (pak == NULL) {
	++atalk_stat[MACIP_IP_DDP_CREATE_ERROR];
	datagram_done(inpak);
	return;				/* pkt done */
    }
    /* copy data, free input packet: */
    bcopy((uchar *)inip, pak->transport_start, GETSHORT(&inip->tl));
    datagram_done(inpak);

    /* currently not set by atalk_pak_create: */
    pak->if_input = cptr->srec->ddpidb->phys_idb;

    /* generate checksum: */
    atalk_DDPcksum(pak);

    if (atalk_send(pak))
	++atalk_stat[MACIP_IP_DDP_OUT];
    else
	++atalk_stat[MACIP_IP_DDP_SEND_FAIL];
}
