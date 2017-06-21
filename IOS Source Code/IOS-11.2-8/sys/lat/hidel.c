/* $Id: hidel.c,v 3.2 1995/11/17 17:32:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hidel.c,v $
 *------------------------------------------------------------------
 * $Log: hidel.c,v $
 * Revision 3.2  1995/11/17  17:32:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:22:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hi_delete		(i) delete hic block

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function deletes a HIC block. If a non-zero reason is supplied,
    the function notifies the environment. Any associated queue entry is
    deleted and the lock on the object of CME port is released.

    CALL BY:	void hi_delete(hi,reason)
    		hi_ptr	hi;
		int	reason;

    ON EXIT:	HIC structure is deleted.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "lathi.h"
#include "latob.h"
#include "latqr.h"
#include "latfnc.h"
#include "sysfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

extern	hs_ptr	hs;			/* Local node definitions	*/

void hi_delete(hi,reason)

    hi_ptr	hi;			/* Pointer to HIC structure	*/
    int		reason;			/* Delete reason code		*/
{

/*-----------------------------------------------------------------------
    If reason for delete, notify environment if this is initiate.
 -----------------------------------------------------------------------*/
    if (reason != 0 && hi->hi_sid != SID_NULL)
	hx_session(hi,reason);

#if (LE_OBJECTS || CME_SLAT) && LE_OBJECT_HIC
/*-----------------------------------------------------------------------
    If request is queued, delete it.
 -----------------------------------------------------------------------*/
    if (hi->hi_qr != 0)
	qr_delete(hi->hi_qr);
#endif

#if LE_OBJECTS
/*-----------------------------------------------------------------------
    Release the object associated with the hic request.
 -----------------------------------------------------------------------*/
    if (hi->hi_ob != 0)
    {
	ob_ptr ob;

	ob = hi->hi_ob;
	hi->hi_ob = 0;
	ob_release(ob);
    }
#elif CME_SLAT
/*-----------------------------------------------------------------------
    Release the CME port associated with the hic request.
 -----------------------------------------------------------------------*/
    if (hi->hi_cid != CID_NULL)
    {
	cx_usage(hi->hi_cid,CM_RELEASE);
	hi->hi_cid = CID_NULL;
    }
#endif

/*-----------------------------------------------------------------------
    Unlink the structure and deallocate.
 -----------------------------------------------------------------------*/
    if (hi->hi_link[0] != 0)
	hi->hi_link[0]->hi_link[1]  = hi->hi_link[1];
    else
	hs->hs_hi_head[1] = hi->hi_link[1];

    if (hi->hi_link[1] != 0)
	hi->hi_link[1]->hi_link[0]  = hi->hi_link[0];
    else
	hs->hs_hi_head[0] = hi->hi_link[0];

    hs->hs_hi_current--;
    hi_free(hi);
}
