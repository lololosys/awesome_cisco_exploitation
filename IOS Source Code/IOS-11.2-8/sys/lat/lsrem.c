/* $Id: lsrem.c,v 3.2 1995/11/17 17:34:30 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/lsrem.c,v $
 *------------------------------------------------------------------
 * $Log: lsrem.c,v $
 * Revision 3.2  1995/11/17  17:34:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:26:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    ls_remove		(e) delete all ls structures with given name

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function deletes all ls_node structures with the same service
    name.  If the  associated nd_node structure has no other names and
    no outstanding circuits, it is also deleted.  This function does not
    affect existing connections.

    CALL BY:	void ls_remove(name)
		byte	*name;

    ON EXIT:	All learned service structures with the specified name
		are deleted.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latls.h"
#include "latnd.h"
#include "latob.h"
#include "latfnc.h"

void ls_remove(name)

    byte	*name;			/* Learned service name		*/
{
    ls_ptr	ls;			/* Pointer to LS structure	*/
    nd_ptr	nd;			/* Pointer to ND structure	*/
#if CME_SLAT
    cid_type	null_port = CID_NULL;	/* Value of the null port	*/
#else
    ob_ptr	null_port = 0;
#endif

/*-----------------------------------------------------------------------
    As long as there is a learned service with the given name, delete.
    If learned service is permanent, mark it non-permanent.
 -----------------------------------------------------------------------*/
    while ((ls = ls_show(name,0,null_port,0)) != 0)
    {
        nd = ls->ls_nd;
	nd->nd_flags |= ND_INIT;

	ls->ls_flags &= (~LS_PERM);
        ls_delete(ls);
	nd_fail(nd,0);
    }

/*-----------------------------------------------------------------------
    Now scan and delete the name from the hold list.
 -----------------------------------------------------------------------*/
    while ((ls = ls_look(name)) != 0)
    {
	ls->ls_flags &= (~LS_PERM);
        ls_delete(ls);
    }
}
