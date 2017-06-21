/* $Id: aclset.c,v 3.2 1995/11/17 17:31:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/aclset.c,v $
 *------------------------------------------------------------------
 * $Log: aclset.c,v $
 * Revision 3.2  1995/11/17  17:31:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:24:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:21:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    acl_set_group	(e) set acl group code(s)

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function sets the ACL group code specified by 'grp'.. If the
    group code is -1, the function sets all ACL groups. The function
    ignores an invalid group code number and flags service groups have
    changed so a new directory service sequence number will be generated.

    CALL BY:    void acl_set_groupt(grp)
                int     grp;

    ON EXIT:    The group code set and the changed group flag set.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latmsg.h"
#include "latfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

void acl_set_group(grp)

    int         grp;			/* Bit position to be set       */
{
    int		len;			/* Current length               */

/*-----------------------------------------------------------------------
    If grp is between 0 and 255, calculate the current length, and
    set the appropriate bit. If the position exceeds the current
    field length, zero-extend the group field.
 -----------------------------------------------------------------------*/
    if ((grp >= 0) && (grp <= 255))
    {
	len = 1 + (grp / 8);
        while (hs->hs_acl_group[0] < (byte)(LOBYTE(len)))
            hs->hs_acl_group[++(hs->hs_acl_group[0])] = 0;

        hs->hs_acl_group[len] |= (1 << (grp % 8));
    }

/*-----------------------------------------------------------------------
    If grp is -1, enable all groups.
 -----------------------------------------------------------------------*/
    else if (grp == -1)
    {
	hs->hs_acl_group[0] = 0;

        while (hs->hs_acl_group[0] < GROUPS-1)
            hs->hs_acl_group[++(hs->hs_acl_group[0])] = 0xff;
    }

/*-----------------------------------------------------------------------
    Mark service groups have changed.
 -----------------------------------------------------------------------*/
    hs->hs_as_next_changes |= SVC_GROUPS;
}
