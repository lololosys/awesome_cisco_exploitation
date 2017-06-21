/* $Id: aclchk.c,v 3.2 1995/11/17 17:31:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/aclchk.c,v $
 *------------------------------------------------------------------
 * $Log: aclchk.c,v $
 * Revision 3.2  1995/11/17  17:31:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:24:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:21:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    acl_chk_group	(i) compare input message group to a node
    			    acl

    Copyright 1991 Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function compares an input message group field with a node's
    ACL groups and returns SUCCESS if any match, FAILURE otherwise. The
    function will always fail if the input message group exceeds the
    maximum allowed length. This is a protection against ill-formed
    LAT protocol messages.  If both group lengths are zero, SUCCESS is 
    returned (LAT V5.2 protocol spec).

    CALL BY:    int acl_chk_group(nd,grp)
    		nd_ptr  nd;
                if_byte	grp;

    ON EXIT:    Returns SUCCESS if any bit match between the input
    		group field and hs->hs_acl_group, FAILURE otherwise.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latnd.h"
#include "latfnc.h"

int acl_chk_group(nd,grp)

    nd_ptr	nd;			/* Pointer to node structure	*/
    if_byte	grp;			/* Pointer to group byte string */
{
    int		end;			/* Minimum length of groups	*/

/*-----------------------------------------------------------------------
    If both group lengths are zero, set up to return SUCCESS.
    NOTE: A non-zero value of 'end' implies success.
 -----------------------------------------------------------------------*/
    if (*grp == 0 && nd->nd_acl[0] == 0)
	end = -1;

/*-----------------------------------------------------------------------
    Get the minimum length of the two group fields. If the input
    message field exceeds the legal limit, set the length to zero
    so the routine fails.
 -----------------------------------------------------------------------*/
    else if (*grp >= GROUPS)
        end = 0;
    else
        end = (*grp < nd->nd_acl[0]) ? *grp : nd->nd_acl[0];

/*-----------------------------------------------------------------------
    While any bytes remain, check for any matching group masks.
 -----------------------------------------------------------------------*/
    for (; end > 0; end--)
    {
	if (grp[end] & nd->nd_acl[end])
	    break;
    }
    return( (end != 0) ? SUCCESS : FAILURE);
}
