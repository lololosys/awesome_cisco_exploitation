/* $Id: hiora.c,v 3.2 1995/11/17 17:32:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hiora.c,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: hiora.c,v $
 * Revision 3.2  1995/11/17  17:32:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:23:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    hi_o_ra_message	(i) output agent response message 

    Copyright 1991 Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called to output a response message from an agent
    node.

    CALL BY:	void hi_o_ra_message(of,hi)
		of_ptr	of;
		hi_ptr	hi;

    ON EXIT:	Appropriate message sent to remote node.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latls.h"
#include "latnd.h"
#include "lathi.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	byte	as_temp[];		/* Temporary AS name		*/
extern	byte	nd_temp[];		/* Temporary ND name		*/

void hi_o_ra_message(of,hi)

    of_ptr	of;			/* Output frame pointer		*/
    hi_ptr	hi;			/* HIC request pointer		*/
{
    ri_optr	ri;			/* Output response message	*/
    of_byte	next;			/* Output byte pointer		*/
    of_byte	svc;			/* Count of service entries	*/
    nd_ptr	nd;			/* Pointer to a node block	*/
    ls_ptr	ls;			/* Pointer to a service block	*/
    int		max;			/* Working length of            */
    int         i;                      /* Index                        */
    int         bits,k,svc_class_len;   /* Used to unmask Service Classes */
    of_byte     svc_class_len_ptr;      /* Service Class length pointer */

/*-----------------------------------------------------------------------
    Fill in the fixed fields of a response information message.
 -----------------------------------------------------------------------*/
    hi_str_move_ls(&nd_temp[0],&hi->hi_save_node[0]);
    nd = nd_show(&nd_temp[0], hi->hi_idb);
 
    ri = (ri_optr)of->of_data;
    next = of->of_data + sizeof(ri_msg);

    ri->ri_type         = hi->hi_function;
    ri->ri_format       = RI_ETHER;
    ri->ri_prot_hi      = hs->hs_prot_hi;
    ri->ri_prot_lo      = hs->hs_prot_lo;
    ri->ri_prot_ver     = hs->hs_prot_ver;
    ri->ri_prot_eco     = hs->hs_prot_eco;
    ri->ri_dll_max      = swap(nd->nd_dll_max);
    ri->ri_req_no 	= swap(hi->hi_req_no);
    ri->ri_req_status   = swap((word)hi->hi_status);
    ri->ri_node_status  = swap(RI_START);
    ri->ri_node_addr[0] = nd->nd_host_addr[0];
    ri->ri_node_addr[1] = nd->nd_host_addr[1];
    ri->ri_node_addr[2] = nd->nd_host_addr[2];
    ri->ri_mc_timer     = swap((word)nd->nd_mc_timer);

/*-----------------------------------------------------------------------
    Complete the variable fields in a response message.
 -----------------------------------------------------------------------*/
    next = hi_str_move_os(next,"");

    /* 
    ** Fill in Source Node Group len and Mask (if any)
    */
    if (nd->nd_acl[0] < GROUPS ){
      *next++ = nd->nd_acl[0];
      for ( i = 1; i <= nd->nd_acl[0]; i++)
       *next++ = nd->nd_acl[i];
    }
    else 
      *next++ = 0;

    next = nd_str_move_os(next,&nd->nd_node[0]);
    next = nd_str_move_os(next,&nd->nd_ident[0]);

/*-----------------------------------------------------------------------
    Set count of service entries.
 -----------------------------------------------------------------------*/
    svc  = next++;
    *svc = 0;

    max = nd->nd_dll_max;
    
/*-----------------------------------------------------------------------
    If response status is an error condition, don't bother with 
    services.
 -----------------------------------------------------------------------*/
    if (hi->hi_status & (RI_NO_ACCESS | RI_NO_SERVICE)) {
    	;
    }
/*-----------------------------------------------------------------------
    If solicit message named a specific service, look up the name and
    fill the entry. If service not found, set service unavailabe.
 -----------------------------------------------------------------------*/
    else if (hi->hi_service[0] != 0)
    {
#if LE_SERVER_LS
	hi_str_move_ls(&as_temp[0],&hi->hi_service[0]);
	if ((ls = ls_show(&as_temp[0],&nd_temp[0],0,0)) != 0)
	{
	    if (max - (next - of->of_data) > ls->ls_service[0] + ls->ls_ident[0] + 8)
	    {
	        *svc += 1;
	        *next++ = ls->ls_service[0] + ls->ls_ident[0] + 7 + 3;

                /* 
                ** Fill in Service Class
                ** nd->nd_classes in bit mask format with first byte
                ** as size of following mask (in byte chunks) 0 i.e.
                ** bit (bit 0 leftmost) 000000001 00011010 ( or 0x1 0x1a) 
                ** denotes service class 1, 3 & 4. Size is 1 since the 
                ** classes all fit in the first 8 bits (1 byte).
                ** We must extract the bit info into required 
                ** byte length and unmask the but field into byte
                ** fields. 
                */
                svc_class_len = 0;
                svc_class_len_ptr = next;
                next += 1;
                for (i=0; i < nd->nd_classes[0]; i++) {
	          bits = nd->nd_classes[i+1];
	          for (k=0; k < 8; k++) {
	          if (bits & 1) {
                    *next++ = i*8+k;
                    svc_class_len++;
	          } 
	          bits >>= 1;
	         } /* for k */
                } /* for i */
                *svc_class_len_ptr = svc_class_len;

	        *next++ = 1;		/* set status to 'enabled' */

	        *next++ = ls->ls_rating;
	
                /*
                ** Fill in Service Group Code length and group code range
                */
                if (nd->nd_acl[0] < GROUPS ){
                   *next++ = nd->nd_acl[0];
                   for ( i = 1; i <= nd->nd_acl[0]; i++)
                     *next++ = nd->nd_acl[i];
                }
                else 
                   *next++ = 0;

	        next = ls_str_move_os(next,&ls->ls_service[0]);
	        next = ls_str_move_os(next,&ls->ls_ident[0]);
	    }
	}
#endif
	if (*svc == 0)
	    ri->ri_req_status |= swap(RI_NO_SERVICE);
    }

#if LE_SERVER_LS
/*-----------------------------------------------------------------------
    Otherwise, copy as many service names as will fit into the message.
 -----------------------------------------------------------------------*/
    else
    {
        for (ls = nd->nd_ls[0]; ls != 0; ls = ls->ls_host[0])
	{
	    if (max - (next - of->of_data) > ls->ls_service[0] + ls->ls_ident[0] + 8)
	    {
	        *svc += 1;
	        *next++ = ls->ls_service[0] + ls->ls_ident[0] + 7;
	        *next++ = 0;

	        *next++ = 1;		/* set status to 'enabled' */

	        *next++ = ls->ls_rating;
	        *next++ = 0;
	        next = ls_str_move_os(next,&ls->ls_service[0]);
	        next = ls_str_move_os(next,&ls->ls_ident[0]);
	    }
	}
    }
#endif

/*-----------------------------------------------------------------------
    Set no parameters and compute length of message.
 -----------------------------------------------------------------------*/
    *next++ = 0;
    of->of_size = (int)(next - of->of_data);

/*-----------------------------------------------------------------------
    Delete entry after transmission.
 -----------------------------------------------------------------------*/
    hi->hi_flags |= HI_DELETE;
}
