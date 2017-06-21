/* $Id: if_les_m32.c,v 3.6.28.11 1996/09/07 22:37:18 jliu Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_m32.c,v $
 *------------------------------------------------------------------
 * if_les_m32.c
 *
 * 6 June 1994, Michael Hubenthal
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_les_m32.c,v $
 * Revision 3.6.28.11  1996/09/07  22:37:18  jliu
 * CSCdi64205:  Unprovisioned T1 plugged into AS5200 causes box to hang
 * Branch: California_branch
 *     o The fix included display warning message to console port and used
 *       SNMP notification to send the message to network management
 *       station
 *
 * Revision 3.6.28.10  1996/09/01  21:19:49  fowler
 * CSCdi60734:  Frame Relay rate enforcement is not effective
 *              Remove original fix due to its performance impact
 * Branch: California_branch
 *
 * Revision 3.6.28.9  1996/08/31  04:45:25  fowler
 * CSCdi60734:  Frame Relay rate enforcement is not effective
 * Branch: California_branch
 *
 * Revision 3.6.28.8  1996/08/28  12:58:34  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.28.7  1996/08/13  02:20:22  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.6.28.6  1996/07/20  03:19:54  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.6.28.5  1996/06/16  21:16:00  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.6.28.4  1996/06/05  20:58:29  wmay
 * CSCdi59462:  DSX1 remote interface loopback process needs help
 * Fix up remote loopback state machine - use far_end_looped field
 * to indicate actual loop state - change buffer display from serial
 * int debug to serial packet
 * Branch: California_branch
 *
 * Revision 3.6.28.3  1996/05/23  23:46:42  hrobison
 * CSCdi50628:  BRI interface stops sending frames on B or D channels on
 * 4x00
 * Branch: California_branch
 * Under unknown circumstances, a 4x00 BRI can stop transmitting.  Appears
 * to be related to CSCdi37200, so choose the lesser of two evils, either
 * have it hang, or have it require a ton of memory just to change the
 * speed of the interface.  Back out CSCdi37200 until we can figure out
 * what is happening.
 *
 * Revision 3.6.28.2  1996/04/17  13:43:53  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.6.28.1  1996/03/18  20:42:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.3  1996/03/13  01:49:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.2  1996/03/07  10:05:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/01/24  22:24:20  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1996/01/22  06:37:42  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1996/01/19  23:09:52  hrobison
 * CSCdi37200:  MBRI requires significant memory to set b channel speed
 * The MBRI driver no longer frees and reallocates ring buffers and
 * descriptors on every dialer call that changes bandwidth.  Set the
 * timeslot bitrate in the M32 on every call.
 *
 * Fix problem (software workaround for probable HW bug) with the M32
 * chip where the device RX descriptor pointer either does not increment
 * or jumps to the start of the ring with an RX interrupt
 *
 * Revision 3.4  1996/01/06  03:22:43  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/17  17:40:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:59:41  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:14:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/06/21  08:55:31  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.4  1995/06/21  02:26:30  jchlin
 * CSCdi35650 (transmit delay bug) and CSCdi35651 (remote loopback bug)
 * are fixed in this commit. Now, the hdlc flags will be used to
 * implement the transmit delay. The interface remote loopback will
 * send out the DDS CSU loopback patterns to activate and deactivate
 * CSU remote loopback.
 * CSCdi35650:  The interface transmit delay for T1/E1/PRI doesnt work
 *
 * Revision 2.3  1995/06/20  20:27:55  jchlin
 * Fix the channel throttling routines so that the rx channel
 * won't get stucked after throttling.
 * Fix the rx channel descriptor ring handling so that rx
 * channel won't get stucked after the ring is full and
 * gets processed during the single interrupt.
 * CSCdi34370:  4000 crashed w/BADMAGIC during stress test
 *
 * Revision 2.2  1995/06/19  06:47:47  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:36:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "interface_private.h"
#include "packet.h"
#include "../if/network.h"
#include "if_les.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../if/ether.h"
#include "../ip/ip.h"
#include "../tcp/tcpcompress.h"
#include "logger.h"

#if defined(XX) | defined(SIERRA)
#   include "if_c4000_mcn.h"
#endif

#if defined(BRASIL)
#   include "if_as5200_mcn.h"
#endif

#include "if_c4000_m32.h"
#include "if_les_m32.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "sys_registry.h"
#include "../if/traffic_shape.h"
#include "../if/if_controller.h"
#include "les_controller.h"
#include "../if/dsx1.h"
#include "isdn_bri.h"
#include "../if/dsx1_registry.h"


extern void dsx1_m32_errors_msg(ulong msg_index);

static void m32_recv_remote_loopback (m32_instance_t *m32_ds,
                                      ulong channel);



/*
 * =====================================================================
 * m32_delay_msec()
 *
 * Description:
 * Delay for some time in milisecond resolution.
 *
 * Parameters:
 * msec - delay msec miliseconds.
 *
 * Returns:
 * None.
 * =====================================================================
 */
static void m32_delay_msec (ulong msec)
{
   ulong ix;

   for (ix=msec; ix; ix--) {
      usecdelay(1000);
   }
}


/*
 * =====================================================================
 * m32_get_channel_bw_inline()
 *
 * Description:
 * Return the bandwidth of a Munich32 channel.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel from which to get the idb
 *
 * Returns:
 * bandwidth in Kbits, ex. a 64 Kbit bandwidth returns a 64
 * =====================================================================
 */
inline ulong m32_get_channel_bw_inline (m32_instance_t *m32_ds,
					ulong channel)
{
    return(m32_ds->channel_params[channel].bandwidth);
}


/*
 * =====================================================================
 * m32_get_channel_idb_inline()
 *
 * Description:
 * Return the idb of a Munich32 channel.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel from which to get the idb
 *
 * Returns:
 * pointer to a hw idb struct
 * =====================================================================
 */
inline hwidbtype *m32_get_channel_idb_inline (m32_instance_t *m32_ds,
					      ulong channel)
{
    return(m32_ds->channel_params[channel].channel_idb);
}

/*
 * =====================================================================
 * m32_get_channel_state_inline()
 *
 * Description:
 * Return the state of a Munich32 channel.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel from which to get the state
 *
 * Returns:
 * state of the channel
 * =====================================================================
 */
inline enum m32_channel_state_t m32_get_channel_state_inline (
    m32_instance_t *m32_ds,
    ulong channel)
{
    return(m32_ds->channel_params[channel].current_state);
}


/*
 * =====================================================================
 * m32_alloc_ctrl()
 *
 * Description:
 * Allocate and initialize a Munich32 configuration and control structure.
 *
 * Parameters:
 * none
 *
 * Returns:
 * a pointer to the m32 configuration and control structure.
 * =====================================================================
 */
static m32_control_t *m32_alloc_ctrl (uint slot)
{
    m32_control_t *m32_ctrl;
    ulong *intq_element_p;
    ushort ix;

    /*
     * allocate the control struct on a long word boundry.
     * write the address into the m32 instance struct. if the
     * alloc fails, things are real bad, crashdump the system.
     */
    m32_ctrl = malloc_iomem_aligned(sizeof(m32_control_t),
				    M32_LONG_WORD_ALIGN);
                                  
    if (m32_ctrl == NULL) {
        crashdump(0);
    }

    /*
     * allocate the interrupt queue on a long word boundry.
     * write the address of the interrupt queue and the number
     * of queue elements into the control struct. if the alloc
     * fails, things are real bad, crashdump the system.
     */
    m32_ctrl->ctrl_intq.intq_addr =
	malloc_iomem_aligned(sizeof(ulong) * M32_INTQ_ELEMENTS,
			  M32_LONG_WORD_ALIGN);
                 
    if (m32_ctrl->ctrl_intq.intq_addr == NULL) {
        crashdump(0);
    }
    m32_ctrl->ctrl_intq.num_q_elements = M32_INTQ_BLOCKS; 
    /*
     * initialize all elements of the interrupt queue.
     */
    intq_element_p = m32_ctrl->ctrl_intq.intq_addr;
    for (ix = 0; ix < M32_INTQ_ELEMENTS; ix++) {
       *intq_element_p = 0;
       intq_element_p++;
    }

    for (ix = 0; ix < M32_NUM_CHANNELS; ix++) {
        /*
         * setup all the timeslots to be turned off.
         * this also zeros out the bitrate field.
         */
        m32_ctrl->ctrl_timeslots[ix].recv_timeslot = M32_TIMESLOT_INHIBIT;
        m32_ctrl->ctrl_timeslots[ix].xmit_timeslot = M32_TIMESLOT_INHIBIT;
        /*
         * setup all the channel specifications.
         */
        m32_ctrl->ctrl_channels[ix].channel_info = 
          M32_CHANNEL_DOWN_MASK;
        m32_ctrl->ctrl_channels[ix].first_recv_desc = NULL;
        m32_ctrl->ctrl_channels[ix].first_xmit_desc = NULL;
        m32_ctrl->ctrl_channels[ix].channel_itbs = 0;
        /*
         * null out the current descriptor pointers.
         */
        m32_ctrl->ctrl_curr_recv_desc[ix] = NULL;
        m32_ctrl->ctrl_curr_xmit_desc[ix] = NULL;
    }
  
    return(m32_ctrl);
}

/*
 * =====================================================================
 * m32_init_channel_params()
 *
 * Description:
 * Initialize a Munich32 channel params structure. 
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - which channel this params struct is associated with
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void m32_init_channel_params (m32_instance_t *m32_ds,
				     ulong channel)
{
    m32_channel_params_t *params;

    params = &(m32_ds->channel_params[channel]);
    /*
     * fill in all the channel info fields
     */
    params->channel_num = channel;
    params->channel_idb = 0;
    params->bandwidth = 0;
    params->timeslot_bitfield = 0;
    params->ints_unused_channel = 0;
    params->throttle_count = 0;
    params->enable_count = 0;
    params->throttle_channel = TRUE;
    params->disable_adaptor = 
                   (m32_disable_adaptor_t)return_nothing;
    params->enable_adaptor = 
                   (m32_enable_adaptor_t)return_nothing;
    params->down_connection = 
                   (m32_down_connection_t)return_nothing;
    params->xmit_handler = (m32_xmit_handler_t)return_nothing;
    params->recv_handler = (m32_recv_handler_t)return_nothing;
    /* 
     * set states to M32_CHANNEL_UNUSED for all channels
     */
    params->initial_state = M32_CHANNEL_UNUSED;
    params->current_state = M32_CHANNEL_UNUSED;
    /* 
     * set all ring stuff to zero
     */
    params->rx_ring_size = 0;
    params->tx_ring_use_count = 0;
    params->tx_ring_size = 0;

    params->rx_ring_head = NULL;
    params->rx_ring_tail = NULL;
    params->rx_desc_throttle_hold = NULL;
    params->tx_ring_head = NULL;
    params->tx_ring_tail = NULL;

    params->rx_pak_head = NULL;
    params->rx_pak_tail = NULL;
    params->rx_pak_first = NULL;
    params->tx_pak_head = NULL;
    params->tx_pak_tail = NULL;
    params->tx_pak_first = NULL;

    params->runts_errors = 0x0;
    params->runt_started = 0x0;
    params->runt_interval = 0x0;
    params->drop_call = TRUE;
    params->first_start = TRUE;
    params->ar_failed = FALSE;
    params->idb_drop = 0x0;
}


/*
 * =====================================================================
 * m32_alloc_instance()
 *
 * Description:
 * Allocate and initialize a Munich32 instance structure. 
 *
 * Parameters:
 * none
 *
 * Returns:
 * a pointer to the m32 instance structure for this slot
 * =====================================================================
 */
static m32_instance_t *m32_alloc_instance (uint slot)
{
    m32_instance_t *m32_ds;
    ushort ix;

    /*
     * if the alloc fails, things are real bad, crashdump the system.
     */
    m32_ds = malloc_fast(sizeof(m32_instance_t));
    if (m32_ds == NULL) {
        crashdump(0);
    }

    m32_ds->chip_version = 0;
    m32_ds->intq_index = 0;
    m32_ds->ar_in_progress = 0;
    m32_ds->waiting_arack_in_interrupt = FALSE;

    m32_ds->idb_runts = 0x0;
    m32_ds->runts_count = 0;
    m32_ds->m32_pro_kill = FALSE;
    m32_ds->no_auto_check = TRUE;
    m32_ds->en_runts_check = TRUE;
    m32_ds->set_calldrop = FALSE;

    /*
     * don't assign an idle channel and don't set any
     * timeslots to idle. let the adaptor sw handle this.
     */
    m32_ds->idle_channel = M32_IDLE_CHNL_UNASSIGNED;
    m32_ds->idle_timeslots = 0;
    m32_ds->idle_idb = NULL;

    /*
     * initialize all the channel param structures.
     */
    for (ix = 0; ix < M32_NUM_CHANNELS; ix++) {
        m32_init_channel_params(m32_ds, ix);
    }

    return(m32_ds);
}


/*
 * =====================================================================
 * m32_ar_poll()
 *
 * Description:
 * Do a action request and poll to tell when it is done.
 * If this routine is called during startup, also update
 * the interrupt queue since the interrupt handler wont
 * be called.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 *
 * Returns:
 * M32_NOERR
 * M32_ERR_AR_TIMEOUT
 * M32_ERR_BAD_INTQ
 * =====================================================================
 */
static enum m32_error_t m32_ar_poll (m32_instance_t *m32_ds)
{
    boolean ar_status_found;
    enum m32_error_t status = M32_ERR_AR_TIMEOUT;
    ulong *intq_element_p = NULL,
          *intq_head = NULL;
    ushort spin_wait_count;

    /*
     * poll the action request status bit. when it is set, 
     * the action request is done. 
     */
    spin_wait_count = 0;
    while ((spin_wait_count < M32_AR_SPIN_WAIT_ITERATIONS) && 
           (status != M32_NOERR)) {
        /*
         * give the munich32 some time to complete the action request 
         */
        usecdelay(M32_AR_SPIN_WAIT_USEC);
        spin_wait_count++;

        if (m32_ds->regs->int_status_reg & M32_AR_INT) {
            /*
             * the action request done bit is set. clear the
             * bit by writing a value to the register with a 
             * zero in that bit location. any bits that need
             * to remain unchanged should have a one in that
             * bit position.
             */
            status = M32_NOERR;
            m32_ds->regs->int_status_reg &= (ushort)M32_CLEAR_AR_INT;
        }
    }

    /*
     * if the action request was done during system init
     * interrupts are still disabled. so we need to update
     * the interrupt queue manually. this is normally done
     * by the Munich32 interrupt service routine.
     */
    if (!systeminit_flag && status == M32_NOERR) {
        /*
         * give the munich32 some time to write out the interrupt element.
         */
        usecdelay(M32_AR_SPIN_WAIT_USEC * 100);
        /*
         * action request happened, get the head of the interrupt queue
         */
        intq_head = m32_ds->ctrl->ctrl_intq.intq_addr;
        intq_element_p = &(intq_head[m32_ds->intq_index]);

        /*
         * it hasn't already been found,
         * get the chip version number and store it. 
         */
        if (m32_ds->chip_version == 0) {
            m32_ds->chip_version = ((*intq_element_p) & M32_INT_VERSION) 
                >> M32_INT_VER_SHIFT;
        }

        /*
         * loop thru the int. queue until the action request
         * status element is found or until a zero'ed out 
         * element is found.
         */
        ar_status_found = FALSE;
        spin_wait_count = M32_AR_SPIN_WAIT_ITERATIONS;
        while ((ar_status_found == FALSE) && (spin_wait_count != 0)) {
            if (*intq_element_p == 0) {
                spin_wait_count--;
                usecdelay(M32_AR_SPIN_WAIT_USEC);
                continue;
            }
            if (*intq_element_p & M32_INT_ARACK) {
                /*
                 * action request succeeded 
                 */
                ar_status_found = TRUE;
                m32_ds->ar_success++;
            } else if (*intq_element_p & M32_INT_ARF) {
                /*
                 * action request failed 
                 */
                ar_status_found = TRUE;
                m32_ds->ar_failure++;
            }
            /*
             * zero out the just processed int queue element.
             * update the index into the interrupt queue. if the
             * previous element was the last element in the queue,
             * reset the index to the first element.
             * point to the next element.
             */
            *intq_element_p = 0;
            m32_ds->intq_index++; 
            if (m32_ds->intq_index >= M32_INTQ_ELEMENTS) {
                m32_ds->intq_index = 0;
                intq_element_p = intq_head;
            } else {
                intq_element_p++;
            }
        }
        if (ar_status_found == FALSE) {
            m32_ds->bad_intq_element_count++;
            status = M32_ERR_BAD_INTQ;
        }
    } 

    if (status != M32_NOERR)
        buginf("M32_AR_POLL fails\n");
    return(status);
}

/*
 * =====================================================================
 * m32_mark_ar_in_progress()
 *
 * Description:
 * Check if the interrupt AR in progress, if yes, do the polling in
 * the process level. No matter what the result of polling is, the
 * current configuration will continue 'cause we have waited long
 * enough for the previous AR.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 *
 * Returns:
 * M32_NOERR
 * =====================================================================
 */
enum m32_error_t m32_mark_ar_in_progress (m32_instance_t *m32_ds)
{
    enum m32_error_t status;

    /*
     * up a counter that warns that an action request
     * is in progress. only one action request is allowed
     * to be started at a time. this is required in case an
     * action request is attempted from interrupt level.
     */
    if (m32_ds->waiting_arack_in_interrupt) {
        /*
         * To avoid the race condition between process and interrupt
         * which may cause the process configuration fails, poll
         * the arack in the process level if any.
         */
        m32_ds->waiting_arack_in_interrupt = FALSE;
        status = m32_ar_poll(m32_ds);
    } else
        m32_ds->ar_in_progress++;
    return M32_NOERR;
}

/*
 * =====================================================================
 * m32_unmark_ar_in_progress_inline()
 *
 * Description:
 * Decrement the config-in-progress counter.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 *
 * Returns:
 * None.
 * =====================================================================
 */
static inline void m32_unmark_ar_in_progress_inline (m32_instance_t *m32_ds)
{
    /*
     * decrement the config-in-progress counter
     */
    if (m32_ds->ar_in_progress) {
        m32_ds->ar_in_progress--;
    }
}


/*
 * =====================================================================
 * m32_execute_ar()
 *
 * Description:
 * Execute an action request from the munich32 chip. Function
 * assumes it is this processes turn to do a action request
 * and all control struct data has been filled in.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel being configured
 *
 * Returns:
 * M32_NOERR
 * M32_ERR_AR_TIMEOUT
 * M32_ERR_ILLEGAL_CHANNEL
 * =====================================================================
 */
static enum m32_error_t m32_execute_ar (m32_instance_t *m32_ds, 
					ulong channel)
{
    enum m32_error_t status = M32_NOERR;
    ushort ix;

    /*
     * setup the action request word in the control struture. 
     */
    if (channel <= M32_NUM_CHANNELS) {
        m32_ds->ctrl->ctrl_action = 
          M32_AR_ONE_CHNL_MASK | (channel << M32_AR_CHANNEL_SHIFT) |
          m32_ds->pcm_format;        
    } else if (channel == M32_AR_ALL_CHANNELS) {
        m32_ds->ctrl->ctrl_action = M32_AR_ALL_CHNL_MASK |
          m32_ds->pcm_format;
    } else {
        status = M32_ERR_ILLEGAL_CHANNEL;
    }

    if (status == M32_NOERR) {      
        /*
         * read the action request register. this causes an
         * action request. the Munich32 loads the configuration 
         * info from the control structure.
         */
        m32_ds->regs->int_status_reg &= (ushort)M32_CLEAR_AR_INT;
        ix = m32_ds->regs->action_request_reg; 
        /*
         * poll the interrupt bit to see when the operation is done.
         */
        status = m32_ar_poll(m32_ds);
    }

    if (status == M32_ERR_AR_TIMEOUT) {
        m32_ds->ar_timeout++;
    }

    return(status);
}


/*
 * =====================================================================
 * m32_startup()
 *
 * Description:
 * perform power-on init of munich32 chip. setups all
 * data structures needed by the chip and supporting 
 * software. programs the chip and to put all channels 
 * in the UNUSED state.
 *
 * Parameters:
 * slot - slot number this card is sitting in
 *
 * Returns:
 * a pointer to the m32 instance structure for this slot
 * or NULL if the startup did not succeed.
 * =====================================================================
 */
m32_instance_t *m32_startup (uint slot, ulong subcont, ulong pcm_format)
{
    enum m32_error_t status = M32_NOERR;
    m32_instance_t *m32_ds = NULL;
    volatile m32_regs_t *reg_set;
    ushort ix;

    /*
     * allocate and initialize an instance structure
     * for this slot.
     */
    m32_ds = m32_alloc_instance(slot); /* slot is unsed */
    m32_ds->pcm_format = pcm_format;

    /*
     * allocate and initialize the control and
     * configuration structure for this slot.
     */
    m32_ds->ctrl = m32_alloc_ctrl(slot); /* slot is unsed */

    /*
     * set the instance struct to point to the m32 registers.
     */
    m32_ds->regs = m32_get_regs(slot, subcont);
    reg_set = m32_ds->regs;
    /*
     * clear the interrupt status register. set the
     * control register to the address of the control struct.
     */
    reg_set->int_status_reg = 0;
    reg_set->control_start_reg = m32_ds->ctrl;

    /*
     * read the action request register. this causes the
     * Munich32 to load the configuration info from the 
     * control structure. the config info was setup when
     * the control struct was allocated.
     */
    m32_ds->ctrl->ctrl_action = M32_AR_STARTUP_MASK | m32_ds->pcm_format;
    ix = reg_set->action_request_reg; 

    /*
     * poll action request status bit waiting for the action
     * request to finish.
     */
    status = m32_ar_poll(m32_ds);

    if (status == M32_NOERR) {
        /*
         * set the interrupt vector to the wrapper for this 
         * device in this slot.
         */
        m32_set_vector(slot);
        /*
         * enable Munich32 interrupts.
         */
        reg_set->int_enable_reg = M32_REG_INT_ENABLE;
    } else {
        /*
         * major problem with the munich32 startup.
         * reset the chip and return a NULL. this
         * tells the calling function that something
         * went very wrong with initialization.
         */
        buginf("\n\nM32 initialization failed\n\n");
        m32_reset_chip(slot);
        m32_ds = NULL;
    }

    return(m32_ds);
} 


/*
 * =====================================================================
 * m32_update_intq_inline()
 *
 * Description:
 * zero out the just processed int queue element.
 * update the index into the interrupt queue. if the
 * previous element was the last element in the queue,
 * reset the index to the first element.
 *
 * Parameters:
 * intq_head - pointer to the head of the interrupt queue
 * intq_index - array index to current element
 * intq_element - pointer to current element
 *
 * Returns:
 * Returns:
 *
 * Notes:
 * this function must precede m32_interrupt() because it's an inline.
 * =====================================================================
 */
static inline void m32_update_intq_inline (ulong *intq_head,
                                           ulong *intq_index,
                                           ulong *intq_element)
{
    /* 
     * zero out the just processed interrupt element
     */
    intq_head[*intq_index] = 0;
    /* 
     * if this was the last element in the list go 
     * back to the start
     */
    if (++(*intq_index) >= M32_INTQ_ELEMENTS) {
        (*intq_index) = 0;
    }
    *intq_element = intq_head[*intq_index];
}


/*
 * =====================================================================
 * m32_getbuf_inline()
 *
 * Description:
 * Get a buffer for a Munich32 ring. Try the cache first, if no
 * cache buffers are available get one from the buffer pool.
 *
 * Parameters:
 * idb
 *
 * Returns:
 * pointer to a packet if successful
 * NULL if buffer alloc failed
 * =====================================================================
 */
static inline paktype *m32_getbuf_inline (hwidbtype *idb, ulong data_offset,
					  boolean cache_ok)
{
    paktype *pak = NULL;

    /*
     * Check for input congestion
     */
    if (idb->input_qcount > idb->input_defqcount)
      return(NULL);

    /*
     * If cache_ok is set to be TRUE when the inline is used, this
     * code will be compiled in. *NO* conditional will be generated.
     */
    if (cache_ok) {
        /*
         * Try to get a cached buffer. if all cache buffers are
         * in use, get a buffer from the buffer pool.
         */
	pak = pool_dequeue_cache(idb->pool);
    }

    /*
     * couldn't get a cache buffer, try the buffer pool.
     */
    if (!pak) {
        pak = pool_getbuffer(idb->pool);
    }
    if (pak) {
        /* 
         * got the packet, setup a pointer to the 
         * start of the data. make sure it's longword
         * aligned. the chip requires this.
         */
        pak->datagramstart = pak->data_area + data_offset;
        (ulong)pak->datagramstart &= BUFFER_LONGWORD_ALIGN;
    }
    return (pak);
}


/*
 * m32_getbuf
 *
 * Function call wrapper around inline function
 */
static paktype *m32_getbuf (hwidbtype *idb, ulong data_offset)
{
    return(m32_getbuf_inline(idb, data_offset, FALSE));
}


/*
 * =====================================================================
 * m32_process_err_int()
 *
 * Description:
 * Process a Munich32 error interrupt. The interrupt queue element
 * is examined to see which error occurred and the appropriate
 * idb counter is up'ed.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * idb - pointer to hw idb associated with the error
 * intq_element - element of int. queue, written by munich32
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void m32_process_err_int (m32_instance_t *m32_ds, 
				 hwidbtype *idb, 
				 ulong intq_element)
{
    ulong                status;
    ulong                now;
    short                error_msec;
    m32_channel_params_t *channel_params;
    enum m32_error_t     result = M32_NOERR;

    channel_params = &m32_ds->channel_params[idb->m32_channel];

    if (idb == NULL) {
        return;
    }

    error_msec = 0x0;
    if (intq_element & M32_INT_FO) {
        /*
         * process an overflow condition
         */
        if (intq_element & M32_INT_RT_RECV) {
            idb->counters.input_error++;
            idb->counters.input_overrun++;
        } else {
            idb->counters.output_error++;
            idb->counters.output_underrun++;
            idb->dma_memory_error++;
        }
    }

    if (intq_element & M32_INT_ERR) {
        /*
         * process an Error condition
         */
        if (intq_element & M32_INT_RT_RECV) {
            /*
             * receive error, up the general counter
             */
            idb->counters.input_error++;

            /*
             * get the status out of the receive descriptor.
             * the status is only valid if the fe bit is set
             */
            status =
            channel_params->rx_ring_head->recv_status;
            if ((status & M32_RECV_DESC_STATUS) &&
                (status & M32_RECV_DESC_FE) &&
                (status & M32_RECV_DESC_C)) {
                /*
                 * classify the receive error and up
                 * the specific input error count.
                 */
                if (status & M32_RECV_STATUS_LOSS) {
                    idb->counters.input_frame++;
                }
                if ((status & M32_RECV_STATUS_CRC) &&
                    !(status & M32_RECV_STATUS_LFD) &&
                    !(status & M32_RECV_STATUS_NOB)) {
                    idb->counters.input_crc++;
                }
                if ((status & M32_RECV_STATUS_NOB) &&
                    !(status & M32_RECV_STATUS_LFD)) {
                    idb->counters.input_frame++;
                }
                if (status & M32_RECV_STATUS_LFD) {
                    idb->counters.giants++;
                }
                if (status & M32_RECV_STATUS_RA) {
                    idb->counters.input_abort++;
                }
                if (status & M32_RECV_STATUS_ROF) {
                    idb->counters.input_resource++;
                    idb->counters.input_overrun++;
                }
            }

            if ((intq_element & M32_INT_SF) &&
                (intq_element & M32_INT_FI)) {
                idb->counters.runts++;
                /*
                 * The no_auto_check & en_runts_check are used as two states for
                 * software to determine where the code is right now.
                 * no_auto_check & en_runts_check = 11 means we start error
                 * checking mechanism for excessive runts error
                 * no_auto_check & en_runts_check = 10 means we detected excessive
                 * runts error and put T1 into loopback mode
                 * no_auto_check & en_runts_check = 01 means we start errors
                 * auto-recovery every 15 seconds to see the errors go away
                 * no_auto_check & en_runts_check = 00 means autorecovery failed
                 * and process go to sleep for 15 seconds
                 *
                 * The error detection mechanism is runt_started in each B channel
                 * is used to timestamp the starting time(t1) of error checking
                 * interval. The end of interval(t2) is when errors count reach 30.
                 * So the delta time is t2-t1. If 30/(t2-t1) is greater than 300
                 * we add this delta time to runt_interval. If runt_interval is
                 * greater than 1 second, we drop the call on that B channel and
                 * change drop_call to FALSE. We also save t2 above to runt_started
                 * so when next runt error from that B channel occurs, we can use
                 * t2 as the starting time of error checking interval. Thus we can
                 * keep checking error rate in a consecutive fashion. If in
                 * unprovision case, the error rate should more than 300. So
                 * the runt_interval keep increment until greater than 3 seconds
                 * then runt_started,runts_errors & runt_interval in each B channel
                 * will be reset to 0 and T1 will be put into loopback mode. If
                 * in per B channel case, the runt_started wasn't zero, so if next
                 * runt error from that B channel occurs, the error rate should not
                 * exceed 300 since the delta time should be longer. Thus
                 * runt_started, runts_errors & runt_interval will be reset to 0.
                 * Error checking on that B channel will be start all over again.
                 */
                if (m32_ds->no_auto_check && m32_ds->en_runts_check) {
                   channel_params->runts_errors++;
                   if (channel_params->ar_failed) {
                      result = m32_tristate_isdn_b_channel(m32_ds,idb->m32_channel);
                      if (result ==  M32_NOERR)
                         channel_params->ar_failed = FALSE;
                   }
                   if ((channel_params->first_start) && (channel_params->runts_errors == 1)) {
                     GET_TIMESTAMP32(channel_params->runt_started);
                     channel_params->first_start = FALSE;
                   } else if (channel_params->runts_errors > M32_RUNT_THRESHOLD) {
                     GET_TIMESTAMP32(now);
                     error_msec = now - (channel_params->runt_started);
                     if ((!error_msec) ||
                              ((((channel_params->runts_errors - 1) * 1000) / error_msec) > M32_ERR_HIGH_MARK)) {
                        channel_params->runt_started = now;
                        channel_params->runt_interval += error_msec;
                        if ((((channel_params->runt_interval) / 1000) > CHAN_INTERVAL) && channel_params->drop_call) {
                           if ((idb->state == IDBS_UP) && (idb->isdn_info) &&
                                    (idb->isdn_info->chantype != ISDN_Dchan)) {
                              result = m32_tristate_isdn_b_channel(m32_ds, idb->m32_channel);
                              if (result == M32_ERR_AR_FAILURE)
                                 channel_params->ar_failed = TRUE;
                              if (!m32_ds->set_calldrop) {
                                 m32_ds->set_calldrop = TRUE;
                                 process_set_boolean(calldrop_process_awaken, TRUE);
                              }
                              channel_params->idb_drop = idb;
                           }
                           channel_params->drop_call = FALSE;
                        } else if ((((channel_params->runt_interval) / 1000) > M32_INTERVAL) && (!channel_params->drop_call)) {
                           m32_ds->idb_runts = idb;
                           m32_ds->en_runts_check = FALSE;
                           if (m32_ds->runts_handle)
                              (*m32_ds->runts_handle)(idb);
                           process_set_boolean(m32print_process_awaken, TRUE);
                        }
                     } else {
                       channel_params->runt_interval = 0x0;
                       channel_params->runt_started = 0x0;
                       channel_params->drop_call = TRUE;
                       channel_params->first_start = TRUE;
                       channel_params->ar_failed = FALSE;
                     }
                     channel_params->runts_errors = 0x0;
                   }
                } else if ((!m32_ds->no_auto_check) && m32_ds->en_runts_check
                                          && (idb == m32_ds->idb_runts)) {
                   channel_params->runts_errors++;
                   if (channel_params->runts_errors == 1) {
                      GET_TIMESTAMP32(channel_params->runt_started);
                   } else {
                      GET_TIMESTAMP32(now);
                      error_msec = now - (channel_params->runt_started);
                      /*
                       * Here means if one error occurs within 200ms then we need
                       * to retry again.
                       */
                      if (error_msec && ((((channel_params->runts_errors - 1) * 1000) / error_msec) < M32_ERR_LOW_MARK)) {
                        channel_params->runt_interval += error_msec;
                        if (((channel_params->runt_interval) / 1000) >  M32_INTERVAL) {
                           m32_ds->m32_pro_kill = TRUE;
                           channel_params->runt_interval = 0x0;
                           channel_params->runt_started = 0x0;
                           channel_params->runts_errors = 0x0;
                        }
                      } else {
                        channel_params->runt_interval = 0x0;
                        channel_params->runt_started = 0x0;
                        channel_params->runts_errors = 0x0;
                        m32_ds->en_runts_check = FALSE;
                        if (m32_ds->runts_handle)
                           (*m32_ds->runts_handle)(idb);
                      }
                   }
                }
            }
            if (intq_element & M32_INT_FO) {
                idb->counters.input_resource++;
                idb->counters.input_overrun++;
            }
        } else {
            /*
             * transmit error
             */
            idb->counters.output_error++;
        }
    }  
}


/*
 * =====================================================================
 * m32_drop_pkt()
 *
 * Description:
 * do cleanup in the pak struct and the idb for a dropped packet.
 *
 * Parameters:
 * idb - pointer to a hardware idb structure
 * pak - pointer to a pak structure
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void m32_drop_pkt (hwidbtype *idb,
			  paktype *pak)
{
    if (pak)
        clear_if_input(pak);
    idb->counters.input_drops++;
    MAYBE_DROPPED(idb);
}


/*
 * =====================================================================
 * m32_throttle()
 *
 * Description:
 * go through the control struct and look for all timeslots that
 * are assinged to the channel to throttle. when such a timeslot
 * is found, set the receive inhibit bit to prevent the timeslot
 * from receiving any data. if any chip configuration is going on
 * throttling will not happen. the munich32 does not allow a
 * configuration atttempt while one is already in progress.
 * throttling is turned off by a background task.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel being configured
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void m32_throttle (m32_instance_t *m32_ds, ulong channel)
{
    hwidbtype *idb;

    m32_ds->channel_params[channel].current_state = M32_CHANNEL_THROTTLED;
    m32_ds->channel_params[channel].throttle_count++;

    idb = m32_get_channel_idb_inline(m32_ds, channel);
    idb_mark_throttled(idb);
}


/*
 * =====================================================================
 * m32_interrupt()
 *
 * Description:
 * The Munich32 puts one or more interrupt elements on the interrupt
 * queue every interrupt. This interrupt handler loops through
 * the interrupt queue processing the interrupts. It may 
 * process a receive or transmit interrupt, an action request 
 * interrupt, or an error interrupt generated by the Munich32. 
 * For recieve and transmit interrupts, update the rings, handle
 * the buffers and pass the packet to the port adaptor handlers.
 * For error interrupts, update the appropriate counters and drop
 * the packet if necessary. For action request interrupts, update 
 * the appropriate counters.
 *
 * Parameters:
 * slot - slot number of the card that interrupted
 *
 * Returns:
 * nothing
 * =====================================================================
 */
void m32_interrupt (ulong slot, ulong subcont)
{
    m32_channel_params_t *channel_params;
    m32_instance_t *m32_ds;
    ulong channel,
          intq_element, 
         *intq_head,
         *intq_index;
    ushort iterations;

    /*
     * get the m32 instance structure and the
     * pointer to the head of the interrupt queue.
     */
    m32_ds = M32_INSTANCE(slot, subcont);
    intq_head = m32_ds->ctrl->ctrl_intq.intq_addr;
    iterations = 0;
    /*
     * get a pointer to the next interrupt queue element
     * to be serviced.
     */
    intq_index = &(m32_ds->intq_index);
    intq_element = intq_head[*intq_index];

    /*
     * loop thru the interrupt queue processing the elements.
     * a NULL element is the end of the elements to be processed.
     */
    while ((iterations++ < M32_INTS_TO_PROCESS) && (intq_element != 0)) {
        hwidbtype *idb;
        /*
         * sanity check. all interrupt elements should have the
         * interrupt bit set.
         */
        if ((intq_element & M32_INT_INT) == 0) {
            /*
             * log bad interrupt q element error then
             * zero out the processed int queue element 
             * and move to the next element.
             */
            m32_ds->bad_intq_element_count++;
            m32_update_intq_inline(intq_head, intq_index, &intq_element);
            continue;
        }

        /*
         * good interrupt queue element, extract
         * needed information out of it.
         */
        channel = intq_element & M32_INT_CHANNEL;
        channel_params = &(m32_ds->channel_params[channel]);
        idb = channel_params->channel_idb;
        /*
         * check bits in the interrupt element to 
         * identify what kind of interrupt this is.
         */
        if (intq_element & M32_INT_ARACK) {
            /*
             * process a successful Action Request then
             * zero out the processed int queue element 
             * and move to the next element.
             */
            m32_ds->ar_success++;
            m32_update_intq_inline(intq_head, intq_index, &intq_element);
            if (m32_ds->waiting_arack_in_interrupt && m32_ds->ar_in_progress) {
                m32_ds->waiting_arack_in_interrupt = FALSE;
                m32_ds->ar_in_progress--;
            }
            continue;
        } 
        if (intq_element & M32_INT_ARF) {
            /*
             * process a failed Action Request then
             * zero out the processed int queue element 
             * and move to the next element.
             */
            m32_ds->ar_failure++;
            m32_update_intq_inline(intq_head, intq_index, &intq_element);
            if (m32_ds->waiting_arack_in_interrupt && m32_ds->ar_in_progress) {
                m32_ds->waiting_arack_in_interrupt = FALSE;
                m32_ds->ar_in_progress--;
            }
            continue;
        } 

        if ((channel_params->current_state == M32_CHANNEL_UNUSED) ||
            (channel_params->current_state == M32_CHANNEL_IDLE)) {
            /*
             * got an interrupt for a channel that's no longer
             * in use. zero out the int queue element and move 
             * to the next element.
             */
            channel_params->ints_unused_channel++;
            m32_update_intq_inline(intq_head, intq_index, &intq_element);
            continue;
        }

        /*
         * the following bits in the interrupt element can
         * be set in combination with other bits. so they
         * must be checked sepeately.
         */
        if ((intq_element & M32_INT_FO) ||
            (intq_element & M32_INT_ERR)) {
            m32_process_err_int(m32_ds, idb, intq_element);
        }

        if (intq_element & M32_INT_HI) {

            m32_recv_desc_t *rx_ring_head, 
                            *rx_ring_tail;
            paktype *pak = NULL, *new_pak = NULL;
            boolean pak_dropped = FALSE;
            /*
             * make sure the channel is still in use before 
             * processing the interrupt. if not, just clear 
             * the int q element and continue.
             */
            /*
             * process a host initiated interrupt in
             * either the receive or transmit direction.
             */
            switch (intq_element & M32_INT_RT) {
            case M32_INT_RT_RECV:  /* receive interrupt */

                rx_ring_head = channel_params->rx_ring_head;
                rx_ring_tail = channel_params->rx_ring_tail;

                if (channel_params->current_state == M32_CHANNEL_UP) {

                    if (sched_run_interval) {
                        /*
                         * if the scheduler needs to run and this
                         * channel can be throttled, turn off recv'ing 
                         * pkts on this channel
                         */
                        if ((AWAKE(sched_run_time)) && 
                            (channel_params->throttle_channel)) {
                            m32_throttle(m32_ds, channel);
                            channel_params->rx_desc_throttle_hold 
                                = channel_params->rx_ring_tail;
                            pak_dropped = TRUE;
                        }
                    } 

                    if (intq_element & M32_INT_ERR) {
                        /* 
                         * this is a bad packet, drop it, 
                         */
                        pak_dropped = TRUE;
                    } else if (!pak_dropped) {
                        /*
                         * get a buffer to replace the just received pkt
                         */
                        new_pak = m32_getbuf_inline(idb,
						    channel_params->data_offset, TRUE); 
                        if (new_pak) {
                            /*
                             * get a ptr to the packet and assign the 
                             * data size based on the value in the 
                             * recv. descriptor
                             */
                            pak = channel_params->rx_pak_head->pak;
                            pak->datagramsize = 
                              (rx_ring_head->recv_status & 
                               M32_RECV_DESC_BNO) 
                               >> M32_RECV_DESC_BNO_SHIFT;
                            /*
                             * set input params in the idb
                             */
                            set_if_input(pak, idb->firstsw);
                            /*
                             * replace the recv'ed pak with the new buffer
                             */
                            channel_params->rx_pak_head->pak = new_pak;
                            rx_ring_head->recv_data_ptr = 
                              new_pak->datagramstart; 
 
                        } else {
                            /* 
                             * couldn't get a new buffer,  break the 
                             * connection and throttle if possible
                             */
                            (*channel_params->down_connection)(idb);
                            if (channel_params->throttle_channel) {
                                m32_throttle(m32_ds, channel);
                                channel_params->rx_desc_throttle_hold 
                                    = channel_params->rx_ring_tail;
                            }
                            pak_dropped = TRUE;
                        }
                    }
                } else {
                    /*
                     * Drop rest of packets which is rxed not under the
                     * M32_CHANNEL_UP state.
                     */
                    pak_dropped = TRUE;
                }

                /*
                 * update the just processed descriptor by 
                 * clearing bits and setting descriptor fields 
                 * to initial values.
                 */
                rx_ring_head->recv_status = M32_RECV_STATUS_INITIAL;
                /*
                 * update the ring heads to the next element
                 * and set the hold bit. this descriptor will 
                 * be the new tail.
                 */
                if(channel_params->current_state != M32_CHANNEL_THROTTLED){
                    rx_ring_tail->next_recv_desc->recv_params 
                                              |= M32_RECV_DESC_HOLD; 
                    rx_ring_tail->recv_params &= ~(M32_RECV_DESC_HOLD); 
                }

                channel_params->rx_ring_head = 
                  rx_ring_head->next_recv_desc;
                channel_params->rx_pak_head =
                  channel_params->rx_pak_head->next_pak;
                /*
                 * clear the hold bit in the tail and update
                 * the tail pointers.
                 */
                channel_params->rx_ring_tail = 
                  rx_ring_tail->next_recv_desc;
                channel_params->rx_pak_tail =
                  channel_params->rx_pak_tail->next_pak;

                if (!pak_dropped) {
                    /*
                     * execute the receive interrupt handler for
                     * the port adaptor software
                     */
                    (*channel_params->recv_handler)(idb, pak);
                } else {
                    m32_drop_pkt(idb, pak);
                    pak_dropped = FALSE;
                }

                break;

            case M32_INT_RT_XMIT:  /* transmit interrupt */
                /*
                 * don't service this interrupt if the use
                 * count is zero. this is needed because when
                 * a transmit ring is initialized a spurious
                 * transmit interrupt is generated.
                 */
                if (channel_params->tx_ring_use_count) {
                    m32_xmit_desc_t *tx_tail;
                    m32_pak_ring_t *tx_pak_tail;
                    /*
                     * move the transmit tail up one for the 
                     * descriptor and packet rings.
                     */
                    channel_params->tx_ring_tail =
                      channel_params->tx_ring_tail->next_xmit_desc;
                    tx_tail = channel_params->tx_ring_tail;
                    channel_params->tx_pak_tail =
                      channel_params->tx_pak_tail->next_pak;
                    tx_pak_tail = channel_params->tx_pak_tail;
                    /*
                     * clean up the transmit descriptor.
                     * decrement the use count for this ring.
                     */
                    tx_tail->xmit_params &= ~(M32_XMIT_DESC_NO);
                    tx_tail->xmit_data_ptr = NULL;
                    channel_params->tx_ring_use_count--;
                    /*
                     * execute the transmit interrupt handler 
                     * for the port adaptor code
                     */
                    (*channel_params->xmit_handler)(idb, 
                                               tx_pak_tail->pak);
                    /*
                     * free the just transmitted buffer
                     */
                    datagram_done(tx_pak_tail->pak);
                    tx_pak_tail->pak = NULL;
                }
                break;
            } /* switch */
        } /* if HI interrupt */

        /*
         * zero out the processed int queue element and move
         * to the next element.
         */
        m32_update_intq_inline(intq_head, intq_index, &intq_element);

    } /* loop thru interrupt queue */
}


/*
 * =====================================================================
 * m32_fastsend()
 *
 * Description:
 * Fastswitch a packet out Munich32 channel.
 *
 * Parameters:
 * idb - interface to transmit the packet out
 * pak - packet to transmit
 *
 * Returns:
 * nothing
 * =====================================================================
 */
void m32_fastsend (hwidbtype *idb, 
                   paktype *pak)
{ 
    m32_channel_params_t *channel_params;
    m32_instance_t *m32_ds;
    m32_xmit_desc_t *prev_tx_ring_head,
                    *tx_ring_head;
    traffic_shape_t *shape;
    ushort bytes;

    m32_ds = M32_INSTANCE(idb->slot, idb->subcont);
    channel_params = &(m32_ds->channel_params[idb->m32_channel]);

    /*
     * determine whether there is window available to send the message with
     * in the rate control bucket
     */
    shape = traffic_shape_required_inline(pak->if_output->traffic_shape, pak);
    if (shape) {
	traffic_shape_defer_sending(shape, pak);
	return;
    }

    if ((channel_params->current_state == M32_CHANNEL_UP)
        || (channel_params->current_state == M32_CHANNEL_THROTTLED)){
        /*
         * if there are no packets on the hold queue and there
         * is room on the transmit ring transmit the packet
         * directly out the interface.
         */
        if ((idb->output_qcount == 0) &&
            (channel_params->tx_ring_use_count <
             channel_params->tx_ring_size - 1)) {
            /*
             * make sure the number of bytes to send does not
             * exceed the MTU for this channel.
             */
            bytes = pak->datagramsize;
            if (bytes > idb->max_pak_size) {
                idb->counters.output_total_drops++;
                datagram_done(pak);
                return;
            }
            /*
             * do packet accounting, count broadcasts.
             */
            idb->counters.tx_cumbytes_fast[pak->acct_proto] += bytes;
            idb->counters.outputs_fast[pak->acct_proto]++;
            if ((pak->enctype == ET_HDLC) &&
                (GETSHORT(pak->datagramstart) & HDLC_BCAST)) {
                idb->counters.output_broad++;
            }

            /*
             * move the head pointer to the next descriptor in the ring.
             * record that another descriptor is in use.
             */
            prev_tx_ring_head = channel_params->tx_ring_head;
            channel_params->tx_ring_head =
                channel_params->tx_ring_head->next_xmit_desc;
            tx_ring_head = channel_params->tx_ring_head;
            channel_params->tx_pak_head =
                channel_params->tx_pak_head->next_pak;
            /*
             * Set up this TX ring entry for this packet. assign
             * the packet to the descriptor and set the status bits.
             */
            channel_params->tx_pak_head->pak = pak;
            tx_ring_head->xmit_data_ptr = pak->datagramstart;
            channel_params->tx_ring_use_count++;

            /*
             * Set transmit timeout timer.
             */
            TIMER_START(idb->xmittimer, TIMEOUT);

            /*
             * set the hold bit and the number of bytes to transfer
             * in the new head descriptor. clear the hold bit on the
             * previous head descriptor. this causes the data in the
             * new head descriptor to be sent.
             */
            tx_ring_head->xmit_params |=
                ((bytes << M32_XMIT_DESC_NO_SHIFT) | M32_XMIT_DESC_HOLD);
            prev_tx_ring_head->xmit_params &= ~(M32_XMIT_DESC_HOLD);
        } else {
            /*
             * There are already packets on the output hold queue or
             * the transmit ring is full. queue this packet to the
             * end of the queue to minimize out of order situations.
             */
            pak->if_output = idb->firstsw;
            holdq_enqueue_fast(idb, pak);
            m32_tx_start(idb);
        }
    }
    else {
        idb->counters.output_total_drops++;
        datagram_done(pak);
    }
  }


/*
 * =====================================================================
 * m32_alloc_idle_idb()
 *
 * Description:
 * Create an idb for an idle channel. This is required so the idle
 * channel's rings can be stocked with buffers. Becaue the idle
 * channel never receives or transmits data, the idle channel idb
 * is not queue'ed onto the idb queue. The idle channel is not a
 * true interface.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 *
 * Returns:
 * pointer to a hw idb
 *
 * Notes:
 * This function requires that the idle channel be stored in the
 * instance structure.
 * =====================================================================
 */
static void m32_alloc_idle_idb (m32_instance_t *m32_ds)
{
    /*
     * free the old idb. this is possible since this idb is not
     * used to represent an interface that actually receives
     * and transmits data. this idb is for the idle channel.
     */
    if (m32_ds->idle_idb) {
        m32_ds->idle_idb->vc = m32_ds->idle_channel;
        m32_ds->idle_idb->m32_channel = m32_ds->idle_channel;
        return;
    }
    /*
     * create and initialize a new idle channel idb.
     */
    m32_ds->idle_idb = idb_create();
    if (m32_ds->idle_idb != NULL) {
        m32_ds->idle_idb->vc = m32_ds->idle_channel;
        m32_ds->idle_idb->m32_channel = m32_ds->idle_channel;
        m32_ds->idle_idb->status = IDB_SERIAL | IDB_BCAST | IDB_HEARSELF |
	    IDB_BRIDGE | IDB_SYNC;
        m32_ds->idle_idb->typestring = "M32_IDLE";
        m32_ds->idle_idb->soutput = m32_tx_start;
        m32_ds->idle_idb->fastsend = m32_fastsend;
        serial_idb_init(m32_ds->idle_idb, MAXSERIALSIZE, PAN_MAXDGRAM,
			VISIBLE_LES_SERIAL_BANDWIDTH, LES_SERIAL_DELAY);
        m32_ds->idle_idb->pool =
	    pak_pool_find_by_size(m32_ds->idle_idb->max_buffer_size);
        m32_ds->idle_idb->xmitdelay = 0;
    }
}


/*
 * =====================================================================
 * m32_adjust_idle_channel()
 *
 * Description:
 * Adjust the timeslots assigned to an idle channel. This
 * function should be called when a channel is created or
 * destroyed because these operations allocate and free
 * idle timeslots.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static enum m32_error_t m32_adjust_idle_channel (m32_instance_t *m32_ds)
{
    enum m32_error_t status; 
    m32_channel_init_info_t channel_init;

    status = M32_NOERR;

    if (m32_ds->idle_channel < M32_NUM_CHANNELS) {
        if (m32_ds->idle_timeslots) {
            /*
             * setup the init struct.
             */
            channel_init.channel_num = m32_ds->idle_channel;
            channel_init.data_offset = 0;
            channel_init.channel_idb = m32_ds->idle_idb;
            channel_init.bitrate = 64;
            channel_init.throttle_channel = FALSE;
            channel_init.loopback = M32_NO_LOOPBACK;
            channel_init.rx_ring_size = 4;
            channel_init.tx_ring_size = 4;
            channel_init.xmit_handler = (m32_xmit_handler_t)return_nothing;
            channel_init.recv_handler = (m32_recv_handler_t)return_nothing;
            channel_init.disable_adaptor = 
                                  (m32_disable_adaptor_t)return_nothing;
            channel_init.enable_adaptor = 
                                  (m32_enable_adaptor_t)return_nothing;
            channel_init.down_connection = 
                                  (m32_down_connection_t)return_nothing;
            channel_init.initial_state = M32_CHANNEL_IDLE;
            channel_init.timeslot_bitfield = m32_ds->idle_timeslots;
            channel_init.internal_buf_size = 2;
            /*
             * now init the channel.
             */
            status = m32_init_channel(m32_ds, &channel_init);
        } else {
            m32_release_channel(m32_ds, m32_ds->idle_channel);
        }
    }
    
    return(status);
}


/*
 * =====================================================================
 * m32_create_idle_channel()
 *
 * Description:
 * Find the first unused channel and make it the idle channel.
 * Start with the highest channel number and work down. If all 
 * channels are in use return a value to reflect that.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 *
 * Returns:
 * nothing
 * =====================================================================
 */
enum m32_error_t m32_create_idle_channel (m32_instance_t *m32_ds)
{
    enum m32_error_t status;
    short ix;

    status = M32_NOERR;

    if (m32_ds->idle_channel == M32_IDLE_CHNL_UNASSIGNED) {
        /*
         * loop thru all channels looking for first available channel.
         */
        for (ix = M32_NUM_CHANNELS - 1; ix >= 0; ix--) {
            if (m32_ds->channel_params[ix].current_state ==
		M32_CHANNEL_UNUSED) { 
                /*
                 * record the idle channel in the instance struct.
                 */
                m32_ds->idle_channel = ix;
                /*
                 * create an idb for the idle channel if one isn't
                 * already created. idle channel idb's are reused.
                 */
                m32_alloc_idle_idb(m32_ds);
                if (m32_ds->idle_idb == NULL) {
                    m32_ds->idle_channel = M32_IDLE_CHNL_UNASSIGNED;
                    return(M32_ERR_MEM_ALLOC);
                } 
                /*
                 * initialize the idle channel.
                 */
                m32_adjust_idle_channel(m32_ds);
                break;
            }
        }
    } 
  
    return(status);
}


/*
 * =====================================================================
 * m32_free_buffers()
 *
 * Description:
 * Free all the  buffers in this channel's receive and transmit rings.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel that is freeing its buffers
 *
 * Returns:
 * m32 error code
 * =====================================================================
 */
static void m32_free_buffers (m32_instance_t *m32_ds,
			      ulong channel)
{
    volatile m32_channel_ctrl_t *channel_control;
    m32_channel_params_t *channel_params;
    m32_pak_ring_t *pak_ring;
    m32_recv_desc_t *rx_descriptor;
    m32_xmit_desc_t *tx_descriptor;
    ulong i;
    ulong *intq_head, intq_index;
    paktype *pak;
    hwidbtype *hwidb;
 
    channel_control = &(m32_ds->ctrl->ctrl_channels[channel]);
    channel_params = &(m32_ds->channel_params[channel]);

    intq_head = m32_ds->ctrl->ctrl_intq.intq_addr;
    intq_index = m32_ds->intq_index;

    /*
     * get a pointer to the first receive descriptor
     */
    rx_descriptor = channel_control->first_recv_desc;
    pak_ring = channel_params->rx_pak_first;

    /*
     * if the descriptors are allocated, go thru
     * the receive descriptor list and free the buffers.
     */
    if (pak_ring) {
        do {
            /*
             * if a buffer is allocated, free it.
             */
            if (pak_ring->pak != NULL) {
                datagram_done(pak_ring->pak);
                pak_ring->pak = NULL;
                rx_descriptor->recv_data_ptr = NULL;
            }
            /*
             * go on to the next ring entry.
             */
            pak_ring = pak_ring->next_pak;
            rx_descriptor = rx_descriptor->next_recv_desc;            
        } while (rx_descriptor != channel_control->first_recv_desc);

        if(serial_debug && (pak_ring != channel_params->rx_pak_first))
            buginf("\nThere are memory leaks in the M32's rx ring.");
    }

    /*
     * get a pointer to the first transmit descriptor
     */
    tx_descriptor = channel_control->first_xmit_desc;
    pak_ring = channel_params->tx_pak_first;
    /*
     * if the descriptors are allocated, go thru
     * the transmit descriptor list and free the buffers.
     */
    if (pak_ring) {
        do {
            /*
             * if a buffer is allocated, free it.
             */
            if (pak_ring->pak != NULL) {
                datagram_done(pak_ring->pak);
                pak_ring->pak = NULL;
                tx_descriptor->xmit_data_ptr = NULL;
            }
            /*
             * go on to the next ring entry.
             */
            pak_ring = pak_ring->next_pak;
            tx_descriptor = tx_descriptor->next_xmit_desc;
        } while (tx_descriptor != channel_control->first_xmit_desc);

        if(serial_debug && (pak_ring != channel_params->tx_pak_first))
            buginf("\nThere are memory leaks in the M32's tx ring.");
    }

    for (i=M32_INTS_TO_PROCESS; i; i--) {
        if ((intq_head[intq_index] & M32_INT_HI) &&
            ((intq_head[intq_index] & M32_INT_CHANNEL) == channel)){
            intq_head[intq_index] &= ~M32_INT_HI;
        }
 
        if (++intq_index == M32_INTQ_ELEMENTS)
           intq_index = 0;
    }

    hwidb = m32_get_channel_idb_inline(m32_ds, channel);
    if (hwidb) {
        pak = (*hwidb->oqueue_dequeue)(hwidb);
        while (pak) {
            hwidb->counters.output_total_drops++;
            datagram_done(pak);
            pak = (*hwidb->oqueue_dequeue)(hwidb);
        }        
    }
}


/*
 * =====================================================================
 * m32_alloc_rx_buffers()
 *
 * Description:
 * alloc buffers for the receive ring
 * loop thru the pak and descriptor rings. if an entry has a
 * bufffer already, free it. then allocate a new buffer for it.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel that needs buffers
 *
 * Returns:
 * M32_NOERR
 * M32_ERR_MEM_ALLOC
 * =====================================================================
 */
static enum m32_error_t m32_alloc_rx_buffers (m32_instance_t *m32_ds,
					      ulong channel)
{
    hwidbtype *idb;
    m32_channel_params_t *channel_params;
    enum m32_error_t status = M32_NOERR;
    m32_recv_desc_t *rx_descriptor;
    m32_pak_ring_t *pak_ring;

    idb = m32_get_channel_idb_inline(m32_ds, channel);
    channel_params = &(m32_ds->channel_params[channel]);

    /*
     * get pointers to the start of the pak and descriptor rings.
     */
    rx_descriptor = m32_ds->ctrl->ctrl_channels[channel].first_recv_desc;
    pak_ring = channel_params->rx_pak_first;

    do {
        /*
         * if a buffer is allocated, free it.
         */
        if (pak_ring->pak != NULL) {
            datagram_done(pak_ring->pak);
            pak_ring->pak = NULL;
            rx_descriptor->recv_data_ptr = NULL;
        }
        /*
         * alloc a new buffer, check for failure
         */
        pak_ring->pak = m32_getbuf(idb, channel_params->data_offset);
        if (pak_ring->pak == NULL) {
            m32_free_buffers(m32_ds, channel);
            status = M32_ERR_MEM_ALLOC;
            break;
        } else {
            /*
             * point the descriptor to the data portion of the packet
             * write the buffer size then go on to the next ring entry.
             */
            rx_descriptor->recv_data_ptr = 
              pak_ring->pak->datagramstart;
            rx_descriptor->recv_params &= ~(M32_RECV_DESC_NO);
            rx_descriptor->recv_params |= 
                (idb->max_buffer_size << M32_RECV_DESC_NO_SHIFT);
            pak_ring = pak_ring->next_pak;
            rx_descriptor = rx_descriptor->next_recv_desc;
        }
    } while (rx_descriptor != m32_ds->ctrl->ctrl_channels[channel].first_recv_desc);
   
    return(status);
}


/*
 * =====================================================================
 * m32_initialize_descriptors
 *
 * Description:
 * This function will reinitialize the m32 channel descriptors
 * based on the first tx/rx descriptor/packet pointers inside the m32_ds.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel to be aborted
 *
 * Returns:
 * Notes:
 * This function can be used to reinitialize channel descriptors when
 * the channel is aborted or turned off before it's turned on again.
 * =====================================================================
 */
static void m32_initialize_descriptors (m32_instance_t *m32_ds,
					ulong channel)
{
    m32_channel_params_t *channel_params;
    volatile m32_channel_ctrl_t *channel_control;
    ulong           i, num_descriptors, xmit_delay;
    m32_recv_desc_t *rx_desc_array,
                    *rx_descriptor;
    m32_pak_ring_t  *pak_array,
                    *pak_ring;
    m32_xmit_desc_t *tx_desc_array,
                    *tx_descriptor;

    channel_params = &(m32_ds->channel_params[channel]);
    channel_control = &(m32_ds->ctrl->ctrl_channels[channel]);

    rx_desc_array = channel_control->first_recv_desc;
    pak_array  = channel_params->rx_pak_first;
    channel_params->rx_ring_head = rx_desc_array;
    channel_params->rx_pak_head = pak_array;
    xmit_delay = channel_params->channel_idb->xmitdelay;

    /*
     * initialize all receive descriptors.
     * however, the last second descriptor needs special handling.
     * after this for loop initialization.
     */
    num_descriptors = channel_params->rx_ring_size;
    for (i = 0; i < num_descriptors; i++) {
        rx_descriptor = &(rx_desc_array[i]);
        rx_descriptor->recv_params = M32_RECV_PARAMS_INITIAL;
        rx_descriptor->recv_status = M32_RECV_STATUS_INITIAL;
        rx_descriptor->recv_data_ptr = NULL;
        rx_descriptor->next_recv_desc =
	    &(rx_desc_array[(i+1) % num_descriptors]);

        pak_ring = &(pak_array[i]);
        pak_ring->pak = NULL;
        pak_ring->next_pak = &(pak_array[(i+1) % num_descriptors]);

    }
    /*
     * this is the last second descriptor in the rx ring.
     * set the hold bit in the last second descriptor.
     */
    rx_descriptor = &(rx_desc_array[num_descriptors - 2]);
    rx_descriptor->recv_params = M32_RECV_PARAMS_INITIAL | M32_RECV_DESC_HOLD;
    pak_ring = &(pak_array[num_descriptors - 2]);

    channel_params->rx_ring_tail = rx_descriptor;
    channel_params->rx_pak_tail = pak_ring;

    /*
     * ***** Initialize Transmit Descriptors *****
     * save a pointer to the first descriptor in the 
     * control and the channel params structs for
     * both the descriptor and pak rings.
     */
    tx_desc_array = channel_control->first_xmit_desc;
    pak_array  = channel_params->tx_pak_first;
    channel_params->tx_ring_head = tx_desc_array;
    channel_params->tx_ring_tail = tx_desc_array;
    channel_params->tx_pak_head = pak_array;
    channel_params->tx_pak_tail = pak_array;

    /*
     * initialize all but the last receive descriptor.
     * the last descriptor needs special handling.
     */
    num_descriptors = channel_params->tx_ring_size;
    for (i = 0; i < num_descriptors - 1; i++) {
        tx_descriptor = &(tx_desc_array[i]);
        tx_descriptor->xmit_params = M32_XMIT_DESC_INITIAL | 
                                    (xmit_delay & M32_XMIT_DESC_FNUM_MAX);
        tx_descriptor->xmit_data_ptr = NULL;
        tx_descriptor->next_xmit_desc = &(tx_desc_array[i+1]);

        pak_ring = &(pak_array[i]);
        pak_ring->pak = NULL;
        pak_ring->next_pak = &(pak_array[i+1]);
    }
    /*
     * the hold bit needs to be set in the first descriptor.
     */
    tx_desc_array[0].xmit_params |= 
                          (M32_XMIT_DESC_HOLD | M32_XMIT_DESC_FE);
    /*
     * this is the last descriptor in the tx ring.
     * set its next pointer to the first element.
     */
    tx_descriptor = &(tx_desc_array[num_descriptors - 1]);
    tx_descriptor->xmit_params = M32_XMIT_DESC_INITIAL 
                              | (xmit_delay & M32_XMIT_DESC_FNUM_MAX);
    tx_descriptor->xmit_data_ptr = NULL;
    tx_descriptor->next_xmit_desc = tx_desc_array;

    pak_ring = &(pak_array[num_descriptors - 1]);
    pak_ring->pak = NULL;
    pak_ring->next_pak = pak_array;
}


/*
 * =====================================================================
 * m32_free_descriptors()
 *
 * Description:
 * Free descriptors in the receive and transmit rings
 * for a channel. Free any buffers first.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel that needs its descriptors freed 
 *
 * Returns:
 * m32 error code
 *
 * Notes:
 * This function expects that the Munich32 is not polling
 * this channel. A function such as m32_down_channel() should
 * be called before this function to insure polling is disabled.
 * =====================================================================
 */
static void m32_free_descriptors (m32_instance_t *m32_ds,
				  ulong channel)
{
    volatile m32_channel_ctrl_t *channel_control;
    m32_channel_params_t *channel_params;
 
    channel_control = &(m32_ds->ctrl->ctrl_channels[channel]);
    channel_params = &(m32_ds->channel_params[channel]);

    /*
     * only allow the descriptors to be freed if the Munich32 is
     * not polling the descriptors. this is true when the channel
     * is in the DOWN and UNUSED state.
     */
    if ((channel_params->current_state == M32_CHANNEL_DOWN) ||
        (channel_params->current_state == M32_CHANNEL_UNUSED)) {
        /*
         * free all rx and tx buffers in the current rings. 
         */
        m32_free_buffers(m32_ds, channel);
        /*
         * free the rx descriptors and pak rings
         */
        if (channel_control->first_recv_desc) {
            free(channel_control->first_recv_desc);
            channel_control->first_recv_desc = NULL;
            m32_ds->ctrl->ctrl_curr_recv_desc[channel] = NULL;
            channel_params->rx_ring_head = NULL;
            channel_params->rx_ring_tail = NULL;
            channel_params->rx_ring_size = 0;
        
        }
        if (channel_params->rx_pak_first) {
            free(channel_params->rx_pak_first);
            channel_params->rx_pak_first = NULL;
            channel_params->rx_pak_head = NULL;
            channel_params->rx_pak_tail = NULL;
        }
        /*
         * free the tx descriptors and pak rings 
         */
        if (channel_control->first_xmit_desc) {
            free(channel_control->first_xmit_desc);
            channel_control->first_xmit_desc = NULL;
            m32_ds->ctrl->ctrl_curr_xmit_desc[channel] = NULL;
            channel_params->tx_ring_head = NULL;
            channel_params->tx_ring_tail = NULL;
            channel_params->tx_ring_size = 0;
            channel_params->tx_ring_use_count = 0;
        }
        if (channel_params->tx_pak_first) {
            free(channel_params->tx_pak_first);
            channel_params->tx_pak_first = NULL;
            channel_params->tx_pak_head = NULL;
            channel_params->tx_pak_tail = NULL;
        }
    }
}


/*
 * =====================================================================
 * m32_alloc_descriptors()
 *
 * Description:
 * Alloc descriptors for the receive and transmit rings
 * for a channel. If descriptors already exist free them
 * and alloc others.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel that needs descriptors 
 *
 * Returns:
 * M32_NOERR
 * M32_ERR_MEM_ALLOC
 * =====================================================================
 */
static enum m32_error_t m32_alloc_descriptors (m32_instance_t *m32_ds,
					       ulong channel)
{
    m32_channel_params_t *channel_params;
    volatile m32_channel_ctrl_t *channel_control;
    m32_recv_desc_t *rx_desc_array;
    m32_pak_ring_t  *rx_pak_array,
                    *tx_pak_array;
    m32_xmit_desc_t *tx_desc_array;
    ulong           num_rx_descriptors, num_tx_descriptors;

    channel_params = &(m32_ds->channel_params[channel]);
    channel_control = &(m32_ds->ctrl->ctrl_channels[channel]);

    /*
     * get a count of the number of rx and tx descriptors needed.
     * free all rx and tx buffers and descriptors in  this ring.
     * this function also zero's out the counts so put them back.
     */
    num_rx_descriptors = channel_params->rx_ring_size;
    num_tx_descriptors = channel_params->tx_ring_size;
    m32_free_descriptors(m32_ds, channel);
    channel_params->rx_ring_size = num_rx_descriptors;
    channel_params->tx_ring_size = num_tx_descriptors;

    /*
     * alloc the receive and transmit descriptors for this channel
     * the descriptors must be long word aligned.
     */
    rx_desc_array =
        malloc_iomem_aligned(sizeof(m32_recv_desc_t)*num_rx_descriptors,
			     M32_LONG_WORD_ALIGN);
 
    if (rx_desc_array == NULL) {
        return(M32_ERR_MEM_ALLOC);
    }
    tx_desc_array =
        malloc_iomem_aligned(sizeof(m32_xmit_desc_t)*num_tx_descriptors,
			     M32_LONG_WORD_ALIGN);
 
    if (tx_desc_array == NULL) {
        /*
         * problem allocating io memory
         * free any previously allocated memory
         */
        free(rx_desc_array);
        return(M32_ERR_MEM_ALLOC);
    }
    rx_pak_array = malloc(sizeof(m32_pak_ring_t) * num_rx_descriptors);
    if (rx_pak_array == NULL) {
        /*
         * problem allocating receive pak ring
         * free any previously allocated memory
         */
        free(rx_desc_array);
        free(tx_desc_array);
        return(M32_ERR_MEM_ALLOC);
    }
    tx_pak_array = malloc(sizeof(m32_pak_ring_t) * num_tx_descriptors);
    if (tx_pak_array == NULL) {
        /*
         * problem allocating transmit pak ring
         * free any previously allocated memory
         */
        free(rx_desc_array);
        free(tx_desc_array);
        free(rx_pak_array);
        return(M32_ERR_MEM_ALLOC);
    }
 
    /*
     * ***** Initialize Receive Descriptors *****
     * save a pointer to the first descriptor in the 
     * control and the channel params structs for
     * both the descriptor and pak rings.
     */
    channel_control->first_recv_desc = rx_desc_array;
    channel_params->rx_ring_head = rx_desc_array;
    channel_params->rx_pak_first = rx_pak_array;
    channel_params->rx_pak_head = rx_pak_array;

    channel_control->first_xmit_desc = tx_desc_array;
    channel_params->tx_pak_first = tx_pak_array;
    m32_initialize_descriptors(m32_ds, channel);

    return(M32_NOERR);
}


/*
 * =====================================================================
 * m32_reset_hdlc_mode
 *
 * Description:
 * This function will restart the particular channel without
 * freeing the channel's descriptors and also set the channel
 * in the hdlc tx/rx mode.
 *
 * Parameters:
 * m32_ds         - pointer to the m32 instance structure
 * channel_params - m32 channel parameter configuration
 *                  structure pointer
 * channel        - the channel number to be reset
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void m32_reset_hdlc_mode (m32_instance_t *m32_ds,
				 m32_channel_params_t *channel_params,
				 ulong channel)
{
    ushort ix;
    ulong bit_mask, timeslot;
    volatile m32_channel_ctrl_t *channel_control;
    volatile m32_timeslot_t *channel_timeslot;

    m32_ds->ctrl->ctrl_action = m32_ds->pcm_format | M32_AR_MFL |
      (channel << M32_AR_CHANNEL_SHIFT) | M32_AR_IN;
    
    channel_control = &(m32_ds->ctrl->ctrl_channels[channel]);
    
    channel_control->channel_info = M32_CHANNEL_INT_MASK |
	M32_CHANNEL_ABORT_CMD | M32_CHANNEL_NITBS;

    ix = m32_ds->regs->action_request_reg;
    
    /*
     * poll the interrupt bit to see when the operation is done.
     */
    m32_ar_poll(m32_ds);
    m32_delay_msec(1);

    channel_control->channel_info = M32_CHANNEL_INT_MASK |
	M32_CHANNEL_OFF_CMD | M32_CHANNEL_NITBS;

    ix = m32_ds->regs->action_request_reg;
    
    /*
     * poll the interrupt bit to see when the operation is done.
     */
    m32_ar_poll(m32_ds);
    m32_delay_msec(1);

    m32_free_buffers(m32_ds, channel);
    m32_initialize_descriptors(m32_ds, channel);
    m32_alloc_rx_buffers(m32_ds, channel);

   /*
    * We have to set the bitrate in the timeslots for this channel.
    * If a remote loopback was initiated the bitrate for the 
    * timeslot would have been set to 64 Kbps.
    * This has to be reset to the configured bitrate.
    */

    bit_mask = channel_params->timeslot_bitfield;

   /*
     * unbundle the timeslot bits. a channel can have multiple
     * timeslots associated with it.
     * assuming the xmit and recv timeslots are always the same
     * also assuming that if a channel has multiple timeslots
     * all timeslots will have the same bitrate.
     */
    timeslot = 0;
    while (bit_mask) {  /* loop while there are still timeslot bits set */
        /*
         * check if the bit for this timeslot is set.
         */
        if (bit_mask & 1) {
            channel_timeslot = &(m32_ds->ctrl->ctrl_timeslots[timeslot]);

            /*
             * put the bit rate into the timeslot field.
             * every bit set in the field represents a bit that
             * the m32 will pass in a 8 bit frame. all 8 bits set
             * is 64Kb, 7 bits is 56Kb etc.
             * The bitrate is being restored from the value saved in 
             * channel_params.
             */
            channel_timeslot->recv_timeslot &=
                          ~M32_TIMESLOT_BITRATE;
            channel_timeslot->recv_timeslot |=
              (channel_params->fillmask & M32_TIMESLOT_BITRATE);
            channel_timeslot->xmit_timeslot &=
                          ~M32_TIMESLOT_BITRATE;
            channel_timeslot->xmit_timeslot |=
              (channel_params->fillmask & M32_TIMESLOT_BITRATE);
            if (channel == m32_ds->idle_channel) {
                channel_timeslot->recv_timeslot |= M32_TIMESLOT_INHIBIT;
            }
 
        }
        /*
         * shift the just checked timeslot off, the next timeslot
         * will now be the right most bit.
         */
        bit_mask >>= 1;
        timeslot++;
    }




    channel_control->channel_info = (M32_CHANNEL_INIT_MASK | M32_CHANNEL_FA);

    ix = m32_ds->regs->action_request_reg;
    
    /*
     * poll the interrupt bit to see when the operation is done.
     */
    m32_ar_poll(m32_ds);
    m32_delay_msec(1);
}

/*
 * =====================================================================
 * m32_send_rloopback_packet
 *
 * Description:
 * This routine will actually put a packet in the m32 channel
 * descriptor for transmitting. The purpose of this function
 * is to send out the packet transparently from the application
 * program point of view. No higher level packet counter will 
 * be increased.
 *
 * Parameters:
 * hwidb - hardware idb pointer
 * pak   - the packet to be transmitted
 * channel_params - m32 channel parameter configuration
 *                  structure pointer
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void m32_send_rloopback_packet (hwidbtype *hwidb, 
				       paktype *pak, 
				       m32_channel_params_t *channel_params)
{
    m32_xmit_desc_t *prev_tx_ring_head,
                    *tx_ring_head;
    /*
     * move the head pointer to the next descriptor in the ring.
     * record that another descriptor is in use.
     */
     prev_tx_ring_head = channel_params->tx_ring_head;
     channel_params->tx_ring_head =
       channel_params->tx_ring_head->next_xmit_desc;
     tx_ring_head = channel_params->tx_ring_head;
     channel_params->tx_pak_head =
       channel_params->tx_pak_head->next_pak;
     /*
      * Set up this TX ring entry for this packet. assign
      * the packet to the descriptor and set the status bits.
      */
     channel_params->tx_pak_head->pak = pak;
     tx_ring_head->xmit_data_ptr = pak->datagramstart;
     channel_params->tx_ring_use_count++;
     
     /*
      * Set transmit timeout timer.
      */
     TIMER_START(hwidb->xmittimer, TIMEOUT);

     /*
      * set the hold bit and the number of bytes to transfer
      * in the new head descriptor. clear the hold bit on the
      * previous head descriptor. this causes the data in the
      * new head descriptor to be sent.
      */
      tx_ring_head->xmit_params |=
        ((pak->datagramsize << M32_XMIT_DESC_NO_SHIFT) | M32_XMIT_DESC_HOLD);
      prev_tx_ring_head->xmit_params &= ~(M32_XMIT_DESC_HOLD);
}

/*
 * =====================================================================
 * m32_send_pattern_packet
 *
 * Description:
 * This routine will send out a packet filled out with 
 * the pattern data passed in as a parameter. The size 
 * of pattern filled is passed as well.
 *
 * Parameters:
 * hwidb - hardware idb pointer
 * channel_params - m32 channel parameter configuration 
 *                  structure pointer
 * pattern - the pattern to fill in the packet
 * size    - the size of the pattern to fill in the packet
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void  m32_send_pattern_packet (hwidbtype *hwidb, 
				      m32_channel_params_t *channel_params,
				      unsigned char pattern,
				      int size)
{
   paktype *pak;
   char *buf;

   if (hwidb->max_pak_size < size) {
        return;
   }

   pak = m32_getbuf(hwidb, 0);   /* data offset = 0 */

   if (pak == NULL)
      return;
 
   buf = pak->datagramstart;
   memset(buf, pattern, size);    /* fill the data pattern in the buffer */
   pak->datagramsize = size;
   m32_send_rloopback_packet(hwidb, pak, channel_params);
}

/*
 * =====================================================================
 * m32_continue_send_rmt_loopback_up
 *
 * Description:
 * Continue to send remote loopup code till max number of loopup packets
 * have been sent.
 *
 * Parameters:
 * hwidb - hardware idb pointer
 * pak   - the packet tarnsmitted by m32
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void m32_continue_send_rmtloopback_up(hwidbtype *hwidb, paktype *pak)
{
    m32_instance_t *m32_ds;
    m32_channel_params_t *channel_params;
    ulong channel;
 
    channel = hwidb->vc;
    m32_ds = M32_INSTANCE(hwidb->slot, hwidb->subcont);
    channel_params = &(m32_ds->channel_params[channel]);

    if (--channel_params->rloopback_frame_sent) {
    m32_send_pattern_packet(hwidb, channel_params,
                            REM_LOOP_FEV_BYTES, REM_LOOPUP_FEV_COUNT);
    }
}



/*
 * =====================================================================
 * m32_check_rmtloopback_up
 *
 * Description:
 * Parse the receive frame when the m32 is turn on for remote
 * loopback mode in the transparent mode. If the frame contents
 * match with the patterns m32 transmitted, the turning on remote
 * loopback is completed successfully. Otherwise, after rxing
 * certain number of frames, the remote loopback mode will be 
 * then turned off.
 *
 * Parameters:
 * hwidb - hardware idb pointer
 * pak   - the packet received by m32
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void m32_check_rmtloopback_up (hwidbtype *hwidb, paktype *pak)
{
    unsigned char *buf;
    int  size, ix;
    m32_instance_t *m32_ds;
    m32_channel_params_t *channel_params;
    ulong channel, count;

    ix = REM_LOOPUP_TIP_CNT-1;
    count = 0;
    buf = pak->datagramstart;
    size = pak->datagramsize;

    channel = hwidb->vc;
    m32_ds = M32_INSTANCE(hwidb->slot, hwidb->subcont);
    channel_params = &(m32_ds->channel_params[channel]);
    if (serial_packet_debug) 
        buginf("\nSlot %d: RemoteLoopbackUp receives frame number %d :\n",
               hwidb->slot,
               REM_LOOP_FRAME_CNT-channel_params->rloopback_frame_cnt+1);

    /*
     * look for the first data pattern loopbacked from DDS CSU
     */
    for (; !REM_LOOP_PATTERN_MATCHED(*(buf++), REM_LOOP_FEV_BYTES) && size;
         size--) {
        if(serial_packet_debug) {
            buginf("%02x ", *(buf-1));
            if (!(++count % 20))
               buginf("\n");
        }
    }

    /*
     * if the first loopbacked pattern is found, check if
     * the next ix consecutive data received also matched 
     * with the loopback pattern.
     */
    if (size != 0) {
        for (; ix != 0; ix--) {
            if(serial_packet_debug) {
                buginf("%02x ", *buf);
                if (!(++count % 20))
                    buginf("\n");
            }

            if (!REM_LOOP_PATTERN_MATCHED(*(buf++), REM_LOOP_FEV_BYTES)) {
                break;
            }
        }
        
        /*
         * if the loopback pattern is checked out fine, it means
         * the remote loopback is turned on successfully and
         * m32 is reset to hdlc mode.
         */
        if (ix == 0) {
            channel_params->xmit_handler = channel_params->xmit_handler_saved;
            channel_params->recv_handler = channel_params->recv_handler_saved;
            channel_params->loopback = M32_REMOTE_LOOPBACK_ACTIVE;
	    /*
	     * Indicate that we're done with this loopback
	     */
	    hwidb->far_end_looped = TRUE;

            m32_reset_hdlc_mode(m32_ds, channel_params, channel);

            errmsg(&msgsym(REMLOOP, LINK), hwidb->hw_namestring,
                   "up", "passed");
        }
    }

    if(serial_packet_debug) {
        buginf("\n");
    }

    datagram_done(pak);

    /* 
     * if the loopback pattern is not in the current frame, the next loopback
     * pattern needs to be sent out. if more than rloopback_frame_cnt
     * number of frames are received, then remote loopback fails and
     * the m32 is reset to hdlc mode.
     */
    if ((size == 0) || (ix != 0)) {
        if (!(--channel_params->rloopback_frame_cnt)) {
            hwidb->ext_loop_type = DSX1_LOOP_NONE;

            /* Send Remote loop down as a backup. Just incase the remote 
             * had looped up but had given the wrong code.
             * We have to loop it down.
             */
           channel_params->xmit_handler = channel_params->xmit_handler_saved;
           channel_params->recv_handler = channel_params->recv_handler_saved;
	   channel_params->loopback = M32_REMOTE_LOOPBACK_WAIT_DOWN;
	   m32_recv_remote_loopback(m32_ds, channel);
        }
    }
}



/*
 * =====================================================================
 * m32_continue_send_rmt_loopback_down
 *
 * Description:
 * Continue to send remote loopdown code till max number of loopdown packets
 * have been sent.
 *
 * Parameters:
 * hwidb - hardware idb pointer
 * pak   - the packet tarnsmitted by m32
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void m32_continue_send_rmtloopback_down(hwidbtype *hwidb, paktype *pak)
{
    m32_instance_t *m32_ds;
    m32_channel_params_t *channel_params;
    ulong channel;
 
    channel = hwidb->vc;
    m32_ds = M32_INSTANCE(hwidb->slot, hwidb->subcont);
    channel_params = &(m32_ds->channel_params[channel]);
 
    if (--channel_params->rloopback_frame_sent) {
       m32_send_pattern_packet(hwidb, channel_params,
                               REM_LOOP_TIP_BYTES, REM_LOOPDOWN_TIP_CNT);
    }
}




/*
 * =====================================================================
 * m32_check_rmtloopback_down
 *
 * Description:
 * Parse the receive frame when the m32 is turn off for remote
 * loopback mode in the transparent mode. If the frame contents
 * match with the patterns m32 transmitted, the turning off remote
 * loopback is completed successfully. Otherwise, after rxing
 * certain number of frames, the remote loopback mode will be 
 * still turned off.
 *
 * Parameters:
 * hwidb - hardware idb pointer
 * pak   - the packet received by m32
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void m32_check_rmtloopback_down (hwidbtype *hwidb, paktype *pak)
{
    unsigned char *buf;
    int  size, ix;
    m32_instance_t *m32_ds;
    m32_channel_params_t *channel_params;
    ulong channel, count;

    ix = REM_LOOPUP_TIP_CNT-1;
    count = 0;
    buf = pak->datagramstart;
    size = pak->datagramsize;

    channel = hwidb->vc;
    m32_ds = M32_INSTANCE(hwidb->slot, hwidb->subcont);
    channel_params = &(m32_ds->channel_params[channel]);
 
    if (serial_packet_debug)
        buginf("\nSlot %d: RemoteLoopbackDown receives frame number %d :\n",
               hwidb->slot, 
               REM_LOOP_FRAME_CNT-channel_params->rloopback_frame_cnt+1);

    /*
     * look for the first data pattern loopbacked from DDS CSU
     */
    for (; !REM_LOOP_PATTERN_MATCHED(*(buf++), REM_LOOP_TIP_BYTES) && size;
	 size--) {
        if(serial_packet_debug) {
            buginf("%02x ", *(buf-1));
            if (!(++count % 20))
               buginf("\n");
          }
      }

    /*
     * if the first loopbacked pattern is found, check if
     * the next ix consecutive data received also matched
     * with the loopback pattern.
     */
    if (size != 0) {
        for (; ix != 0 ; ix--) {
            if(serial_packet_debug) {
                buginf("%02x ", *buf);
                if (!(++count % 20))
                    buginf("\n");
            }

            if (!REM_LOOP_PATTERN_MATCHED(*(buf++), REM_LOOP_TIP_BYTES)) {
                break;
            }
        }

        /*
         * if the loopback pattern is checked out fine, it means
         * the remote loopback is turned off successfully and
         * m32 is reset to hdlc mode.
         */
        if (ix == 0) {
            channel_params->xmit_handler = channel_params->xmit_handler_saved;
            channel_params->recv_handler = channel_params->recv_handler_saved;
            channel_params->loopback = M32_NO_LOOPBACK;

	    /*
	     * remote loop down - ext_loop_type is already DSX1_LOOP_NONE
	     */
            hwidb->far_end_looped = FALSE;

            m32_reset_hdlc_mode(m32_ds, channel_params, channel);

            errmsg(&msgsym(REMLOOP, LINK), hwidb->hw_namestring,
                   "down", "passed");
        }
    }

    if(serial_packet_debug) {
        buginf("\n");
    }

    datagram_done(pak);
 
    /*
     * if the loopback pattern is not in the current frame, the next loopback
     * pattern needs to be sent out. if more than rloopback_frame_cnt
     * number of frames are received, then remote loopback fails and
     * the m32 is reset to hdlc mode.
     */
    if ((size == 0) || (ix != 0)) {
        if (!(--channel_params->rloopback_frame_cnt)) {
            channel_params->xmit_handler = channel_params->xmit_handler_saved;
            channel_params->recv_handler = channel_params->recv_handler_saved;
            channel_params->loopback = M32_REMOTE_LOOPBACK_ACTIVE;
            
            m32_reset_hdlc_mode(m32_ds, channel_params, channel);

	    /*
	     * We failed remote loop down.  Reset the ext_loop_type
	     * field.  far_end_looped is already TRUE
	     */
	    if(hwidb->far_end_looped)  { 
	       /*
		* We failed loop down but the far end is already looped up.
		*/
	 	hwidb->ext_loop_type = DSX1_LOOP_REMOTE;
            	errmsg(&msgsym(REMLOOP, LINK), hwidb->hw_namestring,
                   "down", "failed");
	    }	    
	    else {
               /* 
                * We failed loop down and the far end is not looped. This 
                * situation is when we sent a loopup which failed. In 
                * which case we sent a backup loopdown to make sure 
                * the remote end is not looped.
                */
	     	  errmsg(&msgsym(REMLOOP, LINK), hwidb->hw_namestring,
                   "up", "failed");
	    }

        }
    }
}

/*
 * =====================================================================
 * m32_send_remote_loopback()
 *
 * Description:
 * Put a channel in remote loopback mode and send a specific
 * pattern to the far end.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel to put in remote loopback
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void m32_send_remote_loopback (m32_instance_t *m32_ds,
				      ulong channel)
{
    m32_channel_params_t *channel_params;
    paktype *pak;
    hwidbtype *hwidb;
    char *buf;

    channel_params = &(m32_ds->channel_params[channel]);
    hwidb = channel_params->channel_idb;

    if (hwidb->max_pak_size < (REM_LOOPUP_PREAMBLE_CNT+REM_LOOPUP_FEV_COUNT)) {
        channel_params->loopback = M32_NO_LOOPBACK;
        m32_reset_hdlc_mode(m32_ds, channel_params, channel);
	hwidb->ext_loop_type = DSX1_LOOP_NONE;
        hwidb->far_end_looped = FALSE;
        errmsg(&msgsym(REMLOOP, LINK), hwidb->hw_namestring,
               "up", "failed - datagram size is too small.");
        return;
    }

    pak = m32_getbuf(hwidb, 0);   /* data offset = 0 */

    if (pak == NULL) {
        channel_params->loopback = M32_NO_LOOPBACK;
        m32_reset_hdlc_mode(m32_ds, channel_params, channel);
	hwidb->ext_loop_type = DSX1_LOOP_NONE;
        hwidb->far_end_looped = FALSE;
        errmsg(&msgsym(REMLOOP, LINK), hwidb->hw_namestring,
               "up", "failed - no buffer is available.");
        return;
    }

    buf = pak->datagramstart;

    memset(buf, REM_LOOP_TIP_BYTES, REM_LOOPUP_TIP_CNT);
    buf  += REM_LOOPUP_TIP_CNT;

    memset(buf, REM_LOOP_SELECT, REM_LOOPUP_SELECT_CNT);
    buf += REM_LOOPUP_SELECT_CNT;

    memset(buf, REM_LOOP_LBE_BYTES, REM_LOOPUP_LBE_CNT);
    buf += REM_LOOPUP_LBE_CNT;

    memset(buf, REM_LOOP_FEV_BYTES, REM_LOOPUP_FEV_COUNT);

    pak->datagramsize = REM_LOOPUP_PREAMBLE_CNT + REM_LOOPUP_FEV_COUNT;
    m32_send_rloopback_packet(hwidb, pak, channel_params);
    m32_send_pattern_packet(hwidb, channel_params,
                            REM_LOOP_FEV_BYTES, REM_LOOPUP_FEV_COUNT);

    channel_params->xmit_handler_saved = channel_params->xmit_handler;
    channel_params->recv_handler_saved = channel_params->recv_handler;
    channel_params->xmit_handler = m32_continue_send_rmtloopback_up;
    channel_params->recv_handler = m32_check_rmtloopback_up;
    channel_params->rloopback_frame_cnt = REM_LOOP_FRAME_CNT;
    channel_params->rloopback_frame_sent = REM_LOOP_FRAME_CNT;

    if (serial_debug) {
         buginf("\nSlot %d enables remote loopback pattern\n", hwidb->slot);
    }
}


/*
 * =====================================================================
 * m32_recv_remote_loopback()
 *
 * Description:
 * Analyze received data on a remotely looped channel. Look for
 * a specific pattern. If the pattern is found declare the 
 * channel remotely looped. If not do nothing. 
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel to put in remote loopback
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void m32_recv_remote_loopback (m32_instance_t *m32_ds,
				      ulong channel)
{
    m32_channel_params_t *channel_params;
    paktype *pak;
    hwidbtype *hwidb;
    char *buf;
    
    channel_params = &(m32_ds->channel_params[channel]);
    hwidb = channel_params->channel_idb;

    if (((int) hwidb->max_pak_size) < REM_LOOPDOWN_TIP_CNT) {
        channel_params->loopback = M32_NO_LOOPBACK;
        m32_reset_hdlc_mode(m32_ds, channel_params, channel);
	hwidb->ext_loop_type = DSX1_LOOP_NONE;
        hwidb->far_end_looped = FALSE;
        errmsg(&msgsym(REMLOOP, LINK), hwidb->hw_namestring,
               "down", "failed - datagram size is too small.");
        return;
    }

    pak = m32_getbuf(hwidb, 0);   /* data offset = 0 */

    if (pak == NULL) {
        channel_params->loopback = M32_NO_LOOPBACK;
        m32_reset_hdlc_mode(m32_ds, channel_params, channel);
	hwidb->ext_loop_type = DSX1_LOOP_NONE;
        hwidb->far_end_looped = FALSE;
        errmsg(&msgsym(REMLOOP, LINK), hwidb->hw_namestring,
               "down", "failed - no buffer is available.");
        return;
    }

    buf = pak->datagramstart;
    memset(buf, REM_LOOP_TIP_BYTES, REM_LOOPDOWN_TIP_CNT);
    pak->datagramsize = REM_LOOPDOWN_TIP_CNT;
    m32_send_rloopback_packet(hwidb, pak, channel_params);
    m32_send_pattern_packet(hwidb, channel_params,
                            REM_LOOP_TIP_BYTES, REM_LOOPDOWN_TIP_CNT);

    channel_params->xmit_handler_saved = channel_params->xmit_handler;
    channel_params->recv_handler_saved = channel_params->recv_handler;
    channel_params->xmit_handler = m32_continue_send_rmtloopback_down;
    channel_params->recv_handler = m32_check_rmtloopback_down;
    channel_params->rloopback_frame_cnt = REM_LOOP_FRAME_CNT;
    channel_params->rloopback_frame_sent = REM_LOOP_FRAME_CNT;

    if (serial_debug) {
         buginf("\nSlot %d disables remote loopback pattern\n", hwidb->slot);
    }
}


/*
 * =====================================================================
 * m32_release_channel()
 *
 * Description:
 * Free a channel from use. Free descriptors, buffers and timeslots
 * assigned to the channel. Put it in the UNUSED state.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel to be released 
 *
 * Returns:
 * m32 error code
 *
 * Notes:
 * This function increments the ar_in_progress counter. make sure the 
 * function decrements this variable at all return points.
 * =====================================================================
 */
enum m32_error_t m32_release_channel (m32_instance_t *m32_ds,
                                      ulong channel)
{
    boolean config_changed = FALSE;
    enum m32_error_t status = M32_NOERR;
    volatile m32_timeslot_t *timeslot_array;
    ulong bit_mask;
    ushort timeslot;

    status = m32_mark_ar_in_progress(m32_ds);
    if (status != M32_NOERR)
        return (status);

    /*
     * down the channel. this frees all buffers assigned
     * to the channel. then free the descriptors.
     */
    if (m32_down_channel(m32_ds, channel) == M32_NOERR) {
        m32_free_descriptors(m32_ds, channel);   
    }

    /* 
     * don't need to mess with timeslots at all if we're
     * releasing an idle channel. the init/release of 
     * non-idle channels updates the timeslots.
     */
    if (channel != m32_ds->idle_channel) {
        /*
         * loop thru all the timeslots looking for those that are
         * assigned to the channel that is being released.
         */
        timeslot_array = m32_ds->ctrl->ctrl_timeslots;
        bit_mask = m32_ds->channel_params[channel].timeslot_bitfield;
        timeslot = 0;
        while (bit_mask) {  /* while there are still timeslot bits set */
            if (bit_mask & 1) {
                /*
                 * inhibit recv'ing and xmit'ing for this timeslot
                 * and set the bitrate to zero.
                 */
                timeslot_array[timeslot].recv_timeslot = M32_TIMESLOT_INHIBIT;
                timeslot_array[timeslot].xmit_timeslot = M32_TIMESLOT_INHIBIT;
                /*
                 * mark the timeslot as idle in the instance.
                 */
                m32_ds->idle_timeslots |= (1 << timeslot);
                config_changed = TRUE;
            }
            bit_mask >>= 1;
            timeslot++;
        }
    }

    /*
     * re-initialize the channel params struct
     */
    m32_init_channel_params(m32_ds, channel);

    /*
     * if we're freeing the idle channel then note that there
     * is no idle channel assinged. 
     */
    if (channel == m32_ds->idle_channel) {
        m32_ds->idle_channel = M32_IDLE_CHNL_UNASSIGNED;
    }
    /*
     * tell the chip to disable the timeslots if any were found.
     * if we're freeing any channel besides the idle channel
     * then the idle timeslots have changed and we need to
     * adjust the idle channel.
     */
    if (config_changed) {
        status = m32_execute_ar(m32_ds, channel);
        if (status == M32_NOERR && channel != m32_ds->idle_channel) {
            status = m32_adjust_idle_channel(m32_ds);
        }
    }

    m32_unmark_ar_in_progress_inline(m32_ds);

    return(status);
}


/*
 * =====================================================================
 * m32_abort_channel()
 *
 * Description:
 * Put a channel into the STANDBY state.
 * Should only be called for a channel in the UP or STANDBY state.
 * Should be used to put the channel in a down but ready state.
 * Also used to abort any ongoing receive or transmit operation.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel to be aborted 
 *
 * Returns:
 * m32 error code
 *
 * Notes:
 * This function increments the ar_in_progress counter. make sure the 
 * function decrements this variable at all return points.
 * =====================================================================
 */
static enum m32_error_t m32_abort_channel (m32_instance_t *m32_ds,
					   ulong channel)
{ 
    enum m32_channel_state_t state; 
    enum m32_error_t status = M32_NOERR;

    status = m32_mark_ar_in_progress(m32_ds);
    if (status != M32_NOERR)
        return (status);

    state = m32_ds->channel_params[channel].current_state;
    if ((state == M32_CHANNEL_UP) || 
        (state == M32_CHANNEL_IDLE) ||
        (state == M32_CHANNEL_THROTTLED) ||
        (state == M32_CHANNEL_STANDBY)) {
        /*
         * set the channel command in the control struct to
         * do an abort command.
         */
        m32_ds->ctrl->ctrl_channels[channel].channel_info = 
          M32_CHANNEL_ABORT_MASK;
        m32_ds->ctrl->ctrl_channels[channel].channel_itbs = 0;
        /*
         * execute an action request for this channel
         */
        status = m32_execute_ar(m32_ds, channel);
        if (status == M32_NOERR) {
            /*
             *  set the channels state to show it's in standby.
             */
            m32_ds->channel_params[channel].current_state = 
              M32_CHANNEL_STANDBY;
        }
    } else {
        status = M32_ERR_BAD_STATE;
    }

    m32_unmark_ar_in_progress_inline(m32_ds);

    return(status);
}


/*
 * =====================================================================
 * m32_reset_channel()
 *
 * Description:
 * clear a channel and then put it in the UP state.
 * should only be called for a channel in the UP or STANDBY state.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel to be reset 
 *
 * Returns:
 * m32 error code
 *
 * Notes:
 * This function increments the ar_in_progress counter. make sure the 
 * function decrements this variable at all return points.
 * =====================================================================
 */
enum m32_error_t m32_reset_channel (m32_instance_t *m32_ds,
				    ulong channel)
{
    enum m32_channel_state_t state;
    enum m32_error_t status = M32_NOERR;

    status = m32_mark_ar_in_progress(m32_ds);
    if (status != M32_NOERR)
        return (status);

    state = m32_ds->channel_params[channel].current_state;
    if ((state == M32_CHANNEL_UP) || 
        (state == M32_CHANNEL_STANDBY) ||
        (state == M32_CHANNEL_THROTTLED)) {
        status = m32_abort_channel(m32_ds, channel);
        if (status == M32_NOERR) {
            status = m32_up_channel(m32_ds, channel);
        }
    } else {
        status = M32_ERR_BAD_STATE;
    }

    m32_unmark_ar_in_progress_inline(m32_ds);

    return(status);
}


/*
 * =====================================================================
 * m32_tristate_isdn_b_channel()
 *
 * Description:
 * Tristate the isdn b channel's tx and rx timeslots in the interrupt.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - channel number.
 *
 * Returns:
 * M32_NOERR
 * M32_ERR_AR_FAILURE
 * M32_ERR_ILLEGAL_CHANNEL
 * =====================================================================
 */
enum m32_error_t m32_tristate_isdn_b_channel (m32_instance_t *m32_ds,
                                              ulong channel)
{
    ulong                    bit_mask;
    ushort                   timeslot, ix;
    hwidbtype               *hwidb;
    volatile m32_timeslot_t *channel_timeslot;

    if (m32_ds->ar_in_progress)
        return M32_ERR_AR_FAILURE;

    hwidb = m32_ds->channel_params[channel].channel_idb;
    if (!IS_ISDN(hwidb) || (hwidb->isdn_info->chantype == ISDN_Dchan))
        return M32_ERR_ILLEGAL_CHANNEL;

    /*
     * This is 4000 and access server specific to do action request in
     * interrupt level. The ar_in_progress will be decremented when an
     * ARACK or ARF is received in interrupt handler
     */
    m32_ds->waiting_arack_in_interrupt = TRUE;
    m32_ds->ar_in_progress++;

    channel_timeslot = m32_ds->ctrl->ctrl_timeslots;
    bit_mask = m32_ds->channel_params[channel].timeslot_bitfield;
    for (timeslot=0; bit_mask; bit_mask >>= 1, timeslot++)  {
        /* while there are still timeslot bits set */
        if (bit_mask & 1) {
            channel_timeslot[timeslot].xmit_timeslot |= M32_TIMESLOT_INHIBIT;
            channel_timeslot[timeslot].recv_timeslot |= M32_TIMESLOT_INHIBIT;
        }
    }

    m32_ds->ctrl->ctrl_channels[channel].channel_info   &=
        ~M32_CHANNEL_TX_RX_CMD_MASK;

    m32_ds->ctrl->ctrl_action =
        M32_AR_ONE_CHNL_MASK | (channel << M32_AR_CHANNEL_SHIFT) |
            m32_ds->pcm_format;
    m32_ds->regs->int_status_reg &= M32_CLEAR_AR_INT;
    ix = m32_ds->regs->action_request_reg;
    m32_ds->channel_params[channel].current_state = M32_CHANNEL_STANDBY;

    return M32_NOERR;
}

/*
 * =====================================================================
 * m32_down_channel()
 *
 * Description:
 * Put a channel into the DOWN state and free its buffers.
 * The channel will only be downed if the channel is in the 
 * UP or STANDBY state. If channel is in the UNUSED state or 
 * already in the DOWN state, do nothing.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel to be downed 
 *
 * Returns:
 * m32 error code
 *
 * Notes:
 * This function increments the ar_in_progress counter. make sure the 
 * function decrements this variable at all return points.
 * =====================================================================
 */
enum m32_error_t m32_down_channel (m32_instance_t *m32_ds,
				   ulong channel)
{
    enum m32_channel_state_t state;
    enum m32_error_t status = M32_NOERR;
    m32_channel_params_t *channel_params;

    status = m32_mark_ar_in_progress(m32_ds);
    if (status != M32_NOERR)
        return (status);

    channel_params = &(m32_ds->channel_params[channel]);
    state = channel_params->current_state;
    if ((state == M32_CHANNEL_UP) || 
        (state == M32_CHANNEL_STANDBY) ||
        (state == M32_CHANNEL_IDLE) ||
        (state == M32_CHANNEL_THROTTLED)) {
        /*
         * first abort the channel to stop any data transfer
         * that may be in progress. 
         */
        status = m32_abort_channel(m32_ds, channel);
        if (status == M32_NOERR) {
           /*
             *  Must delay between successive action requests to give the
             *  Munich 32 time to complete the operation.  This delay is
             *  suppose to done via the AR DONE INT but this does not
             *  really work as advertized.  AR DONE INT really means that
             *  the Munich 32 issued an interrupt after an action request
             *  was issued and after the Munich32 read its addr High/ low
             *  registers.  Thus the interrupt generated by the M32 may
             *  or may not be the AR done interrupt, it could be a TX, RX
             *  or error interrupt.  Therefore we must wait for the AR
             *  to be complete via this delay.
             */
	    DELAY_ACTION_REQUEST();

            /*
             * set the channel command in the control struct to
             * do an off command.
             */
            m32_ds->ctrl->ctrl_channels[channel].channel_info =
              M32_CHANNEL_DOWN_MASK;
            m32_ds->ctrl->ctrl_channels[channel].channel_itbs = 0;
            /*
             * execute an action request for this channel
             */
            status = m32_execute_ar(m32_ds, channel);
            if (status == M32_NOERR) {
                /*
                 * free the channels buffers and set the
                 * state to down in the channel info struct.
                 */
                m32_free_buffers(m32_ds, channel);
                channel_params->current_state = 
                  M32_CHANNEL_DOWN;
                if (channel_params->channel_idb && 
                    channel_params->rx_desc_throttle_hold) {
                    channel_params->rx_desc_throttle_hold = NULL;
                    idb_mark_unthrottled(channel_params->channel_idb);
                }
            }
        }
    } else if (state == M32_CHANNEL_ILLEGAL) {
        status = M32_ERR_BAD_STATE;
    }

    m32_unmark_ar_in_progress_inline(m32_ds);

    return(status);
}


/*
 * =====================================================================
 * m32_up_channel()
 *
 * Description:
 * Put a channel into the UP state and assign it buffers.
 * Should only be called for a channel in the DOWN or STANDBY
 * state. If channel is already in the UP state, do nothing.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel to be up'ed 
 *
 * Returns:
 * m32 error code
 *
 * Notes:
 * This function increments the ar_in_progress counter. make sure the 
 * function decrements this variable at all return points.
 * =====================================================================
 */
enum m32_error_t m32_up_channel (m32_instance_t *m32_ds,
				 ulong channel)
{
    enum m32_channel_state_t state;
    enum m32_error_t status = M32_NOERR;

    state = m32_ds->channel_params[channel].current_state;
    /* 
     * do nothing if the channel is already up
     */
    if (state == M32_CHANNEL_UP) {
        return(M32_NOERR);
    }

    status = m32_mark_ar_in_progress(m32_ds);
    if (status != M32_NOERR)
        return (status);

    /*
     * can't UP an un-initialized or throttled channel
     */
    if ((state == M32_CHANNEL_UNUSED) || (state == M32_CHANNEL_ILLEGAL)) {
        status = M32_ERR_BAD_STATE;
    } else if ((state == M32_CHANNEL_DOWN) ||
	       (state == M32_CHANNEL_THROTTLED)) {
        /*
         * if the channel is down it needs to reinitialize the
         * content of all of descriptors and head/tail pointers.
         * And then allocate buffers.
         */
        m32_initialize_descriptors(m32_ds, channel);
        status = m32_alloc_rx_buffers(m32_ds, channel);
    }
    
    if (status == M32_NOERR) {
        /*
         * set the channel command in the control struct to
         * do a clear command.
         */
        m32_ds->ctrl->ctrl_channels[channel].channel_info =
          M32_CHANNEL_CLEAR_MASK;
        m32_ds->ctrl->ctrl_channels[channel].channel_itbs = 0;
        /*
         * execute an action request for this channel
         */
        status = m32_execute_ar(m32_ds, channel);
        if (status == M32_NOERR) {
            /*
             *  set the channels state to show it's up.
             */
            m32_ds->channel_params[channel].current_state = 
              M32_CHANNEL_UP;
        }
    }

    m32_unmark_ar_in_progress_inline(m32_ds);

    return(status);
}


/*
 * =====================================================================
 * m32_tx_start()
 *
 * Description:
 * Start a packet transmission out a Munich32 channel if the 
 * channel is up. Keep transmitting until the hold queue is
 * empty or the transmit ring fills up. Update the ring head
 * and transmit ring use count as packets are transmitted.
 *
 * Parameters:
 * idb pointer
 *
 * Returns:
 * nothing
 * =====================================================================
 */
void m32_tx_start (hwidbtype *idb)
{
    leveltype int_level;
    m32_channel_params_t *channel_params = NULL;
    m32_instance_t *m32_instance;
    m32_pak_ring_t *tx_pak_head;
    m32_xmit_desc_t *prev_tx_ring_head,
                    *tx_ring_head;
    paktype *pak;
    ushort bytes;

    int_level = raise_interrupt_level(NETS_DISABLE);

    m32_instance = M32_INSTANCE(idb->slot, idb->subcont);
    channel_params = &(m32_instance->channel_params[idb->m32_channel]);
    /*
     * only send data out an initialized, up channel.
     */
    if ((channel_params->current_state == M32_CHANNEL_UP) 
        || (channel_params->current_state == M32_CHANNEL_THROTTLED)){
        /*
         * save a pointer to the last used tx ring descriptor
         * also, set the new tx head ptr to this value. it
         * will be updated inside the loop.
         */
        prev_tx_ring_head = channel_params->tx_ring_head;
        tx_ring_head = channel_params->tx_ring_head;

        /*
         * loop thru the hold queue, pulling packets off of it and
         * sending them. keep looping until the hold queue is empty
         * or all the tx descriptors are in use.
         */
        while(channel_params->tx_ring_use_count < 
                              (channel_params->tx_ring_size - 1)
               && (pak = (*idb->oqueue_dequeue)(idb))) {
            /*
             * make sure the number of bytes to send does not
             * exceed the MTU for this channel.
             */
            bytes = pak->datagramsize;
            if (bytes > idb->max_pak_size) {
                idb->counters.output_total_drops++;
                datagram_done(pak);
                continue;
            } 

            /*
             * move the head pointer to the next descriptor in the ring.
             * record that another descriptor is in use.
             */
            channel_params->tx_ring_head = 
              channel_params->tx_ring_head->next_xmit_desc;
            tx_ring_head = channel_params->tx_ring_head;
            channel_params->tx_pak_head = 
              channel_params->tx_pak_head->next_pak;
            tx_pak_head = channel_params->tx_pak_head;

            /*
             * Set up this TX ring entry for this packet:
             * assign the packet to the descriptor, set the number
             * of bytes to be transferred and set the status bits. 
             */
            tx_pak_head->pak = pak;
            tx_ring_head->xmit_params |= (bytes << M32_XMIT_DESC_NO_SHIFT);
            tx_ring_head->xmit_data_ptr = pak->datagramstart;
            channel_params->tx_ring_use_count++;

            /*
             * do packet accounting.
             */
            idb->counters.tx_cumbytes += bytes;
            /*
             * Set timeout timer and bump the output counter
             */
            TIMER_START(idb->xmittimer, TIMEOUT); 
            idb->counters.outputs++;

            /*
             * Count broadcasts
             */
            if ((pak->enctype == ET_HDLC) &&
                (GETSHORT(pak->datagramstart) & HDLC_BCAST)) {
               idb->counters.output_broad++;
            }
        } /* while loop */

        /*
         * if any data needs to be sent set the hold bit
         * in the new head descriptor and clear the hold bit 
         * on the previous head descriptor. this causes the data 
         * in the descriptors after the previous head up to and 
         * including the current tx head to be sent.
         */
        if (tx_ring_head != prev_tx_ring_head) {
            tx_ring_head->xmit_params |= M32_XMIT_DESC_HOLD;
            prev_tx_ring_head->xmit_params &= ~(M32_XMIT_DESC_HOLD);
        }
    }

    /* Restore interrupt level */
    reset_interrupt_level(int_level);    
}


/*
 * =====================================================================
 * m32_print_controller()
 *
 * Description:
 * Print info about the current state of Munich32 channels.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void m32_print_controller (m32_instance_t *m32_ds)
{
    short ix;

    printf("\nInstance Structure");
    printf("\n------------------");
    printf("\nCCS address %x, pcm_format %x",m32_ds->ctrl,m32_ds->pcm_format);
    printf("\nAction request: ");
    printf("in progress %d, success %d, timeout %d, failure %d", 
        m32_ds->ar_in_progress, m32_ds->ar_success, 
        m32_ds->ar_timeout, m32_ds->ar_failure);
    printf("\ninterrupt queue index %d, ", m32_ds->intq_index);
    printf("bad int q elements %d", m32_ds->bad_intq_element_count);
    if (m32_ds->idle_channel == M32_IDLE_CHNL_UNASSIGNED) {
        printf("\nidle channel Unassigned, ");
    } else {
        printf("\nidle channel %d, ", m32_ds->idle_channel);
    }
    printf("idle timeslots %x, idle idb %x", 
        m32_ds->idle_timeslots, m32_ds->idle_idb);


    printf("\n");
    printf("\nControl and Configuration Structure");
    printf("\n-----------------------------------");
    printf("\nAction Specification: %x", m32_ds->ctrl->ctrl_action);

    printf("\nInterrupt Queue: address %x, blocks %d (elements %d)",
        m32_ds->ctrl->ctrl_intq.intq_addr, 
        m32_ds->ctrl->ctrl_intq.num_q_elements,
        (m32_ds->ctrl->ctrl_intq.num_q_elements + 1) * 16);

    printf("\nTimeslots:");
    for (ix = 0; ix < M32_NUM_TIMESLOTS; ix++) {
        if (ix % 6 == 0) printf("\n");
        printf("%d)%04x%04x ", ix,
	       m32_ds->ctrl->ctrl_timeslots[ix].xmit_timeslot,
	       m32_ds->ctrl->ctrl_timeslots[ix].recv_timeslot);
    }
    printf("\nChannel Specifications starts at 0x%x07:", 
           m32_ds->ctrl->ctrl_channels);
    for (ix = 0; ix < M32_NUM_CHANNELS; ix++) {
        printf("\n%d)info %04x, frda %x, ftda %x, itbs %d ",
           ix, m32_ds->ctrl->ctrl_channels[ix].channel_info,
               m32_ds->ctrl->ctrl_channels[ix].first_recv_desc,
               m32_ds->ctrl->ctrl_channels[ix].first_xmit_desc,
               m32_ds->ctrl->ctrl_channels[ix].channel_itbs);
    }
    printf("\nCurrent Receive Descriptors starts at 0x%x07:", 
           m32_ds->ctrl->ctrl_curr_recv_desc);
    for (ix = 0; ix < M32_NUM_CHANNELS; ix++) {
        if (m32_ds->ctrl->ctrl_curr_recv_desc[ix]) {
            printf("\n%d)params %04x, status %04x, data ptr %x, next ptr %x",
               ix, m32_ds->ctrl->ctrl_curr_recv_desc[ix]->recv_params,
                   m32_ds->ctrl->ctrl_curr_recv_desc[ix]->recv_status,
                   m32_ds->ctrl->ctrl_curr_recv_desc[ix]->recv_data_ptr,
                   m32_ds->ctrl->ctrl_curr_recv_desc[ix]->next_recv_desc);
        } else {
            printf("\n%d)params 0, status 0, data ptr 0, next ptr 0", ix);
        }
    }
    printf("\nCurrent Transmit Descriptors starts at 0x%x07:",
           m32_ds->ctrl->ctrl_curr_xmit_desc);
    for (ix = 0; ix < M32_NUM_CHANNELS; ix++) {
        if (m32_ds->ctrl->ctrl_curr_xmit_desc[ix]) {
            printf("\n%d)params %04x, data ptr %x, next ptr %x",
               ix, m32_ds->ctrl->ctrl_curr_xmit_desc[ix]->xmit_params,
                   m32_ds->ctrl->ctrl_curr_xmit_desc[ix]->xmit_data_ptr,
                   m32_ds->ctrl->ctrl_curr_xmit_desc[ix]->next_xmit_desc);
        } else {
            printf("\n%d)params 0, data ptr 0, next ptr 0", ix);
        }
    }
    printf("\n");

    printf("\nChannel data:");
    for (ix = M32_NUM_CHANNELS-1; ix >= 0; ix--) {
        printf("\n%d)chan %d, idb %x, bw %d, ts bitfield %08x, data offset %d "
               "pool group %d",
           ix, m32_ds->channel_params[ix].channel_num,
               m32_ds->channel_params[ix].channel_idb,
               m32_ds->channel_params[ix].bandwidth,
               m32_ds->channel_params[ix].timeslot_bitfield,
               m32_ds->channel_params[ix].data_offset,
               (m32_ds->channel_params[ix].channel_idb?
                m32_ds->channel_params[ix].channel_idb->pool_group:0));
        printf("\n  throttle %d, throttle count %d, enable count %d, ",
               m32_ds->channel_params[ix].throttle_channel,
               m32_ds->channel_params[ix].throttle_count,
               m32_ds->channel_params[ix].enable_count,
               m32_ds->channel_params[ix].loopback);
        printf("loopback ");
        switch (m32_ds->channel_params[ix].loopback) {
        case M32_NO_LOOPBACK:
            printf("NO LOOPBACK, ");
            break;
        case M32_LOCAL_LOOPBACK:
            printf("LOCAL LOOPBACK, ");
            break;
        case M32_REMOTE_LOOPBACK_WAIT_UP:
        case M32_REMOTE_LOOPBACK_ACTIVE:
        case M32_REMOTE_LOOPBACK_WAIT_DOWN:
            printf("REMOTE LOOPBACK, ");
            break;
        default:
            printf("UNKNOWN (%d), ", m32_ds->channel_params[ix].loopback);
            break;
        }
        printf("\n  handlers: recv %x, xmit %x, disable %x, enable %x down %x",
               m32_ds->channel_params[ix].recv_handler,
               m32_ds->channel_params[ix].xmit_handler,
               m32_ds->channel_params[ix].disable_adaptor,
               m32_ds->channel_params[ix].enable_adaptor,
               m32_ds->channel_params[ix].down_connection);
        printf("\n  rx ring size %d, tx ring size %d, tx use count %d",
               m32_ds->channel_params[ix].rx_ring_size,
               m32_ds->channel_params[ix].tx_ring_size,
               m32_ds->channel_params[ix].tx_ring_use_count);
        printf("\n  ints unused/idle chan %d,",
               m32_ds->channel_params[ix].ints_unused_channel);
        printf(" initial state ");
        switch (m32_ds->channel_params[ix].initial_state) {
        case M32_CHANNEL_UNUSED:
            printf("UNUSED, ");
            break;
        case M32_CHANNEL_IDLE:
            printf("IDLE, ");
            break;
        case M32_CHANNEL_DOWN:
            printf("DOWN, ");
            break;
        case M32_CHANNEL_STANDBY:
            printf("STANDBY, ");
            break;
        case M32_CHANNEL_UP:
            printf("UP, ");
            break;
        case M32_CHANNEL_THROTTLED:
            printf("THROTTLED, ");
            break;
        case M32_CHANNEL_ILLEGAL:
        default:
            printf("ILLEGAL (%d), ", m32_ds->channel_params[ix].initial_state);
            break;
        }
        printf("current state ");
        switch (m32_ds->channel_params[ix].current_state) {
        case M32_CHANNEL_UNUSED:
            printf("UNUSED");
            break;
        case M32_CHANNEL_IDLE:
            printf("IDLE");
            break;
        case M32_CHANNEL_DOWN:
            printf("DOWN");
            break;
        case M32_CHANNEL_STANDBY:
            printf("STANDBY");
            break;
        case M32_CHANNEL_UP:
            printf("UP");
            break;
        case M32_CHANNEL_THROTTLED:
            printf("THROTTLED");
            break;
        case M32_CHANNEL_ILLEGAL:
        default:
            printf("ILLEGAL (%d)", m32_ds->channel_params[ix].current_state);
            break;
        }
    }
}


/*
 * =====================================================================
 * m32_print_rings()
 *
 * Description:
 * Print the receive and transmit rings for a Munich32 channel.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel from which to print the rings
 *
 * Returns:
 * nothing
 * =====================================================================
 */
void m32_print_rings (m32_instance_t *m32_ds,
                      ulong channel)
{
    leveltype int_level;
    m32_channel_params_t *channel_params;
    m32_recv_desc_t *rx_ring, *rx_ring_temp, *rx_head;
    m32_xmit_desc_t *tx_ring, *tx_ring_temp, *tx_head;
    ulong rx_ring_size, tx_ring_size;
    ulong bit_mask, timeslot;
    ushort ix, new_line_counter;
    ulong intq_element[30], intq_index;

    channel_params = &(m32_ds->channel_params[channel]);
    rx_ring_size = channel_params->rx_ring_size;
    tx_ring_size = channel_params->tx_ring_size;

    rx_head= m32_ds->ctrl->ctrl_channels[channel].first_recv_desc;
    tx_head= m32_ds->ctrl->ctrl_channels[channel].first_xmit_desc;

    /*
     * This check is neccessary to prevent bus error CSCdj19916.
     * It is possible to have a NULL rx/tx_head if malloc failure
     * occurs during shut/no shut interface, an error message is 
     * generated at that time. In this routine, we just return,   
     * Because rx/tx_head is used later in this funtion to access
     * to ring buffer and cause bus errors. 
     */
    if ((rx_head == NULL) || (tx_head ==NULL))
	return;

    /*
     * Allocate memory for the RX and TX rings
     */
    rx_ring = malloc(sizeof(m32_recv_desc_t) * rx_ring_size + 16);
    if (rx_ring == NULL) {
        return;
    }
    tx_ring = malloc(sizeof(m32_xmit_desc_t) * tx_ring_size + 16);
    if (tx_ring == NULL) {
        free(rx_ring);
        return;
    }

    /*
     * Disable network interrupts 
     */
    int_level = raise_interrupt_level(NETS_DISABLE);
    /*
     * take a snapshot of the rx and tx rings so
     * interrupts don't stay disabled while printing.
     */
    bcopy(rx_head, rx_ring,
          (rx_ring_size * sizeof(m32_recv_desc_t)));
    bcopy(tx_head, tx_ring,
          (tx_ring_size * sizeof(m32_xmit_desc_t)));
 
    {
        ulong *intq_head, *src, *dest;
 
        intq_head = m32_ds->ctrl->ctrl_intq.intq_addr;
        intq_index = m32_ds->intq_index;
        src = &intq_head[intq_index];
        dest = intq_element;
        for(ix=0; ix < 30; ix++) {
            *dest++ = *src++;
            if (++intq_index >= M32_INTQ_ELEMENTS) {
                intq_index = 0;
                src = intq_head;
            }
        }
        intq_index = m32_ds->intq_index;
    }
    /*
     * set network interrupts back to what they were
     */
    reset_interrupt_level(int_level);

    printf("  Interrupt Queue Element(index=%d): interrupt is %s\n  ", intq_index,
           ((m32_ds->regs->int_enable_reg & M32_REG_INT_ENABLE) ?
            "enabled" : "disabled"));
    {
        ushort jx;
        ulong  *intq;
 
        for (ix=0, intq=intq_element; ix < 5; ix++) {
            for (jx=0; jx <6; jx++)
                printf("0x%07x ", *intq++);
            printf("\n  ");
        }
        printf("\n");
    }
 
    printf("  Channel state: ");
    switch (channel_params->current_state) {
    case M32_CHANNEL_UNUSED:
        printf("UNUSED");
        break;
    case M32_CHANNEL_IDLE:
        printf("IDLE");
        break;
    case M32_CHANNEL_DOWN:
        printf("DOWN");
        break;
    case M32_CHANNEL_STANDBY:
        printf("STANDBY");
        break;
    case M32_CHANNEL_UP:
        printf("UP");
        break;
    case M32_CHANNEL_THROTTLED:
        printf("THROTTLED");
        break;
    case M32_CHANNEL_ILLEGAL:
    default:
        printf("ILLEGAL (%d)", channel_params->current_state);
        break;
    }
    printf("  Channel IDB: %x\n", channel_params->channel_idb);

    /* 
     * print out general rx ring information.
     */
    printf("  RX ring entries: %d, buffer size %d\n", 
      rx_ring_size, channel_params->channel_idb->pool->size);
    printf("  RX descriptor ring: head = %x, tail = %x\n", 
      channel_params->rx_ring_head, channel_params->rx_ring_tail);
    printf("  RX pak ring: head = %x, tail = %x\n", 
      channel_params->rx_pak_head, channel_params->rx_pak_tail);
    /* 
     * loop thru the rx ring entries and print out descriptor
     * specific information.
     */
    rx_ring_temp = rx_ring;
    for (ix = 0; ix < rx_ring_size; ix++) {
      printf("  %02d params=0x%x status=0x%x data ptr=0x%07x next ptr=0x%07x\n",
         ix, rx_ring_temp->recv_params, rx_ring_temp->recv_status,
         rx_ring_temp->recv_data_ptr, rx_ring_temp->next_recv_desc); 
      rx_ring_temp++;
    }

    /* 
     * print out general tx ring information.
     */
    printf("  TX ring entries: %d, in use: %d, buffer size %d\n", 
      tx_ring_size, channel_params->tx_ring_use_count, 
      channel_params->channel_idb->pool->size);
    printf("  TX descriptor ring: head = %x, tail = %x\n", 
      channel_params->tx_ring_head, channel_params->tx_ring_tail);
    printf("  TX pak ring: head = %x, tail = %x\n", 
      channel_params->tx_pak_head, channel_params->tx_pak_tail);
    /* 
     * loop thru the tx ring entries and print out descriptor
     * specific information.
     */
    tx_ring_temp = tx_ring;
    for (ix = 0; ix < tx_ring_size; ix++) {
        printf("  %02d params=0x%x data ptr=0x%07x next ptr=0x%07x\n", 
          ix, tx_ring_temp->xmit_params, tx_ring_temp->xmit_data_ptr, 
          tx_ring_temp->next_xmit_desc); 
        tx_ring_temp++;
    }

    /*
     * Give other processes a chance to run.
     */
    process_sleep_for(1);

    /* 
     * list the timeslots assigned to this channel.
     */
    printf("  List of timeslots (sw):");
    bit_mask = channel_params->timeslot_bitfield;
    timeslot = 0;
    new_line_counter = 0;
    while (bit_mask) {  /* while there are still timeslot bits set */
        if (bit_mask & 1) {
            printf(" %d", timeslot);
            new_line_counter++;
            if (new_line_counter > 16) {
                printf("\n                         ");
                new_line_counter = 0;
            }
        }
        bit_mask >>= 1;
        timeslot++;
    }
    printf("\n  List of all timeslots (hw):");
    printf("\n  ");
    for (ix = 0; ix < M32_NUM_TIMESLOTS; ix++) {
        if ((ix % 6 == 0) && (ix != 0)) printf("\n  ");
        printf(" %d:%04x%04x", ix,
	       m32_ds->ctrl->ctrl_timeslots[ix].xmit_timeslot,
	       m32_ds->ctrl->ctrl_timeslots[ix].recv_timeslot);
    }
    printf("\n  Bandwidth: %d, idle channel: ", channel_params->bandwidth);
    if (m32_ds->idle_channel == M32_IDLE_CHNL_UNASSIGNED) {
        printf("Unassigned,");
    } else {
        printf("%d", m32_ds->idle_channel);
    }
    printf(" idle ts bitfield: 0x%x\n", m32_ds->idle_timeslots);
    /* 
     * free the memory used to hold the rings.
     */
    free(rx_ring);
    free(tx_ring);
    if (serial_debug) {
        m32_print_controller(m32_ds);
    }
}


/*
 * =====================================================================
 * m32_enable()
 *
 * Description:
 * re-enable a channel that has been throttled. go through
 * the control struct and look for all timeslots that are
 * assinged to the channel that's getting throttling cleared.
 * when such a timeslot is found, clear the receive inhibit 
 * bit to allow the timeslot to receive data again.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * channel - munich32 channel being configured
 *
 * Returns:
 * nothing
 *
 * Notes:
 * This function increments the ar_in_progress counter. make sure the 
 * function decrements this variable at all return points.
 * =====================================================================
 */
void m32_enable (m32_instance_t *m32_ds, 
                 ulong channel, 
                 enum m32_channel_state_t new_state)
{
    hwidbtype *idb;
    m32_channel_params_t *channel_params;
    
    idb = m32_get_channel_idb_inline(m32_ds, channel);
    channel_params = &(m32_ds->channel_params[channel]);

    /*
     * clear the throttled receive descriptor's hold bit. 
     */
    if ((channel_params->current_state == M32_CHANNEL_THROTTLED) 
        && channel_params->rx_desc_throttle_hold) {
        channel_params->rx_desc_throttle_hold->recv_params &= ~(M32_RECV_DESC_HOLD);
        channel_params->rx_ring_tail->recv_params |= M32_RECV_DESC_HOLD;
        channel_params->rx_desc_throttle_hold = NULL;
        /*
         * set channel param fields and clear the flag which tells
         * the background task that this interface is being throttled.
         */
        channel_params->current_state = new_state;
        channel_params->enable_count++;
        idb_mark_unthrottled(idb);
    }
}


/*
 * =====================================================================
 * m32_init_channel()
 *
 * Description:
 * Initialize a channel. Programs timeslot and channel specific
 * information. Can be called from any state.
 *
 * Parameters:
 * m32_ds - pointer to the m32 instance structure
 * m32_channel_init - pointer to channel init structure
 *
 * Returns:
 * M32_NOERR
 * M32_ERR_NO_TIMESLOT
 * M32_ERR_AR_TIMEOUT
 * M32_ERR_ILLEGAL_CHANNEL
 * M32_ERR_TIMESLOT_IN_USE
 *
 * Notes:
 * This function increments the ar_in_progress counter. make sure the 
 * function decrements this variable at all return points.
 * =====================================================================
 */
enum m32_error_t m32_init_channel (m32_instance_t *m32_ds,
				   m32_channel_init_info_t *m32_channel_init)
{
    m32_channel_params_t *channel_params;
    volatile m32_timeslot_t *channel_timeslot;
    volatile m32_channel_ctrl_t *channel_control;
    enum m32_error_t status = M32_NOERR; 
    uchar fillmask, i;
    ulong bit_mask, channel, 
          timeslot, timeslot_bitrate, timeslots_found;
    ushort ix;

    /*
     * assign some of the long pointer derefrences
     * to local variables.
     */
    channel = m32_channel_init->channel_num;
    channel_params = &(m32_ds->channel_params[channel]);

    if (channel_params->current_state == M32_CHANNEL_ILLEGAL) {
        return(M32_ERR_BAD_STATE);
    }

    status = m32_mark_ar_in_progress(m32_ds);
    if (status != M32_NOERR)
        return (status);

    /*
     * if channel is being throttled clear all throttling values.
     * initializing a channel is meant to clear throttling on
     * that channel.
     */
    if (channel_params->current_state == M32_CHANNEL_THROTTLED) {
        channel_params->current_state = M32_CHANNEL_UP;
        channel_params->rx_desc_throttle_hold = NULL;
        idb_mark_unthrottled(channel_params->channel_idb);
    }
    /*
     * if channel is up put it into a known down state
     */
    if ((channel_params->current_state == M32_CHANNEL_UP) || 
        (channel_params->current_state == M32_CHANNEL_STANDBY) ||
        (channel_params->current_state == M32_CHANNEL_IDLE)) {
        status = m32_down_channel(m32_ds, channel);
        if (status != M32_NOERR) {
            m32_unmark_ar_in_progress_inline(m32_ds);
            return(status);
        }
    }

    /*
     * make sure the bitrate per timeslot makes sense.
     * otherwise return an error.
     */
    timeslot_bitrate = m32_channel_init->bitrate / M32_BITRATE_DIVISOR;
    if ((timeslot_bitrate > M32_MAX_FILLMASK) ||
        (timeslot_bitrate < 1)) {
        m32_unmark_ar_in_progress_inline(m32_ds);
        return(M32_ERR_BAD_BITRATE);
    }
    /*
     * make sure at least one timeslot has been assigned to
     * this channel, otherwise return an error
     */
    bit_mask = m32_channel_init->timeslot_bitfield;
    if (bit_mask == 0) {
        m32_unmark_ar_in_progress_inline(m32_ds);
        return(M32_ERR_NO_TIMESLOT);
    }
    /*
     * make sure all timeslots being assigned to this channel
     * are not assigned to any other channels. don't look at
     * the channel being initialized or the idle channel.
     */
    for (i = 0; i < M32_NUM_CHANNELS; i++) {
        if ((m32_ds->channel_params[i].timeslot_bitfield & bit_mask) &&
            (i != channel) &&
            (i != m32_ds->idle_channel)) {
            m32_unmark_ar_in_progress_inline(m32_ds);
            return(M32_ERR_TIMESLOT_IN_USE);
        }
    }

    /*
     * copy various channel specific values from the init
     * struct to the channel params struct
     */
    channel_params->channel_num = channel;
    channel_params->initial_state = m32_channel_init->initial_state;
    channel_params->channel_idb = m32_channel_init->channel_idb;
    channel_params->data_offset = m32_channel_init->data_offset;
    channel_params->throttle_channel = m32_channel_init->throttle_channel;
    channel_params->loopback = m32_channel_init->loopback;
    channel_params->xmit_handler = m32_channel_init->xmit_handler;
    channel_params->recv_handler = m32_channel_init->recv_handler;
    channel_params->disable_adaptor = m32_channel_init->disable_adaptor;
    channel_params->enable_adaptor = m32_channel_init->enable_adaptor;
    channel_params->down_connection = m32_channel_init->down_connection;
    channel_params->timeslot_bitfield = m32_channel_init->timeslot_bitfield;

    /*
     * The fillmask is calculated based on the configured timeslot_bitrate.
     */
       fillmask = 0;
       for (i = 0; i < timeslot_bitrate; i++) {
           if (m32_channel_init->bitrate_which_bits ==
              M32_TIMESLOT_USE_MSB) {
                fillmask >>= 1;
                fillmask |= 0x80;
            } else {
                fillmask <<= 1;
                fillmask |= 0x1;
            }
        }
    /* Save the calculated fillmask. This will be used to reset the speed to 
     * the configured value 
     */
    channel_params->fillmask = fillmask;  

    /*
     * If a remote loopup or loopdown is being performed the bitrate for
     * the timeslot should be set to 64 Kbps for the loopup/loopdown sequence.
     * The bitrate is set to the configured value when the sequence ends
     * through the m32_reset_hdlc_mode function.
     * Hence the fillmask is precalculated to use all bits and 0xff.
     */

    if ((channel_params->loopback == M32_REMOTE_LOOPBACK_WAIT_UP)
        || (channel_params->loopback == M32_REMOTE_LOOPBACK_WAIT_DOWN)) 
      fillmask = M32_TIMESLOT_BITRATE_64KBPS;


    /*
     * unbundle the timeslot bits. a channel can have multiple
     * timeslots associated with it.
     * assuming the xmit and recv timeslots are always the same
     * also assuming that if a channel has multiple timeslots
     * all timeslots will have the same bitrate.
     */
    timeslot = 0;
    timeslots_found = 0;
    while (bit_mask) {  /* loop while there are still timeslot bits set */
        /* 
         * check if the bit for this timeslot is set.
         */
        if (bit_mask & 1) {
            timeslots_found++;
            /*
             * put the channel into the recv and xmit timeslot fields.
             * this also clears the recv/xmit timeslot inhibit bit.
             */
            channel_timeslot = &(m32_ds->ctrl->ctrl_timeslots[timeslot]);
            channel_timeslot->recv_timeslot = 
               (channel << M32_TIMESLOT_CHNL_SHIFT);
            channel_timeslot->xmit_timeslot = 
               (channel << M32_TIMESLOT_CHNL_SHIFT);
            /*
             * put calculated fillmask in the timeslot field.
             * every bit set in the field represents a bit that
             * the m32 will pass in a 8 bit frame. all 8 bits set
             * is 64Kb, 7 bits is 56Kb etc.
             * The fillmask was calculated before the loop.
             */
            channel_timeslot->recv_timeslot |= 
              (fillmask & M32_TIMESLOT_BITRATE);
            channel_timeslot->xmit_timeslot |= 
              (fillmask & M32_TIMESLOT_BITRATE);
            if (channel == m32_ds->idle_channel) {
                channel_timeslot->recv_timeslot |= M32_TIMESLOT_INHIBIT;
            }

            /*
             * clear the idle timeslot bit in the instance.
             */
            if (channel != m32_ds->idle_channel) {
                m32_ds->idle_timeslots &= ~(1 << timeslot);
            }
        }
        /*
         * shift the just checked timeslot off, the next timeslot
         * will now be the right most bit. 
         */
        bit_mask >>= 1;
        timeslot++;
    }


    /*
     * the overall bandwidth of the channel is the bandwidth of a
     * timeslot times the number of timeslots assigned to this channel.
     */
    channel_params->bandwidth = m32_channel_init->bitrate * timeslots_found;

    /*
     * setup the channel specification used by the Munich32
     * to define the channel.
     */
    channel_control = &(m32_ds->ctrl->ctrl_channels[channel]);
    channel_control->channel_itbs= m32_channel_init->internal_buf_size;
 
    if ((channel_params->loopback == M32_REMOTE_LOOPBACK_WAIT_UP)
        || (channel_params->loopback == M32_REMOTE_LOOPBACK_WAIT_DOWN))
        channel_control->channel_info = (M32_CHANNEL_INIT_MASK
                                         | M32_CHANNEL_FA)
                                      & M32_CHANNEL_TMA_MASK;
    else
        channel_control->channel_info = (M32_CHANNEL_INIT_MASK
                                         | M32_CHANNEL_FA);
 
    /*
     * first free the descriptors and buffers of the old size.
     * initialize the channel ring parameters and allocate new rings
     * always use one more tx ring entry than requested. the way the
     * tx ring is setup, one entry is never used.
     */
    m32_free_descriptors(m32_ds, channel);
    channel_params->rx_ring_size = m32_channel_init->rx_ring_size + 1;
    channel_params->tx_ring_size = m32_channel_init->tx_ring_size + 1;

    status = m32_alloc_descriptors(m32_ds, channel);
    if (status == M32_NOERR) {

        if (channel_params->initial_state != M32_CHANNEL_DOWN) {
            status = m32_alloc_rx_buffers(m32_ds, channel);
        }

        /*
         * perform an action request for this channel
         */
        if (status == M32_NOERR && 
            (status = m32_execute_ar(m32_ds, channel)) == M32_NOERR) {
            channel_params->current_state = M32_CHANNEL_UP;

            if ((status == M32_NOERR) &&
                (channel_params->loopback == M32_LOCAL_LOOPBACK)) {
                /* it's supposed to wait for the the flag FI set in the 
                 * interrupt queue. However, current implementation will
                 * not allow the code to poll the status of interrupt
                 * queue. Therefore, a fix timeout is used to make sure
                 * that the FI is set after this delay.
                 */
               m32_delay_msec(500);
            }

            /* 
             * now that the channel is initialized, 
             * set the state of the channel according to the
             * requested initial state.
             */
            switch (channel_params->initial_state) {
            case M32_CHANNEL_DOWN:
                status = m32_down_channel(m32_ds, channel);
                break;
            case M32_CHANNEL_STANDBY:
                status = m32_abort_channel(m32_ds, channel);
                break;
            case M32_CHANNEL_IDLE:
                channel_params->current_state = M32_CHANNEL_IDLE;
                break;
            default:
            }
        }
    }
    
    if (status == M32_NOERR) {
        /*
         * if we're initializing any channel except the idle channel 
         * the idle timeslots have changed and we need to adjust the 
         * idle channel.
         */

        if (channel != m32_ds->idle_channel) {
            if (channel_params->current_state == M32_CHANNEL_UP) {
                if (channel_params->loopback == M32_LOCAL_LOOPBACK) {
                  m32_ds->ctrl->ctrl_action = m32_ds->pcm_format | M32_AR_MFL |
                                           (channel << M32_AR_CHANNEL_SHIFT) |
                                            M32_AR_EXTERNAL_CHANNEL_LOOP;
                  ix = m32_ds->regs->action_request_reg;
 
                  /*
                   * poll the interrupt bit to see when the operation is done.
                   */
                  m32_ar_poll(m32_ds);
                  m32_delay_msec(600);
                }
                else if (channel_params->loopback
                           == M32_REMOTE_LOOPBACK_WAIT_UP) {
                    m32_send_remote_loopback(m32_ds, channel);
                }
                else if (channel_params->loopback
                           == M32_REMOTE_LOOPBACK_WAIT_DOWN) {
                    m32_recv_remote_loopback(m32_ds, channel);
                }
            }
            m32_adjust_idle_channel(m32_ds);
        }
    } else {
        /*
         * an error occurred during init. free up resources.
         * don't free descriptors unless the channel is turned off.
         */
        m32_release_channel(m32_ds, channel);
    }

    m32_unmark_ar_in_progress_inline(m32_ds);
    return(status);
}
