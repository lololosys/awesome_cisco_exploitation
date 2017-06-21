/* $Id: clns_filter.c,v 3.2.60.3 1996/08/08 14:50:16 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_filter.c,v $
 *------------------------------------------------------------------
 * clns_filter.c--Support for CLNS address filters
 *
 * July 1992, Dave Katz
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_filter.c,v $
 * Revision 3.2.60.3  1996/08/08  14:50:16  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.2.60.2  1996/08/07  08:57:01  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:02:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:39:55  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:40:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:52:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/18  19:39:36  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:17:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../clns/clns.h"
#include "../os/filter.h"
#include "../clns/clns_filter.h"
#include "../clns/clns_externs.h"
#include "../os/filter_externs.h"
#include "../iprouting/route_map.h"

/*
 * Filter name cache manipulation routines
 */
static clns_filtername_cache *name_cache;

/*
 * Get a name given a filter object.  Returns the name, or NULL.
 */
static char *get_name_from_filter (
    void *object,
    clns_filtername_cache_type type)
{
    clns_filtername_cache *ptr = name_cache;

    while (ptr) {
	if (ptr->nametype == type) { /* Right type */
	    switch (type) {
	    case addr_name:
		if (!bcmp(object, &ptr->object.addr, sizeof(nsap_addr)))
		    return(ptr->name);
		break;
		
	    case prefix_name:
		if (!bcmp(object, &ptr->object.pfx, sizeof(nsap_pfx)))
		    return(ptr->name);
		break;
		
	    case masked_name:
		if (!bcmp(object, &ptr->object.masked, sizeof(masked_nsap)))
		    return(ptr->name);
		break;
	    }
	}
	ptr = ptr->next;
    }
    return(NULL);
}


/*
 * Get a filter object given a name.  Returns a copy of the object and the 
 * type, or NULL.
 */
static void *get_filter_from_name (
    char *name,
    clns_filtername_cache_type *type)
{
    clns_filtername_cache *ptr = name_cache;
    void *object;

    while (ptr) {
	if (!strcasecmp(ptr->name, name)) {
	    *type = ptr->nametype;
	    object = malloc(sizeof(ptr->object));
	    if (!object)	/* Quelle dommage, aucune de la memoire */
		return(NULL);
	    bcopy((void *) &ptr->object.addr, object, sizeof(ptr->object));
	    return(object);
	}
	ptr = ptr->next;
    }
    return(NULL);
}

/*
 * Add a name to the name cache.  Replaces any existing name.
 */
static void update_name_cache (
    char *name,
    void *object,
    clns_filtername_cache_type type)
{
    clns_filtername_cache *ptr = name_cache;

    while (ptr) {
	if (!strcasecmp(ptr->name, name)) {	/* Found it */
	    goto write_entry;
	}
	ptr = ptr->next;
    }

    /* Didn't find it.  Allocate a new entry and link it in. */

    ptr = malloc(sizeof(clns_filtername_cache));
    if (!ptr)			/* No memory, tant pis. */
	return;
    ptr->name = strdup(name);
    if (!ptr->name) {		/* Still no memory */
	free(ptr);
	return;
    }
    ptr->next = name_cache;
    name_cache = ptr;

  write_entry:
    bcopy(object, &ptr->object.addr, sizeof(ptr->object));
    ptr->nametype = type;
}


/*
 * Delete a name from the name cache.
 */
static void delete_name_cache (char *name)
{
    clns_filtername_cache *ptr = name_cache;
    clns_filtername_cache **previous = &name_cache;
    while (ptr) {
	if (!strcasecmp(ptr->name, name)) {	/* Found it */
	    *previous = ptr->next; /* Delink it */
	    free(ptr->name);
	    free(ptr);
	    return;
	}
	previous = &ptr->next;
	ptr = ptr->next;
    }
}


/*
 * Command processor for filter object name aliasing
 *
 * clns template-alias name expression
 * no clns template-alias name
 *
 * OBJ(string,1) = name
 * OBJ(string,2) = expression
 */
void clns_template_alias_cmd (parseinfo *csb)
{

    clns_filtername_cache *ptr;
    char *expression_string;
    int template_type;
    void *object;

    /* Do NVGEN processing.  Crank out all of the objects. */

    if (csb->nvgen) {

	ptr = name_cache;
	while (ptr) {
	    switch (ptr->nametype) {
	    case addr_name:
		expression_string = nsap_addr_string(&ptr->object.addr);
		break;

	    case prefix_name:
		expression_string = nsap_pfx_string(&ptr->object.pfx, TRUE);
		break;

	    case masked_name:
		expression_string = masked_nsap_string(&ptr->object.masked);
		break;

	    default:
		expression_string = "";
		break;
	    }
	    nv_write(TRUE, "%s %s %s", csb->nv_command, ptr->name, 
		     expression_string);
	    ptr = ptr->next;
	}

    } else if (!csb->sense) {

	/* "No" form of the command.  Trash the entry. */

	delete_name_cache(GETOBJ(string,1));

    } else {

	/* It's the positive form.  Parse the template. */
	
	template_type = get_nsap_structure(GETOBJ(string,2), &object, 
					   NULL, TRUE);
	switch (template_type) {
	case NSAP_STRUCT_BAD:
	    printf("\n%%CLNS: Invalid NSAP template");
	    return;
	case NSAP_STRUCT_NOMEM:
	    printf("\n%%CLNS: Out of memory");
	    return;
	}

	update_name_cache(GETOBJ(string,1), object, template_type);
    }
}

/*
 * Fasthash--generate a fast CLNS hash by XORing full words and then
 * XORing the bytes.  It breaks even with a brute force hash at
 * 4 bytes.  Somebody clever could do better yet, I'm sure.
 */

static const unsigned long mask[] = {
    0x0,
    0xff000000,
    0xffff0000,
    0xffffff00};

static ulong clns_fasthash (nsap_addr *nsap)
{
    int bytesleft;
    ulong hashword = 0;
    ulong *lptr = (unsigned long *) &(nsap->addr[0]);
    ulong hashbyte;

    /* Do the longwords. */

    for (bytesleft = nsap->length; bytesleft >= 4; bytesleft -=4) {
	hashword ^= GETLONG(lptr++);
    }

    /*
     * If there's stray bytes left, extract them and include them in
     * the longword hash.
     */
    if (bytesleft) {
	hashword ^= GETLONG(lptr) & mask[bytesleft];
    }

    /*
     * Now crunch the longword down into a byte.
     */
    hashbyte = hashword & 0xff;
    hashword >>= 8;
    hashbyte ^= hashword & 0xff;
    hashword >>= 8;
    hashbyte ^= hashword & 0xff;
    hashword >>= 8;
    hashbyte ^= hashword & 0xff;

    return(hashbyte);
}

/*
 * Generic NSAP address comparison.  Returns TRUE if they are equal.
 * Go faster by comparing from the right;  the addresses are more likely
 * to differ at the end.
 */
static boolean nsap_addr_compare (
    nsap_addr *nsap1,
    nsap_addr *nsap2)
{
    int bytecount;
    uchar *ptr1;
    uchar *ptr2;
    if ((bytecount = nsap1->length) != nsap2->length)
	return(FALSE);

    ptr1 = nsap1->addr + bytecount;
    ptr2 = nsap2->addr + bytecount;

    while (bytecount--) {
	if (*(--ptr1) != *(--ptr2))
	    return(FALSE);
    }
    return(TRUE);
}


/*
 * Run through the filter set, calling a routine for each entry
 * in the filter set.
 *
 * The routines should be defined as follows:
 *   boolean fast_routine(clns_filter_fast *);
 *   boolean slow_routine(clns_filter_slow *);
 *
 * The routine should return TRUE to continue, or FALSE to stop.
 */

static void process_filter_set_aux (
    clns_filter_set *set,
    boolean (*fast_routine)(clns_filter_fast *),
    boolean (*slow_routine)(clns_filter_slow *))
{
    int hashvalue;
    clns_filter_fast **curfast;
    clns_filter_fast *fastptr;
    clns_filter_fast *fastnext;
    clns_filter_slow *slowptr;
    clns_filter_slow *slownext;

    if (!set)
	return;

    /* First walk each chain in the fast list. */

    for (hashvalue = 0, curfast = set->fastmatch;  hashvalue < CLNS_CACHE_SIZE;
	 hashvalue++, curfast++) {
	fastptr = *curfast;
	while (fastptr) {
	    fastnext = fastptr->next;
	    if (!(*fast_routine)(fastptr))
		return;
	    fastptr = fastnext;
	}
    }

    /* Now walk the slow chain. */

    slowptr = set->slowmatch;
    while (slowptr) {
	slownext = slowptr->next;
	if (!(*slow_routine)(slowptr))
	    return;
	slowptr = slownext;
    }
}


/*
 * NVGEN routines for filter-sets
 */

static char *nvgen_setname;	/* Name to generate */
static char *nvgen_setcmd;	/* Command to generate */

/* NVGEN a fast filter. */

static boolean nvgen_fast (clns_filter_fast *fastptr)
{
     nv_write(TRUE, "%s %s %s %s", nvgen_setcmd, nvgen_setname,
	     fastptr->grant ? "permit":"deny",
	     nsap_addr_string(&fastptr->address));
    return(TRUE);
}

/* NVGEN a slow filter. */

static boolean nvgen_slow (clns_filter_slow *slowptr)
{
    if (slowptr->filtertype == prefix) {
	nv_write(TRUE, "%s %s %s %s", nvgen_setcmd, nvgen_setname,
		 slowptr->grant ? "permit":"deny",
		 nsap_pfx_string(&slowptr->filterptr.pfx, TRUE));
    } else {
	nv_write(TRUE, "%s %s %s %s", nvgen_setcmd, nvgen_setname,
		 slowptr->grant ? "permit":"deny",
		 masked_nsap_string(&slowptr->filterptr.masked));
    }
    return(TRUE);
}


/*
 * Free a fast or slow filter.  We cheat and don't care which type.
 */
static inline boolean free_filter (void *ptr)
{
    free(ptr);
    return(TRUE);
}

/*
 * Free a fast filter.  Do this to hide the type cheating.
 */
static boolean free_fast_filter (clns_filter_fast *ptr)
{
    return(free_filter(ptr));
}

/*
 * Free a slow filter.  Do this to hide the type cheating.
 */
static boolean free_slow_filter (clns_filter_slow *ptr)
{
    return(free_filter(ptr));
}

/*
 * Process the command for setting up a filter set:
 *
 *   clns filter-set sname template
 *   no clns filter-set sname
 *
 *   OBJ(string,1) = sname
 *   OBJ(string,2) = template
 *   OBJ(int,1) = grant status
 */
void clns_filter_set_cmd (parseinfo *csb)
{
    clns_filtername_cache_type template_type;
    void *ptr;
    ulong index;
    int hashvalue;
    filter_object *fltrptr;
    clns_filter_set *setptr;
    clns_filter_slow *slowptr;
    clns_filter_fast *fastptr;
    clns_filter_slow **curslow;
    clns_filter_fast **curfast;

    /* Check for "no". */

    if (!csb->sense) {

	/* Get the referenced object, if any. */

	index = get_filter_index(GETOBJ(string,1));
	if (!index) {
	    printf("\n%%CLNS: No such set");
	    return;
	}
	fltrptr = filter_obj_ptr(index);
	if (fltrptr->protocol != FILTER_PROTO_CLNS) {
	    printf("\n%%CLNS: Not a CLNS filter");
	    return;
	}
	if (fltrptr->type != FILTER_LEAF) {
	    printf("\n%%CLNS: Not a filter set");
	    return;
	}

	/* Unlink ourselves from anything that might reference us. */

	delete_filter_pointers(index);

	/* Detach the filter set from the filter object. */

	setptr = (clns_filter_set *) fltrptr->filptr.generic;
	fltrptr->filptr.generic = NULL;

	/* Run through it, freeing everything. */

	if (setptr) {
	    process_filter_set_aux(setptr, free_fast_filter, free_slow_filter);
	    free(setptr);
	}

	/* Attempt to free the object. */

	clean_up_filter_objects();
	CONSISTENCY_CHECK();
	DUMP_TREE();

    } else if (csb->nvgen) {

	/*
	 * NVGEN.  Walk the list of filter objects.  Pick those that are
	 * filter sets.
	 */

	nvgen_setcmd = csb->nv_command;

	FOR_ALL_FILTERS(index, fltrptr) {
    
	    /* Skip those with no name. */

	    if (!fltrptr->name)
		continue;

	    /* Skip those that aren't filter sets. */

	    if (fltrptr->type != FILTER_LEAF)
		continue;

	    /* Skip non-CLNS leaves. */

	    if (fltrptr->protocol != FILTER_PROTO_CLNS)
		continue;

	    /* Skip placeholders. */

	    setptr = (clns_filter_set *) fltrptr->filptr.generic;
	    if (!setptr)
		continue;

	    /* Got a live one.  Format and print its pieces. */

	    nvgen_setname = fltrptr->name;
	    process_filter_set_aux(setptr, nvgen_fast, nvgen_slow);
	}
		
    } else {

	/* It's the real thing.  Check to see if the name is too long. */

	if (strlen(GETOBJ(string,1)) > MAX_FILTER_NAME_LENGTH) {
	    printf("\n%%CLNS: Filter name too long");
	    return;
	}

	/* It parsed OK.  Get a filter object. */

	index = create_filter_object(GETOBJ(string,1));
	if (!index) {	/* Out of memory */
	nomem:
	    printf("\n%%CLNS: Out of memory");
	    goto cleanup;
	}
	fltrptr = filter_obj_ptr(index);

	/*
	 * If the filter object has no type yet, assign it.  If it has
	 * a conflicting type and it's not a placeholder, it's an error.
	 */
	if (fltrptr->type == FILTER_UNDEF) {
	    fltrptr->type = FILTER_LEAF;
	    fltrptr->protocol = FILTER_PROTO_CLNS;
	} else if (fltrptr->type != FILTER_LEAF) {
	    if (fltrptr->filptr.expr) {
		printf("\n%%CLNS: Conflicting filter object name");
		goto cleanup;
	    } else {
		fltrptr->type = FILTER_LEAF; /* Force a type change. */
	    }
	}
	if (fltrptr->protocol != FILTER_PROTO_CLNS) {
	    printf("\n%%CLNS: Conflicting protocol type");
	    goto cleanup;
	}

	/* Parse the template.  First try the name. */
	
	ptr = get_filter_from_name(GETOBJ(string,2), &template_type);
	if (!ptr) {		/* Not a valid name */
	    template_type = get_nsap_structure(GETOBJ(string,2), &ptr, 
					       NULL, TRUE);
	    switch (template_type) {
	    case NSAP_STRUCT_BAD:
		printf("\n%%CLNS: Invalid NSAP template");
		goto cleanup;
	    case NSAP_STRUCT_NOMEM:
		goto nomem;
	    default:
		break;
	    }
	    
	}
	
	setptr = (clns_filter_set *) fltrptr->filptr.generic;
	
	/*
	 * If there is no memory associated with this object, allocate
	 * it now.
	 */
	if (!setptr) {
	    setptr = malloc(sizeof(clns_filter_set));
	    fltrptr->filptr.generic = (generic_leaf *) setptr;
	    if (!setptr) {	/* No memory! */
	    free_nomem:
		free(ptr);
		goto nomem;
	    }
	    setptr->hdr.index = index;
	}
	
	/*
	 * Now handle the different types of templates, and create the
	 * appropriate objects.
	 */
	
	switch (template_type) {
	    
	case NSAP_STRUCT_MASKED:
	case NSAP_STRUCT_PFX:
	    
	    /* Masked or prefix.  Allocate a slow filter and link it in. */
	    
	    slowptr = malloc(sizeof(clns_filter_slow));
	    if (!slowptr) {
		goto free_nomem;
	    }
	    
	    if (template_type == NSAP_STRUCT_MASKED) {
		bcopy(ptr, (void *) &(slowptr->filterptr.masked),
		      sizeof(masked_nsap));
		slowptr->filtertype = masked;
	    } else {
		bcopy(ptr, (void *) &(slowptr->filterptr.pfx),
		      sizeof(nsap_pfx));
		slowptr->filtertype = prefix;
	    }
	    slowptr->grant = GETOBJ(int,1);
	    
	    curslow = &setptr->slowmatch;
	    while (*curslow) {
		
		/* Check for a duplicate, and discard if it is. */
		
		if (slowptr->filtertype == (*curslow)->filtertype &&
		    slowptr->grant == (*curslow)->grant &&
		    !bcmp((void *) &(slowptr->filterptr),
			  (void *) &((*curslow)->filterptr),
			  sizeof(slowptr->filterptr))) {
		    free(slowptr);
		    slowptr = NULL;
		    break;
		}
		curslow = &((*curslow)->next);
	    }
	    if (slowptr) 
		*curslow = slowptr;
	    break;
	    
	case NSAP_STRUCT_ADDR:
	    
	    /* Address.  Allocate a fast filter and link it in. */
	    
	    fastptr = malloc(sizeof(clns_filter_fast));
	    if (!fastptr) {
		goto free_nomem;
	    }
	    
	    bcopy(ptr, (void *) &(fastptr->address), sizeof(nsap_addr));
	    fastptr->grant = GETOBJ(int,1);
	    
	    hashvalue = clns_fasthash(&(fastptr->address));
	    curfast = &(setptr->fastmatch[hashvalue]);
	    while (*curfast) {
		
		/* Check for a duplicate, and discard if it is. */
		
		if (fastptr->grant == (*curfast)->grant &&
		    !bcmp((void *) &(fastptr->address), 
			  (void *) &((*curfast)->address), 
			  sizeof(fastptr->address))) {
		    free(fastptr);
		    fastptr = NULL;
		    break;
		}
		curfast = &((*curfast)->next);
	    }
	    if (fastptr)
		*curfast = fastptr;
	    break;
	}
	free(ptr);		/* Don't need the address structure. */

	/* Now update any objects that might have already referenced us. */

	update_filter_pointers(index);
	DUMP_TREE();
    }
    CONSISTENCY_CHECK();
    return;

 cleanup:
    clean_up_filter_objects();
    CONSISTENCY_CHECK();
}



/*
 * Show a fast filter.
 */
static boolean show_fast (clns_filter_fast *fastptr)
{
    char *name;
    name = clns_address_lookup(nsap_addr_to_NSAP_T(&fastptr->address));
    if (!name)
	name = get_name_from_filter(&fastptr->address, addr_name);

    printf("\n  %s %s", fastptr->grant ? "permit":"deny  ",
	   name ? name : nsap_addr_string(&fastptr->address));
    return(TRUE);
}

/*
 * Show a slow filter.
 */
static boolean show_slow (clns_filter_slow *slowptr)
{
    char *name;
    switch (slowptr->filtertype) {
    case prefix:
	name = get_name_from_filter(&slowptr->filterptr.pfx, 
				    slowptr->filtertype);
	if (!name)
	    name = nsap_pfx_string(&slowptr->filterptr.pfx, TRUE);
	break;

    case masked:
	name = get_name_from_filter(&slowptr->filterptr.masked,
				    slowptr->filtertype);
	if (!name)
	    name = masked_nsap_string(&slowptr->filterptr.masked);
	break;

    default:
	name = "";
	break;
    }
    
    printf("\n  %s %s", slowptr->grant ? "permit":"deny  ", name);
    return(TRUE);
}
    
/*
 * Show CLNS filter sets.
 */
void clns_show_filter_set (char *name, boolean name_present)
{
    ulong first, last;
    filter_object *ptr;
    ulong i;

    /* If there are no filter objects, don't bother. */

    if (filter_objects_count == 0)
	return;

    /* If a name is present, ensure it exists. */

    if (name_present) {
	first = last = get_filter_index(name);
	ptr = filter_obj_ptr(first);
	if (!first || !ptr->filptr.generic) {
	    printf("\n%%CLNS: No such object");
	    return;
	}
	if (ptr->type != FILTER_LEAF) {
	    printf("\n%%CLNS: Object is not a filter set");
	    return;
	}
	if (ptr->protocol != FILTER_PROTO_CLNS) {
	    printf("\n%%CLNS: Conflicting protocol type");
	    return;
	}
    } else {
	first = 1;
	last = filter_objects_count - 1;
	ptr = filter_obj_ptr(first);
    }

    /* Run the specified range. */

    automore_enable(NULL);
    for (i=first; i<=last; i++, ptr++) {
	if (!ptr->name)		/* Unused */
	    continue;
	if (ptr->type != FILTER_LEAF) /* Not a set */
	    continue;
	if (ptr->protocol != FILTER_PROTO_CLNS) /* Not CLNS */
	    continue;
	if (!ptr->filptr.generic)	/* Placeholder */
	    continue;
	printf("\nCLNS filter set %s", ptr->name);
	process_filter_set_aux((clns_filter_set *) ptr->filptr.generic, 
			       show_fast, show_slow);
    }
    automore_disable();
}

/*
 * Create a CLNS filter on behalf of some command.
 */
ulong clns_filter_create (char *name, ulong oldindex)
{
    ulong index;
    filter_object *obj;

    /* Remove any old filter reference. */

    unreference_filter(oldindex);

    /* Create the object. */

    index = create_filter_object(name);
    if (!index) {
	printf("%%CLNS: Out of memory");
	return(index);
    } 
    
    /* If it's new, set the protocol ID.  If not, check it. */
    
    obj = filter_obj_ptr(index);
    switch (obj->protocol) {
    case FILTER_NO_PROTO:
	obj->protocol = FILTER_PROTO_CLNS;
	break;
    case FILTER_PROTO_CLNS:
	break;
    default:
	printf("\n%%CLNS: Conflicting protocol type");
	index = 0;
	goto cleanup;
    }
    reference_filter(index);
 cleanup:
    clean_up_filter_objects();
    CONSISTENCY_CHECK();
    DUMP_TREE();
    return(index);
}

/*
 * Destroy a filter on behalf of a command.
 */
void clns_filter_destroy (ulong index)
{
    if (index == 0)
	return;
    unreference_filter(index);
    clean_up_filter_objects();
    CONSISTENCY_CHECK();
    DUMP_TREE();
}

/*
 * Process the "clns access-group" command.
 */
void clns_set_access_group (
    idbtype *idb,
    uchar *name,
    boolean in)
{

    ulong *filterptr = in ? &idb->clns_filter_in : &idb->clns_filter_out;

    /* If the "no" version, destroy any existing filter. */

    if (!name) {
	clns_filter_destroy(*filterptr);
	*filterptr = 0;
    } else {			/* Create the new filter. */
	*filterptr = clns_filter_create(name, *filterptr);
    }
}

/*
 * Process the "clns adjacency-filter" command
 */
void clns_set_adj_filter(idbtype *idb, uchar *name, boolean is)
{
    ulong *filterptr = is ? &idb->clns_is_adj_filter : 
	&idb->clns_es_adj_filter;

    if (name) {			/* Positive */
	*filterptr = clns_filter_create(name, *filterptr);
    } else {			/* Negative */
	clns_filter_destroy(*filterptr);
	*filterptr = 0;
    }
}

static const uchar bytemask[8] = {0x0, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe};

/*
 * Execute a filter set.
 */
static boolean clns_execute_filter_set (
    generic_leaf *gen_leaf,
    filter_type type,
    void *parm1,
    void *parm2)
{
    nsap_addr *address;
    clns_filter_fast *fastptr;
    clns_filter_slow *slowptr;
    nsap_pfx *pfx;
    int prefixbytes;
    int bits;
    uchar *pfxptr;
    uchar *addrptr;
    uchar *maskptr;
    uchar *fltrptr;
    int bytecount;
    int leftbytelength;
    masked_nsap *mskd;
    int addrlen;
    clns_filter_set *leaf = (clns_filter_set *) gen_leaf;
    nsap_addr *destaddr = parm1;
    nsap_addr *srcaddr = parm2;

    /* Pick the address based on whether we want source or destination. */

    address = (type == FILTER_TERM_DST) ? destaddr : srcaddr;

    if (clns_filter_debug) {
	filter_object *fptr = filter_obj_ptr(leaf->hdr.index);
	if (fptr) {
	    buginf("\nCLNS: Filtering %s (%s) through %s", 
		   nsap_addr_string(address), 
		   type == FILTER_TERM_DST ? "dest" : "src",
		   fptr->name);
	} else {
	    buginf("\nCLNS: Invalid filter index");
	}
    }

    /* First try the fast filters. */
    
    fastptr = leaf->fastmatch[clns_fasthash(address)];
    while (fastptr) {
	if (nsap_addr_compare(address, &(fastptr->address))) {
	    clns_fbuginf("\nCLNS: Matched fast filter %s (%s)",
			 nsap_addr_string(&fastptr->address),
			 fastptr->grant ? "permit" : "deny");
	    return(fastptr->grant);
	}
	fastptr = fastptr->next;
    }
    
    /* No luck.  Try the slow filters. */
    
    slowptr = leaf->slowmatch;
    while (slowptr) {
	
	/* Prefixes and masked NSAPs are handled differently. */
	
	switch (slowptr->filtertype) {
	    
	case prefix:
	    
	    /* Prefix.  No match if the address is too short. */
	    
	    pfx = &(slowptr->filterptr.pfx);
	    if (address->length * 8 < pfx->length)
		goto nextone;
	    
	    /* Walk the bytes first.  Go backwards. */
	    
	    bytecount = prefixbytes = pfx->length / 8;
	    pfxptr = pfx->prefix + prefixbytes;
	    addrptr = address->addr + prefixbytes;
	    while (bytecount--) {
		if (*(--pfxptr) != *(--addrptr))
		    goto nextone;
	    }
	    
	    /* The bytes match.  Mask off the last little bit. */
	    
	    bits = pfx->length % 8;
	    if (!bits) {		/* It matched! */
		clns_fbuginf("\nCLNS: matched prefix %s (%s)",
			     nsap_pfx_string(pfx, TRUE),
			     slowptr->grant ? "permit" : "deny");
		return(slowptr->grant);
	    }
	    
	    if ((address->addr[prefixbytes] & bytemask[bits]) == 
		pfx->prefix[prefixbytes]) { /* It matched! */
		clns_fbuginf("\nCLNS: matched prefix %s (%s)",
			     nsap_pfx_string(pfx, TRUE),
			     slowptr->grant ? "permit" : "deny");
		return(slowptr->grant);
	    }
	    
	    /* No match.  Go around. */
	    
	    goto nextone;
		
	
	case masked:
	    
	    /* Masked NSAP. */
	    
	    addrlen = address->length;
	    mskd = &(slowptr->filterptr.masked);
	    leftbytelength = (mskd->leftlength + 7) / 8;
	    
	    /* Try again if exact match and the lengths don't match. */
	    
	    if (mskd->exact && (mskd->leftlength != (addrlen * 8)))
		goto nextone;
	    
	    /* Try again if the lengths just don't make sense. */
	    
	    if (addrlen < leftbytelength || addrlen < mskd->rightlength)
		goto nextone;
	    
	    /*  Do it differently if a mask is needed. */
	    
	    if (mskd->applymask) {
		
		/* Do the right side first, backwards of course. */
		
		if (mskd->rightlength) {
		    
		    addrptr = address->addr + addrlen;
		    maskptr = mskd->mask + leftbytelength + mskd->rightlength;
		    fltrptr = mskd->addr + leftbytelength + mskd->rightlength;
		    bytecount = mskd->rightlength;
		    
		    while (bytecount--) {
			if ((*(--addrptr) & *(--maskptr)) != *(--fltrptr))
			    goto nextone;
		    }
		}
		
		/* So far, so good.  Now try the left side. */
		
		if (leftbytelength) {
		    addrptr = address->addr + leftbytelength;
		    maskptr = mskd->mask + leftbytelength;
		    fltrptr = mskd->addr + leftbytelength;
		    bytecount = leftbytelength;
		    
		    while (bytecount--) {
			if ((*(--addrptr) & *(--maskptr)) != *(--fltrptr))
			    goto nextone;
		    }
		}
		
		/* Got a match. */
		
		clns_fbuginf("\nCLNS: matched masked NSAP %s (%s)",
			     masked_nsap_string(mskd),
			     slowptr->grant ? "permit" : "deny");
		return(slowptr->grant);
		
	    } else {
		
		/* No mask needed.  Same code, slightly optimized. */
		
		/* Do the right side first, backwards of course. */
		
		if (mskd->rightlength) {
		    addrptr = address->addr + address->length;
		    fltrptr = mskd->addr + leftbytelength + mskd->rightlength;
		    bytecount = mskd->rightlength;
		    
		    while (bytecount--) {
			if (*(--addrptr) != *(--fltrptr))
			    goto nextone;
		    }
		}
		
		/* So far, so good.  Now try the left side. */
		
		if (leftbytelength) {
		    addrptr = address->addr + leftbytelength;
		    fltrptr = mskd->addr + leftbytelength;
		    bytecount = leftbytelength;
		    
		    while (bytecount--) {
			if (*(--addrptr) != *(--fltrptr))
			    goto nextone;
		    }
		}
	    }
	    
	    /* Got a match. */
	    
	    clns_fbuginf("\nCLNS: matched masked NSAP %s (%s)",
			 masked_nsap_string(mskd),
			 slowptr->grant ? "permit" : "deny");
	    return(slowptr->grant);
	    break;
	}
    nextone:
	slowptr = slowptr->next;
    }
    
    /* No luck at all.  Imply a "deny". */
    
    clns_fbuginf("\nCLNS: No match");
    return(FALSE);
}


/*
 * Filter an address pair through a filter expression.
 *
 * Returns TRUE if the address should be permitted, or FALSE if not.
 *
 * The source address field can be NULL for applications in which it is
 * inappropriate.
 */
boolean clns_filter_addr (
    ulong index,
    nsap_addr *destaddr,
    nsap_addr *srcaddr)
{
    filter_object *obj = filter_obj_ptr(index);
    boolean result = TRUE;

    /* Proceed if the object exists and is not a placeholder. */

    if (obj) {
	if (obj->filptr.generic) {

	    /* Handle the set and expression cases separately. */
	    
	    switch (obj->type) {

	    case FILTER_LEAF:
		result = clns_execute_filter_set(obj->filptr.generic,
						 FILTER_TERM_DST,
						 destaddr, 
						 srcaddr);
		break;

	    case FILTER_EXPR:
		result = filter_recursive_walk_execute(obj->filptr.expr, 
						clns_execute_filter_set, 
						destaddr, 
						srcaddr);
		break;

	    default:
		break;
	    }
	}
    }
    return(result);
}

/*
 * clns_accesscheck
 * Perform access list check.
 * Return TRUE to send datagram, FALSE otherwise.
 * The 'list' parameter is really a pointer to a string, but it is declared
 * as an int to fit the registry definition.
 */
boolean clns_accesscheck (paktype *pak, int list)
{
    ulong index;

    index = get_filter_index((char *)list);
    if (!index) {
	return(TRUE);
    }

    return(clns_filter_addr(index, (nsap_addr *) PAK2ADDR(pak), /* Dest addr */
				   (nsap_addr *) pak->clns_srcaddr));
}

/*
 * route_map_clns_matches_supported
 *
 * Return the bit mask of the matches supported for a given routing
 * protocol.
 */
ulong route_map_clns_matches_supported (ulong typeflag)
{
    if (typeflag & ISO_STATIC) {
	return(ROUTE_MAP_CLNS_STATIC_MATCHES);
    }
    if (typeflag & ISO_ISISMASK) {
	return(ROUTE_MAP_ISIS_MATCHES);
    }
    if (typeflag & ISO_IGRPMASK) {
	return(ROUTE_MAP_ISO_IGRP_MATCHES);
    }
    return(0);
}

/*
 * route_map_match_clnsaddress
 * 
 * Match address with OSI filter-set entry.
 */
boolean route_map_match_clnsaddress (nsap_addr *prefix,
				     route_map_match_value *match_list)
{
    route_map_match_value *value;

    /*
     * Nothing in list, therefore it matches.
     */
    if (!match_list) {
	return(TRUE);
    }

    /*
     * If any of the values in the list match, return TRUE.
     */
    for (value = match_list; value; value = value->next) {
	if (clns_filter_addr(value->value, prefix, NULL)) {
	    return(TRUE);
	}
    }
    return(FALSE);
}


/*
 * route_map_clns_check
 *
 * Apply route_map specified by caller to contents of an clns_ddbtype.
 * The caller passes in match flags indicating which matches it supports.  
 * This function returns TRUE, if all match criteria are met for a 
 * given permit route-map. It returns FALSE, for a deny route-map. When all 
 * route-map entries are processed and a match has not been satisfied, the 
 * funtion returns FALSE.
 */
route_maptype *route_map_clns_check (uchar *map_name, clns_ddbtype *ddb,
				     clns_rdbtype *rdb)
{
    route_maptype      *map;
    route_map_headtype *head;
    ulong              matches_supported;
    
    head = find_route_map_head(map_name);
    if (!head) {
	return(NULL);
    }

    matches_supported = 
	route_map_clns_matches_supported(rdb->protocol_source);

    for (map = head->map_queue.qhead; map; map = map->next) {

	/*
	 * Match CLNS prefix against OSI filter-set.
	 */
	if (matches_supported & ROUTE_MAP_MATCH_CLNSADDRESS) {
	    if (!route_map_match_clnsaddress((nsap_addr *) ddb->destination,
			   map->match_list.clns_address)) {
		continue;
	    }
	}

	/*
	 * Match next-hop against OSI filter-set.
	 */
	if (matches_supported & ROUTE_MAP_MATCH_CLNSNEXTHOP) {
	    if (!route_map_match_clnsaddress((nsap_addr *) rdb->first_hop,
			     map->match_list.clns_next_hop)) {
		continue;
	    }
	}

	/*
	 * Match info source against OSI filter-set.
	 */
	if (matches_supported & ROUTE_MAP_MATCH_CLNSROUTESOURCE) {
	    if (!route_map_match_clnsaddress((nsap_addr *) rdb->info_source,
		     map->match_list.clns_route_source)) {
		continue;
	    }
	}

	/*
	 * Match ddb metric against match list.
	 */
	if (matches_supported & ROUTE_MAP_MATCH_METRIC) {
	    if (!route_map_match_number(ddb->metric,
			   map->match_list.metric)) {
		continue;
	    }
	}

	/*
	 * Match rdb interface against match list.
	 */
	if (matches_supported & ROUTE_MAP_MATCH_INTERFACE) {
	    if (!route_map_match_number((ulong) rdb->idb,
 	       (route_map_match_value *) map->match_list.interface)) {
		continue;
	    }
	}

	/*
	 * If no map was found with full matches, or one was found with a
	 * deny action, return FALSE.
	 */
	if (!map->grant) {
	    map = NULL;
	}
	break;
    }
    return(map);
}

