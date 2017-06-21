/* $Id: lsscan.c,v 3.2 1995/11/17 17:34:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/lsscan.c,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lsscan.c,v $
 * Revision 3.2  1995/11/17  17:34:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:26:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    ls_scan		(e) return next learned service structure w/duplicates

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function returns the next alphabetically greater learned service
    in the list of learned service names if the sequence argument is zero.
    A null name (name == 0 or *name == 0) matches the first learned service.
    Only services which match the port's IDL are returned. Services are
    returned regardless of the remote node status.

    The function may be used to return duplicate services by calling with
    a non-zero incrementing sequence arguement. When all duplicates have
    been found, the function returns error.

    CALL BY:	ls_ptr ls_list(name,port,seq)
    		byte	  *name;
		lat_port  port;
		int	  seq;

    ON EXIT:	If (seq == 0), the address of the next alphabetically
    		greater service is returned. If no more services are
		found, the function returns zero and sets lat_error.

		If (seq != 0), the address of the (seq)th duplicate
		name is returned. If no more duplicates, the function
		returns zero and sets lat_error.

	LS_ERROR | NO_NEXT	No more services match supplied values
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

ls_ptr ls_scan(name,port,seq)

    byte	*name;			/* Learned service name         */
    lat_port	port;			/* Port ID; 'ob' or 'cid'	*/
    int		seq;			/* Duplicate sequence number	*/
{
    ls_ptr	ls;			/* Pointer to LS structure	*/
    int		alpha;			/* Alphabetic ordering		*/
#if LE_OBJECTS
    ob_ptr	null_port = 0;		/* Null value definitions	*/
#elif CME_SLAT
    cid_type	null_port = CID_NULL;
#endif


/*-----------------------------------------------------------------------
    If no name specified, start with first entry.
 -----------------------------------------------------------------------*/
    if (name == 0 || *name == 0)
        ls = hs->hs_ls_head[0];

/*-----------------------------------------------------------------------
    Scan through list of learned service structures looking for the next
    learned service. Stop when we go beyond specified name.
 -----------------------------------------------------------------------*/
    else if (seq == 0)
    {
        for (ls = hs->hs_ls_head[0]; ls != 0; ls = ls->ls_alpha[0])
        {
	    if (ls->ls_nd == 0) {
	        ;
	    }
#if (LE_OBJECTS || CME_SLAT)
	    else if (port != null_port && !idl_chk_group(port,ls->ls_nd)) {
	        ;
	    }
#endif

	    else if (ls_str_diff_sl(&ls->ls_service[0],name) > 0) {
	        break;
	    }
	}
    }

/*-----------------------------------------------------------------------
    Otherwise, scan the list of learned services for a match on the
    supplied name and go to the next sequence.
 -----------------------------------------------------------------------*/
    else
    {
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
#if (LE_OBJECTS || CME_SLAT)
	        else if (port != null_port && !idl_chk_group(port,ls->ls_nd))
	            ;
#endif

	        else if (--seq == 0)
	            break;
	    }
	}
    }

/*-----------------------------------------------------------------------
    If no learned service was found, set the error code before returning.
 -----------------------------------------------------------------------*/
    if (ls == 0)
        lat_error = LS_ERROR | NO_NEXT;

    return(ls);
}
