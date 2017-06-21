/* $Id: ipradix.c,v 3.2 1995/11/17 17:34:36 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/ipradix.c,v $
 *------------------------------------------------------------------
 * ipradix.c -- Routing Support: interface to radix code
 *
 * March 1995, Paul Traina
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipradix.c,v $
 * Revision 3.2  1995/11/17  17:34:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:07:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ip/ip.h"
#include "../util/radix.h"

/*
 * ip_radix_nextnode
 * Get the next node in a radix tree that uses IP addresses as the key.
 */

rntype *
ip_radix_nextnode (ipaddrtype address, rntype *head)
{

    rntype         *rn;
    ipradixkeytype key;

    /*
     * Find existing node and go to next node. If existing node has been
     * deleted, we'll hit a node less than the deleted node. In this case
     * go through leafs until we find one just greater than the deleted
     * node.
     */
    rn = rn_search(ipaddr2radixkey(address, &key), head);
    while (!(rn->rn_flags & RNF_ROOT)) {
	if (GETLONG(rn->rn_key+1) > address) 
	    break;
	rn = rn_nextleaf(rn);
    }

    /*
     * Never return the ROOT node. Caller wants real data.
     */
    if (rn->rn_flags & RNF_ROOT) 
	rn = NULL;
    return(rn);
}

/*
 * ip_radix_successor
 *
 * This is a LOOSE successor to the node that we just processed in
 * rn_walktree_timed.  Since the target key passed down is already the
 * IP address of the "next" node, the comparison operation down below
 * is <, not <=.  That's NOT a bug.
 */
rntype *
ip_radix_successor (rntype *rn, caddr_t key)
{
	ipaddrtype prefix = *((char *)key + 1);

	while (!(rn->rn_flags & RNF_ROOT) &&
	       (GETLONG(rn->rn_key+1) < prefix))
	    rn = rn_nextleaf(rn);

	return rn;
}

/*
 * ip_radix_ver_successor
 *
 * This is a LOOSE successor to the node that we just processed in
 * rn_walktree_timed.  Since the target key passed down is already the
 * IP address of the "next" node, the comparison operation down below
 * is <, not <=.  That's NOT a bug.
 *
 * This is the same as the above routine, except it respects
 * version numbers.
 */
rntype *
ip_radix_ver_successor (rntype *rn, u_long version, caddr_t key)
{
    ipaddrtype prefix = *((char *)key + 1);

    while (!(rn->rn_flags & RNF_ROOT) &&
	    (GETLONG(rn->rn_key+1) < prefix))
	rn = rn_nextleaf_version(rn, version);

    return rn;
}

