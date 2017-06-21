/* $Id: clns_filter.h,v 3.2 1995/11/17 08:52:07 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/clns_filter.h,v $
 *------------------------------------------------------------------
 * clns_filter.h - Structures used in OSI access/filter lists
 *
 * July 1992, Dave Katz
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
  *------------------------------------------------------------------
 * $Log: clns_filter.h,v $
 * Revision 3.2  1995/11/17  08:52:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:17:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* A fast (full address) match filter. */

typedef struct clns_filter_fast_ {
    struct clns_filter_fast_ *next; /* Next entry in this hash value */
    nsap_addr address;		/* Address to match */
    boolean grant;		/* True if PERMIT */
} clns_filter_fast;


/* A slow (prefix or masked) match filter. */

typedef struct clns_filter_slow_ {
    struct clns_filter_slow_ *next; /* Next entry in list of filters */
    union {
	nsap_pfx pfx;		/* An NSAP Prefix */
	masked_nsap masked;	/* A masked NSAP */
    } filterptr;
    enum {prefix = NSAP_STRUCT_PFX, masked = NSAP_STRUCT_MASKED} 
        filtertype;		/* Prefix or masked NSAP */
    boolean grant;		/* True if PERMIT */
} clns_filter_slow;


/*
 * Filter set--the most granular filter available, essentially the same as
 * an IP access list.  It consists of a hashed set of linked lists of
 * fast filters (containing full NSAP addresses), followed by a linked
 * list of slow filters (containing prefixes and masked NSAPs).
 *
 * An address is compared to a filter by first checking for a matched
 * complete address, and if no match is found, a linear search of the
 * slow list is done.  The first match is taken, and the value of "grant"
 * is returned as the results of the filter.
 */

typedef struct clns_filter_set_ {
    filter_leaf_hdr hdr;
    clns_filter_fast *fastmatch[CLNS_CACHE_SIZE]; /* Hashed full addresses */
    clns_filter_slow *slowmatch; /* List of slow matches */
} clns_filter_set;

/*
 * Filter name cache--a brute-force list of names and address expressions
 *
 * This holds only prefixes and masked NSAPs; straight NSAP addresss are
 * held in the usual name cache.
 */

typedef enum clns_filtername_cache_type_ {addr_name = NSAP_STRUCT_ADDR, 
					      prefix_name = NSAP_STRUCT_PFX, 
					      masked_name = NSAP_STRUCT_MASKED}
    clns_filtername_cache_type;

typedef struct clns_filtername_cache_ {
    struct clns_filtername_cache_ *next; /* Next entry */
    char *name;			/* The name of the object */
    clns_filtername_cache_type nametype; /* The type */
    union {
	nsap_addr addr;		/* An NSAP address */
	nsap_pfx pfx;		/* An NSAP Prefix */
	masked_nsap masked;	/* A masked NSAP */
    } object;
} clns_filtername_cache;
