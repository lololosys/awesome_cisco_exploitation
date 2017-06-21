/* $Id: cbcre.c,v 3.2 1995/11/17 17:31:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/cbcre.c,v $
 *------------------------------------------------------------------
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cbcre.c,v $
 * Revision 3.2  1995/11/17  17:31:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:24:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:22:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    cb_create		(i) create circuit structure

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function creates an circuit structure. The caller is responsible
    for initializing to proper initial state.

    CALL BY:	cb_ptr cb_create(nd,type)
    		nd_ptr	nd;
    		int	type;

    ON EXIT:	Returns the address of the created circuit stricture. If
		the circuit is not created, the function returns zero and
		sets lat_error.

	CB_ERROR | MAX_LIMIT	new circuit exceeds the maximum circuits
	CB_ERROR | NO_MEM	circuit structure could not be allocated
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latnd.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern  hs_ptr  hs;			/* Local node definitions	*/
extern	int	tu_msec_tic;		/* Milliseconds/tic		*/
extern	int	lat_error;		/* LAT error code		*/

cb_ptr cb_create(nd)

    nd_ptr	nd;			/* Pointer to ND structure	*/
 {
    cb_ptr	cb;			/* Pointer to new CB structure	*/
    int		i, j;			/* Do indexes			*/

/*-----------------------------------------------------------------------
    Preset error return.
 -----------------------------------------------------------------------*/
    cb = 0;

/*-----------------------------------------------------------------------
    Find a free slot in the circuit block table for new circuit. Extend
    the table index to the limit if necessary.
 -----------------------------------------------------------------------*/
    for (i = 1; i < hs->hs_cb_index; i++)
    {
	if (hs->hs_cb_table[i] == 0)
	    break;
    }

    if (i == hs->hs_cb_index && i < CB_TABLES)
	hs->hs_cb_index++;

/*-----------------------------------------------------------------------
    Check if there is space in circuit table or memory can be allocated.
 -----------------------------------------------------------------------*/
    if (hs->hs_cb_maximum > 0 && hs->hs_cb_current >= hs->hs_cb_maximum)
        lat_error = CB_ERROR | MAX_LIMIT;

    else if (i == hs->hs_cb_index)
	lat_error = CB_ERROR | MAX_LIMIT;

    else if ((cb = cb_alloc()) == 0)
	lat_error = CB_ERROR | NO_MEM;

/*-----------------------------------------------------------------------
    Initialize the newly allocated circuit block.
 -----------------------------------------------------------------------*/
    else
    {
	count_h(ch_total_circuits);

	if (++hs->hs_cb_current > hs->hs_cb_highest)
	    hs->hs_cb_highest++;

	hs->hs_cb_table[i] = cb;
	if (++hs->hs_cb_seqno[i] == 0)
	    hs->hs_cb_seqno[i] = 1;

	for (j = 0; j < SB_HASHES; j++)
	{
	    cb->cb_sb[j][0] = 0;
	    cb->cb_sb[j][1] = 0;
	}

	cb->cb_slot[0] = 0;
	cb->cb_slot[1] = 0;
	cb->cb_nd      = nd;
	cb->cb_xmit[0] = 0;
	cb->cb_xmit[1] = (of_ptr)&cb->cb_xmit[0];
	nd->nd_usage++;

	cb->cb_tics    = 0;

	cb->cb_rmt_id  = 0;
	cb->cb_lcl_id  = (word)((hs->hs_cb_seqno[i]<<8) + i);

	cb->cb_type        = (byte)-1;
	cb->cb_state       = (byte)-1;
	cb->cb_session     = 0;
	cb->cb_max_ses     = hs->hs_cb_max_ses;
	cb->cb_vc_tics     = (hs->hs_cb_vc_timer + tu_msec_tic - 1) / tu_msec_tic;
	cb->cb_rx_tics     = cb->cb_vc_tics * CB_R1_UNIT;
    	cb->cb_hd_tics	   = 0;
	cb->cb_ka_secs     = hs->hs_cb_ka_timer;
	cb->cb_tics_sec    = (byte)((1000 + tu_msec_tic - 1) / tu_msec_tic);
	cb->cb_retries     = 0;
	cb->cb_halt_reason = 0;
	cb->cb_msg_seq     = 0;
	cb->cb_msg_rcv     = 0xff;
	cb->cb_msg_ack     = 0xff;
	cb->cb_msg_tic     = 0;
	cb->cb_extra       = 0;
	cb->cb_ses_count   = 0;
    }
    return(cb);
}
