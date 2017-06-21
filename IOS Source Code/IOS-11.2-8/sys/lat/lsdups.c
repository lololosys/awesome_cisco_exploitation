/* $Id: lsdups.c,v 3.2 1995/11/17 17:34:23 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/lsdups.c,v $
 *------------------------------------------------------------------
 * $Log: lsdups.c,v $
 * Revision 3.2  1995/11/17  17:34:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:26:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    ls_dups		(i) search ls alphabetic list for duplicate ls

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function searches the ls alphabetic list for a duplicate ls
    structure.  If there is no duplicate, or if the duplicate found
    has a lower rating than the new ls, return FAILURE.  Otherwise
    return SUCCESS.  Ignore any match in the hold list.

    CALL BY:	int ls_dups(name,rating);
		byte	*name;
		byte	rating;

    ON EXIT:	Return FAILURE if no duplicate, or a duplicate has a
		lower rating than the new ls, SUCCESS otherwise.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latls.h"
#include "latnd.h"
#include "latob.h"
#include "latfnc.h"

extern hs_ptr	hs;			/* Local node definitions	*/

int ls_dups(

    byte	*name,			/* Name of new learned service	*/
    byte	rating)			/* Rating of new service	*/
{
    ls_ptr	ls;			/* Pointer to duplicate LS	*/
    nd_ptr	nd;			/* Pointer to remote node	*/
    int		sts;			/* Return argument		*/
#if CME_SLAT
    cid_type	null_port = CID_NULL;	/* Value of the null port	*/
#else
    ob_ptr	null_port = 0;
#endif

/*-----------------------------------------------------------------------
    Scan through list of learned service structures looking for the service
    specified. Stop when we find the name or go beyond it alphabetically.
 ------------------------------------------------------------------------*/
    if ((ls = ls_show(name,0,null_port,0)) == 0)
        sts = FAILURE;

/*-----------------------------------------------------------------------
    Otherwise compare the ratings.  If the new rating is higher than
    the duplicate, delete the duplicate structure and return FAILURE.
    If the duplicate rating is equal to or higher, return SUCCESS.
 -----------------------------------------------------------------------*/
    else if ((ls->ls_nd->nd_flags & (ND_UNKNOWN | ND_UNREACH)) || rating > ls->ls_rating)
    {
	nd = ls->ls_nd;
        ls_delete(ls);
	nd_fail(nd,0);

	sts = FAILURE;
    }
    else
        sts = SUCCESS;

    return(sts);
}
