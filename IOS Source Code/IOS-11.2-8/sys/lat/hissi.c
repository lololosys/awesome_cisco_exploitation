/* $Id: hissi.c,v 3.2 1995/11/17 17:33:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hissi.c,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: hissi.c,v $
 * Revision 3.2  1995/11/17  17:33:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:24:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hi_s_si_message	(i) process solicit message

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called to process a solicit message.

    CALL BY:	void hi_s_si_message(frame,src,dst)
		if_byte	frame;
    		word	src[3];
		word	dst[3];

    ON EXIT:	If we match on nodename, groups, and service; return a
    		response message.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latas.h"
#include "latnd.h"
#include "latls.h"
#include "lathi.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

#define AGENT	1

#define MCAST	0x01
#define NODE	0x02
#define SERVICE 0x04
#define NO_ACC	0x08

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	byte	nd_temp[];		/* ND temporary storage		*/
extern	byte	as_temp[];		/* AS temporary storage		*/
extern	int	tu_msec_tic;		/* Milliseconds per tick	*/

#if LE_SERVER_AGENT
	count	a = 1234567;		/* 'Random' number seed		*/
	count	b = 31415821;		/* Random number multiplier	*/
#endif	

void hi_s_si_message(frame,src,dst,idb)

    if_byte	frame;			/* Pointer to input  message	*/
    word	src[3];			/* Source address of message	*/
    word	dst[3];			/* Desination address		*/
    idbtype	*idb;			/* Interface to use             */
{
    hi_ptr	hi;			/* Pointer to HIC entry		*/
    si_iptr	si;			/* Input solicit message	*/
    if_byte	next;			/* Input byte pointer		*/
    if_byte	dst_node;		/* Dest. node name pointer	*/
    if_byte	src_group;		/* Source group name pointer	*/
    if_byte	src_node;		/* Source node name pointer	*/
    if_byte	dst_svc;		/* Dest. service name pointer	*/

#if LE_SERVER_AGENT    
    nd_ptr	nd;			/* Node pointer			*/
    ls_ptr	ls;			/* Learned service pointer	*/
#endif    
    
    byte	respond;		/* Responder source flag	*/
    byte	status;			/* Status flag			*/

/*-----------------------------------------------------------------------
    Get pointers to the desired node name, source group codes, source
    node name and desired service name; set status to failure.
 -----------------------------------------------------------------------*/
    si   = (si_iptr)frame;
    next = frame + sizeof(si_msg);
    dst_node = next;
    next += (*next) + 1;
    src_group = next;
    next += (*next) + 1;
    src_node = next;
    next += (*next) + 1;
    dst_svc = next;
    
    status = respond = 0;
    
#if LE_SERVER_AGENT    
    nd_temp[0] = 0;
    nd = 0;
#endif    
        
/*-----------------------------------------------------------------------
    See if request was multicast or physically addressed.
 -----------------------------------------------------------------------*/
    if (dst[0] == hs->hs_multicast[0])
    	status = MCAST;
	     
/*-----------------------------------------------------------------------
    If a node specified, see if we are the target node; else, if we are
    an agent see if we can respond.  Also, check group codes for access
    control.
 -----------------------------------------------------------------------*/
    if (*dst_node != 0)
    {
    	str_input(&nd_temp[0],dst_node,ND_STR,1);
	
 	if (hs_str_equal_sl(&hs->hs_node[0],&nd_temp[0]))
	{
	    status |= NODE;
	    if (!acl_cmp_group(src_group))
	    	status |= NO_ACC;
	}

#if LE_SERVER_AGENT
	else if ((hs->hs_status & SVC_AGENT) && 
			((nd = nd_show(&nd_temp[0],idb)) != 0))
	{
	    status |= NODE;
	    respond = AGENT;
	    if (!acl_chk_group(nd,src_group))
	    	status |= NO_ACC;
	}
#endif
	    
    }
   
/*-----------------------------------------------------------------------
    If a service name specified see if we offer it; else if we are an
    agent, see if we can respond.  Again, check group codes for access
    control.
 -----------------------------------------------------------------------*/
   if (*dst_svc != 0 && !(status & NO_ACC))
   {
	str_input(&as_temp[0],dst_svc,SV_STR,1);
	
	if (as_show(&as_temp[0]))
	{
	    status |= SERVICE;
	    if (!acl_cmp_group(src_group))
	    	status |= NO_ACC;
	}

#if LE_SERVER_AGENT	
	else if ((hs->hs_status & SVC_AGENT) && 
			((ls = ls_show(&as_temp[0],&nd_temp[0],0,0)) != 0))
	{
	    if (*dst_node == 0)
	    	nd = ls->ls_nd;
		
	    status |= SERVICE;
	    respond = AGENT;
	    if (!acl_chk_group(ls->ls_nd,src_group))
	    	status |= NO_ACC;
	}
#endif	

    }
    
/*-----------------------------------------------------------------------
    Determine if we should respond to message and, if so, how ---
    
    If neither node name nor service name specified, ignore message.
 -----------------------------------------------------------------------*/
    if ((*dst_node == 0) && (*dst_svc == 0)) {
    	;
    }
/*-----------------------------------------------------------------------
    If node name specified and node not found, ignore message.
 -----------------------------------------------------------------------*/
    else if ((*dst_node != 0) && !(status & NODE)) {
    	;
    }
/*-----------------------------------------------------------------------
    If access denied and (message multicast or from a V5.1 node),
    ignore it.
 -----------------------------------------------------------------------*/
    else if (status & NO_ACC && (status & MCAST || 
				 ((si->si_prot_ver == 5) &&
				  (si->si_prot_eco == 1)))) {
    	;
    }
/*-----------------------------------------------------------------------
    If message multicast and requested service not found and (no node
    name specified or responder is agent), ignore it.
 -----------------------------------------------------------------------*/
    else if ((status & MCAST) &&
	     (*dst_svc != 0) &&
	     !(status & SERVICE) &&
	     ((*dst_node == 0) || (respond == AGENT))) {
    	;
    }
/*-----------------------------------------------------------------------
    Generate an HI entry and fill in for response.
 -----------------------------------------------------------------------*/
    else if ((hi = hi_create(HI_OBJECT)) != 0) {
    	hi->hi_tics = 1;
	
#if LE_SERVER_AGENT
	if (respond == AGENT) {
	    a = (a*b+1) & 0xffff;
	    hi->hi_tics += ((1000 / tu_msec_tic) * swap(si->si_timer) * a) / 0x10000;
	    
	    nd_str_move_ls(&nd_temp[0],&nd->nd_node[0]);
	    hi_str_move_sl(&hi->hi_save_node[0],&nd_temp[0]);
	}
#endif	
				
	hi->hi_retries = 1;
	hi->hi_dll_max = swap(si->si_dll_max) - 18;
	hi->hi_function = LAT_INFO;
	hi->hi_req_no = swap(si->si_req_no);
	
	hi->hi_dest_addr[0] = src[0];
	hi->hi_dest_addr[1] = src[1];
	hi->hi_dest_addr[2] = src[2];
	hi->hi_idb = idb;

	str_input(&nd_temp[0],src_node,ND_STR,1);
	hi_str_move_sl(&hi->hi_node[0],&nd_temp[0]);
	hi_str_move_sl(&hi->hi_service[0],&as_temp[0]);
	
	if (status & NO_ACC)
	    hi->hi_status = RI_NO_ACCESS;
	else if (*dst_svc != 0 && !(status & SERVICE))
	    hi->hi_status = RI_NO_SERVICE;
	else
	    hi->hi_status = 0;
	if (respond == AGENT)
	    hi->hi_modifier = HI_AGENT;
    }
}
