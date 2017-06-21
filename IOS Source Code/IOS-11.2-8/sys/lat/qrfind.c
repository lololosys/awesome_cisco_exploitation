/* $Id: qrfind.c,v 3.2 1995/11/17 17:34:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/qrfind.c,v $
 *------------------------------------------------------------------
 * $Log: qrfind.c,v $
 * Revision 3.2  1995/11/17  17:34:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:28:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:27:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    qr_find		(e/i) find queued request by queue number
    
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO
    
    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved. 
 -----------------------------------------------------------------------*/

/*----------------------------------------------------------------------- 
    This function returns a pointer to the queued request which matches
    the supplied queue number.

    CALL BY:	qr_ptr qr_find(que_no)
		word	que_no;
 
    ON EXIT:	The return argument points to the queued request found.
		If no request is found the function returns zero and
		sets lat_error

	QR_ERROR | NO_IDENT	No matching queue entry found
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "lathi.h"
#include "latqr.h"
#include "laterr.h"
#include "latfnc.h"

extern qr_ptr	qr_head[2];		/* OB structure list header	*/
extern int	lat_error;		/* LAT error code		*/

qr_ptr qr_find(

    word	que_no)			/* Entry number			*/
{
    qr_ptr	qr;			/* Pointer to QR structure	*/

/*-----------------------------------------------------------------------
    Look through all the queued request structures to find the entry
    which matches the entry number. Break when a match is found.
 -----------------------------------------------------------------------*/
    for (qr = qr_head[0]; qr != 0; qr = qr->qr_link[0])
    {	
	if (qr->qr_hi->hi_que_no == que_no)
	    break;
    }

/*-----------------------------------------------------------------------
   If no request was found, set the error code before returning.
 -----------------------------------------------------------------------*/
    if (qr == 0)
        lat_error = QR_ERROR | NO_IDENT;

    return(qr);
}
