/* $Id: cbabo.c,v 3.2 1995/11/17 17:31:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/cbabo.c,v $
 *------------------------------------------------------------------
 * $Log: cbabo.c,v $
 * Revision 3.2  1995/11/17  17:31:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:24:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:21:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    cb_abort		(i) abort established circuit

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function aborts an established circuit. The function returns
    any output frames, generates and sends the circuit halt message and
    then deletes the circuit structure.

    CALL BY:	void cb_abort(cb)
    		cb_ptr	cb;

    ON EXIT:	The circuit halt message is sent (one time, no acknowledge
    		is expected) and all structures related to the circuit are
    		torn down.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latcb.h"
#include "latnd.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

void cb_abort(cb)

    cb_ptr	cb;			/* Pointer to CB structure	*/
 {
    of_ptr	of;			/* Pointer to output frame	*/
    cir_optr	cir;			/* Pointer to output circuit	*/
    halt_optr	halt;			/* Pointer to output halt	*/

/*-----------------------------------------------------------------------
    Free any output frame buffers the circuit may have allocated. Doing
    this now helps make sure we get a buffer for the halt message.
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

/*-----------------------------------------------------------------------
    Allocate and send the circuit halt message.
 -----------------------------------------------------------------------*/
    if ((of = of_alloc()) != 0)
    {
	cir = (cir_optr)of->of_data;

	cir->cir_type   = (byte)(LAT_HALT | cb->cb_type);
	cir->cir_slots  = 0;
	cir->cir_dst_id = swap(cb->cb_rmt_id);
	cir->cir_src_id = 0;
	cir->cir_seq    = cb->cb_msg_seq;
	cir->cir_ack    = cb->cb_msg_rcv;

	halt = (halt_optr)(of->of_data + sizeof(cir_msg));
	halt->halt_code = cb->cb_halt_reason;
	halt->halt_len  = 0;

	of->of_size   = sizeof(cir_msg) + sizeof(halt_msg);
	of->of_size  |= OF_DELETE;

	of->of_dst[0] = cb->cb_nd->nd_host_addr[0];
	of->of_dst[1] = cb->cb_nd->nd_host_addr[1];
	of->of_dst[2] = cb->cb_nd->nd_host_addr[2];
	of->of_idb    = cb->cb_nd->nd_idb;
	of_message(of);
    }

/*-----------------------------------------------------------------------
    Delete the circuit (and any other references made from the circuit).
 -----------------------------------------------------------------------*/
    cb_delete(cb);
}
