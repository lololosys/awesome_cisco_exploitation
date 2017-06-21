/* $Id: if_c4000_mb.c,v 3.4.44.6 1996/09/05 01:16:39 snyder Exp $ 
 * $Source: /release/112/cvs/Xsys/les/if_c4000_mb.c,v $
 *------------------------------------------------------------------
 * if_c4000_mb.c -- The Multi BRI/TP3420 code for the 4xxx.
 *
 * March 1994, Manoj Leelanivas.
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_c4000_mb.c,v $
 * Revision 3.4.44.6  1996/09/05  01:16:39  snyder
 * CSCdi68132:  declare consts in lapb x25 atm_arp tcl
 *              save 248 out of data, 12 from image
 * Branch: California_branch
 *
 * Revision 3.4.44.5  1996/08/28  12:58:04  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.44.4  1996/06/20  21:13:04  rbeach
 * CSCdi58118:  BRI: Switchtype has to be configured before restart.
 * This error message is sent out during system initialization based
 * on the state of the isdn_process_up boolean. This message should
 * only be sent out if system_configuration has completed.
 * Branch: California_branch
 *
 * Revision 3.4.44.3  1996/06/16  21:15:43  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.4.44.2  1996/06/12  16:50:48  wmay
 * CSCdi39956:  BRI: show int stat and show int accounting only show D
 * channel info - put b channel idbs in the idb list.  This removes a
 * lot of special initialization code for BRI.  Also, make the names
 * of the b-channels more consisted with PRI (ie: BRI0:1 and BRI0:2).
 * Branch: California_branch
 *
 * Revision 3.4.44.1  1996/03/18  20:41:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.2  1996/03/13  01:49:23  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.12.1  1996/01/24  22:23:38  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/01/06  03:22:32  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/17  17:39:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:41  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/21  08:55:20  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.2  1995/06/19  06:47:23  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:34:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * This file contains the modules related to the multi-BRI NIM for the
 * 4XXX. The multi-BRI NIM supports a maximum of two BRI port adapters,
 * each containing 4 BRI ports. So a BRI NIM can have a maximum of 8
 * bri interfaces and a minimum of 4 bri interfaces. Most of the functions
 * in this file are closely tied with the S/T interface chip - TP3420.
 * The modules in this file are:
 *
 * 	BRI driver interface with MBRI
 * 	------------------------------
 * 	mb_startup()
 * 	mb_tp3420_init()
 * 	mb_tp3420_enable_line()
 * 	mb_tp3420_disable_line()
 * 	mb_tp3420_deactivate_line()
 * 	mb_interrupt()
 * 	mb_select_master_clock()
 * 	mb_write_sid()
 *
 * 	ISDN interface with MBRI
 * 	------------------------
 * 	isdn_enable_bri_channel()
 * 	isdn_disable_bri_channel()
 * 	isdn_bri_enable_layer1_Q()
 * 	isdn_activate_line()
 *     
 *      User interface command 
 *      ----------------------
 *      mb_set_master_clock()
 *      mb_set_loop_current()
 */
#include "master.h"
#include "interface_private.h"
#include "../ui/debug.h"
#include "logger.h"
#include "config.h"
#include "if_les.h"
#include "../if/bri_debug.h"
#include "if_c4000_mcn.h"
#include "isdn_bri.h"
#include "if_c4000_bri.h"
#include "if_c4000_mb.h"
#include "if_les_bri.h"
#include "../os/timer_generic.h"
#include "msg_bri.c"			/* Not a typo, see logger.h */

/*
 * Static declarations.
 */

/*
 * Error messages
 */
static const char timer_pool_msg[] = "MCN Timer Pool empty.";

/*
 * The multi bri slot can have max 8 interfaces: 0 to 7. Below are the
 * OR masks for enabling each interface and the AND masks for
 * disabling each interface. The high byte is a dont care.
 */
static const ushort MBRI_ENABLE_MASK[] = {
                                           0x0001, 0x0002, 0x0004, 0x0008,
                                           0x0010, 0x0020, 0x0040, 0x0080
                                         };
static const ushort MBRI_DISABLE_MASK[] = {
                                           0x00FE, 0x00FD, 0x00FB, 0x00F7,
                                           0x00EF, 0x00DF, 0x00BF, 0x007F
                                          };

/*
 * The loop current support is global. ie, either all bri interfaces
 * in the box have this support or none at all. So, if a global
 * command is implemented in the parser in future, to change the loop
 * current mode, it will take effect only after a reload.
 * (similar to the switchtype scenario).
 */
static boolean loop_current_mode = FALSE;

/*
 * This flag indicates whether the isdn process has been spawned or
 * not. This is set when isdn process invokes isdn_bri_enable_layer1_Q().
 */
static boolean isdn_process_up = FALSE;


/*
 * =====================================================================
 * mb_write_sid -
 *
 * Description: This routine writes an SID command to the TP3420 SID.
 * This returns only after the completion of the command. To read SID
 * status also, this routine is invoked with a NOP command.
 *
 * Parameters:
 * slot:	 slot # of XX.
 * subunit:	 which bri subunit in that slot. (0 -7)
 * command: 	 TP3420 SID command.
 *
 * Returns:
 * nothing
 *
 * Notes: The maximum spin wait is 3.5 usecs. Note that to read
 * the SID status we write a NOP. Also, any status indication that
 * could be lost is stored in ds->last_sid[subunit], whenever any
 * command is written. And we do force an interrupt in such a case
 * when an interrupt status came in while writing a command.
 *
 * Though there are 4 separate command registers per adapter logically,
 * there is only one command register in the h/w per adapter.
 *
 * =====================================================================
 */
static void mb_write_sid (uint slot, uint subunit, uchar command)
{
    ulong count;
    ushort enable_status;
    uchar sid_status;
    mb_instance_t *ds;

    ds = MB_INSTANCE(slot, SINGLE_SUBCONT);	/* get instance ptr */

    /*
     *  Find the ptr to the cmd_write/status_read register for
     *  this subunit and write command to SID. We have to
     *  choose between stat_reg1 for the first 4 bris and stat_reg2
     *  for the next 4 bris in the slot. 
     */
    if (subunit <  MBRI_MAX_PORTS_PER_ADAPTER)
	ds->mb_regs->stat_reg1[subunit] = command;
    else
	ds->mb_regs->stat_reg2[subunit % MBRI_MAX_PORTS_PER_ADAPTER]
	    = command;

    /* wait for command completion. poll the bit in the cmd_done reg*/
    for (count = 2000; count > 0; count--) {
	if (ds->mb_regs->cmd_done & MBRI_ENABLE_MASK[subunit])
	    break;
	wastetime(ds->delay_500_usec);  /* give the cmd_write_register
					   a break */
    }

    if ((count == 0) && bri_debug)
	buginf("BRI: Write to SID timed out for subunit %d, slot %d.\n",
	       subunit, slot);
    if (bri_debug)
	buginf("BRI: write_sid: wrote %x for subunit %d, slot %d.\n",
	       command, subunit, slot);

    /*
     * read the status reg to see if an interrupt status has shifted in.
     */
    sid_status = (uchar) (subunit < MBRI_MAX_PORTS_PER_ADAPTER) ?
		 ds->mb_regs->stat_reg1[subunit] :
		     ds->mb_regs->stat_reg2[subunit %
					    MBRI_MAX_PORTS_PER_ADAPTER];

    /*
     * if there is a non zero status, keep track of it.
     * if this happened during a write command(not a NOP),
     * force an interrupt next time, since the interrupt status
     * clears automatically.
     */
    if (sid_status) {
	ds->last_sid[subunit] = (ulong) sid_status;
	if ((command != TP3420_CRF_NOP) &&  /* => not during a read */
	    (command != TP3420_CRF_ENST) && /* => not during deliberate reads*/
	    (command != TP3420_CRF_DISST)) {

	    /*
	     * make sure we get an interrupt for this status
	     * by writing to int_status register.
	     */
	    if (bri_debug)
		buginf("BRI: Forced interrupt for subunit %d, slot %d is %x . \n",
		       subunit, slot, sid_status);
	    enable_status = ds->mb_regs->int_enable;
	    if (enable_status & MBRI_ENABLE_MASK[subunit])
	        ds->mb_regs->int_status |= MBRI_ENABLE_MASK[subunit];
	}
    }
}



/*
 * =====================================================================
 * mb_select_master_clock -
 *
 * Description:
 * function to select master clock for all 8 bris in a slot.
 *
 * Parameters:
 * slot:	 slot # of XX.
 * subunit:	 which bri subunit in that slot. (0 -7)
 * flag:	 boolean flag to indicate whether this subunit
 * 		 can serve as master now (TRUE) or
 *               can no longer serve as master (FALSE).
 *
 * returns:
 * nothing
 *
 * Notes:
 * Consider the case when there was only one subunit in the NIM which
 * was active. Suppose this subunit gets a deactivation indication (DI).
 * Then this routine does not remove the master clock entry, in the
 * hope that the DI was a burp on the line. However, after the
 * deactivation timeout, it removes the only master and now the NIM will
 * be driven by the internal clock.
 *
 * This should be called only from interrupt level.
 * =====================================================================
 */
static void mb_select_master_clock (uint slot, uint subunit,
				    master_status_t flag)
{
    ushort i, victim, mask;
    mb_instance_t *ds;

    ds = MB_INSTANCE(slot, SINGLE_SUBCONT);

    mask = MBRI_ENABLE_MASK[subunit];
    if (flag == MASTER_TRUE) {
	/*
	 * This subunit can serve as master if necessary.
	 * Add this to the activated subunits' bitmap, and
	 * make this the master clock only if there is no other
	 * master.
	 */
	ds->activated_bitmap |= mask;
	if (! ds->master_clock_bitmap) {
	    ds->master_clock_bitmap = mask;
	    ds->mb_regs->clock_switch = mask;
            wastetime(ds->delay_4_usec);	/* do not remove: h/w wait */
	}
    } else {
	/*
	 * So this subunit is not activated any more. If this
	 * was the master, we have to choose a new master. Otherwise
	 * we have to remove it from the activated list.
	 */
	if (ds->master_clock_bitmap == mask) { /* this is the master */
	    if (ds->activated_bitmap == mask) {/* only active subunit*/
		/*
		 * If this subunit is currently not active,
		 * there is no choice but to resort to internal clock.
		 * Otherwise, dont do anything with the hope that
		 * the deactivation was a burp.
		 */
		if (flag != MASTER_PENDING) {
		    ds->master_clock_bitmap = 0x0000;
		    ds->activated_bitmap = 0x0000;
		    ds->mb_regs->clock_switch = 0x0000;
                    wastetime(ds->delay_4_usec); /* do not remove: h/w wait */
		}
	    }else { /* implies there are other activated subunits */
		ds->activated_bitmap &= MBRI_DISABLE_MASK[subunit];
		/*
		 * right shift till we find the first activated
		 * subunit and choose that as the master.
		 */
		 for (i = 0, victim = ds->activated_bitmap;
		      i < MBRI_MAX_PORTS_PER_SLOT; i++, victim >>= 1) {
		     if (victim & 0x0001) {
			 mask = MBRI_ENABLE_MASK[i];
			 ds->master_clock_bitmap = mask;
			 ds->mb_regs->clock_switch = mask;
                         wastetime(ds->delay_4_usec);  /* do not remove:
							  h/w wait */
			 if (bri_debug)
			     buginf("BRI: New master in slot%d is subunit%d\n",
				    slot, i);
			 break;
		     }
		 }
	    }
	}else {
	    /*
	     * anyway, this was not the master. so just remove it from
	     * activated bit_map. no write to the clock switch reg.
	     */
	    ds->activated_bitmap &= MBRI_DISABLE_MASK[subunit];
	}
    }
}


/*
 * =====================================================================
 * mb_set_master_clock -
 *
 * Description:
 * Called when the user wants to select a BRI interface as the master.
 *
 * Parameters:
 * parseinfo *csb
 *
 * returns:
 * nothing
 *
 * Notes:	 Set only if the selected interface is active. Disable
 * 		 interrupts.
 * =====================================================================
 */
void mb_set_master_clock (parseinfo *csb)
{
    mb_instance_t *ds;
    hwidbtype *dchan_idb;
    uint slot;
    uint subunit;	        /* which bri subunit ? 0 - 7 */
    ushort mask;
    leveltype SR;


    /*
     * Get the hwidb ptr and make sanity checks.
     */
    dchan_idb = hwidb_or_null(csb->interface);
    if (!dchan_idb)
	return;

    /* Check that this idb really is a BRI. Currently, there is only ISDN*/
    if (!(dchan_idb->status & IDB_ISDN)) {
        if (! csb->nvgen) {
            printf("\ninterface type must be BRI.");
        }
        return;
    }
    
    /*
     * Get the instance pointer and find out the subunit for the
     * tp3420 which is to be initialised.
     */
    ds = MB_INSTANCE(dchan_idb->slot, SINGLE_SUBCONT);
    subunit = dchan_idb->subunit;
    slot = dchan_idb->slot;
    mask = MBRI_ENABLE_MASK[subunit];	/* the mask for the subunit*/

    /*
     * Decide if we need to do a nv_write ?
     */
    if (csb->nvgen) {
	if ((dchan_idb->isdn_info->master) &&
	    (ds->master_clock_bitmap == mask))
	    nv_write(TRUE, csb->nv_command);
	return;
    }

    /*
     * Set/unset the master option for this interface.
     */
    if (csb->sense) {
	/*
	 * Proceed only if the bri interface is activated
	 */
	if (ds->current_state[subunit] != l1_state_F7) {
	    printf("Interface is not activated. Cannot set master clock.\n");
	    return;
	}
	
	SR = raise_interrupt_level(HIRQ_INTLEVEL);
	/*
	 * Make this interface the new master
	 */
	ds->master_clock_bitmap = mask;
	ds->mb_regs->clock_switch = mask;/* this interface is the new master*/
        wastetime(ds->delay_4_usec);	/* do not remove: h/w wait */

	dchan_idb->isdn_info->master = TRUE;
	reset_interrupt_level(SR);	/* restore level */
    } else {
	/*
	 * let it remain as the master, but now it will follow the
	 * usual algorithm..... reset the flag
	 */
	dchan_idb->isdn_info->master = FALSE;
    }
    if (bri_debug)
	buginf("BRI: Master clock for slot %d is subunit %d.\n",
	       slot, subunit);
}


/*
 * =====================================================================
 * mb_interrupt -
 *
 * Description:
 * This handler is invoked when a NIM High interrupt is issued on a MBRI NIM.
 * This is the common interrupt handler for all 4/8 bri ports
 * in a slot. It first checks if the interrupt was caused by a loop
 * current change for all 8 bris, then services the tp3420 interrupts
 * for all 4/8 bris. The last interrupt checked is the timer interrupt.
 *
 * Paramters:
 * slot:	which slot issued the level 4 interrupt.
 *
 * Returns:
 * nothing.
 *
 * Notes: The ISDN states F1 to F8 are the I.430 finite
 * states recommended by ANSI/NET3 specification for the layer 1. 
 * =====================================================================
 */
static void mb_interrupt (uint slot)
{
    uint subunit;		/* which bri subunit ? 0 - 7 */
    uint sequence = 0;		/* identity of element in the deltaQ? */
    ushort status = 0;  	/* interrupt status */
    ushort state = 0;   	/* interrupt state */
    uchar sid_status = 0;	/* TP3420 status */
    mcn_instance_t *ds_mcn;	/* multi-channel NIM instance ptr */
    mb_instance_t *ds;          /* multi-bri instance ptr */
    mcn_timerbuf_t *ptr;	/* ptr to a timer element */
    ushort timer_status;	/* temp store for counter int status*/
    ushort timeout_type;	/* temp store for timeout type */
    hwidbtype *dchan_idb;	/* ptr to dchan idb */

    /*
     * Get the instance pointers.
     */
    ds = MB_INSTANCE(slot, SINGLE_SUBCONT);
    ds_mcn = MCN_INSTANCE(slot);

    /*
     * This block is executed only if loop current mode is enabled.
     * It checks for a loop current status change, and for each
     * subunit with a status change, action is taken based on
     * current detection or current loss.
     *
     * The "status" variable has the loop current status of all 8 bris
     * while "state" variable has the state of loop current (1 means
     * current present, 0 means no current) of all 8 bris. Both these
     * variables' bitfields represent bri 0 - 7, from right to left.
     */
    if (loop_current_mode) {
	status = ds->mb_regs->loop_current_status; /* note: clear on read */
        state  = ds->mb_regs->loop_current_state;  /* state of loop cur. */
	/*
	 * Since the status indication is for bri#0 to bri#7,
	 * shift the bits and test if any of those subunits
	 * had a loop current change.
	 */
	for (subunit = 0;
	     (status && (subunit < ds->no_of_subunits_present));
	     subunit++) {
	    if (status & 0x0001) { 	/* there was a change */
		 if (state & 0x0001) { 	/* loop current present */
		     /*
		      * All we need to do is update the isdn state
		      * to sensing.
		      */
		     ds->current_state[subunit] = l1_state_F2;
		 } else {		/* loss of loop current */
		     /*
		      * First action to do is to select an
		      * alternate master clock
		      * if this subunit is the master.
		      */
		     if ((ds->current_state[subunit] == l1_state_F7) ||
                          (ds->current_state[subunit] == l1_state_F8))
			 mb_select_master_clock(slot, subunit, MASTER_FALSE);
		     
		     /*
		      * Power down the SID.
		      */
		     mb_write_sid(slot, subunit,TP3420_CRF_DR);
		     mb_write_sid(slot, subunit, TP3420_CRF_PDN);
		     
		     /*
		      * Inform ISDN about this only if we were in a state
		      * other than F1 or F2. (ie, only if we are either
		      * activated or in an activation sequence). Also
		      * remove possible outstanding timers.
		      */
		     if (ds->current_state[subunit] > l1_state_F2) {
			 dchan_idb = bri_m32_get_idb(slot, SINGLE_SUBCONT,
                                                     subunit);
			 bri_enq_prim(dchan_idb->unit, ISDN_PH_DEACT_IND);
			 bri_enq_prim(dchan_idb->unit, ISDN_MPH_DEACT_IND);
			 bri_enq_prim(dchan_idb->unit,
				      ISDN_MPH_DISCONNECT_IND);
			 
			 /*
			  * Remove T3 and deact timer entries, if present.
			  * Return the Q elements back to the pool.
			  */
			 sequence = MB_SEQUENCE(slot, subunit,
						MBRI_T3_TIMEOUT_TYPE);
			 ptr = mcn_deltaQ_deQ_element(sequence, slot);
			 if (ptr)
			     enqueue(McnTimerPoolQ, ptr);
			 
			 sequence = MB_SEQUENCE(slot, subunit,
				     		MBRI_DEACT_TIMEOUT_TYPE);
			 ptr = mcn_deltaQ_deQ_element(sequence, slot);
			 if (ptr)
			     enqueue(McnTimerPoolQ, ptr);

			 /*
			  * Put the led off anyway.
			  */
			  ds_mcn->mcn_regs->led_control |=
			      MBRI_ENABLE_MASK[subunit];
		     }
		     /*
		      * Put us back in power off mode.
		      */
		     ds->current_state[subunit] = l1_state_F1;
		 }
	    }
	    status >>= 1;	/* Shift the next bri status out */
	    state >>= 1;	/* Shift the next bri state out */
	}
    }

    /*
     * This block is to handle the tp3420 SID interrupts for all
     * 4/8 BRI subunits present in the slot. Again, interrupt status
     * is right shifted to find out the subunits which need servicing.
     *
     * ISDN state transitions are from F2 to F7; F4 and F5 states
     * are not recognized by TP3420 and hence not implemented. The
     * different interrupt status are LSD, AI, AP, DI, CON, and EI.
     */

    status = ds->mb_regs->int_status;	/* read the 8 bri status*/
    for (subunit = 0; (status && (subunit < ds->no_of_subunits_present));
	 subunit++) {
	 if (status & 0x0001) {	/* this subunit has an interrupt */
	     /*
   	      * Write a NOP to the command status register.
	      * This will put the status in the ds->last_sid[subunit]
	      * variable.
	      * Reading the SID status will automatically clear
	      * the interrupt bit in the int_status register.
	      */
	     mb_write_sid(slot, subunit, TP3420_CRF_NOP);
	     sid_status = (uchar) (ds->last_sid[subunit]);
	     ds->last_sid[subunit] = 0;

	     dchan_idb = bri_m32_get_idb(slot, SINGLE_SUBCONT, 
                                         subunit);/* get dchan idb*/
	      
	     /*
	      * Perform actions on the basis of SID status
	      */
	     switch(sid_status) {
	       case TP3420_SRF_LSD:	/* line signal detect */
		 /*
		  * This is the case of remote activation when a line
		  * signal is detected. This state occurs when the SID
		  * receives an INFO 0 and the isdn state transitions to
		  * F3 from F2. Note that this interrupt status is valid
		  * only in F2.
		  * Inform ISDN and power up SID. If
		  * NT loopback enabled, loopback B channels, else
		  * clear all loopbacks.
		  */

		 if (dchan_idb->loopback) {
                     mb_write_sid(slot, subunit, TP3420_CRF_B1E);
                     mb_write_sid(slot, subunit, TP3420_CRF_B2E);
		     mb_write_sid(slot, subunit, TP3420_CRF_LBL1);
		     mb_write_sid(slot, subunit, TP3420_CRF_LBL2);
		 } else {
		     mb_write_sid(slot, subunit, TP3420_CRF_CAL);
                     mb_write_sid(slot, subunit, TP3420_CRF_B1D);
                     mb_write_sid(slot, subunit, TP3420_CRF_B2D);
		 }
		     
		 mb_write_sid(slot, subunit, TP3420_CRF_PUP);
		 bri_enq_prim(dchan_idb->unit, ISDN_MPH_CONNECT_IND);
		 ds->current_state[subunit] = l1_state_F3;
		 if (bri_debug)
		     buginf("BRI: Line signal detect for unit = %d, current state is F3. \n", dchan_idb->unit);
		 break;

	       case TP3420_SRF_AP:	/* Activation Pending */
		 /*
		  * This indication probably implies the reception of an
		  * INFO 2 signal. Send an Activation Request to SID.
		  * if previous state was F2 or F3, start T3 timer. If it was
		  * either F7 or F8, inform isdn about the Error Indication
		  * primitive.
		  */
		 mb_write_sid(slot, subunit, TP3420_CRF_AR);

		 switch (ds->current_state[subunit]) {

		   case l1_state_F2:
		     bri_enq_prim(dchan_idb->unit, ISDN_MPH_CONNECT_IND);

		   case l1_state_F3:
		     ptr = dequeue(McnTimerPoolQ);
		     if (!ptr) {
			 errmsg(&msgsym(NOMEMORY, BRI), dchan_idb->unit,
				timer_pool_msg);
			 break;
		     }
		     ptr->sequence = MB_SEQUENCE(slot, subunit,
						 MBRI_T3_TIMEOUT_TYPE);
		     ptr->timeout = MBRI_T3_TIMEOUT;
		     ptr->timeout_type = MBRI_T3_TIMEOUT_TYPE;
		     ptr->subunit = subunit;
		     mcn_deltaQ_enQ(slot, ptr);	/* start T3 timer */
		     if (bri_debug)
			 buginf("BRI: Starting T3 timer for unit = %d.\n",
				dchan_idb->unit);
		     break;
		     
		   case l1_state_F7:
		     bri_enq_prim(dchan_idb->unit, ISDN_MPH_EI1_IND);
		     break;

		   case l1_state_F8:
		     bri_enq_prim(dchan_idb->unit, ISDN_MPH_EI2_IND);
		     break;

		   case l1_state_F4:
		   case l1_state_F5:
		   case l1_state_F6:	/* do nothing */
		     break;
		     
		   default:
		     buginf("BRI: Unknown State in unit %d.\n",
			    dchan_idb->unit);
		 }
		 ds->current_state[subunit] = l1_state_F6;
		 if (bri_debug)
		     buginf("BRI: Activation Pending for unit = %d, current state is F6. \n", dchan_idb->unit);
		 break;

	       case TP3420_SRF_CON:	/* d channel collision */
		 /*
		  * Currently, we ignore this. Later, maybe we can bump
		  * the ds->d_chan_coll[subunit].
		  */
		 break;

	       case TP3420_SRF_AI:	/* Activation Indication*/
		 /*
		  * This indicates an INFO 4 has been received from
		  * the network and the layer 1 is ready to go.
		  * Remove T3 and deact timer entries, if present.
		  * Return the Q elements back to the pool.
		  */
		 sequence = MB_SEQUENCE(slot, subunit, MBRI_T3_TIMEOUT_TYPE);
		 ptr = mcn_deltaQ_deQ_element(sequence, slot);
		 if (ptr)
		     enqueue(McnTimerPoolQ, ptr);
		 
		 sequence = MB_SEQUENCE(slot, subunit, MBRI_DEACT_TIMEOUT_TYPE);
		 ptr = mcn_deltaQ_deQ_element(sequence, slot);
		 if (ptr)
		     enqueue(McnTimerPoolQ, ptr);
		 
		 /*
		  * Select this subunit as master if there is no other
		  * master. Queue primitives to ISDN to indicate activation.
		  */
                 if (dchan_idb->isdn_info->loopback_mode != PLUG_LOOPBACK_ENABLED) {
		   mb_select_master_clock(slot, subunit, MASTER_TRUE);
                 }
		 switch (ds->current_state[subunit]) {
		   case l1_state_F2:
		     bri_enq_prim(dchan_idb->unit, ISDN_MPH_CONNECT_IND);
		     break;
		   case l1_state_F6:
		   case l1_state_F8:
		     bri_enq_prim(dchan_idb->unit, ISDN_MPH_EI2_IND);
		     break;
		   case l1_state_F3:
		   case l1_state_F4:
		   case l1_state_F5:
		   case l1_state_F7:	/* do nothing */
		     break;
		   default:
		     buginf("BRI: Unknown State in unit %d.\n",
			    dchan_idb->unit);
		 }
		 bri_enq_prim(dchan_idb->unit, ISDN_PH_ACT_IND);
		 
		 ds->current_state[subunit] = l1_state_F7; /* activated */
		 if (bri_debug)
		     buginf("BRI: Activation for unit = %d, current state is F7. \n", dchan_idb->unit);

		 /*
		  * Put the led on.
		  */
		 ds_mcn->mcn_regs->led_control &=
		     MBRI_DISABLE_MASK[subunit];
		 break;

	     case TP3420_SRF_DI: /* deactivation */
		 /*
		  * This indicates reception of INFO 0 after an subunit
		  * is already activated. Try an alternate master even
		  * if it is a burp on line. Start deactivation timer
		  * if we are in state F7 or F8, otherwise power down
		  * and deactivate the subunit.
		  */
		 mb_select_master_clock(slot, subunit, MASTER_PENDING);

		 switch (ds->current_state[subunit]) {
		   case l1_state_F7:
		   case l1_state_F8:
		     ptr = dequeue(McnTimerPoolQ);
		     if (!ptr) {
			 errmsg(&msgsym(NOMEMORY, BRI), dchan_idb->unit,
				timer_pool_msg);
			 break;
		     }
		     ptr->sequence = MB_SEQUENCE(slot, subunit,
						 MBRI_DEACT_TIMEOUT_TYPE); 
		     ptr->timeout = MBRI_DEACT_TIMEOUT;
		     ptr->timeout_type = MBRI_DEACT_TIMEOUT_TYPE;
		     ptr->subunit = subunit;
		     mcn_deltaQ_enQ(slot, ptr);	/* start deact. timer */
		     if (bri_debug)
			 buginf("BRI: Starting DEACT timer for unit = %d.\n",
				dchan_idb->unit);
		     break;

		   case l1_state_F6:
		     bri_enq_prim(dchan_idb->unit, ISDN_PH_DEACT_IND);

		   default:	/* NOTE: deliberate fall thru */
		     mb_write_sid(slot, subunit, TP3420_CRF_DR);
		     mb_write_sid(slot, subunit, TP3420_CRF_PDN);
		     /*
		      * Remove T3 and deact timer entries, if present.
		      * Return the Q elements back to the pool.
		      */
		     sequence = MB_SEQUENCE(slot, subunit,
					    MBRI_T3_TIMEOUT_TYPE);
		     ptr = mcn_deltaQ_deQ_element(sequence, slot);
		     if (ptr)
			 enqueue(McnTimerPoolQ, ptr);
		     
		     sequence = MB_SEQUENCE(slot, subunit,
					    MBRI_DEACT_TIMEOUT_TYPE);
		     ptr = mcn_deltaQ_deQ_element(sequence, slot);
		     if (ptr)
			 enqueue(McnTimerPoolQ, ptr);
		     ds->current_state[subunit] = l1_state_F2;/*deactivated*/
		     if (bri_debug)
			 buginf("BRI: Deactivation for unit = %d, current state is F2. \n", dchan_idb->unit);
		     /*
		      * Put the led off anyway.
		      */
		     ds_mcn->mcn_regs->led_control |=
			      MBRI_ENABLE_MASK[subunit];
		     break;
		 }
		 break;
	       
	     case TP3420_SRF_EI:  /* error indication */
		 /*
		  * Indicates lost framing. Should happen only in
		  * F6 or F7 states.
		  */
		 if ((ds->current_state[subunit] == l1_state_F6) ||
		     (ds->current_state[subunit] == l1_state_F7)){
		     bri_enq_prim(dchan_idb->unit, ISDN_MPH_EI1_IND);
		     ds->current_state[subunit] = l1_state_F8; 
		 }
		 if (bri_debug)
			 buginf("BRI: Lost Framing for unit = %d, current state is F8. \n", dchan_idb->unit);
		 break;
		 
	     default:	/* should never happen */
		 if (bri_debug) 
		     buginf("BRI: Unexpected  SID int. status reg = %x, unit = %d \n",
			    sid_status, dchan_idb->unit);
		 
	     }
	 }
	 status >>= 1;	/* Shift the next bri status out */
    }

    /*
     * This block is for handling timeouts, due to a
     * level 4 interrupt from the the 125 usec counter. These are
     * the 3ms powerup timeout, 600 ms deactivation timeout, 8 sec
     * isdn T3 timeout for any subunit in the slot.
     * Note that deQ is done in a loop because, there
     * could be multiple timed out entries at the head of the Q.
     */
    timer_status = ds_mcn->ctr_regs->status;
    if (timer_status) {
	ds_mcn->ctr_regs->status = 0x0000;	/* clear status */
	while ((ptr = mcn_deltaQ_deQ_head(slot)) != NULL) {

	    subunit = ptr->subunit;
	    dchan_idb = bri_m32_get_idb(slot, SINGLE_SUBCONT, subunit);
	    timeout_type = ptr->timeout_type;
	    enqueue(McnTimerPoolQ, ptr);	/* return to pool */

	    /*
	     * Decide on basis of timeout type.
	     */
	    switch(timeout_type) {
	      case MBRI_PUP_TIMEOUT_TYPE:
		/*
		 * Should happen in normal power up sequence(for the  
		 * chip to settle down). Note that this will happen
		 * only in F3 state. Send AR to sid, start T3 timer.
		 */
		if (ds->current_state[subunit] == l1_state_F3) {
		    mb_write_sid(slot, subunit, TP3420_CRF_AR);
		    ds->current_state[subunit] = l1_state_F4;

		    ptr = dequeue(McnTimerPoolQ);
		    if (!ptr) {
			errmsg(&msgsym(NOMEMORY, BRI), dchan_idb->unit,
			       timer_pool_msg);
			break;
		    }
		    ptr->sequence = MB_SEQUENCE(slot, subunit,
						MBRI_T3_TIMEOUT_TYPE);
		    ptr->timeout = MBRI_T3_TIMEOUT;
		    ptr->timeout_type = MBRI_T3_TIMEOUT_TYPE;
		    ptr->subunit = subunit;
		    mcn_deltaQ_enQ(slot, ptr);	/* start T3 timer */
		    if (bri_debug)
			buginf("BRI: Starting T3 timer after expiry of Power Up timeout for unit = %d, current state is F4. \n",
			       dchan_idb->unit);
		}
		break;

	      case MBRI_T3_TIMEOUT_TYPE:
		/*
		 * This should happen only in states F4, F5, or F6.
		 * We have to deactivate the subunit on expiry of T3.
		 * Inform ISDN, remove deact timer if present, put the
		 * led off.
		 */

		/*
		 * Dance to pass homologation.
		 */
		mb_write_sid(slot, subunit, TP3420_CRF_ENST);
		mb_write_sid(slot, subunit, TP3420_CRF_DISST);
		if (ds->last_sid[subunit] == TP3420_F6_STATE) {
		    if (bri_debug)
			buginf("BRI: Returning unit = %d to state F6 after expiry of T3 timer.\n", dchan_idb->unit);
		    break;
		}

                /*
                 * Below is the normal sequence of operation:
                 */
		if (bri_debug)
		    buginf("BRI: T3 timer expired for unit = %d, current state is F2.\n",
			    dchan_idb->unit);
		bri_enq_prim(dchan_idb->unit, ISDN_PH_DEACT_IND);
		mb_write_sid(slot, subunit, TP3420_CRF_DR);
		mb_write_sid(slot, subunit, TP3420_CRF_PDN);

		sequence = MB_SEQUENCE(slot, subunit, MBRI_DEACT_TIMEOUT_TYPE);
		ptr = mcn_deltaQ_deQ_element(sequence, slot);
		if (ptr)
		    enqueue(McnTimerPoolQ, ptr);
		ds->current_state[subunit] = l1_state_F2;
		break;

	      case MBRI_DEACT_TIMEOUT_TYPE:
		/*
		 * This case should happen only in isdn states
		 * F7 or F8. Go back to state F2. Inform isdn,
		 * and make sure the master clock entry is removed
		 * for sure.
		 */

		/*
		 * The following dance is to see if the SID
		 * is now in state F7 or not. If so, why deactivate?
		 */
		mb_write_sid(slot, subunit, TP3420_CRF_ENST);
		mb_write_sid(slot, subunit, TP3420_CRF_DISST);
		if (ds->last_sid[subunit] == TP3420_F7_STATE) {
		    if (bri_debug)
			buginf("BRI: Returning unit = %d to state F7 after expiry of Deactivation timer.\n", dchan_idb->unit);
		    break;
		}
		
                /*
                 * Below is the normal sequence of operation:
                 */
		if (bri_debug)
		    buginf("BRI: DEACT timer expired for unit = %d, current state is F2.\n",
			    dchan_idb->unit);
		bri_enq_prim(dchan_idb->unit,ISDN_PH_DEACT_IND);
		if (ds->current_state[subunit] == l1_state_F8)
		    bri_enq_prim(dchan_idb->unit, ISDN_MPH_EI2_IND);
		ds->current_state[subunit] = l1_state_F2;
		mb_select_master_clock(slot, subunit, MASTER_FALSE);

		/*
		 * power down and put off the leds.
		 */
		mb_write_sid(slot, subunit, TP3420_CRF_DR);
		mb_write_sid(slot, subunit, TP3420_CRF_PDN);
		ds_mcn->mcn_regs->led_control |=
		    MBRI_ENABLE_MASK[subunit];
		break;

	      default:	/* should never happen */
		if (bri_debug) 
		    buginf("BRI: Unexpected  counter interrupt in unit = %d.\n",
			   dchan_idb->unit);
		
	    }
	}
    }

    /* all three different interrupts have been serviced, time to return */
    
    /*
     * This is extreme paranoia. In case h/w master clock mechanism
     * fails, try to recover.
     * This check is a software check for a change in master-clock
     * caused by a h/w glitch. Please do not remove this.
     * Get the h/w master clock value and see if the software thinks
     * it is different, if so, write to the h/w register to make the
     * proper master clock assignment.
     */
    status = ds->mb_regs->clock_switch;	
    if ((ds->master_clock_bitmap) && (ds->activated_bitmap) &&
        (ds->master_clock_bitmap != status)) {
	ds->mb_regs->clock_switch = ds->master_clock_bitmap;
        wastetime(ds->delay_4_usec); /* do not remove: wait necessary for h/w*/

	/*
	 * Now make sure the h/w accepted the software written value.
	 */
        status = ds->mb_regs->clock_switch;	
        if (status != ds->master_clock_bitmap) {
	    if (ds->master_clock_bitmap == ds->activated_bitmap) {
		ds->master_clock_bitmap = ds->activated_bitmap = 0;
		ds->mb_regs->clock_switch = 0;
		wastetime(ds->delay_4_usec);     /* do not remove*/
		status = ds->mb_regs->int_status; /* dummy read to clear int */
	    }
        }
	if (bri_debug)
	    buginf("BRI: Master clock overwrite due to h/w signal for slot %d.\n",
		   slot);
    }    
}



/*
 * =====================================================================
 * mb_startup -
 *
 * Description:
 * This function is called by analyze_bri as a one time initialisation
 * per slot.
 *
 * Parameters:
 * slot:		The slot #.
 * adapter_type:	The bri adapter plugged in could be a single or
 *			dual. ie, 4 port or 8 port bri.
 *
 * Returns:
 * nothing.
 * 
 * =====================================================================
 */
static void mb_startup (uint slot, uint adapter_type)
{
    mb_instance_t *ds;
    mcn_instance_t *ds_mcn;

    ds_mcn = MCN_INSTANCE(slot);	/* get the mcn instance */
    
    /*
     * Allocate  memory for the multi-bri instance.
     */
    ds = malloc_fast(sizeof(mb_instance_t));
    if (!ds) {
	crashdump(0);
    }

    /*
     * set the base of the mb_block_t structure for this slot
     * to get to the adapter registers.
     */
    ds->mb_regs = (mb_block_t *) ((uint) GET_SLOT_BASE(slot) +
				  (uint) MBRI_ADAPTER_OFFSET);

    /*
     * update the current no of subunits in this slot.
     */
    switch (adapter_type) {
      case MCN_PA_BRI_SINGLE:
	ds->no_of_subunits_present = MBRI_MAX_PORTS_PER_ADAPTER;
	break;
      case MCN_PA_BRI_DUAL:
	ds->no_of_subunits_present = MBRI_MAX_PORTS_PER_ADAPTER * 2;
	break;
      default:
	errmsg(&msgsym(INITFAIL, BRI), slot, "Unknown adapter.");
	free(ds);
	return;
    }

    /*
     * Store the calibrated delay variables.
     */
    ds->delay_4_usec = timer_calibrate(4, FALSE);
    ds->delay_500_usec = timer_calibrate(500, FALSE);

    /*
     * Store the mb instance pointer in the MCN instance.
     * Henceforth, access the instance using MB_INSTANCE(slot, SINGLE_SUBCONT).
     */
    ds_mcn->mcn_subcont[SINGLE_SUBCONT].info.mb_ds = ds;

    /*
     * init the global queue for passing L1 primitives to isdn code
     * and pre-allocate a pool of L1 elements.
     */
    if (!IsdnL1PrimPoolQ) 
       bri_init_isdn_queue();

    /*
     * register level 4 interrupt for this slot, to handle all
     * interrupts: tp3420, loop current, and counter.
     */
    nim_register_hi_irq_handler((ushort) slot, mb_interrupt,\
				(ushort) IDBTYPE_BRI, (ushort) slot);

    /*
     * perform MCN actions specific to the slot like enabling  the
     * timer interrupt, putting leds off etc.
     * However, keep the tp3420 interrupts disabled, since we do not
     * want the network to interrupt, till the isdn process is spawned.
     */
    ds_mcn->ctr_regs->int_enable = MCN_COUNTER_ENABLE;
    ds_mcn->mcn_regs->led_control = MCN_LED_DISABLE;

    ds->mb_regs->int_enable  = MBRI_ALL_DISABLE;
    ds->mb_regs->loop_current_enable = MBRI_ALL_DISABLE;
}


/*
 * =====================================================================
 * mb_analyze_mcn_port_adaptor -
 *
 * Description:
 *  Initialize the Port Adaptor in a given slot
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 * 
 * =====================================================================
 */
void mb_analyze_mcn_port_adaptor (int slot, int subcont, int total_ctrlr_unit)
{
    mcn_instance_t     *ds;
    uint 		ports, type;
    ushort 		subunit;
    
    type = mcn_get_adapter_type(slot);

    if ((type == MCN_PA_BRI_SINGLE) || (type == MCN_PA_BRI_DUAL)) {
	mb_startup(slot, type);
	ds = MCN_INSTANCE(slot);

	/* 
	 * set the interface count according to
	 * the adaptor type and init the BRI
	 * interfaces.
	 */ 
	if (type == MCN_PA_BRI_SINGLE) 
	    ports = MBRI_MAX_PORTS_PER_ADAPTER;
	else
	    ports = MBRI_MAX_PORTS_PER_ADAPTER * 2;
	
	for (subunit = 0; subunit < ports; subunit++) {
	    init_bri_idb(ds, slot, subcont, nbrinets++, subunit);
	}

	/*
	 * Add parse chains
	 */
	bri_parser_init();
	bri_c4000_parser_init();
    }
}


/*
 * =====================================================================
 * mb_tp3420_init -
 *
 * Description:
 * This function initialises  a specific tp3420 chip, ie per
 * bri interface. Invoked for each dchannel idb by bri_init ().
 * Note that we do not enable interrupts.
 *
 * Parameters:
 * dchannel_idb: 	hwidbtype ptr to the bri interface.
 *
 * Returns:
 * Nothing
 * =====================================================================
 */
void mb_tp3420_init (hwidbtype *dchan_idb)
{
    mb_instance_t *ds;
    uint slot;
    uint subunit;	/* which bri subunit ? 0 - 7 */

    /*
     * Get the instance pointer and find out the subunit for the
     * tp3420 which is to be initialised.
     */
    slot = dchan_idb->slot;
    ds = MB_INSTANCE(slot, SINGLE_SUBCONT);
    subunit = dchan_idb->subunit;

    /*
     * write commands to the SID to set it in the approp. mode
     */
    mb_write_sid(slot, subunit, TP3420_CRF_TES);/* we are a TE slave */
    mb_write_sid(slot, subunit, TP3420_CRF_DIF1);/* use digital subunit 1*/
    mb_write_sid(slot, subunit, TP3420_CRF_EID); /* disable end of
						      message interrupt*/
    mb_write_sid(slot, subunit, TP3420_CRF_MID);/* disable
						     multiframe support*/
    mb_write_sid(slot, subunit, TP3420_CRF_DIS3X); /* disable 3X checking*/

    /*
     * Check for the NT loopback mode, ie loopback type2. Note that the
     * driver does not directly support the other loopback types. They
     * are supported by the test_subunit module by test commands.
     */
    if (dchan_idb->loopback) {
        mb_write_sid(slot, subunit, TP3420_CRF_B1E);
        mb_write_sid(slot, subunit, TP3420_CRF_B2E);
	mb_write_sid(slot, subunit, TP3420_CRF_LBL1);
	mb_write_sid(slot, subunit, TP3420_CRF_LBL2);
	mb_write_sid(slot, subunit, TP3420_CRF_PUP);
    } else {
	mb_write_sid(slot, subunit, TP3420_CRF_CAL); /* clear all loopbacks*/
        mb_write_sid(slot, subunit, TP3420_CRF_B1D);
        mb_write_sid(slot, subunit, TP3420_CRF_B2D);
    }
}


/*
 * =====================================================================
 * mb_tp3420_enable_line -
 *
 * Description:
 * This function is called when switch type is first configured
 * and whenever the subunit comes out of shutdown. All interrupts are
 * enabled here.
 *
 * Parameters:
 * dchannel_idb: 	hwidbtype ptr to the bri subunit.
 *
 * Returns:
 * nothing
 *
 * Note: if the subunit is coming out of shutdown without a switch
 * type configuration, just return.
 * 
 * =====================================================================
 */
void mb_tp3420_enable_line (hwidbtype *dchan_idb)
{
    mb_instance_t *ds;
    uint slot;
    uint subunit;	/* which bri subunit ? 0 - 7 */
    leveltype SR;	/* interrupt level */

    /*
     * Check if isdn process is up, otherwise flag message.
     */
    if (!isdn_process_up) {
        /*
         * Only display the error message if we are up and running.
         */
        if (system_configured)
	    printf("BRI: Switchtype has to be configured before restart.\n");
	return;
    }
    
    /*
     * Get the instance pointer and find out the subunit for which the
     * interrupts are to be enabled.
     */
    slot = dchan_idb->slot;
    ds = MB_INSTANCE(slot, SINGLE_SUBCONT);
    subunit = dchan_idb->subunit;

    
    SR = raise_interrupt_level(HIRQ_INTLEVEL);

    /*
     * Enable loop current interrupt for this subunit only if
     * supported. Update the initial isdn state of the subunit.
     */
    if (ds->current_state[subunit] != l1_state_F7) {
        if (loop_current_mode) {
           ds->mb_regs->loop_current_enable |= MBRI_ENABLE_MASK[subunit];
           ds->current_state[subunit] = l1_state_F1;
        }
        else {
           ds->current_state[subunit] = l1_state_F2;
        }
    }

    /*
     * Now enable the tp3420 SID.
     */
    ds->mb_regs->int_enable |= MBRI_ENABLE_MASK[subunit];
    
    reset_interrupt_level(SR);
}


/*
 * =====================================================================
 * mb_tp3420_disable_bri_line -
 *
 * Description:
 * This has to be called during a shutdown to disable all
 * bri interrupts of that particular subunit.
 *
 * Parameters:
 * dchannel_idb: 	hwidbtype ptr to the bri subunit.
 *
 * Returns:
 * nothing.
 *
 * Notes:
 * This function assumes that all current state information and chip
 * power downs have been already taken care of. (ie, a
 * mb_tp3420_deactivate_line() has been invoked before this.)
 * =====================================================================
 */
void mb_tp3420_disable_line (hwidbtype *dchan_idb)
{
    mb_instance_t *ds;
    uint slot;
    uint subunit;	/* which bri subunit ? 0 - 7 */
    leveltype SR;	/* interrupt level */

    /*
     * Get the instance pointer and find out the subunit for which the
     * interrupts are to be disabled.
     */
    slot = dchan_idb->slot;
    ds = MB_INSTANCE(slot, SINGLE_SUBCONT);
    subunit = dchan_idb->subunit;
    
    SR = raise_interrupt_level(HIRQ_INTLEVEL);

    /*
     * Disable loop current interrupt for this subunit only if the
     * mode is supported. Always disable tp3420 interrupt.
     */
    if (loop_current_mode)
	ds->mb_regs->loop_current_enable &= MBRI_DISABLE_MASK[subunit];
    ds->mb_regs->int_enable &= MBRI_DISABLE_MASK[subunit];

    reset_interrupt_level(SR);
}


/*
 * =====================================================================
 * mb_tp3420_deactivate_line -
 *
 * Description:
 * This function is called during a bri_reset or a bri_shutdown to
 * make sure that the subunit TP3420 is in a known state and also
 * to select an alternate master clock, if this subunit has been the
 * master.
 *
 * Parameters:
 * dchannel_idb: 	hwidbtype ptr to the bri subunit.
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
void mb_tp3420_deactivate_line (hwidbtype *dchan_idb)
{
    uint slot;
    uint subunit;	/* which bri subunit ? 0 - 7 */
    uint sequence = 0;	/* which element in the deltaQ? */
    mcn_timerbuf_t *ptr;/* ptr to a timer element */
    mb_instance_t *ds;
    mcn_instance_t *ds_mcn;
    leveltype SR;	/* interrupt level */

    /*
     * Get the instance pointer and find out the subunit for which the
     * the tp3420 has to be deactiavted.
     */
    slot = dchan_idb->slot;
    ds = MB_INSTANCE(slot, SINGLE_SUBCONT);
    ds_mcn = MCN_INSTANCE(slot);
    subunit = dchan_idb->subunit;

    SR = raise_interrupt_level(HIRQ_INTLEVEL);

    /*
     * First action to do is to select an alternate master clock
     * if this subunit is the master.
     */
    if ((ds->current_state[subunit] == l1_state_F7) ||
         (ds->current_state[subunit] == l1_state_F8))
	mb_select_master_clock(slot, subunit, MASTER_FALSE);

    /*
     * Power down the SID.
     */
    mb_write_sid(slot, subunit,TP3420_CRF_DR);
    mb_write_sid(slot, subunit, TP3420_CRF_PDN);

    /*
     * Inform ISDN about this only if we were in a state
     * other than F1 or F2. (ie, only if we are either
     * activated or in an activation sequence). Also
     * remove possible outstanding timers.
     */
    if ((isdn_process_up) && (ds->current_state[subunit] >= l1_state_F2)) {
  	bri_enq_prim(dchan_idb->unit, ISDN_PH_DEACT_IND);
	bri_enq_prim(dchan_idb->unit, ISDN_MPH_DEACT_IND);

	/*
	 * Remove T3 and deact timer entries, if present.
	 * Return the Q elements back to the pool.
	 */
	sequence = MB_SEQUENCE(slot, subunit, MBRI_T3_TIMEOUT_TYPE);
	ptr = mcn_deltaQ_deQ_element(sequence, slot);
	if (ptr)
	    enqueue(McnTimerPoolQ, ptr);

	sequence = MB_SEQUENCE(slot, subunit, MBRI_DEACT_TIMEOUT_TYPE);
	ptr = mcn_deltaQ_deQ_element(sequence, slot);
	if (ptr)
	    enqueue(McnTimerPoolQ, ptr);
    }

    /*
     * Set the appropriate current state variable. If there is no loop
     * current support go straight to F2 state.
     */
    if ((loop_current_mode) && (ds->current_state[subunit] < l1_state_F2))
	ds->current_state[subunit] = l1_state_F1;
    else
	ds->current_state[subunit] = l1_state_F2;
    
    /*
     * Put the led off anyway. Note: active low and hence this dance.
     */
    ds_mcn->mcn_regs->led_control |= MBRI_ENABLE_MASK[subunit];

    reset_interrupt_level(SR);
}



/*
 * =====================================================================
 * mb_tp3420_enable_loopback -
 *
 * Description:
 * The function which initialises the tp3420 chip in loopback mode:
 * mode 0, disable loopback 
 * mode 1, enable  system loopback 
 * mode 2, enable  remote loopback  <--- TO BE DONE
 * mode 3, enable  plug   loopback
 *
 * Parameters:
 * dchan_idb - pointer to d channel idb
 * loopback_mode - loopback mode 
 *
 * Returns:
 * nothing
 *
 * Notes:
 * Loopback mode 2: remote loopback, i.e., NT loopback, where data recceived
 * from the network are looped back to the network was implemented before 
 * this function is created and is not implemented here. It use 'loopback'
 * field in idb structure, while system loopback and chip loopback
 * that are implemented here use 'loopback_mode' in isdn_info structure.
 * For the purpose of integrity, putting all these fields in one structure
 * is recommended.
 * =====================================================================
 */
void mb_tp3420_enable_loopback (hwidbtype *dchan_idb,
				enum Loopback loopback_mode)
{
    mb_instance_t *ds;
    uint slot;
    uint subunit;                  /* which bri subunit ? 0 - 7 */
    static boolean loop_enabled = FALSE;

    /*
     * Get the instance pointer and find out the subunit for the
     * tp3420 which is to be initialised.
     */
    slot = dchan_idb->slot;
    ds = MB_INSTANCE(slot, SINGLE_SUBCONT);
    subunit = dchan_idb->subunit;

    switch (dchan_idb->isdn_info->loopback_mode) {
      case LOOPBACK_DISABLED:
        mb_write_sid(slot, subunit, TP3420_CRF_CAL); /* clear all loopbacks*/

        /* 
         * If loopback was enabled before, power down TP3420 so that it is
         * back to the state before the loopback test for which the interface 
         * is manully shut down by a user.
         */
        if (loop_enabled) { 
            mb_write_sid(slot, subunit, TP3420_CRF_PDN); 
        }
        break;

      case SYSTEM_LOOPBACK_ENABLED:
        mb_write_sid(slot, subunit, TP3420_CRF_PDN); /* power down */
        mb_write_sid(slot, subunit, TP3420_CRF_TES); /* we are a TE slave */
        mb_write_sid(slot, subunit, TP3420_CRF_PUP); /* power up */
        mb_write_sid(slot, subunit, TP3420_CRF_LBS); /* system loop */
        mb_write_sid(slot, subunit, TP3420_CRF_B1E);
        mb_write_sid(slot, subunit, TP3420_CRF_B2E);
        loop_enabled = TRUE;  
        break;

      case REMOTE_LOOPBACK_ENABLED:
        /*
         * TO BE DONE
         */
        break;
 
      case PLUG_LOOPBACK_ENABLED:
        mb_write_sid(slot, subunit, TP3420_CRF_PDN);
        mb_write_sid(slot, subunit, TP3420_CRF_NTF);
        mb_write_sid(slot, subunit, TP3420_CRF_PUP);
        DELAY(2);
        mb_write_sid(slot, subunit, TP3420_CRF_AR);
        mb_write_sid(slot, subunit, TP3420_CRF_B1E);
        mb_write_sid(slot, subunit, TP3420_CRF_B2E);
        loop_enabled = TRUE;  
        break;

      default:
        break;
    } 
}


/*
 * The following routines are called by the isdn code.
 */

/*
 * =====================================================================
 * mb_isdn_enable_bri_channel -
 *
 * Description:
 * called by isdn code to enable individual B channels to reduce CRC
 * and abort errors before connection.
 *
 * Parameters:
 * bchan_idb:	idb ptr to the bchannel.
 *
 * Returns:
 * nothing.
 * =====================================================================
 */
void mb_isdn_enable_bri_channel (hwidbtype *bchan_idb)
{
    leveltype SR;
    enum Chan bchan;
    mb_instance_t *ds;
    uint slot, subunit;

    /*
     * Get the slot, subunit, and channel information.
     */
    slot = bchan_idb->slot;
    ds = MB_INSTANCE(slot, SINGLE_SUBCONT);
    subunit = bchan_idb->subunit;
    bchan = bchan_idb->isdn_info->chantype;

    if (bri_debug)
	buginf("BRI: enable channel %s \n",
	       (bchan == B1) ? "B1": "B2");
    
    SR = raise_interrupt_level(HIRQ_INTLEVEL);

    /*
     * Enable the particular B channel by writing SID command
     */
    if (bchan == B1)
	mb_write_sid(slot, subunit, TP3420_CRF_B1E);
    else if (bchan == B2)
	mb_write_sid(slot, subunit, TP3420_CRF_B2E);

    reset_interrupt_level(SR);
}


/*
 * =====================================================================
 * mb_isdn_disable_bri_channel -
 *
 * Description:
 * called by isdn code to disable individual B channels to reduce CRC
 * and abort errors before connection.
 *
 * Parameters:
 * bchan_idb:	idb ptr to the bchannel.
 *
 * Returns:
 * nothing.
 * =====================================================================
 */
void mb_isdn_disable_bri_channel (hwidbtype *bchan_idb)
{
    leveltype SR;
    enum Chan bchan;
    mb_instance_t *ds;
    uint slot, subunit;

    /*
     * Get the slot, subunit, and channel information.
     */
    slot = bchan_idb->slot;
    ds = MB_INSTANCE(slot, SINGLE_SUBCONT);
    subunit = bchan_idb->subunit;
    bchan = bchan_idb->isdn_info->chantype;

    if (bri_debug)
	buginf("BRI: disable channel %s \n",
	       (bchan == B1) ? "B1": "B2");
    
    SR = raise_interrupt_level(HIRQ_INTLEVEL);

    /*
     * Disable the particular B channel by writing SID command
     */
    if (bchan == B1)
	mb_write_sid(slot, subunit, TP3420_CRF_B1D);
    else if (bchan == B2)
	mb_write_sid(slot, subunit, TP3420_CRF_B2D);

    reset_interrupt_level(SR);
}


/*
 * =====================================================================
 * isdn_bri_enable_layer1_Q -
 *
 * Description:
 * This routine is called once the user configures
 * a switch type  or when the switch type command is loaded from nv ram.
 * Note that the interrupts for the tp3420 are enabled for the first time
 * here so that the isdn process will be ready to receive any
 * primtives that could be queued into the global Q.
 *
 * Parameters:
 * none
 *
 * Returns:
 * nothing
 *
 * Notes: This routine is called only one time when the isdn process
 * is spawned. After that the flag "isdn_process_up" has the
 * information about the isdn process.
 * =====================================================================
 */
void isdn_bri_enable_layer1_Q (void)
{
    hwidbtype *idb;

    /*
     * flag for other init/restart routines which need to know about
     * the existence of isdn process.
     */
    isdn_process_up = TRUE;
    
    /*
     * Enable tp3420 interrupts for all subunits in the box having
     * a BRI interface.
     */
    FOR_ALL_HWIDBS(idb) 
	if ((idb->type == IDBTYPE_BRI) &&
	    (idb->isdn_info->chantype == ISDN_Dchan) &&
	    (idb->state != IDBS_ADMINDOWN))
	    mb_tp3420_enable_line(idb);
}


/*
 * =====================================================================
 * isdn_activate_line -
 *
 * Description:
 * For isdn initiated activation.
 *
 * Parameters:
 * dchan_idb:	ptr to dchannel idb.
 *
 * Returns:
 * nothing.
 *
 * Notes: Some switch types might not need isdn to do this.
 * so critical during homologation.
 * =====================================================================
 */
void mb_isdn_activate_line (hwidbtype *dchan_idb)
{
    mb_instance_t *ds;
    uint slot;
    uint subunit;	/* which bri subunit ? 0 - 7 */
    mcn_timerbuf_t *ptr;
    leveltype SR;

    /*
     * Get the instance pointer and find out the subunit for the
     * tp3420 which is to be initialised.
     */
    ds = MB_INSTANCE(dchan_idb->slot, SINGLE_SUBCONT);
    subunit = dchan_idb->subunit;
    slot = dchan_idb->slot;

    /*
     * Try activation only if we have not started the 
     * activation sequence.
     * Send PUP and start PUP timer.
     */
    if (ds->current_state[subunit] < l1_state_F4) {
	SR = raise_interrupt_level(HIRQ_INTLEVEL);

	/*
	 * if loopback NT enabled, then loop b channels.
	 */
	if (dchan_idb->loopback) {
            mb_write_sid(slot, subunit, TP3420_CRF_B1E);
            mb_write_sid(slot, subunit, TP3420_CRF_B2E);
	    mb_write_sid(slot, subunit, TP3420_CRF_LBL1);
	    mb_write_sid(slot, subunit, TP3420_CRF_LBL2);
	} else {
	    mb_write_sid(slot, subunit, TP3420_CRF_CAL);
            mb_write_sid(slot, subunit, TP3420_CRF_B1D);
            mb_write_sid(slot, subunit, TP3420_CRF_B2D);
	    mb_tp3420_init(dchan_idb);
	}
	mb_write_sid(slot, subunit, TP3420_CRF_PUP);

        /*
	 * Start PUP timer.
	 */
	ptr = dequeue(McnTimerPoolQ);
	if (!ptr) {
	    errmsg(&msgsym(NOMEMORY, BRI), dchan_idb->unit, timer_pool_msg);
	    reset_interrupt_level(SR);
	    return;
	}
	ptr->sequence = MB_SEQUENCE(slot, subunit, MBRI_PUP_TIMEOUT_TYPE); 
	ptr->timeout = MBRI_PUP_TIMEOUT;
	ptr->timeout_type = MBRI_PUP_TIMEOUT_TYPE;
	ptr->subunit = subunit;
	mcn_deltaQ_enQ(slot, ptr);   /* enq to start timer */

	if (bri_debug)
	   buginf("BRI: Starting Power Up timer for unit = %d. \n",
		  dchan_idb->unit);
	ds->current_state[subunit] = l1_state_F3;

	reset_interrupt_level(SR);
    }
}
