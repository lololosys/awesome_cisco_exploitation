/*-----------------------------------------------------------------------
    hi_o_request	(i) output host-initiated request status

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called to output the request part of a host-initiated
    status message. The timer is reset if periodic requests should be made.

    CALL BY:	of_byte hi_o_request(ptr,hi)
		of_byte	ptr;
		hi_ptr	hi;

    ON EXIT:	Appropriate request status stored in message.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "lathi.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	tu_msec_tic;		/* Milliseconds per tic		*/

of_byte hi_o_request(ptr,hi)

    of_byte	ptr;			/* Output frame pointer		*/
    hi_ptr	hi;			/* HIC request pointer		*/
{
    req_optr	req;			/* Output entry field		*/

#if !(LE_OBJECTS || CME_SLAT)
/*-----------------------------------------------------------------------
    If no object support and not using CME, call environment to
    get request status.
 -----------------------------------------------------------------------*/
    hx_status(hi);
#endif

/*-----------------------------------------------------------------------
    Complete the variable fields in a status message.
 -----------------------------------------------------------------------*/
    req  = (req_iptr)ptr;
    ptr += sizeof(req_field);

    req->req_status   = hi->hi_status;
    req->req_error    = hi->hi_error;
    req->req_req_no   = swap(hi->hi_req_no);
    req->req_que_no   = swap(hi->hi_que_no);
    req->req_que_time = swap(hi->hi_que_time);
    req->req_p_queue  = swap(hi->hi_p_queue);
    req->req_q_queue  = swap(hi->hi_q_queue);

    ptr = (of_byte)hi_str_move_os(ptr,&hi->hi_service[0]);
    ptr = (of_byte)hi_str_move_os(ptr,&hi->hi_port[0]);
    *ptr++ = 0;

    req->req_length = (byte)(ptr - (of_byte)req - 1);

    if ((int)(ptr - (of_byte)req) & 1)
	*ptr++ = 0;

/*-----------------------------------------------------------------------
    If delivering error status, flag to delete the hi entry.
 -----------------------------------------------------------------------*/
    if (hi->hi_status & STS_ERROR)
        hi->hi_flags |= HI_DELETE;

/*-----------------------------------------------------------------------
    If periodic status, reset the timer and mark as multiple.
 -----------------------------------------------------------------------*/
    if (hi->hi_modifier & CMD_SEND_TIMED)
    {
	hi->hi_flags |= HI_MULTI;
	hi->hi_tics    = (1000 / tu_msec_tic) * hs->hs_hi_sts_timer;
	hi->hi_retries = 1;
    }
    else
    {
	hi->hi_flags &= ~HI_MULTI;
	hi->hi_tics    = 0;
	hi->hi_retries = 0;
    }

    return(ptr);
}
