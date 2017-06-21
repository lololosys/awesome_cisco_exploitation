/* $Id: sierra_nim.c,v 3.4.56.2 1996/08/28 13:11:19 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c4000/sierra_nim.c,v $
 *------------------------------------------------------------------
 * Sierra specific NIM code
 *
 * 10/93, Lele Nardin
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: sierra_nim.c,v $
 * Revision 3.4.56.2  1996/08/28  13:11:19  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.56.1  1996/03/18  21:58:24  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.22.2  1996/03/09  05:11:55  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.22.1  1996/01/24  22:50:40  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/11/20  23:22:28  wmay
 * CSCdi43620:  Remove or shut virtual interfaces could crash the box -
 * virtual interfaces have slot and slotunit value of MAXUINT - check
 * for it.
 *
 * Revision 3.3  1995/11/17  18:41:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:30:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:22:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:53:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "logger.h"
#include "../les/msg_nim.c"		/* Not a typo, see logger.h */
#include "../les/if_les.h"


/*
 * Externs
 */
extern hi_irq_handler_t *hi_irq_dispatch_table[(1<<C4000_NUM_SLOTS)];

/*
 * turn health light of nim on or off depending on state.
 */
void nim_health_sierra_light (hwidbtype *idb)
{
    ushort     bitset = 1;
    hwidbtype *hwidb;
    boolean    light_led = TRUE;
    boolean    valid_idb = FALSE;
    
    if (idb->slot >= C4000_NUM_SLOTS)
	return;
    /*
     * Run down the hwidbQ list and look at every interface that's on
     * this NIM slot. The LED gets turned off if:
     *   Any of the interfaces are Down and are expected to be Up
     *   All interfaces are Shutdown
     *
     */
    for (hwidb = (hwidbtype*) hwidbQ->qhead;
	 hwidb != NULL;
	 hwidb = hwidb->next) {
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
     * set proper bit depending on slot.
     * slot 1 is bit 12
     * slot 2 is bit 13
     * slot 3 is bit 14
     */
    bitset = bitset << (idb->slot + SHMEM_SLOT1_BIT);

    if ((light_led) && (valid_idb))
	IO_ASIC->shmem_ctl &= ~bitset;
    else
	IO_ASIC->shmem_ctl |= bitset;
    flush_io_wbuffer();
}


/*
 * set_slot_idb:
 * Install the idb pointer into the appropriate element in
 * the idb array of the lev2_jumptable
 */
void set_slot_idb (hwidbtype *idb)
{
    lev2_jumptable_t *ptr;

    if (idb->slot >= C4000_NUM_SLOTS) {
	errmsg(&msgsym(BADSLOT, NIM), idb->slot);
	crashdump(0);
    } else {
	ptr = &lev2_jumptable[idb->slot];
	if (idb->subunit > SIERRA_SUBUNIT_H) {
	    errmsg(&msgsym(BADSUBUNIT, NIM), idb->subunit, idb->slot);
	    crashdump(0);
	} else {
	    ptr->idb[idb->subunit] = idb;
	}
    }
}

/*
 * set_slot_vector:
 * Install the wrapper pointer, the IACK register pointer
 * and the slot base address into the level 2 jumptable
 */
void set_slot_vector (uint slot, void (*handler)(void))
{
    lev2_jumptable_t *ptr;

    if (slot >= C4000_NUM_SLOTS) {
	errmsg(&msgsym(BADSLOT, NIM), slot);
	crashdump(0);
    }
    ptr = &lev2_jumptable[slot];
    ptr->wrapper = handler;
    ptr->slot_base_address = (void *)GET_SLOT_BASE(slot);
    ptr->slot_iack_reg = (uchar *)GET_IACK_ADDR(slot);
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
    status = ~(IO_ASIC->nim_stat) >> NMS_NIM_HIRQ_SH;
    status &= ((1<<C4000_NUM_SLOTS) - 1); /* make and index out of it */

    /* Dispatch all slot interrupt handlers ONCE */
    while (status) {
        hdlr_ptr = hi_irq_dispatch_table[status];
        (* hdlr_ptr->func)(hdlr_ptr->arg);
        status &= hdlr_ptr->mask;
    }
}


/* End of file */
 
