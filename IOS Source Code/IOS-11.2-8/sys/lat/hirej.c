/* $Id: hirej.c,v 3.2 1995/11/17 17:33:02 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hirej.c,v $
 *------------------------------------------------------------------
 * $Log: hirej.c,v $
 * Revision 3.2  1995/11/17  17:33:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:23:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hi_reject		(e) reject hic session

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called when the envionrment wishes to reject a
    host-initiated session.

    CALL BY:	void hi_reject(hi,reason)
		hi_ptr	hi;
		int	reason;

    ON EXIT:	The reject reason is stored and the hi is flagged
    		to send an error status.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "lathi.h"
#include "latnd.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	byte	nd_temp[];		/* Temporary ND name		*/

void hi_reject(hi,reason)

    hi_ptr	hi;			/* Pointer to HIC entry		*/
    int		reason;			/* Reject reason		*/
{
    nd_ptr	nd;			/* Pointer to remote node	*/

/*-----------------------------------------------------------------------
    Count the solicit as failed.
 -----------------------------------------------------------------------*/
    hi_str_move_ls(&nd_temp[0],&hi->hi_node[0]);
    if ((nd = nd_show(&nd_temp[0],hi->hi_idb)) != 0)
    {
	count_1(co_solicit_reject);
    }

/*-----------------------------------------------------------------------
    Store reject reason and set to transmit an error status.
 -----------------------------------------------------------------------*/
    hi->hi_status  = STS_ERROR;
    hi->hi_error   = (byte)(LOBYTE(reason));
    hi->hi_tics    = 1;
    hi->hi_retries = 1;
}
