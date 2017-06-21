/* $Id: cbrxmt.c,v 3.2 1995/11/17 17:32:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/cbrxmt.c,v $
 *------------------------------------------------------------------
 * $Log: cbrxmt.c,v $
 * Revision 3.2  1995/11/17  17:32:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:22:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    cb_retransmit	(i) retransmit all unacknowledged messages

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called to retransmit any unacknowledged messages.
    It handles the case of a host balanced message which has already been
    discarded.  If the flag passes is SUCCESS, the message is counted
    as a retransmission, if it is FAILURE it is not counted as this message
    is being retransmitted as part of the natural workings of the LAT
    protocol and does not indicate a potential problem on the circuit.

    CALL BY:	void cb_retransmit(cb,rxmit)
		cb_ptr	cb;
		int	rxmit;

    ON EXIT:	All unacknowledged messages sent to partner.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latnd.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

void cb_retransmit(cb,rxmit)

    cb_ptr	cb;			/* Pointer to circuit block	*/
    int		rxmit;			/* Rexmit count flag		*/
{
    nd_ptr	nd;			/* Pointer to remote node	*/
    of_ptr	of;			/* Pointer to output frame	*/
    of_ptr	ox;			/* Pointer to output frame	*/
    cir_optr	cir;			/* Pointer to circuit header	*/
    byte	ack;			/* Acknowledgement number	*/

/*-----------------------------------------------------------------------
    The unacknowledged message range from ack+1 to seq-1. Send each
    message in low-to-high order.
 -----------------------------------------------------------------------*/
    nd = cb->cb_nd;

    for (of = cb->cb_xmit[0], ack = cb->cb_msg_ack+1; ack != cb->cb_msg_seq; ack++)
    {
        if (rxmit)
	    count_1(co_retransmit);

/*-----------------------------------------------------------------------
    If holding message, update our acknowledgement and retransmit.
 -----------------------------------------------------------------------*/
	if ((of != 0) && *(of->of_data+6) == ack)
	{
	    cir = (cir_optr)of->of_data;
	    cir->cir_ack = cb->cb_msg_rcv;

	    of->of_dst[0] = nd->nd_host_addr[0];
	    of->of_dst[1] = nd->nd_host_addr[1];
	    of->of_dst[2] = nd->nd_host_addr[2];
	    of->of_idb    = nd->nd_idb;
	    of_message(of);

	    of = of->of_link;
	}

/*-----------------------------------------------------------------------
    Otherwise, the missing message was a balanced message. Regenerate
    and transmit.
 -----------------------------------------------------------------------*/
	else if ((ox = of_alloc()) != 0)
	{
	    cir = (cir_optr)ox->of_data;
	    cir->cir_type   = (byte)(LAT_RUN) | cb->cb_type;
	    cir->cir_slots  = 0;
	    cir->cir_dst_id = swap(cb->cb_rmt_id);
	    cir->cir_src_id = swap(cb->cb_lcl_id);
	    cir->cir_seq    = ack;
	    cir->cir_ack    = cb->cb_msg_rcv;

	    ox->of_size = sizeof(cir_msg) | OF_DELETE;
	    ox->of_dst[0] = nd->nd_host_addr[0];
	    ox->of_dst[1] = nd->nd_host_addr[1];
	    ox->of_dst[2] = nd->nd_host_addr[2];
	    ox->of_idb    = nd->nd_idb;
	    of_message(ox);
	}
    }
}
