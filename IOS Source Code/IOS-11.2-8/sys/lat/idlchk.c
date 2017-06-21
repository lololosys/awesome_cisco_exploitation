/* $Id: idlchk.c,v 3.2 1995/11/17 17:33:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/idlchk.c,v $
 *------------------------------------------------------------------
 * $Log: idlchk.c,v $
 * Revision 3.2  1995/11/17  17:33:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:24:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    idl_chk_group	(i) compare remote node groups to idl

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function compares the remote node groups with the local IDL
    groups and returns SUCCESS if any match, FAILURE otherwise. If
    the port ID ('ob' or 'cid') is zero, compare against the master IDL
    groups (hs_idl_group).  If both groups are zero length, return
    SUCCESS (LAT V5.2 protocol spec).

    CALL BY:    int idl_chk_group(port,nd)
    		lat_port  ob;
                nd_ptr	  nd;

    ON EXIT:    Returns SUCCESS if any bit match in two groups, FAILURE
    		otherwise.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latnd.h"
#include "latob.h"
#include "latfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

extern	hs_ptr	hs;			/* Local node definitions	*/

#if !CME_SLAT
typedef byte	OB_VIEW(grp_ptr);
#endif

int idl_chk_group(port,nd)

    lat_port	port;			/* Port ID; 'ob' pointer or
						CME 'cid'		*/
    nd_ptr	nd;			/* Pointer to remote node	*/
{
    int		end;			/* Minimum length of groups	*/
#if (LE_OBJECTS || CME_SLAT)
    grp_ptr	gp;			/* Pointer to group string	*/
#endif

/*-----------------------------------------------------------------------
    If no port, process against hs_idl_group.
    NOTE: A non-zero value of 'end' implies success.
 -----------------------------------------------------------------------*/
#if CME_SLAT
    if (port == CID_NULL)
#elif LE_OBJECTS
    if (port == 0)
#else
    if (TRUE)
#endif
    {
	if (nd->nd_acl[0] == 0 && hs->hs_idl_group[0] == 0)
	    end = -1;
	else
            end = (nd->nd_acl[0] < hs->hs_idl_group[0]) ? nd->nd_acl[0] : hs->hs_idl_group[0];

        for (; end > 0; end--)
        {
	    if (nd->nd_acl[end] & hs->hs_idl_group[end])
	        break;
        }
    }

#if (LE_OBJECTS || CME_SLAT)
/*-----------------------------------------------------------------------
    If object, process against ob_idl.
 -----------------------------------------------------------------------*/
    else
    {
/*-----------------------------------------------------------------------
    Get the pointer to the idl group string.
 -----------------------------------------------------------------------*/
#if CME_SLAT
	gp = cx_group(port);
#elif LE_OBJECTS
	gp = (grp_ptr)&port->ob_idl[0];
#endif
/*-----------------------------------------------------------------------
    Do the processing.
 -----------------------------------------------------------------------*/
	if (nd->nd_acl[0] == 0 && gp[0] == 0)
	    end =  -1;
	else
            end = (nd->nd_acl[0] < gp[0]) ? nd->nd_acl[0] : gp[0];

        for (; end > 0; end--)
        {
	    if (nd->nd_acl[end] & gp[end])
	        break;
        }
    }
#endif

    return( (end != 0) ? SUCCESS : FAILURE);
}
