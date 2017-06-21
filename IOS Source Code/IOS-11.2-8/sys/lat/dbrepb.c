/* $Id: dbrepb.c,v 3.2 1995/11/17 17:32:22 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/dbrepb.c,v $
 *------------------------------------------------------------------
 * $Log: dbrepb.c,v $
 * Revision 3.2  1995/11/17  17:32:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:22:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    db_report_break	(e) report break condition

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is used to report a break condition. If your environment
    can distinguish between a long and short break, you may supply the
    type of break in the sts field; use a value of zero if you can not
    distinguish between long and short breaks.

    CALL BY:	void db_report_break(sb,sts)
    		sb_ptr	sb;
    		byte	sts;

    ON EXIT:	Stores break information and flags session to send
    		a full REPORT DATA_B slot with the break bit set.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latob.h"
#include "latsb.h"
#include "latslt.h"
#include "latfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

#if !(LE_OBJECTS || CME_SLAT)
extern	ob_node	ob_dummy;
#endif

#if !CME_SLAT
typedef data_b	OB_VIEW(dab_ptr);
#endif

void db_report_break(

    sb_ptr	sb,			/* Pointer to session block	*/
    byte	sts)			/* Status code (from latsys.h)	*/

{
    dab_ptr	db;			/* Pointer to Data-B structure	*/

#if !(LE_OBJECTS || CME_SLAT)
    ob_ptr	ob;
    ob = (ob_ptr)&ob_dummy;
#endif

#if CME_SLAT
    db = cx_data_b(sb->sb_cid,CX_DATAB_LCL);
#elif LE_OBJECTS
    db = (dab_ptr)&sb->sb_ob->ob_lcl_data;
#else
    db = (dab_ptr)&ob->ob_lcl_data;
#endif

    db->db_config |= SLOT_BREAK;

    if (sts)
    {
	db->db_flags |= DB_E_STATUS;
    	sb->sb_lcl_data_b_status = sts;
    }

    sb->sb_data_b_type |= SB_DATA_B_REP;
    sb_signal(sb,SB_DATA_B,0);
}
