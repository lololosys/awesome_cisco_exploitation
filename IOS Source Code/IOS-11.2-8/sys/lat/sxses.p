/*-----------------------------------------------------------------------
    sx_session		(x) process session state change

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called by the LAT engine when the session state
    changes.

    CALL BY:	void sx_session(sb,reason)
    		sb_ptr	sb;
    		int	reason;

    ON EXIT:	Client is notified of session state change.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latcb.h"
#include "laths.h"
#include "latsb.h"
#include "latmsg.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

void sx_session(sb,reason)

    sb_ptr	sb;			/* Pointer to session block	*/
    int		reason;			/* Session delete reason	*/
{
    ses_ptr	ses;			/* Pointer to session		*/

/*-----------------------------------------------------------------------
    Using the SID, Get the environment session data.
 -----------------------------------------------------------------------*/
    <get environment session data from sb->sb_sid>

/*-----------------------------------------------------------------------
    Check out possible values for reason.
 -----------------------------------------------------------------------*/
    switch (reason)
    {

#if LE_HOST
/*-----------------------------------------------------------------------
    A new remote session (start/by service) is trying to access us.
    If the host wants to accept the session at this time, it can call
    sb_accept. If the host wants to reject the session at this time,
    it calls sb_reject.
 -----------------------------------------------------------------------*/
	case SX_AS_ACCESS:
	    break;
#endif

#if LE_SUBJECT_HIC
/*-----------------------------------------------------------------------
    A start has been received for a session we requested using hi_intiate
    (start/by HIC). If the host wants to accept the session at this time,
    it can call sb_accept. If the host wants to reject the session at this
    time, it calls sb_reject.
 -----------------------------------------------------------------------*/
	case SX_HI_ACCESS:
	    break;
#endif

/*-----------------------------------------------------------------------
    If session is going down, abort the session. Use the sb_stop_reason
    to get the error code.  (NOTE:  In many implementations, SX_STOP,
    SX_REJECT, and SX_CIRCUIT cases may be handled the same way.)
 -----------------------------------------------------------------------*/
        case SX_STOP:
	    break;

	case SX_REJECT:
	    break;

/*-----------------------------------------------------------------------
    If circuit is stopping, abort the session. Classify the reason
    from the cb_halt_reason value.
 -----------------------------------------------------------------------*/
        case SX_CIRCUIT:
	    break;

#if LE_SERVER
/*-----------------------------------------------------------------------
    A start slot exchange has completed, the server session is now
    ready for character I/O.
 -----------------------------------------------------------------------*/
	case SX_CONNECT:
	    break;

/*-----------------------------------------------------------------------
    A start slot exchange has completed for a host initiated connect,
    the server session is now ready for character I/O.
 -----------------------------------------------------------------------*/
	case SX_CONNECT_HIC:
	    break;

/*-----------------------------------------------------------------------
    If the service to which you are trying to connect is not in the 
    learned service cache, the engine will solicit Agent/Host nodes for
    the service name in an attempt to complete the connection.  If no 
    response is received, sx_session will be called with the reason
    SX_TIMEOUT; if a response is received but there are not sufficient
    resources on the local node to complete the connection, the reason
    will be SX_RESOURCE.  In either case, the session will be aborted
    upon return from the sx_session call.  You should take whatever action
    is appropriate to free any environment resources you have associated
    with the session connect attempt.
 -----------------------------------------------------------------------*/
	case SX_TIMEOUT:
	case SX_RESOURCE:
	    break;

#endif
    }
}
