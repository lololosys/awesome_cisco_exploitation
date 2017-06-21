/* $Id: dbsetm.c,v 3.2 1995/11/17 17:32:30 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/dbsetm.c,v $
 *------------------------------------------------------------------
 * $Log: dbsetm.c,v $
 * Revision 3.2  1995/11/17  17:32:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:22:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    db_set_mode		(e) update session transparency mode

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------
    This function updates the session mode.  If the mode changes, the
    session is marked to send a SET DATA-B slot.

    CALL BY:	void db_set_mode(sb,mode)
    		sb_ptr	sb;
    		byte	mode;

    ON EXIT:	Updates the session transparency mode in the session block
    		(local only).
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latsb.h"
#include "latfnc.h"

void db_set_mode(

    sb_ptr	sb,			/* Pointer to session block	*/
    byte	mode)			/* Session transparency mode	*/

{
/*-----------------------------------------------------------------------
    Update the session mode and set to send a SET DATA-B if different.
 -----------------------------------------------------------------------*/
    if (sb->sb_data_b_lcl_mode != mode)
    {
    	sb->sb_data_b_lcl_mode   = mode;
	sb->sb_tmp_data.db_flags |= DB_E_MODE;
	sb->sb_data_b_type       |= SB_DATA_B_SET;
	sb_signal(sb,SB_DATA_B,0);
    }
}
