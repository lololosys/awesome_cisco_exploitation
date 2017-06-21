/* $Id: if_c4000_mcn.c,v 3.4.44.3 1996/08/28 12:58:07 thille Exp $ 
 * $Source: /release/112/cvs/Xsys/les/if_c4000_mcn.c,v $
 *------------------------------------------------------------------
 * if_c4000_mcn.c -- The Multi channel NIM specific code for the 4xxx.
 *
 * March 1994, Manoj Leelanivas.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file contains timer routines and generic mcn routines. The
 * routines in this file is generic to the core-board. Keep adapter
 * card specific modules out of this.*
 *------------------------------------------------------------------
 * $Log: if_c4000_mcn.c,v $
 * Revision 3.4.44.3  1996/08/28  12:58:07  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.44.2  1996/06/16  21:15:49  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.4.44.1  1996/03/18  20:41:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.1  1996/01/24  22:23:45  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/01/06  03:22:34  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/17  17:39:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:46  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/21  08:55:22  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.2  1995/06/19  06:47:24  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:34:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "subsys.h"
#include "sys_registry.h"
#include "if_les.h"
#include "if_c4000_mcn.h"
#include "if_c4000_m32.h"
#include "msg_bri.c"			/* Not a typo, see logger.h */
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

/*
 * Global declarations
 */
/* 
 * ptr to free pool queue for the mcn timer code 
 */
queuetype *McnTimerPoolQ = NULL;		

/*
 * array of pointers to mcn instance structures.
 */
mcn_instance_t *mcn_ds[C4000_NUM_SLOTS];


/*
 * =====================================================================
 * mcn_get_adapter_type -
 *
 * Description:
 * Get the port adapter type.
 *
 * Parameters:
 * slot:	slot # of 4000
 *
 * Returns:
 * adapter_type
 * =====================================================================
 */
uint mcn_get_adapter_type (uint slot)
{
    mcn_block_t *reg_ptr;
    ushort type;

    reg_ptr = (mcn_block_t *)GET_SLOT_BASE(slot);
    type = ((ushort) reg_ptr->adapter_type & 0x00FF);

    switch(type) {
    case MCN_PA_T1_U1:
         return MCN_PA_T1;
 
    case MCN_PA_E1_U1:
         return MCN_PA_E1;
 
    default:                /* for non T1/E1 adapters */
         return type;
    }    
}


/*
 * =====================================================================
 * 		deltaQ  routines for the MCN
 *	--------------------------------------------
 *			A deltaQ is a priority queue used to schedule
 * multiple requests for the same resource. In this case, upto 8
 * interfaces are competing for a counter to set a timeout. The deltaq
 * always maintains a sorted queue - ie the event which is to timeout
 * the earliest will be the head of the queue, the next earliest the
 * next and so on. The entries will carry time deltas(relative time wrt
 * to the previous entry's expiry).
 *
 * eg: assume a deltaq, -[2]-[1]-[6]......
 *     the head times out at t = 2, from now
 *     the next at t = 3, and the next at t = 9 ......
 *
 * If a new request comes with a timeout earlier than that of the head,
 * the new request becomes the head. The following are deq and enq functions
 * and all of them are called explicitly from level4 interrupt handler.
 * so interrupts are not disabled.
 *
 * All routines which use enQ and deQ ftions should make sure they
 * get elements from the McnTimerPoolQ for enQ and free it back to the
 * McnTimer pool after deQ.
 * =====================================================================
 */

/*
 * =====================================================================
 * 		Details of The 125 usecs counter on MCN nim
 *	    -------------------------------------------------
 *			(currently, the high register is not used
 * 			 for timeouts):
 *
 * [Counter enable]: 	  Should be done by the adapter code(one time).
 *			  Write 0000h to clear high register.
 *			  Enable the counter interrupt after setting it to
 * 			  high priority level4 interrupt.
 *			  Never disabled any time.
 *
 * [To start the counter]:Make sure the high register bits are clear.
 * 			  Write the timeout value into the low register.
 *			  This will start the timer.
 *
 * [To stop the counter]: Write 0000h into the high register. This will stop
 * 			  the counter and the low register will contain the
 *			  timeout remaining.
 *
 * [To restart counter]:  Write the timeout needed to low register.
 *
 * [To read the counter]: Always read the low register; the counter will
 * 			  keep ticking. If ever the high register is read,
 *			  the counter stops immediately, since the low
 *			  register should not give erroneous values.
 *
 * [When the counter interrupts]: Clear the interrupt(should be done by
 *				  the interrupt routine). Use overflow
 *				  register to find the lag.
 * =====================================================================
 */


/*
 * =====================================================================
 * mcn_deltaQ_enQ -
 *
 * Description:
 * This is a generic enQ routine for any deltaQ using the
 * on-board counter. This routine enQs a new element into the deltaQ based
 * on its timeout needed. This routine ensures that the element is added
 * in its proper place in the Q.
 *
 * Parameters:
 * slot :	which deltaQ to choose based on the slot #.
 * new  :	mcn_timerbuf_t * to the new element to be enqueued.
 *
 * Returns:
 * Nothing.
 *
 * Notes:
 * An enQ should never fail.
 * =====================================================================
 */
void mcn_deltaQ_enQ (uint slot,
		     mcn_timerbuf_t *new)
{
    mcn_timerbuf_t  *current = NULL, *prev = NULL, *head = NULL;
    ulong temp_time, t_remaining, timeout;	/* temp stores*/
    mcn_instance_t *ds;				/* mcn instance*/

    /*
     * get the mcn instance ptr.
     */
    ds = MCN_INSTANCE(slot);

    head = ds->q_head;		/* The current head in the deltaQ for */
				/* this slot*/
    timeout = new->timeout;	/* Get the timeout for the new element*/
    
    if (!head) {		/* fresh entry in deltaQ */
	new->next = NULL;       /* the only element, hence is the tail */
	new->delta = 0;		/* head does not have a delta */
	head = new;		/* so the new element is the head */

	/*
	 * start counter with timeout
	 */
        ds->ctr_regs->ctr_high = 0x0000;	/* just to make sure */
	ds->ctr_regs->ctr_low = timeout;        /* timeout in units of 125us*/
    } else {
	/*
	 * implies at least one element is present in the deltaQ
	 */
	t_remaining = ds->ctr_regs->ctr_low;	/* read current val */
	if (t_remaining > timeout) {	
	    /*
	     * head has to be pre-empted in favor of the new element.
	     */
	    ds->ctr_regs->ctr_high = 0x0000;	/* just to make sure */
	    ds->ctr_regs->ctr_low = timeout; 	/* new timeout */
	    new->delta = 0;
	    new->next  = head;
	    head->delta = t_remaining - timeout;
	    head = new;
	} else {
	    /*
	     * head remains where it is. insertion is somwhere in the
	     * Q after the head.
	     */
	    temp_time = t_remaining;
	    /*
	     * start loop to find out which location to insert new el.
	     */
	    current = head;
	    while (temp_time <= timeout) {
		prev = current;
		current = current->next;
		if (current) /* we have to check if current is null */
		    temp_time += current->delta;
		else
		    break;
	    }
	    /* Since we could have overshot in time calc. */
	    if (temp_time > timeout)
		temp_time -= current->delta;

	    /*
	     * Set the appropriate next pointers in the Q and deltas.
	     */
	    prev->next = new;
	    new->delta = timeout - temp_time;
	    new->next = current;
	    if (current)	/* only if non - null */
		current->delta = current->delta - (timeout - temp_time);
	}
    }
    ds->q_head = head;	/* in case the head got changed */
}


/*
 * =====================================================================
 * mcn_deltaQ_deQ_head -
 *
 * Description:
 * This routine is called on a level4 counter
 * interrupt. It deqs the head and returns a pointer to it. It also
 * starts the counter for the next element taking the overflow
 * register's value into consideration. However, if the next element
 * has a zero timeout, the counter is not set again. The routine calling
 * should ensure that all timed out elements are dequed, by calling this
 * routine in a loop till a NULL is returned.
 *
 * Parameters:
 * slot :      4000 slot # to determine which deltaQ?
 *
 * Returns:
 * an (mcn_timerbuf_t *) to the dequeued element. The calling routine
 * should free the ptr.
 * 
 * Notes:
 * Clearing the interrupt status is a bother of the routine which invokes
 * this.
 * =====================================================================
 */
mcn_timerbuf_t *mcn_deltaQ_deQ_head (uint slot)
{
    long temp_time;
    mcn_timerbuf_t *head, *temp_ptr;
    mcn_instance_t *ds;

    /*
     * get the mcn instance ptr
     */
    ds = MCN_INSTANCE(slot);

    /* find the head ptr */
    head = ds->q_head;

    if (!head) 	/* sanity check. should never happen */
	return(NULL);

    /*
     * Another sanity check, to see if counter is still running?
     * This means, it should not have interrupted...... or
     * the routine which uses the deq routine is calling it in a loop
     * to see if any timed out elements are present. If the counter
     * is running, there are no elements which have timed out and hence
     * return a NULL.
     */
    if (ds->ctr_regs->ctr_low) {
	return(NULL);	/* ignore the counter interrupt */
    }
   
    /*
     * Since the current head has expired. we have to start counter
     * for the next element, if present.
     */
    if (head->next) {	/* implies at least one element is present */
        /*
	 * Start next counter taking care of any overflow due to
	 * interrupt latency.
	 */
	temp_time = head->next->delta - ds->ctr_regs->overflow;

	/*
	 * start  counter only if the next element has a non-zero delta.
	 */
	ds->ctr_regs->ctr_high = 0x0000;	/* just to make sure */
        ds->ctr_regs->ctr_low = (temp_time > 0) ? temp_time: 0x0000;

	/*
	 * set proper pointers and delta.
	 */
	temp_ptr = head;
	head = head->next;
	head->delta = 0;	/* since the new head is currently running*/
    } else {
	/* only element was the head ; remove it */
	temp_ptr = head;
	head = NULL;
    }
    ds->q_head = head;		/* in case the head got changed */
    temp_ptr->next = NULL;	/* To ensure proper enQ to pool later */
    return(temp_ptr);		/* return ptr to dequeued element */
}


/*
 * =====================================================================
 * mcn_deltaQ_deQ_element -
 *
 * Description:
 * This routine is used to cancel timeouts.
 * It returns a ptr to the cancelled element.
 *
 * Parameters:
 * slot : 	4000 slot # to determine which deltaQ?
 * sequence :	The identifier which uniquely identifies a Q element.
 *
 * Returns:
 * A ptr to element with sequence same as i/p sequence. if
 * the element could not be found, then null ptr returned.
 *
 * Notes:
 * No check on the boundaries of input
 * values.
 * =====================================================================
 */
mcn_timerbuf_t *mcn_deltaQ_deQ_element (uint sequence, uint slot)
{
    ulong t_remaining;
    mcn_timerbuf_t *head, *temp_ptr, *prev = NULL, *current;
    mcn_instance_t *ds;

    /*
     * get the mcn instance ptr
     */
    ds = MCN_INSTANCE(slot);

    /* find the head ptr */
    head = ds->q_head;

    if (!head) 		/* sanity check. should never happen */
	return(NULL);

    /*
     * start searching from head to tail for the element.
     */
    current = head;
    while ((current) && (current->sequence != sequence)) {
	prev = current;
	current = current->next;
    }
    if (current) {		/* element has been found */
	if (current == head) {	/* so the head is the victim */
	    /*
	     * stop the timer on the counter and read the counter
	     */
	    t_remaining = ds->ctr_regs->ctr_high; /*junk read to stop counter*/
	    t_remaining = ds->ctr_regs->ctr_low;  /*read actual current val */

	    /*
	     * Start timer for next entry and set proper next ptr values.
	     */
	    temp_ptr = head;
	    head = head->next;
	    if (head) {
	        /*
	         * start new counter with new head's delta + old value,
	         * provided the new head is not null
	         */
	         ds->ctr_regs->ctr_high = 0x0000;	/* just to make sure */
                 ds->ctr_regs->ctr_low = head->delta + t_remaining;
	         head->delta = 0;
	    }
	} else {
	    prev->next = current->next;
	    if (current->next)	/* if the next element is not null */
		current->next->delta += current->delta;
	    temp_ptr = current;
	}
    } else
	return (NULL);	/* there is no element with this sequence*/

    ds->q_head = head;		/* in case the head got changed */
    temp_ptr->next = NULL;	/* To ensure proper enQ to pool later */
    return(temp_ptr);		/* ptr to the victim which is to be dequeued */
}


/*
 * =====================================================================
 * mcn_create_timer_pool -
 *
 * Description:
 * For all MCNs this preallocation routine
 * is called to create the global McnTimerPoolQ. Make sure this is called
 * only once during startup.
 *
 * Parameters:
 * none.
 *
 * Returns:
 * nothing.
 * =====================================================================
 */
static void mcn_create_timer_pool (void)
{
    int i;
    mcn_timerbuf_t *pool, *ptr, *ptr_next;
    
    /*
     * Allocate the McnTimerPoolQ Q.
     */
    McnTimerPoolQ = malloc(sizeof(queuetype));
    if (!McnTimerPoolQ) {
	crashdump(0);
    }

    queue_init(McnTimerPoolQ, MCN_MAX_TIMER_ENTRIES);

    /*
     * Allocate memory for MAX timerbuf_t entries in the McnTimerPoolQ Q
     * with a single malloc. Then enqueue each element into the
     * McnTimerPoolQ.
     */
    pool = malloc(sizeof(mcn_timerbuf_t) * MCN_MAX_TIMER_ENTRIES);
    if (!pool) {
	crashdump(0);
    }
    
    for (i = 0, ptr = pool; i < MCN_MAX_TIMER_ENTRIES; i++, ptr = ptr_next) {
	ptr_next = (mcn_timerbuf_t *) ((char *)ptr + sizeof(mcn_timerbuf_t));
	enqueue(McnTimerPoolQ, ptr);
    }
}

static ushort mcn_get_total_subcont (int slot)
{
    mcn_block_t *reg_ptr;
    ushort type;
    
    reg_ptr = (mcn_block_t *)GET_SLOT_BASE(slot);
    type = ((ushort) reg_ptr->adapter_type & 0x00FF);

    switch(type) {
     case MCN_PA_T1_U1:
     case MCN_PA_E1_U1:
          return 1;          /* single T1/E1 board */

     case MCN_PA_BRI_SINGLE:
     case MCN_PA_BRI_DUAL:
          return 1;          /* bri has its own way to implement the subcont */
 
     default:                /* empty or we don't know about it */
          return 0;
    }
}
 

/*
 * =====================================================================
 * mcn_analyze_interface -
 *
 * Description:
 * The general MCN startup routine which updates the
 * mcn_instance and set appropriate ptr to nim regs and counter regs.
 * called one time per slot.
 *
 * Parameters:
 * slot	: slot # for XX.
 *
 * Returns:
 * none
 * =====================================================================
 */
static void mcn_analyze_interface (int slot)
{
    mcn_instance_t *ds;
    ulong pcm_format, subcont, total_subcont;
    ushort i;
    
    /*
     * allocate the mcn instance for this slot.
     * mcn_ds is a global array.
     */
    ds = malloc_fast(sizeof(mcn_instance_t));
    if (!ds) {
        crashdump(0);
    }
    mcn_ds[slot] = ds;
    
    /*
     * Fill in the generic mcn info in the instance.
     */
    ds->mcn_regs = (mcn_block_t *)((uint) GET_SLOT_BASE(slot) +
                                   (uint) MCN_REG_OFFSET);
    ds->ctr_regs = (mcn_counter_block_t *)((uint) GET_SLOT_BASE(slot) +
                                           (uint) MCN_COUNTER_REG_OFFSET);
    ds->q_head = NULL;	/* timer head is initially null. */
    ds->mcn_cardtype = mcn_get_adapter_type(slot);
    if (ds->mcn_cardtype == MCN_PA_T1) {
        pcm_format = M32_AR_PCM_T1;
    } else {
        pcm_format = M32_AR_PCM_E1;
    }
    
    /*
     * Take the munich32 and adaptor cards out of reset. Wait
     * for approximately 1s to enable M32 TSP rising pulse.
     */
    ds->mcn_regs->reset_control = MCN_ENABLE_MASK;
    for (i = 0; i < 200; i++)
        usecdelay(5000);
    ds->mcn_regs->adapter2_vector = 0xff;
    ds->mcn_regs->adapter_type = 0xff;
    
    dsx1_pro_add(slot);
    /*
     * create timer pool of pre-allocated elements.
     */
    if (!McnTimerPoolQ)	/* needed only one time per box */
        mcn_create_timer_pool();

    ds->total_subcont = total_subcont = mcn_get_total_subcont(slot);
    for (subcont=0; subcont < total_subcont; subcont++) {
        /*
         * initialise the munich 32 hdlc controller.
         */
        ds->mcn_subcont[subcont].m32_ds = m32_startup(slot, subcont, pcm_format);
        if (ds->mcn_subcont[subcont].m32_ds == NULL) {
            char tmp[20];
            sprintf(tmp, "m32 startup at slot %d, subcontroller ", slot);
            errmsg(&msgsym(NOMEMORY,SYS), tmp, subcont);
            crashdump(0);
        }
        ds->mcn_subcont[subcont].m32_ds->runts_handle = dsx1_set_runts_handle;

        /*
         * Start the Port Adapter up for this slot
         */
        reg_invoke_analyze_mcn_port_adaptor(ds->mcn_cardtype, slot, subcont, 
                                            C4000_MCN_UNITS);
    }
}


/*
 * =====================================================================
 * mcn_subsys_init -
 *
 * Description:	MCN subsystem initialisation entry point
 *
 * Parameters:
 * subsys:	subsystype ptr.
 *
 * Returns:
 * Nothing.
 * 
 * =====================================================================
 */
static void mcn_subsys_init (subsystype *subsys)
{
    /*
     * Register the NIM ID for the MCN base card
     */
    reg_add_analyze_interface(XX_MCN,
			      mcn_analyze_interface,
			      "mcn_analyze_interface");
}

/*
 * MCN subsystem header
 */

#define MCN_MAJVERSION   1
#define MCN_MINVERSION   0
#define MCN_EDITVERSION  1

SUBSYS_HEADER(mcn, MCN_MAJVERSION, MCN_MINVERSION, MCN_EDITVERSION,
	      mcn_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);




