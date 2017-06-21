/* $Id: hiscmd.c,v 3.2 1995/11/17 17:33:04 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hiscmd.c,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: hiscmd.c,v $
 * Revision 3.2  1995/11/17  17:33:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:23:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hi_s_cmd_message	(i) process command message

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called to process a command message.

    CALL BY:	void hi_s_cmd_message(frame,src)
		if_byte	frame;
    		word	src[3];

    ON EXIT:	An HI request structure is created and the environment
    		is notified.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "lathi.h"
#include "latnd.h"
#include "latob.h"
#include "latqr.h"
#include "latas.h"
#include "latmsg.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	byte	as_temp[];		/* Temporary AS name		*/
extern	byte	nd_temp[];		/* Temporary ND name		*/
extern  byte	ob_temp[];		/* Temporary OB name		*/
extern	byte	id_temp[];		/* Temporary password string	*/
extern	hi_node	hi_dummy;		/* Dummy HI structure		*/

#if LE_OBJECTS
extern	int	ob_highest;		/* Largest number objects used	*/
extern	ob_ptr	ob_map[];		/* Object mapping table		*/
#endif

void hi_s_cmd_message(frame,src,idb)
    if_byte	frame;			/* Pointer to input  message	*/
    word	src[3];			/* Source address of message	*/
    idbtype	*idb;			/* Input interface		*/
{
    cmd_iptr	cmd;			/* Input command message	*/
    if_byte	next;			/* Input byte pointer		*/
    if_byte	node;			/* Input pointer to subj. node	*/
    if_byte	group;			/* Input pointer to subj. group	*/
    of_ptr	of;			/* Output frame structure	*/
    hi_ptr	hi;			/* Pointer to HI structure	*/
    nd_ptr	nd;			/* Pointer to ND structure	*/
    as_ptr	as;			/* Pointer to AS structure	*/
#if LE_OBJECTS
    ob_ptr	ob;			/* Pointer to OB structure	*/
    int		i;			/* For index			*/
#endif        
    byte	sts;			/* Internal status		*/
    byte	gp_match;		/* Group matched flag		*/
    byte	stat;			/* Internal status		*/
#if CME_SLAT
    cid_type	cid;			/* CME port identifier		*/
#endif        
    byte	sc = SVC_CLASS_TERM;	/* Service class */

/*-----------------------------------------------------------------------
    Get the pointer to command message.
 -----------------------------------------------------------------------*/
    cmd  = (cmd_iptr)frame;
    next = frame + sizeof(cmd_msg);

/*----------------------------------------------------------------------
    Check the group codes, ala a DECServer 200
 ----------------------------------------------------------------------*/
    node = next;
    node += *node + 1;
    group = node;
    node += *node + 1;
    gp_match =FAILURE;
    if (*node == 0) {
	;
    } else {
    	str_input(&nd_temp[0],node,ND_STR,1);
	nd = nd_show(&nd_temp[0],idb);
	if (!nd) {
	    nd = nd_create(&nd_temp[0],src,idb);
	}
	if (nd->nd_acl[0] == 0) {
	    gp_match = idl_cmp_group(0,group);
	} else {
	    gp_match = idl_chk_group(0,nd);
	}
    }

    sts = 0;

/*-----------------------------------------------------------------------
    If object node is not us or there is no match on groups or the
    source node name is illegal, ignore the command message.
 -----------------------------------------------------------------------*/
    str_input(&nd_temp[0],next,ND_STR,1);

    if (!hs_str_equal_sl(&hs->hs_node[0],&nd_temp[0])) {
        ;

    } else if (next += *next+1, (gp_match == FAILURE)) {
        ;

    } else if (next += *next+1, !str_input(&nd_temp[0],next,ND_STR,1)) {
        ;
    }
/*-----------------------------------------------------------------------
    If access command, create new queue entry (or just notify environment
    if objects not supported).
 -----------------------------------------------------------------------*/
    else if (cmd->cmd_operation == CMD_ACCESS || cmd->cmd_operation == CMD_ACCESS_Q)
    {
	next += *next + 1;
	next += *next + 1;

	next += *next + 1;
	str_input(&as_temp[0],next,SV_STR,1);

	next += *next + 1;
	str_input(&ob_temp[0],next,OB_STR,1);
	
	id_temp[0] = 0;
	while (next += *next + 1, *next != 0)
	{
	    switch (*next) {
	      case CMD_P_SC:		/* Service class */
		sc = next[1];		/* Pick it up */
		break;

	      case CMD_P_SERVER:	/* Server */
		str_input(&ob_temp[0],next+1,OB_STR,1);
		break;

	      case CMD_PASSWD:
	    	str_input(&id_temp[0],next+1,SV_STR,1);
		break;

	      default:
		break;		/* Unrecognized parameter, skip */
	    }
	}

/*-----------------------------------------------------------------------
    Scan the current list to see if this is duplicate request. If same
    service/port as previous, generate back status message. Otherwise,
    delete this entry and create a new one.
 -----------------------------------------------------------------------*/
	if ((hi = hi_find(swap(cmd->cmd_req_no),0,HI_OBJECT,&nd_temp[0])) != 0)
	{
	    if (hi_str_equal_sl(&hi->hi_service[0],&as_temp[0]) &&
                hi_str_equal_sl(&hi->hi_port[0],&ob_temp[0]) &&
		(sc == hi->hi_sc))
	    {
		hi->hi_tics    = 1;
		hi->hi_retries = 1;
		hi->hi_status  = STS_QUEUED;
	    }
	    else
	    {
		hi_delete(hi,0);
		hi = 0;
	    }
	}

/*-----------------------------------------------------------------------
    Otherwise, this is a new entry. Allocate the HI structure and link
    to tail of list. If this fails, we just give up.
 -----------------------------------------------------------------------*/
        if (hi == 0 && (hi = hi_create(HI_OBJECT)) != 0)
        {

/*-----------------------------------------------------------------------
    Initialize the new HI entry from the command message.
 -----------------------------------------------------------------------*/
	    hi->hi_function = LAT_STATUS;
	    hi->hi_tics     = 1;
	    hi->hi_retries  = 1;
	    hi->hi_dll_max  = swap(cmd->cmd_dll_max) - 18;

	    hi->hi_command  = cmd->cmd_operation;
	    hi->hi_modifier = cmd->cmd_modifier;
	    hi->hi_req_no   = swap(cmd->cmd_req_no);
	    hi->hi_status   = 0;

	    hi->hi_dest_addr[0] = src[0];
	    hi->hi_dest_addr[1] = src[1];
	    hi->hi_dest_addr[2] = src[2];

	    hi->hi_idb = idb;
	    hi->hi_sc = sc;

	    hi_str_move_sl(&hi->hi_node[0],&nd_temp[0]);
	    hi_str_move_sl(&hi->hi_service[0],&as_temp[0]);
	    hi_str_move_sl(&hi->hi_port[0],&ob_temp[0]);
	    hi_str_move_sl(&hi->hi_pass[0],&id_temp[0]); 

/*-----------------------------------------------------------------------
    If HIC is to a password protected service or port associated with 
    such a service, fail if correct password is not provided.
 -----------------------------------------------------------------------*/
	    stat = SUCCESS;
	    
	    if ((as = as_show(as_temp)) != 0 && as->as_password[0] != 0 &&
	    	    as_str_diff_sl(&as->as_password[0],&id_temp[0]) != 0)
	    	stat = FAILURE;
#if LE_OBJECTS
	    else if ((ob = ob_show(ob_temp)) != 0)
	    {
	    	for (i = 0; i < ob_highest; i++)
		    if (ob == ob_map[i])
		    	break;
		for (as = hs->hs_as_head[0]; as != 0; as = as->as_link[0])
		    if ((as->as_ob_map[i/8] & (1 << (i%8))) &&
		    	    as->as_password[0] != 0 &&
			    as_str_diff_sl(&as->as_password[0],&id_temp[0]) != 0)
		    {
		    	stat = FAILURE;
			break;
		    }
	    }
#elif CME_SLAT
	    else if ((cid = cx_show(ob_temp)) != CID_NULL)
	    {
	    	for (as = hs->hs_as_head[0]; as != 0; as = as->as_link[0])
		    if ((as->as_port_map[cid/8] & (1 << (cid%8))) &&
		    	    as->as_password[0] != 0 &&
			    as_str_diff_sl(&as->as_password[0],&id_temp[0]) != 0)
		    {
		    	stat = FAILURE;
			break;
		    }
	    }
#endif
	    if (stat == FAILURE)
	    	hi_reject(hi,SLOT_STOP_PASSWORD);
		
	    else if (hs->hs_status & SVC_NODE_OFF)
	        hi_reject(hi,SLOT_STOP_SYSTEM);
	    else
	    {
	
#if (LE_OBJECTS || CME_SLAT)
/*-----------------------------------------------------------------------
    Create a queued entry.
 -----------------------------------------------------------------------*/
                if (qr_create(hi) == 0)
	            hi_reject(hi,SLOT_STOP_CANNOT_START);
#else
	        hx_session(hi,HX_ARRIVED);
#endif
	    }
	}

/*-----------------------------------------------------------------------
    If no entry created, flag to send no resource error.
 -----------------------------------------------------------------------*/
	if (hi == 0)
	    sts = SLOT_STOP_NO_RESOURCE;
    }

/*-----------------------------------------------------------------------
    If cancel, delete the matching queue entry and set send back the
    appropriate status.
 -----------------------------------------------------------------------*/
    else if (cmd->cmd_operation == CMD_CANCEL)
    {
	if ((hi = hi_find(swap(cmd->cmd_req_no),swap(cmd->cmd_que_no),HI_OBJECT,&nd_temp[0])) != 0)
	{
#if (LE_OBJECTS || CME_SLAT)
	    if (hi->hi_qr != 0)
	        qr_delete(hi->hi_qr);
#else
	    hx_session(hi,HX_CANCEL);
#endif

	    hi_reject(hi,SLOT_STOP_ENTRY_DELETED);
	}
	else
	    sts = SLOT_STOP_ENTRY_DELETED;
    }

/*-----------------------------------------------------------------------
    If status, mark HI to send back status message of the entry. If
    we cannot find entry, we ignore the request.
 -----------------------------------------------------------------------*/
    else if (cmd->cmd_operation == CMD_STS_ENTRY)
    {
	if ((hi = hi_find(swap(cmd->cmd_req_no),swap(cmd->cmd_que_no),HI_OBJECT,&nd_temp[0])) != 0)
	{
	    hi->hi_tics    = 1;
	    hi->hi_retries = 1;
	}
	else
	    sts = SLOT_STOP_ENTRY_DELETED;
    }

/*-----------------------------------------------------------------------
    If all, mark all HI entries belonging to remote node to send back
    status.
 -----------------------------------------------------------------------*/
    else if (cmd->cmd_operation == CMD_STS_ALL)
    {
	next += *next + 1;
	next += *next + 1;

	next += *next + 1;
	str_input(&as_temp[0],next,SV_STR,1);

	next += *next + 1;
	str_input(&ob_temp[0],next,OB_STR,1);

	for (hi = hs->hs_hi_head[0]; hi != 0; hi = hi->hi_link[0]) {
	    if (!hi_str_equal_sl(&hi->hi_node[0],&nd_temp[0])) {
		;

	    } else if ((as_temp[0] != 0) &&
		       !hi_str_equal_sl(&hi->hi_service[0],&as_temp[0])) {
		;

	    } else if ((ob_temp[0] != 0) &&
		       !hi_str_equal_sl(&hi->hi_port[0],&ob_temp[0])) {
		;

	    } else {
		hi->hi_flags |= HI_MULTI;
		hi->hi_tics    = 1;
		hi->hi_retries = 1;
	    }
	}
    }

/*-----------------------------------------------------------------------
    Anything else is a bad command.
 -----------------------------------------------------------------------*/
    else
	sts = SLOT_STOP_BAD_COMMAND;

/*-----------------------------------------------------------------------
    Return error status using a dummy HI structure.
 -----------------------------------------------------------------------*/
    if (sts != 0 && (of = of_alloc()) != 0)
    {
	if ((nd = nd_show(&nd_temp[0],idb)) != 0)
	{
	    count_1(co_solicit_reject);
	}

	hi = (hi_ptr)&hi_dummy;

	hi->hi_function = LAT_STATUS;
	hi->hi_status   = STS_ERROR;
	hi->hi_error    = sts;
	hi->hi_req_no   = swap(cmd->cmd_req_no);
	hi_str_move_sl(&hi->hi_node[0],&nd_temp[0]);	

	hi_o_sts_message(of,hi);

	of->of_size |= OF_DELETE;
	of->of_dst[0] = src[0];
	of->of_dst[1] = src[1];
	of->of_dst[2] = src[2];
	of_message(of);
    }
}
