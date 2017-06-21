/* $Id: c4000_nim.c,v 3.3.62.2 1996/08/28 13:12:42 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-c4000/c4000_nim.c,v $
 *------------------------------------------------------------------
 * c4000_nim.c - General purpose routines for handling 4000/4500 NIMs
 *
 * February 1995, Scott Mackie (moved from init_c4000.c)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c4000_nim.c,v $
 * Revision 3.3.62.2  1996/08/28  13:12:42  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.1  1996/03/18  22:00:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  23:02:54  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  18:45:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:32:27  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:26:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:31:01  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  22:56:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "packet.h"
#include "subsys.h"
#include "stacks.h"
#include "exception.h"
#include "logger.h"
#include "../les/if_les.h"
#include "../les/init_les.h"
#include "../les/msg_nim.c"		/* Not a typo, see logger.h */

/*
 * Variables
 */
hi_irq_handler_t hi_irq_handler_table[C4000_NUM_SLOTS + 1];
hi_irq_handler_t *hi_irq_dispatch_table[(1<<C4000_NUM_SLOTS)];


/*
 * nim_analyze
 *
 * Run down the NIM slots and start the relevent NIM driver, if present and
 * registered with the system
 */

static void nim_analyze (void)
{
    ushort type;
    int    slot;

    /*
     * Look at each slot and start a driver
     */
    for (slot = 0; slot < C4000_NUM_SLOTS; slot++) {
	/*
	 * Anyone there?
	 */
	if (nim_exists(slot)) {
	    /*
	     * Get the NIM type from the NIM ID block
	     */
	    type = nim_get_type(slot);

	    /*
	     * Start it up
	     */
	    reg_invoke_analyze_interface(type, slot);
	}
    }
}

/*
 * nim_subsys_init_class
 *
 * Callback routine from the subsys_class_init() call
 *
 */

void nim_subsys_init_class (ushort class)
{
   /*
    * If we've just started the driver subsystems, start the NIMs up
    * for real.
    */
    if (class == SUBSYS_CLASS_DRIVER)
	nim_analyze();
}

/*
 * nim_get_type
 *
 * Get the NIM type of the module in the specified slot
 */

ushort nim_get_type (int slot)
{
    ushort type;
    
    /*
     * Get the NIM type code. It's always the first byte of the NIM
     * control address space
     */
    type = GETSHORT(GET_SLOT_BASE(slot)) & 0xFF;
    return(type);
}

/*
 * nim_get_version
 *
 * Get the NIM version of the module in the specified slot
 */

ushort nim_get_version (int slot)
{
    ushort version;
    
    /*
     * Get the NIM version code. It's always the second byte of the NIM
     * control address space
     */
    version = GETSHORT(GET_SLOT_BASE(slot)+1) & 0x00FF;
    return(version);
}

/*
 * nim_exists
 *
 * Verifies if there is a device on a specific slot, by using touch_device().
 * Higher level of abstraction is needed to remove platform dependencies
 * from common routines.
 */

boolean nim_exists(int slot)
{
    return(touch_device(GET_SLOT_BASE(slot)));
}

/*
 * nim_get_slot_hardware_address
 *
 * Returns the valid address from the cookie range
 */

void nim_get_slot_hardware_address (hwidbtype *idb, uchar *addr)
{
    charlong host;
    uchar *ptr;
    int i, offset;
 
    /*
     * Copy the base hardware address into addr:
     */
    ptr = cookie->ether0_hw_addr;
    for (i = 0; i < IEEEBYTES; i++)
	addr[i] = *ptr++;
    
    offset = idb->slot + (idb->subunit * C4000_NUM_SLOTS);
	
    if (offset < C4000_MAX_COOKIE_ADDRESS) {
	/*
	 * If offset is greater than zero then we have to get funky with
	 * the address in order to avoid address collision
	 */
	if (offset) {
	    
	    host.d.byte[0] = 0;	/* Copy the 24 bit host number from cookie */
	    for (i = 1; i < 4; i++)
		host.d.byte[i] = cookie->ether0_hw_addr[i + 2];

	    host.d.lword += offset;
	    
	    /*
	     * Copy incremented host number to addr
	     */
	    for (i = 1; i < 4; i++) 
		addr[i + 2] = host.d.byte[i];
	}
    } else {
	/*
	 * We're all out of valid addresses. Crash and burn.
	 */
	errmsg(&msgsym(NOHWADDRESS, NIM), C4000_MAX_COOKIE_ADDRESS);
	crashdump(0);
    }
}

/*
 * Null function called when a spurious interrupt is generated.
 */
static void null_func (ushort arg)
{
}

/*
 * nim_init_hi_irq_handler
 *
 * Initialize the high IRQ interrupt tables and register the interrupt
 *  dispatcher.
 */

void nim_init_hi_irq_handler (void)
{
    ushort i;

    for (i=0; i<=C4000_NUM_SLOTS; i++) {
        hi_irq_handler_table[i].func = null_func;
        hi_irq_handler_table[i].arg = 0;
        hi_irq_handler_table[i].mask = ~(1 << i);
        hi_irq_handler_table[i].prior = C4000_NIM_HI_IRQ_PRIOR_ZERO;
    }

    for (i=0; i<(1<<C4000_NUM_SLOTS); i++)
        hi_irq_dispatch_table[i] = &hi_irq_handler_table[C4000_NUM_SLOTS];

    createlevel (HIRQ_INTLEVEL, hi_irq_dispatcher, "High IRQ Int Handler");
}

/*
 * nim_register_hi_irq_handler
 *
 * Interrupt Handlers are assigned priorities based on the NIM
 * type. Each dispatch table entry contains a pointer to the
 * interrupt handler table entry which describes the function
 * (associated to the highest priority NIM) to be called when
 * the interrupt mask bits assume the combination which indexes
 * the dispatch table.
 */

void nim_register_hi_irq_handler (ushort  slot,
				  void   *func,
				  int     type,
				  ushort  arg)
{
    ushort index;
    ushort i;
    ushort index_mask;
    struct hi_irq_handler_struc *cur_ptr;
    struct hi_irq_handler_struc *hi_prior_ptr;

    /* Check arguments */
    if (slot>=C4000_NUM_SLOTS || !func)
        return;

    /* Fill slot table entry */
    hi_irq_handler_table[slot].func = func;
    hi_irq_handler_table[slot].arg = arg;

    /* Assign priority according to NIM type */
    if (type == IDBTYPE_BSI)
        hi_irq_handler_table[slot].prior = C4000_NIM_HI_IRQ_PRIOR_BSI;
    else if (type == IDBTYPE_BRI)
        hi_irq_handler_table[slot].prior = C4000_NIM_HI_IRQ_PRIOR_BRI;
    else if ((type == IDBTYPE_E1) || (type == IDBTYPE_T1))
	hi_irq_handler_table[slot].prior = C4000_NIM_HI_IRQ_PRIOR_BRI;
    else
        hi_irq_handler_table[slot].prior = C4000_NIM_HI_IRQ_PRIOR_LOW;

    /* Update dispatching table according to new priorities */
    for (index=1; index<(1<<C4000_NUM_SLOTS); index++) {
        hi_prior_ptr = hi_irq_dispatch_table[index]; /* current setting */
        for (i=0, index_mask=1; i<C4000_NUM_SLOTS; i++, index_mask<<=1) {
            if (index & index_mask) {
                cur_ptr = &hi_irq_handler_table[i];
                if (cur_ptr->prior > hi_prior_ptr->prior)
                    hi_prior_ptr = cur_ptr;
            }
        }
        hi_irq_dispatch_table[index] = hi_prior_ptr;
    }
}

