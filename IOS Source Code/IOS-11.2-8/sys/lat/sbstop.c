/* $Id: sbstop.c,v 3.2 1995/11/17 17:35:18 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/sbstop.c,v $
 *------------------------------------------------------------------
 * $Log: sbstop.c,v $
 * Revision 3.2  1995/11/17  17:35:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:28:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:28:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    sb_stop		(e) stop session with no signal

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called when the envionrment wishes to stop a
    session and receive no further sx_ calls. The session is marked
    to send a stop slot, the sid is set to SID_NULL, and the object
    is released.

    CALL BY:	void sb_stop(sb,reason)
		sb_ptr	sb;
		int	reason;

    ON EXIT:	Set session to send stop and release object.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latob.h"
#include "latsb.h"
#include "latfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

void sb_stop(sb,reason)

    sb_ptr	sb;			/* Pointer to session block	*/
    int		reason;			/* Reject reason		*/
{
#if LE_OBJECTS
/*-----------------------------------------------------------------------
    If using objects, release the object.
 -----------------------------------------------------------------------*/
    if (sb->sb_ob != 0)
    {
	ob_ptr ob;

	ob = sb->sb_ob;
	sb->sb_ob = 0;
	ob_release(ob);
    }
#elif CME_SLAT
/*-----------------------------------------------------------------------
    If using CME, release the CME port.
 -----------------------------------------------------------------------*/
    if (sb->sb_cid != CID_NULL)
    {
	cid_type cid;
	cid = sb->sb_cid;
	sb->sb_cid = CID_NULL;
	cx_usage(cid,CM_RELEASE);
    }
#endif

/*-----------------------------------------------------------------------
    Store stop reason and signal just in case circuit is balanced.
 -----------------------------------------------------------------------*/
    sb->sb_sid = SID_NULL;
    sb->sb_stop_reason = (byte)(LOBYTE(reason));
    sb_signal(sb,(int)SB_STOP,0);
}
