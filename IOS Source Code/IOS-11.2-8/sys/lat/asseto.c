/*
 * $Id: asseto.c,v 3.2 1995/11/17 17:31:53 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/asseto.c,v $
 *-----------------------------------------------------------------
 * $Log: asseto.c,v $
 * Revision 3.2  1995/11/17  17:31:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:24:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:21:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *-----------------------------------------------------------------
 * $Endlog$
 */
 
/*-----------------------------------------------------------------------
    as_set_object	(e) associate object with advertised service

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function associates a specified object with an advertised
    service. If the oid is invalid or the new object exceeds the
    maximum objects which can be associated with the service, the
    function returns failure.

    This function may only be used by platforms which select object
    support (LE_OBJECTS=1).

    CALL BY:    int as_set_object(as,oid)
    		as_ptr    as;
    		oid_type  oid;

    ON EXIT:	Return SUCCESS if object mapping is stored, FAILURE
		otherwise and lat_error set as follows:

	AS_ERROR | MAX_SPACE	New object exceeds maximum number
	AS_ERROR | NO_IDENT	No object found with oid identifier
	OB_ERROR | OP_ILLEGAL	Cannot assign cloned object
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latas.h"
#include "latob.h"
#include "laterr.h"
#include "latfnc.h"

extern	int	lat_error;		/* LAT error code		*/

#if LE_OBJECTS
extern ob_ptr	ob_map[];		/* Object mapping array		*/
extern int	ob_highest;		/* Largest number of objects	*/
#endif

int as_set_object(as,oid)

    as_ptr	as;			/* Pointer to AS structure	*/
    oid_type	oid;			/* Object identification	*/
{
#if LE_OBJECTS
    ob_ptr	ob;			/* Pointer to object structure	*/
    int		i;			/* Index counter		*/
#endif LE_OBJECTS
    int		sts;			/* SUCCESS/FAILURE return value	*/

/*-----------------------------------------------------------------------
    Preset status to failure.
 -----------------------------------------------------------------------*/
    sts = FAILURE;

#if LE_OBJECTS
/*-----------------------------------------------------------------------
    If OID is not valid, return FAILURE.
 -----------------------------------------------------------------------*/
    if ((ob = ob_find(oid)) == 0)
	lat_error = AS_ERROR | NO_IDENT;

/*-----------------------------------------------------------------------
    If object is cloned, return FAILURE.
 -----------------------------------------------------------------------*/
    else if (ob->ob_flags & OB_CLONED)
	lat_error = OB_ERROR | OP_ILLEGAL;

/*-----------------------------------------------------------------------
    Scan the object mapping array to find the object position and set
    the corresponding bit in the AS object bit map.
 -----------------------------------------------------------------------*/
    else
    {
#if LE_OBJECT_HIC
	extern int qr_changes;
	qr_changes = SUCCESS;
#endif

	for (i = 0; i < ob_highest; i++)
	{
	    if (ob_map[i] == ob)
	    {
		as->as_cur_objects++;
		as->as_ob_map[i/8] |= (1 << (i%8));
		break;
	    }
	}
	sts = SUCCESS;
    }
#endif

    return(sts);
}
