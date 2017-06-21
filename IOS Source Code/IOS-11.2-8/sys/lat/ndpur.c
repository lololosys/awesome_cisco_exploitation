/* $Id: ndpur.c,v 3.2 1995/11/17 17:34:46 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/ndpur.c,v $
 *------------------------------------------------------------------
 * $Log: ndpur.c,v $
 * Revision 3.2  1995/11/17  17:34:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:27:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    nd_purge		(e/i) purge aged remote nodes, services

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function deletes remote nodes which have aged beyond the
    supplied time and have no current usage. The LAT engine keeps
    track of unreachable or unknown nodes using a negative time. For
    such entries, the learned services are deleted.

    This function may be used by environments whenever it is desirable
    to delete unreachable or unknown nodes (call with a time of 0). The
    function is used internally whenever a nd or ls structure allocation
    fails to free space for the new information.
    
    CALL BY:	int nd_purge(time);
    		int	time;

    ON EXIT:	Returns SUCCESS if either a learned service or a
    		remote node is deleted, FAILURE otherwise.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latls.h"
#include "latnd.h"
#include "latfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

int nd_purge(time)

    int		time;			/* Purge entries old than time	*/
{
    nd_ptr	nd;			/* pointer to returning arg	*/
    nd_ptr      nx;			/* pointer to next node		*/
    int		sts;			/* Status return		*/
    int		i;			/* loop counter			*/

/*-----------------------------------------------------------------------
    Scan through list of remote node structures.
 -----------------------------------------------------------------------*/
    sts = FAILURE;
    
    for (i = 0; i < ND_HASHES; i++)
    {
	for (nd = hs->hs_nd_hash[i][0]; nd != 0; nd = nx)
	{
	    nx = nd->nd_link[0];
	    
/*-----------------------------------------------------------------------
    If age of structure is older than specified time, try to purge.
 -----------------------------------------------------------------------*/
	    if (nd->nd_timer < time)
	    {

#if LE_SERVER_LS
/*-----------------------------------------------------------------------
    If time is negative, remote node is unknown or unreachable. Delete
    its learned services.
 -----------------------------------------------------------------------*/
	        if (nd->nd_timer < 0)
		{
		    while (nd->nd_ls[0] != 0)
		    {
		        sts = SUCCESS;
		        ls_delete(nd->nd_ls[0]);
		    }
		}
#endif
		
/*-----------------------------------------------------------------------
    If remote node has no sessions and services, delete it.
 -----------------------------------------------------------------------*/
		if (nd->nd_usage == -1 && nd->nd_ls[0] == 0)
		{
		    sts = SUCCESS;
		    nd_delete(nd);
		}
	    }
	}
    }
    return(sts);
}
