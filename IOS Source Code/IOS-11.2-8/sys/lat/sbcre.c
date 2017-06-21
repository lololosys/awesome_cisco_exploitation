/* $Id: sbcre.c,v 3.2 1995/11/17 17:35:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/sbcre.c,v $
 *------------------------------------------------------------------
 * $Log: sbcre.c,v $
 * Revision 3.2  1995/11/17  17:35:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:28:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:28:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    sb_create		(i) create session block

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function creates and initializes a session block. On successful
    return, the caller should initialize the session state flags, sid,
    local credits, object pointer and service name.

    CALL BY:	sb_ptr sb_create(cb)
    		cb_ptr	cb;

    ON EXIT:	Returns pointer to session block if the session is
    		created. Otherwise the function returns a zero and
		sets lat_error.

	SB_ERROR | MAX_SPACE	session limit for circuit/local host exceeded
	SB_ERROR | NO_MEM	session block cannot be allocated
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latnd.h"
#include "latsb.h"
#include "latmsg.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	lat_error;		/* LAT error code		*/

typedef	byte	SB_VIEW(sb_byte);	/* Pointer to byte fields	*/

sb_ptr sb_create(cb)

    cb_ptr	cb;			/* Pointer to circuit block	*/

{
    sb_ptr	sb;			/* Pointer to session block	*/
    sb_ptr	sx;			/* Pointer to session block	*/
    int		ses;			/* Session ID			*/
    sb_byte	ptr;			/* Pointer to data_b structure	*/
    int		i;			/* Loop counter			*/

/*-----------------------------------------------------------------------
    Preset error return.
 -----------------------------------------------------------------------*/
    sb = 0;

/*-----------------------------------------------------------------------
    Check if total session or sessions/circuit limit reached.
 -----------------------------------------------------------------------*/
    if (cb->cb_ses_count >= cb->cb_max_ses)
	lat_error = SB_ERROR | MAX_LIMIT;

    else if (hs->hs_sb_maximum > 0 && hs->hs_sb_current >= hs->hs_sb_maximum)
	lat_error = SB_ERROR | MAX_LIMIT;

/*-----------------------------------------------------------------------
    If session block cannot be allocated, return error
 -----------------------------------------------------------------------*/
    else if ((sb = sb_alloc()) == 0)
	lat_error = SB_ERROR | NO_MEM;

/*-----------------------------------------------------------------------
    Otherwise, initialize new session block.
 -----------------------------------------------------------------------*/
    else
    {
	if (++hs->hs_sb_current > hs->hs_sb_highest)
	    hs->hs_sb_highest++;

	count_h(ch_total_sessions);
	cb->cb_ses_count++;
	cb->cb_nd->nd_usage++;

	while (SUCCESS)
	{
	    if ((ses = ++cb->cb_session) == 0)
		continue;
            for (sx = cb->cb_sb[sb_hash(ses)][0]; sx != 0; sx = sx->sb_link[0])
            {
                if (sx->sb_lcl_ses_id == (byte)ses)
		    break;
	    }
            if (sx == 0)
	        break;
	}

	sb->sb_link[0] = 0;
	sb->sb_link[1] = cb->cb_sb[sb_hash(ses)][1];
	cb->cb_sb[sb_hash(ses)][1] = sb;
	if (sb->sb_link[1] != 0)
	    sb->sb_link[1]->sb_link[0] = sb;
	else
	    cb->cb_sb[sb_hash(ses)][0] = sb;

	sb->sb_slot[0] = 0;
	sb->sb_slot[1] = cb->cb_slot[1];
	cb->cb_slot[1] = sb;
	if (sb->sb_slot[1] != 0)
	    sb->sb_slot[1]->sb_slot[0] = sb;
	else
	    cb->cb_slot[0] = sb;

	sb->sb_cb = cb;
	sb->sb_ob = 0;

	sb->sb_sid    = SID_NULL;
#if CME_SLAT
	sb->sb_cid    = CID_NULL;
#endif
	sb->sb_flags  = 0;
	sb->sb_req_no = 0;

	sb->sb_rmt_ses_id = 0;
	sb->sb_lcl_ses_id = (byte)ses;

	sb->sb_rmt_credits     = 0;
	sb->sb_lcl_credits     = 0;
	sb->sb_ext_credits     = 0;
	sb->sb_stop_reason     = 0;
	sb->sb_max_data        = 0;
	sb->sb_max_attn        = 0;
	sb->sb_data_b_type     = SB_DATA_B_UNK;

	sb->sb_data_b_lcl_mode = SB_INTERACTIVE;
	sb->sb_data_b_rmt_mode = SB_INTERACTIVE;
	sb->sb_name[0]         = 0;
	sb->sb_port[0]         = 0;
	sb->sb_password[0]     = 0;
	sb->sb_sc = SVC_CLASS_TERM;	/* Default service class */

/*-----------------------------------------------------------------------
    Initialize data_b structure for remote DATA-B information.
 -----------------------------------------------------------------------*/
	for (ptr = (sb_byte)&sb->sb_rmt_data, i = 0; i < sizeof(data_b); i++)
	    *ptr++ = 0;
	for (ptr = (sb_byte)&sb->sb_tmp_data, i = 0; i < sizeof(data_b); i++)
	    *ptr++ = 0;

    }
    return(sb);
}
