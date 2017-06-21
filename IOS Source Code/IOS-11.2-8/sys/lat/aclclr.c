/* $Id: aclclr.c,v 3.2 1995/11/17 17:31:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/aclclr.c,v $
 *------------------------------------------------------------------
 * $Log: aclclr.c,v $
 * Revision 3.2  1995/11/17  17:31:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:24:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:21:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    acl_clr_group	(e) clear acl group code(s)

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function clears the ACL group code specified by 'grp'. If the
    group code is -1, the function clears all groups. The function ignores
    an invalid group code number and flags service groups have changed so
    a new directory service sequence number will be generated.

    CALL BY:    void acl_clr_group(grp)
                int     grp;

    ON EXIT:    The ACL group code(s) is cleared and the changed group
    		flag is set.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latmsg.h"
#include "latfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

void acl_clr_group(grp)

    int         grp;			/* Bit position to be set       */
{
    int		len;			/* Current length               */

/*-----------------------------------------------------------------------
    If grp is between 0 and 255, calculate the current length, and
    clear the appropriate bit.
 -----------------------------------------------------------------------*/
    if ((grp >= 0) && (grp <= 255))
    {
	len = 1 + (grp / 8);
	hs->hs_acl_group[len] &= ~(1 << (grp % 8));

/*-----------------------------------------------------------------------
    If we clear a group in the last group byte, scan backwards to find
    first non-zero byte and update the length of the acl group field.
 -----------------------------------------------------------------------*/
    	if (len == hs->hs_acl_group[0])
    	{
    	    for (; len != 0; --len)
   	    {
    		if (hs->hs_acl_group[len] != 0)
    		    break;
    	    }
	    hs->hs_acl_group[0] = (byte)(LOBYTE(len));
    	}
    }

/*-----------------------------------------------------------------------
    If grp is -1, disable all acl groups.
 -----------------------------------------------------------------------*/
    else if (grp == -1)
        hs->hs_acl_group[0] = 0;

/*-----------------------------------------------------------------------
    Mark service groups have changed.
 -----------------------------------------------------------------------*/
    hs->hs_as_next_changes |= SVC_GROUPS;
}
