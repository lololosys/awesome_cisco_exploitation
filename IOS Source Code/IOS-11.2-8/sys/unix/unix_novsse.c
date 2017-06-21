/* $Id: unix_novsse.c,v 3.4.20.2 1996/06/28 23:31:24 hampton Exp $
 * $Source: /release/112/cvs/Xsys/unix/unix_novsse.c,v $
 *------------------------------------------------------------------
 * unix_novsse.c -- Unix IPX specific support code
 *
 * Oct 1993 Tony Li
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_novsse.c,v $
 * Revision 3.4.20.2  1996/06/28  23:31:24  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.20.1  1996/03/18  22:30:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  10:59:17  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  21:27:53  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/06  17:43:47  ahh
 * CSCdi48168:  UNIX SSE file does not compile
 * One more.
 *
 * Revision 3.3  1995/11/17  17:55:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:46:31  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:42:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/01  06:55:11  hampton
 * Make the unix images compile again.  [CSCdi39708]
 *
 * Revision 2.2  1995/07/01 03:38:30  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi36668]
 *
 * Revision 2.1  1995/06/07 23:12:20  hampton
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
#include "logger.h"
#include "subsys.h"

#include "../if/ether.h"
#ifdef notdef
#include "../ip/ip.h"
#include "../hes/if_mci.h"
#include "../ip/ipfast.h"
#endif
#include "../xns/novfast.h"
#ifdef notdef
#include "config.h"
#include "../parser/parser_defs_sse.h"
#include "../util/range.h"
#include "../srt/span.h"
#include "../hes/sse_private.h"
#include "unix.h"
#endif

#include "unix_sse.h"
#include "../hes/sse.h"
#include "../hes/sse_registry.h"
#include "../parser/parser_defs_sse.h"

/*
 * novell_read_cachefile
 * Read in a file containing output from show novell cache for priming
 * cache for SSE debugging
 */

void novell_read_cachefile (uchar* buf, uchar* end)
{
    uchar macbuf[200], *macptr, *curr, c, *tail;
    int maclen, num;
    novpathtype *npath;
    uchar novdestaddr[XNS_HOSTBYTES];
    ulong novbucket, net;
    novcachetype *nptr;
    boolean addpath;
    int enctype;

    novcache_version++;		/* hack to make life easier */
/*
 * Novell cache looks like this:
 *  *         1.ffff.ffff.ffff   TokenRing0  000030C0C2DB000030804F3BE0E003
 *  *         2.ffff.ffff.ffff   TokenRing0  000030C0C2DB000030804F3BE0E003
 *  *         8.0000.0c02.87b3   Ethernet1   00000C0287B300000C01EFD30064
 *  *         5.0000.30c0.c2db   TokenRing0  000030C0C2DB000030804F3BE0E003
 */
    curr = buf;
    while (curr < end) {
	addpath = FALSE;
	while (isspace(*curr)) curr++;	/* skip whitespace */
	if (*curr == '*') curr++; /* skip "*" char */
	while (isspace(*curr)) curr++; /* skip whitespace */
	net = 0;
	c = toupper(*curr++);
	while (((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F'))) {
	    if ((c >= '0') && (c <= '9'))
		net = net*16 + (c - '0');
	    else
		net = net*16 + (c - 'A' + 10);
	    c = toupper(*curr++);
	}
	tail = curr;
	while (!isspace(*tail)) tail++;
	*tail = '\0';
	if (!parse_ether_address((char*)curr, novdestaddr)) {
	    buginf("\nSSE: parse ether address failed");
	    return;
	}
	curr = ++tail;
	while (isspace(*curr)) curr++;	/* skip whitespace */
	enctype = (*curr == 'E') ? ET_NOVELL_ETHER : ET_ARPA; /* debug kludge! */
	while (!isspace(*curr)) curr++;	/* skip idb name */
	while (isspace(*curr)) curr++;	/* skip whitespace */
	c = toupper(*curr++);
	macptr = macbuf;
	maclen = 0;
	while (((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F'))) {
	    num = 0;
	    if (((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F'))) {
		if ((c >= '0') && (c <= '9')) 
		    num = num*16 + (c - '0');
		else
		    num = num*16 + (c - '7');
		c = toupper(*curr++);
	    }
	    if (((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F'))) {
		if ((c >= '0') && (c <= '9')) 
		    num = num*16 + (c - '0');
		else
		    num = num*16 + (c - '7');
		c = toupper(*curr++);
	    }
	    *macptr++ = num;
	    maclen++;
	}
	novbucket = net;
	novbucket ^= GETSHORT(novdestaddr) << 16;
	novbucket ^= GETSHORT(novdestaddr+2);
	novbucket ^= GETSHORT(novdestaddr+4);
	novbucket ^= (novbucket >> 16);
	novbucket ^= (novbucket >> 8);
	novbucket &=0xFF;

	for (nptr = (novcachetype *)TABLE(0,novbucket);
	     nptr != (novcachetype*)0;
	     nptr = nptr->novc_next_ptr) {
	    if ((nptr->novc_dest_net == net) && 
		ieee_equal(nptr->novc_dest_address,novdestaddr)) {
		addpath = TRUE;
		goto augment;
	    }
	}

	nptr = malloc(sizeof(novcachetype));
	if (!nptr) {
	    buginf("\nSSE: no cache entry memory");
	    return;
	}
	nptr->novc_path_count = 0;
	nptr->novc_dest_net = net;
	if (net & 0x01)
	    nptr->novc_connected = TRUE; /* temporary hack */
	else
	    nptr->novc_connected = FALSE;
	ieee_copy(novdestaddr, nptr->novc_dest_address);
#ifdef notdef
	if (sse_debug)
	    buginf("\nSSE: novdestaddr %e nptr->novc_dest_address %e", novdestaddr,
		   nptr->novc_dest_address);
#endif
	GET_TIMESTAMP(nptr->novc_lastupdate);
	
    augment:
	nptr->novc_path_count++;
	npath = malloc(sizeof(novpathtype)*nptr->novc_path_count);
	if (!npath) {
	    buginf("\nSSE: no cache entry memory for path entry");
	    nptr->novc_path_count--;
	    return;
	}
	bcopy(nptr->novc_path_list_ptr, npath,
	      sizeof(novpathtype)*(nptr->novc_path_count-1));
	nptr->novc_path_list_ptr = npath;
	npath = (novpathtype*)((uchar*)npath+(sizeof(novpathtype)*(nptr->novc_path_count-1)));
	npath->novp_idb_ptr = SWIDBQ_HEAD;
	npath->length = maclen;
	bcopy(macbuf, npath->novp_mh.mac_bytes, maclen);
	bcopy(npath->novp_mh.mac_bytes, npath->novp_mh_oa.mac_bytes+1, maclen);
	npath->total_encap_bytes = maclen; /* assume ETHER or HDLC for now*/
	npath->extra_bytes = 0;/* no odd byte start bug here*/
	npath->enctype = enctype;
	if (!addpath) {
	    nptr->novc_next_ptr = novcache[novbucket];
	    nptr->novc_path_next_ptr = npath;
	    novcache[novbucket] = nptr;
	}
	nptr->novc_version = novcache_version;
	npath = nptr->novc_path_list_ptr;
	for (num = 0; num < nptr->novc_path_count-1; num++) {
	    npath->novp_next_ptr = npath++;
	}
	npath->novp_next_ptr = NULL;
    }
}

/*
 * unix_ipxsse_register_thyself
 */

void unix_ipxsse_register_thyself (subsystype *subsys)
{
    reg_add_sse_preload_cache (LINK_NOVELL, novell_read_cachefile,
			       "novell_read_cachefile");
}

/* 
 * unix_ipxsse subsystem header
 */

#define SSE_MAJVERSION 1
#define SSE_MINVERSION 0
#define SSE_EDITVERSION 1

SUBSYS_HEADER(unix_ipxsse, SSE_MAJVERSION, SSE_MINVERSION, SSE_EDITVERSION,
	      unix_ipxsse_register_thyself, SUBSYS_CLASS_PROTOCOL,
	      "seq: ipx",
	      "req: sse, ipx");
