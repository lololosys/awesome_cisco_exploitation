/* $Id: tcpcompress.c,v 3.4.38.7 1996/08/21 01:14:22 ahh Exp $
 * $Source: /release/112/cvs/Xsys/tcp/tcpcompress.c,v $
 *------------------------------------------------------------------
 * tcpcompress.c -- Routines for TCP/IP header compression
 *
 * 17-January-1991, Tony Li
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Search for "PERFORMANCE" to find places where performance could be
 * improved. 
 *------------------------------------------------------------------
 * $Log: tcpcompress.c,v $
 * Revision 3.4.38.7  1996/08/21  01:14:22  ahh
 * CSCdi63481:  SegV crash in thc_uncompress_tcp
 * Don't try to uncompress if there is no compression state.
 * Branch: California_branch
 *
 * Revision 3.4.38.6  1996/07/23  13:30:00  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.4.38.5  1996/07/06  05:57:08  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.4.38.4  1996/06/28  23:30:14  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.38.3  1996/06/25  22:47:49  snyder
 * CSCdi61272:  fix embedded University copyrights in tcpcompress and ppp
 * Branch: California_branch
 *      change char* to static const char array, being anal I know...
 *      fixing misspellings did it to me
 *
 * Revision 3.4.38.2  1996/04/15  21:23:47  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.4.38.1  1996/03/18  22:13:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.8.2  1996/03/13  02:03:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.8.1  1996/03/07  10:53:00  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4  1996/01/08  18:29:15  ahh
 * CSCdi38666:  ip tcp header-compression causes %LINK-3-TOOBIG
 * Avoid unnecessary re-encapsulation of non-compressible packets.
 *
 * Revision 3.3  1995/11/17  18:48:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:44:18  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:35:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/28  21:46:48  jenny
 * CSCdi39127:  TCP header compression debugs print negative numbers
 *
 * Revision 2.2  1995/06/17  06:26:32  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.1  1995/06/07  23:05:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 *
 * Some of the code in this file is derived from the code provided in RFC
 * 1144 written by Van Jacobson (van@helios.ee.lbl.gov) and is covered
 * under the following copyright: 
 *
 * Copyright (c) 1989 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#ifndef lint
static const char berkeley_copyright[] =
"Copyright (c) 1989 Regents of the University of California. All rights reserved. Redistribution and use in source and binary forms are permitted provided that the above copyright notice and this paragraph are duplicated in all such forms and that any documentation, advertising materials, and other materials related to such distribution and use acknowledge that the software was developed by the University of California, Berkeley.  The name of the University may not be used to endorse or promote products derived from this software without specific prior written permission. THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.";
#endif

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "interface_private.h"
#include "config.h"
#include "packet.h"
#include "ttysrv.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../ip/ipoptions.h"
#include "tcp_debug.h"
#include "tcpcompress.h"
#include "../wan/serial.h"
#include "../if/network.h"
#include "../if/network_debug.h"
#include "../os/buffers.h"
#include "logger.h"
#include "../parser/parser_defs_tcpip.h"
#include "../ip/ip_registry.h"
#include "subsys.h"

/*
 * thc_enqueue
 * Interrupt level enqueue routine for compressed TCP/IP packets.
 */
void thc_enqueue (paktype *pak)
{
    idbtype *swidb = pak->if_input;
    hwidbtype *idb = swidb->hwptr;

    if (swidb->ip_enabled) {
	if (idb->tcp_compress) {
	    ip_simple_enqueue(pak);
	} else {
	    if (thc_debug || pkt_debug) 
		buginf("\n%s: compressed TCP/IP packet dropped",
		       swidb->namestring); 
	    protocol_discard(pak, TRUE);
	    return;
	}
    } else {
	if (ROUTING_IF(swidb, LINK_IP))
	    pak->flags |= PAK_ABORT;
	protocol_discard(pak, ROUTING_IF(swidb, LINK_IP));
    }
}

/*
 * thc_verify_compress
 * Verify we can compress a TCP packet
 */

boolean thc_verify_compress (paktype *pak)
{
    iphdrtype *ip;
    uint hlen;
    tcptype *th;

    ip = (iphdrtype *)ipheadstart(pak);

    /*
     * Minor additions before we get into the bulk of the existing code.
     * In undoing this encapsulation, we might still be one byte off (see
     * hdlc_encapsulate).  Get the length from ip.
     * Make sure that the packet contains TCP.
     */
    if (ip->prot != TCP_PROT)
	return(FALSE);

    /*
     * Bail if this is an IP fragment or if the TCP packet isn't
     * `compressible' (i.e., ACK isn't set or some other control bit is
     * set).  (We assume that the caller has already made sure the
     * packet is IP proto TCP).
     */
    if ((ip->fo & h2n(0x3fff)) || ip->tl < 40)
	return (FALSE);
    
    hlen = ip->ihl;

    th = (tcptype *) &((int *) ip)[hlen];
    if (th->fin || th->syn || th->rst || !th->ack)
	return (FALSE);
    return(TRUE);
}

/*
 * thc_encapsulation
 */
paktype *thc_encapsulation (hwidbtype *idb, paktype *pak)
{
    /*
     * Only reencapsulate if the packet needs it.
     */
    if (pak->linktype != LINK_COMPRESSED_TCP &&
	pak->linktype != LINK_UNCOMPRESSED_TCP)
	return(pak);

    if (!pak_encap(idb, pak, pak->datagramsize, pak->desthost)) {
	if (thc_debug)
	    buginf("\nTHC %s: uncompressed encapsulation failed",
		pak->if_output->namestring);
	pak->flags |= PAK_ABORT;
	datagram_done((paktype *) pak);
	return(NULL);
    }
    return(pak);
}

/*
 * thc_compress_tcp
 * Compress a TCP/IP packet if possible.  Return the packet after it has
 * been compressed.  If compress_cid is true, we try to compress the
 * connection ID too.
 * NB: idb is used for encapsulation and error messages, but thcdb
 *   is not taken from it.  Thus slip passes a dummy slip idb and
 *   a thcdb taken from the tty data block.
 */
static paktype *thc_compress_tcp (
    hwidbtype *idb,
    paktype *pak,
    boolean compress_cid,
    thcdbtype *thcdb)
{
    struct cstate *cs; 
    iphdrtype *ip;
    tcptype *oth, *th;
    int len, hlen;
    uint deltaS, deltaA, changes;
    uchar new_seq[16], *cp, *dp;
    paktype *newp;
    char *namestring = idb->hw_namestring;

    ip = (iphdrtype *)ipheadstart(pak);

    /*
     * Minor additions before we get into the bulk of the existing code.
     * In undoing this encapsulation, we might still be one byte off (see
     * hdlc_encapsulate).  Get the length from ip.
     * Make sure that the packet contains TCP.
     */
    if (ip->prot != TCP_PROT)
	return(pak);

    /*
     * Bail if this is an IP fragment or if the TCP packet isn't
     * `compressible' (i.e., ACK isn't set or some other control bit is
     * set).  (We assume that the caller has already made sure the
     * packet is IP proto TCP).
     */
    if ((ip->fo & h2n(0x3fff)) || ip->tl < 40)
	return (pak);
    
    hlen = ip->ihl;

    th = (tcptype *) &((int *) ip)[hlen];
    if (th->fin || th->syn || th->rst || !th->ack)
	return (pak);
    cs = thcdb->last_cs->cs_next;
    changes = 0;
    cp = new_seq;

    /*
     * If the packet is shared then it may have all sorts of wierd things
     * going on and we shouldn't play with the packet proper.  Make a copy
     * and continue. 
     */
    if (pak->refcount > 1) {
#ifdef DEBUG
	if (thc_debug)
	    buginf("\nTHC: duplicating locally sourced packet");
#endif
	newp = pak_duplicate(pak);
	if (newp == NULL) {
	    thcdb->buffail++;
	    return(pak);
	}
	datagram_done(pak);
	pak = newp;
	ip = (iphdrtype *)ipheadstart(pak);
	th = (tcptype *) &((int *) ip)[hlen];
    }

    /*
     * Packet is compressible -- we're going to send either a
     * COMPRESSED_TCP or UNCOMPRESSED_TCP packet.  Either way we need
     * to locate (or create) the connection state.  Special case the
     * most recently used connection since it's most likely to be used
     * again & we don't have to do any reordering if it's used.
     */
    thcdb->packets++;
    if (ip->srcadr != cs->cs_ip.srcadr ||
	ip->dstadr != cs->cs_ip.dstadr ||
	*(int *)th != ((int *)&cs->cs_ip)[cs->cs_ip.ihl]) {
	/*
	 * Wasn't the first -- search for it.
	 *
	 * States are kept in a circularly linked list with
	 * last_cs pointing to the end of the list.  The
	 * list is kept in lru order by moving a state to the
	 * head of the list whenever it is referenced.  Since
	 * the list is short and, empirically, the connection
	 * we want is almost always near the front, we locate
	 * states via linear search.  If we don't find a state
	 * for the datagram, the oldest state is (re-)used.
	 */
	struct cstate *lcs;
	struct cstate *lastcs = thcdb->last_cs;
	
	thcdb->searches++;
	do {
	    lcs = cs; cs = cs->cs_next;
	    if (ip->srcadr == cs->cs_ip.srcadr
		&& ip->dstadr == cs->cs_ip.dstadr
		&& *(int *)th == ((int *)&cs->cs_ip)[cs->cs_ip.ihl])
		goto found;
	} while (cs != lastcs);
	
	/*
	 * Didn't find it -- re-use oldest cstate.  Send an
	 * uncompressed packet that tells the other side what
	 * connection number we're using for this conversation.
	 * Note that since the state list is circular, the oldest
	 * state points to the newest and we only need to set
	 * last_cs to update the lru linkage.
	 */
	thcdb->misses++;
	thcdb->last_cs = lcs;
	hlen += th->dataoffset;
	hlen <<= 2;
	/*
	 * Make sure that hlen isn't too large for the cstate struct, and
	 * that there is at least hlen bytes in the packet (we need to subtract
	 * off the encapsulation size from the datagramsize.  Since currently
	 * only the serial encapsulations initialize encsize, we have to do
	 * it the hard way...
	 */
	if ((hlen > MAX_HDR) || (hlen > 
	        pak->datagramsize - (ipheadstart(pak) - pak->datagramstart))) {
	    if (thc_debug)
	        buginf("\nTHC %s: header too long", namestring);
	    return(pak);
	}
	if (thc_debug)
	    buginf("\nTHC %s: new connection, conn %d", namestring,
		   cs->cs_id);  
	goto uncompressed;

    found:
	/*
	 * Found it -- move to the front on the connection list.
	 */
	if (cs == lastcs)
	    thcdb->last_cs = lcs;
	else {
	    lcs->cs_next = cs->cs_next;
	    cs->cs_next = lastcs->cs_next;
	    lastcs->cs_next = cs;
	}
    }
    
    /*
     * Make sure that only what we expect to change changed. The first
     * line of the `if' checks the IP protocol version, header length &
     * type of service.  The 2nd line checks the "Don't fragment" bit.
     * The 3rd line checks the time-to-live and protocol (the protocol
     * check is unnecessary but costless).  The 4th line checks the TCP
     * header length.  The 5th line checks IP options, if any.  The 6th
     * line checks TCP options, if any.  If any of these things are
     * different between the previous & current datagram, we send the
     * current datagram `uncompressed'.
     */
    oth = (tcptype *)&((int *)&cs->cs_ip)[hlen];
    deltaS = hlen;
    hlen += th->dataoffset;
    hlen <<= 2;
    /*
     * Make sure that hlen isn't too large for the cstate struct, and
     * that there is at least hlen bytes in the packet (we need to subtract
     * off the encapsulation size from the datagramsize.  Since currently
     * only the serial encapsulations initialize encsize, we have to do
     * it the hard way...
     */
    if ((hlen > MAX_HDR) || (hlen > 
	    pak->datagramsize - (ipheadstart(pak) - pak->datagramstart))) {
	if (thc_debug)
	    buginf("\nTHC %s: header too long", namestring);
	return(pak);
    }
    
    if (((ushort *)ip)[0] != ((ushort *)&cs->cs_ip)[0] ||
	((ushort *)ip)[3] != ((ushort *)&cs->cs_ip)[3] ||
	((ushort *)ip)[4] != ((ushort *)&cs->cs_ip)[4] ||
	th->dataoffset != oth->dataoffset ||
	(deltaS > 5 && bcmp(ipdatastart(pak),
			    (uchar *)(&cs->cs_ip + 1),
			    (deltaS - 5) << 2)) || 
	(th->dataoffset > 5 && bcmp((uchar *)(th + 1), (uchar *)(oth + 1),
				    (th->dataoffset - 5) << 2))) {
	if (thc_debug)
	    buginf("\nTHC %s: odd header change, conn %d", namestring,
		   cs->cs_id);  
	goto uncompressed;
    }

    /*
     * Figure out which of the changing fields changed.  The
     * receiver expects changes in the order: urgent, window,
     * ack, seq (the order minimizes the number of temporaries
     * needed in this section of code).
     */
    if (th->urg) {
	deltaS = n2h(th->urgentpointer);
	ENCODEZ(deltaS);
	changes |= NEW_U;
    } else if (th->urgentpointer != oth->urgentpointer) {
	/* argh! URG not set but urp changed -- a sensible
	 * implementation should never do this but RFC793
	 * doesn't prohibit the change so we have to deal
	 * with it. */
	if (thc_debug)
	    buginf("\nTHC %s: URG bug, conn %d", namestring,
		   cs->cs_id);  
	goto uncompressed;
    }

    deltaS = (u_short)(n2h(th->window) - n2h(oth->window));
    if (deltaS) {
	ENCODE(deltaS);
	changes |= NEW_W;
    }
    
    deltaA = n2hl(th->acknowledgementnumber) -
	n2hl(oth->acknowledgementnumber);
    if (deltaA) {
	if (deltaA > 0xffff) {
	    if (thc_debug)
		buginf("\nTHC %s: ACK change too large, conn %d",
		       namestring, cs->cs_id);  
	    goto uncompressed;
	}
	ENCODE(deltaA);
	changes |= NEW_A;
    }
    
    deltaS = n2hl(th->sequencenumber) - n2hl(oth->sequencenumber);
    if (deltaS) {
	if (deltaS > 0xffff) {
	    if (thc_debug)
		buginf(
		    "\nTHC %s: SEQ change too large, conn %d, old %u, new %u",
		    namestring, cs->cs_id,
		    n2hl(oth->sequencenumber), n2hl(th->sequencenumber));
	    goto uncompressed;
	}
	ENCODE(deltaS);
	changes |= NEW_S;
    }

    switch(changes) {
	
    case 0:
	/*
	 * Nothing changed. If this packet contains data and the
	 * last one didn't, this is probably a data packet following
	 * an ack (normal on an interactive connection) and we send
	 * it compressed.  Otherwise it's probably a retransmit,
	 * retransmitted ack or window probe.  Send it uncompressed
	 * in case the other side missed the compressed version.
	 */
	if (ip->tl != cs->cs_ip.tl &&
	    n2h(cs->cs_ip.tl) == hlen)
	    break;
	
	if (thc_debug)
	    buginf("\nTHC %s: retransmit or window probe, conn %d",
		   namestring, cs->cs_id);  
	goto uncompressed;
	
    case SPECIAL_I:
    case SPECIAL_D:
	/*
	 * actual changes match one of our special case encodings --
	 * send packet uncompressed.
	 */
	if (thc_debug)
	    buginf("\nTHC %s: special case %d, conn %d", namestring,
		   changes, cs->cs_id);  
	goto uncompressed;
	
    case NEW_S|NEW_A:
	if (deltaS == deltaA &&
	    deltaS == n2h(cs->cs_ip.tl) - hlen) {
	    /* special case for echoed terminal traffic */
	    changes = SPECIAL_I;
	    cp = new_seq;
	}
	break;
	
    case NEW_S:
	if (deltaS == n2h(cs->cs_ip.tl) - hlen) {
	    /* special case for data xfer */
	    changes = SPECIAL_D;
	    cp = new_seq;
	}
	break;
    }
    
    deltaS = n2h(ip->id) - n2h(cs->cs_ip.id);
    if (deltaS != 1) {
	ENCODEZ(deltaS);
	changes |= NEW_I;
    }
    if (th->psh)
	changes |= TCP_PUSH_BIT;
    /*
     * Grab the cksum before we overwrite it below.  Then update our
     * state with this packet's header.
     */
    deltaA = n2h(th->checksum);
    bcopy((uchar *) ip, (uchar *) &cs->cs_ip, hlen);
    if (thc_debug && tcppkt_debug) {
	buginf(
	       "\nTHC %s: output compressed, conn %d, cksum 0x%x, seq %u",
	       namestring, cs->cs_id, th->checksum, th->sequencenumber); 
    }
    
    /*
     * We want to use the original packet as our compressed packet.
     * (cp - new_seq) is the number of bytes we need for compressed
     * sequence numbers.  In addition we need one byte for the change
     * mask, one for the connection id and two for the tcp checksum.
     * So, (cp - new_seq) + 4 bytes of header are needed.  hlen is how
     * many bytes of the original packet to toss so subtract the two to
     * get the new packet size.  Up until this point, we have not written
     * on the packet.  After this, there's no turning back.
     */
    deltaS = cp - new_seq;
    cp = (uchar *) ip;
    dp = cp + hlen;
    len = ip->tl - hlen;
    if (compress_cid == FALSE || thcdb->last_xmit != cs->cs_id) {
	thcdb->last_xmit = cs->cs_id;
	hlen -= deltaS + 4;
	*cp++ = changes | NEW_C;
	*cp++ = cs->cs_id;
    } else {
	hlen -= deltaS + 3;
	*cp++ = changes;
    }
    thcdb->saved += hlen;

    *cp++ = deltaA >> 8;
    *cp++ = deltaA;
    bcopy(new_seq, cp, deltaS);

    /*
     * len is the length of data to copy, if any.  Use bcopy here.
     * Although this overlaps, it's safe.  The minimum TCP/IP header is 40
     * bytes, and the maximum compressed header is 18 bytes.  Thus, the
     * offset is more than a long and we're safe.  It would be nice not to
     * copy the data, but everything expects the entire packet to be
     * aligned to the end of PAKMACRO.
     * PERFORMANCE improvement:  it would be nice if we didn't have to copy
     * the data at all.  Could we just build the new header at the start of
     * the data and then play games with pak->datagramstart?  If so, we
     * can't use hdlc_encapsulate(), so this is painful.
     */
    if (len)
	bcopy(dp, (uchar *) cp + deltaS, len);
    len += deltaS + (cp - (uchar *)ip);
    thcdb->sent += len;
    thcdb->compressed++;

    /*
     * Fix up the link type. 
     */
    pak->linktype = LINK_COMPRESSED_TCP;
    pak->datagramsize = len;
    if (thc_debug)
	buginf("\nTHC %s: output COMPRESSED, conn %d, cksum 0x%x, seq %u",
	       namestring, cs->cs_id, th->checksum, th->sequencenumber);
#ifdef DEBUG
    if (thc_debug && tcppkt_debug)
	bugpak("\nTHC compressed:", pak);
#endif

    return (pak);
    
    /*
     * Update connection state cs & send uncompressed packet ('uncompressed'
     * means a regular ip/tcp packet but with the 'conversation id' we hope
     * to use on future compressed packets in the protocol field).
     */
 uncompressed:
    if (thc_debug)
	buginf("\nTHC %s: output uncompressed, conn %d, cksum 0x%x, seq %u",
	       namestring, cs->cs_id, th->checksum, th->sequencenumber);
    bcopy((uchar *) ip, (uchar *) &cs->cs_ip, hlen);
    ip->prot = cs->cs_id;
    thcdb->last_xmit = cs->cs_id;
    /*
     * Fix up the link type.
     */
    pak->linktype = LINK_UNCOMPRESSED_TCP;
    pak->datagramsize = ip->tl;
    return (pak);
}

/*
 * thc_mini_sanity_check_inline
 * this checks vital parts of the IP header. Note the ipinput
 * thread will not recheck uncompressed TCP-compressed packets.
 * So any missings here cause crashes in dofrag, makehole, etc. etc.
 * Also note this routine is copied from ip_sanity_check_inline, as
 * we don't want to recheck crc and neither wanted to add another
 * parameter to ip_sanity_check_inline...
 */
static inline boolean thc_mini_sanity_check_inline (paktype *pak)
{
    iphdrtype *ip = (iphdrtype *)ipheadstart(pak);

    /*
     * We don't need to check ip checksum. We placed it in the
     * packet
     */

    if ((ip->version != IPVERSION) ||
	(ip->tl > MAXDATAGRAMSIZE) ||
	(ip->tl < MINIPHEADERBYTES) ||
	(ip->ihl < btol(MINIPHEADERBYTES)) ||
	(ip->tl > (pak->datagramsize -
		    (pak->network_start - pak->datagramstart)))) {
	return(FALSE);
    }
    
    if ((ip->ihl > btol(MINIPHEADERBYTES)) || ip_secopt_bytes) {
	if (!ipopt_inproc(pak)) {
	    return(FALSE);
	}
    }

    return(TRUE);
}

/*
 * thc_compress_header
 * Compress header of a packet for output.
 * Called by SERVICE_COMPRESS_HEADER for LINK_IP
 */

static paktype *thc_compress_header(paktype *pak)
{
    hwidbtype *idb;

    idb = pak->if_output->hwptr;
    if (idb->tcp_compress && pak && (idb->tcp_compress->flags & THC_ACTIVE)) {
	pak = thc_compress_tcp(idb, pak, FALSE, idb->tcp_compress);
	if (pak)
	    pak = thc_encapsulation(idb, pak);
    }
    return(pak);
}

/*
 * thc_uncompress_tcp
 * Uncompress a compressed TCP packet.
 * Return a packet (possibly a different pointer) after uncompression.
 * Note: we derive an idb from the packet, but we use it only for
 *  error messages and interface encapsulation checks.  thcdb must
 *  be passed, because for SLIP it comes from the tty data block, not
 *  the idb (which is a dummy).  For other use, idb should be set to
 *  pak->if_input
 */ 
static paktype *thc_uncompress_tcp (paktype *pak, hwidbtype *idb, thcdbtype *thcdb)
{
    uchar *cp;
    uint hlen, changes;
    tcptype *th;
    struct cstate *cs;
    iphdrtype *ip;
    int shift, len;
    uchar *ep, *nep;
    paktype *newp;
    char *namestring = idb->hw_namestring;
#ifdef DEBUG
    paktype *debug_pak = NULL;
#endif

    if (thcdb == NULL) {
	if (thc_debug)
	    buginf("\nTHC %s: no context", namestring);
	return(NULL);
    }

    /*
     * First, check the encapsulation to see if it is worth worrying about.
     */
    if (pak->linktype == LINK_UNCOMPRESSED_TCP) {
	/*
	 * Save the incoming header in the state list.
	 */
	ip = (iphdrtype *)ipheadstart(pak);
	if (ip->prot >= thcdb->rstatecount) 
	    goto bad;
	cs = &thcdb->rstate[thcdb->last_recv = ip->prot];
	hlen = ip->ihl;
	if (thc_debug) {
	    th = (tcptype *) &((int *) ip)[hlen];
	    cs->cs_id = ip->prot;
	    buginf("\nTHC %s: rcv uncompressed, conn %d, cksum 0x%x, seq %u",
		   namestring, ip->prot, th->checksum,
		   th->sequencenumber);
	}
	ip->prot = TCP_PROT;
	ip->version = IPVERSION;
	if (!ip_sanity_check(pak)) {
	    /*
	     * ip_sanity_check does most of the cleanup for us.
	     */
	    if (thc_debug)
		buginf("\nTHC %s: tossing bad packet", namestring);
	    thcdb->flags |= THC_TOSS;
	    thcdb->errorin++;
	    return(NULL);
	}
	thcdb->flags &= ~THC_TOSS;
	if (idb->enctype != ET_PPP) /* PPP negotiates this - don't touch */
	  thcdb->flags |= THC_ACTIVE; 
	hlen += ((tcptype *)&((int *)ip)[hlen])->dataoffset;
	hlen <<= 2;
	bcopy((uchar *) ip, (uchar *) &cs->cs_ip, hlen);
	cs->cs_ip.checksum = 0;		
	cs->cs_hlen = hlen;
	thcdb->uncompressedin++;
	pak->linktype = LINK_IP; /* Now a regular IP packet */
	return (pak);
    } else if (pak->linktype != LINK_COMPRESSED_TCP)
	return(pak);
    /* 
     * We've got a compressed packet. 
     */
#ifdef NOTDEF
    if (thc_debug && tcppkt_debug) {
	buginf("\nTHC %s: rcv compressed packet", namestring);
    }
#endif
    changes = 0;
    if (idb->enctype != ET_PPP)
      thcdb->flags |= THC_ACTIVE;
    thcdb->compressedin++;
    ip = (iphdrtype *)ipheadstart(pak);
    cp = (uchar *) ip;
    len = pak->datagramsize - (pak->network_start - pak->datagramstart);
	if (idb->enctype == ET_HDLC) {
	    vhdlctype *hdlc = (vhdlctype *)pak->datagramstart;

	    if (hdlc->var_hdlcflags & HDLC_PADDED) {
		len--;
		pak->datagramsize--;
	    }
	}
#ifdef DEBUG
    if (thc_debug) {
	debug_pak = pak_duplicate(pak);
    }
#endif

    changes = *cp++;
    if (changes & NEW_C) {
	/* 
	 * Make sure the state index is in range, then grab the state.
	 * If we have a good state index, clear the 'discard' flag. 
	 */
	if (*cp >= thcdb->rstatecount) 
	    goto bad;
	
	thcdb->flags &= ~THC_TOSS;
	thcdb->last_recv = *cp++;
    } else {
	/*
	 * This packet has an implicit state index.  If we've
	 * had a line error since the last time we got an
	 * explicit state index, we have to toss the packet.
	 */
	if (thcdb->flags & THC_TOSS) {
	    thcdb->tossed++;
	    protocol_discard(pak, TRUE);
#ifdef DEBUG
	    if (thc_debug)
		retbuffer(debug_pak);
#endif
	    return (NULL);
	}
    }
    cs = &thcdb->rstate[thcdb->last_recv];
    hlen = cs->cs_ip.ihl << 2;
    th = (tcptype *)&((uchar *)&cs->cs_ip)[hlen];
    th->checksum = h2n((*cp << 8) | cp[1]);
    cp += 2;
    if (changes & TCP_PUSH_BIT)
	th->psh = 1;
    else
	th->psh = 0;
    switch (changes & SPECIALS_MASK) {
    case SPECIAL_I:
	{
	    uint i = n2h(cs->cs_ip.tl) - cs->cs_hlen;
	    th->acknowledgementnumber =
		h2nl(n2hl(th->acknowledgementnumber) + i); 
	    th->sequencenumber = h2nl(n2hl(th->sequencenumber) + i);
	}
	break;
    case SPECIAL_D:
	th->sequencenumber = h2nl(n2hl(th->sequencenumber) +
				  n2h(cs->cs_ip.tl) - cs->cs_hlen);
	break;
    default:
	if (changes & NEW_U) {
	    th->urg = 1;
	    DECODEU(th->urgentpointer);
	} else
	    th->urg = 0;
	if (changes & NEW_W)
	    DECODES(th->window);
	if (changes & NEW_A)
	    DECODEL(th->acknowledgementnumber);
	if (changes & NEW_S)
	    DECODEL(th->sequencenumber);
	break;
    }
    if (changes & NEW_I) {
	DECODES(cs->cs_ip.id);
    } else
	cs->cs_ip.id = h2n(n2h(cs->cs_ip.id) + 1);
    /*
     * At this point, cp points to the first byte of data in the
     * packet and len is the length of data.  cs->cs_ip is the
     * uncompressed header.
     */
    len -= (cp - (uchar *)ip);
    if (len < 0) {
	/*
	 * We must have dropped some characters (crc should detect
	 * this but the old slip framing won't)
	 */
	goto bad;
    }

    /*
     * If the uncompressed packet will fit in this pak, then slide the
     * data down and copy the header into place.  If it won't fit,
     * then get a pak of the right size and set it up.
     */
    if (pak_get_pool_size(pak) >= len + cs->cs_hlen) {
	shift = cs->cs_hlen - (cp - (uchar *) ip);
	ep = cp + len - 1;
	nep = ep + shift;

	/*
	 * It fits, shift the data down (slowly).
	 * PERFORMANCE improvement: RVBCOPY is really slow.  Write a faster
	 * one.  Possibly expand the header in the encapsulation space and
	 * play more games with pak->datagramstart.  This is unlikely to be
	 * easy. 
	 */
	RVBCOPY(ep, nep, len);
	bcopy((uchar *) &cs->cs_ip, (uchar *) ip, cs->cs_hlen);
    } else {
	/*
	 * Allocate a new pak of the appropriate size, copy in the pak
	 * header, the new TCP/IP header and then the data.  Then clean up
	 * lots of little pointers and mark the old one as done.
	 * PERFORMANCE improvement: take advantage of buffer slop, if there
	 * is any.  Convince the interface input routine that the buffer it
	 * should start with should be a little bit bigger.  
	 */
	newp = getbuffer(cs->cs_hlen + len);
	if (newp == NULL) {
	    if (thc_debug)
		buginf("\nTHC %s: getbuffer failed, length=0x%x",
		       namestring, cs->cs_hlen + len);
	    thcdb->buffail++;
	    thcdb->flags |= THC_TOSS;
	    protocol_discard(pak, TRUE);
#ifdef DEBUG
	    if (thc_debug)
		retbuffer(debug_pak);
#endif
	    return(NULL);
	}
	thcdb->bufswaps++;
	pak_copy(pak, newp, 0);

	ip = (iphdrtype *)ipheadstart(newp);
	bcopy((uchar *) &cs->cs_ip, (uchar *) ip, cs->cs_hlen);
	bcopy(cp, (uchar *) ip+cs->cs_hlen, len);
	protocol_discard(pak, TRUE);
	pak = newp;
    }

    /*
     * Recompute the ip header checksum
     */
    ip = (iphdrtype *)ipheadstart(pak);
    cs->cs_ip.tl = ip->tl = len + cs->cs_hlen;
    ip->checksum = ipcrc((ushort *) ip, ip->ihl<<2);
    pak->datagramsize = ip->tl;
    pak->datagramstart = pak->network_start;
    pak->linktype = LINK_IP;	/* Now an IP packet */

    /*
     * At this point sanity check some of the critical
     * fields of IP header. IP checksum can be ignored
     * as we computed it ourselves. See CSCdi25947
     * Note this check doesn't free the packet in case of failure.
     */

    if (!thc_mini_sanity_check_inline(pak)) {

	if (thc_debug) {
	    uchar *ipbyte = ((uchar *) ipheadstart(pak));
	    int i;

	    buginf("\nTHC: Bad TCP compressed Packet from peer, %s: rcv "
		   "decoded, conn %d, cksum 0x%x, seq %u", namestring,
		   thcdb->last_recv, th->checksum, th->sequencenumber);
	    buginf("\nIP header: ");
	    for (i= 0; i<MINIPHEADERBYTES; i++)
		buginf(" 0x%x", ipbyte[i]);
	    buginf("\n");
	}
	goto bad;
    }

    /*
     * This is some legacy debug checks that I don't want
     * to take out at this time.
     */

    if (thc_debug) {
	boolean failed;
	th = (tcptype *) &((int *) ip)[ip->ihl];
	failed = CheckSum((uchar *)th, ip->tl - (ip->ihl<<2), ip->srcadr,
			  ip->dstadr, TCP_PROT);
	if (failed || tcppkt_debug) 
	    buginf("\nTHC %s: rcv decoded, conn %d, cksum 0x%x, seq %u",
		   namestring, thcdb->last_recv, th->checksum,
		   th->sequencenumber); 
	if (failed) {
	    buginf("\nTHC %s: checksum incorrect",
		   namestring); 
#ifdef DEBUG
	    bugpak("\nTHC error: ", debug_pak);
#endif
	}
    }
#ifdef DEBUG
    if (thc_debug)
	retbuffer(debug_pak);
#endif
    return (pak);

 bad:
    if (thc_debug)
	buginf("\nTHC %s: tossing bad packet", namestring);
    thcdb->flags |= THC_TOSS;
    thcdb->errorin++;
    protocol_discard(pak, TRUE);
#ifdef DEBUG
    if (thc_debug)
	retbuffer(debug_pak);
#endif
    return (NULL);
}

/*
 * show_header_compression
 * Print out information on the network interfaces
 */
void show_header_compression (parseinfo *csb)
{
    hwidbtype *target;
    hwidbtype *idb;
    target = hwidb_or_null(GETOBJ(idb,1));

    automore_enable(NULL);
    printf("TCP/IP header compression statistics:");
    FOR_ALL_HWIDBS(idb) {
	if (target && (target != idb)) {
	    continue;
	}
	if (idb->enctype == ET_FRAME_RELAY) {
	    reg_invoke_show_fr_tcpip_header_compression(csb);
	    return;
	} else {
	    show_compression_stats(idb, idb->tcp_compress);
	}
    }
    automore_disable();
}

/*
 * show_compression_stats
 * Print out header compression information for a single interface.
 * Returns -1 if we should quit looking at interfaces.
 */
void show_compression_stats (hwidbtype *idb, thcdbtype *thcdb)
{
    uint percent;

    /*
     * LINE_COUNT is the number of lines that we might want to print out.
     */
#define LINE_COUNT 9
    /*
     * If compression is not enabled for this interface, then skip it
     */
    if (! thcdb)
	return;
    automore_conditional(LINE_COUNT);
    printf("\n  Interface %s:", idb->hw_namestring);
    if (!(thcdb->flags & THC_PERMACTIVE)) {
	printf(" (passive, compression ");
	if (thcdb->flags & THC_SESSION)
	    printf("forced by user)");
	else if (thcdb->flags & THC_ACTIVE)
	    printf("on)");
	else
	    printf("off)");
    };
    printf("\n    Rcvd:    %u total, %u compressed, %u errors",
	   thcdb->uncompressedin + thcdb->compressedin,
	   thcdb->compressedin, thcdb->errorin); 
    printf("\n             %u dropped, %u buffer copies, %u buffer failures",
	   thcdb->tossed, thcdb->bufswaps, thcdb->buffail);
    printf("\n    Sent:    %u total, %u compressed, ", 
	   thcdb->packets, thcdb->compressed); 
    printf("\n             %u bytes saved, %u bytes sent", thcdb->saved,
	   thcdb->sent);  
    if (thcdb->sent) {

	/*
	 * Try to estimate the improvement that we've seen.  Our
	 * "improvement factor" is
	 *
	 *	(total number of bytes sent and saved)
	 *	--------------------------------------
	 *		number of bytes sent
	 *
	 * We calculate this in a fairly crass way.
	 */
	if (thcdb->saved > (MAXUINT / 100)) {
	    percent = thcdb->saved / (thcdb->sent / 100);
	} else
	    percent = thcdb->saved * 100 / thcdb->sent;
	percent += 100;
	printf("\n             %d.%d efficiency improvement factor",
	       percent / 100, percent % 100);
    }
    printf("\n    Connect: %d rx slots, %d tx slots, "
	   "%u long searches, %u misses",
	   thcdb->rstatecount, thcdb->tstatecount,
	   thcdb->searches, thcdb->misses);
    if (thcdb->packets) {

	/*
	 * These calculations are a little bit sloppy, but they're close
	 * enough for now.
	 */
	if (thcdb->packets > (MAXUINT / 100)) {
	    percent = (thcdb->packets - thcdb->misses) /
		(thcdb->packets / 100); 
	} else
	    percent = (thcdb->packets - thcdb->misses) * 100 /
		thcdb->packets;
	printf(
"\n             %d%% hit ratio, five minute miss rate %u misses/sec, "
"%u max",
	       percent, thcdb->missrate, thcdb->maxmissrate);
    }
    printc('\n');
#undef LINE_COUNT
}

/*
 * thc_compress_reset - Reset context and state maximum
 *
 * This routine is used to reset our context and state maximum. The
 * transmit or receive states may be set up to as high as the maximum
 * allocated. We do not actually do any memory allocation here.
 *
 * If either parameter is zero, it is treated as the maximum.
 */

void thc_compress_reset (thcdbtype *thcdb, int tstates, int rstates)
{
    int i;
    struct cstate *tstate;
    
    if (!tstates)
      tstates = thcdb->statecount; /* Default transmit states */

    if (!rstates)
      rstates = thcdb->statecount; /* Default receive states */

    thcdb->tstatecount = tstates;
    thcdb->rstatecount = rstates;
    thcdb->last_cs = tstate = thcdb->tstate;
    thcdb->last_recv = 255;
    thcdb->last_xmit = 255;
    thcdb->flags |= THC_TOSS;
    if (!(thcdb->flags & THC_PERMACTIVE))
      thcdb->flags &= ~THC_ACTIVE;
    thcdb->packets = thcdb->compressed = thcdb->searches = 0;
    thcdb->saved = thcdb->misses = thcdb->uncompressedin = 0;
    thcdb->compressedin = thcdb->errorin = thcdb->tossed = 0;
    thcdb->intmisses = thcdb->missrate = thcdb->maxmissrate = 0;
    thcdb->sent = 0;
    /*
     * Initialize the linked list of connection states.
     */
    for (i = 0; i < tstates; i++, tstate++) {
	tstate->cs_id = i;
	tstate->cs_next = tstate - 1;
    }
    thcdb->tstate->cs_next = thcdb->tstate + tstates - 1;
    thcdb->tstate->cs_id = 0;
}

/*
 * Tell the Compression machine to take a hickup. Set the toss flag
 * For passive compression, go back to no-compression until we get
 * another Compressed packet from the peer.
 * NB: Caller should make sure hwidb, hwidb->tcp_compress are non-null
 */

static inline void thc_hickup (hwidbtype *hwidb)
{
    /*
     * Note for PPP interfaces, THC_PERMACTIVE should be ignored
     * If THC_ACTIVE is turned off for PPP idb's, there is no way
     * to turn it back on, without renegotiation (for passive vj).
     */
    if ( !( (ET_PPP == hwidb->enctype) ||
	   (hwidb->tcp_compress->flags & THC_PERMACTIVE)))
	hwidb->tcp_compress->flags &= ~THC_ACTIVE;
    hwidb->tcp_compress->flags |= THC_TOSS;
}

/*
 * thc_compress_restart
 * Restart the header compression state machine
 */

static void thc_hard_restart (hwidbtype *hwidb)
{
    if (hwidb->tcp_compress)
	thc_compress_reset(hwidb->tcp_compress, 0, 0);
}

/*
 * thc_soft_restart
 * Flag an error so the compression machine starts tossing packets
 * Called by SERVICE_NET_CSTATE whenever the hardware twitches.
 */

static void thc_soft_restart (hwidbtype *hwidb)
{
    if (hwidb->tcp_compress) {
	thc_hickup(hwidb);
    }
}

/*
 * thc_route_adjust
 * If there was a keepalive transition on this interface, flag an 
 * error so the compression machine starts tossing packets
 * Called by SERVICE_ROUTE_ADJUST whenever the software twitches.
 */

static void thc_route_adjust (idbtype *swidb)
{
    hwidbtype *hwidb;

    hwidb = swidb->hwptr;
    if (hwidb && hwidb->keep_transition && hwidb->tcp_compress) 
	thc_hickup(hwidb);
}

/*
 * thc_show_tty
 * Display for "show line" command
 * Called by SERVICE_TTY_SHOW
 */

static void thc_show_tty (tt_soc *tty)
{
    hwidbtype *hwidb;
    thcdbtype *thcdb;

    hwidb = tty->tty_idb;
    if (!hwidb)
	return;
    thcdb = hwidb->tcp_compress;
    if (!thcdb)
	return;
    if (tty->statbits & SLIPMODE) {
	if (thcdb->flags & (THC_ACTIVE|THC_PERMACTIVE|THC_SESSION))
	    show_compression_stats(hwidb, thcdb);
    } else
        printf("\nTCP/IP header compression enabled");
}

/*
 * thc_compress_init
 * Set up the maximum number of connection states to keep around.
 */

static boolean thc_compress_init (
    thcdbtype *thcdb,
    int states)			/* Maximum number of states allowed */
{
    struct cstate *tstate;
    struct cstate *rstate;

    /*
     * Register some functions
     */
    reg_add_raw_enqueue(LINK_UNCOMPRESSED_TCP, thc_enqueue, "thc_enqueue");
    reg_add_raw_enqueue(LINK_COMPRESSED_TCP, thc_enqueue, "thc_enqueue");
    reg_add_compress_header(LINK_IP,thc_compress_header,"thc_compress_header");
    reg_add_fast_setup(thc_hard_restart, "thc_hard_restart");
    reg_add_net_cstate(thc_soft_restart, "thc_soft_restart");
    reg_add_route_adjust(thc_route_adjust, "thc_route_adjust");
    reg_add_tty_show(thc_show_tty, "thc_show_tty");

    /*
     * Record the number of states to keep.  If we already have states
     * around, then discard them and allocate new ones.  Re-initialize the
     * connection.
     */
    if (thcdb->statecount != states) {

	tstate = malloc(sizeof(struct cstate)*states); 
	if (!tstate)
	  return(FALSE);

	rstate = malloc(sizeof(struct cstate)*states);
	if (!rstate) {
	    free(tstate);
	    return(FALSE);
	}

	thcdb->statecount = states;

	if (thcdb->tstate)
	    free(thcdb->tstate);
	if (thcdb->rstate)
	    free(thcdb->rstate);
#ifdef DEBUG
	if (thc_debug)
	    buginf("\nTHC: malloc'ing %d bytes for connection states",
		   2 * sizeof(struct cstate) * states);
#endif
	thcdb->tstate = tstate;
	thcdb->rstate = rstate;
    }
    thc_compress_reset(thcdb, states, states);
    return(TRUE);
}

/*
 * ip_compression_connections_command
 * Set the number of compression connections that will be used on this
 * interface. 
 */
void ip_compression_connections_command (parseinfo *csb)
{
    hwidbtype *idb;

    idb = csb->interface->hwptr;
    if (csb->nvgen) {
	if (idb->tcp_compress) {
	    nv_write(idb->tcp_compress->statecount != DEFAULT_THC_STATES,
		     "%s %d", csb->nv_command, idb->tcp_compress->statecount);
	}
	return;
    }
    if (!idb->tcp_compress) {
	printf("\nInterface %s not configured for header compression",
	       csb->interface->namestring);
	return;
    }

    if (csb->sense) {
	if (!thc_compress_init(idb->tcp_compress, GETOBJ(int,1))) {
	    printf(nomemory);
	}
    } else {
	if (!thc_compress_init(idb->tcp_compress, DEFAULT_THC_STATES)) {
	    printf(nomemory);
	}
    }
}    

/*
 * ip_header_compression_set
 * turn on header compression on an interface.  This is split out so that
 * it can be called from places other than the "ip tcp header..." commmand
 * (eg SLIP) when header compression needs to be turned on.
 */
boolean ip_header_compression_set (
    idbtype *swidb,
    boolean passive,
    boolean sense)
{
    thcdbtype *thcdb;
    hwidbtype *idb = swidb->hwptr;

    if (!sense) {
	thcdb = idb->tcp_compress;
	if (thcdb) {
	    idb->tcp_compress = NULL;
	    if (thcdb->tstate)
	      free(thcdb->tstate);
	    if (thcdb->rstate)
	      free(thcdb->rstate);
	    free(thcdb);
	}

	/*
	 * Possibly turn fast switching back on.
	 */
	reg_invoke_ipcache_setup_interface(idb);
    } else {
	/* Sanity checking */
	if (!swidb->ip_enabled)
	    return(FALSE);
	if (idb->enctype != ET_HDLC && idb->enctype != ET_SLIP && 
	    idb->enctype != ET_PPP && !is_x25(idb))
	    return(FALSE);
	if (!idb->tcp_compress) {
	    thcdb = malloc_named(sizeof(thcdbtype), "TCP thc");
	    if (!thcdb)
		return(FALSE);
	    if (!thc_compress_init(thcdb, DEFAULT_THC_STATES)) {
		free(thcdb);
		return(FALSE);
	    }
	    idb->tcp_compress = thcdb;
	} else {
	    thcdb = idb->tcp_compress;
	    thc_compress_reset(idb->tcp_compress, 0, 0);
	}
	/*
	 * If header compression is on, we need to disable fast switching.
	 */
	if (passive) 
	    thcdb->flags &= ~(THC_PERMACTIVE|THC_ACTIVE);
	else {
	    thcdb->flags |= THC_PERMACTIVE;
	    if (idb->enctype != ET_PPP)
	      thcdb->flags |= THC_ACTIVE;
	}
	reg_invoke_ipcache_setup_interface(idb);
    }
    return(TRUE);
}

/*
 * ip_header_compression_command
 * Config command to enable/disable compression on the interface.
 */
void ip_header_compression_command (parseinfo *csb)
{
    hwidbtype *idb;

    idb = hwidb_or_null(csb->interface);
    if (csb->nvgen) {
	if (idb->tcp_compress) {
	    if (idb->tcp_compress->flags & THC_PERMACTIVE)
		nv_write(TRUE, csb->nv_command);
	    else
		nv_write(TRUE, "%s passive", csb->nv_command);
	}
	return;
    }

    if (idb->enctype == ET_FRAME_RELAY) {
	if (idb->tcp_compress)
	    (void) ip_header_compression_set(csb->interface, FALSE, FALSE);
	reg_invoke_fr_tcpip_header_compression_command(csb);
	return; 
    }

    if (GETOBJ(int,1) == TCP_COMPRESS_OFF)
	csb->sense = FALSE;
    if (!csb->sense) {

	/*
	 * If they said 'no', clean up all our data structures.
	 */
	if (!idb->tcp_compress)
	    return;
	(void) ip_header_compression_set(csb->interface, FALSE, FALSE);
	reg_invoke_interface_cfg_modified(idb);
	return;
    }
    if (is_x25(idb)) {
	printf("\nUse x25 map commands for TCP Header compression over X25");
	return;
    }
    if (!csb->interface->ip_enabled) {
	printf("\nIP not enabled on this interface");
	return;
    }
    if (idb->enctype != ET_HDLC && idb->enctype != ET_SLIP && 
	idb->enctype != ET_PPP) {
	printf("\nHeader compression not supported for this encapsulation");
	return;
    }
    if (!ip_header_compression_set(csb->interface, 
		   GETOBJ(int,1) == TCP_COMPRESS_PASSIVE ? TRUE : FALSE, TRUE))
	printf(nomemory);
    reg_invoke_interface_cfg_modified(idb);
}

/*
 * rarp_init
 * Initialize the THC subsystem
 */

static void thc_init (subsystype *dummy)
{
    reg_add_compress_tcpip_header(LINK_IP, thc_compress_tcp,
	"thc_compress_tcp");
    reg_add_uncompress_tcpip_header(LINK_IP, thc_uncompress_tcp,
	"thc_uncompress_tcp");
    reg_add_hdr_compress_init(LINK_IP, thc_compress_init,
	"thc_compress_init");
    reg_add_show_compression_stats(LINK_IP, show_compression_stats,
	"show_compression_stats");
    tcpcompress_parser_init();
}

/*
 * THC subsystem header
 */

#define THC_MAJVERSION 1
#define THC_MINVERSION 0
#define THC_EDITVERSION  1

SUBSYS_HEADER(thc, THC_MAJVERSION, THC_MINVERSION, THC_EDITVERSION,
	      thc_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      subsys_req_iphost);
