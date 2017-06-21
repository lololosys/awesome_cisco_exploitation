/* $Id: exec_debug_ip_rsvp.h,v 1.1.4.3 1996/08/12 16:07:03 widmer Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/exec_debug_ip_rsvp.h,v $
 *------------------------------------------------------------------
 * Resource Reservation Protocol
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_ip_rsvp.h,v $
 * Revision 1.1.4.3  1996/08/12  16:07:03  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 1.1.4.2  1996/08/09  00:17:59  asastry
 * CSCdi65504:  Allow <debug ip rsvp detail> command to be RESV-only or
 * PATH-only
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/04/17  13:57:14  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * [un]debug ip rsvp [detail [resv | path] [acl]]
 *
 *	OBJ(paddr,1) - session name or address
 */
EOLNS(debug_ip_eol, rsvp_debug_commands);
NUMBER (debug_ip_acl_list, debug_ip_eol, debug_ip_eol, 
        OBJ(int,4), 1, 99, "Access list");
SET (debug_ip_acl_set, debug_ip_acl_list, OBJ(int,4), 0);
KEYWORD_ID(debug_ip_rsvp_path, debug_ip_acl_set, debug_ip_acl_set,
	   OBJ(int,3), TRUE, "path", "RSVP packet contents (PATH only)",
	   PRIV_CONF);
KEYWORD_ID(debug_ip_rsvp_resv, debug_ip_acl_set, debug_ip_rsvp_path,
	   OBJ(int,2), TRUE, "resv", "RSVP packet contents (RESV only)",
	   PRIV_CONF);
KEYWORD_ID(debug_ip_rsvp_detail, debug_ip_rsvp_resv, debug_ip_eol, 
	   OBJ(int,1), TRUE, "detail", "RSVP packet contents", PRIV_CONF);
KEYWORD_ID(debug_ip_rsvp_special, debug_ip_eol, debug_ip_rsvp_detail,
	   OBJ(int,5), TRUE, "special", "RSVP special debugging", PRIV_HIDDEN);
KEYWORD(debug_ip_rsvp, debug_ip_rsvp_special, ALTERNATE,
	"rsvp", "RSVP protocol activity", PRIV_OPR);

#undef ALTERNATE
#define ALTERNATE debug_ip_rsvp
