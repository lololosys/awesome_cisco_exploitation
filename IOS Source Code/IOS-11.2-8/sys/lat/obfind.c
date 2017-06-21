/* $Id: obfind.c,v 3.2 1995/11/17 17:34:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/obfind.c,v $
 *------------------------------------------------------------------
 * $Log: obfind.c,v $
 * Revision 3.2  1995/11/17  17:34:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:27:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    ob_find		(e/i) find object structure (by oid)

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function returns a pointer to the object structure specified
    by the given object identifier (oid).

    CALL BY:	ob_ptr ob_find(oid)
		oid_type oid;

    ON EXIT:	The return argument points to the object found.  If
    		no  object is found the function returns zero and sets
		lat_error.

    	OB_ERROR | NO_IDENT	No object with supplied oid.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latob.h"
#include "laterr.h"
#include "latfnc.h"

extern ob_ptr	ob_head[2];		/* OB structure list header	*/
extern int	lat_error;		/* LAT error code		*/

ob_ptr ob_find(oid)

    oid_type	oid;			/* The object identifier	*/
{
    ob_ptr	ob;			/* Pointer to OB structure	*/

/*-----------------------------------------------------------------------
    Look through all the object structures to find the object which has
    the oid given.  Break when a match is found.
 -----------------------------------------------------------------------*/
    for (ob = ob_head[0]; ob != 0; ob = ob->ob_link[0])
    {
	if (ob->ob_oid == oid)
	    break;
    }

/*-----------------------------------------------------------------------
   If no object was found, set the error code before returning.
 -----------------------------------------------------------------------*/
    if (ob == 0)
        lat_error = OB_ERROR | NO_IDENT;

    return(ob);
}
