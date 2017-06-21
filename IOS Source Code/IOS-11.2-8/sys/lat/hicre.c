/* $Id: hicre.c,v 3.2 1995/11/17 17:32:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hicre.c,v $
 *------------------------------------------------------------------
 * $Log: hicre.c,v $
 * Revision 3.2  1995/11/17  17:32:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:22:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hi_create		(i) create hic entry

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function creates and initializes (to zero) an HIC entry.

    CALL BY:	hi_ptr hi_create(type)
		int	 type;

    ON EXIT:	Returns pointer to HIC entry if structure allocated,
    		a zero value otherwise and sets lat_error as follows:

	HI_ERROR | MAX_LIMIT	New entry exceeds HIC entry limit
	HI_ERROR | NO_MEM	Could not allocate HIC entry
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latsb.h"
#include "lathi.h"
#include "latmsg.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	lat_error;		/* LAT error code		*/

hi_ptr hi_create(type)

    int		type;			/* Type of entry		*/
{
    hi_ptr	hi;			/* Pointer to HIC block		*/

/*-----------------------------------------------------------------------
    Preset the return status to FAILURE.
 -----------------------------------------------------------------------*/
    hi = 0;

/*-----------------------------------------------------------------------
    Check if total entry limit reached.
 -----------------------------------------------------------------------*/
    if (hs->hs_hi_maximum > 0 && hs->hs_hi_current == hs->hs_hi_maximum)
	lat_error = HI_ERROR | MAX_LIMIT;

/*-----------------------------------------------------------------------
    If HIC entry cannot be allocated, return error
 -----------------------------------------------------------------------*/
    else if ((hi = hi_alloc()) == 0)
	lat_error = HI_ERROR | NO_MEM;

/*-----------------------------------------------------------------------
    Initialize the HIC entry.
 -----------------------------------------------------------------------*/
    else
    {
        if (++hs->hs_hi_current > hs->hs_hi_highest)
	    hs->hs_hi_highest++;

	hi->hi_link[0] = 0;
	hi->hi_link[1] = hs->hs_hi_head[1];
	hs->hs_hi_head[1] = hi;
	if (hi->hi_link[1] != 0)
	    hi->hi_link[1]->hi_link[0] = hi;
	else
	    hs->hs_hi_head[0] = hi;

#if !CME_SLAT
	hi->hi_ob       = 0;
#endif
	hi->hi_qr       = 0;
	hi->hi_sid      = SID_NULL;

	hi->hi_tics     = 0;
	hi->hi_retries  = hs->hs_hi_cmd_retries;
	hi->hi_dll_max  = 0;

	hi->hi_flags    = (byte)(LOBYTE(type));
	hi->hi_function = 0;
	hi->hi_command  = 0;
	hi->hi_modifier = 0;

	hi->hi_status   = 0;
	hi->hi_error    = 0;
	hi->hi_req_no   = 0;
	hi->hi_que_no   = 0;
	hi->hi_que_time = 0;
	hi->hi_p_queue  = 0;
	hi->hi_q_queue  = 0;

	hi->hi_dest_addr[0] = 0;
	hi->hi_dest_addr[1] = 0;
	hi->hi_dest_addr[2] = 0;

	hi->hi_node[0]      = 0;
	hi->hi_service[0]   = 0;
	hi->hi_port[0]      = 0;
#if LE_SUBJECT_HIC && LE_SERVER
	hi->hi_sb           = 0;
#endif		

	hi->hi_idb = 0;		/* Interface is unknown */
	hi->hi_sc = SVC_CLASS_TERM; /* Default service class */

/*-----------------------------------------------------------------------
    If this is for subject system, generate an unique, non-zero
    identifier.
 -----------------------------------------------------------------------*/
	if (type == HI_SUBJECT)
	{
	    while (hi_find(++hs->hs_hi_req_no,0,type,0) != 0 || hs->hs_hi_req_no == 0)
	        ;

	    hi->hi_req_no = hs->hs_hi_req_no;
	 }
    }
    return(hi);
}
