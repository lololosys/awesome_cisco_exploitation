/* $Id: lsshow.c,v 3.2 1995/11/17 17:34:36 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/lsshow.c,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lsshow.c,v $
 * Revision 3.2  1995/11/17  17:34:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:26:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*----------------------------------------------------------------------
    ls_show		(e) find learned service by name

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function returns a pointer to the learned service structure
    specified by  the supplied name. The function only return a name
    which matches the supplied objects IDL and remote node name. The
    flags field may be used to exclude services offered by unreachable
    and/or unknown remote nodes.  A null port ID, node name pointer
    (name == 0 or *name == 0) or flags is a don't care.

    CALL BY:	ls_ptr ls_show(name,node,port,flags)
    		byte	  *name;
		byte	  *node;
		lat_port  port;
		int	  flags;

    ON EXIT:	The return argument points to the learned service structure.
    		If no learned service is found, the function returns zero
		and sets lat_error. If there are multiple occurances of a
		learned service, the service with the highest rating is
		returned.

	LS_ERROR | NO_NAME	No service matching supplied values
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latls.h"
#include "latnd.h"
#include "latob.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	lat_error;		/* LAT error code		*/

ls_ptr ls_show(name,node,port,flags)

    byte	*name;			/* Name of LS structure		*/
    byte	*node;			/* Remote node name		*/
    lat_port	port;			/* Port ID; 'ob' pointer or
						CME 'cid		*/
    int		flags;			/* Node exclusion flags		*/
{
    ls_ptr	ls;			/* Return arguement pointer	*/
    ls_ptr      ls_high;		/* Storage for highest LS	*/
    int	        ls_rate;		/* Highest rating found		*/
    int		alpha;			/* Current alphabetic order	*/
#if LE_OBJECTS
    ob_ptr	null_port = 0;		/* Null value definitions	*/
#elif CME_SLAT
    cid_type	null_port = CID_NULL;
#endif

/*-----------------------------------------------------------------------
    Scan through list of learned service structures looking for the service
    specified. Stop when we find the name or go beyond it alphabetically.
 -----------------------------------------------------------------------*/
    for (ls = hs->hs_ls_head[0]; ls != 0; ls = ls->ls_alpha[0])
    {
	alpha = ls_str_diff_sl(&ls->ls_service[0],name);

	if (alpha > 0)
	{
	    ls = 0;
	    break;
	}

	else if (alpha == 0)
	{
	    if (ls->ls_nd == 0) {
	        ;
	    }
	    else if (ls->ls_nd->nd_flags & flags) {
	        ;
	    }
#if (LE_OBJECTS || CME_SLAT)
	    else if (port != null_port && !idl_chk_group(port,ls->ls_nd)) {
	        ;
	    }
#endif

	    else if ((node != 0) &&
		     (*node != 0) &&
		     !nd_str_equal_sl(&ls->ls_nd->nd_node[0],node)) {
	        ;
	    }

	    else
	        break;
	}
    }

/*-----------------------------------------------------------------------
    If a learned service was found, check to make sure that the occurance
    with the highest rating is returned.
 -----------------------------------------------------------------------*/
    if (ls != 0)
    {
	ls_rate = ls->ls_rating;
	ls_high = ls;

	while ((ls = ls->ls_alpha[0]) != 0 &&
	       ls_str_equal_ss(&ls->ls_service[0],&ls_high->ls_service[0])) {
	    if (ls->ls_nd == 0) {
	        ;
	    }
	    else if (ls->ls_nd->nd_flags & flags) {
	        ;
	    }
#if (LE_OBJECTS || CME_SLAT)
	    else if (port != null_port && !idl_chk_group(port,ls->ls_nd)) {
	        ;
	    }
#endif

	    else if ((node != 0) &&
		     (*node != 0) &&
		     !nd_str_equal_sl(&ls->ls_nd->nd_node[0],node)) {
	        ;
	    }
	    else if (ls_rate < ls->ls_rating) {
		ls_rate = ls->ls_rating;
		ls_high = ls;
	    }
	}
	ls = ls_high;

    }

/*-----------------------------------------------------------------------
    If the learned service was not found, set the error code before returning.
 -----------------------------------------------------------------------*/
    else
	lat_error = LS_ERROR | NO_NAME;

    return(ls);
}
