/* $Id: x25_route.c,v 3.5.10.5 1996/09/10 03:08:34 snyder Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_route.c,v $
 *------------------------------------------------------------------
 * x25_route.c -- X25 Routing Table Support
 *
 * July 1989, Greg Satz
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_route.c,v $
 * Revision 3.5.10.5  1996/09/10  03:08:34  snyder
 * CSCdi68568:  more constant opportuniites
 *              160 out of data
 *               72 out of image
 * Branch: California_branch
 *
 * Revision 3.5.10.4  1996/08/28  13:23:39  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.10.3  1996/08/07  09:04:50  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.5.10.2  1996/06/28  23:32:53  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.5.10.1  1996/03/18  22:50:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.22.4  1996/03/16  07:57:47  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.22.3  1996/03/13  02:13:46  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.22.2  1996/03/07  11:19:54  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.22.1  1996/02/20  21:56:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/24  01:08:57  carvalho
 * CSCdi46637:  X.25 permits Calls to be routed to receiving interface
 * Check route's interface against call's interface.
 *
 * Revision 3.4  1995/11/21  08:27:35  ahh
 * CSCdi42413:  XOT: cannot tune TCP keepalive behavior
 * Allow keepalive rate to be controlled for XOT connections.
 *
 * Revision 3.3  1995/11/17  18:08:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:52:13  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:54:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/12  19:19:07  gglick
 * CSCdi35754:  Incoming x25 call is accepted as PAD call i.s.o being
 * routed
 *
 * Revision 2.2  1995/06/20  20:39:04  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:22:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "address.h"
#include "packet.h"
#include "logger.h"
#include "../x25/msg_x25.c"		/* Not a typo, see logger.h */
#include "../if/network.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "x25.h"
#include "x25_address.h"
#include "x25_switch.h"
#include "x25_route.h"
#include "x25_proto.h"
#include "../ip/ip_registry.h"
#include "../tcp/tcpdriver.h"
#include "x25_pbp.h"


boolean x25_routing;
x25routeentrytype *x25_routing_table;
boolean x25_tcp_use_if_defs;
boolean x25_xot_confirm_svc_reset = FALSE;

extern queuetype x25rpQ;

char *const x25_funcs[] = {
    "CHAR.GEN.",
    "DISCARD",
    "ECHO",
    "HELP",
};

const char not_routing_x25_msg[] = "%%X.25 routing is not enabled";

/*
 * x25_route_init
 * Initialize X.25 switch stuff
 */

void x25_route_init (void)
{
    x25_routing = FALSE;
    x25_routing_table = NULL;
    x25_tcp_use_if_defs = FALSE;
}

/*
 * x25_route_lookup
 * Find a route table entry based on X.121 address and CUD.
 * Return exact match on address and CUD if present. Otherwise match
 * address and no CUD if present.
 */

x25routeentrytype *x25_route_lookup (
    idbtype *idb,
    addrtype *addr,
    uchar *cud,
    uint cud_len)
{
    x25routeentrytype *pt, *savept;
    uchar *x121;
    char cud_templ[X25_PAD_CUD_MAX + 1];
    leveltype level;

    if (!x25_routing)
	return(NULL);

    /*
     *  create a copy of the X.121 address to stuff in the 'last matched'
     *  field of the route
     */
    if ((x121 = malloc(addr->length + 1)) == NULL)
	return(NULL);
    bcopy(addr->x121_addr, x121, addr->length);
    x121[addr->length] = '\0';

    /*
     *  create the CUD match template
     */
    if (cud_len) {
	if (cud_len > X25_PAD_CUD_MAX)
	    cud_len = X25_PAD_CUD_MAX;
	bcopy(cud, cud_templ, cud_len);
    }
    cud_templ[cud_len] = '\0';

    savept = NULL;

    level = raise_interrupt_level(NETS_DISABLE);
    for (pt = x25_routing_table; pt; pt = pt->next) {
	if ((pt->type == X25_ROUTE_IF && !interface_up(pt->idb)) ||
	    (pt->type == X25_ROUTE_ALIAS && pt->idb != idb))
	    continue;

	if (regexec(pt->x121pattern, (char *)x121)) {
	    if ((pt->cudpattern == NULL && cud_len == 0) ||
		(cud_len != 0 && pt->cudpattern != NULL &&
		 regexec(pt->cudpattern, cud_templ))) {
		savept = pt;
		break;
	    }

	    /*
	     *  save the first matching route that doesn't specify CUD
	     */
	    if (savept == NULL && pt->cudpattern == NULL)
		savept = pt;
	}
    }
    reset_interrupt_level(level);

    if (savept) {
	if (savept->x121match)
	    free(savept->x121match);
        savept->x121match = x121;
    } else
        free(x121);

    return(savept);
}

/*
 *  x25_for_interf
 *  Determine if a VC's destination X.121 address can be accepted by the
 *  receiving interface, or matches a route.
 *
 *  Note: the rules for matching the null destination address are kinky;
 *  if any user data beyond the protocol ID exists, the routes should
 *  be checked before assigning the call to the receiving interface.
 */
boolean x25_for_interf (
    lcitype *lci,
    char *cud,
    uint cud_len)
{
    hwidbtype *hwidb = lci->lci_idb->hwptr;
    addrtype *addr = &lci->lci_dst_addr;
    x25routeentrytype *route;
    int inx;

    /*
     *  check for an unambiguous match of the interface address
     *  (the null address is ambiguous if user data is present)
     */
    if (((addr->length == 0) && (cud_len == 0)) ||
	(addr->length &&
	 (hwidb->x25_address->length == addr->length) &&
	 (bcmp(hwidb->x25_address->x121_addr,
		addr->x121_addr, addr->length) == 0))) {
	return(TRUE);
    }

    /*
     *  check for an alias or a usable route
     */
    if ((route = x25_route_lookup(lci->lci_idb,
				  &lci->lci_dst_addr, cud, cud_len))) {
	switch (route->type) {
	  case X25_ROUTE_ALIAS:
	    route->usage++;
	    return(TRUE);
	  case X25_ROUTE_IF:
	    /* if route goes back through same serial interface, refuse
	     * to forward the call.
	     */
	    if (hwidb == route->idb->hwptr)
		lci->lci_linktype = LINK_ILLEGAL;
	    else
		lci->lci_linktype = LINK_X25;
	    break;
	  case X25_ROUTE_IPADDR:
	    lci->lci_linktype = LINK_X25TCP;
	    break;
	  case X25_ROUTE_IPPT:
	    lci->lci_linktype = LINK_PAD;
	    break;
	  case X25_ROUTE_FUNC:
	    lci->lci_linktype = LINK_X25SERVICE;
	    break;
	  case X25_ROUTE_CLNPPT:
	  case X25_ROUTE_CMNS:
	  default: /* keep the compiler happy */
	    /*
	     *  not routable
	     */
	    lci->lci_linktype = LINK_ILLEGAL;
	    break;
	}

	if (lci->lci_linktype != LINK_ILLEGAL) {
	    /*
	     *  routable
	     */
	    route->usage++;
	    lci->lci_swtype = route->type;
	    lci->lci_rtp = route;
	}

	/*
	 *  cannot be accepted on interface
	 */
	return(FALSE);
    }

    /*
     *  check for a match with the null destination address
     */
    if (addr->length == 0) {
	return(TRUE);
    }

    /*
     *  check for a match of the interface address with trailing zero digits
     */
    if ((hwidb->x25_address->length < addr->length) &&
	(bcmp(hwidb->x25_address->x121_addr,
	      addr->x121_addr, hwidb->x25_address->length) == 0)) {
	for (inx = hwidb->x25_address->length; inx < addr->length; inx++) {
	    if (addr->x121_addr[inx] != '0')
		return(FALSE);
	}
	return(TRUE);
    }

    /*
     *  unroutable
     */
    return(FALSE);
}

/*
 *  x25_direct_call
 *  Determine what to do with a Call (excepting TCP Calls)
 */

static uchar *no_reverse = "\nReverse charge refused";

boolean x25_direct_call (
    lcitype *lci,
    uchar *cud,
    uint cud_len,
    uchar *cause,
    uchar *diag,
    uchar **reason)
{
    x25map *map;
    uint pid;
    uint pid_len;
    char parse_nuid_ret_value;
    boolean compression;

    *cause = 0;
    *diag = 0;
    *reason = NULL;
    lci->lci_linktype = LINK_ILLEGAL;

    /*
     *  CMNS functionality--
     *  any destination NSAP takes precedence over an X.121 destination
     *  address, so we need to check if we can route the Call using the NSAP
     */
    if (lci->lci_dst_nsap.length &&
	(map = x25_map_find(NULL, &lci->lci_dst_nsap,
			    X25_MATCH_NSAP_BESTFIT, X25_MAPS_CMNS))) {

	/*
	 *  found a CMNS route--
	 *  check that the route isn't circular and that the outgoing
	 *  interface is legal
	 */
	if ((map->map_idb->hwptr == lci->lci_idb->hwptr) ||
	    !(ISIEEEMSK(map->map_idb->hwptr) ?
		ISCMNS_IEEE(map->map_idb->hwptr) :
		is_x25(map->map_idb->hwptr))) {
	    *cause = X25_CLEAR_NOT_OBTAINABLE;
	    *diag = X25_DIAG_CALL_ERROR;
	    *reason = "\nBad CMNS destination";
	} else if (lci->lci_reverse &&
		   ((lci->lci_idb->hwptr->x25_flags & X25_ACCEPTREVERSE) ==
			0)) {
	    *cause = X25_CLEAR_REVERSE_CHARGE_ERR;
	    *diag = X25_DIAG_NO_INFO;
	    *reason = no_reverse;
	} else {
	    lci->lci_map = map;
	    lci->lci_linktype = LINK_CMNS;
	}

	return(*cause == 0);
    }

    /*
     *  try to decode the CUD; even if the Call is routed, we need to
     *  skip any recognized PID when scanning the route table
     */
    compression = FALSE;
    pid = x25_encaps_parse_cud_pid(lci, cud, cud_len, &pid_len, &compression);

    /*
     *  determine if the Call can be routed or accepted
     */
    if (!x25_for_interf(lci, &cud[pid_len], cud_len - pid_len)) {
	/*
	 *  set return values if the Call isn't routable
	 */
	if (lci->lci_rtp == NULL) {
	    *cause = X25_CLEAR_NOT_OBTAINABLE;
	    *diag = X25_DIAG_INVALID_CALLED_ADDR;
	    *reason = "\nCannot route call";
        } else if ((lci->lci_reverse && (lci->lci_linktype != LINK_PAD)) &&
		   ((lci->lci_idb->hwptr->x25_flags & X25_ACCEPTREVERSE) ==
			0)) {
	    *cause = X25_CLEAR_REVERSE_CHARGE_ERR;
	    *diag = X25_DIAG_NO_INFO;
	    *reason = no_reverse;
	}
    } else {
	/*
	 *  we have a Call destined for the receiving interface--
	 *  check if we can accept this as a valid encapsulation or PAD Call
	 */
	lci->lci_linktype = pid;

	/*
	 *  map the source address for an incoming Call, taking care
	 *  to map to any single-VC protocol map first
	 */
	map = x25_map_find(lci->lci_idb->hwptr, &lci->lci_src_addr,
			   X25_MATCH_EXACT | X25_MATCH_INCOMING,
			   x25_map_link2service(pid));

	/*
	 *  if an incoming multiprotocol Call can't map a protocol that
	 *  requires a single VC, look for a map that will
	 */
	if (map && (pid != map->map_linktype) &&
	    ((map->map_services & X25_MAPE_SINGLE_VC) == 0)) {
	    x25map *map2;

	    map2 = x25_map_find(lci->lci_idb->hwptr, &lci->lci_src_addr,
				X25_MATCH_EXACT | X25_MATCH_INCOMING,
				X25_MAPE_SINGLE_VC);
	    if (map2 && (map2->map_nvc < map2->map_maxnvc))
		map = map2;
	}

	if (map) {
	    /*
	     *  kill any VC in hold-down
	     */
	    x25_map_flush_hold(map);

	    if ((pid != map->map_linktype) &&
		(pid != LINK_X25_MULTI_ENC))
		errmsg(&msgsym(PARTIALMAP, X25), map->map_idb->namestring,
			link2name(pid), lci->lci_src_addr.x121_addr,
			link2name(map->map_linktype));
	}

	/*
	 *  if no map is found for an IP encapsulation, check to
	 *  see if one can be created
	 */
	if ((map == NULL) && (pid == LINK_IP))
	    map = x25_ip_icall_setup(lci);

	/*
	 *  determine if we can accept the Call
	 *  Acceptance check for reverse charge facility for pad calls is done
  	 *  in pad_acceptcall to allow reverse charge calls for protocol 
	 *  translator address that has reverse charge configured even though
	 *  the interface doesn't accept them 
	 */

        if ((lci->lci_reverse && (lci->lci_linktype != LINK_PAD)) &&
	    ((map == NULL) || ((map->map_flags & X25_MAPF_REV_OK) == 0)) &&
	    ((lci->lci_idb->hwptr->x25_flags & X25_ACCEPTREVERSE) == 0)) {
	    *cause = X25_CLEAR_REVERSE_CHARGE_ERR;
	    *diag = X25_DIAG_NO_INFO;
	    *reason = no_reverse;
	} else if (compression &&
		   ((map == NULL) ||
		    ((map->map_flags & X25_MAPF_PBP) == 0) ||
		    ((x25_map_link2service(pid) | X25_MAPS_ENCAPS) !=
			X25_MAPS_ENCAPS))) {
	    *cause = X25_CLEAR_NOT_OBTAINABLE;
	    *diag = X25_DIAG_CALL_ERROR;
	    *reason = "\nCompression not valid";
	} else if (!map) {
	    if ((pid != LINK_PAD) ||
		(lci->lci_idb->hwptr->x25_flags & X25_PADMAP)) {
		*cause = X25_CLEAR_NOT_OBTAINABLE;
		*diag = X25_DIAG_INVALID_CALLING_ADDR;
		*reason = "\nAddress map failed";
	    }
	} else if ((pid == LINK_X25_MULTI_ENC) &&
		   (map->map_services & X25_MAPE_CISCO_ONLY)) {
	    *cause = X25_CLEAR_NOT_OBTAINABLE;
	    *diag = X25_DIAG_CALL_ERROR;
	    *reason = "\nInvalid encapsulation requested for map";
	} else if (!x25_map_match_service(map, x25_map_link2service(pid)) ) {
	    *cause = X25_CLEAR_ACCESS_BARRED;
	    *diag = X25_DIAG_CALL_ERROR;
	    *reason = "\nProtocol not mapped to source address";
	} else if (lci->lci_fast == X25_FACILITY_FAST_SELECT_RES) {
	    *cause = X25_CLEAR_NOT_OBTAINABLE;
	    *diag = X25_DIAG_PACK_INCOMPAT_FACILITY;
	    *reason = "\nRestricted fast select";
	} else if (lci->lci_nuid && lci->lci_nuidlen &&
		   map->map_fac.cisco_nuid &&
		   (parse_nuid_ret_value = x25_parse_nuid(lci)) != TRUE) {
	    if (parse_nuid_ret_value == X25_ERROR) {
		*cause = X25_CLEAR_ACCESS_BARRED;
		*diag = X25_DIAG_INVALID_NUID_FACILITY;
	    } else {
		*cause = X25_CLEAR_INVALID_FACILITY;
		*diag = X25_DIAG_FACILITY_PARAM_ILLEGAL;
	    }
	    *reason = "\nNetwork User ID failure";
	} else if (lci->lci_nuid && lci->lci_nuidlen &&
		   !map->map_fac.cisco_nuid &&
		   ((lci->lci_nuidlen != map->map_fac.nuid_len) ||
		    bcmp(lci->lci_nuid, map->map_fac.nuid, lci->lci_nuidlen))) {
	    *cause = X25_CLEAR_ACCESS_BARRED;
	    *diag = X25_DIAG_INVALID_NUID_FACILITY;
	    *reason = "\nNetwork User ID failure";
	} else if (compression && !x25_pbp_init(lci, FALSE)) {
	    *cause = X25_CLEAR_NOT_OBTAINABLE;
	    *diag = X25_DIAG_NO_INFO;
	    *reason = "\nUnable to initialize compression";
	} else if (!x25_map_add_lci(lci, map)) {
	    *cause = X25_CLEAR_NUMBER_BUSY;
	    *diag = X25_DIAG_LC_UNAVAIL;
	    *reason = "\nMaximum NVCs exceeded";
	}
    }

    if (*cause) {
	lci->lci_linktype = LINK_ILLEGAL;
	return(FALSE);
    }

    return(TRUE);
}

/*
 * x25_forus_stream
 * Returns an idb pointer which would result based on the routing table
 * and incoming X.121 address and Call User Data. This routine is used by
 * the incoming stream connections to crack the chicken-egg problem. When
 * an incoming call arrives over a stream such as TCP, we need an idb to
 * associate an LCN and address map.
 */

x25routeentrytype *x25_forus_stream (
    addrtype *addr,
    uchar *cud,
    int cud_len)
{
    x25routeentrytype *route;
    uint pid_len = 0;
    boolean compress;

    /*
     *  skip past any recognized PID
     */
    (void) x25_encaps_parse_cud_pid(NULL, cud, cud_len, &pid_len, &compress);

    if (pid_len >= cud_len)
	cud_len = 0;
    else
	cud_len -= pid_len;

    cud += cud_len;

    /*
     *  Check routing table for a route to an interface
     */
    if ((route = x25_route_lookup(NULL, addr, cud, cud_len)) &&
	(route->type == X25_ROUTE_IF)) {
	route->usage++;
	if (is_x25(route->idb->hwptr) ||
	    ISCMNS_IEEE(route->idb->hwptr))
	    return(route);
    }

    return(NULL);
}

/*
 * x25_add_route
 * Add and remove X.25 routes from the routing table
 */

void x25_add_route (
    boolean add,
    uchar *x121,
    uchar *cud,
    uchar *saddr,
    uchar *daddr,
    idbtype *idb,
    void *data,
    int service,
    int pos,
    int no_ipaddr,		/* range 1 - 6 */
    ipaddrtype ipaddr_1,
    ipaddrtype ipaddr_2,
    ipaddrtype ipaddr_3,
    ipaddrtype ipaddr_4,
    ipaddrtype ipaddr_5,
    ipaddrtype ipaddr_6,
    ulong keepalive_period,
    int keepalive_tries)
{
    x25routeentrytype *pt, *ppt;
    leveltype level;
    ipaddrtype ipaddr[X25_MAX_ALTERNATE];
    short j;

    if (!x25_routing) {
	printf(not_routing_x25_msg);
	return;
    }

    if (service != X25_ROUTE_IPADDR) {/*fix up for alternate routing*/
	no_ipaddr = 0;	              /*no ip address's present*/
    } else {                          /*ip address present*/
	ipaddr[0] = ipaddr_1;         /*place in array since parser can't*/
	if (no_ipaddr > 1) {          /*alternate ip routes present*/
	    ipaddr[1] = ipaddr_2;
	    ipaddr[2] = ipaddr_3;
	    ipaddr[3] = ipaddr_4;
	    ipaddr[4] = ipaddr_5;
	    ipaddr[5] = ipaddr_6;
	}
    }

    level = raise_interrupt_level(NETS_DISABLE);

    /* Before adding do check of the table for
       pre-existing entry and delete it. 
       The reason we scan first is to catch cases 
       where pos are different but same route entry.
    */
    if (add) {
         x25routeentrytype *ptm, *pptm;

    	ptm = pptm = x25_routing_table;

	while (ptm != NULL) {
	   if (!strcasecmp((char *)x121, (char *)ptm->x121)) {
	    if (service == X25_ROUTE_IF && idb == ptm->idb &&
		!strcasecmp((char *)cud, (char *)ptm->cud))
		break;
	    if (service == X25_ROUTE_IPADDR && ipaddr[0] == ptm->ipaddr[0] &&
		!strcasecmp((char *)cud, (char *)ptm->cud))
		break;
	    if (service == X25_ROUTE_ALIAS && idb == ptm->idb &&
		!strcasecmp((char *)cud, (char *)ptm->cud))
		break;
	    if (service == X25_ROUTE_FUNC && data == ptm->data &&
		!strcasecmp((char *)cud, (char *)ptm->cud))
		break;
	   }
	   pptm = ptm;
	   ptm = ptm->next;
	}
        if (ptm != NULL) { /* a match */	 
	   if (ptm == pptm)
		x25_routing_table = ptm->next;
	    else
		pptm->next = ptm->next;
	    free(ptm->x121pattern);
	    free(ptm->x121);
	    free(ptm->cudpattern);
	    free(ptm->cud);
	    free(ptm->repsrc);
	    free(ptm->repdst);
	    free(ptm->x121match);
	    free(ptm);
	}
    }

    pt = ppt = x25_routing_table;

    while (pt != NULL) {
	if (pos != -1 && --pos == 0)
	    break;
	if (pos == -1 && !strcasecmp((char *)x121, (char *)pt->x121)) {
	    if (!add)
		break;
	    if (service == X25_ROUTE_IF && idb == pt->idb &&
		!strcasecmp((char *)cud, (char *)pt->cud))
		break;
	    if (service == X25_ROUTE_IPADDR && ipaddr[0] == pt->ipaddr[0] &&
		!strcasecmp((char *)cud, (char *)pt->cud))
		break;
	    if (service == X25_ROUTE_ALIAS && idb == pt->idb &&
		!strcasecmp((char *)cud, (char *)pt->cud))
		break;
	    if (service == X25_ROUTE_FUNC && data == pt->data &&
		!strcasecmp((char *)cud, (char *)pt->cud))
		break;
	}
	ppt = pt;
	pt = pt->next;
    }
    if (!add) {
	if (pt == NULL)
	    printf("\nCouldn't find matching routing entry to delete");
	else {
	    if (pt == ppt)
		x25_routing_table = pt->next;
	    else
		ppt->next = pt->next;
	    free(pt->x121pattern);
	    free(pt->x121);
	    free(pt->cudpattern);
	    free(pt->cud);
	    free(pt->repsrc);
	    free(pt->repdst);
	    free(pt->x121match);
	    free(pt);
	}
    } else {
	if (x25_routing_table == NULL) {
	    x25_routing_table =	malloc(sizeof(x25routeentrytype));
	    if (x25_routing_table == NULL) {
		reset_interrupt_level(level);
		printf(nomemory);
		return;
	    }
	    pt = x25_routing_table;
	} else if (pt == ppt) {
	    pt = malloc(sizeof(x25routeentrytype));
	    if (pt == NULL) {
		reset_interrupt_level(level);
		printf(nomemory);
		return;
	    }
	    pt->next = x25_routing_table;
	    x25_routing_table = pt;
	} else if (pt == NULL || pos == 0) {
	    ppt->next =	malloc(sizeof(x25routeentrytype));
	    if (ppt->next == NULL) {
		reset_interrupt_level(level);
		printf(nomemory);
		return;
	    }
	    ppt->next->next = pt;
	    pt = ppt->next;
	}
	setstring((char **)&pt->x121, x121);
	free(pt->x121pattern);
	pt->x121pattern = regcomp(pt->x121);
	setstring((char **)&pt->cud, cud);
	free(pt->cudpattern);
	pt->cudpattern = regcomp(pt->cud);
	setstring((char **)&pt->repsrc, saddr);
	setstring((char **)&pt->repdst, daddr);
	pt->x121match = NULL;
	pt->type = service;
	if (service == X25_ROUTE_IPADDR) {
	    pt->idb = NULL;
	    pt->source_idb = idb;
	} else {
	    pt->idb = idb;
	    pt->source_idb = NULL;
	}
	for (j=0; j < no_ipaddr; j++) 
	   pt->ipaddr[j] = ipaddr[j];
	pt->ipaddr_max = no_ipaddr;
	pt->data = data;
	pt->usage = 0;
	pt->tcp_keepalive_period = keepalive_period;
	pt->tcp_keepalive_tries = keepalive_tries;
    }
    reset_interrupt_level(level);
}

/*
 * show_x25route
 * Display X.25 routing information
 */

static const char x25rthdr[] =
"Number             X.121               CUD      Forward To";

void show_x25route (void)
{
    x25routeentrytype *pt;
    int entry;
    short j;

    if (!x25_routing) {
	printf(not_routing_x25_msg);
	return;
    }
    pt = x25_routing_table;
    entry = 0;
    automore_enable(x25rthdr);
    while (pt != NULL) {
	entry++;
	printf("\n%-3d    %27s %12s ", entry, pt->x121,
	       pt->cud ? pt->cud : " ");
	switch (pt->type) {
	  case X25_ROUTE_IF:
	    printf("%s", pt->idb->namestring);
	    break;
	  case X25_ROUTE_IPADDR:
	    printf("%i", pt->ipaddr[0]);
	    if (pt->ipaddr_max > 1)
		for (j=1; j < pt->ipaddr_max; j++)
		    printf("\n \t\t\t\t\t\t%i", pt->ipaddr[j]);
	    break;
	  case X25_ROUTE_IPPT:
	    printf("translation");
	    break;
	  case X25_ROUTE_ALIAS:
	    printf("alias %s", pt->idb->namestring);
	    break;
	  case X25_ROUTE_FUNC:
	    printf("function %s", x25_funcs[(int)pt->data]);
	    break;
	}
	printf(", %d uses", pt->usage);
	if (pt->repsrc != NULL || pt->repdst != NULL) {
	    printf("\n    Sub-source %s Sub-dest %s",
		pt->repsrc ? pt->repsrc : "(none)",
		pt->repdst ? pt->repdst : "(none)");
	}
	pt = pt->next;
    }
    automore_disable();
}

/*
 * pick_x25_idb
 * pick an interface on which to send a call for a specific address/cud
 * pair.  This is used by the PAD code, and potentially by other software
 * that wants to originate an X.25 call, but doesn't know which idb to use.
 */
idbtype *pick_x25_idb (
    uchar *x121addr,
    uchar *cud,
    int cud_len)
{
    addrtype addr;
    hwidbtype *hwidb;
    x25routeentrytype *route;

    memset(&addr, 0, sizeof(addr));
    addr.type = ADDR_X121;
    addr.length = strlen(x121addr);
    bcopy(x121addr, addr.x121_addr, addr.length);

    route = x25_route_lookup(NULL, &addr, cud, cud_len);
    if (route && route->type == X25_ROUTE_IF)
	return(route->idb);
    /*
     * There is no explicit route to this destination.  Return the first
     * x.25 interface as a "best guess"
     */
    FOR_ALL_HWIDBS(hwidb)
	if (is_x25(hwidb))
	    break;
    return(firstsw_or_null(hwidb));
}
