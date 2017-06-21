/*-----------------------------------------------------------------------
    qr_list		(e) find queued request
    
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO
    
    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved. 
 -----------------------------------------------------------------------*/

/*----------------------------------------------------------------------- 
    This function is used to find a queue request by five different
    criteria: issuing node, requested service, request port, queue
    number, and sequence. Don't care conditions are specified as zero
    or null strings. The function return a pointer to the found
    queued request.

    CALL BY:	qr_ptr qr_list(seq,que,node,name,port)
		int	seq;
		int	que;
		byte	*node;
		byte	*name;
		byte	*port;
		 
    ON EXIT:	The return argument points to the queued request found.
		If no request is found the function returns zero and
		sets lat_error

	QR_ERROR | NO_NEXT	No matching queue entry.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "lathi.h"
#include "latqr.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern qr_ptr	qr_head[2];		/* QR structure list header	*/
extern int	lat_error;		/* LAT error code		*/

qr_ptr qr_list(seq,que,node,name,port)

    int		seq;			/* Sequence number (0 first)	*/
    int		que;			/* Desired HIC entry (0 any)	*/
    byte	*node;			/* Desired node (null is any)	*/
    byte	*name;			/* Desired service (null is any)*/
    byte	*port;			/* Desired port (null is any)	*/
{
    qr_ptr	qr;			/* Pointer to QR structure	*/

/*-----------------------------------------------------------------------
    Look through all the queued request structures to find the entry
    which matches the session/HIC entry. Break when a match is found.
 -----------------------------------------------------------------------*/
    for (qr = qr_head[0]; qr != 0; qr = qr->qr_link[0])
    {	
	if (que != 0 && que != qr->qr_hi->hi_que_no)
	    ;
	else if (node != 0 && *node != 0 && !hi_str_equal_sl(&qr->qr_hi->hi_node[0],node))
	    ;
	else if (name != 0 && *name != 0 && !hi_str_equal_sl(&qr->qr_hi->hi_service[0],name))
	    ;
	else if (port != 0 && *port != 0 && !hi_str_equal_sl(&qr->qr_hi->hi_port[0],port))
	    ;
	else if (--seq < 0)
	    break;
    }

/*-----------------------------------------------------------------------
   If no request was found, set the error code before returning.
 -----------------------------------------------------------------------*/
    if (qr == 0)
        lat_error = QR_ERROR | NO_NEXT;

    return(qr);
}
