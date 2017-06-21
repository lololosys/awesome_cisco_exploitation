/* $Id: hszero.c,v 3.2 1995/11/17 17:33:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hszero.c,v $
 *------------------------------------------------------------------
 * $Log: hszero.c,v $
 * Revision 3.2  1995/11/17  17:33:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:24:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hs_zero		(e) zero host counters

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    Zero the host counters.

    CALL BY:    void hs_zero()

    ON EXIT:    All host counters set to zero.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

void hs_zero()
{
#if LE_HS_STATS
    byte	*ptr;			/* Pointer to counters		*/
    int		i;			/* Do index			*/

/*-----------------------------------------------------------------------
    Loop through the host counters by word and zero.
 -----------------------------------------------------------------------*/
     for (ptr = (byte *)&hs->hs_stats, i = 0; i < sizeof(hs->hs_stats); i++)
         *ptr++ = 0;
     for (ptr = (byte *)&hs->hs_count, i = 0; i < sizeof(hs->hs_count); i++)
         *ptr++ = 0;
#endif
}
