/* $Id: registry.c,v 3.6.20.3 1996/08/28 13:04:08 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/registry.c,v $
 *------------------------------------------------------------------
 * registry.c -- Function Registration
 *
 * 4-March-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: registry.c,v $
 * Revision 3.6.20.3  1996/08/28  13:04:08  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.20.2  1996/04/27  06:36:12  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.5.16.5  1996/04/26  01:18:54  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.5.16.4  1996/04/17  03:54:18  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.5.16.3  1996/02/23  20:52:42  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.5.16.2  1996/01/26  20:25:51  agh
 * Branch: Dial1_branch
 * Initial Checkin of MLPVT Code.
 *
 * Revision 3.5.16.1  1996/01/18  00:16:52  syiu
 * Branch: Dial1_branch
 * PPP L2F registry support
 *
 * Revision 3.6.20.1  1996/03/18  21:31:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.20.3  1996/03/09  05:10:18  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.5.20.2  1996/03/07  10:19:34  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.20.1  1996/02/20  16:41:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/01/25  11:20:34  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.5  1995/12/01  19:04:26  sdurham
 * CSCdi44362:  envmon is non-modular w.r.t envmon mib.
 *   Remove direct calls from envmon code to mib code. Use service points.
 *
 * Revision 3.4  1995/11/20  22:27:03  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  18:50:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:18:37  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:47:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:19:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/09/23  21:41:54  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Add an ISIS registry.
 *
 * Revision 2.4  1995/09/19  18:38:51  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.3  1995/06/21  08:56:18  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.2  1995/06/19  06:48:53  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:58:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <string.h>
#include <ciscolib.h>
#include "subsys.h"
#include "logger.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "address.h"
#include "packet.h"

#include "../h/sys_registry.regc"
#include "registry_private.h"
#include "chunk.h"
#include "msg_system.c"			/* Not a typo, see logger.h */


/*
 * Local Definitions
 */

#define REGISTRY_FUNCTION_CHUNK_MAX 100


/*
 * Local Storage
 */
registrytype registry_array[REG_MAXCOUNT];

registrystats registry_statistics;
registrystats registry_service_statistics[SERVICE_MAX];
registrystats registry_function_statistics;

static chunk_type *registry_function_chunk;


/*
 * registry_default_routine
 */
static boolean registry_default_routine (void)
{
    return(FALSE);	/* Do nothing. */
}

/*
 * registry_increment_statistics
 *
 * Add to the item and byte count of a particular registry counter
 */

static void registry_increment_statistics (registrystats *statistics,
					   ulong          bytes)
{
    statistics->items++;
    statistics->bytes += bytes;
}


/*
 * registry_decrement_statistics
 *
 * Subtract from the item and byte count of a particular registry counter
 */

static void registry_decrement_statistics (registrystats *statistics,
					   ulong          bytes)
{
    statistics->items--;
    statistics->bytes -= bytes;
}

/*
 * registry_init
 * Initialize registration support
 */

void registry_init (void)
{
    /*
     * Initialize our statistic counters
     */
    registry_function_statistics.items = 0;
    registry_function_statistics.bytes = 0;

    memset(registry_service_statistics, 0,
	   sizeof(registry_service_statistics));

    registry_statistics.items = sizeof(registry_array)/sizeof(registrytype);
    registry_statistics.bytes = sizeof(registry_array);

    /*
     * Initialize the list of well known registries.
     */
    memset(registry_array, 0, sizeof(registry_array));

    /*
     * Create and populate the system registry
     */
    registry_create(REG_SYS, SERVICE_SYS_MAX, "System");
    create_registry_sys();

    /*
     * Initialize the chunk handlers
     */
    registry_function_chunk = chunk_create(sizeof(functype),
					   REGISTRY_FUNCTION_CHUNK_MAX,
					   CHUNK_FLAGS_DYNAMIC,
					   NULL, 0,
					   "Reg Function");

    /*
     * Start the other registry subsystems
     */
    subsys_init_class(SUBSYS_CLASS_REGISTRY);
}

/*
 * service_lookup
 * Find or create a registry data structure
 */

static inline regservicetype *service_lookup_inline (regtype_t regcode,
						     ushort    servcode,
						     ushort    servtype,
						     boolean   quiet)
{
    registrytype   *reg;
    regservicetype *serv;

    if ((regcode >= REG_MINCOUNT ) && (regcode < REG_MAXCOUNT))  {
	reg = &registry_array[regcode];
	if (reg->services) {
	    if (servcode < reg->servicecount) {
		serv = &reg->services[servcode];
		if (serv->servtype != servtype) {
		    (*kernel_errmsg)(&msgsym(REGTYPE, SYS), regcode,
				     servcode, servtype);
		    return(NULL);
		}
		return(serv);
	    }

	    /*
	     * Bad service code or service not initialized.
	     */
	    if (!quiet)
		(*kernel_errmsg)(&msgsym(REGNONE, SYS), regcode, servcode,
		       "add/delete/invoke");
	    return(NULL);
	}
    }
	
    /*
     * Bad registry code or registry not initialized.
     */
    if (!quiet)
	(*kernel_errmsg)(&msgsym(SUPNONE, SYS), regcode);
    return(NULL);
}

static regservicetype *service_lookup (regtype_t regcode,
				       ushort    servcode,
				       ushort    servtype)
{
    return(service_lookup_inline(regcode, servcode, servtype, FALSE));
}


/*
 * registry_create
 *
 * Create a registry. If the registry code supplied is REG_UNKNOWN, we
 * have a dynamic registry. Search for a slot to use and create the
 * registry there. The function always returns the index of the registry
 * created.
 */

regtype_t registry_create (regtype_t regcode, int servcount, char *name)
{
    /*
     * If no registry code is supplied, we have a dynamic registry
     * and should search for a registry code
     */
    if (regcode == REG_UNKNOWN) {
	for (regcode = REG_DYNAMIC1; regcode < REG_MAXCOUNT; regcode++) {
	    /*
	     * Is this registry slot in use?
	     */
	    if (!registry_array[regcode].servicecount)
		break;
	}
    } else {
	/*
	 * Make sure we haven't done this before
	 */
	if (registry_allocated(regcode)) {
	    (*kernel_errmsg)(&msgsym(SUPTWICE, SYS), name);
	    return(REG_ILLEGAL);
	}
    }

    /*
     * Make sure the registry index is within bounds and that the
     * number of services requested isn't outrageous
     */
    if ((regcode < REG_MINCOUNT) ||
	(regcode >= REG_MAXCOUNT) ||
	(servcount > REG_SERVICE_MAXIMUM)) {
	/*
	 * Can't create registry.  Return a clearly bogus index.
	 * Put out an error message on the console.
	 */
	(*kernel_errmsg)(&msgsym(SUPNOT, SYS), name);
	return(REG_ILLEGAL);
    } else {
	/*
	 * We have a blank slot.  Reserve it, zero index array
	 * and return index.
	 */
	registry_array[regcode].servicecount  = servcount;
	registry_array[regcode].services      = NULL;
	registry_array[regcode].name          = name;
    }
    return(regcode);
}


/*
 * service_create
 * Create a service within a registry and give it a name.
 */

void service_create (regtype_t  regcode,
		     ushort     servcode,
		     ushort     servtype,
		     ushort     maxentries,
		     ushort     numargs,
		     char      *name)
{
    registrytype   *reg;
    regservicetype *serv;
    int             service_bytes = 0, function_bytes = 0, i;
    boolean         function_table;
 
    /*
     * Validate the registry code and make sure its assigned.
     */
    if ((regcode < REG_MINCOUNT) ||
        (regcode >= REG_MAXCOUNT) ||
        (!registry_array[regcode].servicecount)) {
        (*kernel_errmsg)(&msgsym(SUPNONE, SYS), regcode);
        return;
    }
 
    /*
     * Find a pointer to the registry
     */
    reg = &registry_array[regcode];
 
    /*
     * Validate the registry service code.
     */
    if (servcode >= reg->servicecount) {
        (*kernel_errmsg)(&msgsym(REGNONE, SYS), regcode, servcode, "create");
        return;
    }
 
    /*
     * Do we need to create the service array?
     */
    if (!reg->services) {
        /*
         * Malloc an array to hold all the services
         */
        service_bytes = sizeof(regservicetype) * reg->servicecount;
        reg->services = malloc_named(service_bytes, "Registry Services");
        if (!reg->services) {
            return;
        }
    }
    serv = &reg->services[servcode];
 
    /*
     * Make sure we have a reasonable number of arguments
     */
    if (numargs > SERVICE_MAXARG) {
        (*kernel_errmsg)(&msgsym(REGARG, SYS), regcode, servcode);
        return;
    }
 
    /*
     * Create and initialize a new service.
     * If maxentries is positive and we are creating a service that'll
     * use a lookup table, create a table for more efficient lookups.
     */
    function_table = ((maxentries > 0) &&
                      ((servtype == SERVICE_CASE) ||
                       (servtype == SERVICE_VALUE)));

    /*
     * If we have a function_table, malloc() and build the lookaside list.
     */
    if (function_table) {
        function_bytes = maxentries * sizeof(functype);
        serv->function_table = malloc_named(function_bytes,
					    "Registry Functions");
        if (!serv->function_table) {
            return;
        }
        
        /*
	 * Mark all of the elements as being unassigned and set the functions
	 * to registry_default_routine() to protect the fast registry
	 */
        for (i = 0; i < maxentries; i++) {
            serv->function_table[i].i.assigned = FALSE;
            serv->function_table[i].j.routine  =
		(void *)registry_default_routine;
        }
    } else {
        /*
         * Just be double sure
         */
        serv->function_table = NULL;
    }
 
    /*
     * Initialize the rest of our motley crew
     */
    serv->function_queue   = NULL;
    serv->function_default = NULL;
    serv->servtype     = servtype;
    serv->maxentries   = maxentries;
    serv->numargs      = numargs;

    registry_increment_statistics(&registry_service_statistics[servtype],
				  service_bytes + function_bytes);

    REGISTRY_STRING_DEFINE(serv->name = name;)
}

/*
 * registry_function_enqueue
 *
 * Barebones routine that allows a function structure to be enqueued for
 * a service without using a queuetype. It's geared for minimalist 
 * memory useage over performance
 */

static void registry_function_enqueue (functype **head, functype *func)
{
    functype *tail = *head;

    /*
     * If we don't have a tail, we don't have any elements. This function
     * goes to the head
     */
    if (tail == NULL) {
	*head = func;
	return;
    }
    
    /*
     * Find the end
     */
    while (tail->i.next)
	tail = tail->i.next;

    /*
     * Append this function to the end
     */
    func->i.next = NULL;
    tail->i.next = func;
}

/*
 * registry_function_unqueue
 *
 * Barebones routine that allows a function structure to be dequeued from
 * a service without using a queuetype. 
 */

static void registry_function_unqueue (functype **head, functype *func)
{
    functype *prev, *ptr;

    prev = ptr = *head;

    /*
     * Handle the head special case
     */
    if (func == ptr) {
	*head = func->i.next;
	return;
    }

    /*
     * Wade down through the list looking for our prey
     */
    while (ptr) {
	if (func == ptr) {
	    prev->i.next = func->i.next;
	    return;
	}
	prev = ptr;
	ptr = ptr->i.next;
    }
}


/*
 * registry_add_case
 * Add a function to a case service
 * This routine is always invoked using its index.
 */

boolean registry_add_case (regtype_t  regcode,
			   ushort     servcode,
			   long       index,
			   void      *routine,
			   char      *name)
{
    regservicetype *serv;
    functype *func;
    leveltype level;

    /*
     * Get pointer to registry data structure
     */
    serv = service_lookup(regcode, servcode, SERVICE_CASE);
    if (!serv)
	return(FALSE);

    level = raise_interrupt_level(ALL_DISABLE);
    
    if (serv->function_table &&
        ((index >= 0) && (index < serv->maxentries))) {
        /*
         * Use the lookaside list
         */
        func = &serv->function_table[index];
        func->i.assigned = TRUE;
    } else {
        /*
         * We're using the queue. Make sure that there isn't a entry
         * for this index already
         */
        for (func = serv->function_queue; func; func = func->i.next) {
            if (func->index == index) {
                registry_delete_case(regcode, servcode, index);
                break;
            }
        }
        func = chunk_malloc(registry_function_chunk);
        if (!func) {
            reset_interrupt_level(level);
            return(FALSE);
        }
	registry_increment_statistics(&registry_function_statistics,
				      sizeof(functype));
        registry_function_enqueue(&serv->function_queue, func);
    }
    
    /*
     * New func, create a registration for it.
     */
    func->j.routine = routine;
    func->index = index;

    REGISTRY_STRING_DEFINE(func->name = name;)

    reset_interrupt_level(level);
    return(TRUE);
}

/*
 * registry_add_list
 * Add a function to a list service
 * This routine is always invoked as a member of a list.
 */

boolean registry_add_list (regtype_t  regcode,
			   ushort     servcode,
			   void      *routine,
			   char      *name)
{
    regservicetype *serv;
    functype *func;

    /*
     * Get pointer to service data structure
     * Make sure it is a list service.
     */
    serv = service_lookup(regcode, servcode, SERVICE_LIST);
    if (!serv)
	return(FALSE);

    /*
     * First we see if the function is already in the service
     */
    for (func = serv->function_queue; func; func = func->i.next) {
        if (func->j.routine == routine)
            return(TRUE);
    }
 
    /*
     * New function, register it.
     */
    func = chunk_malloc(registry_function_chunk);
    if (!func)
        return(FALSE);
    func->j.routine = routine;

    REGISTRY_STRING_DEFINE(func->name = name;)

    registry_increment_statistics(&registry_function_statistics,
				  sizeof(functype));

    registry_function_enqueue(&serv->function_queue, func);
    return(TRUE);
}

/*
 * registry_add_default
 * Add a default function to a case service
 */

boolean registry_add_default (regtype_t  regcode,
			      ushort     servcode,
			      void      *routine,
			      char      *name)
{
    regservicetype *serv;
    functype *func;

    /*
     * Get the service, make sure it is an case service.
     */
    serv = service_lookup(regcode, servcode, SERVICE_CASE);
    if (!serv)
	return(FALSE);

    /*
     * If the default already exists and is different, complain
     * Otherwise, ignore it.
     */
    func = serv->function_default;
    if (func) {
        if (func->j.routine != routine) {
            (*kernel_errmsg)(&msgsym(REGDFLT, SYS), regcode, servcode);
            return(FALSE);
        } else {
            return(TRUE);
        }
    }
 
    /*
     * Register the default function
     */
    func = chunk_malloc(registry_function_chunk);
    if (!func)
        return(FALSE);
    func->j.routine = routine;
    serv->function_default = func;      

    registry_increment_statistics(&registry_function_statistics,
				  sizeof(functype));

    REGISTRY_STRING_DEFINE(func->name = name;)

    return(TRUE);
}

/*
 * registry_add_stub
 * Add a function to a stub service
 */

boolean registry_add_stub (regtype_t  regcode,
			   ushort     servcode,
			   void      *routine,
			   char      *name)
{
    regservicetype *serv;
    functype *func;
    leveltype level;

    /*
     * Get the service, make sure it is an stub service.
     */
    serv = service_lookup(regcode, servcode, SERVICE_STUB);
    if (!serv)
	return(FALSE);

    level = raise_interrupt_level(ALL_DISABLE);
    
    /*
     * If the default already exists, delete it
     */
    func = serv->function_default;
    if (func) {
        registry_delete_stub(regcode, servcode);
    }
 
    /*
     * Register the default function
     */
    func = chunk_malloc(registry_function_chunk);
    if (!func) {
        reset_interrupt_level(level);
        return(FALSE);
    }
    func->j.routine = routine;
    serv->function_default = func;      

    registry_increment_statistics(&registry_function_statistics,
				  sizeof(functype));

    REGISTRY_STRING_DEFINE(func->name = name;)

    reset_interrupt_level(level);
    return(TRUE);
}

/*
 * registry_add_value
 * Add a value to a value service
 * This routine is always invoked using its index.
 */
boolean registry_add_value (regtype_t  regcode,
			    ushort     servcode,
			    long       index,
			    ulong      value,
			    char      *name)
{
    regservicetype *serv;
    functype *func;

    /*
     * Get pointer to registry data structure
     */
    serv = service_lookup(regcode, servcode, SERVICE_VALUE);
    if (!serv)
	return(FALSE);

    /*
     * New func, create a registration for it.
     */
    if (serv->function_table &&
        ((index >= 0) && (index < serv->maxentries))) {
        func = &serv->function_table[index];
        func->i.assigned = TRUE;
    } else {
        /*
         * If we've already got an entry in a queue, update it
         */
        for (func = serv->function_queue; func; func = func->i.next){
            if (func->index == index) {
                func->j.value = value;
		REGISTRY_STRING_DEFINE(func->name = name;)
                return(TRUE);
            }
        }
 
        func = chunk_malloc(registry_function_chunk);
        if (!func)
            return(FALSE);

	registry_increment_statistics(&registry_function_statistics,
				      sizeof(functype));

        registry_function_enqueue(&serv->function_queue, func);
    }
    func->j.value = value;
    func->index = index;
    REGISTRY_STRING_DEFINE(func->name = name;)
    return(TRUE);
}

/*
 * registry_add_default_value
 * Add a default function to a case service
 */

boolean registry_add_default_value (regtype_t  regcode,
				    ushort     servcode,
				    ulong      value,
				    char      *name)
{
    regservicetype *serv;
    functype *func;

    /*
     * Get the service, make sure it is an case service.
     */
    serv = service_lookup(regcode, servcode, SERVICE_VALUE);
    if (!serv)
	return(FALSE);

    /*
     * If the default already exists and is different, complain
     * Otherwise, ignore it.
     */
    func = serv->function_default;
    if (func) {
        if (func->j.value != value) {
            (*kernel_errmsg)(&msgsym(REGDFLT, SYS), regcode, servcode);
            return(FALSE);
        } else {
            return(TRUE);
        }
    }
 
    /*
     * Register the default function
     */
    func = chunk_malloc(registry_function_chunk);
    if (!func)
        return(FALSE);
    func->j.value = value;
    serv->function_default = func;      

    registry_increment_statistics(&registry_function_statistics,
				  sizeof(functype));

    REGISTRY_STRING_DEFINE(func->name = name;)
    return(TRUE);
}

/*
 * registry_delete_case
 * Delete a particular function from a service
 */

void registry_delete_case (regtype_t regcode, ushort servcode, long index)
{
    regservicetype *serv;
    functype *func;

    serv = service_lookup(regcode, servcode, SERVICE_CASE);
    if (!serv)
	return;
    if (serv->function_table && (index >= 0) && (index < serv->maxentries)) {
        serv->function_table[index].i.assigned = FALSE;
        return;
    }
    for (func = serv->function_queue; func; func = func->i.next) {
        if (func->index == index) {
            registry_function_unqueue(&serv->function_queue, func);
            chunk_free(registry_function_chunk, func);
	    registry_decrement_statistics(&registry_function_statistics,
					  sizeof(functype));
            return;
        }
    }
}

/*
 * registry_delete_default
 * Delete the default function from a case service
 */

void registry_delete_default (regtype_t regcode, ushort servcode)
{
    functype *function_default;
    regservicetype *serv;

    serv = service_lookup(regcode, servcode, SERVICE_CASE);
    if (!serv)
	return;
    function_default = serv->function_default;
    if (function_default) {
        serv->function_default = NULL;
        chunk_free(registry_function_chunk, function_default);
	registry_decrement_statistics(&registry_function_statistics,
				      sizeof(functype));
    }
}

/*
 * registry_delete_list
 * Delete an entry from a list
 */

void registry_delete_list (regtype_t regcode, ushort servcode, void *routine)
{
    regservicetype *serv;
    functype *func;

    serv = service_lookup(regcode, servcode, SERVICE_LIST);
    if (!serv)
	return;

    /*
     * find the routine
     */
    for (func = serv->function_queue; func; func = func->i.next) {
        if (func->j.routine == routine)
            break;
    }
 
    /*
     * not found, nothing to do
     */
    if (func == NULL)
        return;
 
    registry_function_unqueue(&serv->function_queue, func);
    chunk_free(registry_function_chunk, func);
    registry_decrement_statistics(&registry_function_statistics,
				  sizeof(functype));

    return;
}

/*
 * registry_delete_stub
 * Delete a stub
 */

void registry_delete_stub (regtype_t regcode, ushort servcode)
{
    functype *function_default;
    regservicetype *serv;

    serv = service_lookup(regcode, servcode, SERVICE_STUB);
    if (!serv)
	return;
    function_default = serv->function_default;
    if (function_default) {
        serv->function_default = NULL;
        chunk_free(registry_function_chunk, function_default);
	registry_decrement_statistics(&registry_function_statistics,
				      sizeof(functype));
    }
}

/*
 * registry_add_pid_list
 *
 * Add a pid to a list of pids.  When the regitry is invoked, all
 * processes on the list will be sent the requested message.
 *
 * WARNING: Unlike the registry_list call, any processing that occurs
 * because of this registry call will happen on the stack of the
 * process that requested the notification, not on sender's stack.
 * This is a Good Thing (TM).  There is also no way to guarantee the
 * order in which the receivers wil wake up and process the message.
 * (Contrasted to registry_list where the receeivers code fragments
 * will always be executed in the same order every time.)
 */

boolean registry_add_pid_list (regtype_t  regcode,
			       ushort     servcode,
			       pid_t      pid,
			       char      *name)
{
    regservicetype *serv;
    functype *func;

    /*
     * Get pointer to service data structure
     * Make sure it is a list service.
     */
    serv = service_lookup(regcode, servcode, SERVICE_PID_LIST);
    if (!serv)
	return(FALSE);

    /*
     * First we see if the pid is already in the service
     */
    for (func = serv->function_queue; func; func = func->i.next) {
        if (func->j.value == pid)
            return(TRUE);
    }
 
    /*
     * New pid, register it.
     */
    func = chunk_malloc(registry_function_chunk);
    if (!func)
        return(FALSE);
    func->j.value = pid;
    registry_function_enqueue(&serv->function_queue, func);
    registry_increment_statistics(&registry_function_statistics,
				  sizeof(functype));
    REGISTRY_STRING_DEFINE(func->name = name;)
    return(TRUE);
}

/*
 * registry_delete_pid_list
 *
 * Delete a particular pid from a service
 */
void registry_delete_pid_list (regtype_t regcode, ushort servcode, pid_t pid)
{
    regservicetype *serv;
    functype *func;

    /*
     * Get pointer to service data structure
     * Make sure it is a list service.
     */
    serv = service_lookup(regcode, servcode, SERVICE_PID_LIST);
    if (!serv)
	return;

    /*
     * Find we see if the pid is already in the service
     */
    for (func = serv->function_queue; func; func = func->i.next) {
        if (func->j.value == pid)
            break;
    }
 
    /*
     * not found, nothing to do
     */
    if (func == NULL)
        return;
 
    registry_function_unqueue(&serv->function_queue, func);
    chunk_free(registry_function_chunk, func);
    registry_decrement_statistics(&registry_function_statistics,
				  sizeof(functype));

    return;
}

/*
 * registry_add_pid_case
 *
 * Add a pid to a case service. This routine is always invoked using
 * its index.
 *
 * WARNING: Unlike the registry_case call, any processing that occurs
 * because of this registry call will happen on the stack of the
 * process that requested the notification, not on sender's stack.
 * This is a Good Thing (TM).
 */

boolean registry_add_pid_case (regtype_t  regcode,
			       ushort     servcode,
			       long       index,
			       pid_t      pid,
			       char      *name)
{
    regservicetype *serv;
    functype *func;
    leveltype level;

    /*
     * Get pointer to registry data structure
     */
    serv = service_lookup(regcode, servcode, SERVICE_CASE);
    if (!serv)
	return(FALSE);

    level = raise_interrupt_level(ALL_DISABLE);

    if ((serv->function_table) &&
        ((index >= 0) && (index < serv->maxentries))) {
        func = &serv->function_table[index];
        func->i.assigned = TRUE;
    } else {
        /*
         * Check to see if we're only changing an entry
         */
        for (func = serv->function_queue; func; func = func->i.next) {
            if (func->index == index) {
                registry_delete_pid_case(regcode, servcode, index);
                break;
            }
        }
 
        func = chunk_malloc(registry_function_chunk);
        if (!func) {
            reset_interrupt_level(level);
            (*kernel_errmsg)(&msgsym(NOMEMORY, SYS),
			     "registry_add_case", servcode);
            return(FALSE);
        }
	registry_increment_statistics(&registry_function_statistics,
				      sizeof(functype));
        registry_function_enqueue(&serv->function_queue, func);
    }
    func->j.value = pid;
    func->index = index;
    REGISTRY_STRING_DEFINE(func->name = name;)
    reset_interrupt_level(level);
    return(TRUE);
}

/*
 * registry_delete_pid_case
 *
 * Delete a particular pid from a service
 */
void registry_delete_pid_case (regtype_t regcode, ushort servcode, long index)
{
    registry_delete_case(regcode, servcode, index);
}

/*
 * function_execute
 * A local function to execute a registered function
 */

/*
 *      Warning:  violence has been done to type checking.
 *
 * We register functions as anonymous pointers, i.e. "void *".
 * ANSI C requires that we give these pointers some sort of type when
 * we dereference them as functions.  That is the point of the following
 * typedefs used solely by function_execute().  If you feel the desire
 * to use these typedefs elsewhere, you are making a marginal situation
 * worse.
 * 
 * Also, because some called functions may indeed return a value (most
 * do not), we pretend to return a value, in particular, a long, when
 * in fact the function may be void or passing back a pointer.
 *
 * I realize doing it this way circumvents prototyping, which we all know
 * is A Bad Thing.  Perhaps at some later date when registry is a mature
 * part of the system, some enterprising person could remedy this defect.
 */

typedef long (*reg_t0)(void);
typedef long (*reg_t1)(char *);
typedef long (*reg_t2)(char *,char *);
typedef long (*reg_t3)(char *,char *,char *);
typedef long (*reg_t4)(char *,char *,char *,char *);
typedef long (*reg_t5)(char *,char *,char *,char *,char *);
typedef long (*reg_t6)(char *,char *,char *,char *,char *,char *);
typedef long (*reg_t7)(char *,char *,char *,char *,char *,char *,char *);
typedef long (*reg_t8)(char *,char *,char *,char *,char *,char *,char *,
		       char *);
typedef long (*reg_t9)(char *,char *,char *,char *,char *,char *,char *,
		       char *,char *);
typedef long (*reg_t10)(char *,char *,char *,char *,char *,char *,char *,
			char *,char *,char *);


static inline long function_execute_inline (regservicetype *serv,
					    functype *func,
					    va_list argp)
{
    char *data1, *data2, *data3, *data4, *data5, *data6, *data7, *data8,
         *data9, *data10;
    long retval;

    switch (serv->numargs) {
      case SERVICE_0ARG:
	return((*((reg_t0)func->j.routine))());
	break;
      case SERVICE_1ARG:
	data1 = va_arg(argp, char *);
	return((*((reg_t1)func->j.routine))(data1));
	break;
      case SERVICE_2ARG:
	data1 = va_arg(argp, char *);
	data2 = va_arg(argp, char *);
	retval = (*((reg_t2)func->j.routine))(data1, data2);
	return(retval);
	break;
      case SERVICE_3ARG:
	data1 = va_arg(argp, char *);
	data2 = va_arg(argp, char *);
	data3 = va_arg(argp, char *);
	return((*((reg_t3)func->j.routine))(data1, data2, data3));
	break;
      case SERVICE_4ARG:
	data1 = va_arg(argp, char *);
	data2 = va_arg(argp, char *);
	data3 = va_arg(argp, char *);
	data4 = va_arg(argp, char *);
	return((*((reg_t4)func->j.routine))(data1, data2, data3, data4));
	break;
      case SERVICE_5ARG:	/* ping parsing is baroque */
	data1 = va_arg(argp, char *);
	data2 = va_arg(argp, char *);
	data3 = va_arg(argp, char *);
	data4 = va_arg(argp, char *);
	data5 = va_arg(argp, char *);
	return((*((reg_t5)func->j.routine))(data1, data2, data3, data4,
					    data5));
	break;
      case SERVICE_6ARG:
	data1 = va_arg(argp, char *);
	data2 = va_arg(argp, char *);
	data3 = va_arg(argp, char *);
	data4 = va_arg(argp, char *);
	data5 = va_arg(argp, char *);
	data6 = va_arg(argp, char *);
	return((*((reg_t6)func->j.routine))(data1, data2, data3, data4,
					     data5, data6));
	break;
      case SERVICE_7ARG:	/* printf is a pig */
	data1 = va_arg(argp, char *);
	data2 = va_arg(argp, char *);
	data3 = va_arg(argp, char *);
	data4 = va_arg(argp, char *);
	data5 = va_arg(argp, char *);
	data6 = va_arg(argp, char *);
	data7 = va_arg(argp, char *);
	return((*((reg_t7)func->j.routine))(data1, data2, data3, data4,
					     data5, data6, data7));
	break;
      case SERVICE_8ARG:
	data1 = va_arg(argp, char *);
	data2 = va_arg(argp, char *);
	data3 = va_arg(argp, char *);
	data4 = va_arg(argp, char *);
	data5 = va_arg(argp, char *);
	data6 = va_arg(argp, char *);
	data7 = va_arg(argp, char *);
	data8 = va_arg(argp, char *);
	return((*((reg_t8)func->j.routine))(data1, data2, data3, data4,
					     data5, data6, data7, data8));
	break;
      case SERVICE_9ARG:
	data1 = va_arg(argp, char *);
	data2 = va_arg(argp, char *);
	data3 = va_arg(argp, char *);
	data4 = va_arg(argp, char *);
	data5 = va_arg(argp, char *);
	data6 = va_arg(argp, char *);
	data7 = va_arg(argp, char *);
	data8 = va_arg(argp, char *);
	data9 = va_arg(argp, char *);
	return((*((reg_t9)func->j.routine))(data1, data2, data3, data4,
					    data5, data6, data7, data8,
					    data9));
	break;
      case SERVICE_10ARG:
	data1 = va_arg(argp, char *);
	data2 = va_arg(argp, char *);
	data3 = va_arg(argp, char *);
	data4 = va_arg(argp, char *);
	data5 = va_arg(argp, char *);
	data6 = va_arg(argp, char *);
	data7 = va_arg(argp, char *);
	data8 = va_arg(argp, char *);
	data9 = va_arg(argp, char *);
	data10 = va_arg(argp, char *);
	return((*((reg_t10)func->j.routine))(data1, data2, data3, data4,
					     data5, data6, data7, data8,
					     data9, data10));
	break;
      default:
	break;
    }
    return(0L);
}

static long function_execute (regservicetype *serv, functype *func,
			      va_list argp)
{
    return(function_execute_inline(serv, func, argp));
}

/*
 * registry_case
 * Given an index code, execute a registered routine
 * If no matching code, execute the default routine, if we have one.
 * We can be called with zero to SERVICE_MAXARG arguments.
 *
 * Note: If you make any changes to this routine, you probably also
 * need to change registry_retval and registry_pid_case.
 */

void registry_case (regtype_t regcode, ulong servcode, long index, ...)
{
    regservicetype *serv;
    functype *func;
    va_list argp;

    va_start(argp, index);	/* varargs start after index */
    serv = service_lookup_inline(regcode, servcode, SERVICE_CASE, TRUE);
    if (!serv)
	return;

    /*
     * If we have a case table look there first.
     */
    if (serv->function_table) {
        if ((index >= 0) && (index < serv->maxentries)) {
            func = &serv->function_table[index];
            if (func->i.assigned) {
                function_execute_inline(serv, func, argp);
                return;
            }
        }
    }

    /*
     * Look in the function list for it
     */
    for (func = serv->function_queue; func; func = func->i.next) {
	if (func->index == index) {
	    function_execute_inline(serv, func, argp);
	    return;
	}
    
    }

    /*
     * Getting desparate. Have we a default?
     */
    if (serv->function_default) {
        function_execute(serv, serv->function_default, argp);
    }
    va_end(argp);
}

/*
 * registry_list
 * Execute all functions registered in this service.
 * We can be called with zero to SERVICE_MAXARG arguments.
 */

void registry_list (regtype_t regcode, ulong servcode, ...)
{
    regservicetype *serv;
    functype *func;
    va_list argp;

    va_start(argp, servcode);	/* varargs start after servcode */
    serv = service_lookup_inline(regcode, servcode, SERVICE_LIST, TRUE);
    if (!serv)
	return;

    for (func = serv->function_queue; func; func = func->i.next)
        function_execute_inline(serv, func, argp);
 
    va_end(argp);
}

/*
 * registry_retval
 * Given an index code, execute a registered function and return a value.
 * If no matching code, execute the default function, if we have one.
 * We can be called with zero to SERVICE_MAXARG arguments.
 *
 * Note: If you make any changes to this routine, you probably also
 * need to change registry_case.
 */

long registry_retval (regtype_t regcode, ulong servcode, long index, ...)
{
    regservicetype *serv;
    functype *func;
    va_list argp;
    long retval;

    va_start(argp, index);	/* varargs start after index */
    serv = service_lookup_inline(regcode, servcode, SERVICE_CASE, TRUE);
    if (!serv)
	return(0L);

    /*
     * If we have a case table, look there.
     */
    if (serv->function_table) {
        if ((index >= 0) && (index < serv->maxentries)) {
            func = &serv->function_table[index];
            if (func->i.assigned) {
                retval = function_execute_inline(serv, func, argp);
                return(retval);
            }
        }
    }

    /*
     * Look in the function list for it
     */
    for (func = serv->function_queue; func; func = func->i.next) {
	if (func->index == index) {
	    retval = function_execute_inline(serv, func, argp);
	    return(retval);
	}
    }

    /*
     * Getting desparate. Have we a default?
     */
    if (serv->function_default) {
        retval = function_execute(serv, serv->function_default, argp);
        return(retval);
    }
    va_end(argp);
    return(0L);
}

/*
 * registry_value
 * Given an index code, return the value that is paired with the index.
 * If no matching code, return the default value, if we have one.
 * There should never be any arguments.
 */

ulong registry_value (regtype_t regcode, ulong servcode, long index)
{
    regservicetype *serv;
    functype *func;

    serv = service_lookup_inline(regcode, servcode, SERVICE_VALUE, TRUE);
    if (!serv)
	return(0L);

    /*
     * If we have a case table, look there.
     */
    if (serv->function_table) {
        if ((index >= 0) && (index < serv->maxentries)) {
            func = &serv->function_table[index];
            if (func->i.assigned) {
                return(func->j.value);
            }
        }
    }

    /*
     * Look in the function list for it
     */
    for (func = serv->function_queue; func; func = func->i.next) {
	if (func->index == index) {
	    return(func->j.value);
	}
    }

    /*
     * Getting desparate. Have we a default?
     */
    if (serv->function_default) {
        return(serv->function_default->j.value);
    }
    return(0L);
}

/*
 * registry_case_used
 * Given an index code, look to see if there is a function registered and
 * return TRUE if there is.
 */

boolean registry_case_used (regtype_t regcode, ulong servcode, long index)
{
    regservicetype *serv;
    functype *func;

    serv = service_lookup_inline(regcode, servcode, SERVICE_CASE, TRUE);
    if (!serv)
	return(FALSE);

    /*
     * If we have a case table look there first.
     */
    if (serv->function_table) {
	if ((index >= 0) && (index < serv->maxentries)) {
	    func = &serv->function_table[index];
	    return(func->i.assigned);
	}
    }

    /*
     * It wasn't in the case table.  Maybe it's in the linked list.
     * If no joy there, use the default case, if any
     */
    for (func = serv->function_queue; func; func = func->i.next) {
	if (func->index == index) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * registry_loop
 * Scan down a list of boolean functions.  Return TRUE after the first
 * function returns TRUE.  Return FALSE if no functions matched.
 * We can be called with zero to SERVICE_MAXARG arguments.
 */

boolean registry_loop (regtype_t regcode, ulong servcode, ...)
{
    regservicetype *serv;
    functype *func;
    va_list argp;

    va_start(argp, servcode);	/* varargs start after servcode */
    serv = service_lookup_inline(regcode, servcode, SERVICE_LIST, TRUE);
    if (!serv)
	return(FALSE);

    for (func = serv->function_queue; func; func = func->i.next) {
        if (function_execute_inline(serv, func, argp)) {
            return(TRUE);
        }
    }
    va_end(argp);
    return(FALSE);
}

/*
 * registry_stub
 * Execute a registered stub function.
 * Do nothing if nothing registered.
 * We can be called with zero to SERVICE_MAXARG arguments.
 */

long registry_stub (regtype_t regcode, ulong servcode, ...)
{
    regservicetype *serv;
    va_list argp;
    long retval;

    va_start(argp, servcode);	/* varargs start after code */
    serv = service_lookup_inline(regcode, servcode, SERVICE_STUB, TRUE);
    if (!serv)
	return(0L);

    if (serv->function_default) {
        retval = function_execute_inline(serv, serv->function_default, argp);
        return(retval);
    }
    va_end(argp);
    return(0L);
}

/*
 * registry_pid_list
 *
 * Walk a list of pid values, sending the same message to each
 * process.  This routine is meant for asynchronous notification.
 *
 * WARNING: Unlike the registry_list call, any processing that occurs
 * because of this registry call will happen on the stack of the
 * process that requested the notification, not on sender's stack.
 * This is a Good Thing (TM).  There is also no way to guarantee the
 * order in which the receivers wil wake up and process the message.
 * (Contrasted to registry_list where the receeivers code fragments
 * will always be executed in the same order every time.)
 */
void registry_pid_list (regtype_t regcode, ulong servcode, ulong signal, ...)
{
    regservicetype *serv;
    functype *func;
    va_list argp;
    void *pointer_arg;
    ulong numeric_arg;

    va_start(argp, signal);	/* varargs start after the signal */
    serv = service_lookup_inline(regcode, servcode, SERVICE_PID_LIST, TRUE);
    if (!serv)
	return;

    switch (serv->numargs) {
      case SERVICE_0ARG:
	pointer_arg = NULL;
	numeric_arg = 0;
	break;
      case SERVICE_1ARG:
	pointer_arg = va_arg(argp, void *);
	numeric_arg = 0;
	break;
      case SERVICE_2ARG:
	pointer_arg = va_arg(argp, void *);
	numeric_arg = va_arg(argp, ulong);
	break;
      default:
	return;
    }

    for (func = serv->function_queue; func; func = func->i.next)
        process_send_message(func->j.value, signal, pointer_arg, numeric_arg);
    va_end(argp);
    return;
}

/*
 * registry_pid_case
 *
 * Given an index code, send a message to the registered pid.  This
 * routine is meant for asynchronous notification. 
 *
 * WARNING: Unlike the registry_case call, any processing that occurs
 * because of this registry call will happen on the stack of the
 * process that requested the notification, not on sender's stack.
 * This is a Good Thing (TM).
 */
void registry_pid_case (regtype_t regcode,
			ulong     servcode,
			long      index,
			ulong     signal, ...)
{
    regservicetype *serv;
    functype *func;
    va_list argp;
    void *pointer_arg;
    ulong numeric_arg;

    va_start(argp, signal);	/* varargs start after signal */
    serv = service_lookup_inline(regcode, servcode, SERVICE_CASE, TRUE);
    if (!serv)
	return;

    switch (serv->numargs) {
      case SERVICE_0ARG:
	pointer_arg = NULL;
	numeric_arg = 0;
	break;
      case SERVICE_1ARG:
	pointer_arg = va_arg(argp, void *);
	numeric_arg = 0;
	break;
      case SERVICE_2ARG:
	pointer_arg = va_arg(argp, void *);
	numeric_arg = va_arg(argp, ulong);
	break;
      default:
	return;
    }

    /*
     * If we have a case table look there first.
     */
    if (serv->function_table) {
        if ((index >= 0) && (index < serv->maxentries)) {
            func = &serv->function_table[index];
            if (func->i.assigned) {
                process_send_message(func->j.value, signal, pointer_arg,
                                    numeric_arg);
                return;
            }
        }
    }
 
    /*
     * It wasn't in the case table.  Maybe it's in the linked list.
     * If no joy there, use the default case, if any
     */
    for (func = serv->function_queue; func; func = func->i.next) {
        if (func->index == index) {
            process_send_message(func->j.value, signal, pointer_arg,
                                numeric_arg);
            return;
        }
    }
    va_end(argp);
}

/*
 * registry_allocated
 *
 * Indicate whether or not a particular registry number has been allocated.
 */
boolean registry_allocated (regtype_t regcode)
{
    if ((regcode >= REG_MINCOUNT) && (regcode < REG_MAXCOUNT))
	return(registry_array[regcode].servicecount != 0);
    return(FALSE);
}

/*
 * registry_populated
 *
 * Indicate whether or not a particular registry number has been allocated.
 */
boolean registry_populated (regtype_t regcode)
{
    if ((regcode >= REG_MINCOUNT) && (regcode < REG_MAXCOUNT))
	return((registry_array[regcode].servicecount != 0) &&
	       registry_array[regcode].services);
    return(FALSE);
}
