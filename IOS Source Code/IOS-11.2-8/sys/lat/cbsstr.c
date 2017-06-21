/* $Id: cbsstr.c,v 3.2 1995/11/17 17:32:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/cbsstr.c,v $
 *------------------------------------------------------------------
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cbsstr.c,v $
 * Revision 3.2  1995/11/17  17:32:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:22:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    cb_s_start_message	(i) process start message to server

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called when a start message to the server is received.
    If the circuit is waiting for the start (CB_STARTA), the circuit goes to
    run state. For all other states, the start message is fatal.

    CALL BY:	void cb_s_start_message(frame,src,cb)
		if_byte	frame;
    		word	src[3];
		cb_ptr	cb;

    ON EXIT:	Returns with message processed.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latcb.h"
#include "latnd.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

void cb_s_start_message(frame,cb)

    if_byte	frame;			/* Pointer to input  message	*/
    cb_ptr	cb;			/* Pointer to circuit block	*/
{
    cir_iptr	cir;			/* Pointer to circuit message	*/
    str_iptr	str;			/* Pointer to start   message	*/
    of_ptr	of;			/* Output frame pointer		*/

/*-----------------------------------------------------------------------
    If we in CB_START_W state, we can now go to run state.
 -----------------------------------------------------------------------*/
    if (cb->cb_state == CB_START_W)
    {
	cb->cb_state   = CB_RUN;
	cb->cb_tics    = 1;
	cb->cb_msg_rcv = 0;
    	cb->cb_msg_ack = 0;

/*-----------------------------------------------------------------------
    Set the remote system ID and use the message size that the remote
    node indicated it will accomodate.
 -----------------------------------------------------------------------*/
	cir = (cir_iptr)frame;
	str = (str_iptr)(frame + sizeof(cir_msg));
	cb->cb_rmt_id = swap(cir->cir_src_id);

	cb->cb_nd->nd_dll_max   = swap(str->str_dll_max) - 18;

#if LE_ND_STATS
	cb->cb_nd->nd_prot_ver  = str->str_prot_ver;
	cb->cb_nd->nd_prot_eco  = str->str_prot_eco;
	cb->cb_nd->nd_facility  = swap(str->str_facility);
	cb->cb_nd->nd_prod_code = str->str_prod_code;
	cb->cb_nd->nd_prod_ver  = str->str_prod_ver;
#endif

	if (str->str_max_ses < cb->cb_max_ses)
	  cb->cb_max_ses = str->str_max_ses;
	cb->cb_extra   = str->str_rcv_extra;

/*-----------------------------------------------------------------------
    Deallocate the start message we sent.
 -----------------------------------------------------------------------*/
	while ((of = cb->cb_xmit[0]) != 0)
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
