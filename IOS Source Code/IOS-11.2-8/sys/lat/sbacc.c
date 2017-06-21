/* $Id: sbacc.c,v 3.2 1995/11/17 17:35:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/sbacc.c,v $
 *------------------------------------------------------------------
 * $Log: sbacc.c,v $
 * Revision 3.2  1995/11/17  17:35:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:28:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:28:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    sb_accept		(e) accept session

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called when the environment wishes to accept a
    start slot. The SID and initial credits are assigned (note it is
    legal to accept a session with no initial credits).

    CALL BY:	void sb_accept(sb,sid,credits)
		sb_ptr	 sb;
		sid_type sid;
		int	credits;

    ON EXIT:	The session is flagged to send start slot.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latsb.h"
#include "latfnc.h"

void sb_accept(sb,sid,credits)

    sb_ptr	sb;			/* Pointer to session block	*/
    sid_type	sid;			/* Session identification	*/
    int		credits;		/* Initial credits to extend	*/
{

/*-----------------------------------------------------------------------
    Store the session identification.
 -----------------------------------------------------------------------*/
    sb->sb_sid = sid;

/*-----------------------------------------------------------------------
    Clear the stall mask and signal the session is to send start.
 -----------------------------------------------------------------------*/
    sb->sb_flags &= ~SB_STALL;
    sb_signal(sb,SB_START,credits);
}
