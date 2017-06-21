/* $Id: hihri.c,v 3.2 1995/11/17 17:32:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hihri.c,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: hihri.c,v $
 * Revision 3.2  1995/11/17  17:32:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:23:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hi_h_ri_message	(i) process response message

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is  used on subject systems to process a HIC response
    message. If the object node does not offer the service we solicitied
    or has connections disabled, notify the environment that a reject
    occured.  If this is a multicast solicit save the response offering
    the highest rated service.  The 'best' response will be used by
    'hi_h_ri_action' to initiate the connection.

    CALL BY:	void hi_h_ri_message(frame,src)
		if_byte	frame;
		word	src[3];

    ON EXIT:	The matching HIC entry processed.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "lathi.h"
#include "latmsg.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	byte	nd_temp[];		/* Local node name storage	*/
extern	byte	as_temp[];		/* Local service name storage	*/

void hi_h_ri_message(frame,src,idb)

    if_byte	frame;			/* Pointer to input  message	*/
    word	src[3];			/* Source address of message	*/
    idbtype	*idb;			/* Input interface */
{
    hi_ptr	hi;			/* HIC request structure	*/
    ri_iptr	ri;			/* Input  host message		*/
    if_byte	next;			/* Next input byte		*/
    word	sts;			/* Remote node status		*/
    int		i;			/* Loop index			*/
    byte	svc_count;		/* Respose msg. service count	*/
    byte	svc_match;		/* Service name match flag	*/
    byte	svc_stat;		/* Service status		*/
    byte	svc_rating = 0;		/* Service rating		*/
    if_byte	svc_ptr;		/* Pointer to service name	*/
    if_byte	node_ptr;		/* Pointer to node name		*/
    byte	resp_source;		/* Response from node or agent	*/

    ri   = (ri_iptr)frame;
    next = frame + sizeof(ri_msg);

/*-----------------------------------------------------------------------
    All we care about is the request number. Match the request against
    our current list.
 -----------------------------------------------------------------------*/
    if ((hi = hi_find(swap(ri->ri_req_no),0,HI_SUBJECT,0)) != 0)
    {
	sts = swap(ri->ri_node_status);
	next += (*next) + 1;
	next += (*next) + 1;
 	node_ptr = next;

/*-----------------------------------------------------------------------
    If request specified a service name, check to see that the name is
    in the response message.  Some terminal servers do respond with a
    a list of all the services they offer, even if they don't offer the
    one the we need.  Also check that the service is enabled.
 -----------------------------------------------------------------------*/
	if (!(*hi->hi_service))
	    svc_match = SUCCESS;
	else
	{
	    svc_match = FAILURE;
	    next += (*next) + 1;
	    next += (*next) + 1;
	    if ((svc_count = (*next++)) == 0) {
		;
	    } else {
		for (i = 0; i < svc_count; i++)
		{
		    svc_ptr = next + 1;
		    svc_ptr += (*svc_ptr) + 3;
		    svc_rating = *(svc_ptr - 1);
		    svc_stat = *(svc_ptr - 2);
		    svc_ptr += (*svc_ptr) + 1;
		    str_input(&as_temp[0],svc_ptr,SV_STR,1);
		    if ((svc_stat & 1) && hi_str_equal_sl(&as_temp[0],&hi->hi_service[0]))
		    {
			svc_match = SUCCESS;
			break;
		    }
		    next += (*next) + 1;
		}
	    }
	}
		
/*-----------------------------------------------------------------------
    If our request is for general queue status, just use response to
    set ethernet address.
 -----------------------------------------------------------------------*/
        if (hi->hi_command == CMD_STS_QUEUE || hi->hi_command == CMD_STS_ALL)
	{
	    hi->hi_function = LAT_COMMAND;
	    hi->hi_tics     = 1;
	    hi->hi_retries  = 1;

	    hi->hi_dest_addr[0]  = ri->ri_node_addr[0];
	    hi->hi_dest_addr[1]  = ri->ri_node_addr[1];
	    hi->hi_dest_addr[2]  = ri->ri_node_addr[2];

	    hi->hi_flags |= HI_DELETE;
	    hi->hi_req_no = 0;
	}

/*-----------------------------------------------------------------------
    If remote node does not offer the service, ignore the message. If
    a specific remote node was named, set service unknown and declare
    a rejected error.
 -----------------------------------------------------------------------*/
        else if ((swap(ri->ri_req_status) & RI_NO_SERVICE) ||
		svc_match == FAILURE)
	{
	    if (hi->hi_node[0] != 0)
	    {
	        hi->hi_status = STS_ERROR;
	        hi->hi_error  = SLOT_STOP_SRV_UNKNOWN;
	        hi_delete(hi,HX_REJECTED);
	    }
	}

/*-----------------------------------------------------------------------
    If the remote system has connections disabled, ignore the message.
    If a specific remote node was named, set system shutting down  and
    declare a rejected error.
 -----------------------------------------------------------------------*/
        else if (sts & RI_DISABLED)
	{
	    if (hi->hi_node[0] != 0)
	    {
	        hi->hi_status = STS_ERROR;
	        hi->hi_error  = SLOT_STOP_SYSTEM;
	        hi_delete(hi,HX_REJECTED);
	    }
	}


/*-----------------------------------------------------------------------
    Process response message ----
 -----------------------------------------------------------------------*/
	else
	{
/*-----------------------------------------------------------------------
    Determine if response is from a LAT V5.2 Agent node.
 -----------------------------------------------------------------------*/
	    if (src[0] != ri->ri_node_addr[0] || src[1] != ri->ri_node_addr[1]
			|| src[2] != ri->ri_node_addr[2])
		resp_source = HI_AGENT;
	    else
		resp_source = 0;

/*----------------------------------------------------------------------
    If this is the first message -or- a service has been specified and this
    response has a higher service rating than any received so far, save
    this response.
 -----------------------------------------------------------------------*/
	    if (!(hi->hi_resp_status & HI_RESPONSE) ||
		(hi->hi_service[0] && svc_rating > hi->hi_save_rating))
	    {
		hi->hi_save_rating = svc_rating;
		hi->hi_save_status = sts;
		hi->hi_save_addr[0] = ri->ri_node_addr[0];
		hi->hi_save_addr[1] = ri->ri_node_addr[1];
		hi->hi_save_addr[2] = ri->ri_node_addr[2];
		hi->hi_save_idb = idb;
		str_input(&nd_temp[0],node_ptr,ND_STR,1);
		hi_str_move_sl(&hi->hi_save_node[0],&nd_temp[0]);
	    }

/*----------------------------------------------------------------------
    If we have an agent response and this is a response from the
    actual node, replace the node status.  We do this because VAX/VMS
    LAT V5.2 acting as an agent does not report the 'true' node status.
 ----------------------------------------------------------------------*/
	    if ((hi->hi_resp_status & HI_AGENT) &&
		    src[0] == hi->hi_save_addr[0] &&
		    src[1] == hi->hi_save_addr[1] &&
		    src[2] == hi->hi_save_addr[2])
		hi->hi_save_status = sts;

/*---------------------------------------------------------------------
    Update the response status.
 ---------------------------------------------------------------------*/
	    hi->hi_resp_status &= ~HI_AGENT;
	    hi->hi_resp_status |= (HI_RESPONSE | resp_source);
	}
    }
}
