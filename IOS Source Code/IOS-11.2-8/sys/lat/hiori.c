/* $Id: hiori.c,v 3.2 1995/11/17 17:32:53 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hiori.c,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: hiori.c,v $
 * Revision 3.2  1995/11/17  17:32:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:23:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hi_o_ri_message	(i) output response message

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called to output a response message.

    CALL BY:	void hi_o_ri_message(of,hi)
		of_ptr	of;
		hi_ptr	hi;

    ON EXIT:	Appropriate message sent to remote node.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latas.h"
#include "lathi.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	byte	as_temp[];		/* Temporary AS name		*/

void hi_o_ri_message(of,hi)

    of_ptr	of;			/* Output frame pointer		*/
    hi_ptr	hi;			/* HIC request pointer		*/
{
    ri_optr	ri;			/* Output response message	*/
    of_byte	next;			/* Output byte pointer		*/
    of_byte	svc;			/* Count of service entries	*/
    int		i;
/*-----------------------------------------------------------------------
    Fill in the fixed fields of a solicit host-initiated message.
 -----------------------------------------------------------------------*/
    ri = (ri_optr)of->of_data;
    next = of->of_data + sizeof(ri_msg);
    of->of_idb = hi->hi_idb;

    ri->ri_type         = hi->hi_function;
    ri->ri_format       = RI_ETHER;
    ri->ri_prot_hi      = hs->hs_prot_hi;
    ri->ri_prot_lo      = hs->hs_prot_lo;
    ri->ri_prot_ver     = hs->hs_prot_ver;
    ri->ri_prot_eco     = hs->hs_prot_eco;
    ri->ri_dll_max      = (hs->hs_inp_max != 0) ? swap(hs->hs_inp_max) : swap(hs->hs_dll_max);
    ri->ri_req_no       = swap(hi->hi_req_no);
    ri->ri_req_status   = 0;
    ri->ri_node_status  = swap(LOBYTE(hs->hs_status));

    ri->ri_node_addr[0] = ((ushort *)(hi->hi_idb->hwptr->hardware))[0];
    ri->ri_node_addr[1] = ((ushort *)(hi->hi_idb->hwptr->hardware))[1];
    ri->ri_node_addr[2] = ((ushort *)(hi->hi_idb->hwptr->hardware))[2];

    if (hi->hi_idb->hwptr->status & IDB_TR)
      for (i = 0; i < IEEEBYTES; i++)
	((uchar *)ri->ri_node_addr)[i] = 
	  bit_swaps[((uchar *)ri->ri_node_addr)[i]];

    ri->ri_mc_timer     = swap(((word)hs->hs_as_timer));

/*-----------------------------------------------------------------------
    Complete the variable fields in a response message.
 -----------------------------------------------------------------------*/
    next = hi_str_move_os(next,&hi->hi_node[0]);
    next = hs_str_move_os(next,&hs->hs_acl_group[0]);
    next = hs_str_move_os(next,&hs->hs_node[0]);
    next = hs_str_move_os(next,&hs->hs_ident[0]);

/*-----------------------------------------------------------------------
    Set count of service entries.
 -----------------------------------------------------------------------*/
    svc  = next++;
    *svc = 0;

/*-----------------------------------------------------------------------
    If solicit message named a specific service, look up the name and
    fill the entry. If service not found, set service unavailabe.
 -----------------------------------------------------------------------*/
    if (hi->hi_service[0] != 0)
    {
#if LE_HOST_AS
	as_ptr as;
	int    max;
	
	hi_str_move_ls(&as_temp[0],&hi->hi_service[0]);
	max = (hi->hi_dll_max < hs->hs_dll_max) ? hi->hi_dll_max : hs->hs_dll_max;
	
	if ((as = as_show(&as_temp[0])) != 0)
	{
	    if (max - (next - of->of_data) > as->as_service[0] + as->as_ident[0] + 8)
	    {
	        *svc += 1;
	        *next++ = as->as_service[0] + as->as_ident[0] + 7;
	        *next++ = 0;
	        *next++ = 0;

		if (as->as_flags & AS_ENABLED)
		    *(next-1) |= RI_ENABLED;
		if (as->as_flags & AS_QUEUED)
		    *(next-1) |= RI_QUEUE;

	        *next++ = as->as_rating;
	        *next++ = 0;
	        next = as_str_move_os(next,&as->as_service[0]);
	        next = as_str_move_os(next,&as->as_ident[0]);
	    }
	}
#endif
	if (*svc == 0)
	    ri->ri_req_status |= swap(RI_NO_SERVICE);
    }

#if LE_HOST_AS
/*-----------------------------------------------------------------------
    Otherwise, copy as many service names as will fit into the message.
 -----------------------------------------------------------------------*/
    else
    {
    	as_ptr	as;
	int    max;
	
	max = (hi->hi_dll_max < hs->hs_dll_max) ? hi->hi_dll_max : hs->hs_dll_max;

        for (as = hs->hs_as_head[0]; as != 0; as = as->as_link[0])
	{
	    if (max - (next - of->of_data) > as->as_service[0] + as->as_ident[0] + 8)
	    {
	        *svc += 1;
	        *next++ = as->as_service[0] + as->as_ident[0] + 7;
	        *next++ = 0;
	        *next++ = 0;

		if (as->as_flags & AS_ENABLED)
		    *(next-1) |= RI_ENABLED;
		if (as->as_flags & AS_QUEUED)
		    *(next-1) |= RI_QUEUE;

	        *next++ = as->as_rating;
	        *next++ = 0;
	        next = as_str_move_os(next,&as->as_service[0]);
	        next = as_str_move_os(next,&as->as_ident[0]);
	    }
	}
    }
#endif

/*-----------------------------------------------------------------------
    Set no parameters and compute length of message.
 -----------------------------------------------------------------------*/
    *next++ = 0;
    of->of_size = (int)(next - of->of_data);

/*-----------------------------------------------------------------------
    Delete entry after transmission.
 -----------------------------------------------------------------------*/
    hi->hi_flags |= HI_DELETE;
}
