/* $Id: send2kdc.c,v 3.7.22.1 1996/04/19 15:12:22 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/csfhost/cisco/send2kdc.c,v $
 *------------------------------------------------------------------
 * send2kdc.c - Send packet to KDC for realm;  wait for response, 
 * 		retransmitting as necessary.
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: send2kdc.c,v $
 * Revision 3.7.22.1  1996/04/19  15:12:22  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.6  1996/04/10  23:06:50  lol
 * Branch: IosSec_branch
 *         Incorporate code review comments.
 *
 * Revision 3.1.2.5  1996/04/09  20:51:29  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.1.2.4  1996/04/02  08:56:56  carrel
 * cvs fixes
 *
 * Revision 3.1.2.3  1996/03/05  04:46:58  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:45:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1.2.2  1996/02/21  03:38:02  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 3.1.2.1  1996/01/23  05:57:31  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 3.7  1996/01/24  20:17:30  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.6  1996/01/22  08:07:19  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1996/01/13  01:34:03  che
 * CSCdi46854:  Kerberos KDC ports should be definable for each KDC server
 *
 * Revision 3.4  1996/01/06  20:55:52  che
 * CSCdi46135:  Need more Kerberos debugging messages
 * Added more debugging messages to track Kerberos ticket requests.
 *
 * Revision 3.3  1996/01/04  22:51:25  che
 * CSCdi46133:  Kerberos KDC port should be configurable
 * Added "kerberos kdc-port <port number>" command.
 *
 * In send2kdc.c:
 *
 * Will now wait a minimum of 15 seconds for KDC reply instead of 5 ms.
 *
 * Revision 3.2  1995/11/17  17:45:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright (C) 1993 Open Computing Security Group
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#define CSF_HDRI_NET
#include <csfc5/header.h>
#include <csfc5/net/ip.h>
#include "../ip/ip.h"
#include <address.h>
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"

extern csf_sts csfC5_netinet_locate_kdc(const csfC5_data *, struct sockaddr **, int *);

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x, y) (((x) >= (y)) ? (x) : (y))
#endif

/*
 * csfC5_net_sendto_kdc()
 *	Send data to one of the KDCs for a realm
 *
 * If the message is sent and a response is received, CSF_STS_OK is returned,
 * otherwise an error code is returned.
 *
 * The storage for 'reply' is allocated and should be freed by the caller
 * when finished.
 */
csf_sts
csfC5netSendtoKdc(const csfC5_data *msg,
	const csfC5_data *realm, csfC5_data *reply)
{
    int try, naddr, max_dgram_size, skdc_timeout, skdc_max_retry,
	skdc_max_kdc, cc, host;
    boolean haveReply;
    uint start = 0, waitlen = 0;
    struct sockaddr *addr = NULL;
    ipsocktype *sock = NULL;
    csf_sts retval = 0;
    struct sockaddr_in *sp = NULL;
    paktype *pakout, *pakin = NULL;
    udptype *udp = NULL;
    uchar *data = NULL;

    /*
     * Initialize some locals
     */
    haveReply = FALSE;
    addr = NULL;
    sock = NULL;
    retval = CSF_STS_OK;
    max_dgram_size = (int) csfC5_getcfgnum( CSF_CFG_NET_MAX_DGRAM_SIZE );
    skdc_timeout = (int) csfC5_getcfgnum( CSFC5_CFG_NET_SKDC_TIMEOUT );
    skdc_max_retry = (int) csfC5_getcfgnum( CSFC5_CFG_NET_SKDC_MAX_RETRY );
    skdc_max_kdc = (int) csfC5_getcfgnum( CSFC5_CFG_NET_SKDC_MAX_KDC );

    /*
     * Pre initialize the reply
     */
    reply->dl = 0;
    reply->dp = NULL;

    /*
     * Find the KDC location(s) for realm
     */
    retval = csfC5_netinet_locate_kdc(realm, &addr, &naddr);
    if (retval == CSF_STS_OK) {
	if (naddr == 0) {
	    retval = CSFC5_STS_REALM_UNKNOWN;
	}
    }

    /*
     * Allocate the reply
     */
    if (retval == CSF_STS_OK) {
	reply->dl = max_dgram_size;
	reply->dp = (void *) csf_malloc(max_dgram_size);
	if (reply->dp == NULL) {
	    retval = CSFC5_STS_ENOMEM;
	}
    }

    /*
     * Get the socket
     */
    if (retval == CSF_STS_OK) {
	sock = open_ipsocket(UDP_PROT, NULL, NULL, 0, 0, SOCK_SRC_DYN);
	if (sock == NULL) {
	    retval = CSFC5_STS_ENOMEM;
	}
    }

    /*
     * Do DNS style backoff
     */
    for (try = 0; !retval && !haveReply && (try < skdc_max_retry); ++try) {
	for (host = 0; (retval == CSF_STS_OK) && !haveReply && (host < naddr);
		host++)
	{
	    /*
	     * Only AF_INET handled
	     */
	    sp = (void *)(addr+host);
	    if (sp->sin_family != AF_INET) {
		continue;
	    }

	    /*
	     * Fill in the socket with the latest addr/port.  We have
	     * specified automatic calculation of source addr, so we
	     * don't have to bother with that ourselves.
	     */
	    sock->faddress.ip_addr = sp->sin_addr.s_addr;
	    sock->faddress.type = ADDR_IP;
	    sock->fport = sp->sin_port;

	    /*
	     * Set up a packet to fire off
	     */
	    pakout = setup_ipsocket(sock, msg->dl, &start);
	    if (pakout == NULL) {
		retval = CSFC5_STS_ENOMEM;
		break;
	    }
	    memcpy(pakout->data_area + start, msg->dp, msg->dl);

	    /*
	     * Send it
	     */
	    write_ipsocket(sock, pakout, msg->dl);

	    if (kerberos_debug) {
		buginf("\nKerberos:\tSent TGT request to KDC");
	    }

	    /*
	     * Compute wait time. This is derived from DNS resolver source
	     */
	    waitlen = skdc_timeout << try;
	    if (try > 0) {
		waitlen /= MIN(naddr, skdc_max_kdc);
		waitlen = MIN(waitlen, 60000);
	    }
	    waitlen = MAX(waitlen, 15000);

	    /*
	     * Wait for data back or timeout.  There doesn't appear to
	     * be any way to abort a read on an unreachable, might need
	     * further thought.
	     */
	    pakin = read_ipsocket(sock, TRUE, waitlen, &udp, &data);
	    if (pakin) {

		if (kerberos_debug) {
		    buginf("\nKerberos:\tReceived TGT reply from KDC");
		}

		/*
		 * We might consider here verifying that the reply
		 * came from one of the KDC's listed for that haddr type,
		 * but that check can be fouled by some implementations of
		 * some network types which might show a loopback return
		 * haddr, for example, if the KDC is on the same host
		 * as the client.
		 */
		cc = MIN(udp->length - UDPHEADERBYTES, reply->dl);
		reply->dl = cc;
		memcpy(reply->dp, data, cc);
		haveReply = TRUE;
		retbuffer(pakin);
	    }
	}
    }

    /*
     * If an error did not occur but we don't have a reply,
     * then none of the KDCs (if there is any) are reachable.
     */
    if (!retval && !haveReply) {
	retval = CSFC5_STS_KDC_UNREACH;
    }

    /*
     * Clean up.  NB, these routines handle NULL pointers.
     */
    close_ipsocket(sock);
    csf_free(addr);

    /*
     * If there was an error release the reply buffer
     */
    if (retval != CSF_STS_OK) {
	csf_free(reply->dp);
	reply->dl = 0;
	reply->dp = NULL;
    }

    return(retval);
}
