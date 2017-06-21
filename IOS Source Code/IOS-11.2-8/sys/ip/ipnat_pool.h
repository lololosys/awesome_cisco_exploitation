/* $Id: ipnat_pool.h,v 3.1.6.4 1996/06/03 23:23:19 ahh Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipnat_pool.h,v $
 *------------------------------------------------------------------
 * IP Network Address Translation address pooling
 *
 * November 1995, Andy Heffernan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipnat_pool.h,v $
 * Revision 3.1.6.4  1996/06/03  23:23:19  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.1.6.3  1996/05/13  23:21:09  ahh
 * CSCdi56426:  NAT: EFT bugs and feedback
 * Fix hash algorithm, fix address block coalescing, reduce up-front
 * memory allocation, drop untranslatable packets, clean up debugs.
 * Branch: California_branch
 *
 * Revision 3.1.6.2  1996/04/24  00:06:35  ahh
 * CSCdi55483:  NAT: code review comments, various bug fixes
 * Fast-path fixup, cruft removal.
 * Branch: California_branch
 *
 * Revision 3.1.6.1  1996/04/16  18:53:25  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/07  05:59:45  ahh
 * Placeholder...
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IP_NAT_POOL_H__
#define __IP_NAT_POOL_H__

#define IPNAT_GENERIC_CHUNK_MAX		200

/*
 * Even though all these symbols have "ipnat" in them,
 * these functions are really quite generic.
 */

STRUCTURE_REF(ipnat_addrpool);
STRUCTURE_REF(ipnat_addrpool_range);

enum ipnat_addrpool_type {
    IPNAT_POOL_UNKNOWN,
    IPNAT_POOL_GENERIC,
    IPNAT_POOL_COUNTING,
    IPNAT_POOL_ROTARY,
    IPNAT_POOL_MATCHHOST
};

struct ipnat_addrpool_generic_block {
    ipaddrtype start, end;
    struct {
	unsigned allocated : 1;
	unsigned refcount : 31;
    } info;
    struct ipnat_addrpool_generic_block *next;
};

struct ipnat_addrpool_generic {
    struct ipnat_addrpool_generic_block *blocks;
};

struct ipnat_addrpool_rotary {
    int next_index;
    ipnat_addrpool_range *current_range;
};

struct ipnat_addrpool_ {
    char *name;				/* name of the pool */
    enum ipnat_addrpool_type type;	/* type of pool */

    list_header address_list;
    ipaddrtype mask;			/* netmask */
    int prefix_len;

    int size;				/* raw size */
    ulong allocated;			/* number of addresses allocated */
    ulong misses;			/* number of failed allocations */

    /* functions */
    ipaddrtype (*alloc)(ipnat_addrpool *, ipaddrtype);
    boolean (*free)(ipnat_addrpool *, ipaddrtype);
    boolean (*inc_refcount)(ipnat_addrpool *, ipaddrtype);
    ulong (*refcount)(ipnat_addrpool *, ipaddrtype);

    list_element element;		/* queueing on ipnat_addrpool_list */

    union {
	struct ipnat_addrpool_generic generic;
	struct ipnat_addrpool_rotary rotary;
    } u;
};

struct ipnat_addrpool_range_ {
    ipaddrtype start, end;		/* start and end */
    list_element element;		/* queueing on addresses */
};


/*
 * Prototypes
 */

extern ipaddrtype ipnat_addrpool_alloc(char *, ipaddrtype);
extern boolean ipnat_addrpool_exists(char *);
extern void ipnat_addrpool_free(char *, ipaddrtype);
extern boolean ipnat_addrpool_bump(char *, ipaddrtype);
extern boolean ipnat_addrpool_inuse(char *);
extern ulong ipnat_addrpool_refcount(char *, ipaddrtype);
extern enum ipnat_addrpool_type ipnat_addrpool_pooltype(char *);
extern void ipnat_addrpool_init(void);
extern void ipnat_pool_dump_stats(char *);
extern void ipnat_pool_command(parseinfo *);
extern void ipnat_pool_parser_init(void);

#endif /* __IP_NAT_POOL_H__ */
