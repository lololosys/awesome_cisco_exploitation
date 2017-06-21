/* $Id: subsys.c,v 3.4.20.5 1996/09/06 01:11:46 snyder Exp $
 * $Source: /release/112/cvs/Xsys/os/subsys.c,v $
 *------------------------------------------------------------------
 * subsys.c -- Discover and Initialize the Subsystems
 *
 * 1-June-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: subsys.c,v $
 * Revision 3.4.20.5  1996/09/06  01:11:46  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.4.20.4  1996/08/28  13:04:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.20.3  1996/07/06  05:55:46  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.4.20.2  1996/05/24  19:30:05  hampton
 * Improve the granularity of low memory corruption checks during
 * subsystem initialization.  [CSCdi58674]
 * Branch: California_branch
 *
 * Revision 3.4.20.1  1996/03/18  21:31:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/09  05:10:36  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.26.2  1996/03/07  10:20:00  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  16:42:00  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/01/25  11:20:39  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.3  1995/11/17  18:52:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:19:01  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:48:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/19  18:38:54  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.3  1995/07/11  02:50:47  smackie
 * Add some paranoia to the subsystem sequence recursion and prevent
 * bogusness from causing stack problems. Also throw in support for
 * subsystems in multiple data segments. (CSCdi36893)
 *
 * Revision 2.2  1995/06/28  09:18:48  smackie
 * Finally make subsystem sequencing robust with multiple hierarchy
 * sequences. Tidy up subsystem code into the bargain and split the
 * parser code into a new file. Make the subsystem code far noisier
 * about bogusness. (CSCdi23568)
 *
 * Revision 2.1  1995/06/07  21:59:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#include <ciscolib.h>
#include "config.h"
#include "sys_registry.h"
#include "ttysrv.h"
#include "logger.h"
#include "subsys.h"
#include "subsys_private.h"
#include "packet.h"
#include "../os/chunk.h"
#include "../os/region.h"
#include "../os/msg_subsys.c"		/* Not a typo, see logger.h */

/*
 * Storage
 */
list_header subsyslist[SUBSYS_CLASS_MAX];
const char  subsys_property_seq[] = "seq:";
const char  subsys_property_req[] = "req:";

static list_header pendinglist;
static ulong       subsysID = 0;


/*
 * subsys_find_by_name_inline
 *
 * Given a list pointer, check to see if a subsystem called "name" exists
 * on it.
 */

static inline subsystype *subsys_find_by_name_inline (list_header *list,
						      char        *name)
{
    list_element *element;
    subsystype   *subsys;

    FOR_ALL_DATA_IN_LIST(list, element, subsys) {
	/*
	 * Find out if this is the one
	 */
	if (!strcmp(subsys->namestring, name))
	    return(subsys);
    }

    return(NULL);
}


/*
 * subsys_find_by_name
 *
 * Returns a pointer to a subsystem structure given a name
 */

subsystype *subsys_find_by_name (char *name)
{
    subsystype   *subsys;
    int           count;

    for (count = 0; count < SUBSYS_CLASS_MAX; count++) {
	/*
	 * Look down the class list for this subsystem
	 */
	subsys = subsys_find_by_name_inline(&subsyslist[count], name);
	if (subsys)
	    return(subsys);
    }

    return(NULL);
}

/*
 * subsys_get_property_list
 *
 * Returns true and fills the optional string with property information
 * if a property is specified for this subsystem
 */
boolean subsys_get_property_list (subsystype *subsys,
				  const char *const property,
				  char       *property_list)
{
    int      count, len = SUBSYS_PROPERTIES_LIST_SIZE;
    const char * ptr;

    /*
     * Protect ourselves from evil, clumsy people
     */
    if ((!subsys) || (!property))
	return(FALSE);
    
    /*
     * Ensure the string is terminated for strcat()
     */
    if (property_list)
	*property_list = 0;

    for (count = 0; count < SUBSYS_PROPERTIES_MAX; count++) {
	/*
	 * Get a pointer to the first property
	 */
	ptr = subsys->properties[count];

	/*
	 * If the pointer is NULL or isn't our property then
	 * just skip it
	 */
	if ((!ptr) || ((ptr = strstr(ptr, property)) == NULL))
	    continue;

	/* 
	 * If we have a property string to copy into, skip past the
	 * property marker and copy in the rest of the info to our
	 * string
	 */
	ptr += strlen(property);
	
	/*
	 * Absorb leading whitespace
	 */
	while (isspace(*ptr) || (*ptr == ','))
	    ptr++;
	
	if (property_list)
	    sstrncat(property_list, ptr, len);
	
	/*
	 * Adjust the freespace left in list
	 */
	len -= strlen(ptr);
    }

    return(len != SUBSYS_PROPERTIES_LIST_SIZE);
}

/*
 * subsys_get_property_element
 *
 * Given a property list string and a pointer, returns the next
 * element in the list
 */

static char *subsys_get_property_element (char *property_list,
					  char *name)
{
    char *ptr = property_list;
    int   len;
    
    /*
     * Absorb whitespace
     */
    while (isspace(*ptr) || (*ptr == ','))
	ptr++;
    
    /*
     * Get ready to strip out another name if we've got one
     */
    len = 0;
    
    if (*ptr != 0) {
	/*
	 * Whilst we have genuine data, copy the subsystem name
	 * into our local string
	 */
	while (!isspace(*ptr) && (*ptr != ',') &&
	       (*ptr != 0) && (len < SUBSYS_PROPERTIES_LIST_SIZE))
	    name[len++] = *ptr++;
    }	
    name[len] = 0;

    return(ptr);
}


/*
 * subsys_sequenced_insert
 *
 * Takes a subsystem as a seed and inserts any pending sequence
 * dependancies before it in initialization order.
 *
 * This routine is recursive. All stack useage must be carefully
 * controlled - the chunk manager is used for string storage.
 */

static void subsys_sequenced_insert (subsystype *subsys,
				     chunk_type *chunk,
				     int         depth)
{
    subsystype *sequence;
    char       *ptr, *property_list, *name;

    /*
     * Make sure we've got something to work with
     */
    if ((!subsys) || (!chunk))
	return;

    /*
     * Whine if we're in too deep
     */
    if (depth > SUBSYS_SEQUENCE_MAX_DEPTH) {
	(*kernel_errmsg)(&msgsym(RECURSION, SUBSYS),
	       SUBSYS_SEQUENCE_MAX_DEPTH, subsys->namestring);
	return;
    }

    /*
     * Grab some space for our property strings
     */
    property_list = chunk_malloc(chunk);
    if (!property_list)
	return;

    name = chunk_malloc(chunk);
    if (!name) {
	chunk_free(chunk, property_list);
	return;
    }

    /*
     * Check for a "sequence" property for this subsystem
     */
    if (!subsys_get_property_list(subsys,
				  subsys_property_seq,
				  property_list)) {
	chunk_free(chunk, property_list);
	chunk_free(chunk, name);
	return;
    }
    
    /*
     * Point our working pointer at the head of the sequence string
     */
    ptr = property_list;

    /*
     * As long as we have names, run through this loop.
     */
    while (*ptr != 0) {
	/*
	 * Get the next subsystem name from the sequence list
	 */
	ptr = subsys_get_property_element(ptr, name);
	
	/*
	 * Is this subsystem pending insertion into a class list?
	 */
	sequence = subsys_find_by_name_inline(&pendinglist, name);
	if (sequence) {
	    /*
	     * Yes it is. First, remove the subsystem from the pending
	     * list...
	     */
	    list_remove(&pendinglist, NULL, sequence);
	    
	    if (subsys->class != sequence->class)
		(*kernel_errmsg)(&msgsym(BADSEQUENCE, SUBSYS),
		       subsys->namestring, sequence->namestring);

	    /*
	     * ..and attempt to make sure *it's* sequence dependancies
	     * are met before proceeding.
	     */
	    subsys_sequenced_insert(sequence, chunk, ++depth);
	} else {
	    sequence = subsys_find_by_name(name);
	    if ((sequence) && (subsys->class != sequence->class))
		(*kernel_errmsg)(&msgsym(BADSEQUENCE, SUBSYS),
		       subsys->namestring, sequence->namestring);
	}
    }

    /*
     * All the problematic sequence dependancies have been dealt with.
     * Move our subsystem to the real subsystem list.
     */
    list_enqueue(&subsyslist[subsys->class], NULL, subsys);

    /*
     * Free our property strings
     */
    chunk_free(chunk, property_list);
    chunk_free(chunk, name);
}


/*
 * subsys_verify_required_list
 *
 * Returns TRUE if all the required subsystems are present and active
 * for the subsystem supplied
 */

boolean subsys_verify_required_list (subsystype *subsys, boolean verbose)
{
    char       *ptr;
    char        property_list[SUBSYS_PROPERTIES_LIST_SIZE];
    char        name[SUBSYS_PROPERTIES_LIST_SIZE];
    boolean     failures = FALSE;

    if (!subsys)
	return(TRUE);

    /*
     * Check for a "required" property for this subsystem
     */
    if (!subsys_get_property_list(subsys,
				  subsys_property_req,
				  property_list))
	return(TRUE);

    /*
     * Scan our property list
     */
    ptr = property_list;
    while (*ptr != 0) {
	ptr = subsys_get_property_element(ptr, name);
	/*
	 * Check to see if that subsystem is present
	 */
	if (!subsys_find_by_name(name)) {
	    if (verbose)
		(*kernel_errmsg)(&msgsym(NOTFOUND, SUBSYS),
				 subsys->namestring, name);
	    
	    failures = TRUE;
	}
    }

    return(!failures);
}

/*
 * subsys_init
 *
 * Find all subsystems in the linked image.
 * We assume the subsystem headers are in the initialized data segment.
 * Subsystem headers may possibly not be aligned on longword boundaries,
 * so some care is needed.
 */

void subsys_init (void)
{
    list_header *list;
    subsystype  *subsys;
    ushort      *sptr;
    int          count;
    subsys_class class;
    regiontype  *data_region;
    chunk_type  *property_chunk;

    /*
     * Initialize our subsystem class lists
     */
    for (count = 0; count < SUBSYS_CLASS_MAX; count++) {
	list_create(&subsyslist[count], 0,
		    subsys_get_class_string(count),
		    LIST_FLAGS_AUTOMATIC);
    }

    list_create(&pendinglist, 0, "Subsys Pending", LIST_FLAGS_AUTOMATIC);

    /*
     * Walk down the known data segments looking for subsystems
     */
    data_region = region_find_by_class(REGION_CLASS_IMAGEDATA);
    
    while (data_region) {
	/*
	 * Run up through the data segment looking for subsystem headers
	 */
	for (sptr = (ushort *)data_region->start;
	     sptr < (ushort *)data_region->end;
	     sptr++) {
	    if (GETLONG((uchar *)sptr) == SUBSYS_MAGIC_NUMBER && 
		GETLONG((uchar *)sptr + sizeof(ulong)) == SUBSYS_MAGIC_NUMBER){
	    
		subsys = (subsystype *)sptr;
		
		/*
		 * Make sure the header version is understandable
		 */
		if (subsys->header_version != SUBSYS_HEADER_VERSION) {
		    (*kernel_errmsg)(&msgsym(BADVERSION, SUBSYS),
			   subsys->header_version);
		    continue;
		}
		
		/*
		 * Make sure target kernel version matches ours
		 */
		if ((subsys->kernel_majversion != SYS_MAJORVERSION) ||
		    (subsys->kernel_minversion != SYS_MINORVERSION)) {
		    (*kernel_errmsg)(&msgsym(MISMATCH, SUBSYS),
			   subsys->kernel_majversion,
			   subsys->kernel_minversion);
		    continue;
		}
		
		/*
		 * Figure out which queue we go into
		 */
		class = subsys->class;
		
		if ((class > 0) && (class < SUBSYS_CLASS_MAX)) {
		    list = &subsyslist[class];
		} else {
		    (*kernel_errmsg)(&msgsym(BADCLASS, SUBSYS), class);
		    continue;		
		}
		
		/*
		 * Give the subsystem a unique ID
		 */
		subsys->ID = ++subsysID;
		
		/*
		 * If the subsystem has a sequence property, it goes on
		 * to the pending queue.
		 */
		if (subsys_get_property_list(subsys,
					     subsys_property_seq, NULL))
		    list = &pendinglist;
		
		list_enqueue(list, NULL, subsys);
	    }
	}

	/*
	 * Get the next data region from the region manager
	 */
	data_region = region_find_next_by_class(data_region,
						REGION_CLASS_IMAGEDATA);
    }

    /*
     * Create a memory chunk to deal with our pending sequencing
     */
    property_chunk = chunk_create(SUBSYS_PROPERTIES_LIST_SIZE,
				  SUBSYS_PROPERTIES_CHUNK_SIZE,
				  CHUNK_FLAGS_DYNAMIC,
				  NULL, 0,
				  "Properties");
    
    /*
     * For all the subsystems in the pending list, dequeue each one
     * in turn and evaluate whether their sequence properties have
     * been met.
     */
    subsys = list_dequeue(&pendinglist);
    while (subsys) {
	/*
	 * Attempt to sequence it
	 */
	subsys_sequenced_insert(subsys, property_chunk, 0);

	/*
	 * Grab the next victim
	 */
	subsys = list_dequeue(&pendinglist);
    }

    /*
     * We're finished. Destroy the pending chunk
     */
    chunk_destroy(property_chunk);
}



/*
 * subsys_init_routine
 * Initialize a sorted list of subsystems
 */

static void subsys_init_routine (subsystype *subsys)
{
    if (!subsys_verify_required_list(subsys, TRUE))
	return;

    subsys->status = SUBSYS_STATUS_ACTIVE;

    if (subsys->entry)
	(*subsys->entry)(subsys);

}

/*
 * subsys_init_class
 * Initialize a class of sorted subsystems
 */

void subsys_init_class (subsys_class class)
{
    list_element *element;
    subsystype   *subsys;

    if ((class <= 0) || (class >= SUBSYS_CLASS_MAX))
	return;

    if (sched_test_page_zero())
	(*kernel_errmsg)(&msgsym(PAGEZERO, SUBSYS),
			 subsys_get_class_string(class), "'start'");

    FOR_ALL_DATA_IN_LIST(&subsyslist[class], element, subsys) {
	subsys_init_routine(subsys);
	if (sched_test_page_zero())
	    (*kernel_errmsg)(&msgsym(PAGEZERO, SUBSYS),
			     subsys_get_class_string(class),
			     subsys->namestring);
    }

    reg_invoke_subsys_init_class(class);
    if (sched_test_page_zero())
	(*kernel_errmsg)(&msgsym(PAGEZERO, SUBSYS),
			 subsys_get_class_string(class), "'end'");
}

/*
 * subsys_get_class_string
 * Helper routine that converts a class ID to a string
 */    

char *subsys_get_class_string (subsys_class class)
{
    switch (class) {
      case SUBSYS_CLASS_SYSINIT:
	return("SystemInit");
      case SUBSYS_CLASS_KERNEL:
	return("Kernel");
      case SUBSYS_CLASS_LIBRARY:
	return("Library");
      case SUBSYS_CLASS_DRIVER:
	return("Driver");
      case SUBSYS_CLASS_PROTOCOL:
	return("Protocol");
      case SUBSYS_CLASS_MANAGEMENT:
	return("Management");
      case SUBSYS_CLASS_REGISTRY:
	return("Registry");
      default:
	return("unknown");
    }
}
