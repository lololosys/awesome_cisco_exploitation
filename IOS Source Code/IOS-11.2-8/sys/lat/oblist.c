/*-----------------------------------------------------------------------
    ob_list		(e) return next object name

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function returns a pointer to the next alphabetically greater
    object in the list of objects.  A null name (*name == 0) matches the
    first object.

    CALL BY:	ob_ptr ob_list(name)
    		byte	*name;

    ON EXIT:	The address of the next alphabetically greater object is
		returned.  If no object is found the function returns zero
		and sets lat_error.

    	OB_ERROR | NO_NEXT	No more objects found.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latob.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	ob_ptr	ob_head[2];     	/* OB structure list header	*/
extern	int	lat_error;		/* LAT error code		*/

ob_ptr ob_list(name)

    byte	*name;			/* Last object name		*/
{
    ob_ptr	ob;			/* Pointer to OB structure	*/

/*-----------------------------------------------------------------------
    Scan through list of object structures looking for the object specified.
    Stop when we find the name or go beyond it alphabetically.
 -----------------------------------------------------------------------*/
    for (ob = ob_head[0]; ob != 0; ob = ob->ob_link[0])
    {
	if (ob_str_diff_sl(&ob->ob_name[0],name) > 0)
	    break;
    }

/*-----------------------------------------------------------------------
    If the object was not found, set the error code before returning.
 -----------------------------------------------------------------------*/
    if (ob == 0)
       lat_error = OB_ERROR | NO_NEXT;

    return(ob);
}
