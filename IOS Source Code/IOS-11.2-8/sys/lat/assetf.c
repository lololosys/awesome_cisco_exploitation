/* $Id: assetf.c,v 3.2 1995/11/17 17:31:52 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/assetf.c,v $
 *------------------------------------------------------------------
 * $Log: assetf.c,v $
 * Revision 3.2  1995/11/17  17:31:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:24:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:21:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    as_set_flags	(e) update advertised service flags

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This functions updates the flags of an advertised service, making
    sure there is sufficient space in directory service message if the
    service is to be  offered. The function also signals a queue scan
    if there is any change in the service enabled/disabled or queued/no
    queue.

    CALL BY:	int as_set_flags(as,flags)
    		as_ptr	as;
    		int	flags;

    ON EXIT:	Returns SUCCESS if flags updated, FAILURE if new flags
    		cause service to be offered in directory service message
    		and there is not enough remaining space.

    	AS_ERROR | MAX_SPACE	Offered service exceeds maximum message size
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latas.h"
#include "laterr.h"
#include "latmsg.h"
#include "latfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	lat_error;		/* LAT error code		*/

int as_set_flags(as,flags)

    as_ptr	as;			/* Pointer to new AS structure	*/
    int		flags;			/* New service flags		*/
 {
    int		sts;			/* Status return		*/

#if (LE_OBJECTS || CME_SLAT) && LE_OBJECT_HIC
/*-----------------------------------------------------------------------
    If new flags changes enabled queue status, this may effect queued
    request. Signal a queue scan.
------------------------------------------------------------------------*/
    if ((as->as_flags & (AS_ENABLED | AS_QUEUED)) ^ (flags & (AS_ENABLED | AS_QUEUED)))
    {
	extern int qr_changes;
        qr_changes = SUCCESS;
    }
#endif

/*-----------------------------------------------------------------------
    If previous unoffered service is now being offered, check for
    sufficient space in directory service message.
------------------------------------------------------------------------*/
    if (!(as->as_flags & AS_OFFERED) && (flags & AS_OFFERED) &&
	 (hs->hs_as_space < (as->as_service[0] + as->as_ident[0] + 3)))
    {
	sts = FAILURE;
        lat_error = AS_ERROR | MAX_SPACE;
    }

/*-----------------------------------------------------------------------
    Store the flags and update the remaining space in directory service
    message.
------------------------------------------------------------------------*/
    else
    {
	sts = SUCCESS;

	if (as->as_flags & AS_OFFERED)
	{
	    hs->hs_as_next_changes |= (SVC_SERVICES | SVC_DESCRIPTOR);
	    hs->hs_as_space += (as->as_service[0] + as->as_ident[0] + 3);
	}

	as->as_flags = (byte)flags;

	if (as->as_flags & AS_OFFERED)
	{
	    hs->hs_as_next_changes |= (SVC_SERVICES | SVC_DESCRIPTOR);
	    hs->hs_as_space -= (as->as_service[0] + as->as_ident[0] + 3);
	}
    }

    return(sts);
}
