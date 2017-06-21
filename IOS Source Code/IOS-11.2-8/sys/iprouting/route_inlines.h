/* $Id: route_inlines.h,v 3.2.60.2 1996/08/28 12:53:53 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/route_inlines.h,v $
 *------------------------------------------------------------------
 * Inline routines for IP routing
 *
 * July 1993, Bruce Cole
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: route_inlines.h,v $
 * Revision 3.2.60.2  1996/08/28  12:53:53  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.1  1996/03/18  20:29:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:50:36  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  14:30:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:37:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:10:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/23  21:41:08  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Reference renamed fields.  Clean up various interfaces to the IP
 * forwarding table to support protocol-specific functions.
 *
 * Revision 2.1  1995/06/07  21:10:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <string.h>
#include <ciscolib.h>
#include "logger.h"			/* Required for next include */
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../os/chunk.h"

extern chunk_type *rdb_chunks;

/*
 * rdbtype's are now allocated and controlled by the chunk manager,
 * not by the main memory pool, so we need special versions of the
 * malloc/free/lock/refcount functions that implement chunk controls.
 */

static inline rdbtype *rdb_malloc(void)
{
    rdbtype *rdb;

    rdb = chunk_malloc(rdb_chunks);

    if (rdb) {
	memset(rdb, 0, sizeof(rdb));
	rdb->refcount = 1;
    }

    return rdb;
}

/*
 * rdb_free
 *
 * Free an RDB.  Drops the refcount and frees the structure if the refcount
 * went to zero.  Does protocol-specific cleanup if the structure is about
 * to be freed, or if the caller requests it.  The former is really a
 * last-ditch paranoia effort;  the real cleanup should normally take place
 * upon request.  cleanup_now will normally be TRUE, except for rdb_free's
 * that undo rdb_lock's.
 */
static inline void rdb_free(rdbtype *rdb, boolean cleanup_now)
{
    pdbtype *pdb;

    if (!rdb)
	return;

    if (rdb->refcount < 1) {
	errmsg(&msgsym(BADSHARE, SYS), "rdb_free", rdb, rdb->refcount);
	return;
    }


    /*
     * Do protocol-specific cleanup if requested, or if freeing the RDB.
     */
    if (cleanup_now || rdb->refcount == 1) {
	pdb = INDEX2PDB(rdb->pdbindex);
	if (pdb && pdb->rdb_cleanup) {
	    (*pdb->rdb_cleanup)(pdb, rdb);
	}
    }
    if (--rdb->refcount)
	return;
    chunk_free(rdb_chunks, rdb);
    return;
}

static inline void rdb_lock(rdbtype *rdb)
{
    if (rdb->refcount < 1) {
	rdb->refcount = 1;
	errmsg(&msgsym(BADSHARE, SYS), "rdb_lock", rdb, rdb->refcount);
	return;
    }

    rdb->refcount++;
}

static inline int rdb_refcount(rdbtype *rdb)
{
    return rdb->refcount;
}
/*
 * ndb_mask
 * Return the mask appropriate for a given routing descriptor.
 */
static inline ipaddrtype ndb_mask(ndbtype *db)
{
    return db->attrflags & NET_SUBNETTED ?
	get_majormask(db->netnumber) : db->netsmask;
}

/*
 * ip_def_metric_configured
 * Determine if a default metric has been configured for a particular
 * redistribution.  Redistribution of connected is a special case as
 * we do not want metrics configured via the "default-metric" command
 * to apply to connected routes.
 */

static inline boolean ip_def_metric_configured (pdbtype *pdb, int index)
{
    if (pdb->metric[index].flag)
	return(TRUE);
    if (pdb->defaultmetricflag && (index != pdb_CONNECTED->index))
	return(TRUE);
    return(FALSE);
}

/*
 * iprouting_distcmp_inline
 * Compare administrative distances in the traditional manner.
 * Return an indication of whether or not 'our' distance is better than
 * 'their' distance.
 */

static inline enum DISTTYPE
iprouting_distcmp_inline (uchar our_dist, uchar their_dist)
{
    if (our_dist > their_dist)
	return(IP_DIST_WORSE);
    if (our_dist < their_dist)
	return(IP_DIST_BETTER);
    return(IP_DIST_SAME);
}

/*
 * ip_own_ndb
 * Determine if a route is owned by the given pdb.
 */

static inline boolean ip_own_ndb (ndbtype *ndb, pdbtype *pdb)
{
    return(ndb->pdbindex == pdb->index);
}

/*
 * ip_ndb_pdbmask
 * Return a mask of those pdb's which either own the given network route
 * or are configured over it.
 *
 *("Configured over" means the pdb references the network route via 
 * the "network" config command.)
 */

static inline ulong ip_ndb_pdbmask (ndbtype *ndb)
{
    return(ndb->pdb_overmask | INDEX2MASK(ndb->pdbindex));
}

/*
 * ip_pdb_on_ndb
 * Determine if the given pdb is one of those which either owns or is
 * configured over the given network route.
 */

static inline boolean ip_pdb_on_ndb (ndbtype *ndb, pdbtype *pdb)
{
    return((ndb->pdb_overmask | INDEX2MASK(ndb->pdbindex)) & pdb->mask);
}

/*
 * ip_redist_flag
 * Determine if the given pdb may wish to redistribute the given route.
 *
 * If the given pdb is redistributing any pdb which either owns or is
 * configured over the given route then return TRUE.
 */

static inline boolean ip_redist_flag (ndbtype *ndb, pdbtype *pdb)
{
    return(pdb->distflag & (ndb->pdb_overmask | INDEX2MASK(ndb->pdbindex)));
}

/*
 * ospf_check_match_routetype
 * Determine if the route-type of the given ndb is the type that is allowed
 * by argument of match keyword in the old style "redistribute ospf match
 * internal external 1 external 2" command.   Caller of this routine
 * should have checked that src_pdb is an OSPF pdb.
 */
static inline boolean ospf_check_match_routetype (pdbtype *pdb, 
						  pdbtype *src_pdb,
						  ndbtype *ndb)
{
    /*
     * if it matches the route type for OSPF redistribution
     */
    if (ndb->route_type & pdb->criteria[src_pdb->index].routetype)
	return(TRUE);

    /*
     * if it is a connected net and pdb is redistributing internal OSPF route
     */
    if ((ndb->attrflags & NET_ATTACHED) &&
	(pdb->criteria[src_pdb->index].routetype & OSPF_IROUTE_TYPES))
	return(TRUE);

    return(FALSE);
}

/*
 * ip_onifsubnet
 * Return TRUE if source IP address resides on one of the subnetworks
 * configured on this interface.
 */

static inline boolean ip_onifsubnet (idbtype *idb, ipaddrtype source)
{
    ipaddtype *secptr;

    /*
     * Let unnumbered interfaces match any source
     */
    if (idb->ip_unnumbered)
	return(TRUE);
    if ((idb->ip_address & idb->ip_nets_mask) == (source & idb->ip_nets_mask))
	return(TRUE);
    for (secptr = (ipaddtype *) idb->ip_addressQ.qhead; secptr;
	 secptr = secptr->next) {
	if ((secptr->address & secptr->mask) == (source & secptr->mask))
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * ip_free_all_rdb
 * Delete all rdbs of a ndb and set ndb->routecount to 0.
 * This is similar to free_all_rdbs() except no cache cleanup is done here.
 */

static inline void ip_free_all_rdb (ndbtype *db)
{
    int ki;

    for (ki = 0; ki < db->routecount; ki++) {
	rdb_free(db->rdb[ki], TRUE);
	db->rdb[ki] = NULL;
    }
    db->routecount = 0;
}

/*
 * ip_pdb_net_advertise
 * Given an interface's IP address, determine if the pdb is configured to
 * run over the majornet represented by that address.
 * As net_lookup is likely to return a dummy ndb, be careful to not pay
 * attention to the actual ownership of that route.
 */

static inline boolean
ip_pdb_net_advertise (pdbtype *pdb, ipaddrtype ifaddr, ndbtype **ndb)
{
    *ndb = net_lookup(ifaddr, FALSE);
    if (*ndb == NULL)
	return(FALSE);
    if ((*ndb)->pdb_overmask & pdb->mask)
	return(TRUE);
    return(FALSE);
}

