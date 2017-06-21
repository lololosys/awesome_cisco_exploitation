/* $Id: hierpt.c,v 3.2 1995/11/17 17:32:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hierpt.c,v $
 *------------------------------------------------------------------
 * $Log: hierpt.c,v $
 * Revision 3.2  1995/11/17  17:32:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:23:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hi_entry_report	(e) get status of previously queued entry

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function gets the status of a previous queued HIC request.

    CALL BY:	void hi_entry_report(hi)
    		hi_ptr	hi;

    ON EXIT:	Issues request for status.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "lathi.h"
#include "latmsg.h"
#include "latfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

void hi_entry_report(hi)

    hi_ptr	hi;			/* Pointer to HIC block		*/
{

/*-----------------------------------------------------------------------
    If we do have queue number, we can issue report request.
 -----------------------------------------------------------------------*/
    if (hi->hi_que_no != 0)
    {
	hi->hi_tics     = 1;
	hi->hi_retries  = hs->hs_hi_cmd_retries;

	hi->hi_function = LAT_COMMAND;
	hi->hi_command  = CMD_STS_ENTRY;
    }
}
