/* $Id: aclcmp.c,v 3.2 1995/11/17 17:31:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/aclcmp.c,v $
 *------------------------------------------------------------------
 * $Log: aclcmp.c,v $
 * Revision 3.2  1995/11/17  17:31:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:24:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:21:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    acl_cmp_group	(i) compare input message group to acl

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function compares an input message group field with the local
    ACL groups and returns SUCCESS if any match, FAILURE otherwise. The
    function will always fail if the input message group exceeds the
    maximum allowed length. This is a protection against ill-formed
    LAT protocol messages.  If both group lengths are zero, SUCCESS is 
    returned (LAT V5.2 protocol spec).

    CALL BY:    int acl_cmp_group(grp)
                if_byte	grp;

    ON EXIT:    Returns SUCCESS if any bit match between the input
    		group field and hs->hs_acl_group, FAILURE otherwise.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

int acl_cmp_group(grp)

    if_byte	grp;			/* Pointer to group byte string */
{
    int		end;			/* Minimum length of groups	*/

/*-----------------------------------------------------------------------
    If both group lengths are zero, set up to return SUCCESS.
    NOTE: A non-zero value of 'end' implies success.
 -----------------------------------------------------------------------*/
    if (*grp == 0 && hs->hs_acl_group[0] == 0)
	end = -1;

/*-----------------------------------------------------------------------
    Get the minimum length of the two group fields. If the input
    message field exceeds the legal limit, set the length to zero
    so the routine fails.
 -----------------------------------------------------------------------*/
    else if (*grp >= GROUPS)
        end = 0;
    else
        end = (*grp < hs->hs_acl_group[0]) ? *grp : hs->hs_acl_group[0];

/*-----------------------------------------------------------------------
    While any bytes remain, check for any matching group masks.
 -----------------------------------------------------------------------*/
    for (; end > 0; end--)
    {
	if (grp[end] & hs->hs_acl_group[end])
	    break;
    }
    return( (end != 0) ? SUCCESS : FAILURE);
}
