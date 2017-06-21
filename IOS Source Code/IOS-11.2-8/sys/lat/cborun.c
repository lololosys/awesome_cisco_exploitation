/* $Id: cborun.c,v 3.2 1995/11/17 17:32:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/cborun.c,v $
 *------------------------------------------------------------------
 * $Log: cborun.c,v $
 * Revision 3.2  1995/11/17  17:32:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:22:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    cb_o_run_message	(i) generate circuit run message

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called to generate the circuit run message.

    CALL BY:	void cb_o_run_message(of,cb)
		of_ptr	of;
		cb_ptr	cb;

    ON EXIT:	Circuit run message generated in supplied output frame.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latnd.h"
#include "latsb.h"
#include "latmsg.h"
#include "latslt.h"
#include "latfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	cb_ptr	reject_cb[];		/* Reject CB table		*/
extern	byte	reject_id[];		/* Reject slot ID's		*/
extern	int	reject_index;		/* Current pending rejects	*/

void cb_o_run_message(of,cb)

    of_ptr	of;			/* Pointer to output frame	*/
    cb_ptr	cb;			/* Pointer to circuit block	*/
{
    sb_ptr	sb;			/* Pointer to session block	*/
    sb_ptr	sx;			/* Pointer to session block	*/
    nd_ptr	nd;			/* Pointer to remote node	*/
    cir_optr	cir;			/* Pointer to output message	*/
    of_byte	next;			/* Pointer to next output byte	*/
    int		max;			/* Remaining length of message	*/
    int		len;			/* Returned data length		*/
    int		i;			/* Do index			*/

/*-----------------------------------------------------------------------
    Setup the pointer we need to outging message.
 -----------------------------------------------------------------------*/
    cir = (cir_optr)of->of_data;
    nd = cb->cb_nd;

/*-----------------------------------------------------------------------
    Fill in the stardard parts of a circuit message.
 -----------------------------------------------------------------------*/
    cir->cir_type   = (byte)(LAT_RUN | cb->cb_type);
    cir->cir_slots  = 0;
    cir->cir_dst_id = swap(cb->cb_rmt_id);
    cir->cir_src_id = swap(cb->cb_lcl_id);
    cir->cir_seq    = cb->cb_msg_seq++;
    cir->cir_ack    = cb->cb_msg_rcv;

    if (cb->cb_type == CB_HOST)
	cir->cir_type |= LAT_RESPONSE;

/*-----------------------------------------------------------------------
    Get pointer to first slot and maximum space available.
 -----------------------------------------------------------------------*/
    next = of->of_data + sizeof(cir_msg);
    max  = (nd->nd_dll_max < hs->hs_dll_max) ? nd->nd_dll_max : hs->hs_dll_max;
    max -= sizeof(cir_msg);

/*-----------------------------------------------------------------------
    Check for any reject slots to send. The table is kept in
    compressed format.
 -----------------------------------------------------------------------*/
    if (reject_index != 0)
    {
	for (i = 0; i < reject_index; i++)
	{
	    if (reject_cb[i] == cb)
	    {
		cir->cir_slots++;

		max -= 4;
		*next++ = reject_id[i];
		*next++ = 0;
		*next++ = 0;
		*next++ = (byte)(SLOT_REJECT | SLOT_STOP_NO_RESOURCE);

		--reject_index;
		reject_cb[i] = reject_cb[reject_index];
		reject_id[i] = reject_id[reject_index];
		i--;
	    }
	}
    }

/*-----------------------------------------------------------------------
    Scan sessions and generate slots.
 -----------------------------------------------------------------------*/
    for (sb = cb->cb_slot[0]; sb != 0; sb = sx)
    {

/*-----------------------------------------------------------------------
    Set the next slot to examine and check flags to see of any slot
    to generate.
 -----------------------------------------------------------------------*/
	sx = sb->sb_slot[0];
	if (sb->sb_flags)
	{

/*-----------------------------------------------------------------------
    Generate the slot and update pointers, maximum size.
 -----------------------------------------------------------------------*/
	    if ((len = sb_1_slot_out(next,sb,max)) != 0)
	    {
		count_x(co_byte_xmit,(len - sizeof(slot_hdr)));
		cir->cir_slots++;

		if (len & 1)
		    len++;

		max  -= len;
		next += len;

/*-----------------------------------------------------------------------
    If the slot is to be deleted, do so now.
 -----------------------------------------------------------------------*/
		if (sb->sb_flags & (SB_DEL_STOP | SB_DEL_NONE))
		{
		    if (sb->sb_flags & SB_DEL_STOP)
			sb_delete(sb,SX_STOP);
		    else
			sb_delete(sb,0);
		}

/*-----------------------------------------------------------------------
    Otherwise, unlink this session block and link to end of the chain.
 -----------------------------------------------------------------------*/
		else
		{
		    if (sb->sb_slot[0] != 0)
			sb->sb_slot[0]->sb_slot[1] = sb->sb_slot[1];
		    else
			cb->cb_slot[1] = sb->sb_slot[1];

		    if (sb->sb_slot[1] != 0)
			sb->sb_slot[1]->sb_slot[0] = sb->sb_slot[0];
		    else
			cb->cb_slot[0] = sb->sb_slot[0];

		    sb->sb_slot[0] = 0;
		    sb->sb_slot[1] = cb->cb_slot[1];
		    cb->cb_slot[1] = sb;
		    if (sb->sb_slot[1] != 0)
			sb->sb_slot[1]->sb_slot[0] = sb;
		    else
			cb->cb_slot[0] = sb;

/*-----------------------------------------------------------------------
    If no next session (we were at list end), set ourselves as the
    next session.
 -----------------------------------------------------------------------*/
		    if (sx == 0)
			sx = sb;
		}

/*-----------------------------------------------------------------------
    Check if enough space remains to generate more slots.
 -----------------------------------------------------------------------*/
		if (max <= sizeof(slot_hdr) + 1)
		    break;
	    }
	}
    }

/*-----------------------------------------------------------------------
    Fill in the message size and destination.
 -----------------------------------------------------------------------*/
    of->of_size   = (int)(next - of->of_data);
    of->of_dst[0] = nd->nd_host_addr[0];
    of->of_dst[1] = nd->nd_host_addr[1];
    of->of_dst[2] = nd->nd_host_addr[2];
    of->of_idb    = nd->nd_idb;
}
