/* $Id: sbsig.c,v 3.2 1995/11/17 17:35:16 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/sbsig.c,v $
 *------------------------------------------------------------------
 * sb_signal - signal state change to session
 *
 * December 1993, Shoou Yiu
 *
 * Copyright (c) 1989-1994 by cisco Systems, Inc.
 * LAT technology licensed from Meridian Technology, Copyright 1989.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sbsig.c,v $
 * Revision 3.2  1995/11/17  17:35:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:28:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:28:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    sb_signal		(e) signal state change to session

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called by the environment to signal a state change
    to a session. A server circuit which is balanced is put into RUN
    state and set to trigger at the next tic. A host circuit which is
    balanced outputs a RUN message.

    CALL BY:	void sb_signal(sb,flags,credits)
		sb_ptr	sb;
		int	flags;
		int	credits;

    ON EXIT:	The appropriate signal raised for the session and
		balanced circuits put into RUN state.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latsb.h"
#include "latfnc.h"

extern hs_ptr	hs;			/* Local node definitions	*/

void sb_signal(sb,flags,credits)

    sb_ptr	sb;			/* Pointer to session block	*/
    int		flags;			/* State flags			*/
    int		credits;		/* Credits to issue to remote	*/
{
    cb_ptr	cb;			/* Pointer to circuit block	*/

/*-----------------------------------------------------------------------
    Set the session flags and credits.
 -----------------------------------------------------------------------*/
    sb->sb_flags |= (word)flags;
    if (flags & (SB_STOP | SB_REJECT))
	sb->sb_flags &= ~SB_STALL;

    if (credits != 0)
    {
	sb->sb_flags |= SB_CREDITS;
	sb->sb_lcl_credits += (byte)credits;
    }

/*-----------------------------------------------------------------------
    If circuit is balanced, take appropriate action to go to RUN state.
 -----------------------------------------------------------------------*/
    cb = sb->sb_cb;

    if (cb->cb_state == CB_BALANCE)
    {

/*-----------------------------------------------------------------------
    For server circuit, issue RUN message at next tic.
 -----------------------------------------------------------------------*/
	if (cb->cb_type == CB_SERVER)
	{
	    cb->cb_state = CB_RUN;
	    cb->cb_tics  = 1;
	}

/*-----------------------------------------------------------------------
    For host circuit, try outputting message.
 -----------------------------------------------------------------------*/
	else
	{
	    cb->cb_state = CB_BALANCE_R;
	    cb_run_output(cb);
	}
    }
}
