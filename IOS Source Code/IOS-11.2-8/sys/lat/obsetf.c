/* $Id: obsetf.c,v 3.2 1995/11/17 17:34:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/obsetf.c,v $
 *------------------------------------------------------------------
 * $Log: obsetf.c,v $
 * Revision 3.2  1995/11/17  17:34:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:27:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    ob_set_flags	(e) update object flags

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function updates the enabled, queued, and busy object flags  If the
    new object status could effect the queue, mark queue changes. This
    function cannot be used to change the type of object (local, remote,
    or dynamic) as these settings can only be made when the object is
    created.

    CALL BY:	void ob_set_flags(ob,flags)
    		ob_ptr	ob;
    		int	flags;

    ON EXIT:	Updates the flags and set queue change if object is
    		enabled or changes in queue status.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latob.h"
#include "latfnc.h"

void ob_set_flags(ob,flags)

    ob_ptr	ob;			/* Pointer to new AS structure	*/
    int		flags;			/* New service flags		*/
 {

/*-----------------------------------------------------------------------
    Mask off all but enabled and queued flags.
------------------------------------------------------------------------*/
    flags &= (OB_ENABLED | OB_QUEUED | OB_BUSY);

#if LE_OBJECTS && LE_OBJECT_HIC
/*-----------------------------------------------------------------------
    If new flags enabled previously disabled object or change queue
    status, this may effect queueing.
------------------------------------------------------------------------*/
    if ((ob->ob_flags & (OB_ENABLED | OB_QUEUED | OB_BUSY)) ^ flags)
    {
	extern int qr_changes;
        qr_changes = SUCCESS;
    }
#endif

/*-----------------------------------------------------------------------
    Set new enabled, queued status.
------------------------------------------------------------------------*/
    ob->ob_flags &= ~(OB_ENABLED | OB_QUEUED | OB_BUSY);
    ob->ob_flags |= flags;
}
