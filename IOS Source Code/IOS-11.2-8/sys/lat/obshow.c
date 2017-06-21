/*-----------------------------------------------------------------------
    ob_show		(e) find object structure (by name)

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function returns a pointer to an object structure specified by
    the given object name.

    CALL BY:	ob_ptr ob_show(name)
    		byte	*name;

    ON EXIT:	The return argument points to the object structure.  If
    		no object is found, the function returns zero and sets
    		lat_error.

	OB_ERROR | NO_NAME	No object with supplied name
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latob.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	ob_ptr	ob_head[2];     	/* OB structure list header	*/
extern	int	lat_error;		/* LAT error code		*/

ob_ptr ob_show(name)

    byte	*name;			/* Name of OB structure		*/
{
    ob_ptr	ob;			/* Return arguement pointer	*/
    int		alpha;			/* Comparsion results		*/

/*-----------------------------------------------------------------------
    Scan through list of object structures looking for the object name
    specified. Stop when we find the name.
 -----------------------------------------------------------------------*/
    for (ob = ob_head[0]; ob != 0; ob = ob->ob_link[0])
    {
	alpha = ob_str_diff_sl(&ob->ob_name[0],name);

	if (alpha > 0)
	    ob = 0;
	if (alpha >= 0)
	    break;
    }

/*-----------------------------------------------------------------------
    If no object was found, set the error code before returning.
 -----------------------------------------------------------------------*/
    if (ob == 0)
        lat_error = OB_ERROR | NO_NAME;

    return(ob);
}
