/*-----------------------------------------------------------------------
    qr_create		(i) create queue entry
	 
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO
	 
    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/
 
/*-----------------------------------------------------------------------
    This function creates a queue entry and computes its initial entry
    position.
	 
    CALL BY:	qr_ptr  qr_create(hi)
		hi_ptr	hi;

    ON EXIT:	Returns pointer to created queue entry, zero if could
    		not create entry. lat_error will be set as follows:

	QR_ERROR | MAX_LIMIT 	Maximum number queue entries reached
    	QR_ERROR | NO_MEM 	Queue structure could be allocated.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "lathi.h"
#include "latqr.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	qr_ptr	qr_head[2];		/* Queued request header	*/
extern	int	qr_maximum;		/* Maximum queued request	*/
extern	int	qr_current;		/* Current queued request	*/
extern	int	qr_highest;		/* Highest queued request	*/
extern	int	qr_changes;		/* Queue scan requested		*/
extern	word	qr_que_no;		/* Number queued request	*/
extern	int	lat_error;		/* LAT error code		*/

qr_ptr qr_create(hi)

    hi_ptr	hi;			/* Pointer to HIC entry		*/
{
    qr_ptr	qr;			/* Pointer to queue entry	*/
    qr_ptr	qx;			/* Pointer to queue entry	*/
    
/*-----------------------------------------------------------------------
    Generate a unique queue entry number (not zero).
 -----------------------------------------------------------------------*/
    while (qr_find(++qr_que_no) != 0 || qr_que_no == 0)
	;

/*-----------------------------------------------------------------------
    Preset error return.
 -----------------------------------------------------------------------*/
    qr = 0;

/*-----------------------------------------------------------------------
    Check if total queue limit reached.
 -----------------------------------------------------------------------*/
    if (qr_maximum > 0 && qr_current >= qr_maximum)
	lat_error = QR_ERROR | MAX_LIMIT;

/*-----------------------------------------------------------------------
    If session block cannot be allocated, return error
 -----------------------------------------------------------------------*/
    else if ((qr = qr_alloc()) == 0)
	lat_error = QR_ERROR | NO_MEM;

/*-----------------------------------------------------------------------
    Otherwise, initialize new queue entry.
 -----------------------------------------------------------------------*/
    else
    {
	if (++qr_current > qr_highest)
	    qr_highest++;

	qr_changes = SUCCESS;

	qr->qr_link[0] = 0;
	qr->qr_link[1] = qr_head[1];
	qr_head[1] = qr;
	if (qr->qr_link[1] != 0)
	    qr->qr_link[1]->qr_link[0] = qr;
	else
	    qr_head[0] = qr;

	qr->qr_hi = hi;
	hi->hi_qr = qr;
	hi->hi_que_no = qr_que_no;

/*-----------------------------------------------------------------------
    Determine the initial position of this entry.
 -----------------------------------------------------------------------*/
	for (qx = qr_head[0]; qx != 0; qx = qx->qr_link[0])
	{
	    if (hi_str_equal_ss(&hi->hi_service[0],&qx->qr_hi->hi_service[0]))
	        hi->hi_p_queue++;

	    hi->hi_q_queue++;
	}
    }
    return(qr);
} 							   	 	 
