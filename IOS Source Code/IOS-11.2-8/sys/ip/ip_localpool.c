/* $Id: ip_localpool.c,v 3.6.10.2 1996/05/21 09:51:09 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/ip_localpool.c,v $
 *------------------------------------------------------------------
 * IP local address pool handling
 *
 * April 1995, Irfan Ashraf
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_localpool.c,v $
 * Revision 3.6.10.2  1996/05/21  09:51:09  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.1  1996/03/18  20:16:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.3  1996/03/16  06:57:49  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.12.2  1996/03/07  09:45:45  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  00:59:07  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/22  14:34:29  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/01/29  07:28:32  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1996/01/03  20:33:11  vandys
 * CSCdi43813:  IP local pools too small for max VTY-Async configs
 *
 * Revision 3.3  1995/12/17  18:29:31  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:33:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/09/18  19:35:45  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.7  1995/08/29  06:05:05  fox
 * CSCdi38536:  peer default ip address pool command does not match
 * documentation
 * Commit changes to mainline in order to match 11.0 Release
 * documentation.
 *
 * Revision 2.6  1995/08/17  01:43:19  tli
 * CSCdi38878:  DHCP: change ip local-pool to ip local pool
 *
 * Revision 2.5  1995/06/30  08:54:07  irfan
 * CSCdi36622:  Turning off IP local address pooling prints garbage
 * Remember a % makes printf() look for an extra parameter.
 *
 * Revision 2.4  1995/06/30  05:21:09  hampton
 * Move externs for the random number generator to their own file.  Remove
 * redundant initialization of the random number generator.  [CSCdi36588]
 *
 * Revision 2.3  1995/06/28 09:26:43  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/20  18:49:12  irfan
 * CSCdi34969:  IP pooling gives addresses that are already in use.
 * Flag a member address in in-use state, as to-be-deleted, when the
 * parent scope is killed. Delete such orphan addresses when released
 * by the user code. Perform additional sanity checks while creating
 * new scopes.
 *
 * Revision 2.1  1995/06/07  20:57:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "interface_private.h"
#include "address.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "logger.h"
#include "ip.h"
#include "../ui/debug.h"
#include "ip_debug.h"
#include "ip_localpool.h"
#include "ip_addrpool.h"
#include "ipaddress.h"
#include "ip_registry.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"


/*
 * Each "pool" is a named range of IP addresses.  Each range
 * exists independantly of one another although in-use addresses
 * are stored in a common queue.
 * The addresses are kept in a local (to the pool) queue to avoid the
 * need to check each element for the correct poolname.  The free
 * queue is a FIFO so that an address is more likely to be available
 * to be given out to the last user who used it.  The user could end
 * up with the same address because his software remembered the address
 * and asked for it again or because we search the freeQ using the
 * user name from authentication.
 */
typedef struct ippool_pooltype_ {
    struct ippool_pooltype_ *next;	/* Next pool list */
    char *name;				/* Pool name */
    ipaddrtype lowip, highip;		/* Boundaries of range */
    int size;				/* # of entries in pool */
    queuetype freeQ;			/* Set of unused addresses in FIFO */
} pool_type;


/*
 * Each IP address has one of these.  They are chained together,
 * and point back to the pool in which they exist.  They are tagged
 * with user and idb as keys for address reuse.
 */
typedef struct pooladdr_type_ {
    struct pooladdr_type_ *next;        /* Next address in list */
    pool_type *pool;                    /* Pool this Address is in */
    ipaddrtype ipaddr;                  /* This address */
    char *user;                         /* Current/last user */
    idbtype *idb;                       /* idb if any */
} pooladdr_type;

static void print_ip_localpool_addr(pooladdr_type *);
static void print_ip_localp_pool(pool_type *);
static void ippool_remove(pool_type *);
static boolean ippool_add(char *, ipaddrtype, ipaddrtype);
static inline void purge_pool_free_addresses(pool_type *);
static inline void purge_pool_used_addresses(pool_type *);
static void purge_pool_addresses(pool_type *);
static void mark_pooladdr_used(pooladdr_type *, idbtype *,
			       char *, char *);
static void mark_pooladdr_free(pooladdr_type *);
static void show_ip_localp(parseinfo *);
static pool_type *find_pool_name (char *);

static pooladdr_type *find_localpool_addr_addr(queuetype *, ipaddrtype);
static pooladdr_type *find_localpool_addr_idb(queuetype *, idbtype *);
static pooladdr_type *find_localpool_addr_ustring(queuetype *, char *);

static void ip_localpool_free(ipaddrtype, idbtype *);
static int ip_localpool_allocate(ipaddrtype *, idbtype *,
				 uchar *, uchar *, ulong);
static void ip_localpool_shutdown(void);
static void ip_localpool_startup(void);
static void ip_localpool_chown(ipaddrtype, idbtype *);
static void show_all_pools (void);

/*
 * Utilities for walking pools and pools
 */
#define FOR_ALL_IPLOCALPOOL_ADDR(que_ptr, pa) \
    for ((pa) = ((pooladdr_type *) ((que_ptr)->qhead)); \
	  (NULL != (pa)); \
	  (pa) = (pa)->next)

#define FOR_ALL_POOLS(ss) \
    for ((ss) = poolQ.qhead; (ss); \
	 (ss) = (ss)->next)

/*
 * Local storage
 */
static queuetype poolQ;
static queuetype iplocalp_inuseQ;

static ip_addresspool_type ippool_pool = \
       {PEER_ADDR_POOL,"Local Database", ip_localpool_startup,
	ip_localpool_shutdown, show_all_pools,
	ip_localpool_allocate, ip_localpool_free,
	ip_localpool_chown};

/*
 * ip_localpool_command
 *
 * Handle pool configuration
 */
void ip_localpool_command (parseinfo *csb)
{
    pool_type *ss;
    ipaddrtype ipa, ll, uu;
    char *poolname;
    ipaddress_type thistype;

    if (csb->nvgen) {

	/* Walk the pool list */

	FOR_ALL_POOLS(ss) {

	    nv_write(TRUE, "%s %s %i", csb->nv_command, ss->name,
		     ss->lowip);
	    if (ss->highip != ss->lowip) {
		nv_add(TRUE, " %i", ss->highip);
	    }
	}
	return;
    }

    /*
     * User issued command
     */
    if (GETOBJ(int,1))
	poolname = DEFAULT_LOCAL_POOL_NAME;
    else
        poolname = GETOBJ(string,1);
    ss = find_pool_name(poolname);
    if (!(csb->sense)) {

	/*
	 * Need to delete this pool
	 */
	if (!ss) {
	    /*
	     * exit silently, as we can't delete a non-existent pool
	     */
	    return;
	}

	/*
	 * Delete pool
	 */
	ippool_remove(ss);

	return;

    }

    /*
     * Add a new pool
     */
    uu = ll = GETOBJ(paddr,1)->ip_addr;
    if (GETOBJ(paddr,2)->type == ADDR_IP) {
	uu = GETOBJ(paddr,2)->ip_addr;
    }
    if ((ll > uu) || ((uu - ll) > MAX_POOL_ADDRS)) {
	printf("\n%%Bad IP range, %i-%i", ll, uu);
	return;
    }
    for (ipa = ll; ipa <= uu; ipa += 1) {
	if (martian(ipa, TRUE)) {
	    printf("\n%%IP address not allowed in pool: %i", ipa);
	    return;
	}
    }

    /*
     * Already configured?
     */
    if (ss) {
	/*
	 * The config may be re-read, so don't do anything here,
	 * except barf if range is not what we have.
	 */
	if ((ss->lowip != ll) || (ss->highip != uu)) {
	    printf("\n%%Pool %s already defined as %i - %i",
		   ss->name, ss->lowip, ss->highip);
	}
	return;
    }

    /*
     * Check for overlap in ranges with other pools
     */
    FOR_ALL_POOLS(ss) {
	if (((ll >= ss->lowip) && (ll <= ss->highip)) ||
	    ((uu >= ss->lowip) && (uu <= ss->highip))) {
	    printf("\n%%IP address range overlaps with pool: %s",
		   ss->name);
	    return;
	}
    }

    /*
     * Check for overlap between pool and hash table
     */
    for (ipa = ll; ipa <= uu; ipa += 1) {
	thistype = ip_ouraddress_type(ipa);
	if (thistype != IPADDR_UNKNOWN) {
	    printf("\n%%IP address %i is already defined as %s.", ipa, 
		   ipaddresstype_string(thistype));
	}
    }

    /*
     * The pool is new; add it
     */
    if (!ippool_add(poolname, ll, uu)) {
	printf(nomemory);
    }
}

/*
 * purge_pool_addresses
 *
 * delete all addresses that belong to the given pool
 * It's not fool-proof as there is a slight chance an
 * address may be missed, because we are switching between
 * queues.
 */
static void purge_pool_addresses (pool_type *ss)
{
    purge_pool_free_addresses(ss);
    purge_pool_used_addresses(ss);
}

/*
 * purge_pool_free_addresses
 *
 * Remove a pool from the free queue
 */
static inline void purge_pool_free_addresses (pool_type *ss)
{
    pooladdr_type *pool_addr;
    pooladdr_type dummy_addr;   /* To keep the next pointer */

    FOR_ALL_IPLOCALPOOL_ADDR(&ss->freeQ, pool_addr) {
	dummy_addr.next = pool_addr->next;
	p_unqueue(&ss->freeQ, pool_addr);
	if (pool_addr->user)
	    free(pool_addr->user);
	free(pool_addr);
	pool_addr = &dummy_addr;
    }
}

/*
 * purge_pool_used_addresses
 *
 * Mark a pool's used addresses as to-be-deleted
 */
static inline void purge_pool_used_addresses (pool_type *ss)
{
    pooladdr_type *pool_addr;

    FOR_ALL_IPLOCALPOOL_ADDR(&iplocalp_inuseQ, pool_addr) {
	if (pool_addr->pool == ss) {
	    pool_addr->pool = NULL;
	}
    }
}

/*
 * find_localpool_addr_addr
 *
 * Find pool element given IP address
 */
static pooladdr_type *find_localpool_addr_addr (queuetype *que,
						ipaddrtype ipaddr)
{
    pooladdr_type *pa;
    
    FOR_ALL_IPLOCALPOOL_ADDR(que, pa) {
	if (pa->ipaddr == ipaddr) {
	    return pa;
	}
    }
    return NULL;
}

/*
 * find_localpool_addr_idb
 *
 * Find pool element given IDB
 */
static pooladdr_type *find_localpool_addr_idb (queuetype *que,
					       idbtype *idb)
{
    pooladdr_type *pa;
    
    FOR_ALL_IPLOCALPOOL_ADDR(que, pa) {
	if (pa->idb == idb) {
	    return pa;
	}
    }
    return NULL;
}

/*
 * find_localpool_addr_user
 *
 * Find last pool element with given user string
 *
 * We return the first element found that matches the user string since
 * it is unlikely that the user will have more than one used element on
 * the free queue.
 */
static pooladdr_type *find_localpool_addr_ustring (queuetype *que,
						   char *ustring)
{
    pooladdr_type *pa;
    
    FOR_ALL_IPLOCALPOOL_ADDR(que, pa) {
	if (!strcmp(pa->user, ustring))
	    return pa;
    }
    return NULL;
}

/*
 * ippool_add
 *
 * Add named pool to IP address pool.  Returns TRUE on success,
 * FALSE on failure.
 */
static boolean ippool_add (char *name_str, ipaddrtype ll, ipaddrtype uu)
{
    pool_type *ss;
    pooladdr_type *pool_addr;

    /*
     * Create a new pool record and link it into the pool queue
     */
    ss = malloc_named(sizeof(pool_type), "ippool_add pool");
    if (NULL == ss) {
	return FALSE;
    }

    /*
     * Duplicate name
     */
    setstring(&ss->name, name_str);
    if (!ss->name) {
	free(ss);
	return FALSE;
    }

    /*
     * Fill in IP address(es)
     */
    ss->lowip = ll;
    ss->highip = uu;
    ss->size = 0;

    /*
     * Initialize the pool's free queue
     */
    queue_init(&ss->freeQ, 0);

    /*
     * Create individual pool elements for each individual IP
     * address.
     */
    while (ll <= uu) {
	/*
	 * check this address in inuseQ
	 * for maybe the pool was deleted while the address
	 *   was still allocated.
	 * Note we already checked for overlap in range, earlier
	 */
	pool_addr = find_localpool_addr_addr(&iplocalp_inuseQ, ll);
	if (pool_addr) {
	    pool_addr->pool = ss;
	} else {
	    pool_addr = malloc_named(sizeof(pooladdr_type),
				     "ippool_add address");
	    if (NULL == pool_addr) {
		/*
		 * This is bad news. We need to flush all added addresses
		 */
		purge_pool_addresses(ss);
		free(ss->name);
		free(ss);
		return FALSE;
	    }
	    pool_addr->user = NULL;
	    pool_addr->ipaddr = ll;
	    pool_addr->pool = ss;
	    p_enqueue(&ss->freeQ, pool_addr);
	}
	ll++;
	ss->size++;
    }

    /*
     * Now add the actual pool structure
     */
    p_enqueue(&poolQ, ss);
    return(TRUE);
}

/*
 * ippool_remove
 *
 * Remove an entire pool
 */
static void ippool_remove (pool_type *ss)
{
    purge_pool_addresses(ss);
    p_unqueue(&poolQ, ss);
    free(ss->name);
    free(ss);
}

/*
 * nmember
 *
 * Return count of number of pool elements in a queue
 */
static int nmember (queuetype *q, pool_type *ss)
{
    int count;
    pooladdr_type *pa;

    count = 0;
    FOR_ALL_IPLOCALPOOL_ADDR(q, pa) {
	if (pa->pool == ss) {
	    count += 1;
	}
    }
    return(count);
}

/*
 * print_pool_state
 *
 * Print out state of a pool nicely
 */
static const char pool_header[] =
    " Pool     Begin           End             Free InUse\n";

static void print_pool_state (pool_type *ss)
{
    printf(" %8s %15i %15i %3d  %3d\n", ss->name, ss->lowip, ss->highip,
					nmember(&ss->freeQ, ss),
					nmember(&iplocalp_inuseQ, ss));
}

/*
 * show_all_pools
 */
static void show_all_pools (void)
{
    pool_type *ss;
    /*
     * Walk the pool list, print the results
     */
    if (QUEUEEMPTY(&poolQ)) {
	printf("\nNo IP Local Pools configured");
	return;
    }
    printf(pool_header);
    FOR_ALL_POOLS(ss) {
	print_pool_state(ss);
    }
}

/*
 * show_ip_localp
 *
 * print a detailed individual pool or else the list
 * of all the pools
 */
static void show_ip_localp (parseinfo *csb)
{
    pool_type *ss;

    if (GETOBJ(string,1) && ('\0' != (GETOBJ(string,1))[0])) {

	ss = find_pool_name(GETOBJ(string,1));
	if (!ss) {
	    printf("\n%%No IP Local Pool named %s", GETOBJ(string,1));
	    return;
	}
	print_ip_localp_pool(ss);
	return;
    }
	show_all_pools();
}

/*
 * mark_pooladdr_used
 *
 * The passed pool address pointer must be in the free list
 */
static void mark_pooladdr_used (pooladdr_type *pa, idbtype *idb,
				char *ustring, char *pool_str)
{
    p_unqueue(&pa->pool->freeQ, pa);
    p_enqueue(&iplocalp_inuseQ, pa);  /* Add in any order */
    pa->idb = idb;
    setstring(&pa->user, ustring);
    idb->ip_peer->ippeer_addr = pa->ipaddr;
    idb->ip_peer->ippeer_addr_source = PEER_ADDR_FROM_POOL;
}

/*
 * mark_pooladdr_free
 *
 * Move IP element to free queue
 */
static void mark_pooladdr_free (pooladdr_type *pa)
{
    p_unqueue(&iplocalp_inuseQ, pa);
    /*
     * check if the pool still exists
     * it might have been deleted, underneath us
     */
    if (pa->pool) {
	p_enqueue(&pa->pool->freeQ, pa);  /* Add in an LRU fashion */
    } else {
	if (pa->user) {
	    free(pa->user);
	    pa->user = NULL;
	}
	free(pa);
    }
}


/*
 * ip_localpool_chown
 *
 * Called to move an address from one interface to another
 */
static void ip_localpool_chown (ipaddrtype ipaddr, idbtype *idb)
{
    pooladdr_type *pa = find_localpool_addr_addr(&iplocalp_inuseQ, ipaddr);

    if (pa)
        pa->idb = idb;
}

/*
 * ip_localpool_free
 *
 * Called to free use of an address
 */
static void ip_localpool_free (ipaddrtype ipaddr, idbtype *idb)
{
    pooladdr_type *pa = 0;

    if (ipaddr) {
	pa = find_localpool_addr_addr(&iplocalp_inuseQ, ipaddr);
    } else if (idb) {
	pa = find_localpool_addr_idb(&iplocalp_inuseQ, idb);
    }
    if (pa) {
	mark_pooladdr_free(pa);
    }
}

/*
 * ip_localpool_allocate
 *
 * attempt to provide a temporary IP address from the
 * local pool.
 * If the caller specifies a particular IP address,
 *     then we only allocate that address if possible.
 * The user string is first preference but not binding.
 *     If no match is found in free list, we will allocate
 *     address.
 * The specified interface pointer are used for
 *     indexing. This and the specified address itself should
 *     not be in the InUse-Q.  Value of 0 is not valid.
 */
static int ip_localpool_allocate (ipaddrtype *newaddr,
				  idbtype *idb, uchar *ustring,
			   uchar *pool_str, ulong wait_ticks)
{
    pooladdr_type *pa = NULL;
    pooladdr_type *old_pa = NULL;
    pool_type *ss;
    ipaddress_type thistype;
    idbtype *dup_idb;		/* for DUP IP address if found */

    /*
     * Cleanup for some checks to come
     */
    if (ustring && !*ustring)
	ustring = NULL;		/* make ptr to empty string a NULL ptr */
    if (!pool_str || !*pool_str)
	pool_str = DEFAULT_LOCAL_POOL_NAME; /* make it ptr to default string */
    ss = find_pool_name(pool_str);

    /*
     * Check for obvious errors
     */
    if (!newaddr || !idb || !ss || !idb->ip_peer)
	return POOL_RET_FAIL;

    /*
     * Do we already have an address?
     */
    if (idb->ip_peer->ippeer_addr_source == PEER_ADDR_FROM_POOL) {
        old_pa = find_localpool_addr_addr(&iplocalp_inuseQ,
					  idb->ip_peer->ippeer_addr);
	/*
	 * We have an address that we will retain for now.  If it is the
	 * address we were looking for, we are done early.  Otherwise, hold
	 * on to it in case the allocation below fails.  Don't free it until
	 * after we look AND find a different address!
	 */
	if (old_pa->pool == ss) {
	    /*
	     * Right pool, is this the address we are asking for?
	     */
	    if (*newaddr == idb->ip_peer->ippeer_addr)
		return POOL_RET_OK;
	    /*
	     * If he didn't have an address in mind, give him this one.
	     */
	    if (!*newaddr && !ustring) {
		*newaddr = idb->ip_peer->ippeer_addr;
		return POOL_RET_OK;
	    }
	}
    }

    /*
     * try to allocate the address if in free list
     * Only a specific ipaddr itself  or a username string
     * should trigger an attempt to re-allocate the previously
     * used address.  Note the  tty/idb  could be same
     * as previous call. So don't assume we want to use these
     * to re-allocate.
     */

    /* Allocate a specific address */
    if (*newaddr) {
	pa = find_localpool_addr_addr(&ss->freeQ, *newaddr);
	if (pa) {
	    /* Well it is certain, free the retained address (if any) */
	    if (old_pa)
		mark_pooladdr_free(old_pa);
	    mark_pooladdr_used(pa, idb, ustring, pool_str);
	    return POOL_RET_OK;
	}
    }

    /*
     * Allocate any available address with a preference for the last 
     * address given to this user.
     */
    if (ustring)
	/*
	 * Try to find this user's last address
	 */
	pa = find_localpool_addr_ustring(&ss->freeQ, ustring);

    /*
     * If that didn't work....
     */
    if (!pa) {
        /*
         * Next preference is given to an address already owned if it
	 * is from the right pool.  We get here if the user string was
	 * non-NULL and/or if the user had an address in mind and this
	 * wasn't it.
         */
        if (old_pa && (old_pa->pool == ss)) {
	    /*
	     * Well then re-use so we don't keep changing addresses
	     */
	    *newaddr = idb->ip_peer->ippeer_addr;
	    return POOL_RET_OK;
        }

	/*
	 * No better address, so just pick the first available address 
	 */
	pa = (pooladdr_type *)ss->freeQ.qhead;

    }

    if (!pa)
	return POOL_RET_FAIL;

    /*
     * Ensure current IP address is NOT in the IP global hash table
     * where all IP address users store their IP addresses in as
     * defined as one of ipaddress_type(i.e: INTERFACE, ALIAS, etc..).
     * This check is done here, so that we ensure only unique IP
     * address wiil be allocated for the requesting peer.
     */
    thistype = ip_ouraddress_type(pa->ipaddr);
    if ((thistype != IPADDR_UNKNOWN)) {
	dup_idb = ipaddr_findidb(pa->ipaddr);
	printf ("\n%%IP address %i is being used by %s.", pa->ipaddr,
		dup_idb->namestring);
	return POOL_RET_FAIL;
    }

    /*
     * free the retained address (if any) as we have finally decided
     * that we don't need it.
     */
    if (old_pa)
	mark_pooladdr_free(old_pa);

    mark_pooladdr_used(pa, idb, ustring, pool_str);
    *newaddr = pa->ipaddr;
    return POOL_RET_OK;
}

/*
 * print_ip_localp_pool
 *
 * Print all addresses and their status in a pool
 */
static void print_ip_localp_pool (pool_type *ss)
{
    pooladdr_type *pool_addr;
    int i;

    printf(pool_header);
    print_pool_state(ss);
    printf("Available addresses:\n");
    i = 0;
    FOR_ALL_IPLOCALPOOL_ADDR(&ss->freeQ, pool_addr) {
	print_ip_localpool_addr(pool_addr);
	i++;
    }
    if (!i) {
	printf("     None\n");
    }
    i = 0;
    printf("\nInuse addresses:\n");
    FOR_ALL_IPLOCALPOOL_ADDR(&iplocalp_inuseQ, pool_addr) {
	if (pool_addr->pool == ss) {
	    print_ip_localpool_addr(pool_addr);
	    i++;
	}
    }
    if (!i) {
	printf("     None\n");
    }
    printf("\n");
}

/*
 * print_ip_localpool_addr
 *
 * Print an IP pool element
 */
static void print_ip_localpool_addr (pooladdr_type *pool_addr)
{
    printf("   %17i    %15s      %s\n", pool_addr->ipaddr,
	   (pool_addr->idb)?(pool_addr->idb->short_namestring):"",
	   (pool_addr->user)?(pool_addr->user):"");
}

/*
 * find_pool_name
 *
 * Given user string, return pool
 */
static pool_type *find_pool_name (char *name)
{
    pool_type *ss;

    FOR_ALL_POOLS(ss) {
	if (!strcmp(ss->name, name))
	    return ss;
    }
    return NULL;
}

/* Registry interface */
static boolean
ip_pool_exists (char *name)
{
    return(find_pool_name(name) != NULL);
}

/*
 * ip_localpool_shutdown
 */
static void ip_localpool_shutdown (void)
{
}

/*
 * ip_localpool_startup
 */
static void ip_localpool_startup (void)
{
}

/*
 * Parse chains for IP local pool subcommand
 */
#define	ALTERNATE	NONE
#include "cfg_ip_localpool.h"
LINK_POINT(ip_localp_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for IP local pool subcommand
 */
#define	ALTERNATE	NONE
#include "cfg_ip_local_pool.h"
LINK_POINT(ip_local_pool_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for SHOW IP local pool commands
 */
#define	ALTERNATE	NONE
#include "exec_show_ip_localpool.h"
LINK_POINT(iplocalp_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for SHOW IP local pool commands
 */
#define	ALTERNATE	NONE
#include "exec_show_ip_local_pool.h"
LINK_POINT(ip_local_pool_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registration array for IP Local Pool UI
 */
static parser_extension_request ip_localpool_chain_init_table[] = {
    { PARSE_ADD_CFG_RTG_IP_CMD, &pname(ip_localp_commands) },
    { PARSE_ADD_SHOW_IP_CMD, &pname(iplocalp_show_commands) },
    { PARSE_ADD_CFG_IP_LOCAL_CMD, &pname(ip_local_pool_commands) },
    { PARSE_ADD_SHOW_IP_LOCAL_CMD, &pname(ip_local_pool_show_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * ip_localpool_init
 *
 * Set up IP local pool structures, register parse chains
 */
static void ip_localpool_init (subsystype *subsys)
{
    queue_init(&poolQ, 0);
    queue_init(&iplocalp_inuseQ, 0);
    parser_add_command_list(ip_localpool_chain_init_table, "IP Local pool");
    reg_invoke_ip_add_pooling_mechanism(&ippool_pool);
    reg_add_ip_pool_exists(ip_pool_exists, "ip_pool_exists");
}

/*
 * IP LOCAL POOL subsystem header
 */
#define IP_LOCALPOOL_MAJVERSION 1
#define IP_LOCALPOOL_MINVERSION 0
#define IP_LOCALPOOL_EDITVERSION  1

SUBSYS_HEADER(ip_localpool_sys,
	      IP_LOCALPOOL_MAJVERSION, IP_LOCALPOOL_MINVERSION,
	      IP_LOCALPOOL_EDITVERSION,
	      ip_localpool_init, SUBSYS_CLASS_LIBRARY,
	      "seq: ip_addrpool_sys",
	      "req: ip_addrpool_sys");
