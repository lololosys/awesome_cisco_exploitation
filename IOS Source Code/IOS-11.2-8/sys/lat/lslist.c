/* $Id: lslist.c,v 3.2 1995/11/17 17:34:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/lslist.c,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lslist.c,v $
 * Revision 3.2  1995/11/17  17:34:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:26:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    ls_list		(e) return next learned service structure

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function returns the next alphabetically greater learned service
    in the list of learned service names.  A null name (*name == 0) matches
    the first learned service.  If multiple services exist, the service with
    the highest rating is returned. Only services which match the port's IDL
    and the given node are returned. The flags may be used to exclude services
    offerred by remote nodes which are unknown or unreachable are returned.
    A null node, port ID (object pointer or CME cid), or flags is a don't
    care.

    CALL BY:	ls_ptr ls_list(name,node,port,flags)
    		byte	  *name;
		byte	  *node;
		lat_port  port;
		int	  flags;

    ON EXIT:	The address of the next alphabetically greater, highest
		rated learned service is returned.  If no service is found
		the function returns zero and sets lat_error.

    	LS_ERROR | NO_NEXT	No more matching services
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

ls_ptr ls_list(name,node,port,flags)

    byte	*name;			/* Learned service name         */
    byte	*node;			/* Remote node name		*/
    lat_port	port;			/* Port ID; 'ob' pointer or
						CME port ID		*/
    int		flags;			/* Node exclusion flags		*/
{
    ls_ptr	ls;			/* Pointer to LS structure	*/
    ls_ptr      ls_high;		/* Pointer to LS w/highest rate */
    int		ls_rate;		/* Storage for high rating      */
#if CME_SLAT
    cid_type	null_port = CID_NULL;	/* Null value definitions	*/
#else
    ob_ptr	null_port = 0;
#endif

/*-----------------------------------------------------------------------
    Scan through list of learned service structures looking for next
    learned service. Stop when we go beyond supplied name.
 -----------------------------------------------------------------------*/
    for (ls = hs->hs_ls_head[0]; ls != 0; ls = ls->ls_alpha[0]) {
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
#else
	else if (!idl_chk_group(null_port,ls->ls_nd)) {
	    ;
	}
#endif

	else if ((node != 0) &&
		 (*node != 0) &&
		 !nd_str_equal_sl(&ls->ls_nd->nd_node[0],node)) {
	    ;
	}
	else if (ls_str_diff_sl(&ls->ls_service[0],name) > 0) {
	    break;
	}
    }

/*-----------------------------------------------------------------------
    If a learned service was found, check to make sure that the occurance
    with the highest rating is returned.
 -----------------------------------------------------------------------*/
    if (ls != 0) {
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
#else
	    else if (!idl_chk_group(null_port,ls->ls_nd)) {
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
    If no learned service was found, set the error code before returning.
 -----------------------------------------------------------------------*/
    else
        lat_error = LS_ERROR | NO_NEXT;

    return(ls);
}
