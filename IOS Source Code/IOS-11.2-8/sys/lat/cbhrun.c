/* $Id: cbhrun.c,v 3.2 1995/11/17 17:32:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/cbhrun.c,v $
 *------------------------------------------------------------------
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cbhrun.c,v $
 * Revision 3.2  1995/11/17  17:32:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:22:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    cb_h_run_message	(i) process run message to host

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called when a run message to the host is received.

    CALL BY:	void cb_h_run_message(frame,cb)
		if_byte	frame;
		cb_ptr	cb;

    ON EXIT:	Returns when the message processed.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latnd.h"
#include "latsb.h"
#include "latmsg.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

void cb_h_run_message(frame,cb)

    if_byte	frame;			/* Pointer to input  message	*/
    cb_ptr	cb;			/* Pointer to circuit block	*/
{
    of_ptr	of;			/* Pointer to output frame	*/
    sb_ptr	sb;			/* Pointer to session block	*/
    nd_ptr	nd;			/* Pointer to remote node	*/
    cir_iptr	cir;			/* Pointer to circuit header	*/
    slot_iptr	slot;			/* Pointer to ../hes/slot.header	*/
    if_byte	next;			/* Pointer to next input byte	*/
    int		no_slots;		/* Number of slots in message	*/
    int		ses;			/* Local session number		*/

/*-----------------------------------------------------------------------
    Get a pointer to the message header.
 -----------------------------------------------------------------------*/
    cir = (cir_iptr)frame;
    nd  = cb->cb_nd;

/*-----------------------------------------------------------------------
    Process any acknowledgement. The sixth byte of the circuit frame
    is the message sequence number.
 -----------------------------------------------------------------------*/
    if ((cb->cb_msg_ack - cir->cir_ack) & 0x80)
    {
        while (cir->cir_ack != cb->cb_msg_ack)
        {
	    cb->cb_msg_ack++;
	    if ((of = cb->cb_xmit[0]) != 0 && *(of->of_data+6) == cb->cb_msg_ack)
	    {
		if ((cb->cb_xmit[0] = of->of_link) == 0)
		     cb->cb_xmit[1] = (of_ptr)&cb->cb_xmit[0];

	        if (of->of_size & OF_OUTPUT)
		    of->of_size |= OF_DELETE;
	        else
		    of_free(of);
	    }
	}
    }

/*-----------------------------------------------------------------------
    Check if this is next message we expect. Count as duplicate if not
    and set number of slots to zero.
 -----------------------------------------------------------------------*/
    if (LOBYTE(cir->cir_seq - cb->cb_msg_rcv) != 1)
    {
	count_1(co_duplicates);
	no_slots = 0;
    }
    else
    {
	cb->cb_msg_rcv++;
	no_slots = cir->cir_slots;
    }

/*-----------------------------------------------------------------------
    Loop through the slots in the message and process each.
 -----------------------------------------------------------------------*/
    next = frame + sizeof(cir_msg);

    count_1(co_msgs_recv);
    count_x(co_slot_recv,no_slots);

    while (no_slots-- != 0)
    {
	slot = (slot_iptr)next;
	count_x(co_byte_recv,slot->slot_len);

/*-----------------------------------------------------------------------
    Put ourselves in RUN WAIT so any sb_signals do not trigger yet.
 -----------------------------------------------------------------------*/
	cb->cb_state = CB_RUN_W;

/*-----------------------------------------------------------------------
    Use the destination slot index (ours) to lookup the session block.
 -----------------------------------------------------------------------*/
	ses = slot->slot_dst;
	for (sb = cb->cb_sb[sb_hash(ses)][0]; sb != 0; sb = sb->sb_link[0])
	{
	    if (sb->sb_lcl_ses_id == slot->slot_dst)
		break;
	}

/*-----------------------------------------------------------------------
    Process the slot.
 -----------------------------------------------------------------------*/
	sb_1_slot_inp(next,sb,cb);

/*-----------------------------------------------------------------------
    Advance to the next slot, adjust to next word if needed.
 -----------------------------------------------------------------------*/
	next += slot->slot_len + sizeof(slot_hdr);
	if (slot->slot_len & 1)
	    next++;
    }

/*-----------------------------------------------------------------------
    When incoming message processed, generate return circuit message.
    If any unacknowledged message are pending, send those again with
    an updated acknowldgement count.
 -----------------------------------------------------------------------*/
    if (LOBYTE(cb->cb_msg_seq - cb->cb_msg_ack) != 1)
	cb_retransmit(cb,FAILURE);

/*-----------------------------------------------------------------------
    If the host message should be delayed, just put us into RUN state.
    and let the tic processing generate message.
 -----------------------------------------------------------------------*/
    else if (cb->cb_hd_tics != 0)
    {
	cb->cb_state = CB_RUN;
	cb->cb_tics  = cb->cb_hd_tics;
    }

/*-----------------------------------------------------------------------
    Generate return message(s), goto next state.
 -----------------------------------------------------------------------*/
    else
    {
	cb->cb_state = CB_RUN;
	cb_run_output(cb);
    }
}
