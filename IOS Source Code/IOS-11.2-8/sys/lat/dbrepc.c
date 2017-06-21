/* $Id: dbrepc.c,v 3.2 1995/11/17 17:32:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/dbrepc.c,v $
 *------------------------------------------------------------------
 * $Log: dbrepc.c,v $
 * Revision 3.2  1995/11/17  17:32:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:22:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    db_report_config	(e) report local configuration

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function reports a session's object local port configuration.

    CALL BY:	void db_report_config(sb)
    		sb_ptr	sb;

    ON EXIT:	Marks session to send report data-b slot.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latsb.h"
#include "latfnc.h"

void db_report_config(sb)

    sb_ptr	sb;			/* Pointer to session block	*/
{
    sb->sb_data_b_type |= SB_DATA_B_REP;
    sb_signal(sb,SB_DATA_B,0);
}
