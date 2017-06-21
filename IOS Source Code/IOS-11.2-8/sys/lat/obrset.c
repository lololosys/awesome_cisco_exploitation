/* $Id: obrset.c,v 3.2 1995/11/17 17:34:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/obrset.c,v $
 *------------------------------------------------------------------
 * $Log: obrset.c,v $
 * Revision 3.2  1995/11/17  17:34:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:27:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    ob_reset		(e) delete all objects

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function deletes all objects structures.

    CALL BY:	void ob_reset()

    ON EXIT:	All objects are deleted.
 -----------------------------------------------------------------------*/

extern	ob_ptr	ob_head[2];		/* OB structure list header	*/

void ob_reset()

{
/*-----------------------------------------------------------------------
    Move through the object list and try to delete all objects.
 -----------------------------------------------------------------------*/
    while (ob_head[0] != 0)
	ob_delete(ob_head[0]);
}
