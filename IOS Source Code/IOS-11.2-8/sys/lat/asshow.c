/*-----------------------------------------------------------------------
    as_show		(e) find named advertised service structure

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function returns a pointer to an advertised service structure
    specified by the given name.

    CALL BY:	as_ptr as_show(name)
    		byte	*name;

    ON EXIT:	The return argument, as, points to the advertised service
    		structure. If no service is found, the function returns
		zero and sets lat_error.

    	AS_ERROR | NO_NAME	No such named service exists
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latas.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	lat_error;		/* LAT error code		*/

as_ptr as_show(name)

    byte	*name;			/* Name of AS structure		*/
{
    as_ptr	as;			/* Return arguement pointer	*/
    int		alpha;			/* Current alphabetic order	*/

/*-----------------------------------------------------------------------
    Scan through list of advertised service structures looking for the
    advertised service specified. Stop when we find the name or go beyond
    it alphabetically.
 -----------------------------------------------------------------------*/
    for (as = hs->hs_as_head[0]; as != 0; as = as->as_link[0])
    {
	alpha = as_str_diff_sl(&as->as_service[0],name);

	if (alpha > 0)
	    as = 0;
	if (alpha >= 0)
	    break;
    }

/*-----------------------------------------------------------------------
    If as is zero, set the error code before returning.
 -----------------------------------------------------------------------*/
    if (as == 0)
        lat_error = AS_ERROR | NO_NAME;

    return(as);
}
