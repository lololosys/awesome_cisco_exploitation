/*-----------------------------------------------------------------------
    ax_set_rate		(x) set dynamic rating by service
    
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO
    
    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved. 
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called when a directory service message is generated
    for each service flagged as AS_CLIENT. The function returns the rate
    which should be used for the service as the number of available objects.

    CALL BY:	byte ax_set_rate(as)
    		as_ptr	as;

    ON EXIT:	Returns the rating to be used for the service.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latas.h"
#include "latob.h"
#include "latfnc.h"
#include "sysfnc.h"

byte ax_set_rate(as)

    as_ptr	as;			/* Pointer to advertised serv	*/
{
    ob_ptr	ob;			/* Pointer to object		*/
    byte	rating;			/* Return rating		*/
    int		i;			/* Do index			*/
    
/*-----------------------------------------------------------------------
    This sample code sets the rating to the number of available objects.
    Any other algorithms of your choosing may be used.

    Loop through objects and offer a rating of the number of available
    remote/dynamic objects.
 -----------------------------------------------------------------------*/
    for (rating = 0, i = 0; i < as->as_cur_objects; i++)
    {
        ob = as->as_ob[i];
	if (!(ob->ob_flags & OB_LOCAL))
	{
	    if (ob->ob_rmt_use < ob->ob_rmt_max)
	        rating++;
	}
    }
    return(rating);
}
