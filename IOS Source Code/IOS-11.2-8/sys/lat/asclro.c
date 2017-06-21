/* $Id: asclro.c,v 3.2 1995/11/17 17:31:46 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/asclro.c,v $
 *------------------------------------------------------------------
 * $Log: asclro.c,v $
 * Revision 3.2  1995/11/17  17:31:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:24:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:21:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    as_clr_object	(e) clear object from advertised service

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function removes an object association from the specified
    advertised service. If the oid is invalid or the object is not
    associated with the service, no operation is performed.

    This function may only be used by platforms which select object
    support (LE_OBJECTS=1).

    CALL BY:	void as_clr_object(as,oid)
    		as_ptr    as;
    		oid_type  oid;

    ON EXIT:	The object specified by oid argument is removed from
    		the advertised service structure as_ob array. A queue
    		scan is requested.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latas.h"
#include "latob.h"
#include "latfnc.h"

#if LE_OBJECTS
extern	int	ob_highest;		/* Largest number of objects	*/
extern	ob_ptr	ob_map[];		/* Object mapping array		*/
#endif

void as_clr_object(as,oid)

    as_ptr	as;  			/* Pointer to AS structure	*/
    oid_type	oid;			/* Object identification	*/
{
#if LE_OBJECTS
    ob_ptr	ob;			/* Pointer to object structure	*/
    int		i;  			/* Array position counter	*/

/*-----------------------------------------------------------------------
    Convert the oid to an object. If no object found, all done.
 -----------------------------------------------------------------------*/
    if ((ob = ob_find(oid)) != 0)
    {

/*-----------------------------------------------------------------------
    Scan through the object mapping array to find the object position
    and clear the corresponding bit in the AS object bit map.  The change
    may effect current queue entries, so qr_changes is set to success.
 -----------------------------------------------------------------------*/
        for (i = 0; i < ob_highest; i++)
        {
	    if (ob_map[i] == ob)
	    {

#if LE_OBJECT_HIC
		extern int qr_changes;
		qr_changes = SUCCESS;
#endif
		as->as_cur_objects--;
	        as->as_ob_map[i/8] &= ~(1 << (i%8));
       		    break;
    	    }
        }
    }
#endif

}
