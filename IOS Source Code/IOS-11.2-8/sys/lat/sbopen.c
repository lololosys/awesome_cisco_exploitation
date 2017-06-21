/* $Id: sbopen.c,v 3.2 1995/11/17 17:35:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/sbopen.c,v $
 *------------------------------------------------------------------
 * $Log: sbopen.c,v $
 * Revision 3.2  1995/11/17  17:35:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:28:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:28:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    sb_open		(e) open service session to host

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function starts the process of opening a session to the supplied
    service name. The supplied name is used to match a learned service and
    then a session block (and possibly circuit block) is created. The start
    slot message will be sent at the next circuit interval.

    If no service name is found we may solicit host/agent nodes for the
    service.  Then the start message(s) will be sent upon receipt of a
    response information message.

    Note, the node, port, and pass arguments may be specified as null
    pointers or null strings if they do not apply to connection.

    Also, if your environment has enabled CME_SLAT, sb_open is called from
    cm_open.  In this case cm_open checks to ensure the cm structure is enabled
    for local connections, and that making a new connection will not exceed
    the specified maximum connection value.

    CALL BY:	sb_ptr sb_open(name,sid,port_id,credits,node,port,pass)
		byte	 *name;
		sid_type sid;

		cid_type port_id;	if using CME
		   -- or  --
		oid_type port_id;	if not using CME

    		int	 credits;
		byte	 *node;
		byte	 *port;
		byte	 *pass;

    ON EXIT:	Returns pointer to session block if the session is
    		start, a zero value otherwise. If error, lat_error
		will be set as follows:

	SB_ERROR | NU_IDENT	SID is already used by session
	SB_ERROR | MAX_LIMIT	Reached limit of sessions on circuit or host
	SB_ERROR | NO_MEM	Could not allocate session block
	CB_ERROR | MAX_LIMIT	Reached limit of number of circuits
	CB_ERROR | NO_MEM	Could not allocate circuit block
	OB_ERROR | NO_IDENT	No object exists with matching OID
	OB_ERROR | MAX_SES	New sesison exceeds maximum allowed sessions
	OB_EROOR | OP_DISABLED	Object is disabled, session not allowed
	OB_ERROR | OP_ILLEGAL	Object does not support local sessions
	OB_ERROR | NO_MEM	Could not allocate clone object for template
	LS_ERROR | NO_NAME	No such service name exists
	HI_ERROR | NO_MEM	Could not allocate HIC entry
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latls.h"
#include "latnd.h"
#include "latob.h"
#include "latsb.h"
#include "lathi.h"
#include "laterr.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	lat_error;		/* LAT error code		*/

#if CME_SLAT
extern	int	cme_error;		/* CME error code		*/
#endif

sb_ptr sb_open(name,sid,port_id,credits,node,port,pass,sc)

    byte	*name;			/* Pointer to service name	*/
    sid_type	sid;			/* Session ID 			*/
#if CME_SLAT
    cid_type	port_id;		/* CME port ID			*/
#else
    oid_type	port_id;		/* Object  ID			*/
#endif
    int		credits;		/* Number of credits to extend	*/
    byte	*node;			/* Remote node name		*/
    byte	*port;			/* Remote port name		*/
    byte	*pass;			/* Pointer to password		*/
    int		sc;			/* Service class		*/
{
    sb_ptr	sb;			/* Pointer to session block	*/
    cb_ptr	cb;			/* Pointer to circuit block	*/
    nd_ptr	nd;			/* Pointer to remote node	*/
#if CME_SLAT
    cid_type	ob;			/* CME port identifier		*/
#else
    ob_ptr	ob;			/* Pointer to object  block	*/
#endif
    ls_ptr	ls;			/* Pointer to learned service	*/
#if LE_SUBJECT_HIC
    hi_ptr	hi;			/* Pointer to HIC		*/
#endif
    int		i;			/* Do index			*/

/*-----------------------------------------------------------------------
    Preset the return status to FAILURE.
 -----------------------------------------------------------------------*/
    sb = 0;
#if !CME_SLAT
    ob = 0;
#else
    ob = port_id;
#endif
#if LE_SUBJECT_HIC
    hi = 0;
#endif

/*-----------------------------------------------------------------------
    If the session ID is not unique, fail.
 -----------------------------------------------------------------------*/
   if (sb_find(sid) != 0)
	lat_error = SB_ERROR | NU_IDENT;

#if LE_SUBJECT_HIC || LE_OBJECT_HIC
   else if (hi_session(sid) != 0)
	lat_error = SB_ERROR | NU_IDENT;
#endif

#if LE_OBJECTS
/*-----------------------------------------------------------------------
    If the object identifer is not valid, fail.
 -----------------------------------------------------------------------*/
    else if ((ob = ob_find(port_id)) == 0)
	lat_error = OB_ERROR | NO_IDENT;

    else if (!(ob->ob_flags & OB_ENABLED))
	lat_error = OB_ERROR | OP_DISABLED;

/*-----------------------------------------------------------------------
    If the object does not support outgoing connections, fail.
 -----------------------------------------------------------------------*/
    else if (ob->ob_flags & OB_REMOTE)
	lat_error = OB_ERROR | OP_ILLEGAL;

/*-----------------------------------------------------------------------
    Check that object allows this new session.
 -----------------------------------------------------------------------*/
    else if (ob->ob_lcl_max != 0 && ob->ob_lcl_use >= ob->ob_lcl_max)
	lat_error = OB_ERROR | MAX_SES;

/*-----------------------------------------------------------------------
    Check for template object and clone if necessary.
 -----------------------------------------------------------------------*/
    else if ((ob = ob_clone(ob)) == 0)
	lat_error = OB_ERROR | NO_MEM;

#endif

/*-----------------------------------------------------------------------
    If there is no learned service with this name or the service is
    not valid, try to find a service by sending a solicit message.
    If we take the solicit route, we know we don't have the information
    we need to call sb_create(), so we will allocate an sb structure,
    fill in all the information we can and fill in the remainder and
    link the session block to the circuit block when we get the response
    information message.  If we don't get a response information message
    we will undo everything we have done here in routine sb_ri_action().
 -----------------------------------------------------------------------*/
    else if ((ls = ls_show(name,node,ob,ND_UNKNOWN|ND_UNREACH)) == 0)
#if LE_SUBJECT_HIC
    {
	if (name == 0 || *name == 0)
	    lat_error = LS_ERROR | NO_NAME;
	else if (hs->hs_sb_maximum > 0 && hs->hs_sb_current >= hs->hs_sb_maximum)
	    lat_error = SB_ERROR | MAX_LIMIT;
	else if ((sb = sb_alloc()) == 0 || (hi = hi_create(HI_SUBJECT)) == 0)
	{
	    if (sb == 0)
	    	lat_error = SB_ERROR | NO_MEM;
	    else if (hi == 0)
	    	lat_error = HI_ERROR | NO_MEM;
	    if (sb != 0)
	    {
	    	sb_free(sb);
		sb = 0;
	    }
	    if (hi != 0)
	    	hi_delete(hi,0);
	}
	else
	{
	    if (++hs->hs_sb_current > hs->hs_sb_highest)
	    	hs->hs_sb_highest++;
	
/*-----------------------------------------------------------------------
    Initialize as much of the session block as we can.
 -----------------------------------------------------------------------*/
#if LE_OBJECTS
	    sb->sb_ob = ob;
	    ob->ob_flags |= OB_LOCAL;
	    ob->ob_lcl_use++;
#else
	    sb->sb_ob = 0;
#endif
	    sb->sb_sid = sid;
#if CME_SLAT
	    sb->sb_cid = port_id;
#endif

	    sb->sb_cb = 0;
	
	    sb->sb_flags = 0;
	    if (node != 0 && *node != 0)
		sb->sb_flags|= SB_NODE;
	    sb->sb_req_no = 0;
	
	    sb->sb_rmt_ses_id = 0;

	    sb->sb_rmt_credits = 0;
	    sb->sb_lcl_credits = credits;
	    sb->sb_ext_credits = 0;
	    sb->sb_stop_reason = 0;
	    sb->sb_max_data = 0;
	    sb->sb_max_attn = 0;
	    sb->sb_data_b_type = SB_DATA_B_UNK;
	    sb->sb_name[0] = 0;
	    if (pass != 0)
		sb_str_move_sl(&sb->sb_password[0],pass);
	    else
	    	sb->sb_password[0] = 0;
	    if (port != 0)
		sb_str_move_sl(&sb->sb_port[0],port);
	    else
	    	sb->sb_port[0] = 0;

	    sb->sb_sc = sc;

/*-----------------------------------------------------------------------
    Initialize the HI structure and send a solicit message.
 -----------------------------------------------------------------------*/
	    hi->hi_sb = sb;
	    hi->hi_sid = sid;
#if CME_SLAT
	    hi->hi_cid = port_id;
#endif
#if LE_OBJECTS
	    hi->hi_ob = ob;
#endif
	    hi->hi_command = CMD_ACCESS;
	    hi->hi_modifier = 0;
	    hi->hi_resp_status = HI_SBINIT;

	    hi_str_move_sl(&hi->hi_service[0],name);
	    if (node != 0)
		hi_str_move_sl(&hi->hi_node[0],node);
	    if (port != 0)
		hi_str_move_sl(&hi->hi_port[0],port);

	    hi->hi_tics = 1;
	    hi->hi_retries = hs->hs_hi_cmd_retries;
	    hi->hi_function = LAT_SOLICIT;
	    hi->hi_dest_addr[0] = hs->hs_slave_mc[0];
	    hi->hi_dest_addr[1] = hs->hs_slave_mc[1];
	    hi->hi_dest_addr[2] = hs->hs_slave_mc[2];

	    hi->hi_sc = sc;
	}
    }
#else
	lat_error = LS_ERROR | NO_NAME;
#endif

    /*
     * Found the service; ensure that the proper service class is
     * offered.
     */

    else if (!(ls->ls_nd->nd_classes[(sc >> 3) + 1] & (1 << (sc & 7))))
	  lat_error = LS_ERROR | BAD_CLASS;
/*-----------------------------------------------------------------------
    Everything has been checked out. Create the session (and circuit
    block if needed). Timebased scheduling will generate messages.
 -----------------------------------------------------------------------*/
    else
    {
/*-----------------------------------------------------------------------
    Scan the existing circuit blocks for a match on this service node.
 -----------------------------------------------------------------------*/
	nd = ls->ls_nd;
	for (cb = 0, i = 1; i < hs->hs_cb_index; i++)
	{
	    if (hs->hs_cb_table[i] != 0 && hs->hs_cb_table[i]->cb_type == CB_SERVER)
	    {
		if (hs->hs_cb_table[i]->cb_nd == nd)
		{
		    cb = hs->hs_cb_table[i];
		    break;
		}
	    }
	}

/*-----------------------------------------------------------------------
    If no circuit block found, allocate and initialize a new circuit to
    generate a circuit START at the next tic interval.
 -----------------------------------------------------------------------*/
	if (cb == 0)
	{
	    if ((cb = cb_create(ls->ls_nd)) != 0)
	    {
		cb->cb_type  = CB_SERVER;
		cb->cb_state = CB_START;
		cb->cb_tics  = 1;
	    }
	}

/*-----------------------------------------------------------------------
    If we have a circuit block, create the session block and initialize
    to generate a START SLOT in the next circuit RUN message.
 -----------------------------------------------------------------------*/
	if (cb != 0)
	{
	    if ((sb = sb_create(cb)) != 0)
	    {
		if (node != 0 && *node != 0)
		    sb->sb_flags |= SB_NODE;

		sb->sb_sid   = sid;
#if CME_SLAT
		sb->sb_cid   = port_id;
#endif		
		sb_str_move_sl(&sb->sb_name[0],name);

		if (pass != 0)
		    sb_str_move_sl(&sb->sb_password[0],pass);
		if (port != 0)
		    sb_str_move_sl(&sb->sb_port[0],port);

		if (sc)
		  sb->sb_sc = sc;

		sb_signal(sb,SB_START,credits);

#if LE_OBJECTS
		sb->sb_ob     = ob;
		ob->ob_flags |= OB_LOCAL;
		ob->ob_lcl_use++;
#endif
	    }
	    else
		lat_error = SB_ERROR | NO_MEM;
	}
	else
	    lat_error = CB_ERROR | NO_MEM;
    }
    return(sb);
}
