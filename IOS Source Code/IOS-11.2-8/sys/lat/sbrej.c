/* $Id: sbrej.c,v 3.2 1995/11/17 17:35:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/sbrej.c,v $
 *------------------------------------------------------------------
 * $Log: sbrej.c,v $
 * Revision 3.2  1995/11/17  17:35:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:28:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:28:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    sb_reject		(e) reject start session

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called when the envionrment wishes to reject a
    start slot.

    CALL BY:	void sb_reject(sb,reason)
		sb_ptr	sb;
		int	reason;

    ON EXIT:	The reject reason is stored and the session is flagged
    		to send a reject slot.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latsb.h"
#include "latfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

void sb_reject(sb,reason)

    sb_ptr	sb;			/* Pointer to session block	*/
    int		reason;			/* Reject reason		*/
{
/*-----------------------------------------------------------------------
    Store reject reason and signal just in case circuit is balanced.
 -----------------------------------------------------------------------*/
    sb->sb_stop_reason = (byte)(LOBYTE(reason));

#if CME_SLAT
    if (sb->sb_cid != CID_NULL)
    {
	cx_usage(sb->sb_cid, CM_RELEASE);
	sb->sb_cid = CID_NULL;
    }
#endif

    sb->sb_flags &= ~SB_STALL;
    sb_signal(sb,(int)SB_REJECT,0);
}
