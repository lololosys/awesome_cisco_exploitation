/* $Id: hiosts.c,v 3.2 1995/11/17 17:32:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hiosts.c,v $
 *------------------------------------------------------------------
 * $Log: hiosts.c,v $
 * Revision 3.2  1995/11/17  17:32:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:23:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hi_o_sts_message	(i) output host-initiated status message

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called to output a host initiated status message.
    If non-error status is returned, scan the rest of the hi entries
    for other status which can be piggybacked in this message.

    CALL BY:	void hi_o_sts_message(of,hi)
		of_ptr	of;
		hi_ptr	hi;

    ON EXIT:	Appropriate message sent to remote node.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "lathi.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

void hi_o_sts_message(of,hi)

    of_ptr	of;			/* Output frame pointer		*/
    hi_ptr	hi;			/* HIC request pointer		*/
{
    hi_ptr	hx;			/* HIC request pointer		*/
    sts_optr	sts;			/* Output status message	*/
    of_byte	next;			/* Output byte pointer		*/
    of_byte	entries;		/* Number of entries		*/
    int		max;			/* Maximum datalink size	*/
    
/*-----------------------------------------------------------------------
    Fill in the fixed fields of a status host-initiated message.
 -----------------------------------------------------------------------*/
    sts = (sts_optr)of->of_data;
    next = of->of_data + sizeof(sts_msg);

    sts->sts_type     = hi->hi_function;
    sts->sts_format   = 0;
    sts->sts_prot_hi  = hs->hs_prot_hi;
    sts->sts_prot_lo  = hs->hs_prot_lo;
    sts->sts_prot_ver = hs->hs_prot_ver;
    sts->sts_prot_eco = hs->hs_prot_eco;
    sts->sts_dll_max  = (hs->hs_inp_max != 0) ? swap(hs->hs_inp_max) : swap(hs->hs_dll_max);
    sts->sts_timer    = swap(hs->hs_hi_sts_timer);

    entries  = next++;
    *entries = 0;

    next = hi_str_move_os(next,&hi->hi_node[0]);
    if ((int)(next - of->of_data) & 1)
	*next++ = 0;

/*-----------------------------------------------------------------------
    If this is a error or action status report or not marked as multiple
    send single status entry.
 -----------------------------------------------------------------------*/
    if ((hi->hi_status & (STS_ERROR | STS_STARTED)) || !(hi->hi_flags & HI_MULTI))
    {
	*entries += 1;
	next = hi_o_request(next,hi);
    }

/*-----------------------------------------------------------------------
    Otherwise, send this entry and any others we can find.
 -----------------------------------------------------------------------*/
    else
    {
	hi->hi_tics = 1;
	max  = (hi->hi_dll_max < hs->hs_dll_max) ? hi->hi_dll_max : hs->hs_dll_max;
	max -= 258;

	for (hx = hi; hx != 0; hx = hx->hi_link[0])
	{
	    if ((int)(next - of->of_data) > max)
		break;

	    if (hx->hi_tics == 1 && (hx->hi_flags & HI_MULTI))
	    {
		if (!(hx->hi_status & (STS_ERROR | STS_STARTED)))
		{
		    if (hx->hi_dest_addr[0] == hi->hi_dest_addr[0] &&
			hx->hi_dest_addr[1] == hi->hi_dest_addr[1] &&
			hx->hi_dest_addr[2] == hi->hi_dest_addr[2] &&
			hx->hi_idb == hi->hi_idb)
		    {
			*entries += 1;
			next = hi_o_request(next,hx);

			if (hx != hi)
			    hx->hi_tics++;
		    }
		}
	    }
	}
    }

/*-----------------------------------------------------------------------
    Set no parameters and compute length of message.
 -----------------------------------------------------------------------*/
    if ((int)(next - of->of_data) & 1)
	*next++ = 0;

    *next++ = 0;
    of->of_size = (int)(next - of->of_data);
}
