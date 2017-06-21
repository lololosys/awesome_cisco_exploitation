/* $Id: novell_test.c,v 3.2.62.2 1996/07/26 00:09:03 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/novell_test.c,v $
 *------------------------------------------------------------------
 * Novell transmit test command
 *
 * September 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: novell_test.c,v $
 * Revision 3.2.62.2  1996/07/26  00:09:03  hampton
 * Various minor IPX cleanups.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  22:53:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/13  02:14:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.2  1996/03/07  11:22:10  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  22:00:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  19:23:18  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:59:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:28:51  hampton
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
#include "../xns/novell.h"
#include "ieee.h"
#include "ttysrv.h"


static ulong tt_novsnet = 0;
static ulong tt_novdnet = 0;
static uchar tt_novshost[IEEEBYTES] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uchar tt_novdhost[IEEEBYTES] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


static void novell_transmit_frames_protocol_print (void)
{
    printf("\n3) IPX");
}

static paktype *build_novell_header_by_hand (idbtype *idb, paktype *pak)
{
    printf("Not written yet.\n");
    datagram_done(pak);
    return(NULL);
}

static void build_novell_header (idbtype *idb, paktype **pakp,
			  int encap, ulong testframesize)
{
    paktype *pak = *pakp;
    char *args, buf[BUFLEN];
    int c, i;
    xnshdrtype *xns;
    ushort *sptr;
    uchar *cptr;
    boolean done;
    ulong net;
    hwidbtype *hwidb = hwidb_or_null(idb);

    ieee_copy(idb->novell_node, tt_novshost);

    sptr = (ushort *)((uchar *)pak->datagramstart + pak->datagramsize);
    done = FALSE;
    switch (encap) {
      case '1':
	*sptr++ = TYPE_NOVELL1;
	done = TRUE;
	break;
	
      case '2':
	if (hwidb->status & (IDB_ETHER|IDB_LEX)) {
	    *sptr++ = testframesize; /* length */
	}
	*sptr++ = (SAP_NOVELL << 8) | SAP_NOVELL;
	*sptr = (LLC1_UI << 8);
	sptr = (ushort *)((uchar *)sptr + 1);
	done = TRUE;
	break;
	
      case '3':
	if (hwidb->status & (IDB_ETHER|IDB_LEX)) {
	    *sptr++ = testframesize; /* length */
	}
	*sptr++ = SNAPSNAP;
	*sptr++ = 0x0300;
	*sptr++ = 0x0000;
	*sptr++ = TYPE_NOVELL1;
	done = TRUE;
	break;
	
      case '4':
	if (hwidb->status & (IDB_ETHER|IDB_LEX)) {
	    *sptr++ = testframesize; /* length */
	}
	*sptr++ = SNAPSNAP;
	*sptr++ = 0x0300;
	*sptr++ = 0x000c;
	*sptr++ = TYPE_NOVELL1;
	done = TRUE;
	break;
	    
      case '5':
	if (hwidb->status & (IDB_ETHER|IDB_LEX)) {
	    *sptr++ = testframesize; /* length */
	}
	*sptr++ = SNAPSNAP;
	*sptr++ = 0x0300;
	*sptr++ = 0x00F8;
	*sptr++ = TYPE_NOVELL1;
	done = TRUE;
	break;

      case '6':
	*sptr++ = testframesize; /* length */
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
	printf("\nIPX Protocol Data:");
	printf("\n1) Specify source/destination");
	printf("\n9) Ask for everything");
	printf("\nChoice: ");
	READ_OR_EXIT;
	switch (c) {
	  case '1':
	    do {
		printf("Source address [%x.%e]: ", tt_novsnet, tt_novshost);
		READ_OR_EXIT;
	    } while (!parse_novelladdress(buf, &net, tt_novshost) &&
		     !tt_novsnet);

	    tt_novsnet = net;

	    PUT2WD(xns->ssnet, tt_novsnet);
	    ieee_copy(tt_novshost, xns->shost);

	    do {
		printf("Destination address [%x.%e]: ",
		       tt_novdnet, tt_novdhost);
		READ_OR_EXIT;
	    } while (!parse_novelladdress(buf, &net, tt_novdhost) &&
		     !tt_novdnet);

	    tt_novdnet = net;

	    PUT2WD(xns->ddnet, tt_novdnet);
	    ieee_copy(tt_novdhost, xns->dhost);

	    done = TRUE;
	    break;
	    
	  case '9':
	    pak = build_novell_header_by_hand(idb, pak);
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

void novell_test_init (void)
{
    reg_add_transmit_frames_protocol('3', build_novell_header,
				     "build_novell_header");
    reg_add_transmit_frames_protocol_print('3',
				   novell_transmit_frames_protocol_print,
				   "novell_transmit_frames_protocol_print");
}
