/* $Id: asrset.c,v 3.2 1995/11/17 17:31:50 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/asrset.c,v $
 *------------------------------------------------------------------
 * $Log: asrset.c,v $
 * Revision 3.2  1995/11/17  17:31:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:24:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:21:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    as_reset		(e) delete all advertised services

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function deletes all advertised services.

    CALL BY:	void as_reset()

    ON EXIT:	All advertised services deleted.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latas.h"
#include "latfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

void as_reset()

{
/*-----------------------------------------------------------------------
    Loop through the advertised service list and delete all advertised
    services.
 -----------------------------------------------------------------------*/
    while (hs->hs_as_head[0] != 0)
	as_delete(hs->hs_as_head[0]);
}
