/* $Id: idlcmp.c,v 3.2 1995/11/17 17:33:15 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/idlcmp.c,v $
 *------------------------------------------------------------------
 * $Log: idlcmp.c,v $
 * Revision 3.2  1995/11/17  17:33:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:24:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    idl_cmp_group	(i) compare input message group to idl

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function compares an input message group field with the local
    IDL groups and return SUCCESS if any match, FAILURE otherwise. The
    function will always fail if the input message group exceeds the
    maximum allowed length. If the port ID (ob or cid) is zero, compare
    against the master IDL groups (hs_idl_group).  If both groups are
    zero length, return SUCCESS (LAT V5.2 protocol spec).

    CALL BY:    int idl_cmp_group(port,grp)
    		lat_port  port;
                if_byte	  grp;

    ON EXIT:    Returns SUCCESS if any bit match in two groups, FAILURE
    		otherwise.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latob.h"
#include "latfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

extern	hs_ptr	hs;			/* Local node definitions	*/

#if !CME_SLAT
typedef byte	OB_VIEW(grp_ptr);
#endif

int idl_cmp_group(port,grp)

    lat_port	port;			/* Port ID; 'ob' pointer or
					      CME 'cid' 		*/
    if_byte	grp;			/* Pointer to group byte string */
{
    int		end;			/* Minimum length of groups	*/
#if (LE_OBJECTS || CME_SLAT)
    grp_ptr	pgp;			/* Pointer to group field	*/
#endif

/*-----------------------------------------------------------------------
    If no port, process against hs_idl_group.
    NOTE: A non-zero value of 'end' implies success.
 -----------------------------------------------------------------------*/
#if CME_SLAT
    if (port ==  CID_NULL)
#elif LE_OBJECTS
    if (port == 0)
#else
    if (TRUE)
#endif
    {
	if (*grp == 0 && hs->hs_idl_group[0] == 0)
	    end = -1;
	else if (*grp >= GROUPS)
	    end = 0;
	else
            end = (*grp < hs->hs_idl_group[0]) ? *grp : hs->hs_idl_group[0];

        for (; end > 0; end--)
        {
	    if (grp[end] & hs->hs_idl_group[end])
	        break;
        }
    }

#if (LE_OBJECTS || CME_SLAT)
/*-----------------------------------------------------------------------
    If port, process against the port idl groups.
 -----------------------------------------------------------------------*/
    else
    {
/*-----------------------------------------------------------------------
    Get pointer to the idl group string.
 -----------------------------------------------------------------------*/
#if CME_SLAT
	pgp = cx_group(port);
#else LE_OBJECTS
	pgp = (grp_ptr)&port->ob_idl[0];
#endif

/*-----------------------------------------------------------------------
    Do the processing.
 -----------------------------------------------------------------------*/
	if (*grp == 0 && pgp[0] == 0)
	    end = -1;
	else if (*grp >= GROUPS)
	    end = 0;
	else
            end = (*grp < pgp[0]) ? *grp : pgp[0];

        for (; end > 0; end--)
        {
	    if (grp[end] & pgp[end])
	        break;
        }
    }
#endif

    return( (end != 0) ? SUCCESS : FAILURE);
}
