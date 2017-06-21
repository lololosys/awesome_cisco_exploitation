/* $Id: cbhstr.c,v 3.2 1995/11/17 17:32:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/cbhstr.c,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cbhstr.c,v $
 * Revision 3.2  1995/11/17  17:32:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:22:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    cb_h_start_message	(i) process start message to host

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called when a start message to the host is received.

    CALL BY:	void cb_h_start_message(frame,src)
		if_byte	frame;
    		word	src[3];

    ON EXIT:	Returns when the circuit is started.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latnd.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"
#include "lat.h"
#include "../ui/debug.h"
#include "lat_debug.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	byte	id_temp[];		/* ID temporary storage		*/
extern	byte	nd_temp[];		/* ND temporary storage		*/
extern	int	tu_msec_tic;		/* Milliseconds/tic		*/

void cb_h_start_message(frame,src,idb)
    if_byte	frame;			/* Pointer to input  message	*/
    word	src[3];			/* Source address of message	*/
    idbtype	*idb;			/* Input interface              */
{
    cb_ptr	cb;			/* Pointer to circuit block	*/
    nd_ptr	nd;			/* Pointer to node structure	*/
    of_ptr	of;			/* Output frame buffer		*/
    cir_iptr	cir;			/* Pointer to input fields	*/
    str_iptr	str;			/* Pointer to start fields	*/
    if_byte	next;			/* Input frame byte pointer	*/
    int		i;			/* Do index			*/

/*-----------------------------------------------------------------------
    Setup the pointers we need to incoming message.
 -----------------------------------------------------------------------*/
    cir = (cir_iptr)frame;
    str = (str_iptr)(frame + sizeof(cir_msg));
    next  = frame + sizeof(cir_msg) + sizeof(str_msg);

/*-----------------------------------------------------------------------
    Get the master node name from the message.
 -----------------------------------------------------------------------*/
    next += *next + 1;
    str_input(&nd_temp[0],next,ND_STR,1);

/*-----------------------------------------------------------------------
    Check to see if we have started this circuit already.
 -----------------------------------------------------------------------*/
    for (i = 1; i < hs->hs_cb_index; i++)
    {
	if ((cb = hs->hs_cb_table[i]) != 0)
	{
	    if (cb->cb_type == CB_HOST                &&
		cb->cb_nd->nd_host_addr[0] == src[0]  &&
		cb->cb_nd->nd_host_addr[1] == src[1]  &&
		cb->cb_nd->nd_host_addr[2] == src[2]  &&
    		nd_str_equal_sl(&cb->cb_nd->nd_node[0],&nd_temp[0]) &&
		cb->cb_nd->nd_idb == idb)
	    {

/*-----------------------------------------------------------------------
    We have. If we have the same circuit_ID, retransmit our start.
    Otherwise abort the circuit and start a new circuit.
 -----------------------------------------------------------------------*/
		if (swap(cir->cir_src_id) == cb->cb_rmt_id)
		{
		    i = 0;
		    if (cb->cb_xmit[0] != 0 && cb->cb_msg_seq == 1)
			of_message(cb->cb_xmit[0]);
		}
		else
		    cb_abort(cb);
		break;
	    }
	}
    }

/*-----------------------------------------------------------------------
    Allocate an output frame to return either START or HALT message. If
    this fails, no sense in going any further.
 -----------------------------------------------------------------------*/
    if (i != 0 && (of = of_alloc()) != 0)
    {
	cb = 0;

/*-----------------------------------------------------------------------
    Create the initial circuit block (and node block if needed) and
    send back start message.
 -----------------------------------------------------------------------*/
	if ((nd = nd_create(&nd_temp[0],src,idb)) != 0)
	{
#if LE_ND_STATS
	    nd->nd_prot_ver  = str->str_prot_ver;
	    nd->nd_prot_eco  = str->str_prot_eco;
	    nd->nd_facility  = swap(str->str_facility);
	    nd->nd_prod_code = str->str_prod_code;
	    nd->nd_prod_ver  = str->str_prod_ver;
#endif
	    if ((cb = cb_create(nd)) != 0)
	    {
		cb->cb_type    = CB_HOST;
		cb->cb_rmt_id  = swap(cir->cir_src_id);
		cb->cb_msg_rcv = 0;

		next += *next + 1;
		str_input(&id_temp[0],next,ID_STR,0);
		nd_str_move_sl(&nd->nd_ident[0],&id_temp[0]);

		nd->nd_dll_max = swap(str->str_dll_max) - 18;

	        if (cb->cb_max_ses > str->str_max_ses)
		    cb->cb_max_ses = str->str_max_ses;

    		cb->cb_extra   = str->str_rcv_extra;
		cb->cb_ka_secs = str->str_ka_timer;
		cb->cb_vc_tics = ((str->str_vc_timer  * 10) + tu_msec_tic - 1) / tu_msec_tic;

                /*
                 * If the host-delay value was not entered by the user,
                 *   Calculate the host delay based the minimum of 
                 *   the LAT Master VC-timer/2 and cb->cb_hd_tics
                 * Else
                 *   Calculate the host delay based on user specified value 
                 */
		if (lat_host_delay == 0){
                   cb->cb_hd_tics = (hs->hs_cb_hd_timer + tu_msec_tic - 1) / tu_msec_tic; 
                   if (cb->cb_hd_tics > (cb->cb_vc_tics / 2))
		    cb->cb_hd_tics = (cb->cb_vc_tics / 2);
		} else
                    cb->cb_hd_tics = (lat_host_delay + tu_msec_tic - 1) / tu_msec_tic; 
                if (lat_debug)
                   buginf("\nLAT: Host delay = %d tics", cb->cb_hd_tics);

		cb_o_start_message(of,cb);

		cb->cb_xmit[0] = of;
    		cb->cb_xmit[1] = of;
		of->of_link    = 0;
		of_message(of);

		cb->cb_state   = CB_RUN_W;
		cb->cb_tics    = cb->cb_tics_sec * hs->hs_cb_progress;
		cb->cb_retries = 0;
	    }
	    else
		nd_fail(nd,0);
	}

/*-----------------------------------------------------------------------
    We lack resouces to create the circuit, return a HALT message.
 -----------------------------------------------------------------------*/
	if (cb == 0)
	{
	    of_free(of);
	    cb_halt(frame,src,HALTC_NO_RESOURCE,idb);
	}
    }
}
