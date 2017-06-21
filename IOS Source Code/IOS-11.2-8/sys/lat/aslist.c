/*-----------------------------------------------------------------------
    as_list		(e) find next advertised service structure

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This functions returns a pointer to the next alphabetically greater
    advertised service structure. A null name (*name == 0)  matches the
    first advertised service.

    CALL BY:	as_ptr as_list(name)
    		byte	*name;

    ON EXIT:	The address of the next alphabetically greater advertised
    		service is returned.  If no service is found, the function
    		returns zero and sets lat_error.

    	AS_ERROR | NO_NEXT	No more advertised services
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latas.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	lat_error;		/* LAT error code		*/

as_ptr as_list(name)

    byte	*name;			/* Advertised service name	*/
{
    as_ptr	as;			/* Pointer to AS structure	*/

/*-----------------------------------------------------------------------
    Scan through list of advertised service structures looking for the
    advertised service specified. Stop when we find the name or go beyond
    it alphabetically.
 -----------------------------------------------------------------------*/
    for (as = hs->hs_as_head[0]; as != 0; as = as->as_link[0])
    {
	if (as_str_diff_sl(&as->as_service[0],name) > 0)
	    break;
    }

/*-----------------------------------------------------------------------
    If as is zero, set error code before returning.
 -----------------------------------------------------------------------*/
    if (as == 0)
        lat_error = AS_ERROR | NO_NEXT;

    return(as);
}
