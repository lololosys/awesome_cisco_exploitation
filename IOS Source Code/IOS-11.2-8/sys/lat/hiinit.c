/* $Id: hiinit.c,v 3.2 1995/11/17 17:32:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hiinit.c,v $
 *------------------------------------------------------------------
 * $Log: hiinit.c,v $
 * Revision 3.2  1995/11/17  17:32:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:23:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hi_initiate		(e) initiate hic session to server

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function starts the process of opening a session to the supplied
    server node. The session is directed to the requested service and/or
    port. The queue variable should be set SUCCESS if queueing is allowed
    for the remote connect. The report variable is 0 for no reports, 1 if
    periodic status reports are desired, 2 if reports on queue changes,
    and 3 for both.

    If your enviromment has enabled CME_SLAT, this function is called from
    cm_open().  In this case cm_open() checks t0 ensure the cm structure is
    enabled for local connections, and that making a new connection will
    not exceed the specified maximum connection value.

    CALL BY:	hi_ptr hi_initiate(node,name,port,pas,sid,port_id,queue,report)
		byte	 *node;
		byte	 *name;
		byte	 *port;
		byte	 *pas;
		sid_type sid;

		cid_type port_id;
		   -- or --
		oid_type port_id;

		int 	 queue;
		int	 report;

    ON EXIT:	Returns pointer to session block if the session is
    		start, a zero value otherwise.

	HI_ERROR | NU_IDENT	SID already used by session
	HI_ERROR | NO_NAME	Illegal combination of node, port, service names
	HI_ERROR | MAX_LIMIT	Reached maximum limit of hic entries
	HI_ERROR | NO_MEM	Count not allocate hic entry
	OB_ERROR | NO_IDENT	No object exists with matching OID
	OB_ERROR | OP_ILLEGAL	Object is disabled, session not allowed
	OB_ERROR | OP_DISABLED	Object does not support local sessions
	OB_ERROR | MAX_SES	New session exceeds maximum allowed sessions
	OB_ERROR | NO_MEM	Failed to clone template object
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "lathi.h"
#include "latnd.h"
#include "latob.h"
#include "latsb.h"
#include "laterr.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	lat_error;		/* LAT error code		*/

#if CME_SLAT
extern	int	cme_error;		/* CME error code		*/
#endif

hi_ptr hi_initiate(node,name,port,pas,sid,port_id,queue,report,idb,sc)

    byte	*node;			/* Pointer to service nodename	*/
    byte	*name;			/* Pointer to service name	*/
    byte	*port;			/* Pointer to port name		*/
    byte	*pas;			/* Pointer to password		*/
    sid_type	sid;			/* Session ID			*/
#if CME_SLAT
    cid_type	port_id;		/* CME port ID			*/
#else
    oid_type	port_id;		/* Object  ID			*/
#endif
    int		queue;			/* Queueing allowed flag	*/
    int		report;			/* Report type			*/
    idbtype	*idb;			/* Interface to try		*/
    int		sc;			/* Service class		*/
{
    hi_ptr	hi;			/* Pointer to HIC block		*/
    nd_ptr	nd;			/* Pointer to remote node	*/
#if LE_OBJECTS
    ob_ptr	ob;			/* Pointer to object  block	*/
#endif

/*-----------------------------------------------------------------------
    Preset the return status to FAILURE.
 -----------------------------------------------------------------------*/
    hi = 0;

/*-----------------------------------------------------------------------
    If the host ID is not unique, fail.
 -----------------------------------------------------------------------*/
    if (sb_find(sid) != (sb_ptr)0)
	lat_error = HI_ERROR | NU_IDENT;

    else if (hi_session(sid) != 0)
	lat_error = HI_ERROR | NU_IDENT;

/*-----------------------------------------------------------------------
    Check for illegal name combinations. It is illegal to have no
    node and service name or no port and service name.
 -----------------------------------------------------------------------*/
    else if ((node == 0 || *node == 0) && (name == 0 || *name == 0))
        lat_error = HI_ERROR | NO_NAME;

    else if ((port == 0 || *port == 0) && (name == 0 || *name == 0))
        lat_error = HI_ERROR | NO_NAME;

#if LE_OBJECTS
/*-----------------------------------------------------------------------
    If the object identifer does not support an outgoing connection, fail.
 -----------------------------------------------------------------------*/
    else if ((ob = ob_find(port_id)) == 0)
	lat_error = OB_ERROR | NO_IDENT;

    else if (!(ob->ob_flags & OB_ENABLED))
	lat_error = OB_ERROR | OP_DISABLED;

    else if (ob->ob_flags & OB_REMOTE)
	lat_error = OB_ERROR | OP_ILLEGAL;

/*-----------------------------------------------------------------------
    Check that object allows this new session.
 -----------------------------------------------------------------------*/
    else if (ob->ob_lcl_max != 0 && ob->ob_lcl_use >= ob->ob_lcl_max)
	lat_error = OB_ERROR | MAX_SES;

/*-----------------------------------------------------------------------
    Check if template, and if so clone.
 -----------------------------------------------------------------------*/
    else if ((ob = ob_clone(ob)) == 0)
	lat_error = OB_ERROR | NO_MEM;
#endif

/*-----------------------------------------------------------------------
    All is well. Initialize the HI structure to send solicit message.
 -----------------------------------------------------------------------*/
    else if ((hi = hi_create(HI_SUBJECT)) != 0)
    {
	if (node != 0 && (nd = nd_show(node,idb)) != 0)
	{
	    hi->hi_function = LAT_COMMAND;
	    hi->hi_dest_addr[0] = nd->nd_host_addr[0];
	    hi->hi_dest_addr[1] = nd->nd_host_addr[1];
	    hi->hi_dest_addr[2] = nd->nd_host_addr[2];
	    hi->hi_idb = nd->nd_idb;
	}
	else
	{
	    hi->hi_function = LAT_SOLICIT;
	    hi->hi_dest_addr[0] = hs->hs_multicast[0];
	    hi->hi_dest_addr[1] = hs->hs_multicast[1];
	    hi->hi_dest_addr[2] = hs->hs_multicast[2];
	    hi->hi_idb = idb;
	    hi->hi_resp_status = 0;
	}

	hi->hi_sid      = sid;

#if CME_SLAT
	hi->hi_cid	= port_id;
#endif

	hi->hi_tics     = 1;
	hi->hi_retries  = hs->hs_hi_cmd_retries;

	hi->hi_command  = (byte)((queue) ? CMD_ACCESS_Q : CMD_ACCESS);
	hi->hi_modifier = (byte)((queue) ? report : 0);

	if (node != 0)
	    hi_str_move_sl(&hi->hi_node[0],node);
	if (name != 0)
	    hi_str_move_sl(&hi->hi_service[0],name);
	if (port != 0)
 	    hi_str_move_sl(&hi->hi_port[0],port);
	if (pas != 0)
	    hi_str_move_sl(&hi->hi_pass[0],pas);

#if LE_OBJECTS
	hi->hi_ob     = ob;

	ob->ob_flags |= OB_LOCAL;
	ob->ob_lcl_use++;
#endif

	hi->hi_sc = sc;		/* Save service class */
    }
    return(hi);
}
