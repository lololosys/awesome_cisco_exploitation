/*-----------------------------------------------------------------------
    qr_delete		(e/i) delete queued request
	 
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO
	 
    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/
 
/*-----------------------------------------------------------------------
    This function deletes the qr_node structure. The queue is scanned
    from the deleted entry and position data is updated. Any entries
    flagged for queue depth changes are scheduled to send status messages.
    
    Note, it is the responsiblity of the caller to take appropriate
    action with the returned HI status entry.
	 
    CALL BY:	hi_ptr qr_delete(qr)
    		qr_ptr     qr;

    ON EXIT:	The queued request structure is deleted.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "lathi.h"
#include "latqr.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	qr_ptr	qr_head[2];		/* QR structure list header	*/
extern	int	qr_current;		/* Current number entries	*/

hi_ptr qr_delete(qr)

    qr_ptr	qr;                     /* Pointer to QR structure	*/
{
    qr_ptr	qx;			/* Pointer to queue entry	*/
    hi_ptr	hi;			/* Pointer to HIC entry		*/
    
/*------------------------------------------------------------------------
    Scan the rest of the queue and update position information. Also
    schedule any queue depth status messages.
 ------------------------------------------------------------------------*/
    hi = qr->qr_hi;
    hi->hi_qr = 0;
	
    for (qx = qr->qr_link[0]; qx != 0; qx = qx->qr_link[0])
    {
	if (hi_str_equal_ss(&hi->hi_service[0],&qx->qr_hi->hi_service[0]))
	    qx->qr_hi->hi_p_queue--;

	qx->qr_hi->hi_q_queue--;

	if (qx->qr_hi->hi_modifier & CMD_SEND_CHANGE)
	{
	    qx->qr_hi->hi_flags |= HI_MULTI;
	    qx->qr_hi->hi_tics    = 1;
	    qx->qr_hi->hi_retries = 1;
	}
    }

/*------------------------------------------------------------------------
    Remove the queued request from the list.
 ------------------------------------------------------------------------*/
    if (qr->qr_link[0] != 0)
	qr->qr_link[0]->qr_link[1] = qr->qr_link[1];
    else
	qr_head[1] = qr->qr_link[1];

    if (qr->qr_link[1] != 0)
	qr->qr_link[1]->qr_link[0] = qr->qr_link[0];
    else
	qr_head[0] = qr->qr_link[0];

/*------------------------------------------------------------------------
    Deallocate the queued request.
 ------------------------------------------------------------------------*/
    qr_current--;
    qr_free(qr);

    return(hi);
} 							   	 	 
