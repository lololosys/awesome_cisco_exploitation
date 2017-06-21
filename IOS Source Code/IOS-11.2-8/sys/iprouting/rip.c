/* $Id: rip.c,v 3.11.4.7 1996/08/28 12:53:38 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/rip.c,v $
 *------------------------------------------------------------------
 * rip.c -- Routing Information Protocol, aka "routed"
 *
 * 11-December-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rip.c,v $
 * Revision 3.11.4.7  1996/08/28  12:53:38  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.4.6  1996/07/31  19:37:26  dawnli
 * CSCdi63289:  major network summary not sent by rip over unnumbered link
 * Branch: California_branch
 *
 * Revision 3.11.4.5  1996/07/03  01:04:17  dawnli
 * CSCdi61892:  change debug ip rip to show sending to subnet broadcast
 * Branch: California_branch
 *
 * Revision 3.11.4.4  1996/05/23  07:37:59  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.11.4.3  1996/05/19  18:10:56  dawnli
 * CSCdi56737:  RIP fails to send when not using 255.255.255.255 as
 * broadcast
 * Branch: California_branch
 *
 * Revision 3.11.4.2  1996/05/04  01:22:17  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.11.4.1  1996/03/18  20:28:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.2.3  1996/03/16  06:58:04  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.8.2.2  1996/03/07  09:50:13  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.2.1  1996/02/20  01:04:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/03/09  02:11:52  pst
 * CSCdi51161:  RIPv1 should not accept RIPv2 directed to mcast
 *
 * Revision 3.10  1996/01/24  20:16:42  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.9  1996/01/24  03:57:46  pst
 * CSCdi47396:  RIP accepts version 0 packets
 *
 * Revision 3.8  1996/01/22  06:14:50  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.7  1996/01/03  03:32:24  ahh
 * CSCdi45528:  Move key manager out of core
 *
 * Revision 3.6  1995/12/10  00:35:11  pst
 * CSCdi45335:  Make RIPv2 less conservative about mask guesing and fix
 * default
 *
 * Revision 3.5  1995/12/02  10:13:00  pst
 * CSCdi44892:  RIP: do clean up pass on authentication and version
 * control code
 *
 * Revision 3.4  1995/11/26  08:17:53  mmcneali
 * CSCdi43630:  ISL VLAN Multicast changed--IOS gs7 images are DEAF
 * Do not write rip authentication message on subinterface
 * if it's not doing IP routing.
 *
 * Revision 3.3  1995/11/22  17:48:46  albright
 * CSCdi43791:  Helper addresses don't work over unnumbered interfaces
 *
 * Revision 3.2  1995/11/17  17:36:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:10:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:06:42  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/10/20  18:06:13  albright
 * CSCdi42154:  ip helper-address fails on unnumbered interfaces
 *
 * Revision 2.6  1995/09/25  08:31:08  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.5  1995/08/31  20:37:11  tli
 * CSCdi39400:  RIP: default not split horizoned reasonably
 * Split horizon default even if it's not caused by 0.0.0.0
 *
 * Revision 2.4  1995/08/30  18:49:53  klmj
 * CSCdi35234:  Router responds to a rtquery with a TTL of 2.  Unicast RIP
 * Requests will now generate RIP Responses with a TTL of 255.
 *
 * Revision 2.3  1995/08/03  07:42:30  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.2  1995/06/28 09:27:55  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 * Revision 2.1  1995/06/07  21:10:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "address.h"
#include "registry.h"
#include "sys_registry.h"
#include "logger.h"
#include "../iprouting/msg_rip.c"	/* Not a typo, see logger.h */
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "login_public.h"
#include "access.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../ip/ipaccess.h"
#include "../if/network.h"
#include "route.h"
#include "route_inlines.h"
#include "iprouting_debug.h"
#include "rip.h"
#include "route_map.h"
#include "../os/clock.h"
#include "../crypto/keyman.h"
#include "../util/md5.h"

static watched_queue    *RIPsendQ;              /* RIP send queue */
static watched_boolean  *RIPsendQ_empty;        /* RIP send queue empty */
static int              rip_send_pid;           /* RIP send process ID */

static id_string const rip_auth_modes[] = {
    { RIP_AUTH_TYPE_TEXT,	"text" },
    { RIP_AUTH_TYPE_MD,		"md5" },
    { 0, NULL }
};

#define	MAXKEYS	16		/* maximum different key-ID's available */


/*
 * rip1_subnet_test
 *
 * RIP can advertise routes with masks matching the interface and host
 * routes.
 *
 * But if the host route specifies a host number of zero, it will appear
 * like a subnet route, and will be confused with the legitimate subnet
 * route that may be getting advertised.
 */

static boolean
rip1_subnet_test (ipaddrtype mask, pdbtype *pdb, ndbtype *sdb)
{
    if ((sdb->attrflags & NET_SUBNET)
	&& (sdb->netsmask != mask)) {
	if (sdb->netsmask != MASK_HOST)
	    return(FALSE);
	else if ((sdb->netnumber & ~mask) == 0)
	    return(FALSE);
    }

    return(TRUE);
}

/*
 * rip1_network_test
 *
 * RIP version 1 can't handle supernets (default done elsewhere).
 */
static boolean
rip1_network_test (ipaddrtype mask, pdbtype *pdb, ndbtype *sdb)
{
    if (sdb->attrflags & NET_SUPERNET)
	return (FALSE);

    return(TRUE);
}

/*
 * rip2_network_test
 *
 * RIP version 2 can handle supernets, but default is done elsewhere.
 * Check the summary queue and block any routes that are being summarized.
 */
static boolean
rip2_network_test (ipaddrtype mask, pdbtype *pdb, ndbtype *sdb)
{
    if ((sdb->netnumber == 0) && (sdb->netsmask == 0))
	return (FALSE);

    return (TRUE);
}

/*
 * rip_metric
 * Compute the RIP metric.  We have already done the split horizon
 * checking, so we know that none of rdb's match the interface on
 * which we are advertising.
 */

static long
rip_metric (pdbtype *pdb, ndbtype *ndb, idbtype *interface,
	    ipaddrtype dest, pdbtype *src_pdb)
{
    rdbtype *rdb;
    idbtype *idb, *best;
    int i, value;
    int map_metric = 0;
    ulong proctype;
    pdbtype *dist;
    ipaddtype *secptr;
    route_maptype *map;
    ndbtype *sdb;
    boolean map_sets_metric, match;
    ipaddrtype dummy;

    /*
     * No network pointer, return infinity
     */
    if (!ndb)
	return(RIP_MAXMETRIC);

    /*
     * Check if the network is already inaccessible.
     */
    if (ndb->metric == METRIC_INACCESSIBLE)
	return(RIP_MAXMETRIC);

    /*
     * First find the idb that we would use to get to the network.
     * If this is a subnetted network, try to
     * find the first interface to that network that is up.
     * For other networks, search the route descriptor list for an interface.
     */
    best = NULL;
    if (ndb->attrflags & NET_SUBNETTED) {
	FOR_ALL_SWIDBS(idb) {
	    /*
	     * Check interfaces to find ones that are directly connected to
	     * the major net.
	     */
	    match = FALSE;
	    if (!idb->ip_enabled)
		continue;
	    if (idb->ip_majornet == ndb->netnumber) {
		match = TRUE;
	    } else {
		for (secptr = idb->ip_addressQ.qhead; secptr;
		     secptr = secptr->next) {
		    if (get_majornet(secptr->address) == ndb->netnumber) {
			match = TRUE;
			break;
		    }
		}
	    }
	    if (match) {
		best = idb;
		if (interface_up(idb))
		    break;
	    }
	}
    } else {
	for (i = 0, idb = NULL; i < ndb->routecount; i++) {
	    rdb = ndb->rdb[i];
	    idb = rdb ? rdb->idb : NULL;
	    if (idb) {
		best = idb;
		if (interface_up(idb))
		    break;
	    }
	}
	if (!best) {
	    dummy = ndb->netnumber;
	    best = route_lookup(ndb, &dummy, dummy, ROUTINGDEPTH,
			        IP_PATH_USE_IF_GUESS, NULL);
	}
    }
    
    /*
     * If we couldn't find an up interface, say inaccessible.
     * Otherwise determine a metric for this route.
     */
    proctype = src_pdb->proctype;
    value = RIP_MAXMETRIC;
    if (best && !interface_up(best))
	value = RIP_MAXMETRIC;
    else if ((proctype & PROC_RIP) && !(ndb->attrflags & NET_SUBNETTED))
	value = min(RIP_MAXMETRIC, ndb->metric + 1);
    else {
	map = pdb->route_map;
	map_sets_metric = map &&
	    (map->set_list.set_flags & ROUTE_MAP_METRIC_SET) &&
		!map->set_list.metric.igrp_metric;
	if (!ip_def_metric_configured(pdb, src_pdb->index) &&
	    !map_sets_metric) {
	    if (proctype & PROC_STATIC ||
		(ndb->attrflags & NET_ATTACHED))
		value = 1;
	} else {
	    if (ip_def_metric_configured(pdb, src_pdb->index)) {
		map_metric = ip_get_def_metric_scalar(pdb, src_pdb->index);
	    }
	    if (map_sets_metric)
		map_metric = map->set_list.metric.value.scalar;
	    value = min(RIP_MAXMETRIC, max(1, map_metric));
	}
    }
    
    /*
     * We now have the natural metric value.  See if we should add to it.
     */
    value += offset_metric(pdb, interface, ndb->netnumber, OUT);
    if (value > RIP_MAXMETRIC)
	value = RIP_MAXMETRIC;
    
    /*
     * If we're looking at a major ndb, which is NOT redistributed into
     * RIP, and it's variably subnetted and we learned a summary route via
     * another routing protocol, we would generate an infinite metric.  In
     * this case, find the summary route and recurse.
     */
    if ((value == RIP_MAXMETRIC) &&
	(ndb->attrflags & NET_SUBNETTED) &&
	(ndb->attrflags & NET_VARIABLE)) {
	sdb = subnet_lookup(ndb, ndb->netnumber,
			    get_majormask(ndb->netnumber));
	if (sdb) {
	    dist = INDEX2PDB(sdb->pdbindex);
	    value = rip_metric(pdb, sdb, interface, dest, dist);
	}
    }

    /*
     * If we're looking at a dummy ndb and still don't have something,
     * we're in a world of hurt.  Walk all subnets looking for
     * something, anything that would give us a good metric.
     */
    if ((value == RIP_MAXMETRIC) &&
      (ndb->attrflags & NET_SUBNETTED)) {
      for (i = 0; i < NETHASHLEN; i++) {
          for (sdb = ndb->subnettable[i]; sdb; sdb = sdb->next) {
              if (!validnextnetwork(pdb, ndb, sdb, interface, 0,
                                    dest, NULL, FALSE,
				    &dist))
                  continue;
              value = rip_metric(pdb, sdb, interface, dest, dist);
              if (value != RIP_MAXMETRIC)
                  return(value);
          }
      }
    }
    return(value);
}

/*
 * rip_holddown
 * If the new metric is RIP_MAXMETRIC or higher,
 * we want to go into holddown.
 */
static boolean rip_holddown (pdbtype *pdb, ndbtype *ndb, rdbtype *rdb,
                             ulong newmetric)
{
    if (rdb == NULL)
        return(TRUE);
    if (newmetric >= RIP_MAXMETRIC)
        return(TRUE);
    return(FALSE);
}

/*
 * rip_send_version & rip_recv_version
 *
 * If we're not global, use the interface specific parameter.
 * If we are global, pdb->version can be either 0, 1, or 2.
 * 0 = send RIPv1, receive any
 * 1 = send RIPv1, receive RIPv1 only
 * 2 = send RIPv2, receive RIPv2 only
 */

static inline boolean
rip_send_version (pdbtype *pdb, idbtype *idb, int version)
{
    int vers = idb->ip_info->rip_send_vers;

    if (vers)
	return (1<<version) & vers;
    else {
	switch (pdb->version) {
	case 0:
	case 1:
		return version == 1;
	case 2:
		return version == 2;
	default:
		return FALSE;
	}
    }
}

static inline boolean
rip_recv_version (pdbtype *pdb, idbtype *idb, int version)
{
    int vers = idb->ip_info->rip_recv_vers;

    if (vers)
	return (1<<version) & vers;
    else {
	switch (pdb->version) {
	case 0:
		return version >= RIP_MIN_VERSION &&
		       version <= RIP_MAX_VERSION;
	case 1:
		return version == 1;
	case 2:
		return version == 2;
	default:
		return FALSE;
	}
    }
}

/*
 * rip_destination_address
 *
 * Figure out where we should send packets based upon mode.
 * The return value will be passed to ipwrite(),
 * can only be either -1L or multicast.
 */

static inline ipaddrtype
rip_destination_address (int version)
{
    if (version < 2)
	return -1L;
    else
	return ALLRIPROUTERS;
}

/*
 * rip_real_destination
 *
 * Figure out the real destination based upon mode.
 * The return value is used for debug purpose.
 */
static ipaddrtype rip_real_destination (int version, idbtype * idb,
                                        ipaddrtype destination)
{
    if (destination == -1L) {

        if (version < 2)
            return idb->ip_broadcast;
        else
            return ALLRIPROUTERS;
    }
    return destination;
}

static boolean rip_authority_needed (idbtype *idb, char *key, int *id)
{
    char *keys[MAXKEYS];
    int numkeys;

    bzero(keys, sizeof keys);

    numkeys = reg_invoke_key_get_live_keys
      (idb->ip_info->rip_auth_keychain, NULL, keys, MAXKEYS, 
       KEY_SEND_LIFETIME);

    if (numkeys == 0) {
      reg_invoke_key_free_keys(keys, numkeys);          
      return(FALSE);
    }

    sstrncpy(key, keys[0], RIP_AUTH_MD5_LEN);
    *id = reg_invoke_key_get_key_id(idb->ip_info->rip_auth_keychain, key);
    reg_invoke_key_free_keys(keys, numkeys);

    if (*id == -1L) 
      return(FALSE);

    return(TRUE);
}

/*
 * rip_auth_present
 *
 * does this packet contain authentication information?
 */
static inline boolean
rip_auth_present (riptype *rip)
{
    return (rip->version > 1) && (rip->entry[0].family == RIP_AUTH_ID);
}

/*
 * rip_auth_add_prefix
 * Add authentication information to beginning of rip message
 */

static int
rip_auth_add_prefix (riptype *rip, idbtype *idb)
{
    ripauthtype *a_rip = (ripauthtype *) rip;
    int numkeys, id = 0;
    char *keys[MAXKEYS];
    char key[RIP_AUTH_MD5_LEN];

    bzero(keys, sizeof keys);
    numkeys = reg_invoke_key_get_live_keys
	(idb->ip_info->rip_auth_keychain, NULL, keys, MAXKEYS,
	 KEY_SEND_LIFETIME);

    if (numkeys == 0) {
      reg_invoke_key_free_keys(keys, numkeys);          
      return(FALSE);
    }


    sstrncpy(key, keys[0], RIP_AUTH_MD5_LEN);

    id = reg_invoke_key_get_key_id(idb->ip_info->rip_auth_keychain, key);

    if (id == -1L) {
      reg_invoke_key_free_keys(keys, numkeys);      
      return 0;
    }

    reg_invoke_key_free_keys(keys, numkeys);      
    a_rip->authid   = RIP_AUTH_ID;
    a_rip->authtype = idb->ip_info->rip_auth_mode;
    switch (a_rip->authtype) {
    case RIP_AUTH_TYPE_TEXT:
	memset(a_rip->u.authtext, 0, sizeof a_rip->u.authtext);
	strncpy(a_rip->u.authtext, key, sizeof(a_rip->u.authtext));
	return sizeof(ripentrytype);

    case RIP_AUTH_TYPE_MD:
	a_rip->u.md.key_id    = id;
	a_rip->u.md.auth_len  = sizeof(ripmdsuffixtype);
	a_rip->u.md.sequence  = idb->ip_info->rip_auth_sequence++;
	a_rip->u.md.reserved1 = 0;
	a_rip->u.md.reserved2 = 0;
	return sizeof(ripentrytype);
    }

    return 0;
}

/*
 * rip_auth_add_suffix
 * this routine adds the appropriate key to the end of the rip message
 * then computes the digest.
 */

static int
rip_auth_add_suffix (riptype *rip, idbtype *idb, int entries)
{
    ripauthtype *a_rip = (ripauthtype *) rip;
    ripmdsuffixtype *ptr;
    MD5_CTX mdcontext;
    boolean authority;
    int id, bytes;
    char key[RIP_AUTH_MD5_LEN];
    
    authority = rip_authority_needed(idb, key, &id);
    if (!authority)
      return 0;

    switch (a_rip->authtype) {
    /*
     * For keyed MD5, we need to fix up the "packet length" field
     * and then add in the magic numbers specified by the RFC and
     * calculate the message digest over the message data and key.
     */
    case RIP_AUTH_TYPE_MD:
	a_rip->u.md.pkt_length = sizeof(ripauthtype) +
				 entries * sizeof(ripentrytype);


	ptr = (ripmdsuffixtype *) &a_rip->entry[entries];
	memset(ptr->digest, 0, RIP_AUTH_MD5_LEN);
	ptr->magic1 = 0xFFFF;
	ptr->magic2 = 0x0001;
 	strncpy(&ptr->digest[0], key, RIP_AUTH_MD5_LEN);
  
  	/* compute MD5 over the entire mess */
	bytes = a_rip->u.md.pkt_length + sizeof(ripmdsuffixtype);
 	MD5Init(&mdcontext);
 	MD5Update(&mdcontext, (uchar *)a_rip, bytes);
 	MD5Final(&ptr->digest[0], &mdcontext);
	return sizeof(ripmdsuffixtype);
    }
    return 0;
}

/*
 * rip_auth_check
 * Check packet for authentication validity
 *
 * If the router is not configured to authenticate RIP-2 messages, then
 * RIP-1 and unauthenticated RIP-2 messages will be accepted;  authenticated
 * RIP-2 messages shall be discarded.  If the router is configured to
 * authenticate RIP-2 messages,  then RIP-1 messages and
 * RIP-2 messages shall be discarded.  For maximum security, RIP-1 messages
 * -should- be ignored when authentication is in use.
 */
static boolean
rip_auth_check (riptype *rip, idbtype *idb)
{
    static char auth_data[RIP_AUTH_TEXT_DATA_LEN+1];
    ripauthtype *a_rip = (ripauthtype *) rip;
    ripmdsuffixtype *ptr; 
    ipinfotype  *info  = idb->ip_info;
    char        *auth_key, *keys[MAXKEYS];
    int		numkeys, i, id, bytes;
    char        md5_digest_in[RIP_AUTH_MD5_LEN], output[RIP_AUTH_MD5_LEN];
    MD5_CTX mdcontext;

    i = 0;auth_key = NULL;bytes = 0;numkeys = 0;
    if (a_rip->version == 1)
	return TRUE;

    if (rip_auth_present(rip)) {
	if (!info->rip_auth_keychain)
	    return FALSE;

	if (info->rip_auth_mode != a_rip->authtype)
	    return FALSE;

	memset(keys, 0, sizeof keys);
	numkeys = reg_invoke_key_get_live_keys(info->rip_auth_keychain, NULL,
					       keys, MAXKEYS,
					       KEY_ACCEPT_LIFETIME);
	if (numkeys ==  0) {
	  reg_invoke_key_free_keys(keys, numkeys);
	  return FALSE;
	}

	/* 
	 *  to prevent DOS attacks, we check the key id.  if id == 0,
	 *  that means (1) there was no key id associated with validkey.
	 *  (2) validkey has an id == 0.  (A corner case)
	 */

	id = a_rip->u.md.key_id;
	reg_invoke_key_text(info->rip_auth_keychain, id, &auth_key);
	if (!reg_invoke_key_alive_now(info->rip_auth_keychain, id, 
				      KEY_ACCEPT_LIFETIME) || 
				      auth_key == NULL) {
	  reg_invoke_key_free_keys(keys, numkeys);
	  free(auth_key);
	  return(FALSE);
	}
	switch (a_rip->authtype) {
	case RIP_AUTH_TYPE_TEXT:
	    if (rip_debug)
		buginf("\nRIP: received packet with "
		       "text authentication %s", a_rip->u.authtext);
		memset(auth_data, 0, sizeof auth_data);
		sstrncpy(auth_data, auth_key, RIP_AUTH_TEXT_DATA_LEN);
		if (!bcmp(a_rip->u.authtext, auth_data,
			      RIP_AUTH_TEXT_DATA_LEN)) {
		  reg_invoke_key_free_keys(keys, numkeys); 
		  return TRUE;
		}
	    break;
	case RIP_AUTH_TYPE_MD:
	    if (rip_debug)
		buginf("\nRIP: received packet with MD5 authentication");
	    ptr = (ripmdsuffixtype *) ((char *)a_rip + a_rip->u.md.pkt_length);

	    bcopy(ptr->digest, md5_digest_in, RIP_AUTH_MD5_LEN);
	    bzero(ptr->digest, RIP_AUTH_MD5_LEN);
	    sstrncpy(&ptr->digest[0], auth_key, RIP_AUTH_MD5_LEN);
	    bytes = a_rip->u.md.pkt_length + sizeof(ripmdsuffixtype);
	    MD5Init(&mdcontext);
	    MD5Update(&mdcontext, (uchar *)a_rip, bytes);
	    MD5Final(&output[0], &mdcontext);
	    free(auth_key);
	    reg_invoke_key_free_keys(keys, numkeys);
	    if (!bcmp(md5_digest_in, output, RIP_AUTH_MD5_LEN))
	      return TRUE;
	}
	free(auth_key);
	reg_invoke_key_free_keys(keys, numkeys);

    } else
	if (!info->rip_auth_keychain)
	    return TRUE;

    return FALSE;
}

/*
 * rip_validate_msg
 *
 * RFC1058 says that if we get a datagram who's version number is one,
 * all fields described as "must be zero" are to be checked.  If any
 * such field contains a non-zero value, the entire message is to be
 * ignored.
 *
 * We allow REQUESTS with garbage in them, since older cisco routers
 * generate requests with random data in the unused fields.
 */

static boolean
rip_validate_msg (iphdrtype *ip, riptype *rip)
{
    ripentrytype *entry;
    int size;

    if (rip->reserved)
	return FALSE;

    if (rip->opcode == RIPCMD_REQUEST)
	return TRUE;

    if (rip->version > 1)
	return TRUE;

    entry = rip->entry;
    size = ip->tl - ((char *)entry - (char *)ip);

    if (rip_auth_present(rip)) {
	entry++;

	if (((ripauthtype *)rip)->authtype == RIP_AUTH_TYPE_MD)
	    size -= sizeof(ripentrytype) + sizeof(ripmdsuffixtype);
	else
	    size -= sizeof(ripentrytype);
    }

    while (size >= sizeof(ripentrytype)) {
	if (entry->tag || entry->mask || entry->nexthop)
	    return FALSE;
	size -= sizeof(ripentrytype);
	entry++;
    }

    if (size)		/* size should be zero or we have trailing junk */
	return FALSE;	/* this is an anvil test and I think it's stupid */

    return TRUE;
}

/*
 * rip_sendupdate_version
 * Send a RIP update of the appropriate version and authentication key.
 * We update the metric (hop count) at this point instead of at
 * time of reception.
 */
static void
rip_sendupdate_vk (int version, pdbtype *pdb, ndbtype *ndb, 
		   ipaddrtype destination, idbtype *interface, ulong port, 
		   ipaddrtype source, ipaddrtype mask, uchar ttl)
{
    paktype *pak;
    riptype *rip;
    int prev, subprev, sumprev, nbytes, routecount, i, rip_header_size;
    int ndb_bucket, sdb_bucket, rip_suffix_size;
    boolean done, doingsubnets, diddefault, doingnetworks, first, checked;
    ndbtype *sdb, *net, *ndb_ptr, *sdb_ptr, *sum_db;
    rdbtype *rdb;
    ripentrytype *ptr;
    udptype *udp;
    ipaddrtype dest, primary, sourcemask;
    ipaddtype *secptr;
    pdbtype *src_pdb;
    ripsendtype *args;

    prev = 0;
    subprev = 0;
    net = NULL;
    sdb = NULL;
    done = FALSE;
    first = TRUE;

    /*
     * If unnumbered interface, use pointer interface for
     * source address.
     */
    if (interface->ip_unnumbered) {
	primary = interface->ip_unnumbered->ip_address;
	sourcemask = interface->ip_unnumbered->ip_nets_mask;
    } else {
	primary = interface->ip_address;
	sourcemask = interface->ip_nets_mask;
    }
    if (!source) {
	source = primary;
	mask = sourcemask;
    }

    /* 
     * Set up the real destination 
     */ 
    if (destination == -1L)
	dest = rip_destination_address(version);
    else
	dest = destination;

    /*
     * Assume we can send a default network of all zeros unless we
     * are forbidden by the access list.
     */
    diddefault = pdb_outfilter(pdb, interface, NULL, 0L) ? FALSE : TRUE;

    /*
     * Perform split horizon on the default if we are not generating it.
     */
    if (!diddefault && default_ndb)
	if (!ip_valid_splithorizon(destination, default_ndb, interface))
	    diddefault = TRUE;
    
    /*
     * If we have an ndb, we may be sending subnet information.
     * If we have a subnetted ndb that does not correspond to the
     * primary address of the outgoing interface, search the secondary
     * list for an appropriate source address.  If we can't find a good
     * source address, suppress the update, otherwise send out just the
     * subnet information for that network.  Don't forget about un-
     * numbered serial interfaces.
     */
    doingnetworks = TRUE;
    doingsubnets = FALSE;
    if (ndb) {
	if (ndb->attrflags & NET_SUBNETTED)
	    doingsubnets = TRUE;

	if (!interface->ip_split_disabled &&
	    (get_majornet(source) != ndb->netnumber)) { 
	    for (secptr = interface->ip_addressQ.qhead; secptr;
		 secptr = secptr->next) {
		if (get_majornet(secptr->address) == ndb->netnumber)
		    break;
	    }
	    if (!secptr)
		return;
	}
    }

    if (rip_debug || rip_debug_events)
	buginf("\nRIP: sending v%d update to %i via %s (%i)", version,
	       rip_real_destination(version, interface, destination),
               interface->namestring, source);

    ndb_bucket = sdb_bucket = 0;
    ndb_ptr = sdb_ptr = NULL;

    while (!done) {
	/*
	 * Setup the next datagram to be shipped.
	 */
	pak = getbuffer(IPHEADERBYTES(NOPT) + UDPHEADERBYTES +
			RIP_MAXPACKETSIZE);
	if (!pak)
	    return;

	pak->flags |= PAK_PRIORITY;
	pak->if_output = interface;
	udp = (udptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
	rip = (riptype *) ((char *) udp + UDPHEADERBYTES);

	rip->version = version;
	rip->opcode = RIPCMD_REPLY;
	rip->reserved = 0;

	rip_header_size = sizeof(riptype) +
	  rip_auth_add_prefix(rip, interface);

	ptr = (ripentrytype *) ((char *) rip + rip_header_size);
	nbytes = MAXRIPBYTES - rip_header_size;

	routecount = 0;

	/*
	 * First we do the subnets of the directly connected network
	 * on which we are sending this datagram.
	 */
	while ((nbytes > 0) && doingsubnets) {
	    pdb->route_map = NULL;
	    sdb = getnextnetwork(pdb, ndb, sdb, &subprev,
				 interface, mask, destination,
				 version == 1 ? rip1_subnet_test : NULL,
				 TRUE, TRUE, &src_pdb);
	    if (!sdb) {
		doingsubnets = FALSE;
		break;
	    }
	    ptr->family = IN_FAMILY;
	    ptr->address = sdb->netnumber;
	    ptr->metric = rip_metric(pdb, sdb, interface, destination, 
				     src_pdb);

	    if (version == 1) {
		ptr->tag = 0;
		ptr->mask = 0L;
		ptr->nexthop = 0L;

		if (rip_debug) {
		    if (sdb->netsmask == MASK_HOST)
			buginf("\n     host    %i, metric %d",
			       sdb->netnumber, ptr->metric);
		    else
			buginf("\n     subnet  %i, metric %d",
			       sdb->netnumber, ptr->metric);
		}
	    } else {
		ptr->tag = sdb->tag;
		ptr->mask = sdb->netsmask;
		ptr->nexthop = 0L;

		/* set the next hop if we can */
                /*
                 * For an unnumbered interface, simply set next hop to 0,
                 * and skip the checking of source address.
                 * Since 'source' is the ip address of the interface that
                 * the unnumbered link points to.
                 */
                if (!interface->ip_unnumbered) {
                    for (i = 0; i < sdb->routecount; i++) {
                        rdb = sdb->rdb[i];
                        if (!rdb || !rdb->gateway)
                            continue;
                        if ((rdb->gateway & mask) == (source & mask)) {
                            ptr->nexthop = rdb->gateway;
                            break;
                        }
                    }
		}

		if (rip_debug)
		    buginf("\n     %i%m -> %i, metric %d, tag %d",
			   ptr->address, ptr->mask,
			   ptr->nexthop, ptr->metric, ptr->tag);
	    }

	    nbytes -= sizeof(ripentrytype);	
	    ptr++;
	    routecount++;
	}

	/*
	 * Do default gateway handling if we have a default path
	 * (default_ndb) or are a default path (default_netsource).
	 * If we are a default path, advertise directly connected metric.
	 * Otherwise calculate metric based on protocol and mapping.
	 */
	if (!diddefault && (nbytes > 0) &&
	    (default_netsource || default_ndb || 
	     pdb_interface_for_default(pdb, interface))) {
	    diddefault = TRUE;
	    ptr->family = IN_FAMILY;
	    ptr->tag = 0;
	    ptr->address = 0L;
	    ptr->mask = 0L;
	    ptr->nexthop = 0L;
	    if (!default_ndb) {
		ptr->metric = 1;
		ptr->metric += offset_metric(pdb, interface, 0L, OUT);
		if (ptr->metric > RIP_MAXMETRIC)
		    ptr->metric = RIP_MAXMETRIC;
	    } else
		ptr->metric = rip_metric(pdb, default_ndb, interface,
					 destination, 
					 INDEX2PDB(default_ndb->pdbindex));
	    if (rip_debug) {
		if (version == 1)
		    buginf("\n     default, metric %d", ptr->metric);
		else
		    buginf("\n     %i%m -> %i, metric %d, tag %d",
			   ptr->address, ptr->mask,
			   ptr->nexthop, ptr->metric, ptr->tag);
	    }

	    nbytes -= sizeof(ripentrytype);	
	    ptr++;
	    routecount++;
	}

        /*
         * If there are too many updates queued,
         * go to sleep and wait for the queue to drain.
         * It is safe to release CPU now.
         */
        if (process_queue_size(RIPsendQ) >= RIP_SENDQ_THRESHOLD) {
            process_push_event_list(NULL);
            process_set_boolean(RIPsendQ_empty, FALSE);
            process_watch_boolean(RIPsendQ_empty, ENABLE, ONE_SHOT);
            process_wait_for_event();
            process_watch_boolean(RIPsendQ_empty, DISABLE, ONE_SHOT);
            process_pop_event_list(NULL);
        }

	/*
	 * Now do the network entries.
	 */
	if (!doingnetworks)
	    done = TRUE;
	else while ((nbytes > 0) &&
		    (net = geteachroute(&ndb_ptr, &ndb_bucket,
					&sdb_ptr, &sdb_bucket, TRUE))) {
	    if (!net) {
		done = TRUE;
		break;
	    }

	    pdb->route_map = NULL;
	    checked = FALSE;

	    if (net->attrflags & NET_SUBNET) {
		sum_db = net_lookup(get_majornet(net->netnumber), FALSE);
		if (!sum_db)
		    continue;

		/*
		 * If we're doing our interfaces major network, punt,
		 * as we did it up in the local subnets section above.
                 * But, we want to do this for unnumbered links.
		 */
		if (sum_db == ndb && !interface->ip_unnumbered) {
		    sdb_ptr = NULL;
		    sdb_bucket = 0;
		    continue;
		}
		/*
		 * If auto-summarization is enabled, abort if we don't
		 * have a valid subnet under this network entry.
		 */
		if (pdb->auto_summary || version == 1) {
		    net = sum_db;
		    sumprev = 0;
		    if (!find_valid_subnet(pdb, net, NULL, &sumprev,
					   interface, mask, destination,
					   version == 1
					   ? rip1_network_test
					   : rip2_network_test,
					   TRUE, FALSE, &src_pdb)) {
			sdb_ptr = NULL;
			sdb_bucket = 0;
			continue;
		    }
		    checked = TRUE;

		    /*
		     * Skip the rest of the SDB's in the chain,
		     * since we've got our sumamry now
		     */
		    sdb_ptr = NULL;
		    sdb_bucket = 0;
		}
	    }
	    if (!checked && !validnextnetwork(pdb, ndb, net, interface,
					      mask, destination,
					      version == 1
					      ? rip1_network_test
					      : rip2_network_test,
					      TRUE, &src_pdb))
		continue;

	    net->advertisingmask = pdb->mask;

	    ptr->family = IN_FAMILY;
	    ptr->address = net->netnumber;
	    ptr->metric = rip_metric(pdb, net, interface, destination,
				     src_pdb);

	    if (version == 1) {
		ptr->tag = 0;
		ptr->mask = 0L;
		ptr->nexthop = 0L;

		if (rip_debug)
		    buginf("\n     network %i, metric %d",
			   net->netnumber, ptr->metric);

	    } else {

		ptr->tag = net->tag;
		ptr->mask = ndb_mask(net);
		ptr->nexthop = 0L;

		/* set the next hop if we can */
                /*
                 * For an unnumbered interface, simply set next hop to 0,
                 * and skip the checking of source address.
                 * Since 'source' is the ip address of the interface that
                 * the unnumbered link points to.
                 */
                if (!interface->ip_unnumbered) {
                    for (i = 0; i < net->routecount; i++) {
                        rdb = net->rdb[i];
                        if (!rdb || !rdb->gateway)
                            continue;
                        if ((rdb->gateway & mask) == (source & mask)) {
                            ptr->nexthop = rdb->gateway;
                            break;
                        }
                    }
		}

		if (rip_debug)
		    buginf("\n     %i%m -> %i, metric %d, tag %d",
				ptr->address, ptr->mask,
				ptr->nexthop, ptr->metric, ptr->tag);
	    }

	    nbytes -= sizeof(ripentrytype);	
	    ptr++;
	    routecount++;
	}

	/*
	 * Finish up the datagram and ship it.
	 */
	if (routecount == 0) {
	    if (!reg_invoke_snapshot_enabled(interface)) {
		if ((rip_debug || rip_debug_events) && first)
		    buginf(" - suppressing null update");
		retbuffer(pak);
		return;
	    } else {
		done = TRUE;
	    }
	} else {
 	    if (rip_debug_events)
		buginf("\nRIP: Update contains %d routes", routecount);
  	}

	rip_suffix_size = rip_auth_add_suffix(rip, interface, routecount);

	udp->sport = RIP_PORT;
	udp->dport = port ? port : RIP_PORT;
        nbytes = UDPHEADERBYTES + rip_header_size + rip_suffix_size +
	    (routecount * sizeof(ripentrytype));
	udp->length = nbytes;
	udp->checksum = 0;
        /*
         * We don't compute a UDP pseudoheader checksum because
         * our destination address may be changed by ipwrite()
         * if it is broadcast, i.e. -1L.
         */
        /*
         * Queue the updates, let "RIP send" process to send them,
         * where inter-frame gap will be inserted between each packet
         * if output delay is configured.
         */
        args = malloc(sizeof(ripsendtype));
        if (args) {
            if (rip_debug_events)
                buginf("\nRIP: Update queued");
 
            args->pak = pak;
            args->bytes = nbytes + IPHEADERBYTES(NOPT);
            args->source = source;
            args->destination = dest;
            args->ttl = ttl;
            process_enqueue(RIPsendQ, args);
        }

	first = FALSE;
    }
}

/*
 * rip_sendupdate_subr
 * Send a RIP update.  We update the metric (hop count) at this point
 * instead of at time of reception.
 */

static void
rip_sendupdate_subr (pdbtype *pdb, ndbtype *ndb, ipaddrtype destination,
		     idbtype *interface, ulong port, ipaddrtype source,
		     ipaddrtype mask, uchar ttl)
{
    int version;

    if (!interface || !interface->ip_info)
	return;

    /*
     * If the interface is not to receive updates because of a snapshot
     * configuration, don't bother to send any further updates.
     */
    if (reg_invoke_snapshot_update_denied(interface, FALSE))
	return;

    for (version = RIP_MIN_VERSION; version <= RIP_MAX_VERSION; version++) {
	if (!rip_send_version(pdb, interface, version))
	    continue;
	rip_sendupdate_vk(version, pdb, ndb, destination, interface, 
			  port, source, mask, ttl);
    }
}


/*
 * rip_sendupdate
 * Send a RIP update.  We update the metric (hop count) at this point
 * instead of at time of reception.
 */

static void
rip_sendupdate (pdbtype *pdb, ndbtype *ndb, ipaddrtype destination,
		idbtype *interface, ulong port, ipaddrtype source,
		ipaddrtype mask)
{
    rip_sendupdate_subr(pdb, ndb, destination, interface, port, source,
			mask, iprouteparams.ttl);
}

/*
 * rip_rcvupdate
 * Receive a RIP update.  Note that we update the metric (hop count)
 * when sending the packet, not when receiving it.
 */

static void
rip_rcvupdate (pdbtype *pdb, paktype *pak, riptype *rip)
{
    iphdrtype *ip;
    int size, routecount, tag;
    ripentrytype *ptr;
    idbtype *interface;
    ipaddrtype source, address, nexthop, mask;
    long metric;

    ip = (iphdrtype *)ipheadstart(pak);

    interface = pak->if_input;
    source = ip->srcadr;

    if (!interface || !interface->ip_info)
	return;

    /*
     * If split horizon is disabled, or update source validation is enabled,
     * check to make sure that the source address is on one of our nets.
     */
    if (interface->ip_split_disabled || pdb->validate_source) {
	if (!ip_onifsubnet(interface, source)) {
	    if (rip_debug || rip_debug_events)
		buginf("\nRIP: ignored v%d update from bad source %i on %s",
			rip->version, source, interface->namestring);
	    return;
	}
    }

    if (rip_debug || rip_debug_events)
	buginf("\nRIP: received v%d update from %i on %s",
		rip->version, source, pak->if_input->namestring);

    reg_invoke_snapshot_update_received(pak->if_input, pak);

    routecount = 0;

    ptr = (ripentrytype *) ((char *)rip + sizeof(riptype));

    if (rip_auth_present(rip))
	ptr++;

    size = ip->tl - ((char *)ptr - (char *)ip);

    while (size >= sizeof(ripentrytype)) {
	if (ptr->family == IN_FAMILY) {
	    address = ptr->address;
	    metric = ptr->metric;
	    metric += offset_metric(pdb, interface, address, IN);

	    if (rip->version == 1) {

		tag = 0L;
		mask = 0L;
		nexthop = source;

		if (rip_debug) {
		    if (metric < RIP_MAXMETRIC)
			buginf("\n     %i in %d hops", address, metric);
		    else
			buginf("\n     %i in %d hops (inaccessible)",
			       address, metric);
		}

	    } else {

		mask = ptr->mask;

		if (CLASSD_AND_GREATER(address)) {
		    if (rip_debug)
			buginf("\n     %i%m (illegal prefix)", address, mask);
		    goto punt;
		}

		if (mask == MASK_HOST && martian(address, TRUE)) {
		    if (rip_debug)
			buginf("\n     %i%m (illegal address)", address, mask);
		    goto punt;
		}

		if (ptr->nexthop) {
		    /* ignore routes with ourselves as next hop */
		    if (ip_ouraddress(ptr->nexthop))
			goto punt;

		    /* only accept local next hops */
		    if (!(samecable(ptr->nexthop) == interface))
			goto punt;

		    nexthop = ptr->nexthop;
		} else
		    nexthop = source;

		tag = ptr->tag;

		if (rip_debug) {
		    if (metric < RIP_MAXMETRIC)
			buginf("\n     %i%m -> %i in %d hops",
				address, mask, ptr->nexthop, metric);
		    else
			buginf("\n     %i%m -> in %d hops (inaccessible)",
				address, mask, ptr->nexthop, metric);
		}
	    }

	    if (metric == 0)		/* compensate for a common bug */
		metric = 1;

	    if (metric >= RIP_MAXMETRIC)
		metric = METRIC_INACCESSIBLE;

	    (void) network_update(pdb, address, mask, nexthop, interface,
				  metric, 0, source, tag, NULL, 0);
	    process_may_suspend();
	    routecount++;
	}
      punt:
	size -= sizeof(ripentrytype);
	ptr++;
    }
    if (rip_debug_events)
	buginf("\nRIP: Update contains %d routes", routecount);
}

/*
 * rip_sendrequest
 * Broadcast a RIP request for information
 */

static void
rip_sendrequest_vk (int version, pdbtype *pdb, idbtype *idb, 
		    ipaddrtype address)
{
    paktype *pak;
    ipaddrtype dest;
    udptype *udp;
    riptype *rip;
    ripentrytype *ptr;
    int rip_header_size, bytes, size;

    bytes = IPHEADERBYTES(NOPT) + UDPHEADERBYTES +
	    sizeof(ripauthtype) + sizeof(ripentrytype) +
	    sizeof(ripmdsuffixtype);

    pak = getbuffer(bytes);
    if (!pak)
	return;

    udp = (udptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    rip = (riptype *) ((char *)udp + UDPHEADERBYTES);

    rip->opcode = RIPCMD_REQUEST;
    rip->reserved = 0;
    rip->version = version;

    dest = rip_destination_address(version);

    rip_header_size = sizeof(riptype) + rip_auth_add_prefix(rip, idb);

    ptr = (ripentrytype *) ((char *) rip + rip_header_size);

    memset(ptr, 0, sizeof(ripentrytype));
    ptr->family = UNSPECIFIED_FAMILY;
    ptr->metric = RIP_MAXMETRIC;

    udp->sport = RIP_PORT;
    udp->dport = RIP_PORT;
    udp->checksum = 0;
    udp->length = UDPHEADERBYTES + rip_header_size + sizeof(ripentrytype) +
		  rip_auth_add_suffix(rip, idb, 1);

    /*
     * We don't compute a UDP pseudoheader checksum because we don't
     * know what our source address is.  This is silly and should be
     * fixed later.
     * The destination address may be changed by ipwrite()
     * if it is broadcast, i.e. -1L.
     */

    size = udp->length + IPHEADERBYTES(NOPT);

    if (rip_debug || rip_debug_events)
	buginf("\nRIP: sending general request on %s to %i",
	    idb->namestring, rip_real_destination(version, idb, -1L));

    ipwrite(pak,size,UDP_PROT,address,dest,idb,NULL, &iprouteparams);
}
/*
 * rip_sendrequest
 * Broadcast a RIP request for information
 */

static void
rip_sendrequest (pdbtype *pdb, idbtype *sidb)
{
    ipaddrtype address;
    idbtype *idb;
    ndbtype *ndb;
    boolean send;
    ipaddtype *secptr;
    int ver;

    FOR_ALL_SWIDBS(idb) {
	if (idb->ip_enabled && !(idb->ip_passive_router & pdb->mask) &&
	    interface_up(idb) && (!idb->static_routing)) {

	    send = FALSE;
	    address = idb->ip_address;

	    if (!address && idb->ip_unnumbered)
		address = idb->ip_unnumbered->ip_address;

	    if (ip_pdb_net_advertise(pdb, address, &ndb)) {
		send = TRUE;
	    } else {
		for (secptr = idb->ip_addressQ.qhead; secptr;
		     secptr = secptr->next) {
		    if (ip_pdb_net_advertise(pdb, secptr->address,
					     &ndb)) {
			address = secptr->address;
			send = TRUE;
			break;
		    }		    
		}
	    }

	    if (!send)
		continue;

	    for (ver = RIP_MIN_VERSION; ver <= RIP_MAX_VERSION; ver++) {
		if (!rip_send_version(pdb, idb, ver))
		    continue;
		    rip_sendrequest_vk(ver, pdb, idb, address);
	    }
	}
    }
}

/*
 * rip_rcvrequest
 * Handle a RIP request for information.  A request for the entire routing
 * table is a single ripentrytype with an unspecified family and metric
 * of infinity.  At present this is the only form of request to which we will
 * respond.
 */

static void
rip_rcvrequest (pdbtype *pdb, paktype *pak, riptype *rip, ushort port)
{
    iphdrtype *ip;
    ndbtype *ndb;
    int size;
    idbtype *idb;
    ipaddrtype address, srcmajor;
    ipaddtype *secptr;
    boolean send;
    uchar ttl;
    int first;

    ip = (iphdrtype *) ipheadstart(pak);
    idb = pak->if_input;

    if (rip_debug || rip_debug_events)
	buginf("\nRIP: received v%d request from %i on %s",
		rip->version, ip->srcadr, idb->namestring);

    /*
     * If a RIP-2 router receives a RIP-1 request, it should respond with
     * a RIP-1 response.  If the router is configured to send only RIP-2
     * messages, it should not respond to a RIP-1 request.
     */
    if (!rip_send_version(pdb, idb, rip->version))
	return;

    send = FALSE;
    ndb = NULL;
    if (idb->ip_unnumbered) 
	address = idb->ip_unnumbered->ip_address;
    else 
	address = idb->ip_address;

    srcmajor = get_majornet(ip->srcadr);

    if ((get_majornet(address) == srcmajor) &&
	ip_pdb_net_advertise(pdb, address, &ndb)) {
	send = TRUE;
    } else {
	for (secptr = idb->ip_addressQ.qhead; secptr;
	     secptr = secptr->next) {
	    if ((get_majornet(secptr->address) == srcmajor) &&
		ip_pdb_net_advertise(pdb, secptr->address, &ndb)) {
		send = TRUE;
		break;
	    }
	}
    }

    if (!send)
	return;

    size = (ip->tl - (ip->ihl << 2) - UDPHEADERBYTES - sizeof(riptype));
    first = 0;
    if ((rip->entry[first].family == UNSPECIFIED_FAMILY) &&
	(rip->entry[first].metric == RIP_MAXMETRIC) &&
	(size == sizeof(ripentrytype))) {
	/* 
	 * if we received a unicast query, we'll then up the ttl
	 * on the response, because it may have come from a management
	 * station not on a local (or even near) network.  CSCdi35234.
	 */
        ttl = iprouteparams.ttl;
	if (!(ip_broadcast(ip->dstadr, idb))) {
	    ttl = TTL_DEFAULT;
	    if (rip_debug || rip_debug_events)
		buginf("\nRIP: sending update with long TTL");
	}
	rip_sendupdate_subr(pdb, ndb, ip->srcadr, idb, port, 0, 0, ttl);
    }
}

/*
 * rip_cleanup
 * Clean up after RIP router has been turned off.
 * This entails closing the RIP protocol socket.
 */

static void
rip_cleanup (pdbtype *pdb)
{
    idbtype *idb;
    ipinfotype *info;
    ripsendtype *args;

    /*
     * kill the sending process (if it exists)
     */
    if (RIPsendQ) {
	process_watch_queue(RIPsendQ, DISABLE, RECURRING);
	while ((args = process_dequeue(RIPsendQ)) != NULL) {
	    datagram_done(args->pak);
	    free(args);
	}
	delete_watched_queue(&RIPsendQ);
	delete_watched_boolean(&RIPsendQ_empty);
    }
    if (rip_send_pid) {
        process_kill(rip_send_pid);
        rip_send_pid = 0;
    }
    reg_invoke_ip_close_socket(pdb->socket);
    pdb->socket = NULL;
    unqueue(&protoQ, pdb);
    free_pdbindex(pdb);
    free(pdb);

    FOR_ALL_SWIDBS(idb) {
	if (idb->ip_info) {
	    info = idb->ip_info;

	    free(info->rip_auth_keychain);
	    info->rip_auth_keychain = NULL;
	    info->rip_send_vers = 0;
	    info->rip_recv_vers = 0;
	}
    }
}

/*
 * rip_findsocket
 *
 * Given a particular protocol instance, find if we already have an open
 * socket.
 */

static inline ipsocktype *
rip_findsocket (ushort asystem)
{
    ipsocktype *soc;

    for (soc = IPsocketQ.qhead; soc; soc = soc->next)
	if ((soc->protocol == UDP_PROT) &&
	    (soc->fport == RIP_PORT) && (soc->lport == RIP_PORT))
		return (soc);

    return (NULL);
}


/*
 * rip_send_process
 *
 * This process is dedicated to sending updates which have been queued.
 * It controls the inter-frame gap between the individual packets sent
 * in a multiple-packet routing udpate.
 * It assumes that updates destined to the same outgoing interface are
 * queued in sequence.
 *
 * The actual inter-frame gap may differ from pdb->output_delay.
 * It is a multiple of refresh_time (4ms).
 *
 */
static process
rip_send_process (void)
{
    pdbtype *pdb;
    ripsendtype *args;
    ipparamtype params;
    paktype *pak;
    idbtype *interface, *last_out_idb;
    sys_timestamp next_update_time;

    if (process_get_arg_ptr((void **)&pdb) == FALSE) {
        rip_send_pid = 0;
        process_kill(THIS_PROCESS);
    }

    params = iprouteparams;
    last_out_idb = NULL;
    TIMER_STOP(next_update_time);
    process_watch_queue(RIPsendQ, ENABLE, RECURRING);

    while (TRUE) {
        process_wait_for_event();
        /*
         * send updates
         */
        while ((args = process_dequeue(RIPsendQ)) != NULL) {
            pak = args->pak;
            interface = pak->if_output;
            pak->flags |= PAK_PRIORITY;
            /*
             * delay if inter-frame gap is set
             * don't delay if start sending to different interface
             * and don't delay if start a new update cycle
             */
            if (last_out_idb != interface) {
                last_out_idb = interface;
            } else {
                if (TIMER_RUNNING_AND_SLEEPING(next_update_time))
                    process_sleep_until(&next_update_time);
            }
            TIMER_START(next_update_time, pdb->output_delay);
            if (rip_debug_events)
                buginf("\nRIP: Update sent via %s", interface->namestring);

            reg_invoke_snapshot_notice_update_pak(pak, interface);
            params.ttl = args->ttl;
            ipwrite(pak, args->bytes, UDP_PROT,
                    args->source, args->destination,
                    interface, NULL, &params);
            free(args);
        }
        process_set_boolean(RIPsendQ_empty, TRUE);
    }
}

/*
 * rip_process_packets
 *
 * Process all incoming RIP packets from the queue.
 */
static void rip_process_packets (pdbtype *pdb, ipsocktype *soc)
{
    paktype *pak;
    iphdrtype *ip;
    riptype *rip;
    ndbtype *ndb;
    udptype *udp;
    ipaddrtype address;
    int dummy;
    ipaddtype *secptr;
    idbtype *idb;

    /*
     * First empty the RIP packet queue
     */
    while ((pak = read_ipsocket(soc,FALSE,0,&udp,&dummy))) {
	ip  = (iphdrtype *)ipheadstart(pak);
	rip = (riptype *) ((char *) udp + UDPHEADERBYTES);

	idb = pak->if_input;
	if (!idb || !idb->ip_info) {
	    retbuffer(pak);
	    continue;
	}

	/*
	 * Are we allowed to run routing on this interface?
	 */

	if (idb->static_routing) {
	    if (rip_debug)
		buginf("\nRIP: ignored v%d packet from %i "
		       "(routing disabled on %s)",
		       rip->version, ip->srcadr, idb->namestring);
	    retbuffer(pak);
	    continue;
	}

	/*
	 * Check version number.
	 */
	if (!rip_recv_version(pdb, idb, rip->version)) {
	    if (rip_debug || rip_debug_events)
		buginf("\nRIP: ignored v%d packet from %i "
		       "(illegal version)",
		       rip->version, ip->srcadr);
	    retbuffer(pak);
	    continue;
	}

	/*
	 * Don't accept v1 packets addressed to the multicast address.
	 * This is rather stupid given the lack of explicit language to
	 * preclude this behavior, but the ANVL tester and Gary Malkin
	 * both argue against this behavior.
	 */
	if (rip->version < 2 && ip->dstadr == ALLRIPROUTERS) {
	    if (rip_debug || rip_debug_events)
		buginf("\nRIP: ignored v%d packet from %i "
		       "(multicast behavior undefined)",
		       rip->version, ip->srcadr);
	    retbuffer(pak);
	    continue;
	}

	/*
	 * Ensure that RIP is running on this interface.
	 */
	address = pak->if_input->ip_address;
	if (!address)
	    address = pak->if_input->ip_unnumbered->ip_address;
	if (!ip_pdb_net_advertise(pdb, address, &ndb)) {
	    secptr = pak->if_input->ip_addressQ.qhead; 
	    for ( ; secptr; secptr = secptr->next) {
		if (ip_pdb_net_advertise(pdb, secptr->address, &ndb))
		    break;
	    }
	    if (!secptr) {
		if (rip_debug)
		    buginf("\nRIP: ignored v%d packet from %i "
			   "(not enabled on %s)",
			   rip->version, ip->srcadr,
			   pak->if_input->namestring);
		retbuffer(pak);
		continue;
	    }
	}

	/* Don't accept packets from one of our interfaces */
	if (ip_ouraddress(ip->srcadr)) {
	    if (rip_debug || rip_debug_events)
		buginf("\nRIP: ignored v%d packet from %i "
		       "(sourced from one of our addresses)",
		       rip->version, ip->srcadr);
	    retbuffer(pak);
	    continue;
	}

	if (!rip_auth_check(rip, idb)) {
	    if (rip_debug || rip_debug_events)
		buginf("\nRIP: ignored v%d packet from %i "
		       "(invalid authentication)",
		       rip->version, ip->srcadr);
	    retbuffer(pak);
	    continue;
	}

	if (!rip_validate_msg(ip, rip)) {
	    if (rip_debug || rip_debug_events)
		buginf("\nRIP: ignored v%d packet from %i "
		       "(non-zero reserved fields)",
		       rip->version, ip->srcadr);
	    retbuffer(pak);
	    continue;
	}

	/*
	 * Switch on packet type.
	 */
	switch (rip->opcode) {
        case RIPCMD_REQUEST:
	    if (!(idb->ip_passive_router & pdb->mask))
		rip_rcvrequest(pdb, pak, rip, udp->sport);
	    break;
	case RIPCMD_REPLY:
	    rip_rcvupdate(pdb, pak, rip);
	    break;
	case RIPCMD_TRACEON:
	case RIPCMD_TRACEOFF:
	default:
	    break;
	}

	/*
	 * Dispose of processed datagram.
	 */
	retbuffer(pak);
    }
}

/*
 * rip_router
 * RIP router process.  Only one process for all RIP routing networks.
 * If the RIP socket is in use, we assume another router process is already
 * running and so we aren't needed.
 */

static process
rip_router (void)
{
    ipsocktype *soc;
    boolean sendupdate;
    pdbtype *pdb;
    ulong major, minor;

    if (!process_get_arg_ptr((void**)&pdb))
	process_kill(THIS_PROCESS);

    memory_new_owner(pdb);

    edisms((blockproc *)systeminitBLOCK, 0);

    /*
     * Think ahead to multiple RIP-2 instances
     */
    soc = rip_findsocket(pdb->asystem);
    if (soc)
	pdb_killp(pdb, -1);

    soc = open_ipsocket(UDP_PROT, NULL, NULL, RIP_PORT, RIP_PORT, SOCK_ANYSRC);
    if (!soc) {
	errmsg(&msgsym(NOSOCKET, RIP));
	pdb_killp(pdb, -1);
    }

    pdb->socket = soc;
    TIMER_START(pdb->timer, 10*ONESEC);
    /*
     * Create a process dedicated to sending updates down to IP.
     * Output delay will be handled by this process.
     * Set the priority of this process to high, so that it gets
     * scheduled when rip router process calls process_may_suspend().
     */
    RIPsendQ = create_watched_queue("RIP send queue", 0, 0);
    RIPsendQ_empty = create_watched_boolean("RIP send queue empty", 0);
    rip_send_pid = process_create(rip_send_process, "RIP Send",
                                  NORMAL_STACK, PRIO_HIGH);
    if (rip_send_pid != NO_PROCESS) {
        process_set_arg_ptr(rip_send_pid, pdb);
    }

    sendupdate = FALSE;
    rip_sendrequest(pdb, NULL);

    process_watch_timer(&pdb->timer, TRUE);
    process_watch_queue(soc->inq, ENABLE, RECURRING);
    while (TRUE) {

	/*
         * If something changed in the last pass, send a major update now
  	 */
	if (sendupdate || pdb->changed) {
	    pdb_majorupdate(pdb);
	    sendupdate = FALSE;
	}

	/*
	 * Block until RIP input or timer expires.
	 */
	pdb->changed = 0;
	process_wait_for_event();

	/*
	 * Do whatever needs to be done.
	 */
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		rip_process_packets(pdb, soc);
		break;

	      case TIMER_EVENT:
		/*
		 * If the update timer has expired, age entries appropriately.
		 * Set "sendupdate" to force a periodic update even if nothing
		 * has changed.
		 */
		age_all_routes(pdb);
		sendupdate = TRUE;
		TIMER_START_JITTERED(pdb->timer, pdb->broadcasttime,
				     RIP_JITTER);
		process_may_suspend();
		break;

	      case DIRECT_EVENT:
		/*
		 * Currently, the only direct event is the result a topology
		 * change signal.  Ignore it for the nonce, and we'll catch it
		 * after all other wakeup events have been processed.
		 */
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

static void
rip_showipproto (pdbtype *pdb)
{
    ndbtype *ndb;
    ipaddrtype address;
    ipaddtype *secptr;
    idbtype *idb;
    int headerflag = FALSE;
    char *send, *recv;
    ipinfotype  *info;
    int len, ver;

    default_showipproto(pdb);

    switch (pdb->version) {
    case 0:
	send = "version 1";
	recv = "any version";
	break;
    case 1:
	send = recv = "version 1";
	break;
    case 2:
	send = recv = "version 2";
	break;
    default:
	send = recv = "error";
    }

    printf("\n  Default version control: send %s, receive %s", send, recv);

    FOR_ALL_SWIDBS(idb) {
    	info = idb->ip_info;
	if (!info)
	    continue;
	if (idb->static_routing)
	    continue;
	if (idb->ip_passive_router & pdb->mask)
	    continue;

	address = idb->ip_address;
	if (idb->ip_unnumbered)
	    address = idb->ip_unnumbered->ip_address;

	if (!ip_pdb_net_advertise(pdb, address, &ndb)) {
	    secptr = idb->ip_addressQ.qhead; 
	    for ( ; secptr; secptr = secptr->next) {
		if (ip_pdb_net_advertise(pdb, secptr->address, &ndb))
		    break;
	    }
	    if (!secptr)
		continue;
	}
	if (!headerflag) {
	    headerflag = TRUE;
	    automore_header("    Interface        Send  Recv   Key-chain");
	}

	printf("\n    %16s ", idb->namestring);

	for (len = 0, ver = RIP_MIN_VERSION; ver <= RIP_MAX_VERSION; ver++)
	    if (rip_send_version(pdb, idb, ver))
		len += printf("%d ", ver);

	printf("%*c", 6 - len, ' ');

	for (len = 0, ver = RIP_MIN_VERSION; ver <= RIP_MAX_VERSION; ver++)
	    if (rip_recv_version(pdb, idb, ver))
		len += printf("%d ", ver);

	printf("%*c %16s", 6 - len, ' ', info->rip_auth_keychain);
    }
}

/*
 * rip_onoff_idb
 *
 * If we've determined that RIP is/is not running on this IDB, this
 * routine gets called.
 */

static void
rip_onoff_idb (idbtype *idb, boolean sense)
{
    ipinfotype *info = idb->ip_info;

    if (sense) {
        /*
         * Don't add this IP multicast address more than once.
         * Otherwise, it won't be deleted.
         */
        if (!ip_multicast(ALLRIPROUTERS, idb))
            ip_multicast_ctl(idb, ALLRIPROUTERS, sense);
    } else {
        ip_multicast_ctl(idb, ALLRIPROUTERS, sense);
    }

    /*
     * This is a kludge to initialize the IDB for the first time for RIP
     * fields.  This needs to be moved into a registry.
     */
    if (info) {
	if (!info->rip_auth_mode)
	    info->rip_auth_mode = RIP_AUTH_TYPE_TEXT;
    }
}

/*
 * rip_network
 *
 * Process "network" command for a RIP routing process.  This function
 * will obtain the idbs associated with the network number configured.
 */
static void
rip_network (pdbtype *pdb, ipaddrtype net, ipaddrtype mask,
	     uchar flags, ulong area, int area_id_format, boolean sense)
{
    idbtype    *idb, *numbered_idb;
    ipaddtype  *ptr;
    ipaddrtype address;
    boolean    delete;

    net &= mask;

    FOR_ALL_SWIDBS(idb) {
	if (!idb->ip_enabled)
	    continue;

	numbered_idb = idb->ip_unnumbered ? idb->ip_unnumbered : idb;
	address = numbered_idb->ip_address;

	delete = !sense && !get_nets_per_idb(pdb, idb) &&
                 !(idb->ip_passive_router & pdb->mask);

	/*
	 * Check primary address.
	 */
	if ((address & mask) == net) {
            if (sense) {
                rip_onoff_idb(idb, TRUE);
            } else if (delete) {
                rip_onoff_idb(idb, FALSE);
	    }
	}

	/*
	 * Process same thing for secondary addresses.
	 */
	for (ptr = numbered_idb->ip_addressQ.qhead;
	     ptr; ptr = ptr->next) {
	    if ((ptr->address & mask) != net)
		continue;
            if (sense) {
                rip_onoff_idb(idb, TRUE);
            } else if (delete) {
                rip_onoff_idb(idb, FALSE);
	    }
	}
    }
}

/*
 * rip_passive_interface_command
 *
 * Process passive-interface router command for RIP.
 */
static void
rip_passive_interface_command (pdbtype *pdb, idbtype *idb, boolean sense)
{
    if (sense) {
        if (!ip_multicast(ALLRIPROUTERS, idb))
            ip_multicast_ctl(idb, ALLRIPROUTERS, sense);
    } else {
        if (!get_nets_per_idb(pdb, idb))
            ip_multicast_ctl(idb, ALLRIPROUTERS, sense);
    }
}

/*
 * rip_ip_address
 *
 * Called when an IP address is added or deleted from an interface.
 */
static void
rip_ip_address (idbtype *idb, ipaddrtype address, ipaddrtype mask, 
		boolean secondary, boolean add)
{
    pdbtype       *pdb;
    int           count = 0;

    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype != PROC_RIP)
	    continue;
	if (!pdb->running)
	    continue;

	count += get_nets_per_idb(pdb, idb);
    }

    rip_onoff_idb(idb, count);
}

/*
 * rip_pdb_init
 * Initialize the Protocol Descriptor Block for RIP
 * Note that there is only one pdb for RIP.
 */

static pdbtype *
rip_pdb_init (ushort asystem)
{
    pdbtype *pdb;

    pdb = malloc(sizeof(pdbtype));
    if (!pdb)
        return NULL;

    pdb->proctype = PROC_RIP;

    if (allocate_pdbindex(pdb) == -1) {
	free(pdb);
	return NULL;
    }

    pdb->version = 0;		/* send RIPv1, receive any */
    pdb->distflag = pdb->mask;
    pdb->redistallowed = PROC_REDISTMASK;
    pdb->name = "rip";
    pdb->family = PDB_IP;
    pdb->pname = "RIP Router";
    pdb->asystem = asystem;
    pdb->process = (processcbtype)rip_router;
    pdb->cleanup = rip_cleanup;
    pdb->majorupdate = rip_sendupdate;
    pdb->holddown = rip_holddown;
    pdb->query = rip_sendrequest;
    pdb->showipproto = rip_showipproto;
    pdb->network_cb = rip_network;
    pdb->auto_summary = TRUE;
    pdb->holdokay = TRUE;
    pdb->validate_source = TRUE;
    pdb->broadcasttime = pdb->def_broadcasttime = RIP_UPDATETIME;
    pdb->invalidtime = pdb->def_invalidtime = RIP_INVALIDTIME;
    pdb->holdtime = pdb->def_holdtime =  RIP_INVALIDTIME;
    pdb->flushtime = pdb->def_flushtime = RIP_GCTIME;
    pdb->distance = pdb->def_distance = RIP_DISTANCE;
    pdb->distance_cmp = iprouting_distcmp_generic;
    pdb->multipath = pdb->def_multipath = DEFMAXROUTES;
    TIMER_STOP(pdb->sleep_timer);
    pdb->supports_snapshot = TRUE;
    pdb->output_delay = 0;
    enqueue(&protoQ, pdb);

    return pdb;
}

static void
rip_init (subsystype *subsys)
{
    ip_rip_parser_init();
    reg_add_iprouting_pdb_init(PROC_RIP, rip_pdb_init, "rip_pdb_init");
    reg_add_ip_passive_cmd_route_adjust(PROC_RIP,
                                        rip_passive_interface_command,
                                        "rip_passive_interface_command");
    reg_add_ip_address_change(rip_ip_address, "rip_ip_address");
}


/*
 * ip_rip_int_command
 *
 * Handle the "interface xxx / ip rip xxxx" commands.
 */

void
ip_rip_int_command (parseinfo *csb)
{
    idbtype *idb;
    ipinfotype *info;
    int ver;

    idb = csb->interface;
    if (!idb || !idb->ip_info || (idb->ip_enabled == FALSE))
	return;

    info = idb->ip_info;
  
    if (csb->nvgen) {
	switch (csb->which) {
	case RIP_SEND_VERS_CMD:
	    if (info->rip_send_vers) {
		nv_write(TRUE, "%s", csb->nv_command);
		for (ver = RIP_MIN_VERSION; ver <= RIP_MAX_VERSION; ver++)
		    nv_add((1<<ver) & info->rip_send_vers, " %d", ver);
	    }
	    break;
	case RIP_RECV_VERS_CMD:
	    if (info->rip_recv_vers) {
		nv_write(TRUE, "%s", csb->nv_command);
		for (ver = RIP_MIN_VERSION; ver <= RIP_MAX_VERSION; ver++)
		    nv_add((1<<ver) & info->rip_recv_vers, " %d", ver);
	    }
	    break;
	case RIP_AUTH_MODE_CMD:
	    if (info->rip_auth_mode != RIP_AUTH_TYPE_TEXT) {
		nv_write(TRUE, "%s ", csb->nv_command);
		nv_add_id_string(rip_auth_modes, info->rip_auth_mode);
	    }
	    break;
	case RIP_AUTH_KEY_CMD:
	    nv_write(info->rip_auth_keychain != NULL, "%s %s", csb->nv_command,
		     info->rip_auth_keychain);
	    break;
	}
	return;
    }

    switch (csb->which) {
    case RIP_SEND_VERS_CMD:
	info->rip_send_vers = csb->sense ? GETOBJ(int,1) : 0;
	break;
    case RIP_RECV_VERS_CMD:
	info->rip_recv_vers = csb->sense ? GETOBJ(int,1) : 0;
	break;
    case RIP_AUTH_MODE_CMD:
	info->rip_auth_mode = csb->sense ? GETOBJ(int,1) : RIP_AUTH_TYPE_TEXT;
	break;
    case RIP_AUTH_KEY_CMD:
	setstring(&info->rip_auth_keychain,
		  csb->sense ? GETOBJ(string,1) : NULL);
	break;
    }	
}

/*
 * RIP subsystem header
 */

#define RIP_MAJVERSION 1
#define RIP_MINVERSION 0
#define RIP_EDITVERSION  1

SUBSYS_HEADER(rip, RIP_MAJVERSION, RIP_MINVERSION,
	      RIP_EDITVERSION, rip_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iprouting", "req: iprouting keyman");
