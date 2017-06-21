/* $Id: cbhalt.c,v 3.2 1995/11/17 17:32:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/cbhalt.c,v $
 *------------------------------------------------------------------
 * $Log: cbhalt.c,v $
 * Revision 3.2  1995/11/17  17:32:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:22:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    cb_halt		(i) sent halt for bad message.

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function sends a halt message to the sender of a bad message.
    We use the message to get the necessary fields.

    CALL BY:	void cb_halt(frame,src,reason)
    		if_byte	frame;
    		word	src[3];
    		int	reason;

    ON EXIT:	The circuit halt message is sent (one time, no acknowledge
    		is expected).
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

void cb_halt(frame,src,reason,idb)
    if_byte	frame;			/* Pointer to bad message	*/
    word	src[3];			/* Message source address	*/
    int		reason;			/* Halt reason			*/
    idbtype	*idb;			/* Input interface		*/
 {
    of_ptr	of;			/* Pointer to output frame	*/
    cir_optr	cir;			/* Pointer to output circuit	*/
    cir_iptr	inp;			/* Pointer to input  circuit	*/
    halt_optr	halt;			/* Pointer to output halt	*/

/*-----------------------------------------------------------------------
    Allocate and send the circuit halt message.
 -----------------------------------------------------------------------*/
    if ((of = of_alloc()) != 0)
    {
	cir = (cir_optr)of->of_data;
	inp = (cir_iptr)frame;

	if (inp->cir_type & LAT_MASTER)
	    cir->cir_type   = (byte)LAT_HALT;
	else
	    cir->cir_type   = (byte)(LAT_HALT | LAT_MASTER);

	cir->cir_slots  = 0;
	cir->cir_dst_id = inp->cir_src_id;
	cir->cir_src_id = 0;
	cir->cir_seq    = inp->cir_ack + 1;
	cir->cir_ack    = inp->cir_seq;

	halt = (halt_optr)(of->of_data + sizeof(cir_msg));
	halt->halt_code = (byte)(LOBYTE(reason));
	halt->halt_len  = 0;

	of->of_size   = sizeof(cir_msg) + sizeof(halt_msg);
	of->of_size  |= OF_DELETE;

	of->of_dst[0] = src[0];
	of->of_dst[1] = src[1];
	of->of_dst[2] = src[2];
	of->of_idb    = idb;
	of_message(of);
    }
}
