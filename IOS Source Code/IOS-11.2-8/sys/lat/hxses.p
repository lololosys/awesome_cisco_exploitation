/*-----------------------------------------------------------------------
    hx_session		(x) process host-initiate state change

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called by the LAT engine when a host-initiated
    action related to the session occurs.

    CALL BY:	void hx_session(hi,reason)
    		hi_ptr	hi;
    		int	reason;

    ON EXIT:
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "lathi.h"
#include "laths.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/


void hx_session(hi,reason)

    hi_ptr	hi;			/* Pointer to HIC block		*/
    int		reason;			/* Session state reason		*/
{
    int		sts;			/* SUCCESS/FAILURE return arg	*/

/*-----------------------------------------------------------------------
    Examine possible reasons.
 -----------------------------------------------------------------------*/
    switch (reason)
    {

#if LE_OBJECT_HIC
/*-----------------------------------------------------------------------
    Process an arrived HIC session request from a host system.
 -----------------------------------------------------------------------*/
	case HX_ARRIVED:
	    break;

#if LE_OBJECT_HIC & !(LE_OBJECTS || CME_SLAT)
/*-----------------------------------------------------------------------
    Process a cancel request (only used if objects OR CME not supported).
 -----------------------------------------------------------------------*/
	case HX_CANCEL:
	    break;

#if LE_SUBJECT_HIC
/*-----------------------------------------------------------------------
    Process a request attempt timeout.
 -----------------------------------------------------------------------*/
	case HX_TIMEOUT:
	    break;

/*-----------------------------------------------------------------------
    Process a request reject.
 -----------------------------------------------------------------------*/
	case HX_REJECTED:
	    break;

/*-----------------------------------------------------------------------
    Process a request status report.
 -----------------------------------------------------------------------*/
	case HX_STATUS:
	    break;

/*-----------------------------------------------------------------------
    Process an accepted signal.
 -----------------------------------------------------------------------*/
 	case HX_ACCEPT:
	    break;
#endif
    }
}
