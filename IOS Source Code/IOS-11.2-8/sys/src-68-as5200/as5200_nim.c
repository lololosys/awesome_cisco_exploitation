/* $Id: as5200_nim.c,v 1.1.62.2 1996/08/28 13:11:53 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-68-as5200/as5200_nim.c,v $
 *------------------------------------------------------------------
 * as5200_nim.c - Routines for handling AS5200 NIMs
 *
 * December, 1995 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: as5200_nim.c,v $
 * Revision 1.1.62.2  1996/08/28  13:11:53  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.62.1  1996/06/16  21:19:31  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/20  00:08:09  jchlin
 * Placeholders for new development.
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
#include "../les/msg_nim.c"		/* Not a typo, see logger.h */
#include "../les/if_les.h"
#include "../les/if_as5200_mcn.h"
/*
 * Variables
 */
hi_irq_handler_t  hi_irq_handler_table[BRASIL_MAX_SLOTS+1];
hi_irq_handler_t *hi_irq_dispatch_table[(1<<BRASIL_MAX_SLOTS)];
 


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
    static int pri_t1_e1_card_count = 0;

    /*
     * Look at each slot and start a driver
     */
    for (slot = 0; slot < BRASIL_MAX_SLOTS; slot++) {
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
            if (type == BRASIL_NIM_MCN) {
                if (pri_t1_e1_card_count) {
                    buginf("Error: This system doesn't allow "
                           "more than 1 T1/E1/PRI card inserted\n");
                    type = BRASIL_NIM_UNKNOWN;
                }
                pri_t1_e1_card_count++;
            }
            if (mcn_get_adapter_type(slot) == MCN_PA_E1)
               continue;

            /*
             * bit0: PLL reset = 1
             * bit2-1: FSEL2 & 1 = 10 for 1.544Mhz
             * bit3: RSEL = 0 for state machine control
             * bit5-4:MS2 & 1 = 11 for automatic mode
             */
            *(ushort *)ADRSPC_BRASIL_TDM_PLL = 0x35;
            reg_invoke_analyze_interface(type, slot);
	}
    }
}

uint get_slot_number(void)
{
    uint         slot_num, slot;
    ushort       type;

    slot_num = 0xff;
    for (slot = 0; slot < BRASIL_MAX_SLOTS; slot++) {
        /*
         * Try to find which slot the T1 card is in
         */
        if (nim_exists(slot)) {
            /*
             * Get the NIM type from the NIM ID block
             */
            type = nim_get_type(slot);
            if (type == BRASIL_NIM_MCN) {
                slot_num = slot;
                break;
            }
        }
    }

    if (slot_num == 0xff) {
        buginf("******bogus level 6 interrupt occurred********\n");
    }
    return(slot_num);
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
    if (class == SUBSYS_CLASS_DRIVER) {
        /*
         * Create and initialize the High IRQ interrupt dispatcher
         */
        nim_init_hi_irq_handler();
	nim_analyze();
    }
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
    switch (BRASIL_BOARD_ID(slot)) {
    case BRASIL_T1_DPRI_PLUS_CSU:
    case BRASIL_E1_DPRI_PLUS_MODEMS:
         type = BRASIL_NIM_MCN;
         break;

    case BRASIL_24_ASYNC:
         type = BRASIL_NIM_ASYNC;
         break;
                     
    case BRASIL_24_ASYNC_PLUS_MODEMS:
         type = BRASIL_NIM_ASYNC_MODEM;
         break;
                
    case BRASIL_COMPRESSION:
         type = BRASIL_NIM_COMPRESSION;
         break;

    default:
         type = BRASIL_NIM_UNKNOWN;
    }

    return type;
}

/*
 * nim_get_version
 *
 * Get the NIM version of the module in the specified slot
 */

ushort nim_get_version (int slot)
{
    /*
     * Get the NIM version code. It's always the second byte of the NIM
     * control address space
     */
    return BRASIL_BOARD_REV(slot);
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
    return BRASIL_CARD_PRESENT(slot);
}


/*
 * High IRQ dispatcher.
 *
 * It calls all the interrupt handlers associated to all Slots requesting
 * a High IRQ level interrupt (in a sequence based on their priorities).
 */
void hi_irq_dispatcher (register hwidbtype *idb)
{
    hi_irq_handler_t *hdlr_ptr;
    isr_handler_t    *handler;
    short             status;
 
    /* Read Slot Interrupt Status Register */
    status = *(volatile ushort *)ADRSPC_BRASIL_ISR1_REG;
    status &=  ((1<<BRASIL_MAX_SLOTS) - 1);
    status ^= ((1<<BRASIL_MAX_SLOTS) - 1);
 
    /* Dispatch all slot interrupt handlers ONCE */
    while (status) {
        hdlr_ptr = hi_irq_dispatch_table[status];        
        status  &= hdlr_ptr->mask;
        for (handler = hdlr_ptr->handler_list;
             handler->func;
             handler++)
            (*handler->func)(handler->arg);
    }
}


/*
 * nim_init_hi_irq_handler
 *
 * Initialize the high IRQ interrupt tables and register the interrupt
 *  dispatcher.
 */

void nim_init_hi_irq_handler (void)
{
    ushort i, j;
 
    for (i=0; i<=BRASIL_MAX_SLOTS; i++) {
        hi_irq_handler_table[i].mask = ~(1 << i);
        for (j=0; j <= MAX_LEVEL4_ISR_PER_SLOT; j++) {
            hi_irq_handler_table[i].handler_list[j].func = NULL;
            hi_irq_handler_table[i].handler_list[j].arg = 0;
            hi_irq_handler_table[i].handler_list[j].prior = AS5200_NIM_HI_IRQ_PRIOR_ZERO;
        }
    }
 
    for (i=0; i<(1<<BRASIL_MAX_SLOTS); i++)
        hi_irq_dispatch_table[i] = &hi_irq_handler_table[BRASIL_MAX_SLOTS];
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
				  nim_hi_irq_type_t type,
				  ushort  arg)
{
    ushort index, temp;
    ushort i, priority;
    ushort index_mask;
    struct hi_irq_handler_struc *cur_ptr;
    struct hi_irq_handler_struc *hi_prior_ptr;
    isr_handler_t *handler;
    void   *func_temp;
 
    /* Check arguments */
    if (slot>=BRASIL_MAX_SLOTS || !func)
        return;
 
    /* Assign priority according to NIM type */
    if (type == NIM_HI_IRQ_M32)
        priority = AS5200_NIM_HI_IRQ_PRIOR_M32;
    else if ((type == NIM_HI_IRQ_T1) || (type == NIM_HI_IRQ_E1))
        priority = AS5200_NIM_HI_IRQ_PRIOR_FRAMER;
    else
        priority = AS5200_NIM_HI_IRQ_PRIOR_LOW;

    /* Fill slot table entry */
    for (i=0, handler=hi_irq_handler_table[slot].handler_list; 
         i < MAX_LEVEL4_ISR_PER_SLOT; 
         i++, handler++) {
        if (!handler->func) {
            handler->func = func;
            handler->arg = arg;
            handler->prior = priority;
            break; /* found an empty link */
        }

        if (handler->func == func)
            return;   /* already initialized */

        if (handler->prior < priority) {
            /* swap the information */
            func_temp =  handler->func;
            handler->func = func;
            func = func_temp;
            temp = handler->arg;
            handler->arg = arg;
            arg = temp;
            temp = handler->prior;
            handler->prior = priority;
            priority = temp;
            /* continue to propagate to the next link */
        }
    }

    if (i == MAX_LEVEL4_ISR_PER_SLOT) {
        buginf("\nError: nim_register_hi_irq_handler vector table is overflowedl\n");
        return; 
    }

    /* Update dispatching table according to new priorities */
    for (index=1; index<(1<<BRASIL_MAX_SLOTS); index++) {
        hi_prior_ptr = hi_irq_dispatch_table[index]; /* current setting */
        for (i=0, index_mask=1; i<BRASIL_MAX_SLOTS; i++, index_mask<<=1) {
            if (index & index_mask) {
                cur_ptr = &hi_irq_handler_table[i];
                if (cur_ptr->handler_list[0].prior > hi_prior_ptr->handler_list[0].prior)
                    hi_prior_ptr = cur_ptr;
            }
        }
        hi_irq_dispatch_table[index] = hi_prior_ptr;
    }
}

