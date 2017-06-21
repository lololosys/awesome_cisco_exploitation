/* $Id: sbdups.c,v 3.2 1995/11/17 17:35:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/sbdups.c,v $
 *------------------------------------------------------------------
 * $Log: sbdups.c,v $
 * Revision 3.2  1995/11/17  17:35:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:28:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:28:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    sb_dups		(e) open duplicate service

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called when a session fails. If the session was
    starting and a duplicate service is available, the function attempts
    to open the session to that node.

    CALL BY:	int sb_dups(sb)
		sb_ptr	sb;

    ON EXIT:	Returns SUCCESS if new session start initiated, failed
    		otherwise.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latls.h"
#include "latnd.h"
#include "latob.h"
#include "latsb.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	byte	ls_temp[];		/* Temporary LS name		*/

int sb_dups(sb)

    sb_ptr	sb;			/* Pointer to session block	*/
{
    sb_ptr	sx;			/* Pointer to session block	*/
    cb_ptr	cb;			/* Pointer to circuit block	*/
    nd_ptr	nd;			/* Pointer to remote node	*/
    ls_ptr	ls;			/* Pointer to learned service	*/
    int		ses;			/* Session ID			*/
    int		sts;			/* Status return		*/
    int		i;			/* Do index			*/

/*-----------------------------------------------------------------------
    Increment the name sequence and copy the service name.
 -----------------------------------------------------------------------*/
    sb->sb_max_attn++;
    sb_str_move_ls(&ls_temp[0],&sb->sb_name[0]);

/*-----------------------------------------------------------------------
    If circuit not server, fail.
 -----------------------------------------------------------------------*/
    if (sb->sb_cb->cb_type != CB_SERVER)
	sts = FAILURE;

/*-----------------------------------------------------------------------
    If service name is null, fail.
 -----------------------------------------------------------------------*/
    else if (ls_temp[0] == 0)
	sts = FAILURE;

/*-----------------------------------------------------------------------
    If the session not starting, fail.
 -----------------------------------------------------------------------*/
    else if (!(sb->sb_flags & (SB_START | SB_STALL)))
	sts = FAILURE;

/*-----------------------------------------------------------------------
    If no more SID, fail.
 -----------------------------------------------------------------------*/
    else if (sb->sb_sid == SID_NULL)
	sts = FAILURE;

/*-----------------------------------------------------------------------
    If explicit node specified or HIC request, fail.
 -----------------------------------------------------------------------*/
    else if (sb->sb_flags & (SB_NODE | SB_HIC))
	sts = FAILURE;

/*-----------------------------------------------------------------------
    If failed for password, fail.
 -----------------------------------------------------------------------*/
    else if (sb->sb_stop_reason == SLOT_STOP_PASSWORD)
	sts = FAILURE;

/*-----------------------------------------------------------------------
    If there is no more learned services with this name, fail.
 -----------------------------------------------------------------------*/
#if !CME_SLAT
    else if ((ls = ls_scan(&ls_temp[0],sb->sb_ob,(int)sb->sb_max_attn)) == 0)
#else
    else if ((ls = ls_scan(&ls_temp[0],sb->sb_cid,(int)sb->sb_max_attn)) == 0)
#endif
	sts = FAILURE;

/*-----------------------------------------------------------------------
    If this is same node as used in previous attempt, skip.
 -----------------------------------------------------------------------*/
    else if (sb->sb_cb->cb_nd == ls->ls_nd)
	sts = sb_dups(sb);

/*-----------------------------------------------------------------------
    If this remote node is unreachable, unavailable, skip.
 -----------------------------------------------------------------------*/
    else if (ls->ls_nd->nd_flags & (ND_UNREACH | ND_UNKNOWN))
	sts = sb_dups(sb);

/*-----------------------------------------------------------------------
    Everything has been checked out. Create the session (and circuit
    block if needed). Timebased scheduling will generate messages.
 -----------------------------------------------------------------------*/
    else
    {

/*-----------------------------------------------------------------------
    Scan the existing circuit blocks for a match on this service node.
 -----------------------------------------------------------------------*/
	nd = ls->ls_nd;
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
	if (cb == 0)
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
	if (cb == 0)
	    sts = FAILURE;

	else
	{
	    sts = SUCCESS;

	    cb->cb_ses_count++;
	    cb->cb_nd->nd_usage++;

	    while (SUCCESS)
	    {
		if ((ses = ++cb->cb_session) == 0)
		    continue;
		for (sx = cb->cb_sb[sb_hash(ses)][0]; sx != 0; sx = sx->sb_link[0])
		{
		    if (sx->sb_lcl_ses_id == (byte)ses)
			break;
		}
		if (sx == 0)
		    break;
	    }

	    sb->sb_link[0] = 0;
	    sb->sb_link[1] = cb->cb_sb[sb_hash(ses)][1];
	    cb->cb_sb[sb_hash(ses)][1] = sb;
	    if (sb->sb_link[1] != 0)
		sb->sb_link[1]->sb_link[0] = sb;
	    else
		cb->cb_sb[sb_hash(ses)][0] = sb;

	    sb->sb_slot[0] = 0;
	    sb->sb_slot[1] = cb->cb_slot[1];
	    cb->cb_slot[1] = sb;
	    if (sb->sb_slot[1] != 0)
		sb->sb_slot[1]->sb_slot[0] = sb;
	    else
		cb->cb_slot[0] = sb;

	    sb->sb_cb = cb;
	    sb->sb_flags  = 0;
	    sb->sb_req_no = 0;

	    sb->sb_rmt_ses_id = 0;
	    sb->sb_lcl_ses_id = (byte)ses;

	    sb->sb_rmt_credits = 0;
	    sb->sb_lcl_credits += sb->sb_ext_credits;
	    sb->sb_ext_credits = 0;
	    sb->sb_stop_reason = 0;
	    sb_signal(sb,SB_START,0);
	}
    }
    return(sts);
}
