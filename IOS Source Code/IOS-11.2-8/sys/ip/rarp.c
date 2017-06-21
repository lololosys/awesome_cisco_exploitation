/* $Id: rarp.c,v 3.4.12.7 1996/08/28 12:50:20 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/rarp.c,v $
 *------------------------------------------------------------------
 * rarp.c -- RFC903 (RARP) Reverse Address Resolution Protocol
 *
 * 16-July-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rarp.c,v $
 * Revision 3.4.12.7  1996/08/28  12:50:20  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.12.6  1996/07/06  05:53:58  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.4.12.5  1996/07/02  23:11:18  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.4.12.4  1996/06/27  23:59:24  bcole
 * CSCdi54675: router, used as RARP server, is not responding to RARP
 * requests.
 * Branch: California_branch
 *
 * Add debug code.
 *
 * Revision 3.4.12.3  1996/06/18  01:46:05  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.4.12.2  1996/04/15  21:16:50  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.4.12.1  1996/03/18  20:17:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.3  1996/03/13  01:38:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.24.2  1996/03/07  09:47:06  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.24.1  1996/02/20  01:00:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/13  08:13:41  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.3  1995/11/17  09:36:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:41:04  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:58:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:00:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "config.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "packet.h"
#include "../if/network.h"
#include "ieee.h"
#include "ip.h"
#include "ip_media.h"
#include "logger.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "subsys.h"
#include "../ip/ip_registry.h"


/*
 * Storage
 */
watched_queue *RARPpacketQ;

/*
 * rarp_enqueue
 * Enqueue rarp packets.
 */

static void rarp_enqueue (paktype *pak)
{
    arp_traffic.revrcv++;
    process_enqueue_pak(RARPpacketQ, pak);
}

/*
 * rarp_send_request
 * Use reverse ARP to determine an interface's IP address
 */

static void rarp_send_request (idbtype *idb, idbtype *unknown)
{
    paktype *pak;
    rfc_arp *rfcptr;

    if (!unknown->ip_enabled || unknown->ip_address)
	return;
    pak = getbuffer(RBYTES);
    if (!pak)
	return;

    rfcptr = (rfc_arp *)rfc_arp_headstart(pak);
    memset(rfcptr, 0, RBYTES);
    arp_traffic.revsent++;
    rfcptr->opcode = OP_REV_REQ;		/* we are requesting */
    switch (idb->hwptr->enctype) {
      case ET_SNAP:     
	rfcptr->arhrd = HW_802;
        break;
      case ET_ARPA:     
      case ET_SAP:
	rfcptr->arhrd = HW_ETH;
        break;
      case ET_SMDS:     
	rfcptr->arhrd = HW_SMDS_RFC;
        break;
      default:
	retbuffer(pak);         /* Don't know what to do */
	return;			/* Just get out */
    }

    rfcptr->arhln = HL_ETH;	/* 6 byte hdw address (both Ethernet & 802) */
    rfcptr->arpro = PR_IP;			/* IP protocol */
    rfcptr->arpln = PL_IP;			/* 4 byte software address */
    ieee_copy(unknown->hwptr->hardware,rfcptr->ipshdw); /* source hdw */
    ieee_copy(unknown->hwptr->hardware,rfcptr->ipthdw); /* target hdw */
    arp_send(pak, RBYTES, (uchar *)baddr, LINK_RARP, idb,
	     idb->hwptr->enctype, LINK_IP);
}

/*
 * rarp_send_reply
 * send response to reverse arp request, send its IP address
 */

static void rarp_send_reply (arptype *ptr, idbtype *idb)
{
    paktype *pak;
    rfc_arp *rfcptr;

    if (arp_debug) {
	buginf("\nRARP: Sending reply out %s", idb ? idb->namestring : "");
    }
    if (!ptr || !idb)
	return;
    pak = getbuffer(RBYTES);
    if (!pak)
	return;

    rfcptr = (rfc_arp *)rfc_arp_headstart(pak);
    memset(rfcptr, 0, RBYTES);
    arp_traffic.revsent++;
    rfcptr->opcode = OP_REV_REP;		/* we are sending */
    switch (ptr->enctype) {
      case ET_SNAP:     
	rfcptr->arhrd = HW_802;
        break;
      case ET_ARPA:     
      case ET_SAP:
	rfcptr->arhrd = HW_ETH;
        break;
      case ET_SMDS:     
	rfcptr->arhrd = HW_SMDS_RFC;
        break;
      default:
	retbuffer(pak);         /* Don't know what to do */
	return;			/* Just get out */
    }

    /* give answer */
    rfcptr->iptpro = ptr->address;
    PUTLONG(&rfcptr->ipspro, idb->ip_rarp_server_addr);

    rfcptr->arhln = HL_ETH;	/* 6 byte hdw address (both Ethernet & 802) */
    rfcptr->arpro = PR_IP;			/* IP protocol */
    rfcptr->arpln = PL_IP;			/* 4 byte software address */
    ieee_copy(idb->hwptr->hardware,rfcptr->ipshdw); /* source hdw */
    ieee_copy(ptr->hardware,rfcptr->ipthdw); /* target hdw */
    arp_send(pak, RBYTES, rfcptr->ipthdw, LINK_RARP, idb,
	     idb->hwptr->enctype, LINK_IP);
}

/*
 * rarp_process_input
 *
 * Handle RARP packets.
 */

static void rarp_process_input (void)
{
    paktype *pak;
    rfc_arp *rfcptr;
    idbtype *idb;
    hwidbtype *hwidb;
    boolean to_bridge;
    int i, count;
    arptype *arpptr;
    
    count = PAK_SWITCHCOUNT;
    while (TRUE) {
	if (--count == 0)
	    break;
	pak = process_dequeue(RARPpacketQ);
	if (!pak)
	    break;

	rfcptr = (rfc_arp *)rfc_arp_headstart(pak);

	hwidb = pak->if_input->hwptr;
	idb_increment_rx_proto_counts(pak, ACCT_PROTO_ARP);

	to_bridge = TRUE;
	rfcptr = (rfc_arp *)pak->network_start;
	if (rfcptr->opcode == OP_REV_REP) {
	    /*
	     * We get only replies, and then only if we don't know our IP
	     * address.  If the target hardware address is ours, set our IP 
	     * address and remember who gave us the (possibly erroneous!) 
	     * information.
	     */
	    hwidbtype *addridb;
	    if (arp_debug) {
		buginf("\nRARP: Rcvd RARP rep");
	    }
	    FOR_ALL_HWIDBS(addridb) {
		idb = addridb->firstsw;
		if (ieee_equal(rfcptr->ipthdw,addridb->hardware)) {
		    if (idb->ip_enabled && !idb->ip_address
			&& (addridb->status & IDB_IEEEMASK)) {
			idb->ip_address = n2hl(rfcptr->iptpro);
			idb->ip_resolvehost = n2hl(GETLONG(&rfcptr->ipspro));
			idb->resolvemethod = RES_RARP;
			class_resolve(idb);
			idb->ip_nets_mask = MASK_HOST;
		    }
		    to_bridge = FALSE;
		    errmsg(&msgsym(RARP, LINK),
			idb->namestring,idb->ip_address,idb->ip_resolvehost);
		    break;
		}
	    }
	} else {
	    /*
	     * If we are acting as a RARP server, check our arp table and
	     * answer if we know the IP address for the helpless client.
	     */
	    if (arp_debug) {
		buginf("\nRARP: Rcvd RARP req for %e", rfcptr->ipshdw);
	    }
	    if (pak->if_input->ip_rarp_server_addr &&
		(rfcptr->opcode == OP_REV_REQ)) {
		for (i = 0; i < NETHASHLEN; i++) {
		    arpptr = arptable[i];
		    while (arpptr) {
			if ((arpptr->mode == ARP_STATIC) &&
			    (arpptr->type == LINK_IP) && 
			    ieee_equal(rfcptr->ipshdw, arpptr->hardware)) {
			    rarp_send_reply(arpptr, pak->if_input);
			    to_bridge = FALSE;
			    break;
			}
			arpptr = arpptr->next;
		    }
		    if (to_bridge == FALSE)
			break;
		}
	    }
	    /*
	     * This wasn't a reply, so if it isn't something we sent,
	     * bridge it.
	     */
	    FOR_ALL_HWIDBS(hwidb) {
		if (ieee_equal(rfcptr->ipshdw,hwidb->hardware)) {
		    /* one of ours, drop it */
		    to_bridge = FALSE;
		    break;
		}
	    }
	}
	if (to_bridge)
	    netinput_enqueue(pak);
	else
	    retbuffer(pak);
    }
}

/*
 * rarp_process
 * Handle RARP packets.
 */

static process rarp_process (void)
{
    ulong major, minor;
    
    process_watch_queue(RARPpacketQ, ENABLE, RECURRING);
    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		rarp_process_input();
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

/*
 * rarp_init
 * Initialize the RARP support
 */

static void rarp_init (subsystype *dummy)
{
    RARPpacketQ = create_watched_queue("RARP Packets", 0, 0);
    process_create(rarp_process, "RARP Input", NORMAL_STACK, PRIO_NORMAL);
    /*
     * Register some functions
     */
    reg_add_raw_enqueue(LINK_RARP, rarp_enqueue, "rarp_enqueue");
    reg_add_ip_rarp_send_request(rarp_send_request, "rarp_send_request");
}

/*
 * RARP subsystem header
 */

#define RARP_MAJVERSION 1
#define RARP_MINVERSION 0
#define RARP_EDITVERSION  1

SUBSYS_HEADER(rarp, RARP_MAJVERSION, RARP_MINVERSION, RARP_EDITVERSION,
	      rarp_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      subsys_req_iphost);
