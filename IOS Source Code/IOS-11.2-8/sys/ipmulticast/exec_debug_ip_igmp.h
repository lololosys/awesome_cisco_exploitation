/* $Id: exec_debug_ip_igmp.h,v 3.2.60.1 1996/04/03 23:19:04 dino Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/exec_debug_ip_igmp.h,v $
 *------------------------------------------------------------------
 * IGMP Debug Commands
 *
 * November 1993.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_debug_ip_igmp.h,v $
 * Revision 3.2.60.1  1996/04/03  23:19:04  dino
 * CSCdi51598:  Problem of interoperability with switches
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:29:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/09  23:14:15  asastry
 * CSCdi38504:  Limit DVMRP debug output using access lists for the
 * command 'debug ip dvmrp detail [in | out]'
 *
 * Revision 2.2  1995/07/18  07:32:42  asastry
 * CSCdi37082:  Enhance 'debug ip dvmrp detail' to allow '[in | out]'.
 *
 * Revision 2.1  1995/07/01  12:56:50  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:18:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * [un]debug ip cgmp
 */
EOLS	(debug_ip_igmp_cgmp_eol, ipmulticast_debug_commands, DEBUG_CGMP);
KEYWORD (debug_ip_igmp_cgmp, debug_ip_igmp_cgmp_eol, ALTERNATE,
	 "cgmp", "CGMP protocol activity", PRIV_OPR);

/******************************************************************
 * [un]debug ip dvmrp [detail [in | out] [access-list]] [pruning]
 *
 * OBJ(int,1) = TRUE if "detail" is specified.
 * OBJ(int,2) = TRUE if "in" is specified.
 * OBJ(int,3) = TRUE if "out" is specified.
 * OBJ(int,4) = <access-list> value.
 * OBJ(int,5) = TRUE if "pruning" is specified.
 */
EOLS	(debug_ip_dvmrp_eol, ipmulticast_debug_commands, DEBUG_DVMRP);
NUMBER  (debug_ip_dvmrp_debug_list, debug_ip_dvmrp_eol, debug_ip_dvmrp_eol, 
	 OBJ(int,4), 1, 99, "Access list");
SET     (debug_ip_dvmrp_set, debug_ip_dvmrp_debug_list, OBJ(int,4), 0);
KEYWORD_ID(debug_ip_dvmrp_out, debug_ip_dvmrp_set, debug_ip_dvmrp_set,
	   OBJ(int,3), TRUE, "out", "DVMRP packet contents (outgoing only)",
	   PRIV_CONF);
KEYWORD_ID(debug_ip_dvmrp_in, debug_ip_dvmrp_set, debug_ip_dvmrp_out,
	   OBJ(int,2), TRUE, "in", "DVMRP packet contents (incoming only)",
	   PRIV_CONF);
KEYWORD_ID(debug_ip_dvmrp_detail, debug_ip_dvmrp_in, debug_ip_dvmrp_eol,
	   OBJ(int,1), TRUE, "detail", "DVMRP packet contents", PRIV_CONF);
KEYWORD_ID(debug_ip_dvmrp_pruning, debug_ip_dvmrp_eol, debug_ip_dvmrp_detail,
	   OBJ(int,5), TRUE, "pruning", "DVMRP Pruning/Grafting events", 
	   PRIV_CONF);
KEYWORD (debug_ip_dvmrp, debug_ip_dvmrp_pruning, debug_ip_igmp_cgmp,
	 "dvmrp", "DVMRP protocol activity", PRIV_OPR);

/******************************************************************
 * [un]debug ip igmp
 *
 */
EOLS	(debug_ip_igmp_eol, ipmulticast_debug_commands, DEBUG_IGMP);
KEYWORD (debug_ip_igmp, debug_ip_igmp_eol, debug_ip_dvmrp,
	 "igmp", "IGMP protocol activity", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_ip_igmp

