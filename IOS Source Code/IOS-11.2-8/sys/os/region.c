/* $Id: region.c,v 3.2.62.1 1996/03/18 21:31:05 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/os/region.c,v $
 *------------------------------------------------------------------
 * region.c - Region Manager support
 *
 * March 1995, Scott Mackie
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: region.c,v $
 * Revision 3.2.62.1  1996/03/18  21:31:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:19:28  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  16:41:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:50:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/21  19:16:42  hampton
 * Make sure that text segment checksumming gets turned off for all
 * platforms where the text segment is read-only.  [CSCdi35788]
 *
 * Revision 2.1  1995/06/07 21:57:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "region.h"
#include "sum.h"


/*
 * Local storage
 */
static list_header regionlist;


/*
 * region_init
 *
 * Initialize the internal region manager structures
 *
 */

void region_init (void)
{
    list_create(&regionlist, 0, "Region List", 0);
}


/*
 * region_create
 *
 * Fill in the information in a region block
 */

regiontype *region_create (regiontype   *region,
			   char         *name,
			   void         *start,
			   uint          size,
			   region_class  class,
			   uint          flags)
{
    /*
     * Make sure the t's are crossed
     */
    if (!size)
	return(NULL);
    
    /*
     * Check to see if we need to supply the memory or not
     */
    if (!region) {
	/*
	 * Yup. Someone's being cheap.
	 */
	region = malloc(sizeof(regiontype));
	if (!region)
	    return(NULL);
    } else {
	/*
	 * Heh. Let's not rely on folks handing us virgins
	 */
	memset(region, 0, sizeof(regiontype));
    }

    /*
     * Fill in the bill of goods
     */
    region->name   = name;
    region->start  = start;
    region->size   = size;
    region->class  = class;
    region->flags  = flags;
    region->media  = REGION_MEDIA_READWRITE;
    region->status = REGION_STATUS_PARENT;

    /*
     * Contruct an end pointer for future simplicity
     */
    region->end = (uchar *)((uint)start + size);

    /*
     * Add it to our list
     */
    region_insert(region);

    return(region);
}


/*
 * region_add_alias
 *
 * Causes a specified region to me marked as an alias for another.
 */

boolean region_add_alias (regiontype *region, regiontype *alias)
{
    /*
     * We don't allow aliases of aliases.
     */
    if ((!region) || (!alias) || (region->alias))
	return(FALSE);

    /*
     * Ditch alias requests that don't match up as well
     */
    if (region->size != alias->size)
	return(FALSE);

    alias->alias  = region;
    alias->class  = region->class;
    alias->status = REGION_STATUS_ALIAS;

    return(TRUE);
}

/*
 * region_find_by_attributes_internal
 *
 * Attempt to find a region matching a particular class, status
 * and media if specified. The region has to match all of the attributes
 * for it be picked as a positive match.
 *
 * Used as a base for all other search routines
 */

static regiontype *region_find_by_attributes_internal (regiontype    *start,
						region_class   class,
						region_status  status,
						region_media   media)
{
    regiontype   *region;
    list_element *element;

    /*
     * Brief sanity check
     */
    if (!start)
	return(NULL);

    /*
     * Beginning at start, look down the list of regions, searching for
     * our mark.
     */
    element = &start->element;
    while (element) {
	/*
	 * Get the region pointer from the list element
	 */
	region = LIST_GET_DATA(element);
	
	/*
	 * Is this our puppy?
	 */
	if (region_attribute_match(region, class, status, media))
	    return(region);

	/*
	 * Nope. Time to fetch another
	 */
	element = LIST_NEXT_ELEMENT(element);
    }

    return(NULL);
}

/*
 * region_find_by_attributes
 *
 */

regiontype *region_find_by_attributes (region_class  class,
				       region_status status,
				       region_media  media)
{
    return(region_find_by_attributes_internal(LIST_HEAD_DATA(&regionlist),
					      class, status, media));
}

/*
 * region_find_next_by_attributes
 *
 */

regiontype *region_find_next_by_attributes (regiontype    *region,
					    region_class   class,
					    region_status  status,
					    region_media   media)
{
    /*
     * Make sure we've got something to work with
     */
    if (!region)
	return(NULL);

    /*
     * Start from the next element in the list
     */
    region = LIST_NEXT_DATA(&region->element);

    return(region_find_by_attributes_internal(region, class, status, media));
}

/*
 * region_find_by_class
 *
 */

regiontype *region_find_by_class (region_class class)
{
    return(region_find_by_attributes(class,
				     REGION_STATUS_ANY,
				     REGION_MEDIA_ANY));
}

/*
 * region_find_next_by_class
 *
 */

regiontype *region_find_next_by_class (regiontype   *region,
				       region_class  class)
{
    return(region_find_next_by_attributes(region,
					  class,
					  REGION_STATUS_ANY,
					  REGION_MEDIA_ANY));
}

/*
 * region_find_by_addr
 *
 * Attempt to find the region a given memory address lies in. This
 * code attempts to do a best fit match on the address given and will
 * return the smallest region that encloses the address.
 *
 */

regiontype *region_find_by_addr (void *address)
{
    regiontype   *region, *target = NULL;
    list_element *element;

    /*
     * Look down the list of regions, searching for our target.
     */
    FOR_ALL_DATA_IN_LIST(&regionlist, element, region) {
	if (region_addr_match(region, address)) {
	    /*
	     * We've got a match. Is this the smallest region so far?
	     */
	    if (!target || (target == region->parent))
		target = region;
	}
    }

    /*
     * If the target is an alias, return the base region
     */
    if (region_is_alias(target)) {
	ulong  delta;
	uchar *aliased_address;

	/*
	 * Work out the delta from the alias to the base region
	 * and compute our new address.
	 */
	delta = (ulong)(target->alias->start - target->start);
	aliased_address = (uchar *)address + delta;

	/*
	 * Find out the real region this belongs to (as the
	 * base region could be a parent and we want the least
	 * fit match, which could be a child).
	 */
	target = region_find_by_addr(aliased_address);
    }

    return(target);
}


/*
 * region_get_size_by_attributes
 *
 * Given a type for a region, return the size of all of the regions
 * that match the value
 *
 */

uint region_get_size_by_attributes (region_class  class,
				    region_status status,
				    region_media  media)
{
    regiontype   *region;
    list_element *element;
    uint          size = 0;

    /*
     * Look down the list of regions, searching for our mark.
     */
    FOR_ALL_DATA_IN_LIST(&regionlist, element, region) {
	if (region_attribute_match(region, class, status, media))
	    size += region->size;
    }

    return(size);
}

/*
 * region_get_size_by_class
 *
 * Given a class for a region, return the size of all of the parent
 * regions in that class
 *
 */

uint region_get_size_by_class (region_class  class)
{
    return(region_get_size_by_attributes(class,
					 REGION_STATUS_PARENT,
					 REGION_MEDIA_ANY));
}

/*
 * region_set_class
 *
 * Change the class attributes for a region
 */

void region_set_class (regiontype *region, region_class class)
{
    list_element *element;
    regiontype   *child;

    /*
     * Set the class for this region
     */
    region->class = class;

    /*
     * Grab the next element
     */
    element = LIST_NEXT_ELEMENT(&region->element);
    while (element) {
	/*
	 * Get the region pointer from the element
	 */
	child = LIST_GET_DATA(element);

	/*
	 * Is it a child?
	 */
	if (child->parent != region)
	    break;

	/*
	 * If we're s'pposed to inherit, do the magic
	 */
	if (child->flags & REGION_FLAGS_INHERIT_CLASS)
	    region_set_class(child, class);

	element = LIST_NEXT_ELEMENT(element);
    }
}

/*
 * region_get_class
 *
 * Get the class attribute for a region
 */

region_class region_get_class (regiontype *region)
{
    return(region->class);
}

/*
 * region_set_status
 *
 * Change the status attributes for a region. This is static to prevent
 * outside users from corrupting the region hierarchy tree.
 */

static void region_set_status (regiontype *region, region_status status)
{
    /*
     * If we're an alias, then don't mess with the status
     */
    if (region_is_alias(region))
	return;

    /*
     * Set our new status
     */
    region->status = status;

    /*
     * If we're a new child, check the inheritance.
     */
    if (region_is_child(region)) {
	/*
	 * Do we inherit media?
	 */
	if (region->flags & REGION_FLAGS_INHERIT_MEDIA)
	    region_set_media(region, region->parent->media);

	/*
	 * Do we inherit class?
	 */
	if (region->flags & REGION_FLAGS_INHERIT_CLASS)
	    region_set_class(region, region->parent->class);
    }

}

/*
 * region_get_status
 *
 * Get the status attribute for a region
 */

region_status region_get_status (regiontype *region)
{
    return(region->status);
}

/*
 * region_set_media
 *
 * Change the media attributes for a region
 */

void region_set_media (regiontype *region, region_media media)
{
    list_element *element;
    regiontype   *child;

    /*
     * Set the media for this region
     */
    region->media = media;
    if ((region->media == REGION_MEDIA_READONLY) &&
	(region->class == REGION_CLASS_IMAGETEXT))
	defeat_sum();

    /*
     * Grab the next element
     */
    element = LIST_NEXT_ELEMENT(&region->element);
    while (element) {
	/*
	 * Get the region pointer from the element
	 */
	child = LIST_GET_DATA(element);

	/*
	 * Is it a child?
	 */
	if (child->parent != region)
	    break;

	/*
	 * If we're s'pposed to inherit, do the magic
	 */
	if (child->flags & REGION_FLAGS_INHERIT_MEDIA)
	    region_set_media(child, media);

	element = LIST_NEXT_ELEMENT(element);
    }
}

/*
 * region_get_media
 *
 * Change the media attributes for a region
 */

region_media region_get_media (regiontype *region)
{
    return(region->media);
}

/*
 * region_exists
 *
 * Return a boolean as to whether a parent region of a particular
 * class exists anywhere in the system
 */

boolean region_exists (region_class class)
{
    return(region_find_by_attributes(class,
				     REGION_STATUS_ANY,
				     REGION_MEDIA_ANY) != NULL);
}


/*
 * region_is_possible_child
 *
 * Returns true if the region passed in is completely enclosed
 * by the supplied parent region
 */

static inline boolean region_is_possible_child (regiontype   *region,
						regiontype   *parent)
{
    return((region->start >= parent->start) &&
	   (region->end   <= parent->end));

}

/*
 * region_insert
 *
 * Insert a region in the current region list.
 */

void region_insert (regiontype *region)
{
    regiontype   *parent = NULL, *current;
    list_element *element;

    /*
     * Look down the list of regions, searching for our target.
     */
    FOR_ALL_DATA_IN_LIST(&regionlist, element, current) {
	/*
	 * Check to see if we're a child
	 */
	if (region_is_possible_child(region, current))
	    parent = current;
	
	/*
	 * We there yet?
	 */
	if ((!current) || (current->start > region->start))
	    break;
    }

    /*
     * Find the right element to insert after
     */
    if (!element)
	element = LIST_TAIL_ELEMENT(&regionlist);
    else
	element = LIST_PREV_ELEMENT(element);

    current = LIST_GET_DATA(element);

    /*
     * If we're a child, record the fact
     */
    if (parent) {
	region->parent = parent;
	region_set_status(region, REGION_STATUS_CHILD);
    }

    /*
     * Insert our new region after target
     */
    LIST_SET_DATA(&region->element) = region;
    list_insert_element(&regionlist,
			(current) ? &current->element : NULL,
			&region->element);

    /*
     * Finally, find out if our new region is a daddy.
     */
    element = LIST_NEXT_ELEMENT(&region->element);
    while (element) {
	/*
	 * Get the region pointer from the element
	 */
	current = LIST_GET_DATA(element);

	/*
	 * Is this a valid new child?
	 */
	if ((!region_is_possible_child(current, region)) ||
	    ((current->parent) && (current->parent->size < region->size)))
	    break;

	/*
	 * It's our sprog
	 */
	current->parent = region;
	region_set_status(current, REGION_STATUS_CHILD);
	
	/*
	 * On to the next one
	 */
	element = LIST_NEXT_ELEMENT(element);
    }
}
