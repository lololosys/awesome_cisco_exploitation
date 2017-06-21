/*-----------------------------------------------------------------------
    cb_run_output	(i) output run message(s)

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called when run messages should be generated.

    CALL BY:	void cb_run_output(cb)
		cb_ptr	cb;

    ON EXIT:	Returns when the message output.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latnd.h"
#include "latmsg.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

void cb_run_output(cb)

    cb_ptr	cb;			/* Pointer to circuit block	*/
{
    of_ptr	of;			/* Pointer to output frame	*/
    nd_ptr	nd;			/* Pointer to remote node	*/
    int		no_slots;		/* Number of slots in message	*/
    int		flag;			/* Extra receive enabled	*/
    int		max;			/* Maximum message size		*/
    int		i;			/* Do index			*/

/*-----------------------------------------------------------------------
    Get a pointer to the message header.
 -----------------------------------------------------------------------*/
    nd  = cb->cb_nd;

/*-----------------------------------------------------------------------
    If no buffer can be allocated, try again in next tic.
 -----------------------------------------------------------------------*/
    if ((of = of_alloc()) == 0)
	cb->cb_tics  = 1;

/*-----------------------------------------------------------------------
    Generate run message. If slots actually generate, set the response
    required mask.  Otherwise send empty message and put ourselves into
    balanced. Note, in this case we do not save the message.

    The LAT specification states we run a retransmission timer in
    a host implementation if the host is sending an unsolicited message
    (ie we got here from sb_signal - circuit balanced and host wants to 
    send an unsolicited message).  The host retransmit timer runs at
    the same interval as the server retransmit timer - approximately 
    every 1 - 2 seconds. 

    We also start progress timer with a long delay so the host can
    detect a down server.
 -----------------------------------------------------------------------*/
    else
    {
	cb_o_run_message(of,cb);
	no_slots = *(of->of_data+1);

/*-----------------------------------------------------------------------
    If no slots, we are potentially balanced so we must handle speical.
 -----------------------------------------------------------------------*/
	if (no_slots == 0)
	{

/*-----------------------------------------------------------------------
    If idle state, a server response can be discarded. The server goes
    balances and waits the keepalive interval.
 -----------------------------------------------------------------------*/
	    if (cb->cb_state == CB_IDLE)
	    {
		of_free(of);
		cb->cb_msg_seq--;

		cb->cb_state = CB_BALANCE;
		cb->cb_tics  = cb->cb_tics_sec * cb->cb_ka_secs;
	    }

/*-----------------------------------------------------------------------
    If host trying to go from balanced to run state, don't send another
    balanced message, just go back to balanced state.
 -----------------------------------------------------------------------*/
	    else if (cb->cb_state == CB_BALANCE_R)
	    {
		of_free(of);
		cb->cb_msg_seq--;
		
		cb->cb_state = CB_BALANCE;
	    }

/*-----------------------------------------------------------------------
    Otherwise, we will output the message.
 -----------------------------------------------------------------------*/
	    else
	    {

/*-----------------------------------------------------------------------
    If host, we send the message with RRF off and flagged for delete.
    The host goes to balance state.
 -----------------------------------------------------------------------*/
		if (cb->cb_type == CB_HOST)
		{
		    *of->of_data &= (~LAT_RESPONSE);
		    of->of_size  |= OF_DELETE;

		    cb->cb_state   = CB_BALANCE;
		    cb->cb_retries = 0;
		    
		    if (hs->hs_cb_progress == 0)
			cb->cb_tics = 0;
		    else
			cb->cb_tics = cb->cb_tics_sec * (cb->cb_ka_secs + hs->hs_cb_progress);
		}

/*-----------------------------------------------------------------------
    If server, we send as a normal message.
 -----------------------------------------------------------------------*/
		else
		{
		    if (cb->cb_xmit[0] == 0)
			cb->cb_xmit[0] = of;
		    else
			cb->cb_xmit[1]->of_link = of;

		    cb->cb_xmit[1] = of;
		    of->of_link    = 0;

		    cb->cb_state   = CB_RUN_W;
		    cb->cb_tics    = cb->cb_rx_tics;
		    cb->cb_retries = hs->hs_cb_retries;
		}

/*-----------------------------------------------------------------------
    Count message and transmit.
 -----------------------------------------------------------------------*/
		count_1(co_msgs_xmit);
		of_message(of);
	    }
	}

/*-----------------------------------------------------------------------
    Otherwise, send the message(s) and go to run_wait.
 -----------------------------------------------------------------------*/
	else
	{
	    count_1(co_msgs_xmit);
	    count_x(co_slot_xmit,no_slots);

	    if (cb->cb_xmit[0] == 0)
		cb->cb_xmit[0] = of;
	    else
		cb->cb_xmit[1]->of_link = of;

	    cb->cb_xmit[1] = of;
	    of->of_link    = 0;

	    if (cb->cb_type == CB_HOST && cb->cb_state == CB_RUN)
		flag = FAILURE;
	    else
		flag = SUCCESS;

	 
	    if ((cb->cb_type == CB_SERVER) || (cb->cb_type == CB_HOST && cb->cb_state == CB_BALANCE_R))
	    {
		cb->cb_tics    = cb->cb_rx_tics;
	  	cb->cb_retries = hs->hs_cb_retries;
	    }
	    else
	    {
		cb->cb_tics    = cb->cb_tics_sec * hs->hs_cb_progress;
		cb->cb_retries = 0;
	    }

	    cb->cb_state   = CB_RUN_W;

/*-----------------------------------------------------------------------
    If no extra message, output message now. Otherwise, if last message
    was full, send additional messages.
 -----------------------------------------------------------------------*/
	    if (flag || cb->cb_extra == 0)
		of_message(of);

	    else
	    {
		max  = (nd->nd_dll_max < hs->hs_dll_max) ? nd->nd_dll_max : hs->hs_dll_max;
		max -= sizeof(slot_hdr) + 1;

		for (i = 0; i < cb->cb_extra; i++)
		{
		    if ((of->of_size & OF_LENGTH) < max)
			break;

		    if ((of = of_alloc()) == 0)
			break;

		    cb_o_run_message(of,cb);
		    no_slots = *(of->of_data+1);

		    if (no_slots == 0)
		    {
			--cb->cb_msg_seq;
			of_free(of);
		    }

		    else
		    {
			count_1(co_msgs_xmit);
			count_x(co_slot_xmit,no_slots);

		        if (cb->cb_xmit[0] == 0)
			    cb->cb_xmit[0] = of;
		        else
			    cb->cb_xmit[1]->of_link = of;

			cb->cb_xmit[1] = of;
			of->of_link    = 0;
		    }
		}

/*-----------------------------------------------------------------------
    Send all message we can, leave RRF set only in last message.
 -----------------------------------------------------------------------*/
		for (of = cb->cb_xmit[0]; of != 0; of = of->of_link)
		{
		    if (of != cb->cb_xmit[1])
			*of->of_data &= (~LAT_RESPONSE);
		    
		    of_message(of);
		}
	    }
	}
    }
}
