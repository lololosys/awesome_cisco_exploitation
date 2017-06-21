/* $Id: hiecan.c,v 3.2 1995/11/17 17:32:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hiecan.c,v $
 *------------------------------------------------------------------
 * $Log: hiecan.c,v $
 * Revision 3.2  1995/11/17  17:32:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:23:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hi_entry_cancel	(e) cancel previously queued entry

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is used by subject systems to cancel a previous queued
    HIC request.

    CALL BY:	void hi_entry_cancel(hi)
    		hi_ptr	hi;

    ON EXIT:	HIC entry canceled and all resources used by entry released.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "lathi.h"
#include "latob.h"
#include "latmsg.h"
#include "latfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

extern	hs_ptr	hs;			/* Local node definitions	*/

void hi_entry_cancel(hi)

    hi_ptr	hi;			/* Pointer to HIC block		*/
{
/*-----------------------------------------------------------------------
    Set no session ID.
 -----------------------------------------------------------------------*/
     hi->hi_sid = SID_NULL;

#if LE_OBJECTS
/*-----------------------------------------------------------------------
    Release the object.
 -----------------------------------------------------------------------*/
    if (hi->hi_ob != 0)
    {
	ob_ptr ob;

	ob = hi->hi_ob;
	hi->hi_ob = 0;
	ob_release(ob);
    }
#elif CME_SLAT
/*-----------------------------------------------------------------------
    Release the CME port.
 -----------------------------------------------------------------------*/
    if (hi->hi_cid != CID_NULL)
    {
	if (hi->hi_cid != CID_NULL)
	{
	    cx_usage(hi->hi_cid,CM_RELEASE);
	    hi->hi_cid = CID_NULL;
	}
    }
#endif

/*-----------------------------------------------------------------------
    If we do not have queue number, we cannot cancel.
 -----------------------------------------------------------------------*/
    if (hi->hi_que_no == 0)
	hi_delete(hi,0);

/*-----------------------------------------------------------------------
    All is well. Initialize the HI structure to send solicit message.
 -----------------------------------------------------------------------*/
    else
    {
	hi->hi_tics     = 1;
	hi->hi_retries  = hs->hs_hi_cmd_retries;

	hi->hi_function = LAT_COMMAND;
	hi->hi_command  = CMD_CANCEL;
    }
}
