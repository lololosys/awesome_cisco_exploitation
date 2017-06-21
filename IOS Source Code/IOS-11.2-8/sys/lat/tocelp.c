/*-----------------------------------------------------------------------
    toc_elapse		(e) process toc timer

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called by the environment for each toc (10 seconds).
    We perform timeout checks on learned services and issue advertised
    service message if its time has expired. The counters seconds since
    zeroed and time in queue timers are updated.

    Call with force set non-zero if a directory service message should be
    forced out, zero if a regular interval call. A force output will reset
    the multicast interval.

    CALL BY:	void toc_elase(force)
    		int	force;

    ON EXIT:	All multicast time based processing performed.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "lathi.h"
#include "latnd.h"
#include "latqr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr		hs;		/* Local node structure		*/
extern	int		tu_secs_toc;	/* Seconds per toc		*/

void toc_elapse(force)

    int		force;			/* Force service output		*/
{

/*-----------------------------------------------------------------------
    If real toc call, update timers and perform learned service timeout.
 -----------------------------------------------------------------------*/
    if (!force)
    {

#if LE_SERVER_LS
/*-----------------------------------------------------------------------
    Delete any learned services for which we have not had an current
    update message.
 -----------------------------------------------------------------------*/
        ls_timeout();
#endif

#if LE_HS_STATS
/*-----------------------------------------------------------------------
    Update the host counter timer.
 -----------------------------------------------------------------------*/
	if (hs->hs_stats.co_seconds < hs->hs_stats.co_seconds + tu_secs_toc)
	    hs->hs_stats.co_seconds += tu_secs_toc;
#endif

#if LE_ND_STATS
/*-----------------------------------------------------------------------
    Update the remote node counter timers.
 -----------------------------------------------------------------------*/
	{
    	    nd_ptr  nd;
    	    int	    i;
	
	    for (i = 0; i < ND_HASHES; i++)
            {
	        for (nd = hs->hs_nd_hash[i][0]; nd != 0; nd = nd->nd_link[0])
	        {
	            if (nd->nd_stats.co_seconds < nd->nd_stats.co_seconds + tu_secs_toc)
	                nd->nd_stats.co_seconds += tu_secs_toc;
	        }
	    }
        }
#endif

#if (LE_OBJECTS ||CME_SLAT) && LE_OBJECT_HIC
/*-----------------------------------------------------------------------
    Update the time queue entries have spent in the queue.
 -----------------------------------------------------------------------*/
	{
	    extern  qr_ptr qr_head[2];
	            qr_ptr qr;
	
	    for (qr = qr_head[0]; qr != 0; qr = qr->qr_link[0])
            {
	        if (qr->qr_hi->hi_que_time < qr->qr_hi->hi_que_time + tu_secs_toc)
	    	    qr->qr_hi->hi_que_time += (word)(LOWORD(tu_secs_toc));
            }
	}
#endif
    }

#if LE_HOST_AS
/*-----------------------------------------------------------------------
    If it is time or we are to force an output, issue the next advertised
    service message if an output buffer is available. If no buffer, we will
    try again at the next TOC interval.
 -----------------------------------------------------------------------*/
    if (!force && hs->hs_as_elapse > (byte)(LOBYTE(tu_secs_toc)))
	hs->hs_as_elapse -= (byte)(LOBYTE(tu_secs_toc));
    else
    {
        of_ptr	of;

	if ((of = of_alloc()) != 0)

/*-----------------------------------------------------------------------
    Reset the timer and generate the advertised service message. If we
    are not offering any services at this time, don't send the message.
 -----------------------------------------------------------------------*/
	{
	    hs->hs_as_elapse = hs->hs_as_timer;
	    if (as_message(of))
		of_message(of);
	    else
		of_free(of);
	}
    }
#endif
}
