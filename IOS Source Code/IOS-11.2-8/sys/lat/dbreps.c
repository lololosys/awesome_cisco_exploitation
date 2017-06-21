/* $Id: dbreps.c,v 3.2 1995/11/17 17:32:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/dbreps.c,v $
 *------------------------------------------------------------------
 * $Log: dbreps.c,v $
 * Revision 3.2  1995/11/17  17:32:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:22:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    db_report_status	(e) report DATA-B status conditions

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is used to report data stream errors specified by the
    DATA-B status conditions. The error flag and bad character are stored
    in the session block.  Pass a value of zero for err if you can not pass
    the bad character information.

    CALL BY:	void db_report_status(sb,sts,err)
    		sb_ptr	sb;
    		byte	sts;
    		byte	err;

    ON EXIT:	Stores report information and flags session to send
    		a full REPORT DATA_B slot with the status condition set.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latob.h"
#include "latsb.h"
#include "latfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

#if !(LE_OBJECTS || CME_SLAT)
extern ob_node	ob_dummy;
#endif

#if !CME_SLAT
typedef data_b	OB_VIEW(dab_ptr);
#endif

void db_report_status(sb,sts,err)

    sb_ptr	sb;			/* Pointer to session block	*/
    byte	sts;			/* Status code (from latsys.h)	*/
    byte	err;			/* Bad character		*/
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

    db->db_flags  |= DB_E_STATUS;

    sb->sb_lcl_data_b_status = sts;
	
    if (err)	
    	sb->sb_lcl_data_b_status_char = err;
	
    sb->sb_data_b_type |= SB_DATA_B_REP;    	
    sb_signal(sb,SB_DATA_B,0);
}
