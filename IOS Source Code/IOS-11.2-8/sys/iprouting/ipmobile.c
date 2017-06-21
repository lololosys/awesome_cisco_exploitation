/* $Id: ipmobile.c,v 3.7.18.5 1996/06/30 09:11:31 agt Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ipmobile.c,v $
 *------------------------------------------------------------------
 * ipmobile.c -- IP mobile host protocol
 *
 * Oct. 1993 Tony Li
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipmobile.c,v $
 * Revision 3.7.18.5  1996/06/30  09:11:31  agt
 * CSCdi61730:  IP Mobility too aggressive
 * Branch: California_branch
 * Prevent delete/add of existing mobile routes.
 * Limit arp refresh to once per keepalive, rather than each minute.
 *
 * Revision 3.7.18.4  1996/06/28  01:21:02  snyder
 * CSCdi61601:  fix some char * that should have been char [] in iprouting
 * Branch: California_branch
 *
 *              You get space back for the pointer
 *              You get space back for each reference
 *              You get bytes out of data space
 *
 * Revision 3.7.18.3  1996/05/04  01:21:48  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.7.18.2  1996/04/15  21:18:09  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.7.18.1  1996/03/18  20:28:08  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.2  1996/03/07  09:49:29  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.2.1  1996/02/20  01:03:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/07  16:13:09  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.6  1996/02/06  02:24:21  billw
 * CSCdi13190:  Increase password characters encryted
 * Tell users max length.
 *
 * Revision 3.5  1996/02/01  06:05:34  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  00:30:27  billw
 * CSCdi13190:  Increase password characters encryted
 * Make sure that the maximum length password that can be encrypted is
 * the maximum length that will be accepted.
 *
 * Revision 3.3  1996/01/22  06:14:23  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  17:34:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/09/25  08:31:04  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.5  1995/08/24  00:47:26  gchristy
 * CSCdi37313:  IP mobility crashes router
 * - punt if ip_arp_incomplete() fails
 *
 * Revision 2.4  1995/07/12  19:45:58  bcole
 * CSCdi36709:  enabling local-area mobility disables proxy-arp
 *
 * Allow for proxy ARP responses in more situations where IP ARP mobility
 * is in use.
 *
 * Revision 2.3  1995/06/23  18:47:55  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.2  1995/06/19  01:07:46  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:07:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "interface_private.h"
#include "login.h"
#include "logger.h"
#include "sys_registry.h"
#include "access.h"

#include "../if/arp.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../ip/ipaccess.h"
#include "../ip/ip_media.h"
#include "../ui/debug.h"
#include "iprouting_debug.h"
#include "../ip/ip_registry.h"
#include "route.h"
#include "irdp.h"
#include "gdp.h"

#include "ipmobile.h"
#include "parser_defs_ipmobile.h"

/*
 * Globals
 */
static queuetype mobilehostdb;			/* mobile host HA database */
static const char ip_bug_arp_mobility[] = "\nIP arp mobility: ";
static const char ip_bug_mobility[] = "\nIP mobility: ";
pdbtype *pdb_MOBILE;
static int pdb_refcounts;


/*
 * ip_mobile_dblookup
 * Given an IP address, perform a lookup in the database and find a
 * matching entry.
 */

mhdbtype *ip_mobile_mhdblookup (ipaddrtype address)
{
    mhdbtype *mhdb;

    for (mhdb = (mhdbtype *)mobilehostdb.qhead; mhdb; mhdb = mhdb->next) {
	if ((mhdb->lower <= address) &&
	    (mhdb->upper >= address))
	    return(mhdb);
    }
    return(NULL);
}

/*
 * ip_mobile_arp_accept
 * Return TRUE if the IP address is ok to be mobile on this interface.
 */

boolean ip_mobile_arp_accept (idbtype *idb, ipaddrtype address)
{
    ipinfotype *ipinfo;
    boolean result;

    ipinfo = idb->ip_info;
    result = TRUE;

    if (!ipinfo)
	result = FALSE;
    else if (!ipinfo->mobile_arp)
	result = FALSE;
    else if (ipinfo->mobile_arp_permit_list) 
	result = route_check(ipinfo->mobile_arp_permit_list, address,
			     MASK_HOST); 

    if (ip_mobile_debug)
	buginf("%s%s add %i %sted", ip_bug_arp_mobility, idb->namestring,
	       address, result ? "accep" : "rejec");
    return(result);
}

/*
 * ip_mobile_arp_new_host
 * We're passed a new arp entry, and it might want to be a mobile host.
 */

void ip_mobile_arp_new_host (uchar *modeptr, ipaddrtype address, idbtype
			     *idb) 
{
    ndbtype *ndb;

    rdbtype *ok;
    arptype *ptr;

    *modeptr = ARP_MOBILE;

    ptr = arp_table_lookup(LINK_IP, address, NULL);

    ndb = ip_rtlookup(address, MASK_HOST);
    if (ndb) {
	if ((ndb->routecount == 1) &&
	    (ndb->rdb[0]->idb == idb) &&
	    (NDB2PROCTYPE(ndb) & PROC_MOBILE)) {
	    if (ip_mobile_debug)
		buginf("%sroute %i unchanged", ip_bug_arp_mobility, address);
	    return;	
	}
	/*
	 * Something changed.
	 */
	ip_route_delete(pdb_MOBILE, ptr->address, MASK_HOST,
			ptr->address, ptr->address, ptr->idb, 0);
    }
    ok = network_update(pdb_MOBILE, address, MASK_HOST, address,
			idb, 1, 0, address, 0, NULL, 0);
    if (ip_mobile_debug)
	buginf("%sroute add %i %s", ip_bug_arp_mobility, address, ok ?
	       "" : "failed");
}

/*
 * ip_mobile_arp_ager
 * Age mobile arp entries.
 */

static void ip_mobile_arp_ager (boolean flushflag, idbtype *idb)
{
    int i;
    arptype *ptr, *pred;
    ipinfotype *ipinfo;
    ulong ticks;

    if (ip_mobile_debug)
	buginf("%saging arp mobility cache entries", ip_bug_arp_mobility);
    for (i = 0; i < NETHASHLEN; i++) {
	pred = (arptype *) &arptable[i]; /* assumes next is a zero offset */
	ptr = pred->next;		/* really "ptr = arptable[i]" */
	for ( ; ptr; pred = ptr, ptr = ptr->next) {
	    if (ptr->mode != ARP_MOBILE) 
		continue;
	    if (idb && (ptr->idb != idb))
		continue;
	    ipinfo = ptr->idb->ip_info;
	    if (!ipinfo)
		continue;

	    /*
	     * If the holdtime is exceeded, delete the entry.
	     */
  	    ticks = ELAPSED_TIME(ptr->time);
 	    if (ip_mobile_debug)
 		buginf("%saging arp entry %i %d %d %d", ip_bug_arp_mobility,
 		       ptr->address, ticks, 
 		       ipinfo->mobile_arp_keepalive, 
 		       ipinfo->mobile_arp_holdtime );

	    if (!ipinfo->mobile_arp || flushflag ||
  		(ticks > ipinfo->mobile_arp_holdtime+ONEMIN)) { 
		pred->next = ptr->next;	/* link out current entry */
		ip_arp_refresh(ptr); /* refresh just in case */
		ip_route_delete(pdb_MOBILE, ptr->address, MASK_HOST,
				ptr->address, ptr->address, ptr->idb, 0);
		free(ptr);		/* flush entry */
		ptr = pred;		/* point to predecessor */
  	    } else if (ticks > ipinfo->mobile_arp_keepalive) { 
  	        /* if holdtime close to keepalive time, arp every minute */
                 /* if we have just passed a multiple of keepalive, arp */
  	        if ((ipinfo->mobile_arp_keepalive*3 > ipinfo->mobile_arp_holdtime) ||
  		    (ticks - ipinfo->mobile_arp_keepalive * (ticks / ipinfo->mobile_arp_keepalive) <= ONEMIN)) {
  		    ip_arp_refresh(ptr);
  		} 

	    }
	}
    }
}

/*
 * ip_mobile_arp_flood
 * Flood an arp to all interfaces configured for mobile arp with a possible
 * exception interface.
 */

static void ip_mobile_arp_flood (idbtype *exception, ipaddrtype dest)
{
    idbtype *idb;
    ipinfotype *ipinfo;

    if (exception && !exception->circ_canforward)
	return;
    FOR_ALL_SWIDBS(idb) {
	if (exception && (exception == idb))
	    continue;
	ipinfo = idb->ip_info;
	if (!ipinfo || !ipinfo->mobile_arp || !idb->ip_enabled ||
	    (idb->hwptr->state != IDBS_UP))
	    continue;
	if (!idb->circ_canforward)
	    continue;

	ip_arp_resolve(idb, dest);
    }
}

/*
 * ip_mobile_arp_receive
 * We've received an arp packet of some form that mobility may (or may not)
 * care about.  Figure out what to do.
 */

static void ip_mobile_arp_receive (idbtype *idb, boolean request, ipaddrtype src,
			    ipaddrtype dst)
{
    idbtype *cable;
    arptype *ptr;

    /*
     * Ignore replies.
     */
    if (!request)
	return;

    /*
     * If the source is supposed to be on this interface, and it's a
     * mobile interface, then flood.
     */
    cable = samecable(src);
    if (cable && (cable == idb)) {
	if (idb->ip_info && idb->ip_info->mobile_arp) {
	    /*
	     * First, create an incomplete entry so that we don't try to
	     * flood it out all interfaces again.  That would be bad.
	     */
	    ptr = arp_table_lookup(LINK_IP, dst, NULL);
	    if (!ptr) {
		if (!ip_arp_incomplete(idb, dst))
		    return;
		ip_mobile_arp_flood(idb, dst);
	    }
	}
    }
}

/*
 * ip_mobile_arp_proxyarp_proto_action
 * Decide if we want to allow proxy arp on this interface.
 * Be somewhat conservative in doing this, as we don't want to disrupt another
 * co-resident router which is also running IP mobility.  Such a router might
 * construe our proxy ARP reply as an ARP packet from the mobile host.
 */

static boolean ip_mobile_arp_proxyarp_proto_action (idbtype *idb, ipaddrtype src,
					     ipaddrtype dst, boolean *allow,
					     uchar *thdw)
{
    idbtype *cable;
    ndbtype *ndb;
    ipinfotype *ipinfo;

    ipinfo = idb->ip_info;
    if (!ipinfo || !ipinfo->mobile_arp)
	return(FALSE);

    *allow = FALSE;			/* assume no */

    /*
     * If the source isn't supposed to be on this interface, then proxy
     * reply. 
     */
    cable = samecable(src);
    if (!cable || (cable != idb)) {
	*allow = TRUE;
	return(TRUE);
    }
    /*
     * If the destination is currently off net and we have a 
     * host route, then we can safely proxy reply.  Even if a router
     * hears this, it's on-cable, so it won't look like a mobile host.
     */
    ndb = ip_rtlookup(dst, MASK_HOST);
    if (ndb) {
	*allow = TRUE;
	return(TRUE);
    }
    if (!ipinfo->mobile_arp_permit_list) {
	/*
	 * Can't discern which IP addresses relate to mobile hosts.
	 */
	return(TRUE);
    }
    if (nfast_check(ipinfo->mobile_arp_permit_list, src)) {
	/*
	 * Request is from someone whom we consider to be a mobile host.
	 */
	*allow = TRUE;
    } else if (!nfast_check(ipinfo->mobile_arp_permit_list, dst)) {
	/*
	 * Request is for a destination which we believe to not be
	 * mobile.
	 */
	*allow = TRUE;
    }
    return(TRUE);
}

/*
 * ip_mobile_onemin
 * Perform once per minute tasks.
 */

static void ip_mobile_onemin (void)
{
    ip_mobile_arp_ager(FALSE, NULL);	/* age arp entries */
}

/*
 * ip_mobile_clear_arp
 * Flush the arp cache.
 */

static void ip_mobile_clear_arp (idbtype *idb)
{
    ip_mobile_arp_ager(TRUE, idb);	/* flush all entries */
}

/*
 * ip_mobile_delete_pdb
 * Delete the pdb when done.
 */

static void ip_mobile_delete_pdb (void)
{
    table_prune_pdb(pdb_MOBILE);
    unqueue(&protoQ, pdb_MOBILE);
    free_pdbindex(pdb_MOBILE);
    free(pdb_MOBILE);
    pdb_MOBILE = NULL;
}

/*
 * ip_mobile_query
 * Inject routes into the routing table from the arp table as requested.
 */

static void ip_mobile_query (pdbtype *pdb, idbtype *idb)
{
    int i;
    arptype *ptr, *pred;
    rdbtype *ok;

    for (i = 0; i < NETHASHLEN; i++) {
	pred = (arptype *) &arptable[i]; /* assumes next is a zero offset */
	ptr = pred->next;		 /* really "ptr = arptable[i]" */
	for ( ; ptr; pred = ptr, ptr = ptr->next) {
	    if (ptr->mode != ARP_MOBILE) 
		continue;
	    if (idb && (ptr->idb != idb))
		continue;
	    if (!ptr->idb->ip_info)
		continue;

	    ok = network_update(pdb_MOBILE, ptr->address, MASK_HOST,
				ptr->address, ptr->idb, 1, 0,
				ptr->address, 0, NULL, 0);
	    if (ip_mobile_debug)
		buginf("%sroute add %i %s", ip_bug_arp_mobility,
		       ptr->address, ok ? "" : "failed");
	}
    }
}

/*
 * ip_mobile_create_pdb
 * Create and initialize pdb_MOBILE.
 */

static void ip_mobile_create_pdb (void)
{
    pdbtype *pdb;

    pdb = pdb_MOBILE;

    if (!pdb) {
	pdb = malloc(sizeof(pdbtype));
	if (!pdb) {
	    return;
	}
	pdb_MOBILE = pdb;
    }
	
    if (pdb->running) {
	if (ip_mobile_debug)
	    buginf("%smobile pdb already exists", ip_bug_mobility);
	return;
    }

    if (pdb->mask)
	free_pdbindex(pdb);

    memset(pdb, 0, sizeof(pdbtype));

    pdb->name = "mobile";
    pdb->family = PDB_IP;
    pdb->pname = "Mobile Router";
    if (allocate_pdbindex(pdb) == -1)
	return;

    pdb->process = NULL;
    pdb->cleanup = NULL;
    pdb->neighbor = NULL;
    pdb->showipproto = NULL;
    pdb->majorupdate = NULL;
    pdb->holddown = simple_holddown;
    pdb->query = ip_mobile_query;
    pdb->add_route = NULL;		/* XXX */
    pdb->redist_callback = NULL;
    pdb->running = TRUE;
    pdb->proctype = PROC_MOBILE;
    pdb->distflag = 0;       
    pdb->redistallowed = 0;		/* we can't redistribute anything */
    pdb->auto_summary = FALSE;
    pdb->exteriors_out = FALSE;
    pdb->exteriors_in = FALSE;
    pdb->broadcasttime = pdb->def_broadcasttime = 0;
    pdb->invalidtime = pdb->def_invalidtime = 0L;
    pdb->holdtime = pdb->def_holdtime = 0L;
    pdb->flushtime = pdb->def_flushtime = 0L;
    pdb->multipath = pdb->def_multipath = DEFMAXROUTES;
    pdb->distance = pdb->def_distance = MOBILE_DISTANCE;
    pdb->distance_cmp = iprouting_distcmp_generic;
    pdb->asystem = 0;
    pdb->atomic_update = FALSE;
    pdb->malloc_backup = FALSE;

    if (!checkqueue(&protoQ, pdb))
	enqueue(&protoQ, pdb);

    return;
}

/*
 * ip_mobile_lock_pdb
 * Increment the reference count for the mobile pdb.  If there isn't one, then
 * create one.
 */

static void ip_mobile_lock_pdb (void)
{
    if (NULL == pdb_MOBILE) {
	if (pdb_refcounts != 0) {
	    if (ip_mobile_debug)
		buginf("%sno pdb and non-zero refcount", ip_bug_mobility);
	}
	ip_mobile_create_pdb();
    } else {
	if (0 == pdb_refcounts) {
	    if (ip_mobile_debug)
		buginf("%s pdb and zero refcount", ip_bug_mobility);
	}
    }
    if (NULL != pdb_MOBILE)		/* in case malloc fails */
	pdb_refcounts++;
}

/*
 * ip_mobile_unlock_pdb
 * Decrement the reference count for the mobile pdb.  If there are no
 * references, delete it.
 */

static void ip_mobile_unlock_pdb (void)
{
    if (NULL == pdb_MOBILE) {
	if (pdb_refcounts != 0) {
	    if (ip_mobile_debug)
		buginf("%sno pdb and non-zero refcount", ip_bug_mobility);
	}
	pdb_refcounts--;
    } else {
	if (0 == pdb_refcounts) {
	    if (ip_mobile_debug)
		buginf("%s pdb and zero refcount", ip_bug_mobility);
	} else
	    pdb_refcounts--;
	if (0 == pdb_refcounts)
	    ip_mobile_delete_pdb();
    }
}

/*
 * ip_mobile_pdb_init
 * Interface into route3.c
 */
static pdbtype *
ip_mobile_pdb_init (ushort asystem)
{
    if (!pdb_MOBILE)
	ip_mobile_create_pdb();
    return pdb_MOBILE;
}

/*
 * ip_mobile_command
 * Parse ip mobility commands.
 */

void ip_mobile_command (parseinfo *csb)
{
    int cmd, list;
    idbtype *idb = csb->interface;
    idbtype *coa_idb;
    ipinfotype *ipinfo;
    acl_headertype *acl = NULL;
    char *name;

    if (!idb)
	return;
    ipinfo = idb->ip_info;
    if (!ipinfo)
	return;
    cmd = GETOBJ(int,1);
    if (csb->nvgen) {
	switch(cmd) {
	case MOBILE_CMD_ARP:
	    if (ipinfo->mobile_arp) {
		nv_write(TRUE, csb->nv_command);
		if ((ipinfo->mobile_arp_keepalive != MOBILE_ARP_DEFKEEP) ||
		    (ipinfo->mobile_arp_holdtime != MOBILE_ARP_DEFHOLD)) 
		    nv_add(TRUE, " timers %d %d",
			   ipinfo->mobile_arp_keepalive/ONEMIN,
			   ipinfo->mobile_arp_holdtime/ONEMIN);
		if (ipinfo->mobile_arp_permit_list)
		    nv_add(TRUE, " access-group %s", 
			   ipinfo->mobile_arp_permit_list->name);
	    }
	    break;
	case MOBILE_CMD_BEACONING:
	    nv_write(ipinfo->mobile_beaconing, "%s", csb->nv_command);
	    nv_add(ipinfo->mobile_regreq, " registration-required");
	    coa_idb = ipinfo->mobile_coa_idb;
	    if (coa_idb &&
		!(coa_idb->hwptr->status & IDB_DELETED))
		nv_add(TRUE, " care-of %s", coa_idb->namestring);
	    break;
	case MOBILE_CMD_FOREIGN:
	    nv_write(ipinfo->mobile_foreign, csb->nv_command);
	    nv_add(ipinfo->mobile_foreign_auth, " authenticated");
	    nv_add((ipinfo->mobile_foreign_list != NULL), " access-group %s",
		   ipinfo->mobile_foreign_list->name);
	    break;
	case MOBILE_CMD_HOME:
	    nv_write(ipinfo->mobile_home, csb->nv_command);
	    break;
	case MOBILE_CMD_NODE:
	    if (ipinfo->mobile_node &&
		(ipinfo->mobile_solicit == GETOBJ(int,5))) {
		nv_write(ipinfo->mobile_node, csb->nv_command);
	    }
	    break;
	case MOBILE_CMD_PROXY:
	    nv_write(ipinfo->mobile_proxy, csb->nv_command);
	    break;
	default:
	    bad_parser_subcommand(csb, cmd);
	}    
	return;
    }

    switch(cmd) {
    case MOBILE_CMD_ARP:
	if (csb->sense) {

	    if (!(idb->hwptr->status & (IDB_FDDI|IDB_ETHER|IDB_TR))) {
		printf("\n%%Mobile arp not supported on this interface type");
		return;
	    }
	    if (GETOBJ(int, 3)) {
		ipinfo->mobile_arp_keepalive = GETOBJ(int,3)*ONEMIN;
		ipinfo->mobile_arp_holdtime = GETOBJ(int,4)*ONEMIN;
	    } else if (!ipinfo->mobile_arp) {
		ipinfo->mobile_arp_keepalive = MOBILE_ARP_DEFKEEP;
		ipinfo->mobile_arp_holdtime = MOBILE_ARP_DEFHOLD;
	    }
	    list = GETOBJ(int,2);
	    name = GETOBJ(string,1);
	    if (list) {
		acl = access_find_or_create_acl(list);
	    } else if (strlen(name)) {
		if (!is_valid_nacl(name)) {
		    printf(nacl_name_error);
		    return;
		}
		acl = access_find_or_create_nacl(name,
						 ACL_IP_NAMED_SIMPLE);
		if (!acl)
		    return;

		switch (acl->type) {
		case ACL_IP_NAMED_SIMPLE:
		    break;

		default:
		    printf("\n% Only standard access-lists are "
			   "supported.");
		    return;
		}

	    }
	    if (acl)
		ipinfo->mobile_arp_permit_list = acl;
	    else if (!ipinfo->mobile_arp)
		ipinfo->mobile_arp_permit_list = NULL;
	    if (!ipinfo->mobile_arp)
		ip_mobile_lock_pdb();
	} else {
	    ipinfo->mobile_arp_keepalive = MOBILE_ARP_DEFKEEP;
	    ipinfo->mobile_arp_holdtime = MOBILE_ARP_DEFHOLD;
	    ipinfo->mobile_arp_permit_list = NULL;
	    if (ipinfo->mobile_arp)
		ip_mobile_unlock_pdb();
	}
	ipinfo->mobile_arp = csb->sense;
	break;
    case MOBILE_CMD_BEACONING:
	ipinfo->mobile_beaconing = csb->sense;
	ipinfo->mobile_regreq = GETOBJ(int,5);
	coa_idb = GETOBJ(idb,1);
	if (coa_idb && (coa_idb != idb))
	    ipinfo->mobile_coa_idb = coa_idb;
	ip_irdp_setup_if(idb);
	if (!idb->ip_irdp_enabled) {
	    if (csb->sense)
		ip_irdp_comingup(idb);
	    else
		ip_irdp_goingdown(idb);
	}
	break;
    case MOBILE_CMD_FOREIGN:
	ipinfo->mobile_foreign = csb->sense;
	break;
    case MOBILE_CMD_HOME:
	ipinfo->mobile_home = csb->sense;
	break;
    case MOBILE_CMD_NODE:
	ipinfo->mobile_node = csb->sense;
	ipinfo->mobile_solicit = GETOBJ(int,5);
	if (ipinfo->mobile_node && ipinfo->mobile_solicit)
	    send_irdpsolicit(csb->interface);
	break;
    case MOBILE_CMD_PROXY:
	ipinfo->mobile_proxy = csb->sense;
	break;
    default:
	bad_parser_subcommand(csb, cmd);
	break;
    }
}

/*
 * ip_mobile_host_command
 * Parser IP mobile host database commands.
 */

void ip_mobile_host_command (parseinfo *csb)
{
    ipaddrtype lower, upper, temp;
    boolean privacy, passseen;
    int enctype, netenctype;
    char *password;
    mhdbtype *mhdb;
    char temppassword[PASSWORDLEN_ENCRYPTED+1];

    if (csb->nvgen) {
	for (mhdb = (mhdbtype *)mobilehostdb.qhead; mhdb; mhdb =
	     mhdb->next) { 
	    nv_write(TRUE, "%s %i", csb->nv_command, mhdb->lower);
	    nv_add(mhdb->upper != mhdb->lower, " %i", mhdb->upper);
	    nv_add(mhdb->privacy, " privacy");
	    if (mhdb->password) {
		nv_add(TRUE, " password");
		switch(mhdb->netenctype) {
		case MOBILE_PASS_CLEAR:
		    nv_add(TRUE, " clear");
		    break;
		case MOBILE_PASS_MD5:
		    nv_add(TRUE, " md5");
		    break;
		}

		/*
		 * If we are encrypting and we have a non encrypted password
		 * encrypt it.
		 */
		if ((mhdb->enctype == ENCRYPT_NONE) &&
		    password_encryption_enable) { 
		    memset(temppassword, 0, sizeof(temppassword));
		    enctype = encrypt(mhdb->password, temppassword,
				      0, PASSWORDLEN_ENCRYPTED);
		    password = NULL;
		    setstring(&password, temppassword);
		    if (password) {
			mhdb->enctype = enctype;
			free(mhdb->password);
			mhdb->password = password;
		    }
		}
		nv_add(mhdb->enctype, " %d", mhdb->enctype);
		nv_add(TRUE, " %s", mhdb->password);
	    }
	}
	return;
    }

    lower = GETOBJ(paddr,1)->ip_addr;
    upper = GETOBJ(paddr,2)->ip_addr;
    if (!upper)
	upper = lower;
    if (upper < lower) {
	temp = lower;
	lower = upper;
	upper = temp;
    }
	
    /*
     * Look for an overlap.
     */
    for (mhdb = (mhdbtype *)mobilehostdb.qhead; mhdb; mhdb = mhdb->next) {
	if ((lower >= mhdb->lower) && (lower <= mhdb->upper))
	    break;
	if ((upper >= mhdb->lower) && (upper <= mhdb->upper))
	    break;
    }

    if (mhdb) {

	/*
	 * Check to insure that the returned entry matches what we wanted.
	 */
	if ((lower != mhdb->lower) ||
	    (upper != mhdb->upper)) {
	    printf("\nOverlapping mobile host entry for %i", mhdb->lower);
	    if (mhdb->upper != mhdb->lower)
		printf(" %i", mhdb->upper);
	    return;
	}
    } else {

	if (!csb->sense)
	    return;

	/*
	 * Malloc a new entry and link it.
	 */
	mhdb = malloc(sizeof(mhdbtype));
	if (!mhdb) {
	    return;
	}
	mhdb->lower = lower;
	mhdb->upper = upper;
	enqueue(&mobilehostdb, mhdb);
    }

    privacy = GETOBJ(int,1);
    netenctype = GETOBJ(int,2);
    enctype = GETOBJ(int,3);
    passseen = GETOBJ(int,4);
    password = GETOBJ(string,1);

    if (csb->sense) {
	if (privacy)
	    mhdb->privacy = privacy;
	if (passseen && password && *password) {

	    /*
	     * If we are encrypting and we got a non encrypted password
	     * encrypt it.
	     */
	    if ((enctype == ENCRYPT_NONE) && password_encryption_enable) {
		memset(temppassword, 0, sizeof(temppassword));
		if (strlen(password) > PASSWORDLEN) {
		    printf("\n%% Overly long Password truncated "
			   "after %d characters", PASSWORDLEN);
		    password[PASSWORDLEN] = 0;
		}
		enctype = encrypt(password, temppassword, 0,
				  PASSWORDLEN_ENCRYPTED);
		password = temppassword;
	    }
	    setstring(&mhdb->password, password);
	    mhdb->enctype = enctype;
	    mhdb->netenctype = netenctype;
	}
    } else {
	if (privacy)
	    mhdb->privacy = FALSE;
	if (passseen) {
	    mhdb->netenctype = MOBILE_PASS_CLEAR;
	    mhdb->enctype = 0;
	    free(mhdb->password);
	    mhdb->password = NULL;
	}
	if (!privacy && !passseen && password && !(*password)) {
	    unqueue(&mobilehostdb, mhdb);
	    free(mhdb->password);
	    free(mhdb);
	}
    }
}

/*
 * ip_mobile_init
 * Initialize mobility module.
 */

void ip_mobile_init (void)
{
    pdb_MOBILE = NULL;
    pdb_refcounts = 0;

    reg_add_onemin(ip_mobile_onemin, "ip_mobile_onemin");
    reg_add_ar_cache_clear(ip_mobile_clear_arp, "ip_mobile_clear_arp");
    reg_add_ip_mobile_arp_accept(ip_mobile_arp_accept,
				 "ip_mobile_arp_accept");
    reg_add_ip_mobile_arp_new_host(ip_mobile_arp_new_host,
				   "ip_mobile_arp_new_host");
    reg_add_ip_arp_receive(ip_mobile_arp_receive, "ip_mobile_arp_receive");
    reg_add_ip_arp_add_incomplete_entry(ip_mobile_arp_flood,
					"ip_mobile_arp_flood");
    reg_add_ip_proxyarp_proto_action(ip_mobile_arp_proxyarp_proto_action,
				     "ip_mobile_arp_proxyarp_proto_action");
    reg_add_iprouting_pdb_init(PROC_MOBILE, ip_mobile_pdb_init,
			       "ip_mobile_pdb_init");
    ip_mobile_parser_init();
}
