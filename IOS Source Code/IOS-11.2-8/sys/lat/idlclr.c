/* $Id: idlclr.c,v 3.2 1995/11/17 17:33:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/idlclr.c,v $
 *------------------------------------------------------------------
 * $Log: idlclr.c,v $
 * Revision 3.2  1995/11/17  17:33:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:24:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    idl_clr_group	(e) clear idl group code(s)

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function clears an IDL group code. If the group code is -1, the
    function clears all groups. An invalid  group code number is ignored.
    If the port identifier (port) is null, the function clears the IDL
    group in all 'ports'. In all cases, the function rebuilds hs_idl_group
    to be the logical OR of all port IDL groups.

    When using objects, the port ID is an object pointer (ob).

    When using CME, the port ID in the calling arguments is the 'cid' of
    the CM port and 'cid_type' must be an integer in the range or 0 to
    (cm_port_maximum-1).

    If objects or CME are not supported, the function operates only on
    the master IDL group (hs_idl_group).

    CALL BY:    void idl_clr_group(port,grp)
    		lat_port  ob;
                int       grp;

    ON EXIT:    The group code is cleared, hs_idl_group set to logical
    		OR of all port IDL groups.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latob.h"
#include "latfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

extern	hs_ptr	hs;			/* Local node definitions	*/
#if LE_OBJECTS
extern	ob_ptr	ob_head[2];		/* Object list header		*/
#endif
#if CME_SLAT
extern	int	cm_port_maximum;	/* Maximum number of CME ports	*/
#endif

#if !CME_SLAT
typedef	byte	OB_VIEW(grp_ptr);
#endif

void idl_clr_group(port,grp)

    lat_port	port;			/* Pointer to object structure	*/
    int         grp;			/* Bit position to be set       */
{
    int		len;			/* Current length               */
#if (LE_OBJECTS || CME_SLAT)
    byte	i;			/* Mask of changed byte		*/
    byte	bit;			/* Bit mask for testing		*/
#if LE_OBJECTS
    ob_ptr	ob;			/* Pointer to object structure	*/
#elif CME_SLAT
    int		j;			/* CME port identification	*/
#endif
    grp_ptr	gp;			/* Pointer to group field	*/
#endif

#if (LE_OBJECTS || CME_SLAT)
/*-----------------------------------------------------------------------
    If no object or port specified, loop through all ports and clear
    group.
 -----------------------------------------------------------------------*/
#if LE_OBJECTS
    if (port == 0)
    {
        for (ob = ob_head[0]; ob != 0; ob = ob->ob_link[0])
        {
	    if (grp == -1)
	    {
	        ob->ob_idl[0] = 0;
		hs->hs_idl_group[0] = 0;
	    }
	    else
		idl_clr_group(ob,grp);
        }
    }

#elif CME_SLAT
    if (port == CID_NULL)
    {
	for (j = 0; j < cm_port_maximum; j++)
	{
	    gp = cx_group(j);
	    if (grp == -1)
	    {
		gp[0] = 0;
		hs->hs_idl_group[0] = 0;
	    }
	    else
		idl_clr_group(j,grp);
	}
    }
#endif

/*-----------------------------------------------------------------------
    If grp is between 0 and 255, calculate the current length, and
    clear the appropriate bit.
 -----------------------------------------------------------------------*/
    else
    {
        if ((grp >= 0) && (grp <= 255))
        {
	    bit = (byte)(1 << (grp % 8));
	    len = 1 + (grp / 8);

#if CME_SLAT
	    gp = cx_group(port);
#else
	    gp = (grp_ptr)&port->ob_idl[0];
#endif
	    gp[len] &= ~bit;

/*-----------------------------------------------------------------------
    If we clear a group in the last group, scan backwards to find first
    non-zero byte and update the length of the acl group field.
 -----------------------------------------------------------------------*/
    	    if (len == gp[0] && gp[len] == 0)
    	    {
    	        for (--len; len != 0; --len)
		{
		    if (gp[len] != 0)
    		        break;
		}
   	        gp[0] = ((byte)(LOBYTE(len)));
    	    }

/*-----------------------------------------------------------------------
    Regenerate the master idl groups.
 -----------------------------------------------------------------------*/
	    len = 1 + (grp / 8);
#if LE_OBJECTS
	    for (i = 0, ob = ob_head[0]; ob != 0; ob = ob->ob_link[0])
	    {
		gp = (grp_ptr)&ob->ob_idl[0];
#elif CME_SLAT
	    for (i = 0, j = 0; j < cm_port_maximum; j++)
	    {
		gp = cx_group(j);
#endif
	        if (len <= gp[0])
	            i |= gp[len];
	    }
	    if ((i & bit) == 0)
	    {
	        hs->hs_idl_group[len] &= ~bit;
	        if (len == hs->hs_idl_group[0] && hs->hs_idl_group[len] == 0)
	        {
	            for (--len; len != 0; --len)
		    {
		        if (hs->hs_idl_group[len] != 0)
			    break;
		    }
		    hs->hs_idl_group[0] = ((byte)(LOBYTE(len)));
	        }
	    }
        }

/*-----------------------------------------------------------------------
    If grp is -1, disable all object idl groups.
 -----------------------------------------------------------------------*/
        else if (grp == -1)
        {
#if LE_OBJECTS
	    gp = (grp_ptr)&port->ob_idl[0];
#elif CME_SLAT
	    gp = cx_group(port);
#endif
            gp[0] = 0;

/*-----------------------------------------------------------------------
    Regenerate the master idl groups.
 -----------------------------------------------------------------------*/
	    for (len = 0; len < GROUPS; len++)
	        hs->hs_idl_group[len] = 0;

#if LE_OBJECTS
  	    for (ob = ob_head[0]; ob != 0; ob = ob->ob_link[0])
	    {
		gp = (grp_ptr)&ob->ob_idl[0];
#elif CME_SLAT
	    for (j = 0; j < cm_port_maximum; j++)
	    {
		gp = cx_group(j);
#endif
	        for (len = 1; len <= gp[0]; len++)
	            hs->hs_idl_group[len] |= gp[len];

	        if (hs->hs_idl_group[0] < gp[0])
	            hs->hs_idl_group[0] = gp[0];
	    }
        }
    }
#else

/*-----------------------------------------------------------------------
    If grp is between 0 and 255, calculate the current length, and
    clear the appropriate bit.
 -----------------------------------------------------------------------*/
    if ((grp >= 0) && (grp <= 255))
    {
        len = 1 + (grp / 8);
        hs->hs_idl_group[len] &= ~(1 << (grp % 8));

/*-----------------------------------------------------------------------
    If we clear a group in the last group, scan backwards to find first
    non-zero byte and update the length of the acl group field.
 -----------------------------------------------------------------------*/
        if (len == hs->hs_idl_group[0])
        {
	    for (; len != 0; --len)
            {
    	        if (hs->hs_idl_group[len] != 0)
    		    break;
    	    }
	    hs->hs_idl_group[0] = (byte)(LOBYTE(len));
        }
    }

/*-----------------------------------------------------------------------
    If grp is -1, disable all groups.
 -----------------------------------------------------------------------*/
    else if (grp == -1)
        hs->hs_idl_group[0] = 0;

#endif
}
