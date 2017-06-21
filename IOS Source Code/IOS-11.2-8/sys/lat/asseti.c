/*-----------------------------------------------------------------------
    as_set_ident	(e) update advertised service identification

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function updates the identification of an advertised service,
    making sure there is sufficient space in directory service message
    for new string.

    CALL BY:	int as_set_ident(as,ident)
    		as_ptr	as;
    		byte	*ident;

    ON EXIT:	Returns SUCCESS if new string is stored, FAILURE if
		there is not enough remaining space in the directory
		service message for the new string.

	AS_ERROR | MAX_SPACE	indentification exceeds maxmimum size
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latas.h"
#include "laterr.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	lat_error;		/* LAT error code		*/

int as_set_ident(as,ident)

    as_ptr	as;			/* Pointer to new AS structure	*/
    byte	*ident;			/* Identification string	*/
 {
    int		dif;			/* Difference in string size	*/
    int		sts;			/* Status return		*/

/*-----------------------------------------------------------------------
    If the service is offered, check if enough room for new ident string.
------------------------------------------------------------------------*/
    dif = (int)(*ident - as->as_ident[0]);

    if ((as->as_flags & AS_OFFERED) && (dif > 0) && (hs->hs_as_space < (word)dif))
    {
	sts = FAILURE;
        lat_error = AS_ERROR | MAX_SPACE;
    }

/*-----------------------------------------------------------------------
    Store the new string and update the remaining space. Also flag
    a descriptor have changed.
------------------------------------------------------------------------*/
    else
    {
	sts = SUCCESS;
    	as_str_move_sl(&as->as_ident[0],ident);

	if (as->as_flags & AS_OFFERED)
	{
	    hs->hs_as_next_changes |= SVC_DESCRIPTOR;
	    hs->hs_as_space -= (word)dif;
	}
    }

    return(sts);
}
