/* $Id: irdp.c,v 3.4.18.2 1996/06/28 23:18:58 hampton Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/irdp.c,v $
 *------------------------------------------------------------------
 * irdp.c -- ICMP Router Discovery Protocol
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: irdp.c,v $
 * Revision 3.4.18.2  1996/06/28  23:18:58  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.18.1  1996/06/18  01:47:42  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/07  16:13:12  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.3  1996/02/01  06:05:43  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/17  17:34:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/26  17:42:54  pst
 * CSCdi40561:  router crash in ip_idrp_goingdown
 *
 * Revision 2.4  1995/06/30  05:21:18  hampton
 * Move externs for the random number generator to their own file.  Remove
 * redundant initialization of the random number generator.  [CSCdi36588]
 *
 * Revision 2.3  1995/06/28 03:21:16  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.2  1995/06/09 13:06:20  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:08:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "../ui/debug.h"
#include "route.h"
#include "../ip/ip.h"
#include "../ip/ip_debug.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_irdp.h"
#include "../if/network.h"
#include "irdp.h"
#include "sys_registry.h"
#include "iprouting_debug.h"
#include "../util/random.h"

static void show_irdp_int(idbtype *);

/*
 * ip_irdp_command
 * Process all IRDP server related commands
 */

void ip_irdp_command (parseinfo *csb)
{
    int preference, holdtime;
    idbtype *idb = csb->interface;
    rproxy *ptr;
    ipaddrtype address;
    
    if (csb->nvgen) {
	
	if (!idb->ip_enabled || (!idb->ip_irdp_enabled &&
				 !idb->ip_info->mobile_beaconing))
	    return;
	
	switch (csb->which) {
	case IRDP_CMD:
	    nv_write(idb->ip_irdp_enabled, csb->nv_command);
	    break;
	case IRDP_CMD_ADDRESS:
	    for (ptr = (rproxy *) idb->ip_irdp_proxyQ.qhead; ptr;
		 ptr = ptr->next) {
		nv_write(TRUE, "%s %i %d", csb->nv_command, ptr->address,
			 ptr->preference);
	    }
	    break;
	case IRDP_CMD_HOLDTIME:
	    nv_write(idb->ip_irdp_holdtime != IRDP_HOLDTIME, "%s %d",
		     csb->nv_command, idb->ip_irdp_holdtime);
	    break;
	case IRDP_CMD_PREFERENCE:
	    nv_write(idb->ip_irdp_preference != IRDP_PREFERENCE, "%s %d",
		     csb->nv_command, idb->ip_irdp_preference);
	    break;
	case IRDP_CMD_MAXADVERT:
	    if (idb->hwptr->status & IDB_IEEEMASK)
		nv_write(idb->ip_irdp_maxadvert != IRDP_MAXADVERT,
			 "%s %d", csb->nv_command, idb->ip_irdp_maxadvert);
	    else
		nv_write(idb->ip_irdp_maxadvert != IRDP_XMAXADVERT,
			 "%s %d", csb->nv_command, idb->ip_irdp_maxadvert);
	    break;
	case IRDP_CMD_MINADVERT:
	    if (idb->hwptr->status & IDB_IEEEMASK)
		nv_write(idb->ip_irdp_minadvert != IRDP_MAXADVERT * 3 / 4,
			 "%s %d", csb->nv_command, idb->ip_irdp_minadvert);
	    else
		nv_write(idb->ip_irdp_maxadvert != IRDP_XMAXADVERT * 3 / 4,
			 "%s %d", csb->nv_command, idb->ip_irdp_minadvert);
	    break;
	case IRDP_CMD_MULTICAST:
	    nv_write(idb->ip_irdp_multicast, "%s", csb->nv_command);
	    break;
	default:
	    bad_parser_subcommand(csb, GETOBJ(int,1));
	    break;
	}
	return;
    }
    
    switch (csb->which) {
    case IRDP_CMD:
	if (!idb->ip_irdp_enabled && csb->sense) {
	    
	    ip_irdp_setup_if(idb);
	    while (idb->ip_irdp_proxyQ.qhead) {
		ptr = dequeue(&idb->ip_irdp_proxyQ);
		free(ptr);
	    }
	}
	if (csb->sense) {
	    ip_irdp_comingup(idb);
	    ip_multicast_ctl(idb, ALLSYSTEMS, TRUE);
	    ip_multicast_ctl(idb, ALLROUTERS, TRUE);
	} else {
	    ip_irdp_goingdown(idb);
	    ip_multicast_ctl(idb, ALLSYSTEMS, FALSE);
	    ip_multicast_ctl(idb, ALLROUTERS, FALSE);
	}
	idb->ip_irdp_enabled = csb->sense;
	break;
	
    case IRDP_CMD_ADDRESS:
	address = GETOBJ(paddr,1)->ip_addr;
	preference = *((int *) &GETOBJ(int,1));	/* magic cast for signed dec */
	for (ptr = (rproxy *) idb->ip_irdp_proxyQ.qhead; ptr; ptr = ptr->next){
	    if (ptr->address == address) {
	        if (!csb->sense) {
		    unqueue(&idb->ip_irdp_proxyQ, ptr);
		    free(ptr);
		} else
		    ptr->preference = preference;
		return;
	    }
	}
	if (csb->sense) {
	    ptr = malloc(sizeof(rproxy));
	    if (ptr) {
		ptr->address = address;
		ptr->preference = preference ? preference : IRDP_PREFERENCE;
		enqueue(&idb->ip_irdp_proxyQ, ptr);
	    }
	}
	break;
	
    case IRDP_CMD_HOLDTIME:
	if (csb->sense) {
	    if ((holdtime = GETOBJ(int, 1)) < idb->ip_irdp_maxadvert) {
		printf("%%Holdtime must be greater than maxadvert interval and"
		       "less than %d, defaulting to maxadvertinterval * 3",
		       IRDP_MAX_HOLDTIME);
		holdtime = idb->ip_irdp_maxadvert * 3;
	    }
	    idb->ip_irdp_holdtime = min(holdtime, IRDP_MAX_HOLDTIME);
	} else
	    idb->ip_irdp_holdtime = IRDP_HOLDTIME;
	break;
	
    case IRDP_CMD_PREFERENCE:
	idb->ip_irdp_preference = csb->sense ? GETOBJ(int,1) : IRDP_PREFERENCE;
	break;

    case IRDP_CMD_MAXADVERT:
	if (idb->hwptr->status & IDB_IEEEMASK) {
	    idb->ip_irdp_maxadvert =
		csb->sense ? GETOBJ(int,1) : IRDP_MAXADVERT;
	} else
	    idb->ip_irdp_maxadvert =
		csb->sense ? GETOBJ(int,1) : IRDP_XMAXADVERT;
	
	/* default minimum time if new max makes it out of bounds */
	if (idb->ip_irdp_minadvert > idb->ip_irdp_maxadvert) {
	    idb->ip_irdp_minadvert = idb->ip_irdp_maxadvert * 3 / 4;
	}
	idb->ip_irdp_count = idb->ip_irdp_minadvert;
	break;
	
    case IRDP_CMD_MINADVERT:
	idb->ip_irdp_minadvert = csb->sense ? GETOBJ(int,1) :
	    idb->ip_irdp_maxadvert * 3 / 4;
	if (idb->ip_irdp_minadvert > idb->ip_irdp_maxadvert)
	    idb->ip_irdp_minadvert = idb->ip_irdp_maxadvert * 3 / 4;
	idb->ip_irdp_count = idb->ip_irdp_minadvert;
	break;
	
    case IRDP_CMD_MULTICAST:
	idb->ip_irdp_multicast = csb->sense;
	break;
	
    default:
	bad_parser_subcommand(csb, GETOBJ(int,1));
	break;
    }
}

/*
 * show_ipirdp
 *
 * Display IRDP data structures
 */

void show_ipirdp (parseinfo *csb)
{
    idbtype *idb = GETOBJ(idb,1);
    automore_enable(NULL);
    show_swidb_chain(idb, show_irdp_int, 0);
    automore_disable();
}

/*
 * show_ipirdp_int
 * Display IRDP data structures for a particular interface
 * (called through show_swidb_chain by show_ipirdp)
 */

static void show_irdp_int (idbtype *idb)
{
    rproxy *ptr;
    
    if (!idb->ip_enabled || !idb->ip_irdp_enabled) {
	printf("\n%s has router discovery disabled\n", idb->namestring);
	return;
    }
    printf("\n%s has router discovery enabled\n", idb->namestring);
    if (idb->ip_irdp_maxadvert == 0)
	printf("\nAdvertisements will not be sent except upon solicitation.");
    else {
	printf("\nAdvertisements will occur between every %d and %d seconds.",
	       idb->ip_irdp_minadvert, idb->ip_irdp_maxadvert);
	printf("\nAdvertisements are sent with %scasts.",
	       idb->ip_irdp_multicast ? "multi" : "broad");
    }
    printf("\nAdvertisements are valid for %d seconds.",
	   idb->ip_irdp_holdtime);
    printf("\nDefault preference will be %d.", idb->ip_irdp_preference);
    for (ptr = (rproxy *) idb->ip_irdp_proxyQ.qhead; ptr; ptr = ptr->next)
	printf("\nProxy for %i with preference %d.", ptr->address,
	       ptr->preference);
}

/*
 * ip_irdp_send_report
 * Send a report message out the given interface for the given destination
 */

static void ip_irdp_send_report (idbtype *idb, ipaddrtype destination,
			  boolean shutdown)
{
    ipaddtype *ap;
    rproxy *pp;
    int bytes, count, maxpref;
    paktype *pak;
    icmprdptype *icmp;
    irdpinfo *info;
    ipinfotype *ipinfo;
    boolean mobility;
    mobexttype *mobext;
    ipparamtype params;

    /*
     * Count addresses.  One for the primary, then as many secondary and
     * proxy addresses as have been previously specified.
     */
    
    icmp_traffic.rdpadvsent++;
    
    ipinfo = idb->ip_info;
    mobility = FALSE;
    count = 1;
    if ((ipinfo->mobile_foreign || ipinfo->mobile_home) &&
	ipinfo->mobile_beaconing) {
	mobility = TRUE;
	if (ipinfo->mobile_coa_idb && ipinfo->mobile_coa_idb->ip_address)
	    count++;
    }
    for (ap = (ipaddtype *) idb->ip_addressQ.qhead; ap; ap = ap->next)
	count++;
    for (pp = (rproxy *) idb->ip_irdp_proxyQ.qhead; pp; pp = pp->next)
	count++;
    
    /*
     * Build an ICMP packet from scratch.  Do not use IP sockets.
     */
    bytes = IPHEADERBYTES(NOPT) + ICMPHEADERBYTES + IRDPDATABYTES(count);
    if (mobility) 
	bytes += IRDP_MOBILITY_SIZE;
    pak = getbuffer(bytes);
    if (!pak)
	return;
    
    icmp = (icmprdptype *) ((uchar *)ipheadstart(pak) + IPHEADERBYTES(NOPT));
    icmp->type = ICMP_RDPADVERT;
    if (idb->ip_irdp_enabled)
	icmp->code = IRDP_CODE_ADVERTISE;
    else if (ipinfo->mobile_beaconing)
	icmp->code = IRDP_CODE_MOBILEONLY;
    icmp->checksum = 0;
    icmp->num_addrs = count;
    icmp->entry_size = sizeof(irdpinfo) / sizeof(long);
    icmp->lifetime = shutdown ? 0 : idb->ip_irdp_holdtime;
    
    info = (irdpinfo *) icmp->data;
    info->address = idb->ip_address;
    info->preference = idb->ip_irdp_preference;
    maxpref = info->preference;
    
    for (ap = (ipaddtype *) idb->ip_addressQ.qhead; ap; ap = ap->next) {
	info++;
	info->address = ap->address;
	info->preference = idb->ip_irdp_preference;
    }
    if (!shutdown)
	for (pp = (rproxy *) idb->ip_irdp_proxyQ.qhead; pp; pp = pp->next) {
	    info++;
	    info->address = pp->address;
	    info->preference = pp->preference;
	    maxpref = max(maxpref, pp->preference);
	}
    
    if (mobility) {
	/*
	 * Add the mobility extension.
	 */
	if (ipinfo->mobile_coa_idb && ipinfo->mobile_coa_idb->ip_address) {
	    info++;
	    info->address = ipinfo->mobile_coa_idb->ip_address;
	    info->preference = min(maxpref + 1, MAXINT);
	}
	info++;
	mobext = (mobexttype *)info;
	mobext->extension = IRDP_MOBILITY_EXTENSION;
	mobext->length = IRDP_MOBILITY_LENGTH;
	mobext->seqnum = ++ipinfo->mobile_sequence_num;
	mobext->regreq = ipinfo->mobile_regreq;
	mobext->reserved = 0;
	if (ip_mobile_debug && icmppkt_debug)
	    buginf("\nMobileIP: advertisement sent");
    }

    icmp->checksum = ipcrc((ushort *)icmp, bytes - IPHEADERBYTES(NOPT));
    
    if (icmppkt_debug)
	buginf("\nICMP: src=%i, dst=%i, irdp advertisement sent",
	       idb->ip_address, destination);
    
    if (icmppkt_debug) {
	buginf("\nIRDP: entries=%d, size=%d, lifetime=%d, bytes=%d",
	       icmp->num_addrs, icmp->entry_size, icmp->lifetime, bytes);
	for (info = (irdpinfo *) icmp->data; count--; info++)
	    buginf("\nIRDP: address=%i preference=%d",
		   info->address, info->preference);
    }
    
    params = ipdefaultparams;			/* struct copy */
    params.ttl = (IPMULTICAST(destination)) ? TTL_1 : TTL_DEFAULT;
    ipwrite(pak, bytes, ICMP_PROT, idb->ip_address, destination, idb, NULL,
	    &params);
}

/*
 * iprouting_irdp_send_report
 * Registry wrapper routine for ip_irdp_send_report.  Make sure irdp is
 * enabled before attempting to send a report.
 */

void iprouting_irdp_send_report (idbtype *idb, ipaddrtype destination)
{
    if (idb->ip_irdp_enabled)
	ip_irdp_send_report(idb, destination, FALSE);
}

/*
 * ip_irdp_periodic
 * Called by SERVICE_IF_ONESEC
 * Do IRDP only if IP and (IRDP or mobile beaconing) are both enabled.  If
 * there is a zero 
 * report count, then we are only replying to queries, never
 * gratuitously volunteering information (consider the case of X.25).
 * the next time for an advertisement randomly floats between
 * minadvert and maxadvert
 */

void ip_irdp_periodic (hwidbtype *hwidb)
{
    idbtype *swidb;

    if (!hw_interface_up(hwidb))
	return;
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	if (swidb->ip_enabled &&
	    (swidb->ip_irdp_enabled ||
	     swidb->ip_info->mobile_beaconing) && 
	    swidb->ip_irdp_maxadvert) {
	    if (swidb->ip_irdp_count-- == 0) {
		ip_irdp_send_report(swidb, swidb->ip_irdp_multicast ?
				    ALLSYSTEMS : -1L, 
				    FALSE);
		if (swidb->ip_irdp_initial_count > 1) {
		    swidb->ip_irdp_initial_count--;
		    swidb->ip_irdp_count = IRDP_INITIAL_ADVERT_INTERVAL;
		} else
		    swidb->ip_irdp_count = (swidb->ip_irdp_minadvert ==
					    swidb->ip_irdp_maxadvert) ? 
			swidb->ip_irdp_minadvert : swidb->ip_irdp_minadvert +
			(random_gen() %
			 (swidb->ip_irdp_maxadvert - swidb->ip_irdp_minadvert));
	    }
	}
    }
}

/*
 * ip_irdp_reload
 * Do IRDP protocol if box is reloading.
 */
static void ip_irdp_reload (void)
{
    idbtype *idb;

     FOR_ALL_SWIDBS(idb)
	 ip_irdp_goingdown(idb);
}

/*
 * ip_irdp_goingdown
 * Do IRDP protocol if interface is shutting down
 */

void ip_irdp_goingdown (idbtype *idb)
{
    if (interface_up_simple(idb) && idb->ip_enabled && 
	idb->ip_irdp_enabled) {
	ip_irdp_send_report(idb, idb->ip_irdp_multicast ? ALLSYSTEMS : -1L,
			    TRUE);
    }
}

/*
 * ip_irdp_comingup
 * Do IRDP protocol if interface is coming back up.
 */

void ip_irdp_comingup (idbtype *idb)
{
    idb->ip_irdp_initial_count = IRDP_INITIAL_ADVERTISEMENTS;
    idb->ip_irdp_count = 1;		/* wait one second and then advert */
}

/*
 * irdp_idb_init
 * Initialize IRDP specific parts of the idb
 */

static void irdp_idb_init (idbtype *idb, boolean setup_hwidb)
{
    queue_init(&idb->ip_irdp_proxyQ, 0);
}

/*
 * ip_irdp_setup_if
 * Make sure that all of the interface parameters are set to some sane
 * value.
 */

void ip_irdp_setup_if (idbtype *idb)
{
    /*
     * Do not do broadcasts out point-to-point links or X.25,
     * but do respond to solicitations (maxadvert = 0).
     */
    
    if ((idb->ip_irdp_maxadvert == 0) || (idb->ip_irdp_minadvert == 0)) {
	if (idb->hwptr->status & IDB_IEEEMASK)
	    idb->ip_irdp_maxadvert = IRDP_MAXADVERT;
	else
	    idb->ip_irdp_maxadvert = IRDP_XMAXADVERT;
	idb->ip_irdp_minadvert = idb->ip_irdp_maxadvert * 3 / 4;
    }
    if (idb->ip_irdp_holdtime == 0)
	idb->ip_irdp_holdtime = IRDP_HOLDTIME;
    if (idb->ip_irdp_preference == 0)
	idb->ip_irdp_preference = IRDP_PREFERENCE;
}

/*
 *
 */
void irdp_init (void)
{
    /*
     * Parser chain initialization
     */
    irdp_parser_init();

    /*
     * Register some functions
     */
    reg_add_sw_idb_init(irdp_idb_init, "irdp_idb_init");
    reg_add_swif_goingdown(ip_irdp_goingdown, "ip_irdp_goingdown");
    reg_add_swif_comingup(ip_irdp_comingup, "ip_irdp_comingup");
    reg_add_will_reload(ip_irdp_reload, "ip_irdp_reload");
    reg_add_if_onesec(ip_irdp_periodic, "ip_irdp_periodic");
}
