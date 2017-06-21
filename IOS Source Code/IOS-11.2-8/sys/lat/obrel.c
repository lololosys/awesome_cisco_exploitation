/* $Id: obrel.c,v 3.2 1995/11/17 17:34:52 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/obrel.c,v $
 *------------------------------------------------------------------
 * $Log: obrel.c,v $
 * Revision 3.2  1995/11/17  17:34:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:27:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    ob_release		(i) release object usage

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function releases one usage of an object and if possible, flags
    that a queue scan should be performed.

    CALL BY:	void ob_release(ob)
		ob_ptr   ob;

    ON EXIT:	Object released.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latob.h"
#include "latfnc.h"

void ob_release(ob)

    ob_ptr	ob;			/* Pointer to object  block	*/
{

/*-----------------------------------------------------------------------
   Release the lock on the object block.
 -----------------------------------------------------------------------*/
    if (ob->ob_flags & OB_LOCAL)
    {
        if (--ob->ob_lcl_use == 0 && ob->ob_flags & OB_DYNAMIC)
	    ob->ob_flags &= ~OB_LOCAL;
    }
    else if (ob->ob_flags & OB_REMOTE)
    {
	if (--ob->ob_rmt_use == 0 && ob->ob_flags & OB_DYNAMIC)
	    ob->ob_flags &= ~OB_REMOTE;
    }

/*-----------------------------------------------------------------------
    If cloned object and no more usage, delete object.
 -----------------------------------------------------------------------*/
    if ((ob->ob_flags & OB_CLONED) && ob->ob_lcl_use == 0 && ob->ob_rmt_use == 0)
	ob_delete(ob);

#if LE_OBJECTS && LE_OBJECT_HIC
/*-----------------------------------------------------------------------
    If not local object and remote sessions available, flag a
    queue scan should be performed.
 -----------------------------------------------------------------------*/
    else if (!(ob->ob_flags & OB_ENABLED))
	;
    else if (ob->ob_flags & OB_LOCAL)
	;

    else if (ob->ob_rmt_use >= ob->ob_rmt_max)
	;

    else
    {
	extern int qr_changes;
	qr_changes = SUCCESS;
    }

#endif
}
