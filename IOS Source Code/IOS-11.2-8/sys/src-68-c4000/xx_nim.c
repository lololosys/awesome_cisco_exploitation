/* $Id: xx_nim.c,v 3.4.56.2 1996/08/28 13:12:03 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c4000/xx_nim.c,v $
 *------------------------------------------------------------------
 * Dos XX specific NIM code
 *
 * 10/93, Lele Nardin
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: xx_nim.c,v $
 * Revision 3.4.56.2  1996/08/28  13:12:03  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.56.1  1996/03/18  21:59:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.22.1  1996/01/24  23:01:37  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/11/20  23:22:35  wmay
 * CSCdi43620:  Remove or shut virtual interfaces could crash the box -
 * virtual interfaces have slot and slotunit value of MAXUINT - check
 * for it.
 *
 * Revision 3.3  1995/11/17  18:43:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:31:22  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:25:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/20  07:18:24  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  22:55:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../les/msg_nim.c"		/* Not a typo, see logger.h */
#include "../les/if_les.h"


/*
 * Local Storage
 */
queuetype  *ichainQ = NULL;
queuetype  *ichaincacheQ = NULL;


/*
 * turn health light of nim on or off depending on state.
 */
void nim_health_xx_light (hwidbtype *idb)
{
    ulong      bitset = 1;
    hwidbtype *hwidb;
    boolean    light_led = TRUE;
    boolean    valid_idb = FALSE;

    if (idb->slot >= C4000_NUM_SLOTS) 
	return;
    /*
     * set proper bit depending on slot.
     * slot 1 is bit 4
     * slot 2 is bit 5
     * slot 3 is bit 6
     */
    bitset = bitset << (idb->slot + 4);
    
    /*
     * Run down the hwidbQ list and look at every interface that's on
     * this NIM slot. The LED gets turned off iff -
     *
     * o Any of the interfaces are Down and are expected to be Up
     * o All interfaces are Shutdown
     *
     */
    FOR_ALL_HWIDBS(hwidb) {
	/*
	 * Only count those IDB entries on the same slot as us.
	 */
	if (idb->slot != hwidb->slot)
	    continue;

	switch (hwidb->state) {
	    /*
	     * Cover cases where the LED shouldn't be lit
	     */
	  case IDBS_DOWN:
	  case IDBS_GOINGDOWN:
	  case IDBS_TESTING:
	  case IDBS_INIT:
	  case IDBS_RESET:
	    light_led = FALSE;
	    valid_idb = TRUE;
	    break;

	    /*
	     * Cover cases where the LED should be lit
	     */
	  case IDBS_UP:
	    valid_idb = TRUE;
	    break;

	    /*
	     * Cover null cases
	     */
	  case IDBS_ADMINDOWN:
	  default:
	    break;
	}
    }

    /*
     * Any interface that is down that hasn't explicitly been set
     * to ADMINDOWN, should cause the LED to be extinguished as
     * a warning to the user.
     */
    if ((light_led) && (valid_idb))
	*(volatile ulong *)ADRSPC_SHMEM_CONTROL &= ~bitset;
    else
	*(volatile ulong *)ADRSPC_SHMEM_CONTROL |= bitset;
}

/*
 * This function stuffs the address of a handler into the "offset'th"
 * vector for the slot. The handler will normally be a wrapper.
 */
void set_slot_vector (uint slot, uint offset, uint handler)
{
    exceptionHandler(XX_START_VECTOR + (slot * XX_VECTORS_PER_SLOT) + offset,
		     (void (*)(void))handler);
}

void set_slot_vector_range (uint slot,
			    uint start,
			    uint end,
			    uint function)
{
    int i;

    for (i = start; i < end; i++)
        set_slot_vector(slot, i, function);
}

/*
 * get the vector of the first interrupt routine for this slot
 */
ushort get_slot_vector (uint slot, uint offset)
{
    return((ushort)(XX_START_VECTOR + (slot * XX_VECTORS_PER_SLOT) + offset));
}


/*
 * Sets the idb to be used as an argument when calling from assembler wrappers
 */
void set_slot_idb (hwidbtype *idb)
{
    int offset;

    /*
     * Add a bit of bounds checking just to be paranoid
     */
    if (idb->slot >= C4000_NUM_SLOTS) {
	errmsg(&msgsym(BADSLOT, NIM), idb->slot);
	crashdump(0);
    }

    /*
     * Prod the idb into the relevant part of the idb array
     */
    switch (idb->subunit) {
      case XX_SUBUNIT_A:
      case XX_SUBUNIT_B:
      case XX_SUBUNIT_C:
      case XX_SUBUNIT_D:
	offset = (idb->slot * HWIDB_ENTRIES_PER_SLOT) + idb->subunit;
	nim_idb_array[offset] = idb;
	break;

      default:
	errmsg(&msgsym(BADSUBUNIT, NIM), idb->subunit, idb->slot);
	crashdump(0);
     }
}

/*
 * Interrupt Chain (ichain) Functions
 *
 * The following functions implement interrupt chaining for the DosXX
 * platform (i.e. An interrupt on a NIM will cause the other NIM's
 * subscribing to this scheme to be checked too).
 *
 * At high interrupt rates, this causes the basic interrupt driven
 * scheduling to stop and for a poll driven scheduler to take over
 * (because there'll always be an outstanding interrupt to handle)
 *
 */

/*
 * Scurries up the whole of the ichain jump table, resetting all
 * the entries to the default nim exit routine.
 */
static void reset_ichain_jump_table (void)
{
    uint              *jump_ptr;
    int                count;

    jump_ptr = (uint *)ICHAIN_TABLE_BASE;

    /*
     * Reset all the entries to the default exit routine
     */
    for (count = 0; count < ICHAIN_TABLE_SIZE; count++, jump_ptr++)
	*jump_ptr = (uint)nim_ichain_jump_exit;
}

/*
 * Initialise the ichain queue structures and pre-allocate memory.
 * This should be called *once* at startup
 */
void init_ichain_cache (void)
{
    int i;
    ichain_instance_t *ptr;

    /*
     * Allocate memory for the ichainQ structure (i.e. the bit that points to
     * the active ichain entries)
     */
    ichainQ = malloc(sizeof(queuetype));
    if (!ichainQ) {
	crashdump(0);
    }

    queue_init(ichainQ, ICHAIN_MAX_ENTRIES);

    /*
     * Allocate memory for the ichainQ structure (i.e. the bit that points to
     * the cached memory for new ichain entries so we don't have to do allocs
     * and frees in the interrupt contexts)
     */
    ichaincacheQ = malloc(sizeof(queuetype));
    if (!ichaincacheQ) {
	crashdump(0);
    }

    queue_init(ichaincacheQ, ICHAIN_MAX_ENTRIES);

    for (i = 0; i < ICHAIN_MAX_ENTRIES; i++) {

	/*
	 * Malloc some memory to hold the ichain block
	 */
	ptr = malloc(sizeof(ichain_instance_t));
	if (!ptr) {
	    crashdump(0);
	}
	
	enqueue(ichaincacheQ, ptr);
    }

    /*
     * Clear the jump table to the default exit routine
     */
    reset_ichain_jump_table();
}

/*
 * Adds an idb block to the ichain list
 */
void add_idb_to_ichain (uint       slot,
			uint       subunit,
			hwidbtype *idb,
			uint       function)
{
    ichain_instance_t *ptr = NULL, *new_ptr = NULL;

    /*
     * Check to see if this is the first entry in the ichainQ.
     */
    if ((!ichainQ) || (!ichaincacheQ)) {
	errmsg(&msgsym(DEADICHAINQ, NIM), "add_idb_to_ichain");
	return;
    }

    /*
     * Don't try and add an entry for an idb pointer more than once
     */
    for (ptr = (ichain_instance_t *)ichainQ->qhead; ptr; ptr = ptr->next)
	if (ptr->idb == idb)
	    break;

    if (ptr) {

	/*
	 * It's already in here. Check to see if the function's
	 * different and if it is, replace and rebuild.
	 */

	if (ptr->function != function) {

	    ptr->function = function;
	    rebuild_ichain_jump_table();
	}

    } else {

	/*
	 * Get some memory to hold the ichain block
	 */
	new_ptr = dequeue(ichaincacheQ);

	if (!new_ptr) {
	    errmsg(&msgsym(NOMEMORY, SYS),
		   "ichain structure for slot",
		   idb->slot);
	    crashdump(0);
	}

	/*
	 * It's a brand spanking new entry...
	 */
	new_ptr->idb      = idb;
	new_ptr->function = function;
	new_ptr->slot     = slot;
	new_ptr->subunit  = subunit;

	enqueue (ichainQ, new_ptr);

	rebuild_ichain_jump_table();
    }
}

/*
 * Removes an ichain structure from the ichain list
 */
void remove_idb_from_ichain (hwidbtype *idb)
{
    ichain_instance_t *ptr = NULL, *prev_ptr = NULL;

    if ((!ichainQ) || (!ichaincacheQ)) {
	
       /*
	* Uh Oh! We tried to remove from a queue that didn't exist
	*/
	errmsg(&msgsym(DEADICHAINQ, NIM), "remove_idb_from_chain");
	return;
    }

    /*
     * The following bit of code basically does a selective dequeue() for
     * a given pointer value.
     */
    for (ptr = (ichain_instance_t *)ichainQ->qhead; ptr; ptr = ptr->next) {
	if (ptr->idb == idb)
	    break;

	prev_ptr = ptr;
    }

    if (ptr) {                              /* Did we find a dude? */
	if (prev_ptr) {                     /* It's not the head... */

	    prev_ptr->next = ptr->next;     /* Skip past it */

	    if (!prev_ptr->next)            /* Is ptr the tail? */
		ichainQ->qtail = prev_ptr;
	} else {
	                                    /* We're trashing the head */
	    ichainQ->qhead = ptr->next;

	    if (!ptr->next)                 /* Did we kill 'em all? */
		ichainQ->qtail = NULL;      /* No survivors */
	}

	ichainQ->count--;                   
	ptr->next = NULL;

	enqueue (ichaincacheQ, ptr);	    /* Cache for later use*/
    } else {

#ifdef DEBUG
       /*
	* Whoah! We shouldn't have idb guys in here we don't know
	*/
	errmsg(&msgsym(LOSTICHAINQ, NIM), "remove_idb_from_chain");
#endif
    }

    rebuild_ichain_jump_table();
}

/*
 * Builds the complete jump table for the ichain list
 */
void rebuild_ichain_jump_table (void)
{
    ichain_instance_t *ptr = NULL;
    uint              *jump_ptr;
    leveltype          status;
    uint               tableoffset;

    if (!ichainQ) {
	
       /*
	* Uh Oh! We tried to build from an uninitialized chain. Stupid.
	*/
	errmsg(&msgsym(DEADICHAINQ, NIM), "rebuild_ichain_jump_table");
	return;
    }

    /*
     * Play this really safe and disable all the interrupts whilst
     * we're messing with the jump table. Don't want any hyperspace
     * jumps here...
     */
    status = raise_interrupt_level(ALL_DISABLE);

    /*
     * Reset all the entries to the default exit routine
     */
    reset_ichain_jump_table();

    /*
     * Interrupt chaining won't take effect until you have more than 
     * 1 interface subscribed to it
     */
    if (ichainQ->count < 2) {
	reset_interrupt_level(status);
	return;
    }

    for (ptr = (ichain_instance_t *)ichainQ->qhead; ptr; ptr = ptr->next) {

	tableoffset = (ptr->slot * ICHAIN_SLOT_SIZE) + ptr->subunit;

	jump_ptr = (uint *)(ICHAIN_TABLE_BASE +
			   tableoffset * sizeof(int *));
	/*
	 * If there's an entry next in the list, chain to that.
	 * Else, chain back to the head routine to complete the loop
	 */
	if (ptr->next)
	    *jump_ptr = ptr->next->function;
	else
	    *jump_ptr = ((ichain_instance_t *)(ichainQ->qhead))->function;

#ifdef DEBUG
	printf ("Adding slot %d subunit %d at 0x%x for function 0x%x\n",
		ptr->slot,
		ptr->subunit,
		jump_ptr,
		*jump_ptr);
#endif
    }

    reset_interrupt_level(status);
}

/*
 * High IRQ dispatcher.
 *
 * It calls all hte interrupt handlers associated to all Slots requesting
 * a High IRQ level interrupt (in a sequence based on their priorities).
 */
void hi_irq_dispatcher (void)
{
    hi_irq_handler_t *hdlr_ptr;
    ulong status;

    /* Read Slot Interrupt Status Register */
    status = ~(*(volatile ulong *)ADRSPC_SHMEM_CONTROL) >> SHMEM_L4IRQ_ID;
    status &= ((1<<C4000_NUM_SLOTS) - 1); /* make and index out of it */

    /* Dispatch all slot interrupt handlers ONCE */
    while (status) {
        hdlr_ptr = hi_irq_dispatch_table[status];
        (* hdlr_ptr->func)(hdlr_ptr->arg);
        status &= hdlr_ptr->mask;
    }
}

