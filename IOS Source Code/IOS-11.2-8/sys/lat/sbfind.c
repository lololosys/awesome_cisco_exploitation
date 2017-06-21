/* $Id: sbfind.c,v 3.2 1995/11/17 17:35:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/sbfind.c,v $
 *------------------------------------------------------------------
 * $Log: sbfind.c,v $
 * Revision 3.2  1995/11/17  17:35:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:28:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:28:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    sb_find		(e/i) find session structure which matches sid

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function returns a pointer to the session structure specified
    by the given session identifier (sid).

    CALL BY:	sb_ptr sb_find(sid)
		sid_type sid;

    ON EXIT:	The return argument points to the sesion found.  If no
    		session is found, the function returns zero and sets
		lat_error.

    	SB_ERROR | NO_IDENT	No such session with supplied SID.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latsb.h"
#include "laterr.h"
#include "latfnc.h"

extern hs_ptr	hs;			/* Local node definitions	*/
extern int	lat_error;		/* LAT error codes		*/
	
sb_ptr sb_find(sid)

    sid_type	sid;			/* The session identifier	*/
{
    cb_ptr	cb;			/* Pointer to circuit block	*/
    sb_ptr	sb;			/* Pointer to session block	*/
    int		i;			/* Do indexes			*/

/*-----------------------------------------------------------------------
    Look through all the session structures to find the session which has
    the given sid.  Break when a match is found.
 -----------------------------------------------------------------------*/
    sb = 0;

    for (i = 1; i < hs->hs_cb_index; i++)
    {
	if ((cb = hs->hs_cb_table[i]) != 0)
	{
	    for (sb = cb->cb_slot[0]; sb != 0; sb = sb->sb_slot[0])
	    {	
		if (sb->sb_sid == sid)
		    break;
	    }
	    if (sb != 0)
		break;
	}
    }

/*-----------------------------------------------------------------------
    If no session found, set error code before exiting.
 -----------------------------------------------------------------------*/
    if (sb == 0)
	lat_error = SB_ERROR | NO_IDENT;

    return(sb);
}
