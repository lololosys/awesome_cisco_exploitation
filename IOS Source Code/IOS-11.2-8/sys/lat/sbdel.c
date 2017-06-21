/* $Id: sbdel.c,v 3.2 1995/11/17 17:35:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/sbdel.c,v $
 *------------------------------------------------------------------
 * $Log: sbdel.c,v $
 * Revision 3.2  1995/11/17  17:35:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:28:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:28:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    sb_delete		(i) delete session block

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function deletes a session block. The environment layer is
    notified and the usage of the object is released.

    CALL BY:	void sb_delete(sb,reason)
    		sb_ptr	sb;
    		int	reason;

    ON EXIT:	Session block is deleted.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latnd.h"
#include "latob.h"
#include "latsb.h"
#include "lathi.h"
#include "latfnc.h"
#include "sysfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	le_ls_dups;		/* Duplicate name support	*/

void sb_delete(sb,reason)

    sb_ptr	sb;			/* Pointer to session block	*/
    int		reason;			/* Session delete reason	*/
{
    cb_ptr	cb;			/* Pointer to circuit block	*/
    sb_ptr	sx;			/* Pointer to session block	*/
    hi_ptr	hi;			/* Pointer to HI block		*/
    int		ses;			/* Session id			*/


/*-----------------------------------------------------------------------
    If we are in the process of a solicit connect, find and delete the
    associated HI structure.
 -----------------------------------------------------------------------*/
#if LE_SUBJECT_HIC
    for (hi = hs->hs_hi_head[0]; hi != 0; hi = hi->hi_link[0])
    {
    	if (hi->hi_sb == sb)
	    break;
    }
    if (hi != 0)
    	hi_delete(hi,0);	
#endif	
	
/*-----------------------------------------------------------------------
    Count one less session.
 -----------------------------------------------------------------------*/
    if ((cb = sb->sb_cb) != 0)
    {
    	cb->cb_ses_count--;
    	cb->cb_nd->nd_usage--;

/*-----------------------------------------------------------------------
    Unlink the session from the circuit hash table. We must scan the
    entire hash table as local ID is lost when we send the stop, reject
    slot.
 -----------------------------------------------------------------------*/
    	for (ses = 0; ses < SB_HASHES; ses++)
    	{
	    for (sx = cb->cb_sb[ses][0]; sx != 0; sx = sx->sb_link[0])
	    {
	    	if (sx == sb)
	    	{
		    if (sb->sb_link[0] != 0)
		    	sb->sb_link[0]->sb_link[1]  = sb->sb_link[1];
		    else
		    	cb->cb_sb[ses][1] = sb->sb_link[1];

		    if (sb->sb_link[1] != 0)
		    	sb->sb_link[1]->sb_link[0]  = sb->sb_link[0];
		    else
		    	cb->cb_sb[ses][0] = sb->sb_link[0];

		    ses = SB_HASHES;
		    break;
	        }
	    }
        }

/*-----------------------------------------------------------------------
    Unlink the session from the circuit slot chain.
 -----------------------------------------------------------------------*/
    	if (sb->sb_slot[0] != 0)
	    sb->sb_slot[0]->sb_slot[1]  = sb->sb_slot[1];
    	else
	    cb->cb_slot[1] = sb->sb_slot[1];

    	if (sb->sb_slot[1] != 0)
	    sb->sb_slot[1]->sb_slot[0]  = sb->sb_slot[0];
    	else
	    cb->cb_slot[0] = sb->sb_slot[0];
    }

/*-----------------------------------------------------------------------
    Attempt to start session on alternate service. If this fails, then
    complete deleting the session.
 -----------------------------------------------------------------------*/
#if LE_LS_DUPS
    if (!le_ls_dups || !sb_dups(sb))
    {
#endif

/*-----------------------------------------------------------------------
    Notify the environment session is being deleted and why.
 -----------------------------------------------------------------------*/
	if (sb->sb_sid != SID_NULL && reason != 0)
	    sx_session(sb,reason);

#if LE_OBJECTS
/*-----------------------------------------------------------------------
    If we managed objects, release this usage of the object.
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
    If we are using CME, release this usage of the port.
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
    Free the session block.
 -----------------------------------------------------------------------*/
	hs->hs_sb_current--;
	sb_free(sb);

#if LE_LS_DUPS
    }
#endif
}
