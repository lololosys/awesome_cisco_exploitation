/* $Id: cbostr.c,v 3.2 1995/11/17 17:32:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/cbostr.c,v $
 *------------------------------------------------------------------
 * $Log: cbostr.c,v $
 * Revision 3.2  1995/11/17  17:32:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:22:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    cb_o_start_message	(i) generate start circuit message

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called to generate the circuit start message.

    CALL BY:	void cb_o_start_message(of,cb)
		of_ptr	of;
		cb_ptr	cb;

    ON EXIT:	Start message generated in supplied output frame.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latnd.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	tu_msec_tic;		/* Millseconds/tic		*/

void cb_o_start_message(of,cb)

    of_ptr	of;			/* Pointer to output frame	*/
    cb_ptr	cb;			/* Pointer to circuit block	*/
{
    cir_optr	cir;			/* Pointer to output message	*/
    str_optr	str;			/* Pointer to start fields	*/
    of_byte	next;			/* Pointer to next output byte	*/

/*-----------------------------------------------------------------------
    Setup the pointer we need to outging message.
 -----------------------------------------------------------------------*/
    cir = (cir_optr)of->of_data;

/*-----------------------------------------------------------------------
    Fill in the stardard parts of a circuit message.
 -----------------------------------------------------------------------*/
    cir->cir_type   = (byte)(LAT_START | cb->cb_type);
    cir->cir_slots  = 0;
    cir->cir_dst_id = swap(cb->cb_rmt_id);
    cir->cir_src_id = swap(cb->cb_lcl_id);
    cir->cir_seq    = cb->cb_msg_seq;
    cir->cir_ack    = cb->cb_msg_rcv;
    cb->cb_msg_seq++;

/*-----------------------------------------------------------------------
    Fill in the start specific fields.
 -----------------------------------------------------------------------*/
    str = (str_optr)(of->of_data + sizeof(cir_msg));
    str->str_dll_max   = (hs->hs_inp_max != 0) ? swap(hs->hs_inp_max) : swap(hs->hs_dll_max);
    str->str_prot_ver  = hs->hs_prot_ver;
    str->str_prot_eco  = hs->hs_prot_eco;
    str->str_max_ses   = hs->hs_cb_max_ses;
    str->str_rcv_extra = (cb->cb_type == CB_HOST) ? hs->hs_cb_h_rcv_extra : hs->hs_cb_s_rcv_extra;
    str->str_vc_timer  = (cb->cb_vc_tics * tu_msec_tic) / 10;
    str->str_ka_timer  = cb->cb_ka_secs;
    str->str_facility  = swap(hs->hs_facility);
    str->str_prod_code = hs->hs_prod_code;
    str->str_prod_ver  = hs->hs_prod_ver;

    next = of->of_data + sizeof(cir_msg) + sizeof(str_msg);

    if (cb->cb_type == CB_HOST)
    {
        next = hs_str_move_os(next,&hs->hs_node[0]);
        next = nd_str_move_os(next,&cb->cb_nd->nd_node[0]);
    }
    else
    {
        next = nd_str_move_os(next,&cb->cb_nd->nd_node[0]);
        next = hs_str_move_os(next,&hs->hs_node[0]);
    }
    next = hs_str_move_os(next,&hs->hs_ident[0]);
    *next++ = 0;

/*-----------------------------------------------------------------------
    Fill in the message size and destination.
 -----------------------------------------------------------------------*/
    of->of_size   = (int)(next - of->of_data);
    of->of_dst[0] = cb->cb_nd->nd_host_addr[0];
    of->of_dst[1] = cb->cb_nd->nd_host_addr[1];
    of->of_dst[2] = cb->cb_nd->nd_host_addr[2];
    of->of_idb    = cb->cb_nd->nd_idb;
}
