/*-----------------------------------------------------------------------
    qr_scan		(i) scan queue for possible actions

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This routine scans the queued requests for any possible action:
    starting queue entry and/or deleting because of changed status.

    CALL BY:	void qr_scan()

    ON EXIT:	Queue updated as needed.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latas.h"
#include "lathi.h"
#include "latob.h"
#include "latqr.h"
#include "latmsg.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

#if LE_OBJECTS
extern	ob_ptr	ob_map[];		/* Object mapping table		*/
extern	int	ob_highest;		/* Largest number of objects	*/
#endif
#if CME_SLAT
extern	int	cm_port_maximum;	/* Maximum number of CME ports	*/
#endif

extern	qr_ptr	qr_head[2];		/* QR structure list header	*/
extern	byte	as_temp[];		/* Temporary AS name		*/
extern	byte	ob_temp[];		/* Temporary OB name		*/

void qr_scan()

{
#if (LE_OBJECTS || CME_SLAT)
    qr_ptr	qr;			/* Pointer to QR structure	*/
    qr_ptr	qx;			/* Pointer to QR structure	*/
    hi_ptr	hi;			/* Pointer to HI structure	*/
    as_ptr	as;			/* Pointer to AS structure	*/

#if LE_OBJECTS
    ob_ptr	ob;			/* Pointer to OB structure	*/
#endif

#if CME_SLAT
    cid_type	cid;			/* CM port identifier		*/
    int		flags;			/* CM flags			*/
#endif

    int		err;			/* Error status value		*/
    int		que;			/* Queuing allowed flag		*/
    int		i;			/* Do index			*/

/*-----------------------------------------------------------------------
    Look through all the queued request structures and check each
    against current service/object status.
 -----------------------------------------------------------------------*/
    for (qr = qr_head[0]; qr != 0; qr = qx)
    {
	qx  = qr->qr_link[0];
	hi  = qr->qr_hi;
	as  = 0;
	err = 0;
	que = 0;

#if LE_OBJECTS
	ob  = 0;
#elif CME_SLAT
	cid = CID_NULL;
#endif

/*-----------------------------------------------------------------------
    If port specified, lookup port name and declare error if it does
    not exist or has been disabled.
 -----------------------------------------------------------------------*/
	if (hi->hi_port[0] != 0)
	{
	    hi_str_move_ls(&ob_temp[0],&hi->hi_port[0]);

#if LE_OBJECTS
	    if ((ob = ob_show(&ob_temp[0])) == 0)
		err = SLOT_STOP_PORT_UNKNOWN;

	    else if (!(ob->ob_flags & OB_ENABLED))
	        err = SLOT_STOP_SRV_DISABLED;

	    else
	        que = ob->ob_flags & OB_QUEUED;
#elif CME_SLAT
	    if ((cid = cx_show(&ob_temp[0])) == CID_NULL)
		err = SLOT_STOP_PORT_UNKNOWN;
	    else if (!((flags = cx_flags(cid)) & CM_ENABLED))
		err = SLOT_STOP_SRV_DISABLED;
	    else
		que = flags & CM_QUEUED;
#endif
	}

/*-----------------------------------------------------------------------
    If service specified, lookup service name and declare error if it
    does not exist or has been disabled. If port also specified, make
    sure it is offered by the service.
 -----------------------------------------------------------------------*/
	if (hi->hi_service[0] != 0)
	{
	    hi_str_move_ls(&as_temp[0],&hi->hi_service[0]);

	    if ((as = as_show(&as_temp[0])) == 0)
		err = SLOT_STOP_SRV_UNKNOWN;

	    else if (!(as->as_flags & AS_ENABLED))
	        err = SLOT_STOP_SRV_DISABLED;

#if LE_OBJECTS
	    else if (ob != 0)
	    {
	        for (i = 0; i < ob_highest; i++)
		{
		    if (ob_map[i] == ob && (as->as_ob_map[i/8] & (1 << (i%8))))
		        break;
		}
		if (i == ob_highest)
		    err = SLOT_STOP_PORT_UNKNOWN;
#elif CME_SLAT
	    else if (cid != CID_NULL)
	    {
		for (i = 0; i < cm_port_maximum; i++)
		{
		    if ((i == cid && as->as_port_map[i/8] & (1 << (i%8))))
			break;
		}
		if (i == cm_port_maximum)
		    err = SLOT_STOP_PORT_UNKNOWN;
#endif
/*-----------------------------------------------------------------------
    Only queue if service allows queuing.
 -----------------------------------------------------------------------*/
		else
		    que = que && (as->as_flags & AS_QUEUED);
	    }
	else
	    que = as->as_flags & AS_QUEUED;
	}

/*-----------------------------------------------------------------------
    If no error so far, see if this request can be started.
 -----------------------------------------------------------------------*/
	if (err == 0)
	{

#if LE_OBJECTS
/*-----------------------------------------------------------------------
    If we have an object, see if new remote session may be started.
 -----------------------------------------------------------------------*/
 	    if (ob != 0)
	    {
		if ((ob->ob_flags & (OB_LOCAL | OB_BUSY)) || (ob->ob_rmt_use >= ob->ob_rmt_max))
		    ob = 0;
	    }

/*-----------------------------------------------------------------------
    If we have a service, see if object offered by the service may
    be started.
 -----------------------------------------------------------------------*/
 	    else if (as != 0)
	    {
		i = as->as_last_object;
		do
		{
		    if (++i >= ob_highest)
			i = 0;

		    if (ob_map[i] != 0 && as->as_ob_map[i/8] & (1 << (i%8)))
		    {
			if (!(ob_map[i]->ob_flags & (OB_LOCAL | OB_BUSY)) &&
		             (ob_map[i]->ob_flags & OB_ENABLED)           &&
		             (ob_map[i]->ob_rmt_use < ob_map[i]->ob_rmt_max))
			{
			    ob = ob_map[i];
			    as->as_last_object = i;
			    break;
			}
		    }
		} while (i != as->as_last_object);
	    }

#elif CME_SLAT
/*-----------------------------------------------------------------------
    If we have a CME port, see if new remote session may be started.
 -----------------------------------------------------------------------*/
	    if (cid != CID_NULL)
	    {
		if (cx_usage(cid, CM_RESERVE | CM_REMOTE) == FAILURE)
		    cid = CID_NULL;
	    }

/*-----------------------------------------------------------------------
    If we have a service, see if CME port offered by the service may
    be started.
 -----------------------------------------------------------------------*/
	    else if (as != 0)
	    {
		i = as->as_last_port;
		do
		{
		    if (++i >= cm_port_maximum)
			i = 0;
		    if (as->as_port_map[i/8] & (1 << (i%8)))
		    {
			if (cx_usage(i, CM_RESERVE | CM_REMOTE) == SUCCESS)
			{
			    cid = i;
			    as->as_last_port = i;
			    break;
			}
		    }
		} while (i != as->as_last_port);
	    }
#endif

/*-----------------------------------------------------------------------
    If we have neither, this is an illegal request.
 -----------------------------------------------------------------------*/
	    else
	        err = SLOT_STOP_BAD_COMMAND;

/*-----------------------------------------------------------------------
    If we got an object, start it. Otherwise see if request should
    remained queued or an immediate access error returned.
 -----------------------------------------------------------------------*/
#if LE_OBJECTS
 	    if (ob != 0 && (ob = ob_clone(ob)) != 0)
	    {
		ob->ob_flags |= OB_REMOTE;
		ob->ob_rmt_use++;
		hi->hi_ob = ob;
#elif CME_SLAT
	    if (cid != CID_NULL)
	    {
		hi->hi_cid = cid;
#endif
		qr_delete(qr);
#if LE_SERVER
		hi->hi_tics = 100;
		hx_session(hi,HX_ARRIVED);
#else
		hi->hi_tics = 1;
		hi->hi_retries = 1;
		hi->hi_status = STS_STARTED;
#endif
	    }

	    else if (hi->hi_command == CMD_ACCESS   && !que)
		err = SLOT_STOP_SRV_IN_USE;
	    else if (hi->hi_command == CMD_ACCESS   &&  que)
	        err = SLOT_STOP_IMMED_REJECT;
	    else if (hi->hi_command == CMD_ACCESS_Q && !que)
	        err = SLOT_STOP_IMMED_REJECT;
	}

/*-----------------------------------------------------------------------
    If any error occured, delete the queue entry and send back a status
    error message.
 -----------------------------------------------------------------------*/
	if (err != 0)
	{
	    qr_delete(qr);
	    hi_reject(hi,err);
	}
    }
#endif
}
