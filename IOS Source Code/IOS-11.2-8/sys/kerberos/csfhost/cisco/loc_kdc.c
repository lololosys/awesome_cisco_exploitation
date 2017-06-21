/* $Id: loc_kdc.c,v 3.4.28.3 1996/07/24 00:34:27 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/csfhost/cisco/loc_kdc.c,v $
 *------------------------------------------------------------------
 * loc_kdc.c - get socket addresses for KDC.
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: loc_kdc.c,v $
 * Revision 3.4.28.3  1996/07/24  00:34:27  che
 * CSCdi63823:  Multiple Kerberos servers can crash router
 * Branch: California_branch
 * Allocate memory properly for multiple KDCs for any given Kerberos realm.
 *
 * Revision 3.4.28.2  1996/04/19  15:12:20  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.4  1996/04/09  20:51:25  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.1.2.3  1996/04/02  08:56:55  carrel
 * cvs fixes
 *
 * Revision 3.1.2.2  1996/03/05  04:46:57  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.1.2.1  1996/01/23  05:57:30  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 3.4.28.1  1996/03/18  20:31:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.4.1  1996/03/07  10:02:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4  1996/01/13  01:34:02  che
 * CSCdi46854:  Kerberos KDC ports should be definable for each KDC server
 *
 * Revision 3.3  1996/01/04  22:51:24  che
 * CSCdi46133:  Kerberos KDC port should be configurable
 * Added "kerberos kdc-port <port number>" command.
 *
 * In send2kdc.c:
 *
 * Will now wait a minimum of 15 seconds for KDC reply instead of 5 ms.
 *
 * Revision 3.2  1995/11/17  17:45:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright (C) 1993 Open Computing Security Group
 * Copyright 1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#define CSF_HDRI_NET
#include <csfc5/header.h>
#include <csfc5/net/ip.h>
#include "name.h"
#include "kparse.h"
#include <address.h>

extern csf_sts csfC5_get_krbhst(const csfC5_data *, struct krbhst ***);

/*
 * addent()
 *	Add another entry to the socklist provided
 */
static void
addent(struct sockaddr **socklist, int *size, int idx, ipaddrtype ipaddr, 
       int kdc_port)
{
    struct sockaddr *addr_p = NULL;
    struct sockaddr_in *sin_p = NULL;

    /*
     * If we've filled all the current slots, create more.  If we can't
     * get the incremental memory, just return without taking any action.
     */
    addr_p = *socklist;
    if (idx >= *size) {
	addr_p =
	    csfCrealloc(addr_p, sizeof(struct sockaddr) * (*size * 2));
	if (addr_p == NULL) {
	    return;
	}
	*size *= 2;
	*socklist = addr_p;
    }

    /*
     * Point to next slot
     */
    sin_p = (struct sockaddr_in *) &addr_p[idx++];

    /*
     * Fill in the sockaddr_in format
     */
    memset(sin_p, 0, sizeof(*sin_p));
    sin_p->sin_family = AF_INET;
    sin_p->sin_port = kdc_port;
    sin_p->sin_addr.s_addr = ipaddr;

}

/*
 * csfC5_netinet_locate_kdc()
 *	Look up hosts in a realm, return a list
 */
csf_sts
csfC5_netinet_locate_kdc ( const csfC5_data *realm,
	struct sockaddr **addr_pp, int *naddrs)
{
    struct krbhst **hostlist = NULL;
    int code = 0;
    int i = 0, out = 0, count = 0;
    struct sockaddr *addr_p = NULL;

    /*
     * Get list of hosts
     */
    code = csfC5_get_krbhst( realm, &hostlist);
    if (code) {
	free(addr_p);
	return(code);
    }

    /*
     * Count them.  If there are none, return success on an empty list
     */
    for (count = 0; hostlist[count]; count++)
	;
    if (count == 0) {
	*naddrs = 0;
	free(addr_p);
	return CSF_STS_OK;
    }

    /*
     * Get array of sockaddrs to use
     */
    addr_p = csfCmalloc(sizeof(struct sockaddr) * count);
    if (addr_p == NULL) {
	return(CSFC5_STS_ENOMEM);
    }

    /*
     * Walk list of hosts
     */
    for (i = 0, out = 0; hostlist[i]; i++) {
	addrtype address;

	if (numeric_lookup(ADDR_IP, hostlist[i]->k_host, &address)) {
	    addent(&addr_p, &count, out++, address.ip_addr, 
		   hostlist[i]->k_port);
	} else {
	    int j, dummy;
	    nametype *ptr;

	    ptr = name_lookup(hostlist[i]->k_host, &dummy, NAM_IP);
	    if (ptr == NULL) {
		continue;
	    }
	    for (j = 0; j < MAXIPADDRESSES; ++j) {
		if (ptr->v.ip.adr[j].type != ADDR_IP) {
		    continue;
		}
		addent(&addr_p, &count, out++,
		    ptr->v.ip.adr[j].addr.ip_address, hostlist[i]->k_port);
	    }
	}
	csfCfree(hostlist[i]->k_host);
	csfCfree(hostlist[i]);
	hostlist[i] = NULL;
    }
    csf_free(hostlist);

    /*
    * XXX need to distinguish between
    * "can't resolve KDC name" and
    * "can't find any KDC names"
    */
    *addr_pp = addr_p;
    *naddrs = out;

    return(CSF_STS_OK);
}
