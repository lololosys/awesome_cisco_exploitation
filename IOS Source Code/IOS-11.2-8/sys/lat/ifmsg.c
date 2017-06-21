/* $Id: ifmsg.c,v 3.2.62.1 1996/03/18 20:39:15 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lat/ifmsg.c,v $
 *------------------------------------------------------------------
 * $Log: ifmsg.c,v $
 * Revision 3.2.62.1  1996/03/18  20:39:15  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/02/20  14:39:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:33:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:24:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    if_message		(e) process input frame

    Copyright (c) 1989-1996 by cisco Systems, Inc.
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called by the LAT Ethernet services when an input
    LAT Ethernet frame is received. The function type is used to call
    the appropriate processing routine. For circuit messages, the circuit
    block (cb) is found using the circuit hash tables.

    CALL BY:	void if_message(frame,src,dst)
		if_byte	frame;
    		word	src[3];
    		word	dst[3];

    ON EXIT:	Input frame is completely processed.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latmsg.h"
#include "latfnc.h"

extern hs_ptr	hs;			/* Local node definitions	*/

void if_message(frame,src,dst,idb)
    if_byte	frame;			/* Pointer to input frame	*/
    word	src[3];			/* Source address of message	*/
    word	dst[3];			/* Destination address		*/
    idbtype	*idb;			/* Input interface		*/
{
    cb_ptr	cb;			/* Pointer to circuit block	*/
    cir_iptr	cir;			/* Pointer to circuit message	*/
    halt_iptr	halt;			/* Pointer to halt message	*/
    int		type;			/* Message type 		*/
    int		cid;			/* Circuit identification	*/

/*-----------------------------------------------------------------------
    Get the message type and direction. The variable host is set zero
    for message from host->server, non-zero otherwise.
 -----------------------------------------------------------------------*/
    cir  = (cir_iptr)frame;
    type = (int)cir->cir_type;

/*-----------------------------------------------------------------------
    Process run messages. Check for a valid matching circuit and if none,
    return a halt circuit to the sender.
 -----------------------------------------------------------------------*/
    if ((type & LAT_TYPE) == LAT_RUN)
    {
	cid = LOBYTE((swap(cir->cir_dst_id)));

	if ( (cid > hs->hs_cb_index)                    ||
	     ((cb = hs->hs_cb_table[cid]) == 0)         ||
	     ((swap(cir->cir_dst_id)) != cb->cb_lcl_id) ||
	     ((swap(cir->cir_src_id)) != cb->cb_rmt_id) ||
	     ((cir->cir_type & LAT_MASTER) == cb->cb_type))
	{
	    cb_halt(frame,src,HALTC_BAD_MSG,idb);
	}

#if LE_HOST
	else if (!cb->cb_type)
	    cb_h_run_message(frame,cb);
#endif

#if LE_SERVER
	else if (cb->cb_type)
	    cb_s_run_message(frame,cb,src,idb);
#endif
    }

#if LE_HOST
/*-----------------------------------------------------------------------
    Process start message to host.
 -----------------------------------------------------------------------*/
    else if (type == (LAT_START | LAT_MASTER))
	cb_h_start_message(frame,src,idb);
#endif

#if LE_SERVER
/*-----------------------------------------------------------------------
    Process start message to server. If no valid matching circuit block,
    return a halt message to the sender.
 -----------------------------------------------------------------------*/
    else if (type == LAT_START)
    {
	cid = LOBYTE((swap(cir->cir_dst_id)));

	if ( (cid > hs->hs_cb_index)                    ||
	     ((cb = hs->hs_cb_table[cid]) == 0)         ||
	     ((swap(cir->cir_dst_id)) != cb->cb_lcl_id) ||
	     ((cir->cir_type & LAT_MASTER) == cb->cb_type))
	{
	    cb_halt(frame,src,HALTC_BAD_MSG,idb);
	}
	else
	    cb_s_start_message(frame,cb);
    }
#endif

/*-----------------------------------------------------------------------
    Process halt messages. These message are legal in all states. The
    processing is the same for both servers and hosts. We get the halt
    reason and dump the circuit. We ignore any bad halt messages.
 -----------------------------------------------------------------------*/
    else if ((type & LAT_TYPE) == LAT_HALT)
    {
	cid = LOBYTE((swap(cir->cir_dst_id)));

	if ( (cid > hs->hs_cb_index)                    ||
	     ((cb = hs->hs_cb_table[cid]) == 0)         ||
	     ((swap(cir->cir_dst_id)) != cb->cb_lcl_id) ||
	     ((cir->cir_type & LAT_MASTER) == cb->cb_type))
	{
	    ;
	}

	else
	{
	    halt = (halt_iptr)(frame + sizeof(cir_msg));
	    cb->cb_halt_reason = halt->halt_code;
	    cb_delete(cb);
	}
    }

#if LE_SERVER_LS
/*-----------------------------------------------------------------------
    Dispatch if directory service message.
 -----------------------------------------------------------------------*/
    else if (type == LAT_SERVICE)
	ls_message(frame,src,idb);
#endif

#if LE_SUBJECT_HIC
/*-----------------------------------------------------------------------
    Dispatch if host-initiate message to subject.
 -----------------------------------------------------------------------*/
    else if (type == LAT_INFO)
	hi_h_ri_message(frame,src,idb);

    else if (type == LAT_STATUS)
	hi_h_sts_message(frame);
#endif

#if LE_OBJECT_HIC
/*-----------------------------------------------------------------------
    Dispatch if host-initiate message to object.
 -----------------------------------------------------------------------*/
    else if (type == LAT_SOLICIT)
	hi_s_si_message(frame,src,dst,idb);

    else if (type == LAT_COMMAND)
	hi_s_cmd_message(frame,src,idb);
#endif

}
