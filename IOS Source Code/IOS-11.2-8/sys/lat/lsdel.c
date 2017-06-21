/* $Id: lsdel.c,v 3.2 1995/11/17 17:34:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/lsdel.c,v $
 *------------------------------------------------------------------
 * $Log: lsdel.c,v $
 * Revision 3.2  1995/11/17  17:34:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:26:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    ls_delete		(e) delete the learned service structure

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function deletes the learned service structure.  If the associated
    nd_node structure has no other names and no outstanding circuits, it is
    also deleted.  This function does not affect existing connections.

    CALL BY:	void ls_delete(ls)
    		ls_ptr     ls;

    ON EXIT:	The learned service, and possibly the associated nd_node
		structure, is deleted.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latls.h"
#include "latnd.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	ls_ptr	ls_hold[2];		/* LS holding structure header	*/
extern	int	ls_hold_num;		/* Number of services held	*/
extern	nd_ptr	nd_hold[2];		/* ND holding structure header	*/
extern	int	nd_hold_num;		/* Number of remote nodes held	*/

void ls_delete(ls)

    ls_ptr	ls;                     /* Pointer to LS structure	*/
{
    nd_ptr	nd;      		/* Pointer to ND structure	*/

/*-----------------------------------------------------------------------
    Set nd to ls->ls_nd for clarity.
 -----------------------------------------------------------------------*/
    nd = ls->ls_nd;

/*-----------------------------------------------------------------------
    Delete from alphabetic list if we are deleting from hold list (nd == 0)
    or a non-permanent service.
 -----------------------------------------------------------------------*/
    if (nd == 0 || !(ls->ls_flags & LS_PERM))
    {
	if (ls->ls_alpha[0] != 0)
	    ls->ls_alpha[0]->ls_alpha[1] = ls->ls_alpha[1];
	else
	    hs->hs_ls_head[1] = ls->ls_alpha[1];

	if (ls->ls_alpha[1] != 0)
	    ls->ls_alpha[1]->ls_alpha[0] = ls->ls_alpha[0];
	else
	    hs->hs_ls_head[0] = ls->ls_alpha[0];
    }

/*-----------------------------------------------------------------------
    If the nd is not zero (the service is attached to the node linked list),
    then delete the node from the node linked list.
 -----------------------------------------------------------------------*/
    if (nd != 0)
    {
	nd->nd_flags |= ND_INIT;
	
	if (ls->ls_host[0] != 0)
	    ls->ls_host[0]->ls_host[1] = ls->ls_host[1];
	else
	    nd->nd_ls[1] = ls->ls_host[1];

	if (ls->ls_host[1] != 0)
	    ls->ls_host[1]->ls_host[0] = ls->ls_host[0];
 	else
	    nd->nd_ls[0] = ls->ls_host[0];

/*-----------------------------------------------------------------------
    Check to see if the learned service is permanent. If so, set the node
    pointer to zero and deallocate ls to the LS_HOLD structure.
 -----------------------------------------------------------------------*/
	if (ls->ls_flags & LS_PERM)
	{
	    ls_hold_num++;

	    ls->ls_nd     = 0;
    	    ls->ls_rating = 0;

	    ls->ls_host[0] = 0;
	    ls->ls_host[1] = ls_hold[1];
	    ls_hold[1] = ls;
	    if (ls->ls_host[1] != 0)
	        ls->ls_host[1]->ls_host[0] = ls;
	    else
	        ls_hold[0] = ls;
	}

/*-----------------------------------------------------------------------
    Otherwise delete service to free pool.
 -----------------------------------------------------------------------*/
  	else
	{
	    hs->hs_ls_current--;
	    ls_free(ls);
	}
    }

/*-----------------------------------------------------------------------
    If there is no remote node, remove the ls structure from the ls_hold
    linked list. Balance the number of remote nodes in hold list.
 -----------------------------------------------------------------------*/
    if (nd == 0)
    {
        ls_hold_num--;

        if (ls->ls_host[0] != 0)
	    ls->ls_host[0]->ls_host[1] = ls->ls_host[1];
	else
	    ls_hold[1] = ls->ls_host[1];

	if (ls->ls_host[1] != 0)
	    ls->ls_host[1]->ls_host[0] = ls->ls_host[0];
	else
	    ls_hold[0] = ls->ls_host[0];

	hs->hs_ls_current--;
	ls_free(ls);

	while (ls_hold_num > nd_hold_num)
	    nd_delete(nd_hold[0]);
    }
}
