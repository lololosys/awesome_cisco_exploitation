/* $Id: cbsrun.c,v 3.2 1995/11/17 17:32:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/cbsrun.c,v $
 *------------------------------------------------------------------
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cbsrun.c,v $
 * Revision 3.2  1995/11/17  17:32:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:22:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    cb_s_run_message	(i) process run message to server

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called when a run message to the server is received.

    CALL BY:	void cb_s_run_message(frame,cb,src)
		if_byte frame;
		cb_ptr	cb;
		word	src[3];

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

void cb_s_run_message(frame,cb,src,idb)
    if_byte	frame;			/* Pointer to input  message	*/
    cb_ptr	cb;			/* Pointer to circuit block	*/
    word	src[3];			/* Source address		*/
    idbtype     *idb;			/* Input interface              */
{
    of_ptr	of;			/* Pointer to output frame	*/
    sb_ptr	sb;			/* Pointer to session block	*/
    nd_ptr	nd;			/* Pointer to remote node	*/
    cir_iptr	cir;			/* Pointer to circuit header	*/
    slot_iptr	slot;			/* Pointer to slot_header	*/
    if_byte	next;			/* Pointer to next input byte	*/
    int		no_slots;		/* Number of slots in messsage	*/
    int		ses;			/* Local session number		*/
    int		duplicate;		/* Duplicate counter flag	*/

/*-----------------------------------------------------------------------
    Get a pointer to the message header.
 -----------------------------------------------------------------------*/
    cir = (cir_iptr)frame;
    nd  = cb->cb_nd;

/*-----------------------------------------------------------------------
    Count a message received in this tic interval.
 -----------------------------------------------------------------------*/
    cb->cb_msg_tic++;

/*-----------------------------------------------------------------------
    Process any acknowledgements first as it is possible to get a new
    acknowledgement in the duplicate of a previous message.  Set
    duplicate to SUCCESS, and change to FAILURE only if message should not
    be counted as a duplicate.  In other words, we will not count a
    message as a duplicate if it carries a new acknowledgement.
 -----------------------------------------------------------------------*/
    duplicate = SUCCESS;
    if ((cb->cb_msg_ack - cir->cir_ack) & 0x80)
    { 
        duplicate = FAILURE;
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

	if (cb->cb_xmit[0] == 0)
	{
	    cb->cb_state = CB_RUN;
	    cb->cb_tics -= (cb->cb_rx_tics - cb->cb_vc_tics);

	    if (cb->cb_retries != hs->hs_cb_retries || cb->cb_tics <= 0 || cb->cb_tics > cb->cb_vc_tics)
		cb->cb_tics = 1;
	}
    }

/*-----------------------------------------------------------------------
    If in starting state, ignore the message.
 -----------------------------------------------------------------------*/
    if (cb->cb_state == CB_START || cb->cb_state == CB_START_W) {
	;
    }
/*-----------------------------------------------------------------------
    Check if this is next message we expect. Count as duplicate if not
    and retransmit any frames we have after first switch the remote node
    address to the source address of the message.  Only count as
    a duplicate if it really is.
 -----------------------------------------------------------------------*/
    else if (LOBYTE(cir->cir_seq - cb->cb_msg_rcv) != 1)
    {
	nd->nd_host_addr[0] = src[0];
	nd->nd_host_addr[1] = src[1];
	nd->nd_host_addr[2] = src[2];
	nd->nd_idb          = idb;

        if (duplicate)
	    count_1(co_duplicates);
    }

/*-----------------------------------------------------------------------
    Good new message. Count it.
 -----------------------------------------------------------------------*/
    else
    {
	cb->cb_msg_rcv++;

/*-----------------------------------------------------------------------
    If remote sends balanced condition (no response) put us into idle mode.
 -----------------------------------------------------------------------*/
	if (cb->cb_state == CB_RUN && !(cir->cir_type & LAT_RESPONSE))
	    cb->cb_state = CB_IDLE;

/*-----------------------------------------------------------------------
    If circuit is currently balanced, set to run and trigger at next tic.
 -----------------------------------------------------------------------*/
	else if (cb->cb_state == CB_BALANCE)
	{
	    cb->cb_state = CB_RUN;
	    cb->cb_tics  = 1;
	}

/*-----------------------------------------------------------------------
    Loop through the slots in the message and process each.
 -----------------------------------------------------------------------*/
	next = frame + sizeof(cir_msg);
	no_slots = cir->cir_slots;

	count_1(co_msgs_recv);
	count_x(co_slot_recv,no_slots);

	while (no_slots-- != 0)
	{
	    slot = (slot_iptr)next;
	    count_x(co_byte_recv,slot->slot_len);

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
    }
}
