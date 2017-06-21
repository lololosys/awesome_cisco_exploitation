/* $Id: lsrset.c,v 3.2 1995/11/17 17:34:32 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/lsrset.c,v $
 *------------------------------------------------------------------
 * $Log: lsrset.c,v $
 * Revision 3.2  1995/11/17  17:34:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:26:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    ls_reset		(e) delete all learned service structures

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function deletes all learned service structures, including
    permanent names.

    CALL BY:	void ls_reset()

    ON EXIT:	All learned service structures are deleted.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latls.h"
#include "latnd.h"
#include "latfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

void ls_reset()

{
    ls_ptr	ls;			/* Pointer to learned service	*/
    nd_ptr	nd;			/* Pointer to remote node	*/

/*-----------------------------------------------------------------------
    Scan through the learned service list.
 -----------------------------------------------------------------------*/
    while ((ls = hs->hs_ls_head[0]) != 0)
    {
	if ((nd = ls->ls_nd) != 0)
   	    nd->nd_flags |= ND_INIT;

/*-----------------------------------------------------------------------
    Mark the service non-permanent and delete.
 -----------------------------------------------------------------------*/
	ls->ls_flags &= (~LS_PERM);
	ls_delete(ls);
	if (nd != 0)
	    nd_fail(nd,0);
    }
}
