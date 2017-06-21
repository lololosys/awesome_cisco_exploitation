/*-----------------------------------------------------------------------
    nd_list		(e) return next remote node structure

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function returns the next alphabetically greater remote node
    in the list of remote node names.  A null name (*name == 0) matches
    the first alphabetic name.

    CALL BY:	nd_ptr nd_list(name)
    		byte	*name;

    ON EXIT:	The address of the next alphabetically greater, remote
		node is returned.  If no node is found the function
		returns zero (sets lat_error).

	ND_ERROR | NO_NEXT	No more remote nodes
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latnd.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	lat_error;		/* LAT error code		*/

nd_ptr nd_list(name)

    byte	*name;			/* Learned service name         */
{
    nd_ptr	nd;			/* pointer to returning arg	*/
    nd_ptr      nx;			/* pointer to next node		*/
    int		i;			/* loop counter			*/

/*-----------------------------------------------------------------------
    The remote nodes are not kept in alphabetical order, so completely
    scan the known nodes and look for the name closest to the supplied
    name.
 -----------------------------------------------------------------------*/
    for (i = 0, nd = 0; i < ND_HASHES; i++)
    {
	for (nx = hs->hs_nd_hash[i][0]; nx != 0; nx = nx->nd_link[0])
	{
	    if (nd == 0)
	    {
		if (nd_str_diff_sl(&nx->nd_node[0],name) > 0)
		    nd = nx;
	    }
	    else
	    {
		if ((nd_str_diff_sl(&nx->nd_node[0],name)            > 0) &&
		    (nd_str_diff_ss(&nx->nd_node[0],&nd->nd_node[0]) < 0))
		{
		    nd = nx;
		}
	    }
	}
    }

/*-----------------------------------------------------------------------
    If no remote node structure was found, set the error code.
 -----------------------------------------------------------------------*/
    if (nd == 0)
	lat_error = ND_ERROR | NO_NEXT;

    return(nd);
}
