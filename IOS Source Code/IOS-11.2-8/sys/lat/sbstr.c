/*-----------------------------------------------------------------------
    sb_start		(i) start session by service name

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called by the LAT engine when a start slot with
    a service name arrives. The function checks if the session can be
    started and if so, notifies the environment. Otherwise an reject
    reason is returned in the session block.

    CALL BY:	void sb_start(sb,name,port)
		sb_ptr	sb;
		byte	*name;
		byte	*port;

    ON EXIT:	sb_stop_reason set non-zero if session is rejected.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latas.h"
#include "latob.h"
#include "latsb.h"
#include "latmsg.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	byte	id_temp[];		/* Password string		*/
#if LE_OBJECTS
extern	ob_ptr	ob_map[];		/* Object mapping table		*/
extern	int	ob_highest;		/* Largest number of objects	*/
#endif
#if CME_SLAT
extern	int	cm_port_maximum;	/* Maximum number of CME ports	*/
#endif

void sb_start(sb,name,port)

    sb_ptr	sb;			/* Pointer to session block	*/
    byte	*name;			/* Pointer to requested name	*/
    byte	*port;			/* Pointer to port name		*/
{
    as_ptr	as;			/* Pointer to advertise service	*/
#if (LE_OBJECTS || CME_SLAT)
    int		i;			/* Do index			*/
#if LE_OBJECTS
    ob_ptr	ob;			/* Pointer to object		*/
#elif CME_SLAT
    cid_type	cid;			/* CM port identifier		*/
#endif
#endif

/*-----------------------------------------------------------------------
    See if connects are disabled for node.
 -----------------------------------------------------------------------*/
    if (hs->hs_status & SVC_NODE_OFF)
        sb->sb_stop_reason = SLOT_STOP_SYSTEM;

/*-----------------------------------------------------------------------
    See if name is defined at this node.
 -----------------------------------------------------------------------*/
    else if ((as = as_show(name)) == 0)
        sb->sb_stop_reason = SLOT_STOP_SRV_UNKNOWN;

/*-----------------------------------------------------------------------
    See if service is password protected.
 -----------------------------------------------------------------------*/
    else if (as->as_password[0] != 0 && as_str_diff_sl(&as->as_password[0],&id_temp[0]) != 0)
        sb->sb_stop_reason = SLOT_STOP_PASSWORD;

/*-----------------------------------------------------------------------
    See if service supports connections.
 -----------------------------------------------------------------------*/
    else if (!(as->as_flags & AS_ENABLED))
	sb->sb_stop_reason = SLOT_STOP_SRV_DISABLED;

    else
    {
/*-----------------------------------------------------------------------
    If a specific port was named in the start slot, see if known,
    available to service, and in correct state for new session.
 -----------------------------------------------------------------------*/
	if (port[0] != 0)
	{
#if LE_OBJECTS
	    if ((ob = ob_show(port)) == 0)
	        sb->sb_stop_reason = SLOT_STOP_PORT_UNKNOWN;

	    else
	    {
	        for (i = 0; i < ob_highest; i++)
		{
		    if (ob_map[i] == ob && (as->as_ob_map[i/8] & (1 << (i%8))))
		        break;
		}

		if (i == ob_highest)
		    sb->sb_stop_reason = SLOT_STOP_SRV_NOT_OFFER;

		else if (ob->ob_flags & (OB_TEMPLATE | OB_CLONED))
		    sb->sb_stop_reason = SLOT_STOP_SRV_NOT_OFFER;

	        else if (ob->ob_flags & OB_BUSY)
		    sb->sb_stop_reason = SLOT_STOP_SRV_IN_USE;

	        else if (!(ob->ob_flags & OB_ENABLED))
		    sb->sb_stop_reason = SLOT_STOP_SRV_IN_USE;

	        else if (!(ob->ob_flags & (OB_REMOTE | OB_DYNAMIC)))
		    sb->sb_stop_reason = SLOT_STOP_SRV_IN_USE;

	        else if (ob->ob_rmt_use >= ob->ob_rmt_max)
		    sb->sb_stop_reason = SLOT_STOP_SRV_IN_USE;
	    }

#elif CME_SLAT
	    if ((cid = cx_show(port)) == CID_NULL)
		sb->sb_stop_reason = SLOT_STOP_PORT_UNKNOWN;
	    else if (!(as->as_port_map[cid/8] & (1 << (cid%8))))
		sb->sb_stop_reason = SLOT_STOP_SRV_NOT_OFFER;
	    else if (cx_usage(cid, CM_RESERVE | CM_REMOTE) == FAILURE)
		sb->sb_stop_reason = SLOT_STOP_SRV_IN_USE;
#endif
	}

/*-----------------------------------------------------------------------
    See if service supports objects, and if so, find an available
    object. The default error is disabled, change to in use if we
    find at least one remote, busy or in use port.
 -----------------------------------------------------------------------*/
	else
	{
#if LE_OBJECTS
	    sb->sb_stop_reason = SLOT_STOP_SRV_DISABLED;
	    i = as->as_last_object;
	    do
	    {
		if (++i >= ob_highest)
		    i = 0;
		if (as->as_ob_map[i/8] & (1 << (i%8)))
		{
	            ob = ob_map[i];

		    if (ob->ob_flags & OB_BUSY)
	 	    	sb->sb_stop_reason = SLOT_STOP_SRV_IN_USE;
	            else if (!(ob->ob_flags & (OB_REMOTE | OB_DYNAMIC)))
	            	;

	            else if (ob->ob_rmt_use >= ob->ob_rmt_max)
	 	    	sb->sb_stop_reason = SLOT_STOP_SRV_IN_USE;

	            else if (!(ob->ob_flags & OB_ENABLED))
	            	;

	            else if ((ob = ob_clone(ob)) == 0)
	 	    	sb->sb_stop_reason = SLOT_STOP_SRV_IN_USE;

	            else
		    {
	            	sb->sb_stop_reason = 0;
			as->as_last_object = i;
	            	break;
		     }
		}
	    } while ( i != as->as_last_object);

#elif CME_SLAT
	    sb->sb_stop_reason = SLOT_STOP_SRV_DISABLED;
	    i = as->as_last_port;
	    do
	    {
		if (++i >= cm_port_maximum)
		    i = 0;
		if (as->as_port_map[i/8] & (1 << (8%i)))
		{
		    if (cx_usage(i, CM_RESERVE | CM_REMOTE) == FAILURE)
			sb->sb_stop_reason = SLOT_STOP_SRV_IN_USE;
		    else
		    {
			sb->sb_stop_reason = 0;
			sb->sb_cid = i;
			as->as_last_port = i;
			break;
		    }
		}
	    } while (i != as->as_last_port);

#endif
	}

/*-----------------------------------------------------------------------
    If we get here with no error, we got an object. Mark for a remote
    session.
 -----------------------------------------------------------------------*/
#if LE_OBJECTS
	if (sb->sb_stop_reason == 0)
	{
	    sb->sb_ob     = ob;
	    ob->ob_flags |= OB_REMOTE;
	    ob->ob_rmt_use++;
	}
#endif
    }

/*-----------------------------------------------------------------------
    If engine does not manage objects or an object or CME port was found,
    tell the environment about the incoming connection.
 -----------------------------------------------------------------------*/
    if (sb->sb_stop_reason == 0)
    {
	sb->sb_flags = SB_STALL;
	sx_session(sb,SX_AS_ACCESS);
    }
}
