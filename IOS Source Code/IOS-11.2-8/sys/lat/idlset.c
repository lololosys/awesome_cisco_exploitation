/* $Id: idlset.c,v 3.2 1995/11/17 17:33:17 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/idlset.c,v $
 *------------------------------------------------------------------
 * $Log: idlset.c,v $
 * Revision 3.2  1995/11/17  17:33:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:24:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    idl_set_group	(e) set idl group codes

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function sets an IDL group code. If the group code is -1, the
    function sets all groups. An invalid group code number is ignored.
    If the port ID is null, set the group in all ports.  The function
    maintains hs_idl_group as logical OR of all the port groups.

    When using objects, the port ID is an object pointer (ob).

    When using CME, the port ID in the calling arguments is the 'cid' of
    the CM port and 'cid_type' must be an integer in the range of 0 to
    (cm_port_maximum-1).

    If objects or CME is not supported, the function operates only on
    the master idl group (hs_idl_group).

    CALL BY:    void idl_set_groupt(port,grp)
    		lat_port  port;
                int       grp;

    ON EXIT:    The group code is set.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latob.h"
#include "latfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	ob_ptr	ob_head[2];		/* Object list header		*/
#if CME_SLAT
extern	int	cm_port_maximum;	/* Maximum number of CME ports	*/
#else
typedef	byte	OB_VIEW(grp_ptr);
#endif

void idl_set_group(port,grp)

    lat_port	port;			/* Port ID; 'ob' or 'cid'	*/
    int         grp;			/* Bit position to be set       */
{
    int		len;			/* Current length               */
#if (LE_OBJECTS || CME_SLAT)
    grp_ptr	gp;			/* Pointer to a group field	*/
#if LE_OBJECTS
    ob_ptr	ob;			/* Pointer to an object		*/
#elif CME_SLAT
    int		j;			/* CME port identifier		*/
#endif
#endif

#if (LE_OBJECTS || CME_SLAT)
/*-----------------------------------------------------------------------
    If no port specified, loop through port structures and set group.
 -----------------------------------------------------------------------*/
#if LE_OBJECTS
    if (port == 0)
    {
        for (ob = ob_head[0]; ob != 0; ob = ob->ob_link[0])
	    idl_set_group(ob,grp);
    }
#elif CME_SLAT
    if (port == CID_NULL)
    {
	for (j = 0; j < cm_port_maximum; j++)
	    idl_set_group(j,grp);
    }
#endif

/*-----------------------------------------------------------------------
    If grp is between 0 and 255, calculate the current length, and
    set the appropriate bit. If the position exceeds the current
    field length, zero-extend the group field.
 -----------------------------------------------------------------------*/
    else if ((grp >= 0) && (grp <= 255))
    {
	len = 1 + (grp / 8);
#if LE_OBJECTS
	gp = (grp_ptr)&port->ob_idl[0];
#elif CME_SLAT
	gp = cx_group(port);
#endif
        while (gp[0] < ((byte)(LOBYTE(len))))
            gp[++(gp[0])] = 0;

        gp[len] |= (1 << (grp % 8));

        while (hs->hs_idl_group[0] < ((byte)(LOBYTE(len))))
            hs->hs_idl_group[++(hs->hs_idl_group[0])] = 0;

        hs->hs_idl_group[len] |= (1 << (grp % 8));
    }

/*-----------------------------------------------------------------------
    If grp is -1, enable all groups.
 -----------------------------------------------------------------------*/
    else if (grp == -1)
    {
#if LE_OBJECTS
	gp = (grp_ptr)&port->ob_idl[0];
#elif CME_SLAT
	gp = cx_group(port);
#endif
	gp[0] = hs->hs_idl_group[0] = 0;
        while (gp[0] < GROUPS-1)
	{
            gp[++(gp[0])] = 0xff;
            hs->hs_idl_group[++(hs->hs_idl_group[0])] = 0xff;
	}
    }
#else

/*-----------------------------------------------------------------------
    If grp is between 0 and 255, calculate the current length, and
    set the appropriate bit.
 -----------------------------------------------------------------------*/
    if ((grp >= 0) && (grp <= 255))
    {
	len = 1 + (grp / 8);
        while (hs->hs_idl_group[0] < (byte)(LOBYTE(len)))
            hs->hs_idl_group[++(hs->hs_idl_group[0])] = 0;

        hs->hs_idl_group[len] |= (1 << (grp % 8));
    }

/*-----------------------------------------------------------------------
    If grp is -1, enable all groups.
 -----------------------------------------------------------------------*/
    else if (grp == -1)
    {
	hs->hs_idl_group[0] = 0;
        while (hs->hs_idl_group[0] < GROUPS-1)
            hs->hs_idl_group[++(hs->hs_idl_group[0])] = 0xff;
    }
#endif
}
