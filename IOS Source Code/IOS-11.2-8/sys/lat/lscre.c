/* $Id: lscre.c,v 3.2 1995/11/17 17:34:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/lscre.c,v $
 *------------------------------------------------------------------
 * $Log: lscre.c,v $
 * Revision 3.2  1995/11/17  17:34:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:26:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    ls_create		(i) create learned service structure

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function creates a learned service structure and links to
    the alphabetic and node list.

    CALL BY:	ls_ptr ls_create(name,nd)
    		byte	*name;
		nd_ptr	nd;

    ON EXIT:	The learned service structure is created. If any
		error occurs, the function returns zero and sets
		lat_error as follows:

	LS_ERROR | MAX_SPACE	Exceeded limit of ls structures
	LS_ERROR | NO_MEM	Failed to allocate ls structure
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latls.h"
#include "latnd.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	lat_error;		/* LAT error code		*/
extern	int	le_nd_retain;		/* Retain remote node support	*/

ls_ptr ls_create(name,nd)

    byte	*name;			/* Service name			*/
    nd_ptr	nd;			/* Pointer to remote node	*/
 {
    ls_ptr	ls;			/* Pointer to new LS structure	*/
    ls_ptr	ps;		        /* Pointer to previous LS	*/

/*-----------------------------------------------------------------------
    Initialize ls to zero.  If all tests fail the learned service structure
    is not created, and the return argument is zero.
 -----------------------------------------------------------------------*/
    ls = 0;

    if ((hs->hs_ls_maximum > 0 && hs->hs_ls_current >= hs->hs_ls_maximum) ||
	(ls = ls_alloc()) == 0)
    {
#if LE_ND_RETAIN
	if (le_nd_retain)
	{
	    int  i;

	    for (i = -hs->hs_nd_age_max; i < LS_MC_DEF; i += hs->hs_nd_interval)
	    {
		if (nd_purge(i))
		{
		    if ((hs->hs_ls_maximum == 0 || hs->hs_ls_current < hs->hs_ls_maximum) &&
			(ls = ls_alloc()) != 0)
		    {
			break;
		    }
		}
	    }
	}
#endif
    }

/*-----------------------------------------------------------------------
    If no service was allocated, determine specific error code.
 -----------------------------------------------------------------------*/
    if (ls == 0)
    {
        if (hs->hs_ls_maximum > 0 && hs->hs_ls_current >= hs->hs_ls_maximum)
	    lat_error = LS_ERROR | MAX_LIMIT;
	else
	    lat_error = LS_ERROR | NO_MEM;
     }

/*-----------------------------------------------------------------------
    Initialize the service.
------------------------------------------------------------------------*/
    else
    {
	if (++hs->hs_ls_current > hs->hs_ls_highest)
	    hs->hs_ls_highest++;

/*-----------------------------------------------------------------------
    Scan for the correct place to insert the learned service structure.
 -----------------------------------------------------------------------*/
        for (ps = hs->hs_ls_head[0]; ps != 0; ps = ps->ls_alpha[0])
        {
	    if ((ls_str_diff_sl(&ps->ls_service[0],name)) > 0)
    	        break;
        }

/*------------------------------------------------------------------------
    Now put the learned service structure into the learned service linked
    list, in the alphabetically correct place.
-------------------------------------------------------------------------*/
	ls->ls_alpha[0] = ps;

    	if (ps != 0)
    	{
    	    ls->ls_alpha[1] = ps->ls_alpha[1];
    	    ps->ls_alpha[1] = ls;
    	}
    	else
    	{
    	    ls->ls_alpha[1] = hs->hs_ls_head[1];
    	    hs->hs_ls_head[1] = ls;
    	}

    	if (ls->ls_alpha[1] != 0)
    	    ls->ls_alpha[1]->ls_alpha[0] = ls;
    	else
    	    hs->hs_ls_head[0] = ls;

/*-----------------------------------------------------------------------
    Set ls_nd to point to the node given.  Insert the learned service into
    the learned service list for the node given.
 -----------------------------------------------------------------------*/
	ls->ls_nd = nd;

    	ls->ls_host[0] = 0;
    	ls->ls_host[1] = nd->nd_ls[1];
    	nd->nd_ls[1] = ls;
    	if (ls->ls_host[1] != 0)
    	    ls->ls_host[1]->ls_host[0] = ls;
    	else
    	    nd->nd_ls[0] = ls;

/*-----------------------------------------------------------------------
    Initialize the rest of the ls structure.
------------------------------------------------------------------------*/
	ls->ls_flags    = 0;
	ls->ls_rating   = 0;
	ls->ls_ident[0] = 0;
    	ls_str_move_sl(&ls->ls_service[0],name);
    }
    return(ls);
}
