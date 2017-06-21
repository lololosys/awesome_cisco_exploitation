/* $Id: hihsts.c,v 3.2 1995/11/17 17:32:46 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hihsts.c,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: hihsts.c,v $
 * Revision 3.2  1995/11/17  17:32:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:23:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hi_h_sts_message	(i) process status message

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called to process a status message.

    CALL BY:	void hi_h_sts_message(frame)
		if_byte	frame;

    ON EXIT:	The matching HIC entry is processed.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "lathi.h"
#include "latmsg.h"
#include "laterr.h"
#include "latfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	lat_error;		/* LAT error codes		*/

void hi_h_sts_message(frame)

    if_byte	frame;			/* Pointer to input  message	*/
{
    hi_ptr	hi;			/* HIC request structure	*/
    req_iptr	req;			/* Input status fields		*/
    if_byte	next;			/* Pointer to next byte		*/
    int		entries;		/* Number of entries to process	*/

/*-----------------------------------------------------------------------
    Get the number of entries and loop for each entry.
 -----------------------------------------------------------------------*/
    next = frame + sizeof(sts_msg);

    entries = *next++;
    next += *next + 1;

    while (entries-- > 0)
    {

/*-----------------------------------------------------------------------
    Parse the entry into its various components.
 -----------------------------------------------------------------------*/
	if ((next - (if_byte)frame) & 1)
	    next++;

	req = (req_iptr)next;
	next += req->req_length + 1;

/*-----------------------------------------------------------------------
    Scan for a match on the request number.
 -----------------------------------------------------------------------*/
 	for (hi = hs->hs_hi_head[0]; hi != 0; hi = hi->hi_link[0]) {
	    if (!(hi->hi_flags & HI_SUBJECT)) {
	    	;

	    } else if (hi->hi_req_no != swap(req->req_req_no)) {
	    	;

	    } else {
	    	break;
	    }
    	}
	
	if (hi == 0)
	    lat_error = HI_ERROR | NO_IDENT;
	else
	{

/*-----------------------------------------------------------------------
    Update the hi entry with the data from the object system.
 -----------------------------------------------------------------------*/
	    hi->hi_status   = req->req_status;
	    hi->hi_error    = req->req_error;
	    hi->hi_que_no   = swap(req->req_que_no);
	    hi->hi_que_time = swap(req->req_que_time);
	    hi->hi_p_queue  = swap(req->req_p_queue);
	    hi->hi_q_queue  = swap(req->req_q_queue);

/*-----------------------------------------------------------------------
    If this is a cancel request, delete hi entry as all is done.
 -----------------------------------------------------------------------*/
	    if (hi->hi_command == CMD_CANCEL)
		hi_delete(hi,0);

/*-----------------------------------------------------------------------
    Otherwise, if an error is reported, report the request as rejected.
 -----------------------------------------------------------------------*/
	    else if (hi->hi_status & STS_ERROR)
		hi_delete(hi,HX_REJECTED);

#if LE_SERVER
/*-----------------------------------------------------------------------
    Otherwise, if the remote system reports system started but it is
    a host, we must start session from this end.
 -----------------------------------------------------------------------*/
	    else if (hi->hi_status & STS_STARTED)
		hx_session(hi,HX_ACCEPT);
#endif

/*-----------------------------------------------------------------------
    Otherwise, all is well, mark this entry as quiet and call environment
    with updated status.
 -----------------------------------------------------------------------*/
	    else
	    {
		hi->hi_tics    = 0;
		hi->hi_retries = 0;
		hx_session(hi,HX_STATUS);
	    }
	}
    }
}
