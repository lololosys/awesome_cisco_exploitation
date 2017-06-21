/* $Id: hiopen.c,v 3.2 1995/11/17 17:32:50 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hiopen.c,v $
 *------------------------------------------------------------------
 * $Log: hiopen.c,v $
 * Revision 3.2  1995/11/17  17:32:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:23:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hi_open		(e) open HIC session to host

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function starts the process of opening a session to the supplied
    request number. a session block (and possibly circuit block) is created.
    The start slot message will be sent at the next circuit interval.

    CALL BY:	sb_ptr hi_open(hi,sid,credits)
		hi_ptr   hi;
		sid_type sid;
    		int	 credits;

    ON EXIT:	Returns pointer to session block if the session is
    		start, a zero value otherwise.

		N.B If successful, the HI entry is deleted. If error,
    		a reject reason is sent to remote system.

	SB_ERROR | NU_IDENT	SID is already used by session
	SB_ERROR | MAX_LIMIT	Reached limit of sessions on circuit or host
	SB_ERROR | NO_MEM	Could not allocate session block
	CB_ERROR | MAX_LIMIT	Reached limit of number of circuits
	CB_ERROR | NO_MEM	Could not allocate circuit block
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "lathi.h"
#include "latnd.h"
#include "latsb.h"
#include "laterr.h"
#include "latmsg.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	word	nd_addr[3];		/* Destination node address	*/
extern	byte	ls_temp[];		/* Temporary LS name		*/
extern	byte	nd_temp[];		/* Temporary ND name		*/
extern	byte	ob_temp[];		/* Temporary OB name		*/
extern	int	lat_error;		/* LAT error code		*/

sb_ptr hi_open(hi,sid,credits)

    hi_ptr	hi;			/* Pointer to HIC block		*/
    sid_type	sid;			/* Session ID 			*/
    int		credits;		/* Number of credits to extend	*/
{
    sb_ptr	sb;			/* Pointer to session block	*/
    cb_ptr	cb;			/* Pointer to circuit block	*/
    nd_ptr	nd;			/* Pointer to node structure	*/
    int		i;			/* Do index			*/

/*-----------------------------------------------------------------------
    Preset the return status to FAILURE.
 -----------------------------------------------------------------------*/
    sb = 0;

/*-----------------------------------------------------------------------
    If the session ID is not unique, fail.
 -----------------------------------------------------------------------*/
    if (sb_find(sid) != 0)
	lat_error = SB_ERROR | NU_IDENT;

/*-----------------------------------------------------------------------
    Everything has been checked out. Create the session (and circuit
    block if needed). Timebased scheduling will generate messages.
 -----------------------------------------------------------------------*/
    else
    {

/*-----------------------------------------------------------------------
    Copy the destination address and create a node structure if there
    is not one already.
 -----------------------------------------------------------------------*/
	nd_addr[0] = hi->hi_dest_addr[0];
	nd_addr[1] = hi->hi_dest_addr[1];
	nd_addr[2] = hi->hi_dest_addr[2];
	hi_str_move_ls(&nd_temp[0],&hi->hi_node[0]);
	nd = nd_create(&nd_temp[0],&nd_addr[0],hi->hi_idb);

/*-----------------------------------------------------------------------
    Scan the existing circuit blocks for a match on this service node.
 -----------------------------------------------------------------------*/
	for (cb = 0, i = 1; i < hs->hs_cb_index; i++)
	{
	    if (hs->hs_cb_table[i] != 0 && hs->hs_cb_table[i]->cb_type == CB_SERVER)
	    {
		if (hs->hs_cb_table[i]->cb_nd == nd)
		{
		    cb = hs->hs_cb_table[i];
		    break;
		}
	    }
	}

/*-----------------------------------------------------------------------
    If no circuit block found, allocate and initialize a new circuit to
    generate a circuit START at the next tic interval.
 -----------------------------------------------------------------------*/
	if (cb == 0 && nd != 0)
	{
	    if ((cb = cb_create(nd)) != 0)
	    {
		cb->cb_type  = CB_SERVER;
		cb->cb_state = CB_START;
		cb->cb_tics  = 1;
	    }
	}

/*-----------------------------------------------------------------------
    If we have a circuit block, create the session block and initialize
    to generate a START SLOT in the next circuit RUN message.
 -----------------------------------------------------------------------*/
	if (cb != 0 && (sb = sb_create(cb)) != 0)
	{
	    count_1(co_solicit_accept);
	    hi->hi_tics = 0;
	    hi->hi_sid  = sid;

	    sb->sb_sid    = sid;
#if CME_SLAT
	    sb->sb_cid    = hi->hi_cid;
	    hi->hi_cid	  = CID_NULL;
#elif LE_OBJECTS
	    sb->sb_ob     = hi->hi_ob;
	    hi->hi_ob     = 0;
#endif
	    sb->sb_sc	  = hi->hi_sc;
	    sb->sb_flags |= SB_HIC;
	    if (hi->hi_flags & HI_OBJECT)
	    	sb->sb_req_no = hi->hi_req_no;
	    else
	        sb->sb_req_no = hi->hi_que_no;

	    hi_str_move_ls(&ls_temp[0],&hi->hi_service[0]);
	    sb_str_move_sl(&sb->sb_name[0],&ls_temp[0]);

	    hi_str_move_ls(&ob_temp[0],&hi->hi_port[0]);
	    sb_str_move_sl(&sb->sb_port[0],&ob_temp[0]);
	
	    sb_signal(sb,SB_START,credits);
	    hi_delete(hi,0);
	}

    }

/*-----------------------------------------------------------------------
    If we fail, send error status to remote system.
 -----------------------------------------------------------------------*/
    if (sb == 0)
	hi_reject(hi,SLOT_STOP_NO_RESOURCE);

    return(sb);
}
