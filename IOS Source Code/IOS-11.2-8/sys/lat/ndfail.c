/* $Id: ndfail.c,v 3.2 1995/11/17 17:34:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/ndfail.c,v $
 *------------------------------------------------------------------
 * $Log: ndfail.c,v $
 * Revision 3.2  1995/11/17  17:34:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:27:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    nd_fail		(i) process node usage
    
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO
    
    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved. 
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called whenever node usage changes such that the
    remote node structure may no longer be needed: no more services,
    no more circuits, or change in remote node status. There are four
    courses of action:
    
    LE_ND_RETAIN = 1, error  = 0 No action
    LE_ND_RETAIN = 1, error != 0 Set error status in node, services
    LE_ND_RETAIN = 0, error  = 0 Delete node if no usage, services
    LE_ND_RETAIN = 0, error != 0 Delete node if no usage, services

    CALL BY:	void nd_fail(nd,error)
		nd_ptr	nd;
		int	error;
 
    ON EXIT:	Node/services handled according to above table.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latls.h"
#include "latnd.h"
#include "latfnc.h"

extern	int	le_nd_retain;		/* Retain remote nodes		*/

void nd_fail(nd,error)

    nd_ptr	nd;			/* Pointer to ND structure	*/
    int		error;			/* Node failure reason		*/
{
/*-----------------------------------------------------------------------
    If error set error status in node structure and set time to -1.
 -----------------------------------------------------------------------*/
    if (error != 0)
    {
        nd->nd_flags |= (byte)(LOBYTE(error));
	nd->nd_timer  = -1;

#if LE_SERVER_LS
/*-----------------------------------------------------------------------
    If non-zero error and not retaining nodes, loop through services
    and delete.
 -----------------------------------------------------------------------*/
	if (!le_nd_retain)
	{
	    while (nd->nd_ls[0] != 0)
		ls_delete(nd->nd_ls[0]);
	}
#endif
    }

/*-----------------------------------------------------------------------
    If not retaining nodes, see if we can delete the remote node
    structure.
 -----------------------------------------------------------------------*/
    if (!le_nd_retain && nd->nd_usage == -1 && nd->nd_ls[0] == 0)
        nd_delete(nd);
}
