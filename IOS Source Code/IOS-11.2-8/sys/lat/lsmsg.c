/* $Id: lsmsg.c,v 3.2 1995/11/17 17:34:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/lsmsg.c,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lsmsg.c,v $
 * Revision 3.2  1995/11/17  17:34:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:26:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    ls_message		(i) process directory service message

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called when a directory service message is received.
    The services specificed by the sending node are updated. If this is
    the first message from the sending node, new node and learned service
    structures are created.

    CALL BY:	void ls_message(frame,src)
		if_byte	frame;
    		word	src[3];

    ON EXIT:	Learned services updated according to message.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latls.h"
#include "latnd.h"
#include "latob.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern hs_ptr	hs;			/* Local node definitions	*/
extern nd_ptr 	nd_hold[2];		/* nd pointer to node hold list */
extern ls_ptr 	ls_hold[2];		/* ls pointer to ls hold list	*/
extern byte	ls_temp[];		/* Temporary LS name		*/
extern byte	nd_temp[];		/* Temporary ND name		*/
extern byte	id_temp[];		/* Temporary ID name		*/
extern int	tu_secs_toc;		/* Seconds per toc		*/
extern int	le_ls_dups;		/* Duplicate service support	*/

void ls_message(frame,src,idb)
    if_byte	frame;			/* Pointer to input  message	*/
    word	src[3];			/* Source address of message	*/
    idbtype	*idb;			/* Source interface		*/
{
    svc_iptr	svc;			/* Pointer to input  message	*/
    nd_ptr	nd;			/* Pointer to node name		*/
    ls_ptr	ls;			/* Pointer to learned service	*/
    ls_ptr	ps;			/* Pointer to learned service	*/
    if_byte	next;			/* Byte pointer in message	*/
    if_byte	ptr;			/* Byte pointer in message	*/
    if_byte	group;			/* Pointer to group codes	*/
    if_byte	ident;			/* Pointer to ident string	*/
    if_byte	node;			/* Pointer to node name		*/
    byte	changes;		/* Service change mask		*/
    byte	num_services;		/* Number of services		*/
    byte	rating;			/* Rating of service		*/
    int		i;			/* Do index			*/
    byte	group_check;		/* Group code check flag	*/
    int		offset;			/* Offset in service class mask */
#if !LE_SERVER_AGENT
#if CME_SLAT
    cid_type	nul_port = CID_NULL;	/* Null port value		*/
#else
    ob_ptr	nul_port = 0;
#endif
#endif


/*-----------------------------------------------------------------------
    Get pointers to the variable fields in the directory service message
    (group codes, node name, node description).
 -----------------------------------------------------------------------*/
    count_h(ch_svc_recv);

    svc  = (svc_iptr)frame;
    next = frame + sizeof(svc_msg);

    group = next++;
    next += *group;

    node  = next++;
    next += *node;

    ident = next++;
    next += *ident;

/*-----------------------------------------------------------------------
    Get the number of services defined by this message.
 -----------------------------------------------------------------------*/
    num_services = *next++;

/*-----------------------------------------------------------------------
    Check the group codes against the group codes we use to see if
    any further processing is needed.
 -----------------------------------------------------------------------*/
#if LE_SERVER_AGENT
    group_check = SUCCESS;
#else
    group_check = idl_cmp_group(nul_port,group);
#endif

    if (group_check)
    {

/*-----------------------------------------------------------------------
    Find a matching node block or create a new one. If this fails,
    check service names in the message against our permanent list
    to see if special action is needed.
 -----------------------------------------------------------------------*/
	str_input(&nd_temp[0],node,ND_STR,1);
	if (((nd = nd_create(&nd_temp[0],src,idb)) == 0) && (nd_hold[0] != 0))
	{
	    ptr = next;
	    for (i = 0; i < num_services; i++)
	    {

/*-----------------------------------------------------------------------
    Get the rating, name, and description of the service.
 -----------------------------------------------------------------------*/
	        rating = *ptr++;

		str_input(&ls_temp[0],ptr,SV_STR,1);
		ptr += (*ptr) + 1;
		ptr += (*ptr) + 1;

/*-----------------------------------------------------------------------
    If one of the services in the message is in the ls hold list, we want
    to allocate a node by releasing a node from the node hold list and
    continuing.
 -----------------------------------------------------------------------*/
	        if ((ls = ls_look(&ls_temp[0])) != 0)
		{
		    nd_delete(nd_hold[0]);
		    nd = nd_create(&nd_temp[0],src,idb);
		    break;
		}
	    }
	}

/*-----------------------------------------------------------------------
    If we have a node structure we can continue.
 -----------------------------------------------------------------------*/
	if (nd != 0)
	{
	    changes = nd->nd_changes ^ svc->svc_changes;
	    nd->nd_changes = svc->svc_changes;

/*-----------------------------------------------------------------------
    If sequence numbers changed by more than one, set ND_INIT so
    we process entire message (not sure of state of change flag.
 -----------------------------------------------------------------------*/
	    if (nd->nd_seq != svc->svc_sequence)
	    {
	        if (LOBYTE(svc->svc_sequence - nd->nd_seq) != 1)
	            nd->nd_flags |= ND_INIT;
	    }

/*-----------------------------------------------------------------------
    If node address has changed, set new address and count event. If
    we have an active circuit to first node, ignore event and set the
    sequence numbers equal so we do not process message.
 -----------------------------------------------------------------------*/
	    if (nd->nd_host_addr[0] != src[0] ||
	        nd->nd_host_addr[1] != src[1] ||
		nd->nd_host_addr[2] != src[2])
	    {
		if (nd->nd_usage == -1)
		{
		    nd->nd_flags |= ND_INIT;
		    nd->nd_host_addr[0] = src[0];
		    nd->nd_host_addr[1] = src[1];
		    nd->nd_host_addr[2] = src[2];
		    nd->nd_idb          = idb;
	            count_1(co_multiple_node);
		}
		else
		    nd->nd_seq = svc->svc_sequence;
	    }

/*-----------------------------------------------------------------------
    If node has not been initialized, force sequence compare to fail and
    set everything has changed.
 -----------------------------------------------------------------------*/
	    if (nd->nd_flags & ND_INIT)
	    {
		nd->nd_flags   &= ~ND_INIT;
		nd->nd_seq      = svc->svc_sequence - 1;
		changes    = (byte)~0;

		nd->nd_prot_hi  = svc->svc_prot_hi;
		nd->nd_prot_lo	= svc->svc_prot_lo;
		nd->nd_prot_ver = svc->svc_prot_ver;
		nd->nd_prot_eco = svc->svc_prot_eco;
	    }

/*-----------------------------------------------------------------------
    Reset the node timer and check the sequence numbers. We are done if
    there is a match.
 -----------------------------------------------------------------------*/
#if LE_SERVER_AGENT
 	    nd->nd_mc_timer = svc->svc_mc_timer;
#endif	

	    nd->nd_timer = (svc->svc_mc_timer * LS_UPDATES) + (tu_secs_toc - 1);
	    if (nd->nd_seq != svc->svc_sequence)
	    {
    		count_h(ch_svc_used);
		nd->nd_seq     = svc->svc_sequence;
		nd->nd_dll_max = swap(svc->svc_dll_max) - 18;

/*-----------------------------------------------------------------------
    If the node description has changed, update the node identification.
 -----------------------------------------------------------------------*/
		if (changes & SVC_IDENT)
		{
		    str_input(&id_temp[0],ident,ID_STR,0);
		    nd_str_move_sl(&nd->nd_ident[0],&id_temp[0]);
		}

/*-----------------------------------------------------------------------
    If the group codes have changed, update the group identification.
 -----------------------------------------------------------------------*/
		if (changes & SVC_GROUPS)
		{
		    str_input(&id_temp[0],group,GROUPS,0);
		    nd_str_move_sl(&nd->nd_acl[0],&id_temp[0]);
		}

/*-----------------------------------------------------------------------
    Mark all the services currently provided by the node as invalid.
 -----------------------------------------------------------------------*/
	        for (ls = nd->nd_ls[0]; ls != 0; ls = ls->ls_host[0])
		    ls->ls_flags |= LS_AGED;

/*-----------------------------------------------------------------------
    Loop through the services in the message.
 -----------------------------------------------------------------------*/
	        while (num_services-- != 0)
	        {

/*-----------------------------------------------------------------------
    Get the rating, name, and description of the service.
 -----------------------------------------------------------------------*/
		    rating = *next++;

		    str_input(&ls_temp[0],next,SV_STR,1);
		    next += (*next) + 1;

		    str_input(&id_temp[0],next,ID_STR,0);
		    next += (*next) + 1;

/*-----------------------------------------------------------------------
    Scan all the services offered by this node for a match on the name.
 -----------------------------------------------------------------------*/
		    for (ls = nd->nd_ls[0]; ls != 0; ls = ls->ls_host[0])
		    {
			if (ls_str_equal_sl(&ls->ls_service[0],&ls_temp[0]))
			    break;
		    }

/*-----------------------------------------------------------------------
    If we found a matching service, update the rating and mark found.
 -----------------------------------------------------------------------*/
		    if (ls != 0)
		    {
			ls->ls_flags &= (~LS_AGED);
			ls->ls_rating = rating;
			if (changes & SVC_DESCRIPTOR)
			    ls_str_move_sl(&ls->ls_ident[0],&id_temp[0]);
		    }

/*-----------------------------------------------------------------------
    Check the new service name does not exceed maximum size.
 -----------------------------------------------------------------------*/
		    else if (ls_temp[0] == 0) {
			;
		    }
/*-----------------------------------------------------------------------
    Check to see if duplicate services are allowed.  If they are not,
    call ls_dups to handle the situation.
 -----------------------------------------------------------------------*/
		    else if (!le_ls_dups && ls_dups(&ls_temp[0],rating)) {
		        ;
		    }
/*-----------------------------------------------------------------------
    If we were able to allocate memory for an ls structure, then add
    new ls to the alphabetic list.
 -----------------------------------------------------------------------*/
		    else
		    {
		        if ((ls = ls_create(&ls_temp[0],nd)) != 0) {
			    ;
			}
/*-----------------------------------------------------------------------
    If no new learned service created, check to see if this service is in
    the ls hold list.  If so, remove it from the hold list and link to the
    remote node. Note, learned services in hold list are already in the
    alphabetical list.
 -----------------------------------------------------------------------*/
			else if ((ls = ls_look(&ls_temp[0])) != 0)
			{
			    if (ls->ls_host[0] != 0)
			        ls->ls_host[0]->ls_host[1] = ls->ls_host[1];
			    else
			        ls_hold[1] = ls->ls_host[1];

			    if (ls->ls_host[1] != 0)
			        ls->ls_host[1]->ls_host[0] = ls->ls_host[0];
			    else
			        ls_hold[0] = ls->ls_host[0];

			    ls->ls_host[0] = 0;
			    ls->ls_host[1] = nd->nd_ls[1];
			    nd->nd_ls[1]   = ls;
			    if (ls->ls_host[1] != 0)
			        ls->ls_host[1]->ls_host[0] = ls;
			    else
			        nd->nd_ls[0] = ls;
			}

/*-----------------------------------------------------------------------
    If we have an ls structure, complete initialization.
 -----------------------------------------------------------------------*/
			if (ls != 0)
			{
			    ls->ls_nd     = nd;
		 	    ls->ls_rating = rating;

			    ls_str_move_sl(&ls->ls_service[0],&ls_temp[0]);
		    	    ls_str_move_sl(&ls->ls_ident[0],&id_temp[0]);
		        }
		    }
		}

/*-----------------------------------------------------------------------
    Store the service classes supported. Note that the services are
    sent as a list of actual numbers; we convert this into a bitmask.
 -----------------------------------------------------------------------*/
		for (i = 0; i < SVC_CLASSES; i++)
		  nd->nd_classes[i] = 0;

		for (i = 1; i <= *next; i++) {
		    offset = (next[i] >> 3) + 1;
		    if (offset > nd->nd_classes[0])
		      nd->nd_classes[0] = offset;
		    nd->nd_classes[offset] |= (1 << (next[i] & 7));
		}

		next += (*next) + 1;
/*-----------------------------------------------------------------------
    Delete all services still mark invalid. These are no longer
    offered by this host.
 -----------------------------------------------------------------------*/
		ps = nd->nd_ls[0];
		while ((ls = ps) != 0)
		{
		    ps = ls->ls_host[0];

		    if (ls->ls_flags & LS_AGED)
		        ls_delete(ls);
		}
		
/*-----------------------------------------------------------------------
    If node offers no services, try to delete it.
 -----------------------------------------------------------------------*/
		if (nd->nd_ls[0] == 0)
		    nd_fail(nd,0);
	    }
	}
	else
	    count_h(ch_svc_discard);
    }
}
