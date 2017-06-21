/* $Id: hises.c,v 3.2 1995/11/17 17:33:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hises.c,v $
 *------------------------------------------------------------------
 * $Log: hises.c,v $
 * Revision 3.2  1995/11/17  17:33:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:23:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hi_session		(e/i) find hic structure which matches sid

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function returns a pointer to the hic structure specified by
    the given HIC entry identifier (sid).

    CALL BY:	hi_ptr hi_session(sid)
		sid_type sid;

    ON EXIT:	The return argument points to the HIC entry found.  If no
    		entry is found, the function returns zero.

    	HI_ERROR | NO_IDENT	no matching hic entry
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "lathi.h"
#include "laterr.h"
#include "latfnc.h"

extern hs_ptr	hs;			/* Local node definitions	*/
extern int	lat_error;		/* LAT error codes		*/
	
hi_ptr hi_session(sid)

    sid_type	sid;			/* The HIC entry identifier	*/
{
    hi_ptr	hi;			/* Pointer to HIC block		*/

/*-----------------------------------------------------------------------
    Look through all the HIC structures to find the entry which has
    the given sid.  Break when a match is found.
 -----------------------------------------------------------------------*/
    for (hi = hs->hs_hi_head[0]; hi != 0; hi = hi->hi_link[0])
    {	
	if (hi->hi_sid == sid)
	    break;
    }

/*-----------------------------------------------------------------------
    If no entry found, set error code before exiting.
 -----------------------------------------------------------------------*/
    if (hi == 0)
	lat_error = HI_ERROR | NO_IDENT;

    return(hi);
}
