/* $Id: sbria.c,v 3.2 1995/11/17 17:35:15 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/sbria.c,v $
 *------------------------------------------------------------------
 * $Log: sbria.c,v $
 * Revision 3.2  1995/11/17  17:35:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:28:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:28:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    sb_ri_action	(i) act on response messages

    Copyright 1991 Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is  used on subject systems to process a HIC response
    message which originated from a call to sb_open().

    Response information messages are gathered by hi_h_ri_message and
    the one offering the service with the best rating is retained for
    processing by this routine.

    CALL BY:	void sb_ri_action(hi)
		hi_ptr  hi;

    ON EXIT:	The matching HIC entry processed and a session started
    		or error status returned to environment.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latsb.h"
#include "lathi.h"
#include "latls.h"
#include "latnd.h"
#include "latcb.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	lat_error;		/* LAT error code		*/
extern	byte	nd_temp[];		/* Local node name storage	*/
extern	byte	as_temp[];		/* Local service name storage	*/

void sb_ri_action(hi)

    hi_ptr	hi;			/* Pointer to HI structure	*/
{
    sb_ptr	sb;			/* Pointer to SB structure	*/
    sb_ptr	sx;			/* Pointer to SB structure	*/
    nd_ptr	nd;			/* Pointer to ND structure	*/
    cb_ptr	cb;			/* Pointer to CB structure	*/
    word	nd_addr[3];		/* Remote node address		*/
    int		status;			/* Status flag			*/
    int		reason;			/* Failure reason		*/
    int		i;			/* Loop index			*/
    int		ses;			/* Session ID			*/


    status = FAILURE;
    reason = SX_RESOURCE;

    sb = hi->hi_sb;
/*-----------------------------------------------------------------------
    If we didn't get a response, fail
 -----------------------------------------------------------------------*/
    if (!(hi->hi_resp_status & HI_RESPONSE))
    	reason = SX_TIMEOUT;

    else
    {
	hi_str_move_ls(&as_temp[0],&hi->hi_service[0]);

/*-----------------------------------------------------------------------
    Try to find or create the node structure.  If we can't, fail.
 -----------------------------------------------------------------------*/
	hi_str_move_ls(&nd_temp[0],&hi->hi_save_node[0]);
	nd_addr[0] = hi->hi_save_addr[0];
	nd_addr[1] = hi->hi_save_addr[1];
	nd_addr[2] = hi->hi_save_addr[2];
	if ((nd = nd_create(&nd_temp[0],&nd_addr[0], hi->hi_save_idb)) == 0)
	    lat_error = ND_ERROR | NO_MEM;
	else
	{
/*-----------------------------------------------------------------------
    Try to find or create a circuit block.  If we can't, fail
 -----------------------------------------------------------------------*/
 	    for (cb = 0, i = 1; i < hs->hs_cb_index; i++)
	    {
	    	if (hs->hs_cb_table[i] != 0 && hs->hs_cb_table[i]->cb_type
			 == CB_SERVER)
		{
		    if (hs->hs_cb_table[i]->cb_nd == nd)
		    {
			cb = hs->hs_cb_table[i];
			break;
		    }
		}
	    }
	    if (cb == 0)
	    {
	    	if ((cb = cb_create(nd)) != 0)
		{
		    cb->cb_type = CB_SERVER;
		    cb->cb_state = CB_START;
		    cb->cb_tics = 1;
		}
	    }
	    if (cb == 0 || cb->cb_max_ses == 0)
	    	lat_error = CB_ERROR | NO_MEM;

/*-----------------------------------------------------------------------
    We have a circuit block.  Fill in the remainder of the session block,
    link it to the circuit block and start session on next clock tic.
 -----------------------------------------------------------------------*/
	    else
	    {
	    	count_h(ch_total_sessions);
		cb->cb_max_ses--;
		cb->cb_nd->nd_usage++;

		while (SUCCESS)
		{
		    if ((ses = ++cb->cb_session) == 0)
		    	continue;
		    for (sx = cb->cb_sb[sb_hash(ses)][0]; sx != 0;
		    	    sx = sx->sb_link[0])
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
		if (sb->sb_slot[1] != 0)
		    sb->sb_slot[1]->sb_slot[0] = sb;
		else
		    cb->cb_slot[0] = sb;

		sb_str_move_sl(&sb->sb_name[0],&as_temp[0]);
	    	sb->sb_cb = cb;
		sb->sb_lcl_ses_id = (byte)ses;

		i = sb->sb_lcl_credits;
		sb->sb_lcl_credits = 0;

		sb_signal(sb,SB_START,i);

		status = SUCCESS;
	    }
	}
    }

/*-----------------------------------------------------------------------
    Clear object field and delete the HI structure.
 -----------------------------------------------------------------------*/
#if !CME_SLAT
    hi->hi_ob = 0;
#endif
    hi_delete(hi,0);

/*-----------------------------------------------------------------------
    If we don't have a good status, free the session block.  If using
    objects, release the object.  If using CME, release the CME port.
 -----------------------------------------------------------------------*/
    if (status == FAILURE)
    {
#if LE_OBJECTS
	if (sb->sb_ob != 0)
	    ob_release(sb->sb_ob);
#endif

#if CME_SLAT
	if (sb->sb_cid != CID_NULL)
	    cx_usage(sb->sb_cid, CM_LOCAL | CM_RELEASE);
#endif

	sx_session(sb,reason);
    	sb_free(sb);
	hs->hs_sb_current--;
    }
}
