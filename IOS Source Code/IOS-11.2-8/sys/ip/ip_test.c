/* $Id: ip_test.c,v 3.2.60.1 1996/03/18 20:16:21 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/ip/ip_test.c,v $
 *------------------------------------------------------------------
 * IP transmit test command
 *
 * September 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ip_test.c,v $
 * Revision 3.2.60.1  1996/03/18  20:16:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/13  01:37:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.2  1996/03/07  09:45:58  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  14:25:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:33:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:57:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "ieee.h"
#include "registry.h"
#include "sys_registry.h"
#include "../if/transmit_test.h"
#include "../ip/ip.h"
#include "ttysrv.h"

static ipaddrtype tt_ipaddr = 0;


static void ip_transmit_frames_protocol_print (void)
{
    printf("\n1) IP");
}


static paktype *build_ip_header_by_hand (idbtype *idb, paktype *pak)
{
    printf("Not written yet.\n");
    datagram_done(pak);
    return(NULL);
}

void build_ip_header (idbtype *idb, paktype **pakp,
		      int encap, ulong testframesize)
{
    paktype *pak = *pakp;
    char *args, buf[BUFLEN];
    int c, i;
    iphdrtype *ip;
    udptype *udp;
    ushort *sptr;
    uchar *cptr;
    boolean done;
    hwidbtype *hwidb = hwidb_or_null(idb);

    sptr = (ushort *)((uchar *)pak->datagramstart + pak->datagramsize);
    done = FALSE;
    switch (encap) {
      case '1':
	*sptr++ = TYPE_IP10MB;
	done = TRUE;
	break;
	
      case '2':
	if (hwidb->status & IDB_ETHER) {
	    *sptr++ = testframesize; /* length */
	}
	*sptr++ = (SAP_IP << 8) | SAP_IP;
	*sptr = (LLC1_UI << 8);
	sptr = (ushort *)((uchar *)sptr + 1);
	done = TRUE;
	break;
	
      case '3':
	if (hwidb->status & IDB_ETHER) {
	    *sptr++ = testframesize; /* length */
	}
	*sptr++ = SNAPSNAP;
	*sptr++ = 0x0300;
	*sptr++ = 0x0000;
	*sptr++ = TYPE_IP10MB;
	done = TRUE;
	break;
	
      case '4':
	if (hwidb->status & IDB_ETHER) {
	    *sptr++ = testframesize; /* length */
	}
	*sptr++ = SNAPSNAP;
	*sptr++ = 0x0300;
	*sptr++ = 0x000c;
	*sptr++ = TYPE_IP10MB;
	done = TRUE;
	break;
	    
      case '5':
	if (hwidb->status & IDB_ETHER) {
	    *sptr++ = testframesize; /* length */
	}
	*sptr++ = SNAPSNAP;
	*sptr++ = 0x0300;
	*sptr++ = 0x00F8;
	*sptr++ = TYPE_IP10MB;
	done = TRUE;
	break;
    }

    ip = (iphdrtype *)sptr;
    ip->version = 4;
    ip->ihl = 5;
    ip->tl = testframesize - ((uchar *)ip - pak->datagramstart);
    ip->ttl = 255;
    ip->prot = UDP_PROT;
    udp = (udptype *)(ip + 1);
    udp->sport = DISCARD_PORT;
    udp->dport = DISCARD_PORT;
    udp->length = testframesize - ((uchar *)udp - pak->datagramstart);
    udp->checksum = 0;

    done = FALSE;
    while (!done) {
	printf("\nIP Protocol Data:");
	printf("\n1) To the router");
	printf("\n2) To another address");
	printf("\n9) Ask for everything");
	printf("\nChoice: ");
	READ_OR_EXIT;
	switch (c) {
	  case '1':
	    ip->srcadr = 0x99999999;
	    ip->dstadr = idb->ip_address;
	    done = TRUE;
	    break;
	    
	  case '2':
	    do {
		printf("Address [%i]: ", tt_ipaddr);
		READ_OR_EXIT;
	    } while (!parse_ip_address(buf, &tt_ipaddr) && !tt_ipaddr);

	    ip->srcadr = idb->ip_address;
	    ip->dstadr = tt_ipaddr;
	    done = TRUE;
	    break;
	    
	  case '9':
	    pak = build_ip_header_by_hand(idb, pak);
	    break;
	}
    }
    cptr = (uchar *)udp->udpdata;
    for (i = 0; i < udp->length - UDPHEADERBYTES; i++) {
	*cptr++ = i;
    }
    ip->checksum = ipcrc((ushort *)ip, ip->ihl << 2);
    pak->datagramsize = testframesize;
    *pakp = pak;
}

void ip_test_init (void)
{
    reg_add_transmit_frames_protocol('1', build_ip_header, "build_ip_header");
    reg_add_transmit_frames_protocol_print('1',
					   ip_transmit_frames_protocol_print,
					   "ip_transmit_frames_protocol_print"
					   );
}
