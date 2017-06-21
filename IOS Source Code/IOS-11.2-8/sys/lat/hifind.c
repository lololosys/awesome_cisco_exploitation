/* $Id: hifind.c,v 3.2 1995/11/17 17:32:43 hampton Exp $ 
 * $Source: /swtal/cherf.111/ios/sys/lat/hifind.c,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: hifind.c,v $
 * Revision 3.2  1995/11/17  17:32:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:23:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hi_find		(e/i) find hic structure which matches entry
    
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function returns a pointer to the hic structure specified by
    the given entry identifier. Use zero for don't care conditions for
    all arguments but system type.

    CALL BY:	hi_ptr hi_find(req,que,system,node)
		word	 req;
		int	 system;

    ON EXIT:	The return argument points to the HIC entry found.  If no
    		entry is found, the function returns zero and sets lat_error.

	HI_ERROR | NO_IDENT	No hi entry matches the supplied entry number.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "lathi.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern hs_ptr	hs;			/* Local node definitions	*/
extern int	lat_error;		/* LAT error codes		*/

hi_ptr hi_find(
    word	req,			/* The HIC entry identifier	*/
    word	que,			/* The HIC queue identifier	*/
    int		system,			/* Host/server system		*/
    byte	*node)			/* Remote node name		*/
{
    hi_ptr	hi;			/* Pointer to HIC block		*/

/*-----------------------------------------------------------------------
    Look through all the HIC structures to find the entry which has
    the entry.  Break when a match is found.
 -----------------------------------------------------------------------*/
    for (hi = hs->hs_hi_head[0]; hi != 0; hi = hi->hi_link[0])
    {
	if (!(hi->hi_flags & system)) {
	    ;

	} else if (req != 0 && hi->hi_req_no != req) {
	    ;

	} else if (que != 0 && hi->hi_que_no != que) {
	    ;

	} else if (node != 0 && !hi_str_equal_sl(&hi->hi_node[0],node)) {
	    ;

	} else {
	    break;
	}
    }

/*-----------------------------------------------------------------------
    If no entry found, set error code before exiting.
 -----------------------------------------------------------------------*/
    if (hi == 0)
	lat_error = HI_ERROR | NO_IDENT;

    return(hi);
}
