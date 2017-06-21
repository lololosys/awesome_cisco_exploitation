/* $Id: ipnat_pool.c,v 3.1.6.7 1996/09/05 23:06:28 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipnat_pool.c,v $
 *------------------------------------------------------------------
 * IP Network Address Translation pooling
 *
 * November 1995, Andy Heffernan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipnat_pool.c,v $
 * Revision 3.1.6.7  1996/09/05  23:06:28  snyder
 * CSCdi68126:  declare some stuff in ip const
 *              228 out of data, only 4 from image
 * Branch: California_branch
 *
 * Revision 3.1.6.6  1996/08/13  21:27:41  ahh
 * CSCdi65510:  NAT: tftp does not work when overloading global addresses
 * Deal with the server sending packets back from a new port number.
 * Branch: California_branch
 *
 * Revision 3.1.6.5  1996/08/12  16:03:07  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.6.4  1996/06/03  23:23:18  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.1.6.3  1996/05/13  23:21:07  ahh
 * CSCdi56426:  NAT: EFT bugs and feedback
 * Fix hash algorithm, fix address block coalescing, reduce up-front
 * memory allocation, drop untranslatable packets, clean up debugs.
 * Branch: California_branch
 *
 * Revision 3.1.6.2  1996/04/24  00:06:32  ahh
 * CSCdi55483:  NAT: code review comments, various bug fixes
 * Fast-path fixup, cruft removal.
 * Branch: California_branch
 *
 * Revision 3.1.6.1  1996/04/16  18:53:24  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/07  05:59:41  ahh
 * Placeholder...
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "globs.h"
#include "types.h"
#include "../ui/debug.h"
#include "ip_debug.h"
#include "config.h"
#include "parser.h"
#include "../os/chunk.h"
#include "ip.h"
#include "parser_defs_ipnat.h"
#include "ipnat_pool.h"

static list_header ipnat_addrpool_list;
static chunk_type *generic_chunks;

static char *const pooltypenames[] = {
    "unknown", "generic", "counting", "rotary", "match-host"
};

#define csb_pool	userDataBlock[0]

static parser_mode *ipnat_pool_config_mode;

/*
 * Generic functions
 */

/*
 * ipnat_addrpool_generic_alloc
 *
 * Allocate an address from a generic pool.
 */

static boolean
ipnat_addrpool_generic_alloc_address (ipnat_addrpool *pool,
	       struct ipnat_addrpool_generic_block *block,
	       struct ipnat_addrpool_generic_block **lastp,
	       ipaddrtype address)
{
    struct ipnat_addrpool_generic_block *allocblock, *newblock;

    if (block->start == block->end) {
	/* Simple case, just mark this block as allocated */
	block->info.allocated = TRUE;
	block->info.refcount++;
    } else {
	/* Create a block for the allocated address. */
	allocblock = chunk_malloc(generic_chunks);
	if (allocblock == NULL)
	    return(FALSE);	/* Can't allocate. */
	allocblock->start = allocblock->end = address;
	allocblock->info.allocated = TRUE;
	allocblock->info.refcount++;
	if (address == block->start) {
	    /* Link it before this one. */
	    block->start++;
	    if (*lastp)
		(*lastp)->next = allocblock;
	    else
		pool->u.generic.blocks = allocblock;
	    allocblock->next = block;
	} else if (address == block->end) {
	    /* Link it after this one. */
	    block->end--;
	    allocblock->next = block->next;
	    block->next = allocblock;
	} else {
	    /* Urgh, must split the existing block around it. */
	    newblock = chunk_malloc(generic_chunks);
	    if (newblock == NULL) {
		chunk_free(generic_chunks, allocblock);
		return(FALSE);	/* Can't allocate. */
	    }
	    newblock->start = block->start;
	    newblock->end = allocblock->start - 1;
	    block->start = allocblock->end + 1;
	    allocblock->next = block;
	    newblock->next = allocblock;
	    if (*lastp)
		(*lastp)->next = newblock;
	    else
		pool->u.generic.blocks = newblock;
	}
	*lastp = allocblock;
    }
    return(TRUE);
}

static ipaddrtype
ipnat_addrpool_generic_alloc (ipnat_addrpool *pool, ipaddrtype old_address)
{
    struct ipnat_addrpool_generic_block *block, *last;
    ipaddrtype address;
    ulong old_host;

    last = NULL;
    old_host = 0L; 

    /*
     * Find the host number if we are doing host matching.
     */
    if (pool->type == IPNAT_POOL_MATCHHOST)
	old_host = (ulong)(old_address & ~pool->mask); 

    for (block = pool->u.generic.blocks; block; block = block->next) {
	if (!block->info.allocated) {
	    for (address = block->start; address <= block->end; address++) {
		if (pool->type == IPNAT_POOL_MATCHHOST) {
		    /* Only allocate an address with matching host number. */
		    if (((ulong)(address & ~pool->mask)) != old_host)
			continue;
		}

                /* Now see if it's worth returning. */
                if (good_address(address, pool->mask, TRUE)) { 
                    if (ipnat_addrpool_generic_alloc_address(
                                pool, block, &last, address)) {
                        return(address);
                    }

                    /* Can't allocate */
                    return(IPADDR_ZERO);
                }
	    }
	}
	last = block;
    }
    return(IPADDR_ZERO);
}

/*
 * ipnat_addrpool_generic_inc
 *
 * Increment the refcount of an address in a generic pool.
 */

static boolean
ipnat_addrpool_generic_inc (ipnat_addrpool *pool, ipaddrtype address)
{
    struct ipnat_addrpool_generic_block *block, *last;

    last = NULL;
    for (block = pool->u.generic.blocks; block; block = block->next) {
	if (block->start <= address && address <= block->end) {
	    if (block->start == address && block->end == address &&
		block->info.allocated) {
		block->info.refcount++;
	    } else {
		if (!ipnat_addrpool_generic_alloc_address(pool, block,
							  &last, address))
		    return(FALSE);
		pool->allocated++;
	    }
	    return(TRUE);
	}
	last = block;
    }
    return(TRUE);
}

/*
 * ipnat_addrpool_generic_refcount
 *
 * Return the refcount of an address in a generic pool.
 */

static ulong
ipnat_addrpool_generic_refcount (ipnat_addrpool *pool, ipaddrtype address)
{
    struct ipnat_addrpool_generic_block *block;

    for (block = pool->u.generic.blocks; block; block = block->next) {
	if (block->start <= address && address <= block->end) {
	    if (block->start == address && block->end == address &&
		block->info.allocated) {
		return(block->info.refcount);
	    } else {
		return(0);
	    }
	}
    }
    return(0);
}

/*
 * ipnat_addrpool_generic_free
 *
 * Free an address back to a generic pool.
 */

static boolean
ipnat_addrpool_generic_free (ipnat_addrpool *pool, ipaddrtype address)
{
    struct ipnat_addrpool_generic_block *block, *last, *next;

    last = NULL;
    for (block = pool->u.generic.blocks; block; block = block->next) {
	if (block->info.allocated && block->start == address) {
	    /*
	     * Found the right block for this address, decrement its
	     * reference count.  If this is the last reference, try to
	     * coalesce the block with adjacent blocks, discarding the
	     * redundant blocks.
	     */
	    block->info.refcount--;
	    if (block->info.refcount == 0) {
		block->info.allocated = FALSE;
		/* Coalesce with previous block. */
		if (last && !last->info.allocated &&
		    (last->end == address - 1)) {
		    last->next = block->next;
		    last->end = address;
		    chunk_free(generic_chunks, block);
		    block = last;
		}
		/* Coalesce with next block. */
		next = block->next;
		if (next && !next->info.allocated &&
		    (next->start == address + 1)) {
		    block->end = next->end;
		    block->next = next->next;
		    chunk_free(generic_chunks, next);
		}
		return(TRUE);
	    }
	    return(FALSE);
	}
	last = block;
    }

    return(TRUE);
}

/*
 * Rotary functions
 */

/*
 * ipnat_addrpool_rotary_alloc
 *
 * Allocate an address from a rotary pool.
 */

static ipaddrtype
ipnat_addrpool_rotary_alloc (ipnat_addrpool *pool, ipaddrtype old_address)
{
    ipaddrtype address;
    int first_index, trips;
    ipnat_addrpool_range *range;

    first_index = pool->u.rotary.next_index;
    range = pool->u.rotary.current_range;
    if (range == NULL)
	return(IPADDR_ZERO);

    trips = 0;
    while (1) {
	/* Safety belt in case the entire pool is bogus. */
	if (first_index == pool->u.rotary.next_index) {
	    if (trips >= 1)
		return(IPADDR_ZERO);
	    trips++;
	}
	address = range->start + pool->u.rotary.next_index;
	if (address > range->end) {
	    range = LIST_NEXT_DATA(&range->element);
	    if (range == NULL)
		range = LIST_HEAD_DATA(&pool->address_list);
	    pool->u.rotary.current_range = range;
	    pool->u.rotary.next_index = 0;
	    address = range->start;
	}
	pool->u.rotary.next_index++;
	if (good_address(address, pool->mask, TRUE)) {
	    return(address);
	}
    }
}

/*
 * ipnat_addrpool_rotary_free
 *
 * Free an address back to a rotary pool.
 */

static boolean
ipnat_addrpool_rotary_free (ipnat_addrpool *pool, ipaddrtype address)
{
    /* Nothing to do. */
    return(TRUE);
}


/*
 * ipnat_addrpool_find
 *
 * Find an address pool given its name.
 */

static ipnat_addrpool *
ipnat_addrpool_find (char *name)
{
    ipnat_addrpool *pool;
    list_element *el;

    FOR_ALL_DATA_IN_LIST(&ipnat_addrpool_list, el, pool) {
	if (strcmp(pool->name, name) == 0)
	    return(pool);
    }
    return(NULL);
}

/*
 * ipnat_addrpool_sanity_check
 *
 * Do some simple sanity checks on start and end addresses
 * for a semi-configured pool.  Checks for overlaps, subnet
 * crossing, etc.
 */

static boolean
ipnat_addrpool_sanity_check (ipnat_addrpool *pool, ipaddrtype start,
			     ipaddrtype end, boolean verbose)
{
    ipaddrtype start_net, end_net;
    ipnat_addrpool_range *range;
    list_element *el;

    /*
     * Sanity check start and end.
     */
    if (start > end) {
	if (verbose)
	    printf("\n%%End address less than start address");
	return(FALSE);
    }

    start_net = start & pool->mask;
    end_net = end & pool->mask;
    if (start_net != end_net) {
	if (verbose)
	    printf("\n%%Start and end addresses on different subnets");
	return(FALSE);
    }

    FOR_ALL_DATA_IN_LIST(&pool->address_list, el, range) {
	if (start == range->start && end == range->end)
	    return(FALSE);

	if ((start >= range->start && start <= range->end) ||
	    (end >= range->start && end <= range->end)) {
	    if (verbose)
		printf("\n%%Start and end addresses overlap with existing"
		       " range");
	    return(FALSE);
	}
    }

    return(TRUE);
}

/*
 * ipnat_addrpool_new_range
 *
 * Create a new range of addresses for an address pool
 */

static ipnat_addrpool_range *
ipnat_addrpool_new_range (ipnat_addrpool *pool, ipaddrtype start,
			  ipaddrtype end, boolean verbose)
{
    ipnat_addrpool_range *range;

    if (!ipnat_addrpool_sanity_check(pool, start, end, TRUE))
	return(NULL);
    range = malloc(sizeof(ipnat_addrpool_range));
    if (range == NULL) {
	if (verbose)
	    printf(nomemory);
	return(NULL);
    }
    range->start = start;
    range->end = end;
    return(range);
}

/*
 * ipnat_addrpool_add_range
 *
 * Add a new range of addresses to a pool.
 */

static boolean
addrsort (void *vaddr1, void *vaddr2)
{
    ipaddrtype *addr1, *addr2;

    addr1 = vaddr1;
    addr2 = vaddr2;
    return(*addr1 - *addr2);
}

static void
ipnat_addrpool_add_range (ipnat_addrpool *pool, ipnat_addrpool_range *range)
{
    list_insert(&pool->address_list, &range->element, range, addrsort);
}

/*
 * ipnat_addrpool_remove_range
 *
 * Remove a range from a pool.
 */

static void
ipnat_addrpool_remove_range (ipnat_addrpool *pool, ipnat_addrpool_range *range)
{
    list_remove(&pool->address_list, &range->element, NULL);
}

/*
 * ipnat_addrpool_find_range
 *
 * Find range of addresses given a start and end address.
 */

static ipnat_addrpool_range *
ipnat_addrpool_find_range (ipnat_addrpool *pool, ipaddrtype start,
			   ipaddrtype end)
{
    list_element *el;
    ipnat_addrpool_range *range;

    FOR_ALL_DATA_IN_LIST(&pool->address_list, el, range) {
	if (range->start == start && range->end == end) {
	    return(range);
	}
    }

    return(NULL);
}

/*
 * ipnat_addrpool_new_block
 *
 * Create a new address block.
 */

struct ipnat_addrpool_generic_block *
ipnat_addrpool_new_block (ipaddrtype start, ipaddrtype end)
{
    struct ipnat_addrpool_generic_block *block;

    block = chunk_malloc(generic_chunks);
    if (block == NULL) {
	return(NULL);
    }
    block->start = start;
    block->end = end;

    return(block);
}

/*
 * ipnat_addrpool_add_block
 *
 * Add an address block.
 */

static void
ipnat_addrpool_add_block (ipnat_addrpool *pool,
			  struct ipnat_addrpool_generic_block *block)
{
    struct ipnat_addrpool_generic_block *b, *last;

    if (pool->u.generic.blocks) {
	last = NULL;
	for (b = pool->u.generic.blocks; b; b = b->next) {
	    if (b->start > block->start) {
		/* Block belongs in the middle. */
		if (last)
		    last->next = block;
		else
		    pool->u.generic.blocks = block;
		block->next = b;
		return;
	    }
	    last = b;
	}
	/* Block belongs at the end. */
	last->next = block;
    } else {
	/* Block belongs at the beginning. */
	pool->u.generic.blocks = block;
    }
}

/*
 * ipnat_addrpool_remove_block
 *
 * Remove an address block.
 */

static void
ipnat_addrpool_remove_block (ipnat_addrpool *pool, ipaddrtype start,
			     ipaddrtype end)
{
    struct ipnat_addrpool_generic_block *b, *last;

    last = NULL;
    for (b = pool->u.generic.blocks; b; b = b->next) {
	if (b->start == start && b->end == end) {
	    if (last)
		last->next = b->next;
	    else
		pool->u.generic.blocks = b->next;
	    break;
	}
	last = b;
    }
}

/*
 * ipnat_addrpool_create
 *
 * Create the address pool structure and link it into the system.
 * If start and end addresses are zero (IPADDR_ZERO), then the pool
 * is incomplete (it should be filled in with further "ip nat pool"
 * subcommands.
 */

static ipnat_addrpool *
ipnat_addrpool_create (char *name, ipaddrtype start, ipaddrtype end,
		       ipaddrtype mask, int prefix_len,
		       boolean verbose, enum ipnat_addrpool_type type)
{
    ipnat_addrpool *pool;
    ipnat_addrpool_range *range;
    struct ipnat_addrpool_generic_block *block;
    int i;
    ipaddrtype xor_bits, tempmask;

    /*
     * Convert the prefix_len to a netmask.
     */
    if (prefix_len)
	mask = ip_bitcount2mask(prefix_len);

    /*
     * Sanity check the netmask.
     */
    if (start != IPADDR_ZERO && end != IPADDR_ZERO) {
	/*
	 * Find where the addresses differ to figure out what the
	 * minimum mask needs to be, but stop comparing at bit 1
	 * since a prefix of 31 or 32 bits doesn't make sense.
	 */
	xor_bits = start ^ end;
	for (i = 31; i > 1; i--) {
	    if (xor_bits & (1 << i))
		break;
	}

	/*
	 * Compare the minimum mask needed with the supplied mask
	 * and complain if the supplied mask is too small.  Allow
	 * the configuration for now, though, so that existing
	 * configurations aren't completely blown out of the water.
	 */
	tempmask = 0xffffffff << (i + 1);
	if (~tempmask > ~mask) {
	    if (verbose) {
		if (prefix_len) {
		    for (i = 0; i < 32; i++) {
			if (tempmask & 1)
			    break;
			tempmask >>= 1;
		    }
		    printf("\n%%Pool %s prefix length %d too large;"
			   " should be no more than %d", name, prefix_len,
			   32 - i);
		} else
		    printf("\n%%Pool %s mask %i too small; should be"
			   " at least %i", name, mask, tempmask);
	    }
	}
    }

    pool = malloc(sizeof(ipnat_addrpool));
    if (pool == NULL) {
	if (verbose)
	    printf(nomemory);
	return(NULL);
    }

    setstring(&pool->name, name);
    if (pool->name == NULL) {
	if (verbose)
	    printf(nomemory);
	free(pool);
	return(NULL);
    }
    pool->mask = mask;
    pool->prefix_len = prefix_len;
    pool->type = type;
    list_create(&pool->address_list, 0, "IP NAT Address Pool Addresses", 0);

    if (start != IPADDR_ZERO && end != IPADDR_ZERO) {
	range = ipnat_addrpool_new_range(pool, start, end, verbose);
	if (range == NULL) {
	    list_destroy(&pool->address_list);
	    free(pool);
	    return(NULL);
	}
	ipnat_addrpool_add_range(pool, range);
	pool->size = end - start + 1;
    } else {
	range = NULL;
    }

    /*
     * Set up the functions
     */
    switch (type) {
    case IPNAT_POOL_MATCHHOST:
    case IPNAT_POOL_GENERIC:
	pool->alloc = ipnat_addrpool_generic_alloc;
	pool->free = ipnat_addrpool_generic_free;
	pool->inc_refcount = ipnat_addrpool_generic_inc;
	pool->refcount = ipnat_addrpool_generic_refcount;
	if (start != IPADDR_ZERO && end != IPADDR_ZERO) {
	    block = ipnat_addrpool_new_block(start, end);
	    if (block == NULL) {
		if (verbose)
		    printf(nomemory);
		if (range) {
		    ipnat_addrpool_remove_range(pool, range);
		    free(range);
		}
		list_destroy(&pool->address_list);
		free(pool);
		return(NULL);
	    }

	    ipnat_addrpool_add_block(pool, block);
	}
	break;

    case IPNAT_POOL_ROTARY:
	pool->alloc = ipnat_addrpool_rotary_alloc;
	pool->free = ipnat_addrpool_rotary_free;
	pool->u.rotary.next_index = 0;
	pool->u.rotary.current_range = LIST_HEAD_DATA(&pool->address_list);
	pool->inc_refcount = NULL;
	pool->refcount = NULL;
	break;

    default:
	if (verbose)
	    printf("\n%%ipnat_addrpool_create: illegal pool type %d", type);
	if (range) {
	    ipnat_addrpool_remove_range(pool, range);
	    free(range);
	}
	list_destroy(&pool->address_list);
	free(pool);
	return(NULL);
    }
    
    /*
     * Put this pool on the master list.
     */
    list_enqueue(&ipnat_addrpool_list, &pool->element, pool);
    
    return(pool);
}

/*
 * ipnat_addrpool_destroy
 *
 * Destroy an address pool
 */

static void
ipnat_addrpool_destroy_internal (ipnat_addrpool *pool)
{
    ipnat_addrpool_range *range;
    list_element *el, *next;

    if (pool->allocated > 0) {
	if (ipnat_debug)
	    buginf("\nNAT: pool %s in use", pool->name);
	return;
    }

    /* Remove the pool from the master list if it's there. */
    if (LIST_ELEMENT_QUEUED(&pool->element))
	list_remove(&ipnat_addrpool_list, &pool->element, NULL);

    /* Remove and delete all the address ranges in the pool. */
    FOR_ALL_DATA_IN_LIST_W_NEXT(&pool->address_list, el, next, range) {
	list_remove(&pool->address_list, &range->element, NULL);
	free(range);
    }
    list_destroy(&pool->address_list);

    if (pool->name)
	free(pool->name);
    free(pool);
}

void
ipnat_addrpool_destroy (char *poolname)
{
    ipnat_addrpool *pool;

    pool = ipnat_addrpool_find(poolname);
    if (pool == NULL)
	return;

    ipnat_addrpool_destroy_internal(pool);
}

/*
 * ipnat_addrpool_exists
 *
 * Check if the named address pool exists.
 */

boolean
ipnat_addrpool_exists (char *poolname)
{
    return(ipnat_addrpool_find(poolname) != NULL);
}

/*
 * ipnat_addrpool_alloc
 *
 * Allocate an address from a pool.
 */

ipaddrtype
ipnat_addrpool_alloc (char *poolname, ipaddrtype old_address)
{
    ipnat_addrpool *pool;
    ipaddrtype addr;

    pool = ipnat_addrpool_find(poolname);
    if (pool == NULL) {
	if (ipnat_debug)
	    buginf("\nNAT: alloc -- pool %s not found", poolname);
	return(IPADDR_ZERO);
    }

    addr = (*pool->alloc)(pool, old_address);
    if (addr)
	pool->allocated++;
    else
	pool->misses++;
    return(addr);
}

/*
 * ipnat_addrpool_free
 *
 * Free an address to an address pool.
 */

void
ipnat_addrpool_free (char *poolname, ipaddrtype address)
{
    ipnat_addrpool *pool;
    ipnat_addrpool_range *range;
    boolean bad;
    list_element *el;

    pool = ipnat_addrpool_find(poolname);
    if (pool == NULL) {
	if (ipnat_debug)
	    buginf("\nNAT: free -- pool %s not found", poolname);
	return;
    }

    /* Assume it's OK. */
    bad = FALSE;
    if (martian(address, TRUE)) {
	/* OK, it's not OK. */
	bad = TRUE;
    } else {
	/* Now assume it's not OK. */
	bad = TRUE;
	FOR_ALL_DATA_IN_LIST(&pool->address_list, el, range) {
	    if (address >= range->start && address <= range->end) {
		/* Sheesh, now it's OK, make up your mind. */
		bad = FALSE;
		break;
	    }
	}
    }

    if (bad) {
	if (ipnat_debug)
	    buginf("\nNAT: can't free %i", address);
	return;
    }

    if ((*pool->free)(pool, address)) {
	if (pool->allocated == 0) {
	    if (ipnat_debug)
		buginf("\nNAT: free twice, pool %s address %i!",
		       pool->name, address);
	} else {
	    pool->allocated--;
	}
    }
}

/*
 * ipnat_addrpool_bump
 *
 * Bump the recount of an address (reallocate the address).
 */

boolean
ipnat_addrpool_bump (char *poolname, ipaddrtype address)
{
    ipnat_addrpool *pool;

    pool = ipnat_addrpool_find(poolname);
    if (pool == NULL) {
	return(FALSE);
    }

    if (pool->inc_refcount)
	return((*pool->inc_refcount)(pool, address));

    return(TRUE);
}

/*
 * ipnat_addrpool_inuse
 *
 * Check if an address pool has addresses allocated from it.
 */

boolean
ipnat_addrpool_inuse (char *poolname)
{
    ipnat_addrpool *pool;

    pool = ipnat_addrpool_find(poolname);
    if (pool == NULL) {
	return(FALSE);
    }

    return(pool->allocated > 0);
}

/*
 * ipnat_addrpool_refcount
 *
 * Return the refcount for a given address.
 */

ulong
ipnat_addrpool_refcount (char *poolname, ipaddrtype address)
{
    ipnat_addrpool *pool;

    pool = ipnat_addrpool_find(poolname);
    if (pool == NULL) {
	return(0);
    }

    if (pool->refcount)
	return((*pool->refcount)(pool, address));

    return(0);
}

/*
 * ipnat_addrpool_pooltype
 *
 * Return the type of the named pool.
 */

enum ipnat_addrpool_type
ipnat_addrpool_pooltype (char *poolname)
{
    ipnat_addrpool *pool;

    pool = ipnat_addrpool_find(poolname);
    if (pool == NULL) {
	return(IPNAT_POOL_UNKNOWN);
    }

    return(pool->type);
}

/*
 * ipnat_addrpool_init
 *
 * Initiaialize the NAT address pool code.
 */

void
ipnat_addrpool_init (void)
{
    list_create(&ipnat_addrpool_list, 0, "IP NAT Address Pool", 0);
    generic_chunks = chunk_create(sizeof(struct ipnat_addrpool_generic_block),
				     IPNAT_GENERIC_CHUNK_MAX,
				     CHUNK_FLAGS_DYNAMIC, NULL, 0,
				     "NAT Address Chunks");
}

/*
 * ipnat_pool_dump_stats
 *
 */

void
ipnat_pool_dump_stats (char *poolname)
{
    ipnat_addrpool *pool;
    ipnat_addrpool_range *range;
    list_element *el;

    pool = ipnat_addrpool_find(poolname);
    if (pool == NULL) {
	return;
    }

    printf("\n pool %s: netmask %i", poolname, pool->mask);
    FOR_ALL_DATA_IN_LIST(&pool->address_list, el, range)
	printf("\n\tstart %i end %i", range->start, range->end);
    printf("\n\ttype %s, total addresses %d, allocated %d (%d%%), misses %d",
	   pooltypenames[pool->type], pool->size, pool->allocated,
	   (pool->allocated * 100) / pool->size, pool->misses);
}

/*
 * ipnat_add_pool_cfg
 *
 * Add a complete pool definition to the system.
 */

static boolean
ipnat_add_pool_cfg (parseinfo *csb)
{
    enum ipnat_addrpool_type pool_type;
    ipaddrtype start, end;

    start = GETOBJ(paddr,1)->ip_addr;
    end = GETOBJ(paddr,2)->ip_addr;

    if (start == IPADDR_ZERO && end == IPADDR_ZERO) {
	printf("\n%%Invalid IP addresses");
	return(FALSE);
    }

    pool_type = GETOBJ(int,1);
    if (ipnat_addrpool_inuse(GETOBJ(string,1))) {
	printf("\n%%Pool %s in use, cannot redefine", GETOBJ(string,1));
	return(FALSE);
    }
    ipnat_addrpool_destroy(GETOBJ(string,1));

    if (ipnat_addrpool_create(GETOBJ(string,1), start, end,
			      GETOBJ(paddr,3)->ip_addr, GETOBJ(int,2), TRUE,
			      pool_type) == NULL) {
	return(FALSE);
    }

    return(TRUE);
}

/*
 * ipnat_remove_pool_cfg
 *
 * Remove a pool definition from the system.
 */

static boolean
ipnat_remove_pool_cfg (parseinfo *csb)
{
    if (!ipnat_addrpool_exists(GETOBJ(string,1))) {
	printf("\n%%Pool %s not found", GETOBJ(string,1));
	return(FALSE);
    }

    if (ipnat_addrpool_inuse(GETOBJ(string,1))) {
	printf("\n%%Pool %s in use, cannot destroy", GETOBJ(string,1));
	return(FALSE);
    }
    ipnat_addrpool_destroy(GETOBJ(string,1));
    return(TRUE);
}

/*
 * ipnat_addrpool_nv
 *
 */

static void
ipnat_addrpool_nv (parseinfo *csb)
{
    char *nv_command = NULL;
    ipnat_addrpool *pool;
    ipnat_addrpool_range *range;
    list_element *el;

    setstring(&nv_command, csb->nv_command);
    FOR_ALL_DATA_IN_LIST(&ipnat_addrpool_list, el, pool) {
	if (LIST_EMPTY(&pool->address_list))
	    continue;

	if (LIST_SIZE(&pool->address_list) == 1) {
	    range = LIST_HEAD_DATA(&pool->address_list);
	    nv_write(TRUE, "%s %s %i %i", csb->nv_command, pool->name,
		     range->start, range->end);
	} else {
	    nv_write(TRUE, "%s %s", csb->nv_command, pool->name);
	}

	if (pool->prefix_len)
	    nv_add(TRUE, " prefix-length %d", pool->prefix_len);
	else
	    nv_add(TRUE, " netmask %i", pool->mask);

	switch (pool->type) {
	case IPNAT_POOL_GENERIC:
	    break;
	case IPNAT_POOL_MATCHHOST:
	    nv_add(TRUE, " type match-host");
	    break;
	case IPNAT_POOL_ROTARY:
	    nv_add(TRUE, " type rotary");
	    break;
	default:
	    break;
	}
	
	if (LIST_SIZE(&pool->address_list) > 1) {
	    if (set_mode_byname(&csb->mode, "ipnat-pool", MODE_VERBOSE)) {
		*csb->nv_command = '\0';
		csb->csb_pool = pool;
		recurse_node(csb, get_mode_nv(csb));
		strcpy(csb->nv_command, nv_command);
	    }
	}
    }
    free(nv_command);
}

/*
 * ipnat_pool_command
 *
 * Handle "ip nat pool" configuration.
 */

void
ipnat_pool_command (parseinfo *csb)
{
    char *name;
    ipaddrtype start, end, mask;
    int prefix_len;	
    enum ipnat_addrpool_type type;
    ipnat_addrpool *pool;

    if (csb->nvgen) {
	ipnat_addrpool_nv(csb);
	return;
    }

    name = GETOBJ(string,1);
    start = GETOBJ(paddr,1)->ip_addr;
    end = GETOBJ(paddr,2)->ip_addr;
    mask = GETOBJ(paddr,3)->ip_addr;
    prefix_len = GETOBJ(int,2);
    type = GETOBJ(int,1);

    /*
     * Removing the pool.
     */
    if (!csb->sense) {
	ipnat_remove_pool_cfg(csb);
	return;
    }

    /*
     * All on one line config.
     */
    if (start != IPADDR_ZERO && end != IPADDR_ZERO) {
	ipnat_add_pool_cfg(csb);
	return;
    }

    /*
     * Sub-mode configuration.
     */
    pool = ipnat_addrpool_find(name);
    if (pool == NULL) {
	pool = ipnat_addrpool_create(name, IPADDR_ZERO, IPADDR_ZERO,
				     mask, prefix_len, TRUE, type);
	if (pool == NULL)
	    return;
    } else {
	if (ipnat_addrpool_inuse(name)) {
	    printf("\n%%Pool %s in use, cannot redefine", name);
	    return;
	}
	if (mask != pool->mask || prefix_len != pool->prefix_len) {
	    if (prefix_len)
		mask = ip_bitcount2mask(prefix_len);
	    pool->mask = mask;
	    pool->prefix_len = prefix_len;
	}
	if (type != pool->type) {
	    /*
	     * XXX Arggh, fix this.
	     * Need to blow away support structures and recreate
	     * based on existing address ranges.
	     */
	    printf("\n%%Cannot redefine pool type, please delete and"
		   " recreate.");
	    return;
	}
    }
    csb->csb_pool = pool;
    set_mode_byname(&csb->mode, "ipnat-pool", MODE_VERBOSE);
}

/*
 * ipnat_pool_subcommand
 *
 * Do parsing and NV-gen for "ip nat pool" subcommands.
 */

static void
ipnat_pool_subcommand (parseinfo *csb)
{
    ipnat_addrpool *pool;
    ipnat_addrpool_range *range;
    ipaddrtype start, end;
    list_element *el;

    pool = csb->csb_pool;
    if (pool == NULL) {
	/* "shouldn't happen" */
	return;
    }

    if (csb->nvgen) {
	switch (csb->which) {
	case IPNAT_POOL_ADDRESS:
	    FOR_ALL_DATA_IN_LIST(&pool->address_list, el, range)
		nv_write(TRUE, "%s %i %i", csb->nv_command, range->start,
			 range->end);
	    break;

	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
    case IPNAT_POOL_ADDRESS:
	start = GETOBJ(paddr,1)->ip_addr;
	end = GETOBJ(paddr,2)->ip_addr;
	if (csb->sense) {
	    struct ipnat_addrpool_generic_block *block;

	    /*
	     * Create the new range and add it in.
	     */
	    range = ipnat_addrpool_new_range(pool, start, end, TRUE);
	    if (range == NULL)
		return;
	    ipnat_addrpool_add_range(pool, range);

	    /*
	     * Do any per-type fixups.
	     */ 
	    switch (pool->type) {
	    case IPNAT_POOL_GENERIC:
		block = ipnat_addrpool_new_block(start, end);
		if (block == NULL) {
		    printf(nomemory);
		    if (range) {
			ipnat_addrpool_remove_range(pool, range);
			free(range);
		    }
		    list_destroy(&pool->address_list);
		    free(pool);
		}
		ipnat_addrpool_add_block(pool, block);
		break;

	    case IPNAT_POOL_ROTARY:
		if (pool->u.rotary.current_range == NULL)
		    pool->u.rotary.current_range =
			LIST_HEAD_DATA(&pool->address_list);
		break;

	    default:
		break;
	    }
	    pool->size += end - start + 1;
	} else {
	    /*
	     * Find the range.
	     */
	    range = ipnat_addrpool_find_range(pool, start, end);

	    /*
	     * Do any per-type fixups.
	     */
	    switch (pool->type) {
	    case IPNAT_POOL_GENERIC:
		ipnat_addrpool_remove_block(pool, start, end);
		break;

	    case IPNAT_POOL_ROTARY:
		if (pool->u.rotary.current_range == range)
		    pool->u.rotary.current_range =
			LIST_NEXT_DATA(&range->element);
		break;

	    default:
		break;
	    }

	    ipnat_addrpool_remove_range(pool, range);
	    free(range);
	    pool->size -= end - start + 1;

	    /*
	     * If nothing left, destroy the pool and get out
	     * of the submode.
	     */
	    if (LIST_EMPTY(&pool->address_list)) {
		ipnat_addrpool_destroy_internal(pool);
		exit_config_submode(csb);
	    }
	}
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

#include "../parser/macros.h"
#include "../parser/parser_actions.h"
/************************************************************************
 */

#define ALTERNATE	NONE

EOLNS	(cip_nat_pool_exit_eol, exit_config_submode);
KEYWORD	(cip_nat_pool_exit_kwd, cip_nat_pool_exit_eol, NONE,
	 "exit", "Exit from IP NAT pool configuration mode", PRIV_MIN);
TESTVAR	(cip_nat_pool_exit, NONE, cip_nat_pool_exit_kwd, NONE,
	 NONE, ALTERNATE, sense, FALSE);

EOLS    (cip_nat_pool_addr_eol, ipnat_pool_subcommand, IPNAT_POOL_ADDRESS);
IPADDR	(cip_nat_pool_addr_end, cip_nat_pool_addr_eol, NONE,
	 OBJ(paddr,2), "End IP address");
IPADDR	(cip_nat_pool_addr_start, cip_nat_pool_addr_end, NONE,
	 OBJ(paddr,1), "Start IP address");
NVGENS	(cip_nat_pool_addr_nvgen, cip_nat_pool_addr_start,
	 ipnat_pool_subcommand, IPNAT_POOL_ADDRESS);
KEYWORD (cip_nat_pool_address, cip_nat_pool_addr_nvgen, cip_nat_pool_exit,
	 "address", 
	 "Configure a range of addresses", PRIV_CONF);

KEYWORD_ID(cip_nat_pool_no, cip_nat_pool_address, cip_nat_pool_address,
	   sense, FALSE,
	   "no", "Negate or set default values of a command",
	   PRIV_MIN | PRIV_NONVGEN);

HELP	(top_conf_ipnat_pool, cip_nat_pool_no,
	 "IP NAT pool configuration commands:\n");

/*
 * ipnat_pool_parser_init
 *
 */

void
ipnat_pool_parser_init (void)
{
    ipnat_pool_config_mode = parser_add_mode("ipnat-pool", "config-ipnat-pool",
					     "IP NAT pool configuration mode",
					     TRUE, FALSE, "configure", NULL,
					     NULL, &pname(top_conf_ipnat_pool),
					     NULL);
}
