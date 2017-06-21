/* $Id: lstmo.c,v 3.2 1995/11/17 17:34:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/lstmo.c,v $
 *------------------------------------------------------------------
 * $Log: lstmo.c,v $
 * Revision 3.2  1995/11/17  17:34:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:26:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    ls_timeout		(i) delete aged learned services

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function scans the remote nodes and deletes any for which we
    have not received a directory service message in some responible
    time. We do not delete a remote node to which we have an active
    circuit.

    CALL BY:	void ls_timeout()

    ON EXIT:	All aged learned nodes, services deleted.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latls.h"
#include "latnd.h"
#include "latfnc.h"

extern hs_ptr	hs;			/* Local node definitions	*/
extern int	tu_secs_toc;		/* Seconds per toc		*/
extern int	le_nd_retain;		/* Retain remote node		*/

void ls_timeout()

{
    nd_ptr	nd;			/* Pointer to nd structure	*/
    nd_ptr	nx;			/* Pointer to nd structure	*/
    int		i;			/* Do index			*/

/*-----------------------------------------------------------------------
    Loop through the hash table and check timer for each entry. Ignore
    remote nodes with active sessions.
 -----------------------------------------------------------------------*/
    for (i = 0; i < ND_HASHES; i++)
    {
	for (nd = hs->hs_nd_hash[i][0]; nd != 0; nd = nx)
	{
	    nx = nd->nd_link[0];

	    if (nd->nd_usage == -1)
	    {

/*-----------------------------------------------------------------------
    Count time for the remote node (up to -8 hours). On the transistion
    from positive to negative, flag the node as unknown. If the entry has
    aged to the maximum, delete the node and all of its services.
 -----------------------------------------------------------------------*/
		if (nd->nd_timer > (-ND_AGE_LIMIT))
 		{
		    nd->nd_timer -= tu_secs_toc;

		    if (nd->nd_timer <= 0 && nd->nd_timer > (-tu_secs_toc))
	                nd_fail(nd,ND_UNKNOWN);

#if LE_ND_RETAIN
		    else if (le_nd_retain && hs->hs_nd_age_max != 0 && nd->nd_timer < -hs->hs_nd_age_max)
		    {
		        while (nd->nd_ls[0] != 0)
			    ls_delete(nd->nd_ls[0]);

			nd_delete(nd);
		    }
#endif
		}
	    }
	}
    }
}
