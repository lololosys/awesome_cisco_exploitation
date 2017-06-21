/* $Id: ticelp.c,v 3.2 1995/11/17 17:35:32 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/ticelp.c,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ticelp.c,v $
 * Revision 3.2  1995/11/17  17:35:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/20  05:06:34  smackie
 * Remove scary pointer arithmetic from LAT buffer handling. Use a direct
 * embedded pointer to the packet header instead. (CSCdi40672)
 *
 * Revision 2.1  1995/06/07  21:29:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    tic_elapse		(e) process tic timer

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called by the environment for each tic and performs
    four separate sets of processing.

    1.  All active circuit timers are decremented and the appropriate action
    	taked for any circuit which timers which have expired (reaches zero).

    2.  If some event has potentially changed queue status, the queue is
	scanned to see if any entried may be started or should be rejected.

    3.  All active HIC timers are decremented and the appropriate action
	taken for any entries which timers have expried.

    4.	Any internal output messages are input.

    CALL BY:	void tic_elase()

    ON EXIT:	All tic based processing performed.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latsb.h"
#include "lathi.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"
#include "lat.h"
#include "../if/network.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	of_ptr	of_local[2];		/* Local node messages		*/
extern	word	src_addr[3];		/* Source address		*/
extern	word	dst_addr[3];		/* Destination address		*/
extern	int	qr_changes;		/* Possible changes to queue	*/
extern	int	tu_msec_tic;		/* Milliseconds per tic		*/
extern	int	tics_uncounted;		/* Uncounted tics		*/

void tic_elapse()
{
    cb_ptr	cb;			/* Pointer to circuit block	*/
    of_ptr	of;			/* Pointer to output frame	*/
    int		tics;			/* Elapsed tics			*/
    int		i;			/* Index to circuit hash table	*/

/*-----------------------------------------------------------------------
    Set number of tics to process.
 -----------------------------------------------------------------------*/
    if (tics_uncounted == 0)
	tics = 1;
    else
    {
	tics = tics_uncounted;
	tics_uncounted = 0;
    }

/*-----------------------------------------------------------------------
    Cycle through all circuit blocks and count down the timer. When the
    timer expires, dispatch according to the circuit state.
 -----------------------------------------------------------------------*/
    for (i = 1; i < hs->hs_cb_index; i++)
    {
    	if ((cb = hs->hs_cb_table[i]) != 0 && cb->cb_tics != 0)
	{
	    if (cb->cb_tics > tics)
		cb->cb_tics -= tics;
	    else
	    {
		cb->cb_tics = 0;
		
/*-----------------------------------------------------------------------
    If service is balanced, convert state to RUN/RUN_W as it time to generate
    a keepalive message.
 -----------------------------------------------------------------------*/
		if (cb->cb_state == CB_BALANCE)
		{
		    if (cb->cb_type == CB_SERVER)
			cb->cb_state = CB_RUN;
		    else
			cb->cb_state = CB_RUN_W;
		}

/*-----------------------------------------------------------------------
    If service is idle but multiple messages received in last circuit
    interval, stay in RUN state. The host is probably confused and we
    should not go balanced.
 -----------------------------------------------------------------------*/
		if (cb->cb_state == CB_IDLE && cb->cb_msg_tic > 1)
		    cb->cb_state = CB_RUN;

/*-----------------------------------------------------------------------
    Process the three states which generate a RUN message. If no output
    frame can be generated, then reset timer and try next interval.

    These states are typical for a server circuit. A host circuit never
    goes to IDLE state and only goes to RUN state if its output buffer
    allocation fails. The host circuit then tries to generate the RUN
    message at each tic interval.
 -----------------------------------------------------------------------*/
		if (cb->cb_state == CB_RUN  ||
		    cb->cb_state == CB_IDLE ||
		    cb->cb_state == CB_BALANCE_R)
		{
		    cb->cb_msg_tic = 0;

/*-----------------------------------------------------------------------
    If no sessions on the circuit, abort it.
  -----------------------------------------------------------------------*/
		    if (cb->cb_slot[0] == 0 && cb->cb_type ==CB_SERVER)
		    {
			cb->cb_halt_reason = HALTC_DONE;
			cb_abort(cb);
		    }

/*-----------------------------------------------------------------------
    Otherwise, try and output.
  -----------------------------------------------------------------------*/
		    else
			cb_run_output(cb);
		}

/*-----------------------------------------------------------------------
    Process the server mode which generates a START message. If no output
    frame can be generated, then reset timer and try next interval.
 -----------------------------------------------------------------------*/
		else if (cb->cb_state == CB_START)
		{
		    if ((of = of_alloc()) == 0)
			cb->cb_tics = 1;
		    else
		    {
			cb_o_start_message(of,cb);

    			if (cb->cb_xmit[0] == 0)
			    cb->cb_xmit[0] = of;
			else
			    cb->cb_xmit[1]->of_link = of;

    			cb->cb_xmit[1] = of;
			of->of_link    = 0;
			of_message(of);

			cb->cb_state   = CB_START_W;
			cb->cb_tics    = cb->cb_rx_tics;
			cb->cb_retries = hs->hs_cb_retries;
		    }
		}

/*-----------------------------------------------------------------------
    For servers and hosts, check if waiting for an acknowledgement. If so,
    count the retransmission and retransmit the message if any retries are
    remaining.
 -----------------------------------------------------------------------*/
		else if (cb->cb_state == CB_RUN_W || cb->cb_state == CB_START_W)
		{
		    if (cb->cb_retries-- != 0)
		    {
			cb_retransmit(cb,SUCCESS);
			cb->cb_tics = cb->cb_tics_sec;
		    }

/*-----------------------------------------------------------------------
    When retries are exhausted, give up on the circuit. Abort the circuit.
 -----------------------------------------------------------------------*/
		    else
		    {
			count_h(ch_total_timeouts);
			cb->cb_halt_reason = HALTC_RETRANSMIT;
			cb_abort(cb);
		    }
		}
	    }
	}
    }

#if (LE_OBJECTS || CME_SLAT) && LE_OBJECT_HIC
/*-----------------------------------------------------------------------
    If any possible changes to queue status, check the queue.
 -----------------------------------------------------------------------*/
    if (qr_changes)
    {
        qr_changes = FAILURE;
	qr_scan();
    }
#endif

#if (LE_SUBJECT_HIC || LE_OBJECT_HIC)
/*-----------------------------------------------------------------------
    Cycle through all HIC structures and count down the timer. When the
    timer expires, dispatch according the current state.
 -----------------------------------------------------------------------*/
    {
        hi_ptr	hi;
        hi_ptr	hx;

        for (hi = hs->hs_hi_head[0]; hi != 0; hi = hx)
        {
	    hx = hi->hi_link[0];

	    if (hi->hi_tics != 0 && (hi->hi_tics -= tics) <= 0)
	    {
	        of = 0;
	    	
/*-----------------------------------------------------------------------
    If no retries left and this is a solicit message, process it; else
    tell the environment we timed out and delete the HIC structure.
 -----------------------------------------------------------------------*/
	        if (hi->hi_retries-- == 0)
		{
#if LE_SUBJECT_HIC		
		    if (hi->hi_function == LAT_SOLICIT)
		    {
		    	if (hi->hi_resp_status & HI_SBINIT)
			    sb_ri_action(hi);
			else if (hi->hi_resp_status & HI_RESPONSE)
			    hi_h_ri_action(hi);
		    	else
		    	    hi_delete(hi,HX_TIMEOUT);
		    }
		    else
#endif		
		        hi_delete(hi,HX_TIMEOUT);
		}

#if LE_SUBJECT_HIC
/*-----------------------------------------------------------------------
    If this is a solicit message and response(s) received, reset the
    timer and ignore it.
 -----------------------------------------------------------------------*/
 		else if(hi->hi_function == LAT_SOLICIT &&
			  hi->hi_resp_status & HI_RESPONSE)
		    hi->hi_tics = (1000 / tu_msec_tic) * hs->hs_hi_cmd_timer;
#endif
		
/*-----------------------------------------------------------------------
    Allocate an output buffer. If this fails, reset timer interval
    to try function on next tic.
 -----------------------------------------------------------------------*/
	        else if ((of = of_alloc()) == 0)
	        {
		    hi->hi_retries++;
		    hi->hi_tics = 1;
	        }
	
#if LE_SUBJECT_HIC
/*-----------------------------------------------------------------------
    Generate solicit message.
 -----------------------------------------------------------------------*/
	        else if (hi->hi_function == LAT_SOLICIT)
	        {
		    hi->hi_tics = (1000 / tu_msec_tic) * hs->hs_hi_cmd_timer;
		    	hi_o_si_message(of,hi);
	        }

/*-----------------------------------------------------------------------
    Generate command message and reset the timer.
 -----------------------------------------------------------------------*/
	        else if (hi->hi_function == LAT_COMMAND)
	        {
		    hi_o_cmd_message(of,hi);
		    hi->hi_tics = (1000 / tu_msec_tic) * hs->hs_hi_cmd_timer;
	        }
#endif		

#if LE_OBJECT_HIC
/*-----------------------------------------------------------------------
    Generate response message and delete hi.
 -----------------------------------------------------------------------*/
	        else if (hi->hi_function == LAT_INFO)
#if LE_SERVER_AGENT
		    if (hi->hi_modifier == HI_AGENT)
		    	hi_o_ra_message(of,hi);
		    else
#endif
	            	hi_o_ri_message(of,hi);

/*-----------------------------------------------------------------------
    Generate status message and reset the timer if timed status reports
    are desired.
 -----------------------------------------------------------------------*/
	        else if (hi->hi_function == LAT_STATUS)
		    hi_o_sts_message(of,hi);
#endif

/*-----------------------------------------------------------------------
    Send generated HIC message. If done with HI entry, delete it.
 -----------------------------------------------------------------------*/
	        if (of != 0)
	        {
		    of->of_size  |= OF_DELETE;

		    of->of_dst[0] = hi->hi_dest_addr[0];
		    of->of_dst[1] = hi->hi_dest_addr[1];
		    of->of_dst[2] = hi->hi_dest_addr[2];
		    of->of_idb = hi->hi_idb;
	            of_message(of);

		    if (hi->hi_flags & HI_DELETE)
		        hi_delete(hi,0);
	        }
	    }
	}
    }
#endif

/*-----------------------------------------------------------------------
    If any of the above processing generated local messages, input
    them now.
 -----------------------------------------------------------------------*/
    while ((of = of_local[0]) != 0)
    {
	count_h(ch_total_recv);
	
        if ((of_local[0] = of->of_i_o) == 0)
	     of_local[1] = (of_ptr)&of_local[0];
	
	dst_addr[0] = of->of_dst[0];
	dst_addr[1] = of->of_dst[1];
	dst_addr[2] = of->of_dst[2];

	src_addr[0] = swap(((ushort *)nullidb->hwptr->hardware)[0]);
	src_addr[1] = swap(((ushort *)nullidb->hwptr->hardware)[1]);
	src_addr[2] = swap(((ushort *)nullidb->hwptr->hardware)[2]);

        lat_currentpak = of->of_pak;
	if_message(of->of_data,&src_addr[0],&dst_addr[0],nullidb);
	
	of->of_size &= ~OF_OUTPUT;
	if (of->of_size & OF_DELETE)
	    of_free(of);
    }
}
