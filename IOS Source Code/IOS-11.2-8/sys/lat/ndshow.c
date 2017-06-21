/* $Id: ndshow.c,v 3.2 1995/11/17 17:34:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/ndshow.c,v $
 *------------------------------------------------------------------
 * $Log: ndshow.c,v $
 * Revision 3.2  1995/11/17  17:34:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:27:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    nd_show		(e) return named remote node structure

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function returns a pointer to the remote node structure
    specified by the given name.

    CALL BY:	nd_ptr nd_show(name)
    		byte	*name;

    ON EXIT:	Returns pointer to remote node or zero if no node
    		is found (sets lat_error).

	ND_ERROR | NO_NAME	Specified node name not found
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latnd.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	lat_error;		/* LAT error code		*/

nd_ptr nd_show(name,idb)
    byte	*name;			/* Learned service name         */
    idbtype	*idb;			/* Interface			*/
{
    nd_ptr	nd;			/* pointer to returning arg	*/
    int		hash;			/* Index into hash table	*/
    int		i;			/* loop counter			*/

/*-----------------------------------------------------------------------
    Generate the index into the node hash table.  Lookup the node name
    to see if it already exists.  If so, return that node pointer.
 -----------------------------------------------------------------------*/
    for (i = name[0], hash = 0; i != 0; i--)
        hash += name[i];
    hash = nd_hash(hash);

    for (nd = hs->hs_nd_hash[hash][0]; nd != 0; nd = nd->nd_link[0])
    {
        if ((nd_str_equal_sl(&nd->nd_node[0],name) && 
	    ((idb == NULL) || (nd->nd_idb == idb))))
	    break;
    }    

/*-----------------------------------------------------------------------
    If no remote node structure was found, set the error code before returning.
 -----------------------------------------------------------------------*/
    if (nd == 0)
	lat_error = ND_ERROR | NO_NAME;

    return(nd);
}
