/* $Id: hiqrpt.c,v 3.2 1995/11/17 17:32:58 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hiqrpt.c,v $
 *------------------------------------------------------------------
 * $Log: hiqrpt.c,v $
 * Revision 3.2  1995/11/17  17:32:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:23:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hi_queue_report	(e) request queue entry reports

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function initiates a queue report request to the named node.
    The remote node will return status message for each of our queued
    entries. If we know the remote node, a command message is sent.
    Otherwise we first solicit the remote node address.

    CALL BY:	hi_ptr hi_queue_report(node)
		byte	 *node;

    ON EXIT:	Returns pointer to HIC entry if request initiated,
    		a zero value otherwise.

	HI_ERROR | MAX_LIMIT	New entry exceeds HIC entry limit
	HI_ERROR | NO_MEM	Could not allocate HIC entry
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "lathi.h"
#include "latnd.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

hi_ptr hi_queue_report(node,idb)
    byte	*node;			/* Pointer to service nodename	*/
    idbtype	*idb;			/* Interface to connect to      */
{
    hi_ptr	hi;			/* Pointer to HIC block		*/
    nd_ptr	nd;			/* Pointer to remote node	*/

/*-----------------------------------------------------------------------
    Initialize the HI structure.
 -----------------------------------------------------------------------*/
    if ((hi = hi_create(HI_SUBJECT)) != 0)
    {

/*-----------------------------------------------------------------------
    If we know the node, initialize to send one command message.
 -----------------------------------------------------------------------*/
	if ((nd = nd_show(node,idb)) != 0)
	{
	    hi->hi_function = LAT_COMMAND;
	    hi->hi_dest_addr[0] = nd->nd_host_addr[0];
	    hi->hi_dest_addr[1] = nd->nd_host_addr[1];
	    hi->hi_dest_addr[2] = nd->nd_host_addr[2];
	    hi->hi_idb          = nd->nd_idb;
	    hi->hi_tics     = 1;
	    hi->hi_retries  = 1;

	    hi->hi_flags |= HI_DELETE;
	    hi->hi_req_no = 0;
	}

/*-----------------------------------------------------------------------
    Otherwise, first solicit the ethernet address of remote node.
 -----------------------------------------------------------------------*/
	else
	{
	    hi->hi_function = LAT_SOLICIT;
	    hi->hi_dest_addr[0] = hs->hs_multicast[0];
	    hi->hi_dest_addr[1] = hs->hs_multicast[1];
	    hi->hi_dest_addr[2] = hs->hs_multicast[2];
	    hi->hi_idb = idb;
	    hi->hi_tics     = 1;
	    hi->hi_retries  = hs->hs_hi_cmd_retries;
	}

/*-----------------------------------------------------------------------
    Set command to status of all of our entries.
 -----------------------------------------------------------------------*/
	hi->hi_command  = CMD_STS_ALL;
	hi->hi_modifier = 0;

	hi_str_move_sl(&hi->hi_node[0],node);

	hi->hi_sc = SVC_CLASS_TERM; /* Set service class */
    }
    return(hi);
}
