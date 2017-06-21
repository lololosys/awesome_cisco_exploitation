/* $Id: xns_test.c,v 3.2.60.1 1996/03/18 22:54:19 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xns/xns_test.c,v $
 *------------------------------------------------------------------
 * XNS transmit test command
 *
 * September 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: xns_test.c,v $
 * Revision 3.2.60.1  1996/03/18  22:54:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.3  1996/03/13  02:15:23  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.24.2  1996/03/07  11:22:53  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  22:01:57  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  19:25:37  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:30:51  hampton
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
#include "registry.h"
#include "sys_registry.h"
#include "../if/transmit_test.h"
#include "../xns/xns.h"
#include "ieee.h"
#include "ttysrv.h"


static ulong tt_xnsnet = 0;
static uchar tt_xnshost[IEEEBYTES]    = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uchar xns_def_ether_dst[IEEEBYTES] = {0x00, 0x00, 0x0c, 0x99, 0x99, 0x99};


static void xns_transmit_test_protocol_print (void)
{
    printf("\n2) XNS");
}

static paktype *build_xns_header_by_hand (idbtype *idb, paktype *pak)
{
    printf("Not written yet.\n");
    datagram_done(pak);
    return(NULL);
}

static void build_xns_header (idbtype *idb, paktype **pakp,
		       int encap, ulong testframesize)
{
    paktype	*pak = *pakp;
    char	*args, buf[BUFLEN];
    int		c, i;
    xnshdrtype	*xns;
    ushort	*sptr;
    uchar	*cptr;
    boolean	done;
    ulong	net;
    hwidbtype	*hwidb = hwidb_or_null(idb);
    xnssb	*xsb;

    sptr = (ushort *)((uchar *)pak->datagramstart + pak->datagramsize);
    done = FALSE;
    switch (encap) {
      case '1':
	*sptr++ = TYPE_XNS;
	done = TRUE;
	break;
	
      case '2':
	printf("\nXNS is not supported on a SAP encapsulation.");
	datagram_done(pak);
	*pakp = pak;
	return;
	
      case '3':
	if (hwidb->status & IDB_ETHER) {
	    *sptr++ = testframesize; /* length */
	}
	*sptr++ = SNAPSNAP;
	*sptr++ = 0x0300;
	*sptr++ = 0x0000;
	*sptr++ = TYPE_XNS;
	done = TRUE;
	break;
	
      case '4':
	if (hwidb->status & IDB_ETHER) {
	    *sptr++ = testframesize; /* length */
	}
	*sptr++ = SNAPSNAP;
	*sptr++ = 0x0300;
	*sptr++ = 0x000c;
	*sptr++ = TYPE_XNS;
	done = TRUE;
	break;
	    
      case '5':
	if (hwidb->status & IDB_ETHER) {
	    *sptr++ = testframesize; /* length */
	}
	*sptr++ = SNAPSNAP;
	*sptr++ = 0x0300;
	*sptr++ = 0x00F8;
	*sptr++ = TYPE_XNS;
	done = TRUE;
	break;
    }

    xns = (xnshdrtype *)sptr;
    xns->cksum = 0xFFFF;
    xns->len= testframesize - ((uchar *)xns - pak->datagramstart);
    xns->pt = 0;
    xns->tc = 0;
    xns->dsock = 0x516;
    xns->ssock = 0x516;

    done = FALSE;
    while (!done) {
	printf("\nXNS Protocol Data:");
	printf("\n1) To this interface");
	printf("\n2) To another interface");
	printf("\n9) Ask for everything");
	printf("\nChoice: ");
	READ_OR_EXIT;
	switch (c) {
	  case '1':
	    xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
	    PUT2WD(xns->ddnet, xsb->xnsnet);
	    PUT2WD(xns->ssnet, xsb->xnsnet);
	    ieee_copy(xns_def_ether_dst, xns->shost);
	    ieee_copy(xsb->xns_node, xns->dhost);
	    done = TRUE;
	    break;

	  case '2':
	    do {
		printf("Address [%u.%e]: ", tt_xnsnet, tt_xnshost);
		READ_OR_EXIT;
	    } while (!parse_xnsaddress(buf, &net, tt_xnshost) && !tt_xnsnet);

	    if (net != 0 && net != -1) {
		tt_xnsnet = net;
	    }
	    xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
	    PUT2WD(xns->ddnet, tt_xnsnet);
	    PUT2WD(xns->ssnet, xsb->xnsnet);
	    ieee_copy(tt_xnshost, xns->dhost);
	    ieee_copy(xsb->xns_node, xns->shost);
	    done = TRUE;
	    break;

	  case '9':
	    pak = build_xns_header_by_hand(idb, pak);
	    break;
	}
    }
    cptr = (uchar *)xns + XNSOVERHEADBYTES;
    for (i = 0; i < xns->len - XNSOVERHEADBYTES; i++) {
	*cptr++ = i;
    }
    pak->datagramsize = testframesize;
    *pakp = pak;
}

void xns_test_init (void)
{
    reg_add_transmit_frames_protocol('2', build_xns_header,"build_xns_header");
    reg_add_transmit_frames_protocol_print('2',
					   xns_transmit_test_protocol_print,
					   "xns_transmit_test_protocol_print");
}
