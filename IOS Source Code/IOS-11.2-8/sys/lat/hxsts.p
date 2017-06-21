/*-----------------------------------------------------------------------
    hx_status		(x) report hic entry status

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called by the LAT engine to get the status of an
    HIC entry when the server is generating an HIC status message. The
    hx_status function is only needed when HIC processing is done by the
    environment (LE_OBJECTS set to 0).

    CALL BY:	void hx_status(hi)
    		hi_ptr	hi;

    ON EXIT:	hi requests status fields filled with correct values.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "lathi.h"
#include "latmsg.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"

void hx_status(hi)

    hi_ptr	hi;			/* Pointer to HIC block		*/
{

/*-----------------------------------------------------------------------
    Fill in the status fields of the HI structure.
 -----------------------------------------------------------------------*/
    hi->hi_que_no   = <queue entry number>;
    hi->hi_que_time = <time spent in queue, -1 if not kept>;
    hi->hi_p_queue  = <position for requested sevrice/port>;
    hi->hi_q_queue  = <position within total queue>;

/*-----------------------------------------------------------------------
    If the request status has changed from normal,queued then also
    updates the status and reason fields (see latmsg.h, latslt.h).
    The following example shows how the status would be changed if
    the HIC entry has some error and should be deleted.
 -----------------------------------------------------------------------*/
    hi->hi_status = STS_ERROR;
    hi->hi_error  = SLOT_STOP_<error reason>;
}
